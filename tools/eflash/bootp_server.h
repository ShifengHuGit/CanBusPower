//*****************************************************************************
//
// bootp_server.h - Prototypes for the simplistic BOOTP/TFTP server.
//
// Copyright (c) 2009 Luminary Micro, Inc.  All rights reserved.
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

#ifndef __BOOTP_SERVER_H__
#define __BOOTP_SERVER_H__

//*****************************************************************************
//
// The return codes for StartBOOTPUpdate.
//
//*****************************************************************************
#define ERROR_NONE              0
#define ERROR_FILE              1
#define ERROR_BOOTPD            2
#define ERROR_TFTPD             3
#define ERROR_MPACKET           4

//*****************************************************************************
//
// Prototypes for the functions used to interface with the BOOTP/TFTP server.
//
//*****************************************************************************
typedef void (*tfnCallback)(unsigned long ulPercent);
extern unsigned long StartBOOTPUpdate(unsigned char *pucMACAddr,
                                      unsigned long ulLocalAddr,
                                      unsigned long ulRemoteAddr,
                                      char *pcFilename,
                                      tfnCallback pfnCallback);
extern void AbortBOOTPUpdate(void);

#endif // __BOOTP_SERVER_H__
