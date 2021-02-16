//*****************************************************************************
//
// can_net.c - This is the portion of the quick start application for CAN.
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

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/can.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "can_device_qs/can_common.h"
#include "driverlib/uart.h"
#include "audio.h"
#include "can_net.h"


#define MSGOBJ_NUM_BUTTON           1
#define MSGOBJ_NUM_LED              2
#define MSGOBJ_NUM_DATA_TX          3
#define MSGOBJ_NUM_DATA_RX          4
#define MSGOBJ_ENG_STATUS           5
#define MSGOBJ_ENG_RPM              6
#define MSGOBJ_ENG_WORKLOAD         7
#define MSGOBJ_ENG_MILE             8
#define MSGOBJ_ENG_TURBO            9
#define MSGOBJ_ENG_OILLEVEL         10
#define MSGOBJ_ENG_THROLLTEPOSITION 11
#define MSGOBJ_ENG_COOLTEMP         12
#define MSGOBJ_GBX_SHIFTPOSITION    13
#define MSGOBJ_GPS_ATT              14
#define MSGOBJ_GPS_LONG             15
#define MSGOBJ_GPS_SETLNUM          16
#define MSGOBJ_GPS_SETLTRACKED      17
#define MSGOBJ_AIR_INSIDETEMP       18
#define MSGOBJ_SES_CTRL             31


#define CANMSGID_ECU_ENG               0x07E8
#define CANMSGID_TESTER_ENG            0x07E0
#define CANMSGID_ECU_GEARBOX              0x07E9
#define CANMSGID_TESTER_GEARBOX           0x07E1
#define CANMSGID_ECU_SESSION             0x077A
#define CANMSGID_TESTER_SESSION          0x0710
#define CANMSGID_TESTER_INSIDETEMP          0x0746
#define CANMSGID_ECU_INSIDETEMP          0x07B0


#define FLAG_RX_ENG_STATUS                 0x00000010
#define FLAG_RX_ENG_RPM                    0x00000020
#define FLAG_RX_ENG_WORKLOAD               0x00000040
#define FLAG_RX_ENG_MILE                   0x00000080
#define FLAG_RX_ENG_TURBO                  0x00000100
#define FLAG_RX_ENG_OILLEVEL               0x00000200
#define FLAG_RX_ENG_THROLLTEPOSITION       0x00000400
#define FLAG_RX_ENG_COOLTEMP               0x00000800
#define FLAG_RX_GBX_SHIFTPOSITION          0x00001000
#define FLAG_RX_GPS_ATT                    0x00002000
#define FLAG_RX_GPS_LONG                   0x00004000
#define FLAG_RX_GPS_SETLNUM                0x00008000
#define FLAG_RX_GPS_SETLTRACKED            0x00010000
#define FLAG_RX_AIR_INSIDETEMP             0x00020000

#define FLAG_RX_SES_CTRL                   0x00040000

//*****************************************************************************
//
// This holds the information for the data receive message object that is used
// to receive commands.
//
//*****************************************************************************
tCANMsgObject g_MsgObjectRx,CanRxInsideTemp;
tCANMsgObject CanMsg_SessionCtl_TX;
tCANMsgObject CanMsg_SessionCtl_RX;
//*****************************************************************************
//
// This holds the information for the data send message object that is used
// to send commands and to send command responses.
//
//*****************************************************************************
tCANMsgObject g_MsgObjectTx;

//*****************************************************************************
//
// This holds the information for the LED message object that is used
// to transmit updates for the LED.  This message object transmits a single
// byte that indicates the brightness level for the LED on the target board.
//
//*****************************************************************************
tCANMsgObject g_MsgObjectLED;

//*****************************************************************************
//
// This holds the information for the button receive message object.  It is
// used to receive messages from the target board when button press and
// release events occur.  There are two buttons and two events(press/release).
//
//*****************************************************************************
tCANMsgObject g_MsgObjectButton;

//*****************************************************************************
//
// This is the message identifier used to transmit data to the host
// application board. The host application must use the message identifier
// specified by MSGOBJ_ID_DATA_0 to receive data successfully.
//
//*****************************************************************************
#define MSGOBJ_ID_DATA_TX       (MSGOBJ_ID_DATA_0)

//*****************************************************************************
//
// This is the message identifier used to receive data from the host
// application board. The host application must use the message identifier
// specified by MSGOBJ_ID_DATA_1 to transmit data successfully.
//
//*****************************************************************************
#define MSGOBJ_ID_DATA_RX       (MSGOBJ_ID_DATA_1)

//****************************************************************************
//
// This is the message object number used by the Button message object.
//
//*****************************************************************************
#define MSGOBJ_NUM_BUTTON       1

//*****************************************************************************
//
// This is the message object number used by the LED message object.
//
//*****************************************************************************
#define MSGOBJ_NUM_LED          2

//*****************************************************************************
//
// This is the message object number used to transfer data.
//
//*****************************************************************************
#define MSGOBJ_NUM_DATA_TX      3

//*****************************************************************************
//
// This is the message object number used to receive data.
//
//*****************************************************************************
#define MSGOBJ_NUM_DATA_RX      4

//*****************************************************************************
//
// This flag is used by the g_ulFlags global variable to indicate that a
// request to update the LED brightness is being transmitted.  This flag will
// be cleared once the message has been sent.
//
//*****************************************************************************
#define FLAG_LED_TX_PEND        0x00000002

//*****************************************************************************
//
// This flag is used by the g_ulFlags global variable to indicate that a
// data transmission is in process and that no further commands or responses
// can be sent until this flag is cleared.  This flag will be cleared by the
// interrupt handler when the tramission has completed.
//
//*****************************************************************************
#define FLAG_DATA_TX_PEND       0x00000004

//*****************************************************************************
//
// This flag is used by the g_ulFlags global variable to indicate that data
// has been received and ready to be read.  The data  may either be a command
// or response to a command.  This flag will be cleared once the data has
// been processed.
//
//*****************************************************************************
#define FLAG_DATA_RECV          0x00000008

//*****************************************************************************
//
// This global holds the flags used to indicate the state of the message
// objects.
//
//*****************************************************************************
static volatile unsigned long g_ulFlags=0;

//*****************************************************************************
//
// This holds the constant that holds the firmware version for this
// application.
//
//*****************************************************************************
unsigned long const g_ulVersion = CURRENT_VERSION;

//*****************************************************************************
//
// This global is used by the button message object to store the events that
// are coming back from the target board.
//
//*****************************************************************************
static unsigned char g_pucButtonMsg[2];

//*****************************************************************************
//
// This value holds the current LED brightness level.
//
//*****************************************************************************
unsigned char g_ucLEDLevel=0;


void Syswait_ms(int Count)
{
   SysCtlDelay( (SysCtlClockGet() / 1000)*Count);
}



//UART Send to COnsole
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
// This function handles connection with the other CAN device and also
// handles incoming commands.
//
// /return None.
//
//*****************************************************************************
void
CANMain(void)
{
    unsigned char pucData[8];

    //
    // The data has been received.
    //
    if((g_ulFlags & FLAG_DATA_RECV) == 0)
    {
        return;
    }

    //
    // Read the data from the message object.
    //
    g_MsgObjectRx.pucMsgData = pucData;
    g_MsgObjectRx.ulMsgLen = 8;
    CANMessageGet(CAN0_BASE, MSGOBJ_NUM_DATA_RX, &g_MsgObjectRx, 1);

    //
    // Indicate that the data has been read.
    //
    g_ulFlags &= (~FLAG_DATA_RECV);

    switch(g_MsgObjectRx.pucMsgData[0])
    {
        case CMD_GET_VERSION:
        {
            //
            // Send the Version.
            //
            g_ulFlags |= FLAG_DATA_TX_PEND;

            g_MsgObjectTx.pucMsgData = (unsigned char *)&g_ulVersion;
            g_MsgObjectTx.ulMsgLen = 4;
            CANMessageSet(CAN0_BASE, MSGOBJ_NUM_DATA_TX, &g_MsgObjectTx,
                          MSG_OBJ_TYPE_TX);
        }
    }

    //
    // Clear the flag.
    //
    g_ulFlags &= ~(FLAG_DATA_RECV);
}

void
Delay(unsigned long ulCount)
{
    //
    // Loop while there are more clock ticks to wait for.
    //
    while(ulCount--)
    {
        //
        // Wait until a SysTick interrupt has occurred.
        //
        while(!HWREGBITW(&g_ulFlags, 0))
        {
        }

        //
        // Clear the SysTick interrupt flag.
        //
        HWREGBITW(&g_ulFlags, 0) = 0;
    }
}

//*****************************************************************************
//
// Handle any interrupts.
//
//*****************************************************************************
void
ProcessInterrupts(void)
{

    unsigned char SessionFrameTX[8] = {0x06, 0x50, 0x03, 0x00, 0x32,0x01,0xf4,0xaa};
    unsigned char CMD_TEMP_FrameTX[8] = {0x05, 0x62, 0x26, 0x13, 0x00, 0xd4, 0xaa, 0xaa};
    //
    // A request to set or clear the LED was received.
    //
    if(g_ulFlags == FLAG_RX_SES_CTRL)
    {
        //
        // Turn the LED on or off based on the request.
        //
    UARTSend("Sent:",5);
    //Delay(4);
    //Syswait_ms(1);
    CanMsg_SessionCtl_TX.pucMsgData = SessionFrameTX;
    //CanMsg_SessionCtl_TX.ulMsgLen = 8;
    CANMessageSet(CAN0_BASE, MSGOBJ_NUM_DATA_TX, &CanMsg_SessionCtl_TX,
                          MSG_OBJ_TYPE_TX);
   // UARTSend(SessionFrameTX,8);
        //
        // Clear the flag.
        //
        g_ulFlags &= ~(FLAG_RX_SES_CTRL);
         g_ulFlags = 0 ;
    }
    if(g_ulFlags == FLAG_RX_AIR_INSIDETEMP)
    {
        UARTSend("SentTemp:",9);
        CanMsg_SessionCtl_TX.ulMsgID = CANMSGID_ECU_INSIDETEMP;
        CanMsg_SessionCtl_TX.pucMsgData = CMD_TEMP_FrameTX;
        CANMessageSet(CAN0_BASE, MSGOBJ_NUM_DATA_TX, &CanMsg_SessionCtl_TX,
                              MSG_OBJ_TYPE_TX);
            //
            // Clear the flag.
            //
            g_ulFlags &= ~(FLAG_RX_AIR_INSIDETEMP);
             g_ulFlags = 0 ;
    }
}


void
CANConfigureNetwork(void)
{


  // g_MsgObjectTx.ulMsgID = 0x17;
  // g_MsgObjectTx.ulMsgIDMask = 0;
  // g_MsgObjectTx.ulFlags = MSG_OBJ_TX_INT_ENABLE;
  // g_MsgObjectTx.ulMsgLen = 8;
  // g_MsgObjectTx.pucMsgData = (unsigned char *)0xffffffff;

  //  CANMessageSet(CAN0_BASE, MSGOBJ_NUM_DATA_TX, &g_MsgObjectTx,
  //            MSG_OBJ_TYPE_RX);


 //
 // This message object will received data from commands.
 //
    g_MsgObjectRx.ulMsgID = MSGOBJ_ID_DATA_RX;
    g_MsgObjectRx.ulMsgIDMask = 0;
    g_MsgObjectRx.ulFlags = MSG_OBJ_RX_INT_ENABLE;
    g_MsgObjectRx.ulMsgLen = 1;
    g_MsgObjectRx.pucMsgData = (unsigned char *)0xffffffff;
    CANMessageSet(CAN0_BASE, MSGOBJ_NUM_DATA_RX, &g_MsgObjectRx,
                  MSG_OBJ_TYPE_RX);


    CanMsg_SessionCtl_TX.ulMsgID = CANMSGID_ECU_SESSION;
    CanMsg_SessionCtl_TX.ulMsgIDMask = 0;
    CanMsg_SessionCtl_TX.ulFlags = MSG_OBJ_TX_INT_ENABLE;
    // 06-|50 03 00 32 01 f4 aa
    CanMsg_SessionCtl_TX.ulMsgLen = 8;
    CanMsg_SessionCtl_TX.pucMsgData = (unsigned char *)0xffffffff;

    //
    // This message object will received data from commands.
    //
    CanMsg_SessionCtl_RX.ulMsgID = CANMSGID_TESTER_SESSION;
    CanMsg_SessionCtl_RX.ulMsgIDMask = 0x7FF;
    CanMsg_SessionCtl_RX.ulFlags = MSG_OBJ_RX_INT_ENABLE|MSG_OBJ_USE_ID_FILTER;;
    CanMsg_SessionCtl_RX.ulMsgLen = 8;
    CanMsg_SessionCtl_RX.pucMsgData = (unsigned char *)0xffffffff;
    CANMessageSet(CAN0_BASE, MSGOBJ_SES_CTRL, &CanMsg_SessionCtl_RX,
                  MSG_OBJ_TYPE_RX);



    //
    // This message object will received data from Client for Temp.
    //
    CanRxInsideTemp.ulMsgID = CANMSGID_TESTER_INSIDETEMP;
    CanRxInsideTemp.ulMsgIDMask = 0x7FF;
    CanRxInsideTemp.ulFlags = MSG_OBJ_RX_INT_ENABLE|MSG_OBJ_USE_ID_FILTER;;
    CanRxInsideTemp.ulMsgLen = 8;
    CanRxInsideTemp.pucMsgData = (unsigned char *)0xffffffff;
    CANMessageSet(CAN0_BASE, MSGOBJ_AIR_INSIDETEMP, &CanRxInsideTemp,
                  MSG_OBJ_TYPE_RX);

                  
}



//*****************************************************************************
//
// The CAN controller Interrupt handler.
//
//*****************************************************************************
void
CANHandler(void)
{
    unsigned long ulStatus;
     unsigned char pucData[8];
     //【 06-|50 03 00 32 01 f4 aa |
    

    //
    // Find the cause of the interrupt, if it is a status interrupt then just
    // acknowledge the interrupt by reading the status register.
    //
    ulStatus = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    UARTCharPut(UART1_BASE, 'C');
    UARTCharPut(UART1_BASE, 'A');
    UARTCharPut(UART1_BASE, 'N');
    UARTCharPut(UART1_BASE, '-');
    UARTCharPut(UART1_BASE, (unsigned char )ulStatus);

    
    //CANMessageClear(CAN0_BASE, MSGOBJ_SES_CTRL);


    switch(ulStatus)
    {


        //
        // When the transmit data message object interrupts, clear the
        // flag so that more data can be trasferred.
        //
        case MSGOBJ_NUM_DATA_TX:
        {
            g_ulFlags &= (~FLAG_DATA_TX_PEND);
           // g_MsgObjectRx.pucMsgData = pucData;
           // g_MsgObjectRx.ulMsgLen = 8;
           // CANMessageGet(CAN0_BASE, MSGOBJ_NUM_DATA_TX, &g_MsgObjectRx, 8);
           // UARTSend("Itself's Package",16);
           // UARTSend(pucData, 8);
            
            break;
        }

        //
        // When a receive data message object interrupts, set the flag to
        // indicate that new data is ready.
        //
        case MSGOBJ_NUM_DATA_RX:
        {
            g_ulFlags |= FLAG_DATA_RECV;
            break;
        }
         case MSGOBJ_SES_CTRL:
        {
             g_ulFlags = FLAG_RX_SES_CTRL;
             CanMsg_SessionCtl_RX.pucMsgData = pucData;
             CanMsg_SessionCtl_RX.ulMsgLen = 8;
             CANMessageGet(CAN0_BASE, MSGOBJ_SES_CTRL, &CanMsg_SessionCtl_RX, 8);
             UARTSend(pucData, 8);
            break;
        }   
         case MSGOBJ_AIR_INSIDETEMP:
        {
             g_ulFlags = FLAG_RX_AIR_INSIDETEMP;
             CanRxInsideTemp.pucMsgData = pucData;
             CanRxInsideTemp.ulMsgLen = 8;
             CANMessageGet(CAN0_BASE, ulStatus, &CanRxInsideTemp, 8);
             UARTSend(pucData, 8);
            break;
        }   
        //
        // This was a status interrupt so read the current status to
        // clear the interrupt and return.
        //
        default:
        {
            //
            // Read the controller status to acknowledge this interrupt.
            //
            CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

            //
            // If there was a LED transmission pending, then stop it and
            // clear the flag.
            //
            if(g_ulFlags & FLAG_LED_TX_PEND)
            {
                //
                // Disable this message object until we retry it later.
                //
                CANMessageClear(CAN0_BASE, MSGOBJ_NUM_LED);

                //
                // Clear the transmit pending flag.
                //
                g_ulFlags &= (~FLAG_LED_TX_PEND);
            }

            //
            // If there was a Data transmission pending, then stop it and
            // clear the flag.
            //
            if(g_ulFlags & FLAG_DATA_TX_PEND)
            {
                //
                // Disable this message object until we retry it later.
                //
                CANMessageClear(CAN0_BASE, MSGOBJ_NUM_DATA_TX);

                //
                // Clear the transmit pending flag.
                //
                g_ulFlags &= (~FLAG_DATA_TX_PEND);
            }
            return;
        }
    }

    //
    // Acknowledge the CAN controller interrupt has been handled.
    //
    CANIntClear(CAN0_BASE, ulStatus);
}

//*****************************************************************************
//
// This function configures the CAN hardware and the message objects so that
// they are ready to use once the application returns from this function.
//
//*****************************************************************************
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
                    (tCANBitClkParms *)&CANBitClkSettings[CANBAUD_500K]);

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
    CANConfigureNetwork();

    //
    // Enable interrupts for the CAN in the NVIC.
    //
    IntEnable(INT_CAN0);
}
