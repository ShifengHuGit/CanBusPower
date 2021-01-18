//*****************************************************************************
//
// bl_config.h - The configurable parameters of the boot loader.
//
// Copyright (c) 2008-2009 Luminary Micro, Inc.  All rights reserved.
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 4423 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

#ifndef __BL_CONFIG_H__
#define __BL_CONFIG_H__

//*****************************************************************************
//
// The following defines are used to configure the operation of the boot
// loader.  For each define, its interactions with other defines are described.
// First is the dependencies (i.e. the defines that must also be defined if it
// is defined), next are the exclusives (i.e. the defines that can not be
// defined if it is defined), and finally are the requirements (i.e. the
// defines that must be defined if it is defined).
//
// The following defines must be defined in order for the boot loader to
// operate:
//
//     One of ENET_ENABLE_UPDATE, I2C_ENABLE_UPDATE, SSI_ENABLE_UPDATE, or
//         UART_ENABLE_UPDATE
//     APP_START_ADDRESS
//     STACK_SIZE
//     BUFFER_SIZE
//
//*****************************************************************************

//*****************************************************************************
//
// The frequency of the crystal used to clock the microcontroller.
//
// This defines the crystal frequency used by the microcontroller running the
// boot loader.  If this is unknown at the time of production, then use the
// UART_AUTOBAUD feature to properly configure the UART.
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
#define CRYSTAL_FREQ            8000000

//*****************************************************************************
//
// This enables the boosting of the LDO voltage to 2.75V.  For boot loader
// configurations that enable the PLL (i.e. using the Ethernet port) on a part
// that has the PLL errata, this should be enabled.  This applies to revision
// A2 of Fury-class devices.
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
#define BOOST_LDO_VOLTAGE

//*****************************************************************************
//
// The starting address of the application.  This must be a multiple of 1024
// bytes (making it aligned to a page boundary).  A vector table is expected at
// this location, and the perceived validity of the vector table (stack located
// in SRAM, reset vector located in flash) is used as an indication of the
// validity of the application image.
//
// The flash image of the boot loader must not be larger than this value.
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
#define APP_START_ADDRESS       0x00001000

//*****************************************************************************
//
// The amount of space at the end of flash to reserved.  This must be a
// multiple of 1024 bytes (making it aligned to a page boundary).  This
// reserved space is not erased when the application is updated, providing
// non-volatile storage that can be used for parameters.
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
//#define FLASH_RSVD_SPACE        0x00000800

//*****************************************************************************
//
// The number of words of stack space to reserve for the boot loader.
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
#define STACK_SIZE              48

//*****************************************************************************
//
// The number of words in the data buffer used for receiving packets.  This
// value must be at least 3.  If using autobauding on the UART, this must be at
// least 20.  The maximum usable value is 65 (larger values will result in
// unused space in the buffer).
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
//#define BUFFER_SIZE             20

//*****************************************************************************
//
// Enables updates to the boot loader.  Updating the boot loader is an unsafe
// operation since it is not fully fault tolerant (losing power to the device
// part way though could result in the boot loader no longer being present in
// flash).
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
//#define ENABLE_BL_UPDATE

//*****************************************************************************
//
// This definition will cause the the boot loader to erase the entire flash on
// updates to the boot loader or to erase the entire application area when the
// application is updated.  This erases any unused sections in the flash before
// the firmware is updated.
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
//#define FLASH_CODE_PROTECTION

//*****************************************************************************
//
// Enables the call to decrypt the downloaded data before writing it into
// flash.  The decryption routine is empty in the reference boot loader source,
// which simply provides a placeholder for adding an actual decrypter.
//
// Depends on: None
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
//#define ENABLE_DECRYPTION

//*****************************************************************************
//
// Enables the pin-based forced update check.  When enabled, the boot loader
// will go into update mode instead of calling the application if a pin is read
// at a particular polarity, forcing an update operation.  In either case, the
// application is still able to return control to the boot loader in order to
// start an update.
//
// Depends on: None
// Exclusive of: None
// Requires: FORCED_UPDATE_PERIPH, FORCED_UPDATE_PORT, FORCED_UPDATE_PIN,
//           FORCED_UPDATE_POLARITY
//
//*****************************************************************************
//#define ENABLE_UPDATE_CHECK

//*****************************************************************************
//
// The GPIO module to enable in order to check for a forced update.  This will
// be one of the SYSCTL_RCGC2_GPIOx values, where "x" is replaced with the port
// name (such as B).  The value of "x" should match the value of "x" for
// FORCED_UPDATE_PORT.
//
// Depends on: ENABLE_UPDATE_CHECK
// Exclusive of: None
// Requries: None
//
//*****************************************************************************
//#define FORCED_UPDATE_PERIPH    SYSCTL_RCGC2_GPIOB

//*****************************************************************************
//
// The GPIO port to check for a forced update.  This will be one of the
// GPIO_PORTx_BASE values, where "x" is replaced with the port name (such as
// B).  The value of "x" should match the value of "x" for
// FORCED_UPDATE_PERIPH.
//
// Depends on: ENABLE_UPDATE_CHECK
// Exclusive of: None
// Requries: None
//
//*****************************************************************************
//#define FORCED_UPDATE_PORT      GPIO_PORTB_BASE

//*****************************************************************************
//
// The pin to check for a forced update.  This is a value between 0 and 7.
//
// Depends on: ENABLE_UPDATE_CHECK
// Exclusive of: None
// Requries: None
//
//*****************************************************************************
//#define FORCED_UPDATE_PIN       4

//*****************************************************************************
//
// The polarity of the GPIO pin that results in a forced update.  This value
// should be 0 if the pin should be low and 1 if the pin should be high.
//
// Depends on: ENABLE_UPDATE_CHECK
// Exclusive of: None
// Requries: None
//
//*****************************************************************************
//#define FORCED_UPDATE_POLARITY  0

//*****************************************************************************
//
// Selects the UART as the port for communicating with the boot loader.
//
// Depends on: None
// Exclusive of: ENET_ENABLE_UPDATE, I2C_ENABLE_UPDATE, SSI_ENABLE_UPDATE
// Requires: UART_AUTOBAUD or UART_FIXED_BAUDRATE
//
//*****************************************************************************
//#define UART_ENABLE_UPDATE

//*****************************************************************************
//
// Enables automatic baud rate detection.  This can be used if the crystal
// frequency is unknown, or if operation at different baud rates is desired.
//
// Depends on: UART_ENABLE_UPDATE
// Exclusive of: UART_FIXED_BAUDRATE
// Requires: None
//
//*****************************************************************************
//#define UART_AUTOBAUD

//*****************************************************************************
//
// Selects the baud rate to be used for the UART.
//
// Depends on: UART_ENABLE_UPDATE, CRYSTAL_FREQ
// Exclusive of: UART_AUTOBAUD
// Requires: None
//
//*****************************************************************************
//#define UART_FIXED_BAUDRATE     115200

//*****************************************************************************
//
// Selects the SSI port as the port for communicating with the boot loader.
//
// Depends on: None
// Exclusive of: ENET_ENABLE_UPDATE, I2C_ENABLE_UPDATE, UART_ENABLE_UPDATE
// Requires: None
//
//*****************************************************************************
//#define SSI_ENABLE_UPDATE

//*****************************************************************************
//
// Selects the I2C port as the port for communicating with the boot loader.
//
// Depends on: None
// Exclusive of: ENET_ENABLE_UPDATE, SSI_ENABLE_UPDATE, UART_ENABLE_UPDATE
// Requires: I2C_SLAVE_ADDR
//
//*****************************************************************************
//#define I2C_ENABLE_UPDATE

//*****************************************************************************
//
// Specifies the I2C address of the boot loader.
//
// Depends on: I2C_ENABLE_UPDATE
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
//#define I2C_SLAVE_ADDR          0x42

//*****************************************************************************
//
// Selects Ethernet update via the BOOTP/TFTP protocol.
//
// Depends on: None
// Exclusive of: I2C_ENABLE_UPDATE, SSI_ENABLE_UPDATE, UART_ENABLE_UPDATE
// Requires: CRYSTAL_FREQ
//
//*****************************************************************************
#define ENET_ENABLE_UPDATE

//*****************************************************************************
//
// Selects if the Ethernet LEDs should be enabled.
//
// Depends on: ENET_ENABLE_UPDATE
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
#define ENET_ENABLE_LEDS

//*****************************************************************************
//
// Selects the Ethernet MAC address.  If not specified, the MAC address is
// taken from the user registers.
//
// Depends on: ENET_ENABLE_UPDATE
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
//#define ENET_MAC_ADDR0          0x00
//#define ENET_MAC_ADDR1          0x00
//#define ENET_MAC_ADDR2          0x00
//#define ENET_MAC_ADDR3          0x00
//#define ENET_MAC_ADDR4          0x00
//#define ENET_MAC_ADDR5          0x00

//*****************************************************************************
//
// Sets the name of the BOOTP server to use.  This can be used to request that
// a particular BOOTP server respond to our request; the value will be either
// the server's name, or a nickname used by that server.  If not defined then
// any BOOTP server is allowed to respond.
//
// Depends on: ENET_ENABLE_UPDATE
// Exclusive of: None
// Requires: None
//
//*****************************************************************************
#define ENET_BOOTP_SERVER       "stellaris"

#endif // __BL_CONFIG_H__
