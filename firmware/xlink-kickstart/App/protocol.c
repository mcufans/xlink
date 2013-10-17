/******************************************************************************
 * @file     XLINK.c
 * @brief    CMSIS-XLINK Commands
 * @version  V1.00
 * @date     31. May 2012
 *
 * @note
 * Copyright (C) 2012 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/
#include <string.h>
#include "config.h"
#include "protocol.h"

uint8_t   fast_clock;                           // Fast Clock Flag
uint32_t  clock_delay;                          // Clock Delay
uint16_t  retry_count;                          // Number of retries after WAIT response
uint16_t  match_retry;                          // Number of retries if read value does not match
uint32_t  match_mask;                           // Match Mask
uint8_t   turnaround;                           // Turnaround period
uint8_t   data_phase;                           // Always generate Data Phase
uint8_t   idle_cycles;                          // Idle cycles after transfer
volatile uint8_t abortTransfer;                 // Abort current transfer

const char XLINK_Vendor [] = XLINK_VENDOR;      // Vendor name   
const char XLINK_Product[] = XLINK_PRODUCT;     // Product name
const char XLINK_SerNum [] = XLINK_SER_NUM;     // Serial number
const char XLINK_FW_Ver [] = XLINK_FW_VER;      // Firmware version string

// Get XLINK Information
// id:      info identifier
// info:    pointer to info data
// return:  number of bytes in info data
uint8_t XLINK_Info(uint8_t id, uint8_t *info) 
{
    uint8_t length = 0;

    switch (id) {
    case XLINK_INFOCMD_VENDOR:
        memcpy(info, XLINK_Vendor, sizeof(XLINK_Vendor));
        length = sizeof(XLINK_Vendor);
        break;
    case XLINK_INFOCMD_PRODUCT:
        memcpy(info, XLINK_Product, sizeof(XLINK_Product));
        length = sizeof(XLINK_Product);
        break;
    case XLINK_INFOCMD_SER_NUM:
        memcpy(info, XLINK_SerNum, sizeof(XLINK_SerNum));
        length = sizeof(XLINK_SerNum);
        break;
    case XLINK_INFOCMD_FW_VER:
        memcpy(info, XLINK_FW_Ver, sizeof(XLINK_FW_Ver));
        length = sizeof(XLINK_FW_Ver);
    break;
    case XLINK_INFOCMD_CAPABILITIES:
        info[0] = ((1  != 0) ? (1 << 0) : 0) |
                ((1 != 0) ? (1 << 1) : 0);
        length = 1;
    break;
    case XLINK_INFOCMD_PACKET_COUNT:
        info[0] = (uint8_t)(XLINK_PACKET_SIZE >> 0);
        info[1] = (uint8_t)(XLINK_PACKET_SIZE >> 8);
        length = 2;
        break;
    case XLINK_INFOCMD_PACKET_SIZE:
        info[0] = XLINK_PACKET_COUNT;
        length = 1;
        break;
    }

    return (length);
}

// Process LED command and prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t XLINK_LED(uint8_t *request, uint8_t *response) 
{
    switch (*request) {
    case XLINK_DEBUGER_LED:
        LED_CONNECTED_OUT((*(request+1) & 1));
        break;
    case XLINK_RUNNING_LED:
        LED_RUNNING_OUT((*(request+1) & 1));
        break;
    default:
        *response = XLINK_ERROR_ERR;
        return (1);
    }

    *response = XLINK_ERROR_OK;
    return (1);
}

// Process Connect command and prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t XLINK_Connect(uint8_t *request, uint8_t *response) 
{
    uint32_t port;

    if (*request == XLINK_MODE_AUTODETECT) {
        port = XLINK_DEFAULT_PORT;
    } else {
        port = *request;
    }
  
    switch (port) {
    case XLINK_MODE_SWD:
        PORT_SWD_SETUP();
        break;
    default:
        *response = XLINK_MODE_DISABLED;
        return (1);
    }

    *response = port;
    return (1);
}

// Process Disconnect command and prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t XLINK_Disconnect(uint8_t *response) 
{
    PORT_OFF();
    *response = XLINK_ERROR_OK;
    return (1);
}

// Process SWJ Pins command and prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t XLINK_SWJ_Pins(uint8_t *request, uint8_t *response) 
{
    uint32_t value;
    uint32_t select;
    uint32_t wait;

    value  =  *(request+0);
    select =  *(request+1); 
    wait   = (*(request+2) <<  0) |
            (*(request+3) <<  8) |
            (*(request+4) << 16) |
            (*(request+5) << 24);
    if (select & (1 << XLINK_SETPINS_TCK)) {
        if (value & (1 << XLINK_SETPINS_TCK)) {
            PIN_SWCLK_TCK_SET();
        } else {
            PIN_SWCLK_TCK_CLR();
        }
    }
    if (select & (1 << XLINK_SETPINS_TMS)) {
        if (value & (1 << XLINK_SETPINS_TMS)) {
            PIN_SWDIO_TMS_SET();
        } else {
            PIN_SWDIO_TMS_CLR();
        }
    }
    if (wait) {
        if (wait > 3000000) wait = 3000000;
            TIMER_START(wait);
        do {
            if (select & (1 << XLINK_SETPINS_TCK)) {
                if ((value >> XLINK_SETPINS_TCK) ^ PIN_SWCLK_TCK_IN()) 
                    continue;
            }
            if (select & (1 << XLINK_SETPINS_TMS)) {
                if ((value >> XLINK_SETPINS_TMS) ^ PIN_SWDIO_TMS_IN()) 
                    continue;
            }
            break;
        } while (!TIMER_EXPIRED());
        TIMER_STOP();
    }

    value = (PIN_SWCLK_TCK_IN() << XLINK_SETPINS_TCK) |
            (PIN_SWDIO_TMS_IN() << XLINK_SETPINS_TMS);
    *response = (uint8_t)value;
    return (1);
}

// Process SWJ Clock command and prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t XLINK_SWJ_Clock(uint8_t *request, uint8_t *response) 
{
    uint32_t clock;

    clock = (*(request+0) <<  0) |
            (*(request+1) <<  8) |
            (*(request+2) << 16) |
            (*(request+3) << 24);
    if (clock == 0) {
        *response = XLINK_ERROR_ERR;
        return (1);
    }

    // Not impelete
    *response = XLINK_ERROR_OK;
    return (1);
}


// Process SWJ Sequence command and prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t XLINK_SWJ_Sequence(uint8_t *request, uint8_t *response) 
{
    uint32_t count;

    count = *request++;
    if (count == 0) count = 256;
    SWDOutTMSTCK(count, request);
    *response = XLINK_ERROR_OK;
    return (1);
}

// Process SWD Configure command and prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t XLINK_SWD_Configure(uint8_t *request, uint8_t *response) 
{
    uint8_t value;

    value = *request;
    turnaround  = (value & 0x03) + 1;
    data_phase  = (value & 0x04) ? 1 : 0;  
    *response = XLINK_ERROR_OK;
    return (1);
}

// Process SWD Abort command and prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t XLINK_SWD_Abort(uint8_t *request, uint8_t *response) 
{
    uint32_t data;

    data = (*(request+1) <<  0) |
            (*(request+2) <<  8) |
            (*(request+3) << 16) |
            (*(request+4) << 24);
    SWDTransfer(DP_ABORT, &data);
    *response = XLINK_ERROR_OK;    
    return (1);
}

// Process Transfer Configure command and prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t XLINK_TransferConfigure(uint8_t *request, uint8_t *response) 
{
    idle_cycles = *(request+0);
    retry_count = *(request+1) | (*(request+2) << 8);
    match_retry = *(request+3) | (*(request+4) << 8);  
    *response = XLINK_ERROR_OK;
    return (1);
}

// Process SWD Transfer Block command and prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t XLINK_SWDTransferBlock(uint8_t *request, uint8_t *response) 
{
    uint32_t  request_count;
    uint32_t  request_value;
    uint32_t  response_count = 0;
    uint32_t  response_value = 0;
    uint8_t  *response_head = response;
    uint32_t  retry;
    uint32_t  data;

    response      += 3;

    abortTransfer = 0;
    request++; 
    request_count = *request | (*(request+1) << 8);
    request += 2;
    if (request_count == 0) goto end;

    request_value = *request++;
    if (request_value & XLINK_TRANSFER_RnW) {
        if (request_value & XLINK_TRANSFER_APnDP) {
            retry = retry_count;
            do {
                response_value = SWDTransfer(request_value, NULL);
            } while ((response_value == XLINK_TRANSFER_WAIT) && retry-- && !abortTransfer);
            if (response_value != XLINK_TRANSFER_OK) goto end;
        }
        while (request_count--) {
            if ((request_count == 0) && (request_value & XLINK_TRANSFER_APnDP)) {
                request_value = DP_RDBUFF | XLINK_TRANSFER_RnW;
            }
            retry = retry_count;
            do {
                response_value = SWDTransfer(request_value, &data);
            } while ((response_value == XLINK_TRANSFER_WAIT) && retry-- && !abortTransfer);
            if (response_value != XLINK_TRANSFER_OK) goto end;

            *response++ = (uint8_t) data;
            *response++ = (uint8_t)(data >>  8);
            *response++ = (uint8_t)(data >> 16);
            *response++ = (uint8_t)(data >> 24);
            response_count++;
        }
    } else {
        while (request_count--) {
            data = (*(request+0) <<  0) |
                 (*(request+1) <<  8) |
                 (*(request+2) << 16) |
                 (*(request+3) << 24);
            request += 4;
            retry = retry_count;
            do {
                response_value = SWDTransfer(request_value, &data);
            } while ((response_value == XLINK_TRANSFER_WAIT) && retry-- && !abortTransfer);
            if (response_value != XLINK_TRANSFER_OK) goto end;
            response_count++;
        }
        retry = retry_count;
        do {
            response_value = SWDTransfer(DP_RDBUFF | XLINK_TRANSFER_RnW, NULL);
        } while ((response_value == XLINK_TRANSFER_WAIT) && retry-- && !abortTransfer);
    }

end:
    *(response_head+0) = (uint8_t)(response_count >> 0);
    *(response_head+1) = (uint8_t)(response_count >> 8);
    *(response_head+2) = (uint8_t) response_value;
    return (response - response_head);
}

// Process XLINK command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response
uint32_t ProtocolProcCommand(uint8_t *request, uint8_t *response) 
{
    uint32_t num;

    *response++ = *request;
    switch (*request++) {
    case XLINK_CMD_INFO:
        num = XLINK_Info(*request, response+1);
        *response = num;
        return (2 + num);
    case XLINK_CMD_LED:
        num = XLINK_LED(request, response);
        break;
    case XLINK_CMD_MODE:
        num = XLINK_Connect(request, response);
        break;
    case XLINK_CMD_DISCONNECT:
        num = XLINK_Disconnect(response);
        break;
    case XLINK_CMD_SETPINS:
        num = XLINK_SWJ_Pins(request, response);
        break;
    case XLINK_CMD_SET_FREQ:
        num = XLINK_SWJ_Clock(request, response);
        break;
    case XLINK_CMD_SEQUENCE:
        num = XLINK_SWJ_Sequence(request, response);
        break;
    case XLINK_CMD_SWD_CONFIG:
        num = XLINK_SWD_Configure(request, response);
        break;
    case XLINK_CMD_TRANSFER_CONFIG:
        num = XLINK_TransferConfigure(request, response);
        break;
    case XLINK_CMD_TRANSFER_BLOCK:
        num = XLINK_SWDTransferBlock (request, response);
        break;
    case XLINK_CMD_ABORT_TRANSFER:
        num = XLINK_SWD_Abort (request, response);
        break;
    case XLINK_CMD_RESET:      
        break;
    default:
        *(response-1) = XLINK_CMD_INVALID;
        return (1);
    }

    return (1 + num);
}

void ProtocolInit(void) 
{
    fast_clock  = 0;
    clock_delay = 3;
    idle_cycles = 0;
    retry_count = 100;
    match_retry = 0;
    match_mask  = 0x000000;
    turnaround  = 1;
    data_phase  = 0;
    XLINK_SETUP();  // Device specific setup
}
