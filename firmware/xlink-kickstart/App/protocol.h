/////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013 by lishutong
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
/////////////////////////////////////////////////////////////////////////////////
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

// XLink command
#define XLINK_CMD_DUMMY                 0x00            // Dummy
#define XLINK_CMD_INFO                  0x01            // Get firmware information#define XLINK_CMD_LED                   0x02            // Light on/off led
#define XLINK_CMD_MODE                  0x03            // Switch SWD/JTAG mode
#define XLINK_CMD_DISCONNECT            0x04            // Disconnect
#define XLINK_CMD_WRITEABORT            0x05            // Write abort register
#define XLINK_CMD_DELAY                 0x06            // Delay microsecond
#define XLINK_CMD_SETPINS               0x07            // Set SWD pins
#define XLINK_CMD_SET_FREQ              0x08            // Set frequence
#define XLINK_CMD_SEQUENCE              0x09            // Out TMS/TCK sequences
#define XLINK_CMD_SWD_CONFIG            0x0A            // Config SWD mode
#define XLINK_CMD_TRANSFER_CONFIG       0x0B            // Config SWD/JTAG transfer
#define XLINK_CMD_TRANSFER_BLOCK        0x0C            // Continue transfter DP/AP register
#define XLINK_CMD_ABORT_TRANSFER        0x0D            // Abort current transfer
#define XLINK_CMD_RESET                 0x0E            // Reset comunication
#define XLINK_CMD_INVALID               0xFF            // Invalid command

// XLink response
#define XLINK_ERROR_OK                  0x00            // OK
#define XLINK_ERROR_ERR                 0xFF            // Error

// Sub command for XLINK_CMD_INFO
#define XLINK_INFOCMD_VENDOR            0x00            // Get vendor name
#define XLINK_INFOCMD_PRODUCT           0x01            // Get product name
#define XLINK_INFOCMD_SER_NUM           0x02            // Get serial number
#define XLINK_INFOCMD_FW_VER            0x03            // Get firmware version
#define XLINK_INFOCMD_CAPABILITIES      0x04            // Get capabilities
#define XLINK_INFOCMD_PACKET_COUNT      0x05            // Get packet count
#define XLINK_INFOCMD_PACKET_SIZE       0x06            // Get packet size

// Sub command for XLINK_CMD_LED
#define XLINK_RUNNING_LED               0x00            // Running LED
#define XLINK_DEBUGER_LED               0x01            // Debugger LED

// Sub command for XLINK_CMD_MODE
#define XLINK_MODE_AUTODETECT           0x00            // Autodetect Port
#define XLINK_MODE_DISABLED             0x01            // Port Disabled
#define XLINK_MODE_SWD                  0x02            // SWD Port
#define XLINK_MODE_JTAG                 0x03            // JTAG Port

// Sub command for XLINK_CMD_SETPINS
#define XLINK_SETPINS_TCK               0x00            // SWCLK/TCK
#define XLINK_SETPINS_TMS               0x01            // SWDIO/TMS
#define XLINK_SETPINS_nRESET            0x05            // nRESET

// SWD Transfer Request
#define XLINK_TRANSFER_APnDP            (1 << 0)        // Acess AP or DP register
#define XLINK_TRANSFER_RnW              (1 << 1)        // Read/Write request
#define XLINK_TRANSFER_A2               (1 << 2)        // Register adress A2
#define XLINK_TRANSFER_A3               (1 << 3)        // Register adress A3
#define XLINK_TRANSFER_MATCH_VALUE      (1 << 4)
#define XLINK_TRANSFER_MATCH_MASK       (1 << 5)

// Response for SWD Transfer
#define XLINK_TRANSFER_OK               (1 << 0)        // Transfer OK
#define XLINK_TRANSFER_WAIT             (1 << 1)        // Wait 
#define XLINK_TRANSFER_FAULT            (1 << 2)        // Fault
#define XLINK_TRANSFER_ERROR            (1 << 3)        // Protocol error
#define XLINK_TRANSFER_MISMATCH         (1 << 4)        // Mismatch

// Debug Port Register Addresses
#define DP_IDCODE                       0x0             // Identification Code Register
#define DP_ABORT                        0x0             // AP Abort Register
#define DP_CTRL_STAT                    0x4             // Control/Status Register
#define DP_WCR                          0x4             // Wire Control Register
#define DP_RESEND                       0x8             // Read Resend Register ,
#define DP_SELECT                       0x8             // AP Select Register
#define DP_RDBUFF                       0xC             // The Read Buffer, RDBUFF

extern uint8_t   fast_clock;                           // Fast Clock Flag
extern uint32_t  clock_delay;                          // Clock Delay
extern uint16_t  retry_count;                          // Number of retries after WAIT response
extern uint16_t  match_retry;                          // Number of retries if read value does not match
extern uint32_t  match_mask;                           // Match Mask
extern uint8_t   turnaround;                           // Turnaround period
extern uint8_t   data_phase;                           // Always generate Data Phase
extern uint8_t   idle_cycles;                          // Idle cycles after transfer

// Functions
extern void     SWDOutTMSTCK (uint32_t count, uint8_t *data);
extern uint8_t  SWDTransfer (uint32_t request, uint32_t *data);
extern uint32_t ProtocolProcCommand (uint8_t *request, uint8_t *response);
extern void     ProtocolInit (void);

#endif  // PROTOCOL_H
