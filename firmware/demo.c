/*----------------------------------------------------------------------------
 * Name:    demo.c
 * Purpose: USB HID Demo
 * Note(s): 
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stm32l1xx.h>
#include "type.h"
#include "usb.h"
#include "usbuser.h"
#include "usbcfg_STM32F10x.h"
#include "usbhw_STM32F10x.h"

#include "DAP_config.h"
#include "DAP.h"

int main (void) 
{    
    DAP_Setup();     
    USB_Init();         
    
    // 初始化成功，闪烁一次LED
    LED_CONNECTED_OUT(1);                
    LED_RUNNING_OUT(1);                   
    Delayms(200);                         
    LED_RUNNING_OUT(0);                   
    LED_CONNECTED_OUT(0);
    
    USB_Connect(__FALSE);                            
    USB_Connect(__TRUE);                           
    while (!USB_configured());           
        
    // 点亮LED表明枚举始化成功
    LED_CONNECTED_OUT(1);                
    LED_RUNNING_OUT(1);                   
    
    while (1) {
        // 进入DAP命令处理流程
        USB_ProcessCommand();
    }
}
