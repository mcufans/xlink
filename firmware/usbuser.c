/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbuser.c
 * Purpose: USB Custom User Module
 * Version: V1.20
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

#include "type.h"

#include "usb.h"
#include "usbcfg_STM32F10x.h"
#include "usbhw_STM32F10x.h"
#include "usbcore.h"
#include "usbuser.h"

#include "hiduser.h"
#include "demo.h"

#include "DAP_config.h"
#include "DAP.h"

#define XLINK_CMD_ENDP_IN          0x01            // 命令和响应的数据传输端点号
#define XLINK_CMD_ENDP_OUT         0x81            // 命令和响应的数据传输端点号

static U32 usb_configged = 0;
static volatile uint8_t  USB_RequestCount;       // Request  Buffer Usage Flag
static volatile uint32_t USB_RequestIn;         // Request  Buffer In  Index
static volatile uint32_t USB_RequestOut;        // Request  Buffer Out Index

static volatile uint8_t  USB_ResponseIdle;      // Response Buffer Idle  Flag
static volatile uint8_t  USB_ResponseCount;      // Response Buffer Usage Flag
static volatile uint32_t USB_ResponseIn;        // Response Buffer In  Index
static volatile uint32_t USB_ResponseOut;       // Response Buffer Out Index

static  uint8_t  USB_Request [DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Request  Buffer
static  uint8_t  USB_Response[DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Response Buffer
static  uint8_t USB_ResponseLen[DAP_PACKET_COUNT];

U32 USB_configured (void)
{
    return usb_configged;
}

/*
 *  USB Power Event Callback
 *   Called automatically on USB Power Event
 *    Parameter:       power: On(TRUE)/Off(FALSE)
 */

#if USB_POWER_EVENT
void USB_Power_Event (BOOL  power) {
}
#endif


/*
 *  USB Reset Event Callback
 *   Called automatically on USB Reset Event
 */

#if USB_RESET_EVENT
void USB_Reset_Event (void) {
  USB_ResetCore();
}
#endif


/*
 *  USB Suspend Event Callback
 *   Called automatically on USB Suspend Event
 */

#if USB_SUSPEND_EVENT
void USB_Suspend_Event (void) {
}
#endif


/*
 *  USB Resume Event Callback
 *   Called automatically on USB Resume Event
 */

#if USB_RESUME_EVENT
void USB_Resume_Event (void) {
}
#endif


/*
 *  USB Remote Wakeup Event Callback
 *   Called automatically on USB Remote Wakeup Event
 */

#if USB_WAKEUP_EVENT
void USB_WakeUp_Event (void) {
}
#endif


/*
 *  USB Start of Frame Event Callback
 *   Called automatically on USB Start of Frame Event
 */

#if USB_SOF_EVENT
void USB_SOF_Event (void) {
}
#endif


/*
 *  USB Error Event Callback
 *   Called automatically on USB Error Event
 *    Parameter:       error: Error Code
 */

#if USB_ERROR_EVENT
void USB_Error_Event (DWORD error) {
}
#endif


/*
 *  USB Set Configuration Event Callback
 *   Called automatically on USB Set Configuration Request
 */

#if USB_CONFIGURE_EVENT
void USB_Configure_Event (void) 
{
    usb_configged       = 1;
    USB_RequestCount    = 0;
    USB_RequestIn       = 0;
    USB_RequestOut      = 0;
    USB_ResponseIdle    = 1;
    USB_ResponseCount   = 0;
    USB_ResponseIn      = 0;
    USB_ResponseOut     = 0;
}
#endif


/*
 *  USB Set Interface Event Callback
 *   Called automatically on USB Set Interface Request
 */

#if USB_INTERFACE_EVENT
void USB_Interface_Event (void) {
}
#endif


/*
 *  USB Set/Clear Feature Event Callback
 *   Called automatically on USB Set/Clear Feature Request
 */

#if USB_FEATURE_EVENT
void USB_Feature_Event (void) {
}
#endif


#define P_EP(n) ((USB_EP_EVENT & (1 << (n))) ? USB_EndPoint##n : NULL)

/* USB Endpoint Events Callback Pointers */
void (* const USB_P_EP[16]) (U32 event) = {
  P_EP(0),
  P_EP(1),
  P_EP(2),
  P_EP(3),
  P_EP(4),
  P_EP(5),
  P_EP(6),
  P_EP(7),
  P_EP(8),
  P_EP(9),
  P_EP(10),
  P_EP(11),
  P_EP(12),
  P_EP(13),
  P_EP(14),
  P_EP(15),
};


/*
 *  USB Endpoint 1 Event Callback
 *   Called automatically on USB Endpoint 1 Event
 *    Parameter:       event
 */

void USB_EndPoint1 (U32 event) 
{   
    if (event == USB_EVT_OUT) {     // 接收处理
        // 仅当缓冲列表有空闲项时才读取数据
        if (USB_RequestCount < DAP_PACKET_COUNT) {
            USB_ReadEP(XLINK_CMD_ENDP_IN, USB_Request[USB_RequestIn]);
            
            // 增加读取的包计数和写索引
            USB_RequestIn++;                            
            if (USB_RequestIn == DAP_PACKET_COUNT) {
                USB_RequestIn = 0;
            }
            USB_RequestCount++;
        }
    } else {                        // 发送处理
        if (USB_ResponseCount) {
            USB_WriteEP(XLINK_CMD_ENDP_OUT, USB_Response[USB_ResponseOut], USB_ResponseLen[USB_ResponseOut]);
            
            // 纠正读索引和包计数
            USB_ResponseOut++;
            if (USB_ResponseOut == DAP_PACKET_COUNT) {
              USB_ResponseOut = 0;
            }
            USB_ResponseCount--;
        } else {
            // 当所有数据包发送完毕后，设置发送空闲标志
            USB_ResponseIdle = 1;
        }
    }
}

void USB_ProcessCommand (void) 
{
    uint32_t n = 0;
    uint32_t resp_len = 0;

    if (USB_RequestCount) {
        resp_len = DAP_ProcessCommand(USB_Request[USB_RequestOut], USB_Response[USB_ResponseIn]);
        
        USB_ResponseLen[USB_ResponseIn] = resp_len; 
        
        // 校正请求响应写索引和包计数
        n = USB_RequestOut + 1;
        if (n == DAP_PACKET_COUNT) {
            n = 0;
        }
        USB_RequestOut = n;
        USB_RequestCount--;

        if (USB_ResponseIdle) {
            // 如果发送处于空闲状态，则可以写第一次来触发
            // 后续将由中断服务程序自动发送
            USB_ResponseIdle = 0;
            USB_WriteEP(XLINK_CMD_ENDP_OUT, USB_Response[USB_ResponseOut], resp_len);
        } else {  
            // 纠正响应写索引和包计数
            n = USB_ResponseIn + 1;
            if (n == DAP_PACKET_COUNT) {
                n = 0;
            }
            USB_ResponseIn = n;
            USB_ResponseCount++;
        }
    }
}

/*
 *  USB Endpoint 2 Event Callback
 *   Called automatically on USB Endpoint 2 Event
 *    Parameter:       event
 */

void USB_EndPoint2 (U32 event) 
{
}

/*
 *  USB Endpoint 3 Event Callback
 *   Called automatically on USB Endpoint 3 Event
 *    Parameter:       event
 */

void USB_EndPoint3 (U32 event) 
{
}

/*
 *  USB Endpoint 4 Event Callback
 *   Called automatically on USB Endpoint 4 Event
 *    Parameter:       event
 */

void USB_EndPoint4 (U32 event) {
}


/*
 *  USB Endpoint 5 Event Callback
 *   Called automatically on USB Endpoint 5 Event
 *    Parameter:       event
 */

void USB_EndPoint5 (U32 event) {
}


/*
 *  USB Endpoint 6 Event Callback
 *   Called automatically on USB Endpoint 6 Event
 *    Parameter:       event
 */

void USB_EndPoint6 (U32 event) {
}


/*
 *  USB Endpoint 7 Event Callback
 *   Called automatically on USB Endpoint 7 Event
 *    Parameter:       event
 */

void USB_EndPoint7 (U32 event) {
}


/*
 *  USB Endpoint 8 Event Callback
 *   Called automatically on USB Endpoint 8 Event
 *    Parameter:       event
 */

void USB_EndPoint8 (U32 event) {
}


/*
 *  USB Endpoint 9 Event Callback
 *   Called automatically on USB Endpoint 9 Event
 *    Parameter:       event
 */

void USB_EndPoint9 (U32 event) {
}


/*
 *  USB Endpoint 10 Event Callback
 *   Called automatically on USB Endpoint 10 Event
 *    Parameter:       event
 */

void USB_EndPoint10 (U32 event) {
}


/*
 *  USB Endpoint 11 Event Callback
 *   Called automatically on USB Endpoint 11 Event
 *    Parameter:       event
 */

void USB_EndPoint11 (U32 event) {
}


/*
 *  USB Endpoint 12 Event Callback
 *   Called automatically on USB Endpoint 12 Event
 *    Parameter:       event
 */

void USB_EndPoint12 (U32 event) {
}


/*
 *  USB Endpoint 13 Event Callback
 *   Called automatically on USB Endpoint 13 Event
 *    Parameter:       event
 */

void USB_EndPoint13 (U32 event) {
}


/*
 *  USB Endpoint 14 Event Callback
 *   Called automatically on USB Endpoint 14 Event
 *    Parameter:       event
 */

void USB_EndPoint14 (U32 event) {
}


/*
 *  USB Endpoint 15 Event Callback
 *   Called automatically on USB Endpoint 15 Event
 *    Parameter:       event
 */

void USB_EndPoint15 (U32 event) {
}
