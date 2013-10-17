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
#include <stm32l1xx.h>
#include "config.h"
#include "App.h"
#include "usb.h"
#include "usbconfig.h"
#include "protocol.h"

void Delayms (uint32_t ms)
{
    TIMER_STOP();
    TIMER_START(ms * 1000);
    while (!TIMER_EXPIRED());
    TIMER_STOP();
}

static unsigned char in[64];
static unsigned char out[64];

static unsigned char jtag2swd[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x9E, 0xE7,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00
};
uint32_t id = 0;

int main (void) 
{    
    USB_Init();         
    ProtocolInit();
    
		// Connect
		in[0] = XLINK_MODE_SWD;
		XLINK_Connect(in, out);
	
		// Out sequence
		in[0] = sizeof(jtag2swd) * 8;
		memcpy(in + 1, jtag2swd, sizeof(jtag2swd));
		XLINK_SWJ_Sequence(in, out);
		
		// Read ID
		do {
			id = 0;
			in[0] = 0;
			in[1] = 1;
			in[2] = 0;
			in[3] = 1 << 1;
			XLINK_SWDTransferBlock(in, out);
			id = *(unsigned long *)&out[3];
		}while ((id & 0xFFF) == 0x477);
		
    LED_CONNECTED_OUT(0);                
    LED_RUNNING_OUT(0);                   
    
    USB_Connect(__FALSE);                            
    USB_Connect(__TRUE);                           
    while (!USB_configured()) {
        LED_CONNECTED_OUT(0);                
        LED_RUNNING_OUT(0);                   
        Delayms(50);                         
        LED_CONNECTED_OUT(1);                
        LED_RUNNING_OUT(1);                   
        Delayms(50);                         
    }        
    LED_CONNECTED_OUT(1);                
    LED_RUNNING_OUT(1);                   
                
    while (1) {
        USB_ProcessCommand();
    }
}
