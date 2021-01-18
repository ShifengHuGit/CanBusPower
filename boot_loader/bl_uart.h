//*****************************************************************************
//
// bl_uart.h - Definitions for the UART transport functions.
//
// Copyright (c) 2006-2009 Luminary Micro, Inc.  All rights reserved.
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
// This is part of revision 4423 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#ifndef __BL_UART_H__
#define __BL_UART_H__

//*****************************************************************************
//
// This macro is used to generate a constant to represent the UART baud rate to
// processor clock rate ratio.  This prevents the need for run-time calculation
// of the ratio of baud rate to processor clock rate ratio.
//
//*****************************************************************************
#define UART_BAUD_RATIO(ulBaud) ((((CRYSTAL_FREQ * 8) / ulBaud) + 1) / 2)

//*****************************************************************************
//
// This defines the UART receive pin that is being used by the boot loader.
//
//*****************************************************************************
#define UART_RX                 (1 << 0)

//*****************************************************************************
//
// This defines the UART transmit pin that is being used by the boot loader.
//
//*****************************************************************************
#define UART_TX                 (1 << 1)

//*****************************************************************************
//
// This defines the combination of pins used to implement the UART port used by
// the boot loader.
//
//*****************************************************************************
#define UART_PINS               (UART_RX | UART_TX)

//*****************************************************************************
//
// UART Transport APIs
//
//*****************************************************************************
extern void UARTSend(const unsigned char *pucData, unsigned long ulSize);
extern void UARTReceive(unsigned char *pucData, unsigned long ulSize);
extern void UARTFlush(void);
extern int UARTAutoBaud(unsigned long *pulRatio);

//*****************************************************************************
//
// Define the transport functions if the UART is being used.
//
//*****************************************************************************
#ifdef UART_ENABLE_UPDATE
#define SendData                UARTSend
#define FlushData               UARTFlush
#define ReceiveData             UARTReceive
#endif

#endif // __BL_UART_H__
