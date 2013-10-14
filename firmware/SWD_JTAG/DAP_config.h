/**************************************************************************//**
 * @file     DAP_config.h
 * @brief    CMSIS-DAP Configuration File for OpenSDA
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
#ifndef __DAP_CONFIG_H__
#define __DAP_CONFIG_H__

#include <stm32l1xx.h>

#define CPU_CLOCK               32000000            // MCU������Ƶ
#define IO_PORT_WRITE_CYCLES    2                   ///< I/O Cycles: 2=default, 1=Cortex-M0+ fast I/0
#define DAP_DEFAULT_PORT        1                   ///< Default JTAG/SWJ Port Mode: 1 = SWD, 2 = JTAG.
#define DAP_DEFAULT_SWJ_CLOCK   1000000             ///< Default SWD/JTAG clock frequency in Hz.
#define DAP_PACKET_SIZE         64                  ///< USB: 64 = Full-Speed, 1024 = High-Speed.
#define DAP_PACKET_COUNT        64                  ///< Buffers: 64 = Full-Speed, 4 = High-Speed.
#define DAP_VENDOR              "XLINKER"
#define DAP_PRODUCT             "XLink-KickStart"
#define DAP_SER_NUM             "0000101201"

///@}

//**************************************************************************************************
/** 
\defgroup DAP_Config_PortIO_gr CMSIS-DAP Hardware I/O Pin Access
\ingroup DAP_ConfigIO_gr 
@{

Standard I/O Pins of the CMSIS-DAP Hardware Debug Port support standard JTAG mode
and Serial Wire Debug (SWD) mode. In SWD mode only 2 pins are required to implement the debug 
interface of a device. The following I/O Pins are provided:

JTAG I/O Pin                 | SWD I/O Pin          | CMSIS-DAP Hardware pin mode
---------------------------- | -------------------- | ---------------------------------------------
TCK: Test Clock              | SWCLK: Clock         | Output Push/Pull
TMS: Test Mode Select        | SWDIO: Data I/O      | Output Push/Pull; Input (for receiving data)
TDI: Test Data Input         |                      | Output Push/Pull
TDO: Test Data Output        |                      | Input             
nTRST: Test Reset (optional) |                      | Output Open Drain with pull-up resistor
nRESET: Device Reset         | nRESET: Device Reset | Output Open Drain with pull-up resistor


DAP Hardware I/O Pin Access Functions
-------------------------------------
The various I/O Pins are accessed by functions that implement the Read, Write, Set, or Clear to 
these I/O Pins. 

For the SWDIO I/O Pin there are additional functions that are called in SWD I/O mode only.
This functions are provided to achieve faster I/O that is possible with some advanced GPIO 
peripherals that can independently write/read a single I/O pin without affecting any other pins 
of the same I/O port. The following SWDIO I/O Pin functions are provided:
 - \ref PIN_SWDIO_OUT_ENABLE to enable the output mode from the DAP hardware.
 - \ref PIN_SWDIO_OUT_DISABLE to enable the input mode to the DAP hardware.
 - \ref PIN_SWDIO_IN to read from the SWDIO I/O pin with utmost possible speed.
 - \ref PIN_SWDIO_OUT to write to the SWDIO I/O pin with utmost possible speed.
*/


// Configure DAP I/O pins ------------------------------

/** Setup JTAG I/O pins: TCK, TMS, TDI, TDO, nTRST, and nRESET.
Configures the DAP Hardware I/O pins for JTAG mode:
 - TCK, TMS, TDI, nTRST, nRESET to output mode and set to high level.
 - TDO to input mode.
*/ 
static __inline void PORT_JTAG_SETUP (void) {}
 
/** Setup SWD I/O pins: SWCLK, SWDIO, and nRESET.
Configures the DAP Hardware I/O pins for Serial Wire Debug (SWD) mode:
 - SWCLK, SWDIO, nRESET to output mode and set to default high level.
 - TDI, TMS, nTRST to HighZ mode (pins are unused in SWD mode).
*/ 
static __inline void PORT_SWD_SETUP (void) 
{
    GPIOB->BSRRL = (1 << 10);        // PB10(NRST)����ߵ�ƽ
    GPIOB->BSRRL = (1 << 11);        // PB11(SWCLK)����ߵ�ƽ  
    GPIOB->BSRRL = (1 << 12);        // PB12(SWDIO)����ߵ�ƽ  
    GPIOC->BSRRH = (1 << 8);         // PC8(SWDEN)����͵�ƽ��ʹ��SWD
    GPIOC->BSRRH = (1 << 7);         // PC7����͵�ƽ��ʹ��SWDIO���  
}

/** Disable JTAG/SWD I/O Pins.
Disables the DAP Hardware I/O pins which configures:
 - TCK/SWCLK, TMS/SWDIO, TDI, TDO, nTRST, nRESET to High-Z mode.
*/
static __inline void PORT_OFF (void) 
{
    GPIOC->BSRRL = (1 << 8);         // PC8����ߵ�ƽ��ȫ���������
}


/** SWCLK/TCK I/O pin: Get Input.
\return Current status of the SWCLK/TCK DAP hardware I/O pin.
*/
static __forceinline unsigned long PIN_SWCLK_TCK_IN  (void) 
{
  return (0);   
}

/** SWCLK/TCK I/O pin: Set Output to High.
Set the SWCLK/TCK DAP hardware I/O pin to high level.
*/
static __forceinline void PIN_SWCLK_TCK_SET (void) 
{
    GPIOB->BSRRL = (1 << 11);        // PB11����ߵ�ƽ  
}

/** SWCLK/TCK I/O pin: Set Output to Low.
Set the SWCLK/TCK DAP hardware I/O pin to low level.
*/
static __forceinline void PIN_SWCLK_TCK_CLR (void) 
{
    GPIOB->BSRRH = (1 << 11);        // PB11����͵�ƽ       
}


/** SWDIO/TMS I/O pin: Get Input.
\return Current status of the SWDIO/TMS DAP hardware I/O pin.
*/
static __forceinline unsigned long PIN_SWDIO_TMS_IN  (void) 
{
    return ((GPIOB->IDR >> 3) & 0x1);       // PB3����״̬
}

/** SWDIO/TMS I/O pin: Set Output to High.
Set the SWDIO/TMS DAP hardware I/O pin to high level.
*/
static __forceinline void     PIN_SWDIO_TMS_SET (void) 
{
    GPIOB->BSRRL = (1 << 12);
}

/** SWDIO/TMS I/O pin: Set Output to Low.
Set the SWDIO/TMS DAP hardware I/O pin to low level.
*/
static __forceinline void     PIN_SWDIO_TMS_CLR (void) 
{
    GPIOB->BSRRH = (1 << 12);
}

/** SWDIO I/O pin: Get Input (used in SWD mode only).
\return Current status of the SWDIO DAP hardware I/O pin.
*/
static __forceinline unsigned long PIN_SWDIO_IN (void) 
{
    return ((GPIOB->IDR >> 3) & 0x1);       // PB3����״̬
}

/** SWDIO I/O pin: Get Input (used in SWD mode only).
\return Current status of the SWDIO DAP hardware I/O pin.
*/
static __forceinline unsigned long PIN_SWDO_IN (void) 
{
    return ((GPIOB->IDR >> 4) & 0x1);       // PB3����״̬
}


/** SWDIO I/O pin: Set Output (used in SWD mode only).
\param bit Output value for the SWDIO DAP hardware I/O pin.
*/
static __forceinline void PIN_SWDIO_OUT (unsigned long bit) 
{
    if (bit & 0x1) 
        GPIOB->BSRRL = (1 << 12); 
    else 
        GPIOB->BSRRH = (1 << 12);
}

/** SWDIO I/O pin: Switch to Output mode (used in SWD mode only).
Configure the SWDIO DAP hardware I/O pin to output mode. This function is
called prior \ref PIN_SWDIO_OUT function calls.
*/
static __forceinline void PIN_SWDIO_OUT_ENABLE  (void) 
{
    GPIOC->BSRRH = (1 << 7);         // PC7����͵�ƽ    
}

/** SWDIO I/O pin: Switch to Input mode (used in SWD mode only).
Configure the SWDIO DAP hardware I/O pin to input mode. This function is
called prior \ref PIN_SWDIO_IN function calls.
*/
static __forceinline void PIN_SWDIO_OUT_DISABLE (void) 
{
    GPIOC->BSRRL = (1 << 7);         // PC7����ߵ�ƽ
}

/** TDI I/O pin: Get Input.
\return Current status of the TDI DAP hardware I/O pin.
*/
static __forceinline unsigned long PIN_TDI_IN  (void) {
  return (0);   // Not available
}

/** TDI I/O pin: Set Output.
\param bit Output value for the TDI DAP hardware I/O pin.
*/
static __forceinline void     PIN_TDI_OUT (unsigned long bit) {
  ;             // Not available
}

/** TDO I/O pin: Get Input.
\return Current status of the TDO DAP hardware I/O pin.
*/
static __forceinline unsigned long PIN_TDO_IN  (void) {
  return (0);   // Not available
}

/** nTRST I/O pin: Get Input.
\return Current status of the nTRST DAP hardware I/O pin.
*/
static __forceinline unsigned long PIN_nTRST_IN   (void) {
  return (0);   // Not available
}


/** nTRST I/O pin: Set Output.
\param bit JTAG TRST Test Reset pin status:
           - 0: issue a JTAG TRST Test Reset.
           - 1: release JTAG TRST Test Reset.
*/
static __forceinline void PIN_nTRST_OUT  (unsigned long bit) {
  ;             // Not available
}

/** nRESET I/O pin: Get Input.
\return Current status of the nRESET DAP hardware I/O pin.
*/
static __forceinline unsigned long PIN_nRESET_IN  (void) 
{
    return 1;       // Disable
}


/** nRESET I/O pin: Set Output.
\param bit target device hardware reset pin status:
           - 0: issue a device hardware reset.
           - 1: release device hardware reset.
*/
static __forceinline void     PIN_nRESET_OUT (unsigned long bit) 
{
    if (bit)
        GPIOB->BSRRL = (1 << 10);        // PB10����ߵ�ƽ          
    else
        GPIOB->BSRRH = (1 << 10);        // PB10����͵�ƽ    
}

/** Debug Unit: Set status of Connected LED.
\param bit status of the Connect LED.
           - 1: Connect LED ON: debugger is connected to CMSIS-DAP Debug Unit.
           - 0: Connect LED OFF: debugger is not connected to CMSIS-DAP Debug Unit.
*/
static __inline void LED_CONNECTED_OUT (unsigned long bit) 
{
    if (bit & 0x1) {
        GPIOB->BSRRH = (0x1 << 1);                  // PB1����͵�ƽ������
    } else {
        GPIOB->BSRRL = (0x1 << 1);                  // PB1����ߵ�ƽ��Ϩ��
    }
}

/** Debug Unit: Set status Target Running LED.
\param bit status of the Target Running LED.
           - 1: Target Running LED ON: program execution in target started.
           - 0: Target Running LED OFF: program execution in target stopped.
*/
static __inline void LED_RUNNING_OUT (unsigned long bit) 
{
    if (bit & 0x1) {
        GPIOB->BSRRH = (0x1 << 0);                  // PB0����͵�ƽ������
    } else {
        GPIOB->BSRRL = (0x1 << 0);                  // PB0����ߵ�ƽ��Ϩ��
    }
}

/** Setup of the Debug Unit I/O pins and LEDs (called when Debug Unit is initialized).
This function performs the initialization of the CMSIS-DAP Hardware I/O Pins and the 
Status LEDs. In detail the operation of Hardware I/O and LED pins are enabled and set:
 - I/O clock system enabled.
 - all I/O pins: input buffer enabled, output pins are set to HighZ mode.
 - for nTRST, nRESET a weak pull-up (if available) is enabled.
 - LED output pins are enabled and LEDs are turned off.
*/
static __inline void DAP_SETUP (void) 
{
    // LED��ʼ��
    RCC->AHBENR |= (1 << 1);                            // ��GPIOB�˿ڵ�ʱ��
    GPIOB->BSRRL |= (0x3 << 0);                         // ��ʼ�ߵ�ƽ��ȫ��Ϩ��
    GPIOB->MODER &= ~(0xF << 0);
    GPIOB->MODER |= (0x5 << 0);                         // PB0/PB1Ϊ���
    GPIOB->OTYPER &= ~(0x3 << 0);                       // ��Ϊ�������
    
    // EN_SWD��EN_SWDIO��ʼ��
    RCC->AHBENR |= (1 << 2);                            // ��GPIOC�˿ڵ�ʱ��
    GPIOC->BSRRL |= (0x3 << 7);                         // ��ʼ�ߵ�ƽ��ȫ������
    GPIOC->MODER &= ~(0xF << 14);
    GPIOC->MODER |= (0x5 << 14);                        // PC7/PB8Ϊ���
    GPIOC->OTYPER &= ~(0x3 << 7);                       // ��Ϊ�������
    
    // M_NRST��M_SWCLK��M_SWDIO��I_SWDIO��I_SWO��ʼ��
    RCC->AHBENR |= (1 << 1);                            // ��GPIOB�˿ڵ�ʱ��
    
    // M_NRST��M_SWCLK��M_SWDIO��ʼ��
    GPIOB->BSRRL |= (0x7 << 10);                        // ��ʼ�ߵ�ƽ
    GPIOB->MODER &= ~(0x3F << 20);
    GPIOB->MODER |= (0x15 << 20);                       // PB10/PB11/PB12Ϊ���
    GPIOB->OTYPER &= ~(0x7 << 10);                      // ��Ϊ�������
    
    // I_SWIO��I_SWO��ʼ��
    GPIOB->MODER &= ~(0xF << 3);                        // ȫ������Ϊ���    
}

/** Reset Target Device with custom specific I/O pin or command sequence.
This function allows the optional implementation of a device specific reset sequence.
It is called when the command \ref DAP_ResetTarget and is for example required 
when a device needs a time-critical unlock sequence that enables the debug port.
\return 0 = no device specific reset sequence is implemented.\n
        1 = a device specific reset sequence is implemented.
*/
static __inline unsigned long RESET_TARGET (void) {
  return (0);              // change to '1' when a device reset sequence is implemented
}

// Configurable delay for clock generation
#define DELAY_SLOW_CYCLES       3       // Number of cycles for one iteration
static __forceinline void PIN_DELAY_SLOW (uint32_t delay) {
  volatile int32_t count;

  count = delay;
  while (--count);
}

// Fixed delay for fast clock generation
#define DELAY_FAST_CYCLES   1        // ��0.9MHz������
static __forceinline void PIN_DELAY_FAST (void) {
  volatile int32_t count = DELAY_FAST_CYCLES;
  while (--count);
}

#endif /* __DAP_CONFIG_H__ */
