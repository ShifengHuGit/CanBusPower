//*****************************************************************************
//
// packet_handler.h
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
#ifndef __PACKET_HANDLER_H__
#define __PACKET_HANDLER_H__

#define COMMAND_PING                0x20
#define COMMAND_DOWNLOAD            0x21
#define COMMAND_RUN                 0x22
#define COMMAND_GET_STATUS          0x23
#define COMMAND_SEND_DATA           0x24
#define COMMAND_RESET               0x25

#define COMMAND_RET_SUCCESS         0x40
#define COMMAND_RET_UNKNOWN_CMD     0x41
#define COMMAND_RET_INVALID_CMD     0x42
#define COMMAND_RET_INVALID_ADDR    0x43
#define COMMAND_RET_FLASH_FAIL      0x44
#define COMMAND_ACK                 0xcc
#define COMMAND_NAK                 0x33

int AckPacket(void);
int NakPacket(void);
int GetPacket(unsigned char *pucData, unsigned char *pucSize);
int SendPacket(unsigned char *pucData, unsigned char ucSize,
               unsigned long bAck);

#endif // __PACKET_HANDLER_H__
