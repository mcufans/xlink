/******************************************************************************
 * @file     SW_DP.c
 * @brief    CMSIS-XLINK SW DP I/O
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
#include <stm32l1xx.h>
#include "config.h"
#include "protocol.h"

// Output a clock cycle
void __forceinline SWD_CLOCK_CYCLE ()
{
    PIN_SWCLK_TCK_CLR();
    PIN_DELAY();
    PIN_SWCLK_TCK_SET();
    PIN_DELAY();
}

// Write SWIO
void __forceinline SWD_WRITE_BIT (uint32_t bit)
{
    PIN_SWDIO_OUT(bit);
    PIN_SWCLK_TCK_CLR();
    PIN_DELAY();
    PIN_SWCLK_TCK_SET();
   	PIN_DELAY();
}

// Read SWIO
uint32_t __forceinline SWD_READ_BIT () 
{
    uint32_t bit;

    PIN_SWCLK_TCK_CLR();
    PIN_DELAY();
    bit = PIN_SWDIO_IN();
    PIN_SWCLK_TCK_SET();
    PIN_DELAY();
    return bit;
}

// Output TMS/TCK sequences
void SWDOutTMSTCK (uint32_t count, uint8_t *data) 
{
    uint32_t val;
    uint32_t bits;

    val  = 0;
    bits = 0;
    while (count--) {
        if (bits == 0) {
            val = *data++;
            bits = 8;
        }
        if (val & 1) {
            PIN_SWDIO_TMS_SET();
        } else {
            PIN_SWDIO_TMS_CLR();
        }
        SWD_CLOCK_CYCLE();
        val >>= 1;
        bits--;
    }
}

// SWD transfer
uint8_t SWDTransfer(uint32_t request, uint32_t *data) 
{
    uint32_t ack;                                                                 
    uint32_t bit;                                                                 
    uint32_t val;                                                                 
    uint32_t parity;                                                              
                                                                                
    uint32_t n;                                                                   
    
    // Send request
    parity = 0;                                                                   
    SWD_WRITE_BIT(1);                      // Start Bit                         
    bit = request >> 0;                                                           
    SWD_WRITE_BIT(bit);                    // APnDP Bit                        
    parity += bit;                                                                
    bit = request >> 1;                                                           
    SWD_WRITE_BIT(bit);                    // RnW Bit                           
    parity += bit;                                                                
    bit = request >> 2;                                                           
    SWD_WRITE_BIT(bit);                    // A2 Bit                           
    parity += bit;                                                                
    bit = request >> 3;                                                           
    SWD_WRITE_BIT(bit);                    // A3 Bit                          
    parity += bit;                                                                
    SWD_WRITE_BIT(parity);                 // Parity Bit                      
    SWD_WRITE_BIT(0);                      // Stop Bit                         
    SWD_WRITE_BIT(1);                      // Park Bit                         
                                                                                
    // Turnaround                                                             
    PIN_SWDIO_OUT_DISABLE();                                                      
        for (n = turnaround; n; n--) {                              
        SWD_CLOCK_CYCLE();                                                           
    }                                                                             
                                                                                
    // Acknowledge response                                                    
    ack  = SWD_READ_BIT() << 0;                                                              
    ack |= SWD_READ_BIT() << 1;                                                              
    ack |= SWD_READ_BIT() << 2;                                                              
                                                                                
    if (ack == XLINK_TRANSFER_OK) {                 
        if (request & XLINK_TRANSFER_RnW) {                                           
            val = 0;                                                                  
            parity = 0;                                                               
            for (n = 32; n; n--) {                // DATA[31:0]   
								bit = SWD_READ_BIT();
                parity += bit;                                                          
                val >>= 1;                                                              
                val  |= bit << 31;                                                      
            }                                                                         
            if ((parity ^ SWD_READ_BIT()) & 1) {  // Parity bit                                                
                ack = XLINK_TRANSFER_ERROR;                                               
            }                                                                         
            if (data) *data = val;                                                    
                for (n = turnaround; n; n--) {                          
                SWD_CLOCK_CYCLE();                                                       
            }                                                                         
            PIN_SWDIO_OUT_ENABLE();                                                   
        } else {                                                                    
            for (n = turnaround; n; n--) {                          
                SWD_CLOCK_CYCLE();                                                       
            }                                                                         
            PIN_SWDIO_OUT_ENABLE();                                                   
            val = *data;                                                              
            parity = 0;                                                               
            for (n = 32; n; n--) {                                                    
                SWD_WRITE_BIT(val);              // Write WDATA[0:31]                 
                parity += val;                                                          
                val >>= 1;                                                              
            }                                                                         
            SWD_WRITE_BIT(parity);              // Write Parity Bit                  
        }                                                                           
        n = idle_cycles;                                          
        if (n) {                                                                    
            PIN_SWDIO_OUT(0);                                                         
            for (; n; n--) {                                                          
                SWD_CLOCK_CYCLE();                                                       
            }                                                                         
        }                                                                           
        PIN_SWDIO_OUT(1);                                                           
    } else if ((ack == XLINK_TRANSFER_WAIT) || (ack == XLINK_TRANSFER_FAULT)) {              
        if (data_phase && ((request & XLINK_TRANSFER_RnW) != 0)) {  
            for (n = 32+1; n; n--) {                                                  
                SWD_CLOCK_CYCLE();              // Dummy Read RDATA[0:31] + Parity   
            }                                                                         
        }                                                                           
        /* Turnaround */                                                            
        for (n = turnaround; n; n--) {                            
            SWD_CLOCK_CYCLE();                                                         
        }                                                                           
        PIN_SWDIO_OUT_ENABLE();                                                     
        if (data_phase && ((request & XLINK_TRANSFER_RnW) == 0)) {  
            PIN_SWDIO_OUT(0);                                                         
            for (n = 32+1; n; n--) {                                                  
                SWD_CLOCK_CYCLE();              // Dummy Write WDATA[0:31] + Parity  
            }                                                                         
        }                                                                           
        PIN_SWDIO_OUT(1);                                                           
    } else {                                    // Protocol error                           
        for (n = turnaround + 32 + 1; n; n--) {                     
            SWD_CLOCK_CYCLE();                             
        }                                                                             
        PIN_SWDIO_OUT(1);
    }        
    return (ack);                                                                 
}
