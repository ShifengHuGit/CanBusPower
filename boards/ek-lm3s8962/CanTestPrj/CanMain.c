//*****************************************************************************
//
// qs_ek-lm3s8962.c - The quick start application for the LM3S8962 Evaluation
//                    Board.
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

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/can.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
//#include "can_net.h"

#define CANBAUD_500K            2
tCANBitClkParms CANBitClkSettings[] =
{
    {9,6,4,4},  // CANBAUD_125K
    {5,2,2,4},  // CANBAUD_250K
    {5,2,2,2},  // CANBAUD_500K
    {5,2,2,1}   // CANBAUD_1M
};

tBoolean F_led = true;
//****************************************************************************
//
// This is the message identifier to use for sending data in the form of
// commmands or command responses.
//
//****************************************************************************
#define MSGOBJ_ID_DATA_0        0x20

//****************************************************************************
//
// This is the message identifier to use for receiving data in the form of
// commmands or command responses.
//
//****************************************************************************
#define MSGOBJ_ID_DATA_1        0x21
#define MSGOBJ_ID_TEST        0x01

//*****************************************************************************
//
// The index into CANBitClkSettings to use to get the 125K baud clock
// settings.
//
//*****************************************************************************
#define CANBAUD_125K            0

#define MSGOBJ_NUM_DATA_TX      3
#define MSGOBJ_NUM_DATA_RX      4
#define MSGOBJ_ID_DATA_TX       (MSGOBJ_ID_DATA_0)
tCANMsgObject CanTestData;
//*****************************************************************************
//
// A set of flags used to track the state of the application.
//
//*****************************************************************************
unsigned long g_ulFlags;

//*****************************************************************************
//
// The speed of the processor clock, which is therefore the speed of the clock
// that is fed to the peripherals.
//
//*****************************************************************************
unsigned long g_ulSystemClock;

//*****************************************************************************
//
// Storage for a local frame buffer.
//
//*****************************************************************************
unsigned char g_pucFrame[6144];




//*****************************************************************************
//
// The debounced state of the five push buttons.  The bit positions correspond
// to:
//
//     0 - Up
//     1 - Down
//     2 - Left
//     3 - Right
//     4 - Select
//
//*****************************************************************************
unsigned char g_ucSwitches = 0x1f;
void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        //UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
        UARTCharPut(UART1_BASE, *pucBuffer++);
    }
}

//*****************************************************************************
//
// The vertical counter used to debounce the push buttons.  The bit positions
// are the same as g_ucSwitches.
//
//*****************************************************************************
void
CANHandler(void)
{
    unsigned long ulLoop;
    unsigned long ulStatus;

  // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
  // for(ulLoop = 0; ulLoop < 30000; ulLoop++)
  // {
  // }
  // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);
  //        for(ulLoop = 0; ulLoop < 1000; ulLoop++)
  // {
  // }
    F_led =!F_led;
    UARTSend((unsigned char *)"---CAN---!", 10);
    ulStatus = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE); 
    g_ulFlags = ulStatus;       
    CANIntClear(CAN0_BASE, ulStatus);
}

void
CANConfigure(void)
{
    //
    // Configure CAN Pins
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeCAN(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Enable the CAN controllers.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    //
    // Reset the state of all the message object and the state of the CAN
    // module to a known state.
    //
    CANInit(CAN0_BASE);

    //
    // Configure the bit clock parameters for the CAN device.
    //
    CANBitTimingSet(CAN0_BASE,
                    &CANBitClkSettings[CANBAUD_500K]);

    //
    // Take the CAN1 device out of INIT state.
    //
    CANEnable(CAN0_BASE);

    //
    // Enable interrups from CAN controller.
    //
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR);

    //
    // Set up the message object that will receive all messages on the CAN
    // bus.
    //
    //CANConfigureNetwork();

    //
    // Enable interrupts for the CAN in the NVIC.
    //
    IntEnable(INT_CAN0);
}
//*****************************************************************************
//
// The main code for the application.  It sets up the peripherals, displays the
// splash screens, and then manages the interaction between the game and the
// screen saver.
//
//*****************************************************************************
int
main(void)
{
    volatile unsigned long ulLoop;
    volatile unsigned char TempData[] = {0x01,0x3E,0x80,0x55,0x55,0x55,0x55,0x55};
    //volatile  unsigned char TempData[] = {0x55,0x55,0x55,0x55,0x55,0x56,0x55,0x54};

    //
    // If running on Rev A2 silicon, turn the LDO voltage up to 2.75V.  This is
    // a workaround to allow the PLL to operate reliably.
    //
    if(REVISION_IS_A2)
    {
        SysCtlLDOSet(SYSCTL_LDO_2_75V);
    }

    //
    // Set the clocking to run directly from the PLL at 25MHz.
    //
     SysCtlClockSet(SYSCTL_SYSDIV_8 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
           SYSCTL_XTAL_8MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);

//UART 
SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
                    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                     UART_CONFIG_PAR_NONE));
UARTFIFODisable(UART1_BASE); 
 UARTSend((unsigned char *)"-------Welcome!---C-----", 24);

    //
    // Initialize the CAN controller.
    //
    CANConfigure();

    IntMasterEnable();

    CanTestData.ulMsgID = 0x708;
    CanTestData.pucMsgData = (unsigned char *)TempData;
    CanTestData.ulMsgLen = 8;
    CanTestData.ulFlags = MSG_OBJ_TX_INT_ENABLE;
    CANMessageSet(CAN0_BASE, MSGOBJ_NUM_DATA_TX, &CanTestData,
                          MSG_OBJ_TYPE_TX);


    //
    // Loop forever.
    //
    while(1)
    {
     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, F_led);
 //    for(ulLoop = 0; ulLoop < 3000000; ulLoop++)
 //    {
 //    }
 // //  if( cansentCount < 33)
 // //    {
 // //      CanTestData.ulMsgID = 0x12;
 // //      CANMessageSet(CAN0_BASE, MSGOBJ_NUM_DATA_TX, &CanTestData,
 // //                    MSG_OBJ_TYPE_TX);
 // //  }
 //    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);
 //           for(ulLoop = 0; ulLoop < 1000000; ulLoop++)
 //    {
 //    }
        //cansentCount=+1;
    }
}
