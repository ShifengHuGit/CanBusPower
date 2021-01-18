//*****************************************************************************
//
// sounds.h - Prototypes for the music/sound effect arrays.
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
// This is part of revision 4423 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

#ifndef __SOUNDS_H__
#define __SOUNDS_H__

//*****************************************************************************
//
// Declarations for the song and sound effect arrays.
//
//*****************************************************************************
extern const unsigned short g_pusIntro[80];
extern const unsigned short g_pusStartOfGame[24];
extern const unsigned short g_pusEndOfMaze[28];
extern const unsigned short g_pusEndOfGame[36];
extern const unsigned short g_pusFireEffect[30];
extern const unsigned short g_pusWallEffect[100];
extern const unsigned short g_pusMonsterEffect[100];
extern const unsigned short g_pusPlayerEffect[150];

#endif // __SOUNDS_H__
