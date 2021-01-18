//*****************************************************************************
//
// bl_config.c - A dummy C file to generate bl_config.in from bl_config.h.
//
// Copyright (c) 2007-2009 Luminary Micro, Inc.  All rights reserved.
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

#include "bl_config.h"

//*****************************************************************************
//
// Since the RV-MDK assembler is not able to run assembly code through the C
// preprocessor, the relevant contents of bl_config.h need to be converted to
// assembly format for inclusion into the RV-MDK startup code.  This file
// performs this conversion when manually run through the C preprocessor via:
//
//     armcc --device DLM -o bl_config.inc -E bl_config.c
//
// This file does not contain valid C code and will fail to compile (-E tells
// the compiler to preprocess but not attempt to compile the code).
//
//*****************************************************************************

//*****************************************************************************
//
// Define an assembler symbol for the stack size.
//
//*****************************************************************************
_STACK_SIZE                     equ     STACK_SIZE

//*****************************************************************************
//
// Define an assembler symbol for the application starting address.
//
//*****************************************************************************
_APP_START_ADDRESS              equ     APP_START_ADDRESS

//*****************************************************************************
//
// Define an assembler symbol if update via the UART is enabled.
//
//*****************************************************************************
#ifdef UART_ENABLE_UPDATE
_UART_ENABLE_UPDATE             equ     1
#endif

//*****************************************************************************
//
// Define an assember symbol if UART autobauding is enabled.
//
//*****************************************************************************
#ifdef UART_AUTOBAUD
_UART_AUTOBAUD                  equ     1
#endif

//*****************************************************************************
//
// Define an assembler symbol if update via Ethernet is enabled.
//
//*****************************************************************************
#ifdef ENET_ENABLE_UPDATE
_ENET_ENABLE_UPDATE             equ     1
#endif

//*****************************************************************************
//
// Define an assembler symbol if update via CAN is enabled.
//
//*****************************************************************************
#ifdef CAN_ENABLE_UPDATE
_CAN_ENABLE_UPDATE              equ     1
#endif

//*****************************************************************************
//
// Define an assembler symbol if update via USB is enabled.
//
//*****************************************************************************
#ifdef USB_ENABLE_UPDATE
_USB_ENABLE_UPDATE              equ     1
#endif

//*****************************************************************************
//
// The assembler will require an end statement at the end of the output
// bl_config.inc file.
//
//*****************************************************************************
    end
