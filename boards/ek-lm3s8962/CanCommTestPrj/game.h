//*****************************************************************************
//
// game.h - Prototypes for the game.
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
// This is part of revision 4423 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

#ifndef __GAME_H__
#define __GAME_H__

//*****************************************************************************
//
// Declarations for the game state variables.
//
//*****************************************************************************
extern unsigned long g_ulScore;
extern unsigned short g_usPlayerX;
extern unsigned short g_usPlayerY;
extern unsigned short g_pusMonsterX[100];
extern unsigned short g_pusMonsterY[100];
extern char g_ppcMaze[94][127];

//*****************************************************************************
//
// Prototypes for the game.
//
//*****************************************************************************
extern tBoolean MainScreen(void);
extern void PlayGame(void);

#endif // __GAME_H__
