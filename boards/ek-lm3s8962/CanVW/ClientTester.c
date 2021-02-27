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
#include "can.h"
#include "driverlib/uart.h"
#include "19264LCDdriver.h"
#include "utils/ustdlib.h"
#include "my.h"

tCANMsgObject g_MsgObjectRx;
tCANMsgObject CanTxOnce;
tCANMsgObject CanRxInsideTemp;
tCANMsgObject g_MsgObjectTx;
//*****************************************************************************
//
// This global holds the flags used to indicate the state of the message
// objects.
//
//*****************************************************************************
static volatile unsigned long g_ulFlags = 0;

//------------------------------------------------------------------
//UART Send to COnsole
//------------------------------------------------------------------
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


//------------------------------------------------------------------
// CAN Interrupt Function
//------------------------------------------------------------------
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

//------------------------------------------------------------------
// CAN Message Object Configure
//------------------------------------------------------------------
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

//------------------------------------------------------------------
// Initialize the Session with ECU
//------------------------------------------------------------------

int ECU_Session_setup(){
    //CanID-??10 07?? Can Data->?? 02-|10 03 55 55 55 55 55 |]  
    unsigned char SessionFrameTX[8] = {0x02, 0x10, 0x03, 0x55, 0x55,0x55,0x55,0x55};
    unsigned char SessionFrameRX[8];
	char DisplayBuf[24];

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
   // ShowQQCharH(0x90, (Uchar *)DisplayBuf,9);
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
}


//------------------------------------------------------------------
// Handle the CAN interrupt
//------------------------------------------------------------------
void
ProcessInterrupts(void)
{
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

void DisplayMenu(void)
{
    /*----
    xxxxxxxxxxxxxxxxxxxxxxxx
    xxxxxxxxxxxxxxxxxxxxxxxx
    xxxxxxxxxxxxxxxxxxxxxxxx
    xxxxxxxxxxxxxxxxxxxxxxxx

    |xxx|xxxxxxxxxxxxxxxxxxx
    |xxx|xxxxxxxxxxxxxxxxxxx
    |xxx|xxxxxxxxxxxxxxxxxxx
    |xxx|xxxxxxxxx �¶�: 21.2

    -----*/

    Uchar line1[48],line2[48],line3[48],line4[48];
    
}


//------------------------------------------------------------------
// Main 
//------------------------------------------------------------------
int main(void)
{   
    int i,SessionResult, count;
    char DisplayBuf[24];

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
   // ShowQQCharH(0x80, "----Volkswagen-Power----", 12);
    SessionResult = ECU_Session_setup();
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
    
    // Display Test
    CLEARGDRAMH(0x00);
    CLEARGDRAML(0x00);
    DrawModel(3, 29, 104,34,VWcar);
    DrawModel(0, 0, 24,39, Shift_S);
    DrawModel(8, 9, 32,32, Xiexian32x32);
    DrawModel(10,17, 24,16,ENG);
    WRCommandL(0x01);
    WRCommandL(0x20);
    WRCommandH(0x01);
    WRCommandH(0x20);
    ShowQQCharH(0x8A, "17% ",2);  
    ShowQQCharL(0x9A, "22.5",2);
 // 
    while(1)
        {

           // WRGDRAM1(0xFF,0xC0,0x03);
          // DrawModel(0,0, 24,16,ENG);
           
          
           //reg_h(0x80,0,12,2,0xff,0xff);
          //WRGDRAM(1,1,16,Cha);
           Syswait_ms(500);
            if(SessionResult==1)
            {
                
            }

//               ShowQQCharL(0x80, "----Session Failed -----", 12);
//           }
//           else{
//           
//               ShowQQCharL(0x80, "-----Session   OK  -----", 12);
///------------------------------------------------------------------
///  Send all Query Requset to ECU with the interval 10 miliseconds
///  And its response will be checked once interrupt raised
///  Response may not replied with as same sequence as request 
///------------------------------------------------------------------
//                for(i=0;  i<MAX_CMD_LINE; i++){
//                    Syswait_ms(10);
//                    CanTxOnce.ulMsgID = CMDList[i].ulMsgID;
//                    CanTxOnce.pucMsgData = CMDList[i].pucMsgData;
//                    CANMessageSet(CAN0_BASE, 
//                           MSGOBJ_NUM_DATA_TX, 
//                           &CanTxOnce,
//                           MSG_OBJ_TYPE_TX);
//                }
//
//               Syswait_ms(500);
//               ProcessInterrupts();
//            }
        }

}
