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

#define CANMSGID_TESTER_INSIDETEMP          0x0746
#define CANMSGID_ECU_INSIDETEMP          0x07B0



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
tCANMsgObject CanTxOnce;
tCANMsgObject CanRxInsideTemp;

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


//-----------
// CAN Interrupt Function
void
CANHandler(void)
{
    unsigned long ulStatus;
     unsigned char pucData[8];
    //UARTSend((unsigned char *)"CAN Interrupt Triggered", 23);
    //
    // Find the cause of the interrupt, if it is a status interrupt then just
    // acknowledge the interrupt by reading the status register.
    //
    ulStatus = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE); 
    g_ulFlags = ulStatus;
  //UARTSend("Can StatusID:", 13);
  UARTCharPut(UART0_BASE, (unsigned char )ulStatus);

    switch(ulStatus)
    {   
        case MSGOBJ_NUM_DATA_TX:
        {
            g_ulFlags &= (~FLAG_DATA_TX_PEND);
            break;
        }
        case MSGOBJ_NUM_DATA_RX:
        {
            g_ulFlags |= FLAG_DATA_RECV;
            break;
        }
       case MSGOBJ_SES_CTRL:
        {
            //UARTSend((unsigned char *)"Here you are", 12);
            //g_MsgObjectRx.pucMsgData = pucData;
            //g_MsgObjectRx.ulMsgLen = 8;
            //CANMessageGet(CAN0_BASE, MSGOBJ_SES_CTRL, &g_MsgObjectRx, 8);
            //UARTSend(pucData, 8);
            g_ulFlags = FLAG_RX_SES_CTRL;
            break;
        }  
        case MSGOBJ_AIR_INSIDETEMP:
        {
           //UARTSend((unsigned char *)"Here you are", 12);
           //g_MsgObjectRx.pucMsgData = pucData;
           //g_MsgObjectRx.ulMsgLen = 8;
           //CANMessageGet(CAN0_BASE, MSGOBJ_SES_CTRL, &g_MsgObjectRx, 8);
           //UARTSend(pucData, 8);
            g_ulFlags = FLAG_RX_AIR_INSIDETEMP;
            break;
        }  
    }
    CANIntClear(CAN0_BASE, ulStatus);
}


void
CANConfigureNetwork(void)
{
    //
    // This is the message object used to send button updates.  This message
    // object will not be "set" right now as that would trigger a transmission.
    //
    //CanTxOnce.ulMsgID = CANMSGID_TESTER_SESSION;
    CanTxOnce.ulMsgIDMask = 0;
    CanTxOnce.ulFlags = MSG_OBJ_TX_INT_ENABLE;
    CanTxOnce.ulMsgLen = 8;


    CanMsg_SessionCtl_TX.ulMsgID = CANMSGID_TESTER_SESSION;
    CanMsg_SessionCtl_TX.ulMsgIDMask = 0;
    CanMsg_SessionCtl_TX.ulFlags = MSG_OBJ_TX_INT_ENABLE;
    CanMsg_SessionCtl_TX.ulMsgLen = 8;
    //CanMsg_SessionCtl_TX.pucMsgData = SessionFrameTX;

    CanMsg_SessionCtl_RX.ulMsgID = CANMSGID_ECU_SESSION;
    CanMsg_SessionCtl_RX.ulMsgIDMask = 0x7FF;
    CanMsg_SessionCtl_RX.ulFlags = MSG_OBJ_RX_INT_ENABLE|MSG_OBJ_USE_ID_FILTER;
    CanMsg_SessionCtl_RX.ulMsgLen = 8;
    CANMessageSet(CAN0_BASE, MSGOBJ_SES_CTRL, &CanMsg_SessionCtl_RX,
                  MSG_OBJ_TYPE_RX);


   
    
    CanRxInsideTemp.ulMsgID = CANMSGID_ECU_INSIDETEMP;
    CanRxInsideTemp.ulMsgIDMask = 0x7FF;
    CanRxInsideTemp.ulFlags = MSG_OBJ_RX_INT_ENABLE|MSG_OBJ_USE_ID_FILTER;
    CanRxInsideTemp.ulMsgLen = 8;
    CANMessageSet(CAN0_BASE, MSGOBJ_AIR_INSIDETEMP, &CanRxInsideTemp,
                  MSG_OBJ_TYPE_RX);
    

}


int ECU_Session_setup(){
    //CanID-??10 07?? Can Data->?? 02-|10 03 55 55 55 55 55 |]  
    unsigned char SessionFrameTX[8] = {0x02, 0x10, 0x03, 0x55, 0x55,0x55,0x55,0x55};
    unsigned char SessionFrameRX[8],DisplayBuf[24];
    unsigned char RespFlag; 
    unsigned char RespSIDCode, RespLength;
    unsigned long RespData = 0; 
    unsigned long FunctionCode; 

    CanMsg_SessionCtl_TX.pucMsgData = SessionFrameTX;
    CANMessageSet(CAN0_BASE, MSGOBJ_NUM_DATA_TX, &CanMsg_SessionCtl_TX,
                          MSG_OBJ_TYPE_TX);
   // UARTSend((unsigned char *)"-------Wait 4 Interrupt!---------", 33);
  while(g_ulFlags != FLAG_RX_SES_CTRL)
  { 
     
  }

    CanMsg_SessionCtl_RX.pucMsgData = SessionFrameRX;
    CanMsg_SessionCtl_RX.ulMsgLen = 8;
    CANMessageGet(CAN0_BASE, MSGOBJ_SES_CTRL, &CanMsg_SessionCtl_RX, 1);
    
    usprintf(DisplayBuf, "Resp: %12X", SessionFrameRX);
    ShowQQCharH(0x90, (unsigned char *)DisplayBuf,9);
    ////  
    //UARTSend((unsigned char *)"ID |", 4);
    //UARTSend((unsigned char *)&(CanMsg_SessionCtl_RX.ulMsgID), 4);
    //UARTSend((unsigned char *)"--DATA |", 8);
    //UARTSend((unsigned char *)CanMsg_SessionCtl_RX.pucMsgData, 8);
    //
    //
 //RespSIDCode =  CanMsg_SessionCtl_RX.pucMsgData[1];
 if(CanMsg_SessionCtl_RX.pucMsgData[0] == 0x7f 
      || CanMsg_SessionCtl_RX.pucMsgData[1] != 0x50
       || CanMsg_SessionCtl_RX.pucMsgData[2] != 0x03  )
 {
     //UARTSend("Initial Session setup failed!", 29);
     CANMessageClear(CAN0_BASE, MSGOBJ_NUM_DATA_TX);
      g_ulFlags &= (~FLAG_RX_SES_CTRL);
     return 1;
 }
 else
 {
     //UARTSend("Initial Session setup Succeed!", 30);
     CANMessageClear(CAN0_BASE, MSGOBJ_NUM_DATA_TX);
     g_ulFlags &= (~FLAG_RX_SES_CTRL);
     return 0;
 }
//  

   

}

void
ProcessInterrupts(void)
{

    unsigned char SessionFrameTX[8] = {0x06, 0x50, 0x03, 0x00, 0x32,0x01,0xf4,0xaa};
    //03 22 26 13 55 55 55 55
    unsigned char TempRX[8] ;
    int temp,tempI,tempD;
    char DisplayBuf[24];
    memset(DisplayBuf, 0, 24);
    if(g_ulFlags == FLAG_RX_AIR_INSIDETEMP)
    {
        UARTSend("GetTemp:",8);
        CanRxInsideTemp.pucMsgData = TempRX;
        CANMessageGet(CAN0_BASE, MSGOBJ_AIR_INSIDETEMP, &CanRxInsideTemp, 1);
        temp=CanRxInsideTemp.pucMsgData[4]*256+CanRxInsideTemp.pucMsgData[5];
        tempI=temp/10;
        tempD=temp%10;
        usprintf(DisplayBuf, "Temp: %2d.%1d C", tempI, tempD);
        ShowQQCharL(0x90, (unsigned char *)DisplayBuf,5);
            //
            // Clear the flag.
            //
            g_ulFlags &= ~(FLAG_RX_AIR_INSIDETEMP);
             g_ulFlags = 0 ;
    }
    
}


int main(void)
{   
    int r, count;
    unsigned long ulLoop,ulLoop2;
    char DisplayBuf[24];
    unsigned char CMD_AIR_TEMP[8] = {0x03, 0x22, 0x26, 0x13, 0x55, 0x55, 0x55, 0x55};

    memset(DisplayBuf, 0, 24);
    //Set clock
    SysCtlClockSet(SYSCTL_SYSDIV_8 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
           SYSCTL_XTAL_8MHZ);
    //Enable JTAG
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
    
    //WRCommandL(0x01);
    
    //WRCommandH(0x20);
   
    //Enable CAN
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeCAN(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    
    //
    // Enable the CAN controller.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    //Enable UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
    UARTFIFODisable(UART0_BASE); 
   // UARTSend((unsigned char *)"-------This is the CanBoard!-----", 33);
     

    //Enable LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);


    //Enable CAN Interrupt
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

   // SysTickPeriodSet(SysCtlClockGet() / 100);
   // SysTickEnable();
   // SysTickIntEnable();

   // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
    ShowQQCharH(0x80, "----Volkswagen-Power----", 12);
   r = ECU_Session_setup();
   

GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

   
   count=7;
    while(1)
    {
        if(r==1)
        {
            ShowQQCharL(0x80, "----Session Failed -----", 12);
        }
        else{
    
         ShowQQCharL(0x80, "-----Session   OK  -----", 12);
       
         CanTxOnce.ulMsgID = CANMSGID_TESTER_INSIDETEMP;
         CanTxOnce.pucMsgData = CMD_AIR_TEMP;
         CANMessageSet(CAN0_BASE, MSGOBJ_NUM_DATA_TX, &CanTxOnce,
                 MSG_OBJ_TYPE_TX);

          
        Syswait_ms(1000);
         count+=1;
         if(count == 10)
         {count = 0;}
        ProcessInterrupts();
        }
    }

}
