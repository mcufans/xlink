/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbdesc.c
 * Purpose: USB Descriptors
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
 *----------------------------------------------------------------------------
 * History:
 *          V1.20 Changed string descriptor handling
 *                Changed HID Report Descriptor
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/
#include "type.h"
#include "usb.h"
#include "usbconfig.h"
#include "usbdesc.h"

const U8 USB_DeviceDescriptor[] = {         // USB设备描述符
    USB_DEVICE_DESC_SIZE,                   // 设备描述符长度
    USB_DEVICE_DESCRIPTOR_TYPE,             // 设备描述符类型数值
    WBVAL(0x0200),                          // USB协议版本号
    0xFF,                                   // 设备的类型代码,设置为0xFF，即厂商自定义类型
    0x00,                                   // 设备的类型代码
    0xFF,                                   // 设备所遵循的协议,设置为0xFF，即厂商自定义类型
    USB_MAX_PACKET0,                        // 端点0的最大数据包大小
    WBVAL(0xC251),                          // 制造商ID
    WBVAL(0x1C00),                          // 产品ID
    WBVAL(0x0100),                          // 设备ID
    0x01,                                   // 厂商字符串描述符索引
    0x02,                                   // 产品字符串描述符索引
    0x03,                                   // 设备字符串描述符索引
    0x01                                    // 配置描述符数量
};

const U8 USB_ConfigDescriptor[] = {         // USB配置描述符
    // 第一个配置描述符
    USB_CONFIGUARTION_DESC_SIZE,            // 配置描述符大小
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      // 配置描述符类型
    WBVAL(                                  // 所有配置描述符大小
        USB_CONFIGUARTION_DESC_SIZE +
        USB_INTERFACE_DESC_SIZE     +
        4 * USB_ENDPOINT_DESC_SIZE
    ),
    0x01,                                   // 支持的接口总数
    0x01,                                   // 配置设置索引
    0x00,                                   // 配置描述符的字符串，未用
    USB_CONFIG_BUS_POWERED,                 // 总线供电方式
    USB_CONFIG_POWER_MA(500),               // 最大耗电500mA

    // 接口描述符
    USB_INTERFACE_DESC_SIZE,                // 接口描述符长度
    USB_INTERFACE_DESCRIPTOR_TYPE,          // 接口描述符类型
    0x00,                                   // 接口编号
    0x00,                                   // 
    0x04,                                   // 端点总数
    0xFF,                                   // 接口所属类型,0xFF为厂商自定义
    0xFF,                                   // 接口所首郁类型,0xFF为厂商自定义
    0x00,                                   // 接口所用协议
    0x04,                                   // 接口的字符串描述符索引
    
    // 端点描述符1
    USB_ENDPOINT_DESC_SIZE,                 // 端点描述符长度
    USB_ENDPOINT_DESCRIPTOR_TYPE,           // 端点描述符类型
    USB_ENDPOINT_IN(1),                     // 端点描述符地址与方向
    USB_ENDPOINT_TYPE_BULK,                 // 端点的传输类型
    WBVAL(64),                              // 端点传输的包大小
    0x00,                                   // 主机轮循设备的周期，无用

    // 端点描述符1
    USB_ENDPOINT_DESC_SIZE,                 // 端点描述符长度
    USB_ENDPOINT_DESCRIPTOR_TYPE,           // 端点描述符类型
    USB_ENDPOINT_OUT(1),                    // 端点描述符地址与方向
    USB_ENDPOINT_TYPE_BULK,                 // 端点的传输类型
    WBVAL(64),                              // 端点传输的包大小
    0x00,                                   // 主机轮循设备的周期，无用
    
    // 端点描述符2
    USB_ENDPOINT_DESC_SIZE,                 // 端点描述符长度
    USB_ENDPOINT_DESCRIPTOR_TYPE,           // 端点描述符类型
    USB_ENDPOINT_IN(2),                     // 端点描述符地址与方向
    USB_ENDPOINT_TYPE_BULK,                 // 端点的传输类型
    WBVAL(64),                              // 端点传输的包大小
    0x00,                                   // 主机轮循设备的周期，无用

    // 端点描述符2
    USB_ENDPOINT_DESC_SIZE,                 // 端点描述符长度
    USB_ENDPOINT_DESCRIPTOR_TYPE,           // 端点描述符类型
    USB_ENDPOINT_OUT(2),                    // 端点描述符地址与方向
    USB_ENDPOINT_TYPE_BULK,                 // 端点的传输类型
    WBVAL(64),                              // 端点传输的包大小
    0x00,                                   // 主机轮循设备的周期，无用

    0                                       // 结束字节
};

const U8 USB_StringDescriptor[] = {         // 字符串描述符
    // Index 0x00: LANGID Codes
    0x04,                                   // 描述符长度
    USB_STRING_DESCRIPTOR_TYPE,             // 描述符类型
    WBVAL(0x0409),                          // US English编码
    
    // Index 0x01: Manufacturer
    (9*2 + 2),                              // 描述符长度/
    USB_STRING_DESCRIPTOR_TYPE,             // 描述符类型
    'L',0,
    'I',0,
    'S',0,
    'H',0,
    'U',0,
    'T',0,
    'O',0,
    'N',0,
    'G',0,
    // Index 0x02: Product */
    (16*2 + 2),                        /* bLength (17 Char + Type + lenght) */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'x',0,
    'L',0,
    'i',0,
    'n',0,
    'k',0,
    ' ',0,
    'K',0,
    'i',0,
    'c',0,
    'k',0,
    ' ',0,
    'S',0,
    't',0,
    'a',0,
    'r',0,
    't',0,
    // Index 0x03: Serial Number
    (12*2 + 2),                        /* bLength (12 Char + Type + lenght) */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    '0',0,                             /* allowed characters are       */
    '0',0,                             /*   0x0030 - 0x0039 ('0'..'9') */
    '0',0,                             /*   0x0041 - 0x0046 ('A'..'F') */
    '1',0,                             /*   length >= 26               */
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    // Index 0x04: Interface 0, Alternate Setting 0
    ( 3*2 + 2),                        /* bLength (3 Char + Type + lenght) */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'L',0,
    'S',0,
    'T',0,
};
