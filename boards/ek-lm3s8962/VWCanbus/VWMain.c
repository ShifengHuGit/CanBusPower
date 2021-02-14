  #include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/can.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
//#include "can_common.h"
#include "can.h"
#include "driverlib/uart.h"
#include "19264LCDdriver.h"
#include "utils/ustdlib.h"

tCANBitClkParms CANBitClkSettings[] =
{
    {9,6,4,4},  // CANBAUD_125K
    {5,2,2,4},  // CANBAUD_250K
    {5,2,2,2},  // CANBAUD_500K
    {5,2,2,1}   // CANBAUD_1M
};
#define CANBAUD_500K            2

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



#define FLAG_BUTTON_PEND        0x00000001
#define FLAG_UPDATE_LED         0x00000002
#define FLAG_DATA_TX_PEND       0x00000004
#define FLAG_DATA_RECV          0x00000008


#define MSG_OBJ_B0_PRESSED      0x01
#define MSG_OBJ_B0_RELEASED     0x02
#define MSG_OBJ_B1_PRESSED      0x04
#define MSG_OBJ_B1_RELEASED     0x08

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





#define CANMSGID_ECU_ENG               0x07E8
#define CANMSGID_TESTER_ENG            0x07E0
#define CANMSGID_ECU_GEARBOX              0x07E9
#define CANMSGID_TESTER_GEARBOX           0x07E1
#define CANMSGID_ECU_SESSION             0x077A
#define CANMSGID_TESTER_SESSION          0x0710



//***********************************************************************
//
// Volkswagen ECU 0x20 opreation CODE
//
//***********************************************************************


#define RPM_CODE                        0x0000F40C
#define ENGINE_STATUS_CODE              0x00003953
#define ENGINE_WORKLOAD_PRECENTAGE      0x000011E9
#define SHIFT_GEAR_POSITION             0x00003808
#define TURBO_PRESSURE                  0x00002029
#define THROTTLE_PRECENTAGE             0x0000F411


tCANMsgObject CanMsg_ENG_STATUS;             
tCANMsgObject CanMsg_ENG_RPM  ;          
tCANMsgObject CanMsg_ENG_WORKLOAD ;      
tCANMsgObject CanMsg_ENG_MILE  ;         
tCANMsgObject CanMsg_ENG_TURBO   ;        
tCANMsgObject CanMsg_ENG_OILLEVEL ;       
tCANMsgObject CanMsg_ENG_THROLLTEPOSITION;
tCANMsgObject CanMsg_ENG_COOLTEMP  ;      
tCANMsgObject CanMsg_GBX_SHIFTPOSITION ;  
tCANMsgObject CanMsg_SessionCtl;
tCANMsgObject CanMsg_SessionCtl_TX;
tCANMsgObject CanMsg_SessionCtl_RX;


unsigned char rpmRawDataRX[8];
unsigned char rpmRawDataTX[8];

//*****************************************************************************
//
// This holds the information for the data receive message object that is used
// to receive commands.
//
//*****************************************************************************
tCANMsgObject g_MsgObjectRx;
tCANMsgObject CanRx;

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
// to receive updates for the LED.  This message object receives a single
// byte that indicates the brightness level for the LED.
//
//*****************************************************************************
tCANMsgObject g_MsgObjectLED;

//*****************************************************************************
//
// This holds the information for the button request message object.  It is
// used to transmit the current status of the buttons on the target board.
// It does this by sending a single byte containing the bitmask of the
// buttons.
//
//*****************************************************************************
tCANMsgObject g_MsgObjectButton;

//*****************************************************************************
//
// The counter of the number of consecutive times that the buttons have
// remained constant.
//
//*****************************************************************************
static unsigned long g_ulDebounceCounter;

//*****************************************************************************
//
// The counter value used to turn off the led after receiving a command to
// pulse the LED.
//
//*****************************************************************************
static long g_lFlashCounter = -1;

//*****************************************************************************
//
// This variable holds the last stable raw button status.
//
//*****************************************************************************
volatile unsigned char g_ucButtonStatus;

//*****************************************************************************
//
// This variable holds flags indicating which buttons have been pressed and
// released.
//
//*****************************************************************************
volatile unsigned char g_ucButtonFlags = 0;

//*****************************************************************************
//
// This used to hold the message data for the button message object.
//
//*****************************************************************************
unsigned char g_pucButtonMsg[2];

//*****************************************************************************
//
// This value holds the current LED brightness level.
//
//*****************************************************************************
unsigned char g_ucLEDLevel = 0;

//*****************************************************************************
//
// This holds the constant that holds the firmware version for this
// application.
//
//*****************************************************************************
//static unsigned long const g_ulVersion = CURRENT_VERSION;

//*****************************************************************************
//
// This global holds the flags used to indicate the state of the message
// objects.
//
//*****************************************************************************
static volatile unsigned long g_ulFlags = 0;



//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************

//*****************************************************************************
//
// This is the interrupt handler for the SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    unsigned long ulStatus;
    unsigned char ucTemp;
    static unsigned long ulLastStatus = 0;

    //
    // Read the current value of the button pins.
    //
    ulStatus = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    if(ulStatus != ulLastStatus)
    {
        //
        // Something changed reset the compare and reset the debounce counter.
        //
        ulLastStatus = ulStatus;
        g_ulDebounceCounter = 0;
    }
    else
    {
        //
        // Increment the number of counts with the push button in a different
        // state.
        //
        g_ulDebounceCounter++;

        //
        // If four consecutive counts have had the push button in a different
        // state then the state has changed.
        //
        if(g_ulDebounceCounter == 4)
        {
            //
            // XOR to see what has changed.
            //
            ucTemp = g_ucButtonStatus ^ ulStatus;

            //
            // If something changed on button 0 then see what changed.
            //
            if(ucTemp & GPIO_PIN_0)
            {
                if(GPIO_PIN_0 & ulStatus)
                {
                    //
                    // Button released.
                    //
                    g_ucButtonFlags |= MSG_OBJ_B0_RELEASED;
                }
                else
                {
                    //
                    // Button pressed.
                    //
                    g_ucButtonFlags |= MSG_OBJ_B0_PRESSED;
                }
            }

            //
            // If something changed on button 1 then see what changed.
            //
            if(ucTemp & GPIO_PIN_1)
            {
                if(GPIO_PIN_1 & ulStatus)
                {
                    //
                    // Button released.
                    //
                    g_ucButtonFlags |= MSG_OBJ_B1_RELEASED;
                }
                else
                {
                    //
                    // Button pressed.
                    //
                    g_ucButtonFlags |= MSG_OBJ_B1_PRESSED;
                }
            }

            //
            // Save the new stable state for comparison.
            //
            g_ucButtonStatus = (unsigned char)ulStatus;
        }
    }

    //
    // Clear the LED if it is time.
    //
    if(g_lFlashCounter == 0)
    {
        //
        // Turn off LED.
        //
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    }

    //
    // Let this go below just below zero so that the LED is not repeatedly
    // turned off.
    //
    if(g_lFlashCounter >= 0)
    {
        g_lFlashCounter--;
    }
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
        UARTCharPut(UART0_BASE, *pucBuffer++);
    }
}

// CAN Interrupt Function
void
CANHandler(void)
{
    unsigned long ulStatus;
    //UARTSend((unsigned char *)"CAN Interrupt Triggered", 23);
    //
    // Find the cause of the interrupt, if it is a status interrupt then just
    // acknowledge the interrupt by reading the status register.
    //
    ulStatus = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE); 
    g_ulFlags = ulStatus;
    UARTSend("Can StatusID:", 13);
    UARTCharPut(UART0_BASE, (unsigned char )ulStatus);
/*----- Comment out Switch 
    switch(ulStatus)
    {
        //
        // Let the forground loop handle sending this, just set a flag to
        // indicate that the data should be sent.
        //
        case MSGOBJ_SES_CTRL:
        {
            //
            // Indicate a pending button transmission is complete.
            //
            g_ulFlags &= (~FLAG_RX_SES_CTRL);

            break;
        }
        case MSGOBJ_NUM_BUTTON:
        {
            //
            // Indicate a pending button transmission is complete.
            //
            g_ulFlags &= (~FLAG_BUTTON_PEND);

            break;
        }

        case MSGOBJ_NUM_LED:
        {
            //
            // Read the new LED level and let the foreground handle it.
            //
            CANMessageGet(CAN0_BASE, MSGOBJ_NUM_LED, &g_MsgObjectLED, 1);

            //
            // Limit the LED Level to MAX_LED_BRIGHTNESS.
            //
            if((g_ucLEDLevel & LED_FLASH_VALUE_MASK) > MAX_LED_BRIGHTNESS)
            {
                g_ucLEDLevel = MAX_LED_BRIGHTNESS |
                               (g_ucLEDLevel & LED_FLASH_ONCE);
            }

            //
            // Indicate that the LED needs to be updated.
            //
            g_ulFlags |= FLAG_UPDATE_LED;

            break;
        }

        //
        // The data transmit message object has been sent successfully.
        //
        case MSGOBJ_NUM_DATA_TX:
        {
            //
            // Clear the data transmit pending flag.
            //
            g_ulFlags &= (~FLAG_DATA_TX_PEND);
            break;
        }

        //
        // The data receive message object has received some data.
        //
        case MSGOBJ_NUM_DATA_RX:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_DATA_RECV;
            break;
        }
        case MSGOBJ_ENG_STATUS:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_STATUS;
            break;
        }
        case MSGOBJ_ENG_RPM:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_RPM;
            break;
        }
        case MSGOBJ_ENG_WORKLOAD:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_WORKLOAD;
            break;
        }
        case MSGOBJ_ENG_MILE:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_MILE;
            break;
        }
        case MSGOBJ_ENG_TURBO:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_TURBO;
            break;
        }
        case MSGOBJ_ENG_OILLEVEL:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_OILLEVEL;
            break;
        }
        case MSGOBJ_ENG_THROLLTEPOSITION:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_THROLLTEPOSITION;
            break;
        }
        case MSGOBJ_ENG_COOLTEMP:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_COOLTEMP;
            break;
        }
        case MSGOBJ_GBX_SHIFTPOSITION:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_GBX_SHIFTPOSITION;
            break;
        }
        case MSGOBJ_GPS_ATT:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_GPS_ATT;
            break;
        }
        case MSGOBJ_GPS_LONG:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_GPS_LONG;
            break;
        }
        case MSGOBJ_GPS_SETLNUM:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_GPS_SETLNUM;
            break;
        }
        case MSGOBJ_GPS_SETLTRACKED:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_GPS_SETLTRACKED;
            break;
        }
        case MSGOBJ_AIR_INSIDETEMP:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_AIR_INSIDETEMP;
            break;
        }
       
        // This was a status interrupt so read the current status to
        // clear the interrupt and return.
        //
        default:
        {
            CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
            return;
        }
    }
---------*/
    //
    // Acknowledge the CAN controller interrupt has been handled.
    //
    CANIntClear(CAN0_BASE, ulStatus);
}


void FrameSend(int CanID, int SID, int FunctionCode, char * CanMsg)
{

}
void
CANConfigureNetwork(void)
{
    //
    // This is the message object used to send button updates.  This message
    // object will not be "set" right now as that would trigger a transmission.
    //
    CanMsg_SessionCtl_TX.ulMsgID = CANMSGID_TESTER_SESSION;
    CanMsg_SessionCtl_TX.ulMsgIDMask = 0;
    CanMsg_SessionCtl_TX.ulFlags = MSG_OBJ_TX_INT_ENABLE;
    CanMsg_SessionCtl_TX.ulMsgLen = 8;
    //CanMsg_SessionCtl_TX.pucMsgData = SessionFrameTX;

    CanMsg_SessionCtl_RX.ulMsgID = CANMSGID_ECU_SESSION;
    CanMsg_SessionCtl_RX.ulMsgIDMask = 0;
    CanMsg_SessionCtl_RX.ulFlags = MSG_OBJ_RX_INT_ENABLE;
    CanMsg_SessionCtl_RX.ulMsgLen = 8;
    CANMessageSet(CAN0_BASE, MSGOBJ_NUM_DATA_RX, &CanMsg_SessionCtl_RX,
                  MSG_OBJ_TYPE_RX);
    //CanMsg_SessionCtl_RX.pucMsgData = SessionFrameRX;




    CanMsg_ENG_RPM.ulMsgID = CANMSGID_ECU_ENG;
    CanMsg_ENG_RPM.ulMsgIDMask = 0;
    CanMsg_ENG_RPM.ulFlags = MSG_OBJ_RX_INT_ENABLE;
    CanMsg_ENG_RPM.ulMsgLen = 8;
    CanMsg_ENG_RPM.pucMsgData = rpmRawDataRX;

    CanMsg_ENG_RPM.ulMsgID = CANMSGID_TESTER_ENG;
    CanMsg_ENG_RPM.ulMsgIDMask = 0;
    CanMsg_ENG_RPM.ulFlags = MSG_OBJ_TX_INT_ENABLE;
    CanMsg_ENG_RPM.ulMsgLen = 8;
    CanMsg_ENG_RPM.pucMsgData = rpmRawDataTX;
    
    
    }

void
ProcessCmd(void)
{
    volatile unsigned long ulLoop;
    volatile unsigned char Keepalive[] = {0x02,0x3E,0x80,0x55,0x55,0x55,0x55,0x55};
    //Data area is fixed to 0~64bit
    unsigned char pucData[8]; 
    int Datalen =0 ;
    int i=0;
    int NoSingleFrame;
    //unsigned char RawData[16]={"abcdefghijklmnop"};
    unsigned char RespFlag; 
    unsigned char RespSIDCode, RespLength;
    unsigned long RespData = 0; 
    unsigned long FunctionCode;
    char HEX_RespFlag[2];
    char HEX_RespSIDCode[2];
    char HEX_RespData[16];
    char HEX_FunctionCode[4];
    char HEX_RespLength[2];
    char HEX_RawData[16];
    char DisplayBuf[24];
   



  switch(g_ulFlags)
    {
        //
        // Let the forground loop handle sending this, just set a flag to
        // indicate that the data should be sent.
        //
        case MSGOBJ_SES_CTRL:
        {
            //
            // Indicate a pending button transmission is complete.
            //
            g_ulFlags &= (~FLAG_RX_SES_CTRL);

            break;
        }
        case MSGOBJ_NUM_BUTTON:
        {
            //
            // Indicate a pending button transmission is complete.
            //
            g_ulFlags &= (~FLAG_BUTTON_PEND);

            break;
        }
/*----LED
        case MSGOBJ_NUM_LED:
        {
            //
            // Read the new LED level and let the foreground handle it.
            //
            CANMessageGet(CAN0_BASE, MSGOBJ_NUM_LED, &g_MsgObjectLED, 1);

            //
            // Limit the LED Level to MAX_LED_BRIGHTNESS.
            //
            if((g_ucLEDLevel & LED_FLASH_VALUE_MASK) > MAX_LED_BRIGHTNESS)
            {
                g_ucLEDLevel = MAX_LED_BRIGHTNESS |
                               (g_ucLEDLevel & LED_FLASH_ONCE);
            }

            //
            // Indicate that the LED needs to be updated.
            //
            g_ulFlags |= FLAG_UPDATE_LED;

            break;
        }
----*/
        //
        // The data transmit message object has been sent successfully.
        //
        case MSGOBJ_NUM_DATA_TX:
        {
            //
            // Clear the data transmit pending flag.
            //
            g_ulFlags &= (~FLAG_DATA_TX_PEND);
            break;
        }

        //
        // The data receive message object has received some data.
        //
        case MSGOBJ_NUM_DATA_RX:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_DATA_RECV;
            break;
        }
        case MSGOBJ_ENG_STATUS:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_STATUS;
            break;
        }
        case MSGOBJ_ENG_RPM:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_RPM;
            break;
        }
        case MSGOBJ_ENG_WORKLOAD:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_WORKLOAD;
            break;
        }
        case MSGOBJ_ENG_MILE:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_MILE;
            break;
        }
        case MSGOBJ_ENG_TURBO:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_TURBO;
            break;
        }
        case MSGOBJ_ENG_OILLEVEL:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_OILLEVEL;
            break;
        }
        case MSGOBJ_ENG_THROLLTEPOSITION:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_THROLLTEPOSITION;
            break;
        }
        case MSGOBJ_ENG_COOLTEMP:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_ENG_COOLTEMP;
            break;
        }
        case MSGOBJ_GBX_SHIFTPOSITION:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_GBX_SHIFTPOSITION;
            break;
        }
        case MSGOBJ_GPS_ATT:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_GPS_ATT;
            break;
        }
        case MSGOBJ_GPS_LONG:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_GPS_LONG;
            break;
        }
        case MSGOBJ_GPS_SETLNUM:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_GPS_SETLNUM;
            break;
        }
        case MSGOBJ_GPS_SETLTRACKED:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_GPS_SETLTRACKED;
            break;
        }
        case MSGOBJ_AIR_INSIDETEMP:
        {
            //
            // Indicate that the data message object has new data.
            //
            g_ulFlags |= FLAG_RX_AIR_INSIDETEMP;
            break;
        }
       
        // This was a status interrupt so read the current status to
        // clear the interrupt and return.
        //
        default:
        {
            CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
            return;
        }
    }



    g_MsgObjectRx.pucMsgData = pucData;
    g_MsgObjectRx.ulMsgLen = 8;
    CANMessageGet(CAN0_BASE, MSGOBJ_NUM_DATA_RX, &g_MsgObjectRx, 1);

    
    // There are a lots of traffice go through the bus, what I really dont what it is , and it's not useful for now
    // Exclude those frame by the MsgID
    // Currently I dont want to have :
    // --- 0x0007B4
    // --- 0x0007B5
    // --- 0x17F00010
    if((g_MsgObjectRx.ulMsgID != 0x0007B4 ) && (g_MsgObjectRx.ulMsgID != 0x0007B5 ) && 
    (g_MsgObjectRx.ulMsgID != 0x17F00010))
    {
        // Filter the Heartbeat frame, if it is HB frame, sub function will direct return.
        for(Datalen=0; Datalen<g_MsgObjectRx.ulMsgLen; Datalen++)
        {
            /* Also Ignore the Heartbeat Frame sent from the Tester */
            if(g_MsgObjectRx.pucMsgData[Datalen] == Keepalive[Datalen])
            {
                Datalen++;
                if (Datalen == 7)
                { 
                    //This is a Keepalive data , ignore.
                    g_ulFlags &= (~FLAG_DATA_RECV);
                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
                    return;
                }
            }
            else
            {
                break;
            }
         }   
          
        UARTSend((unsigned char *)"ID |", 4);
        UARTSend((unsigned char *)&(g_MsgObjectRx.ulMsgID), 4);
        UARTSend((unsigned char *)"--DATA |", 8);
        UARTSend((unsigned char *)g_MsgObjectRx.pucMsgData, 8);

       /*---
       USD stuff:
        [ byte0 byte1 byte2 byte3 byte4 byte5 byte6 byte7 ] 
                 |    SID : We only use 0x22 Operation, to get the ECU value by identifyID.
                 |->  0x22 :                In Request
                 |->  Value + 0x40 :        In Response : OK! Good,(our case, 0x62)
                 |->  0x7F :                In Response : FxxK, you can't have me, said by ECU and with NRC.
              
        ------*/ 
       RespSIDCode =  g_MsgObjectRx.pucMsgData[1];
       if(RespSIDCode == 0x7f  || RespSIDCode != 0x62 )
       {
            WRCommandH(0x01);
            //WRCommandH(0x20);
            ShowQQCharH(0x80, "??????SID ", 6);
           //This is rejected by ECU, ignore this frame for this time
           g_ulFlags &= (~FLAG_DATA_RECV);
           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
           return;
       }

        /*---
         USD stuff:
         [ byte0      byte1 byte2 byte3 byte4 byte5 byte6 byte7 ] 
         [ bXXXX XXXX byte1 byte2 byte3 byte4 byte5 byte6 byte7 ]
            |
            |->  Single Frame :        0x0X
            |->  FristFrame :          0x1X
            |->  Continue Frame :      0x2X
            |->  Flow Control :        0x3X
        ------*/ 
        RespFlag =  g_MsgObjectRx.pucMsgData[0] & 0xF0;
        if((RespFlag & 0x0F) >>4)
           NoSingleFrame = 1;
         /*---
          USD stuff:
          [ byte0      byte1 byte2 byte3 byte4 byte5 byte6 byte7 ] 
          [ bXXXX XXXX byte1 byte2 byte3 byte4 byte5 byte6 byte7 ] 
                   |
                   |->  Size of data inside this frame, after the 1st byte:
         ------*/ 
         //   { 0x05,0x62, 0xf4, 0x0c,0x0a,0xf9, 0xaa, 0xaa};
        RespLength =  g_MsgObjectRx.pucMsgData[0] & 0x0F;
        for(i=4; i<=RespLength; i++){
              RespData |= g_MsgObjectRx.pucMsgData[i];
              if(i != RespLength ) 
                { 
                    RespData <<=8;
                }
              else
              {
                  break;
              }
              

        }
        /*---
        USD stuff:
        [ byte0 byte1 byte2 byte3 byte4 byte5 byte6 byte7 ] 
                        |    
                        |->  Byte2 and 3 are the function code, and 
                        |->  the Data in the Response frame after Byte3.
                        |->  its length was decided by the byte0 low 4 digitals
         ------*/ 
        FunctionCode = g_MsgObjectRx.pucMsgData[2]<<8|g_MsgObjectRx.pucMsgData[3];

          /*---------------------------------------------------------------------------
              1    2    3    4    5   6    7    8     9   10   11   12
          H 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8A 0x8B
             ??   ??    : 
          H 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9A 0x9B
             FL   ag    :    X   Le   n:    X    D:  X     X
          L 0x80 0x81 0x82 0x83 0x84 0x85 0x86 0x87 0x88 0x89 0x8A 0x8B
             Fu   nc    ti   on  Co   de
          L 0x90 0x91 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9A 0x9B
          ---------------------------------------------------------------------------*/
/*
    switch(FunctionCode){
        case RPM_CODE: 
        case ENGINE_STATUS_CODE:
        case ENGINE_WORKLOAD_PRECENTAGE:
        case SHIFT_GEAR_POSITION:
        case TURBO_PRESSURE:
        case THROTTLE_PRECENTAGE:
        default: break;

    }
*/   
       

        usprintf(DisplayBuf, "Flag is:%02X, Size is:%02X", RespFlag, RespLength);
        ShowQQCharH(0x80, (unsigned char *)DisplayBuf,11);
        memset(DisplayBuf, 0, 24);
        usprintf(DisplayBuf, "Data: %X", RespData);
        ShowQQCharL(0x80, (unsigned char *)DisplayBuf,6);
        memset(DisplayBuf, 0, 24);
        usprintf(DisplayBuf, "FunctionCode: %4X", FunctionCode);
        ShowQQCharL(0x90, (unsigned char *)DisplayBuf,9);
        memset(DisplayBuf, 0, 24);

       //to_hex((char *)g_MsgObjectRx.pucMsgData,8, HEX_RawData);
       //to_hex((char *)&RespFlag,               1, HEX_RespFlag);
       //to_hex((char *)&RespLength,             1, HEX_RespLength);
       //to_hex((char *)&RespData,               (int)RespLength-3,  HEX_RespData);
       //to_hex((char *)&FunctionCode,           2, HEX_FunctionCode);
//
       // ShowQQCharH(0x80, "????: ", 3); 
        //WRCommandH(0x20);
        //ShowQQCharH(0x84, (unsigned char *)HEX_RawData,8);

        //ShowQQCharH(0x80, "Flag: ", 3);
        //WRCommandH(0x01);
        //WRCommandH(0x20);
        //ShowQQCharH(0x80, (unsigned char *)HEX_RespFlag,1);
        //ShowNUMCharH(0x80, g_MsgObjectRx.pucMsgData[0], 1 );
        //ShowAlphabet(0x80, (unsigned char *)"0x31323334", 4);
        
        //ShowQQCharH(0x90, "Len:", 2);
        //ShowQQCharH(0x90, (unsigned char *)HEX_RespLength,1);

        //ShowQQCharL(0x80, "Data: ", 3);
       ///WRCommandH(0x20);
       // ShowQQCharL(0x80, (unsigned char *)HEX_RespData,RespLength-3);
//
        //ShowQQCharL(0x90, "FunctionCode", 6);
       //WRCommandH(0x20);
        //ShowQQCharL(0x90, (unsigned char *)HEX_FunctionCode,2);
         
        //ShowQQCharH(0x93, (unsigned char *)HEX_RespFlag,1);
        //ShowQQCharH(0x96, (unsigned char *)HEX_RespLength,1);
        //ShowQQCharH(0x98, (unsigned char *)HEX_RespData,RespLength-3);
        //ShowQQCharL(0x87, (unsigned char *)HEX_FunctionCode,2);

          //UARTSend((unsigned char *)"ID |", 4);
          //UARTSend((unsigned char *)&(g_MsgObjectRx.ulMsgID), 4);
          //UARTSend((unsigned char *)"--DATA |", 8);
          //UARTSend((unsigned char *)g_MsgObjectRx.pucMsgData, 8);
          //WaitNms(200);
    }

    //
    // Clear the flag to indicate that the data has been read.
    //
    g_ulFlags &= (~FLAG_DATA_RECV);
    for(ulLoop = 0; ulLoop < 50000; ulLoop++)
    {
    }
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
}

void Init_VW(){
    //CanID-??10 07?? Can Data->?? 02-|10 03 55 55 55 55 55 |]  
    unsigned char SessionFrameTX[8] = {0x02, 0x10, 0x03, 0x55, 0x55,0x55,0x55,0x55};
    unsigned char SessionFrameRX[8];
    unsigned char RespFlag; 
    unsigned char RespSIDCode, RespLength;
    unsigned long RespData = 0; 
    unsigned long FunctionCode; 

    CanMsg_SessionCtl_TX.pucMsgData = SessionFrameTX;

    CanMsg_SessionCtl_RX.pucMsgData = SessionFrameRX;
    CanMsg_SessionCtl_RX.ulMsgLen = 8;
    CANMessageSet(CAN0_BASE, MSGOBJ_SES_CTRL, &CanMsg_SessionCtl_TX,
                          MSG_OBJ_TYPE_TX);
    UARTSend((unsigned char *)"-------Wait 4 Interrupt!---------", 33);
   // while(! (g_ulFlags&FLAG_RX_SES_CTRL))
    {
        //UARTSend((unsigned char *)"Wait for the SESSION RESPONSE", 29);
    }
    UARTSend((unsigned char *)"-------Got the Interrupt!--------", 33);

    CANMessageGet(CAN0_BASE, MSGOBJ_NUM_DATA_RX, &CanMsg_SessionCtl_RX, 1);
    UARTSend((unsigned char *)"ID |", 4);
    UARTSend((unsigned char *)&(CanMsg_SessionCtl_RX.ulMsgID), 4);
    UARTSend((unsigned char *)"--DATA |", 8);
    UARTSend((unsigned char *)CanMsg_SessionCtl_RX.pucMsgData, 8);


    RespSIDCode =  CanMsg_SessionCtl_RX.pucMsgData[1];
    if(RespSIDCode == 0x7f  || RespSIDCode != 0x50 || CanMsg_SessionCtl_RX.pucMsgData[2] != 0x03)
    {
         WRCommandH(0x01);
         //WRCommandH(0x20);
         ShowQQCharH(0x80, "Failed to Open Session", 11);
        //This is rejected by ECU, ignore this frame for this time
     //   exit(1);
    }
    else
    {
        WRCommandH(0x01);
        //WRCommandH(0x20);
        ShowQQCharH(0x80, "Session set ", 6);
    }
    

    g_ulFlags &= (~FLAG_RX_SES_CTRL);

}



int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_8 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
           SYSCTL_XTAL_8MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPadConfigSet(GPIO_PORTC_BASE,
                     GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
                     GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    
    //LCD use GPIO
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 );
    
    LCDInit();

    //Enable CAN
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeCAN(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //
    // Enable the CAN controller.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
   
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    UARTFIFODisable(UART0_BASE); 
     UARTSend((unsigned char *)"-------This is the CanBoard!-----", 33);
     

    //LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);


    CANInit(CAN0_BASE);

    //
    // Configure the bit clock parameters for the CAN device.
    //
    CANBitTimingSet(CAN0_BASE, &CANBitClkSettings[CANBAUD_500K]);

    //
    // Take the CAN0 device out of INIT state.
    //
    CANEnable(CAN0_BASE);

    //
    // Enable interrups from CAN controller.
    //
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR);
    
    CANConfigureNetwork();
    IntEnable(INT_CAN0);
    IntMasterEnable();

    SysTickPeriodSet(SysCtlClockGet() / 100);
    SysTickEnable();
    SysTickIntEnable();

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);


    Init_VW();
    while(1)
    {}

}
