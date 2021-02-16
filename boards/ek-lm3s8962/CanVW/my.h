#include "can.h"
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

#define MAX_CMD_LINE 6

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

unsigned char CMD_AIR_TEMP[8] = {0x03, 0x22, 0x26, 0x13, 0x55, 0x55, 0x55, 0x55};
unsigned char CMD_ENG_WLD[8] = {0x03, 0x22, 0x11, 0xe9, 0x55, 0x55, 0x55, 0x55};
unsigned char CMD_ENG_STS[8] = {0x03, 0x22, 0x39, 0x53, 0x55, 0x55, 0x55, 0x55};
unsigned char CMD_ENG_RPM[8] = {0x03, 0x22, 0xf4, 0x0c, 0x55, 0x55, 0x55, 0x55};
unsigned char CMD_ENG_THP[8] = {0x03, 0x22, 0xf4, 0x11, 0x55, 0x55, 0x55, 0x55};
unsigned char CMD_GBX_SHP[8] = {0x03, 0x22, 0x38, 0x08, 0x55, 0x55, 0x55, 0x55};

struct CANMsgObj  CMDList[6] = {
    { CANMSGID_TESTER_ENG    ,      0,0,8,CMD_ENG_STS  },
    { CANMSGID_TESTER_ENG    ,      0,0,8,CMD_ENG_WLD  },
    { CANMSGID_TESTER_ENG    ,      0,0,8,CMD_ENG_RPM  },
    { CANMSGID_TESTER_ENG    ,      0,0,8,CMD_ENG_THP  },
    { CANMSGID_TESTER_GEARBOX,      0,0,8,CMD_GBX_SHP  },
    { CANMSGID_TESTER_INSIDETEMP,   0,0,8,CMD_AIR_TEMP }
} ;
