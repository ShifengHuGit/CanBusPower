//*****************************************************************************
//
// images.h - Prototypes for the image arrays.
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

#ifndef __IMAGES_H__
#define __IMAGES_H__

//*****************************************************************************
//
// Declarations for the image and animation arrays.
//
//*****************************************************************************
extern const unsigned char g_pucLMILogo[2520];
extern const unsigned char g_pucCodeRedLogo[2048];
extern const unsigned char g_pucIarLogo[3111];
extern const unsigned char g_pucCodeSourceryLogo[2176];
extern const unsigned char g_pucKeilLogo[2560];
extern const unsigned char g_pucPlay[1024];
extern const unsigned char g_pucGameOver[481];
extern const unsigned char g_pucScore[286];
extern const unsigned char g_pucAcquiring[832];
extern const unsigned char g_pucIP[117];
extern const unsigned char g_pucDot[26];
extern const unsigned char g_ppucDigits[10][52];
extern const unsigned char g_ppucSprites[56][72];
extern const unsigned char g_pucPlayerStandingDown[14];
extern const unsigned char g_pucPlayerWalkingDown[10];
extern const unsigned char g_pucPlayerStandingUp[6];
extern const unsigned char g_pucPlayerWalkingUp[10];
extern const unsigned char g_pucPlayerStandingLeft[6];
extern const unsigned char g_pucPlayerWalkingLeft[6];
extern const unsigned char g_pucPlayerStandingRight[6];
extern const unsigned char g_pucPlayerWalkingRight[6];
extern const unsigned char g_pucPlayerDying[34];
extern const unsigned char g_pucMonster[10];
extern const unsigned char g_pucExplosion[12];

#endif // __IMAGES_H__
