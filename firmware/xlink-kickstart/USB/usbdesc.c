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

const U8 USB_DeviceDescriptor[] = {         // USB�豸������
    USB_DEVICE_DESC_SIZE,                   // �豸����������
    USB_DEVICE_DESCRIPTOR_TYPE,             // �豸������������ֵ
    WBVAL(0x0200),                          // USBЭ��汾��
    0xFF,                                   // �豸�����ʹ���,����Ϊ0xFF���������Զ�������
    0x00,                                   // �豸�����ʹ���
    0xFF,                                   // �豸����ѭ��Э��,����Ϊ0xFF���������Զ�������
    USB_MAX_PACKET0,                        // �˵�0��������ݰ���С
    WBVAL(0xC251),                          // ������ID
    WBVAL(0x1C00),                          // ��ƷID
    WBVAL(0x0100),                          // �豸ID
    0x01,                                   // �����ַ�������������
    0x02,                                   // ��Ʒ�ַ�������������
    0x03,                                   // �豸�ַ�������������
    0x01                                    // ��������������
};

const U8 USB_ConfigDescriptor[] = {         // USB����������
    // ��һ������������
    USB_CONFIGUARTION_DESC_SIZE,            // ������������С
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      // ��������������
    WBVAL(                                  // ����������������С
        USB_CONFIGUARTION_DESC_SIZE +
        USB_INTERFACE_DESC_SIZE     +
        4 * USB_ENDPOINT_DESC_SIZE
    ),
    0x01,                                   // ֧�ֵĽӿ�����
    0x01,                                   // ������������
    0x00,                                   // �������������ַ�����δ��
    USB_CONFIG_BUS_POWERED,                 // ���߹��緽ʽ
    USB_CONFIG_POWER_MA(500),               // ���ĵ�500mA

    // �ӿ�������
    USB_INTERFACE_DESC_SIZE,                // �ӿ�����������
    USB_INTERFACE_DESCRIPTOR_TYPE,          // �ӿ�����������
    0x00,                                   // �ӿڱ��
    0x00,                                   // 
    0x04,                                   // �˵�����
    0xFF,                                   // �ӿ���������,0xFFΪ�����Զ���
    0xFF,                                   // �ӿ�����������,0xFFΪ�����Զ���
    0x00,                                   // �ӿ�����Э��
    0x04,                                   // �ӿڵ��ַ�������������
    
    // �˵�������1
    USB_ENDPOINT_DESC_SIZE,                 // �˵�����������
    USB_ENDPOINT_DESCRIPTOR_TYPE,           // �˵�����������
    USB_ENDPOINT_IN(1),                     // �˵���������ַ�뷽��
    USB_ENDPOINT_TYPE_BULK,                 // �˵�Ĵ�������
    WBVAL(64),                              // �˵㴫��İ���С
    0x00,                                   // ������ѭ�豸�����ڣ�����

    // �˵�������1
    USB_ENDPOINT_DESC_SIZE,                 // �˵�����������
    USB_ENDPOINT_DESCRIPTOR_TYPE,           // �˵�����������
    USB_ENDPOINT_OUT(1),                    // �˵���������ַ�뷽��
    USB_ENDPOINT_TYPE_BULK,                 // �˵�Ĵ�������
    WBVAL(64),                              // �˵㴫��İ���С
    0x00,                                   // ������ѭ�豸�����ڣ�����
    
    // �˵�������2
    USB_ENDPOINT_DESC_SIZE,                 // �˵�����������
    USB_ENDPOINT_DESCRIPTOR_TYPE,           // �˵�����������
    USB_ENDPOINT_IN(2),                     // �˵���������ַ�뷽��
    USB_ENDPOINT_TYPE_BULK,                 // �˵�Ĵ�������
    WBVAL(64),                              // �˵㴫��İ���С
    0x00,                                   // ������ѭ�豸�����ڣ�����

    // �˵�������2
    USB_ENDPOINT_DESC_SIZE,                 // �˵�����������
    USB_ENDPOINT_DESCRIPTOR_TYPE,           // �˵�����������
    USB_ENDPOINT_OUT(2),                    // �˵���������ַ�뷽��
    USB_ENDPOINT_TYPE_BULK,                 // �˵�Ĵ�������
    WBVAL(64),                              // �˵㴫��İ���С
    0x00,                                   // ������ѭ�豸�����ڣ�����

    0                                       // �����ֽ�
};

const U8 USB_StringDescriptor[] = {         // �ַ���������
    // Index 0x00: LANGID Codes
    0x04,                                   // ����������
    USB_STRING_DESCRIPTOR_TYPE,             // ����������
    WBVAL(0x0409),                          // US English����
    
    // Index 0x01: Manufacturer
    (9*2 + 2),                              // ����������/
    USB_STRING_DESCRIPTOR_TYPE,             // ����������
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
