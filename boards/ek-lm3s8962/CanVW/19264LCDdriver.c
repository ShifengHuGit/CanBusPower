
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
#include "19264LCDdriver.h"



void Syswait_ms(int Count)
{
   SysCtlDelay( (SysCtlClockGet() / 1000)*Count);
}

void Syswait_us(int Count)
{
   SysCtlDelay( (SysCtlClockGet() / 1000000)*Count);
}

//
void E2(int i) { 
   GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, i);
}
void E1(int i) { 
   GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, i);
}
void RW(int i) { 
   GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, i);
}
void RS(int i) { 
   GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, i);
}

void REST(int i) { 
   GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, i);
}
/////////////////////////////////////////////////////////////////////////////////
//以下是串口时开的读写时序


void SendByteLCDH(Uchar WLCDData)
{
 Uchar i;
 for(i=0;i<8;i++)
 {
 if((WLCDData<<i)&0x80) RW(GPIO_PIN_6);
 else RW(0);
 E1(0); 
 E1(GPIO_PIN_5) ;
 }
}


 void SPIWH(Uchar Wdata,Uchar WRS)
 {
   SendByteLCDH(0xf8+(WRS<<1));//寄存器选择WRS
   SendByteLCDH(Wdata&0xf0);
   SendByteLCDH((Wdata<<4)&0xf0);
 }
 

void WRCommandH(Uchar CMD)
{
  RS(0);
  RS(GPIO_PIN_7);
  SPIWH(CMD,0);
  //delay10US(100);//89S52来模拟串行通信,所以,加上89S52的延时,
  Syswait_us(90);
}



void WRDataH(Uchar Data)
{ 
  RS(0);
  RS(GPIO_PIN_7);
  SPIWH(Data,1);

}


void SendByteLCDL(Uchar WLCDData)
{
 Uchar i;
 for(i=0;i<8;i++)
 {
 if((WLCDData<<i)&0x80) RW(GPIO_PIN_6);
 else RW(0);
 E2(0); 
 E2(GPIO_PIN_4) ;
 }
}

 
 void SPIWL(Uchar Wdata,Uchar WRS)
 {
   SendByteLCDL(0xf8+(WRS<<1));
   SendByteLCDL(Wdata&0xf0);
   SendByteLCDL((Wdata<<4)&0xf0);
 }

 
void WRCommandL(Uchar CMD)
{
  RS(0);
  RS(GPIO_PIN_7);
  SPIWL(CMD,0);
  //delay10US(100);//89S52来模拟串行通信,所以,加上89S52的延时,
  Syswait_us(90);
}




void WRDataL(Uchar Data)
{ 
  RS(0);
  RS(GPIO_PIN_7);
  SPIWL(Data,1);

}

/*******************************************************************/
//初始化LCD-8位接口
void LCDInit(void)
{  

Syswait_ms(40);

WRCommandH(0x30); //基本指令集,8位并行
WRCommandL(0x30); //基本指令集,8位并行

 WRCommandH(0x06); //启始点设定：光标右移
 WRCommandL(0x06); //启始点设定：光标右移

 WRCommandH(0x01); //清除显示DDRAM
 WRCommandL(0x01); //清除显示DDRAM

//20ms
Syswait_ms(20);

 WRCommandH(0x0C); //显示状态开关：整体显示开，光标显示关，光标显示反白关
 WRCommandL(0x0C); //显示状态开关：整体显示开，光标显示关，光标显示反白关

 WRCommandH(0x02); //地址归零 
 WRCommandL(0x02); //地址归零 
 //20ms
Syswait_ms(20);
 
}
//---------------

//上屏显示汉字串(显示半宽字型16*16点阵)
//addr为汉字显示位置,*hanzi汉字指针;count为输入汉字串字符数
void ShowQQCharH(Uchar addr,Uchar *hanzi,Uchar count)
{ 
 Uchar i;
 WRCommandH(addr); //设定DDRAM地址
 for(i=0;i<count;)
 { 
     WRDataH(hanzi[i*2]);
      WRDataH(hanzi[i*2+1]);
  i++;
    }
}


//下屏显示汉字字串(显示半宽字型16*16点阵)
void ShowQQCharL(Uchar addr,Uchar *english,Uchar count)
{ 
 Uchar i;
 WRCommandL(addr); //设定DDRAM地址
 for(i=0;i<count;)
 { 
     WRDataL(english[i*2]);
        WRDataL(english[i*2+1]);
  i++;
    }
}

//上半屏显示连续字串(半宽字符16*8)
//addr为半宽字符首个地址,i为首个半宽字符代码,count为需要输入字符个数
void ShowNUMCharH(Uchar addr,Uchar i,Uchar count)
{
     Uchar j;
 for(j=0;j<count;)
 { 
     WRCommandH(addr); //设定DDRAM地址
  WRDataH(i+j);//必为两个16*8位字符拼成一个16*16才能显示
  j++;
  WRDataH(i+j);
  addr++;
  j++;
 }  
}
//下半屏显示连续字串(半宽字符)
void ShowNUMCharL(Uchar addr,Uchar i,Uchar count)
{
     Uchar j;
 for(j=0;j<count;)
 { 
     WRCommandL(addr); //设定DDRAM地址
  WRDataL(i+j);
  j++;
  WRDataL(i+j);
  addr++;
  j++;
 }  
}
//自定义字符写入CGRAM
//data1是高八位,data2是低八位,一存必须存两个字节,横向存两字节,后纵向累加,共16行
//一个自定义字符为16*16点阵
//第一个存入字节为从40H开始,到4F结束为第一个自定义汉字符, 之后调出地址从8000H为始第一个
//addr为存入头地址
void WRCGRAMH(Uchar data1,Uchar data2,Uchar addr)
{     
      Uchar i;
      for(i=0;i<16;)
  {
      WRCommandH(addr+i);   //设定CGRAM地址
      WRDataH(data1);
   WRDataH(data1);
      i++;
   WRCommandH(addr+i);   //设定CGRAM地址
   WRDataH(data2);
   WRDataH(data2);
      i++;
  }  
}
//自定义字符写入CGRAM
void WRCGRAML(Uchar data1,Uchar data2,Uchar addr)
{     
      Uchar i;
      for(i=0;i<16;)
  {
      WRCommandL(addr+i);   //设定CGRAM地址
      WRDataL(data1);
   WRDataL(data1);
      i++;
   WRCommandL(addr+i);   //设定CGRAM地址
   WRDataL(data2);
   WRDataL(data2);
      i++;
  }  
}
//显示上半屏自定义的字符,并把这个字符填满全屏16*16
//addr为显示地址,把自定义字符当一个汉字调出,从8000H开始为第一个,
//8100H为第二个,8200H为第三个,8300H为第四个,中文字库只能自定义四个字符
//i为自定义字符调出地址,先输入低位,再输入高位
//IC决定,中文字库类,一个7920最多只能显示16*2个汉字
void ShowCGCharH(Uchar addr,Uchar i)
{
     Uchar j;
 for(j=0;j<0x20;)
 { 
     WRCommandH(addr+j); //设定DDRAM地址
  WRDataH(0x00);//字符地址低八位
     WRDataH(i);//字符地址高八位
  j++;
 }  
}
//显示下半屏自定义的字符,并把这个字符填满全屏16*16
void ShowCGCharL(Uchar addr,Uchar i)
{
     Uchar j;
 for(j=0;j<0x20;)
 { 
     WRCommandL(addr+j); //设定DDRAM地址
  WRDataL(0x00);
     WRDataL(i);
  j++;
 }  
}

//上半屏清除图形区数据
void CLEARGDRAMH(Uchar c)
{    
       Uchar j;
       Uchar i;
    WRCommandH(0x34);
    WRCommandH(0x36);
       for(j=0;j<32;j++)
    {
      WRCommandH(0x80+j);
      WRCommandH(0x80);//X坐标
      for(i=0;i<12;i++)//
     {
      WRDataH(c);
      WRDataH(c);
     }
     }
  
}
//下半屏清除图形区数据
void CLEARGDRAML(Uchar c)
{    
       Uchar j;
       Uchar i;
    WRCommandL(0x34);
    WRCommandL(0x36);
       for(j=0;j<32;j++)
    {
      WRCommandL(0x80+j);
      WRCommandL(0x80);//X坐标
      for(i=0;i<12;i++)//
     {
      WRDataL(c);
      WRDataL(c);
     }
     }
  
}


//写入GDRAM 绘图,Y是行绘图坐标,2个字节一行,CLONG是图形长度,以字节为单位;字节横向加一
//HIGHT是图形高度,TAB是图形数据表.16064M的图形显示是相当于
//两屏160*32上下两屏组成,
//绘图在串口输入时,会比在并口下的输入要慢一些
//16064:clong为20,hingt为32
//19264:clong为24,hingt为32
//24064:clong为30,hingt为32
/*
void WRGDRAM(Uchar Y1,Uchar clong,Uchar hight,Uchar *TAB1)
{    
       Uint k;
       Uchar j;
       Uchar i;
    WRCommandH(0x34);//去扩展指令寄存器
    WRCommandH(0x36);//打开绘图功能
    for(j=0;j<hight;j++)//32行
  {   //先上半屏
       WRCommandH(Y1+j);  //Y总坐标,即第几行
    WRCommandH(0x80);//X坐标，即横数第几个字节开始写起,80H为第一个字节
       for(i=0;i<clong;i++)//写入一行
      {
       WRDataH(TAB1[clong*j+i]);
      }
  }
//后下半屏
    WRCommandL(0x34);
    WRCommandL(0x36);
    for(j=0;j<hight;j++)//32
   {   
       WRCommandL(Y1+j);  //Y总坐标,即第几行
    WRCommandL(0x80);//X坐标，即横数第几个字节开始写起,80H为第一个字节
       for(k=0;k<clong;k++)//
      {
       WRDataL(TAB1[clong*(j+hight)+k]);
      }
   }
}
*/
void WRGDRAM1(Uchar x,Uchar l,Uchar r )
{    

   Uchar j;
   Uchar i;
    
//先画上半屏192*32    
    WRCommandH(0x34);   //去扩展指令寄存器
    WRCommandH(0x36);   //打开绘图功能
    //两横上边框
    for(j=0;j<2;j++)    //2行   画两横上边框
    {   
      WRCommandH(0x80+j);  //Y总坐标,即第几行
      WRCommandH(0x80); //X坐标，即横数第几个字节开始写起,80H为第一个字节
      for(i=0;i<12;i++) //写入一行
     {
      WRDataH(x);
      WRDataH(x);
     }
    }
  //两横左边框  
    for(j=2;j<32;j++)    //30行   
    {   //先上半屏
      WRCommandH(0x80+j);  //Y总坐标,即第几行
      WRCommandH(0x80); //X坐标，即横数第几个字节开始写起,80H为第一个字节
      WRDataH(l);
      WRDataH(0x00);
    }    
   //两横右边框  
    for(j=2;j<32;j++)    //30行   
    {   //先上半屏
      WRCommandH(0x80+j);  //Y总坐标,即第几行
      WRCommandH(0x80+11); //X坐标，即横数第几个字节开始写起,80H为第一个字节
      WRDataH(0x00);
      WRDataH(r);
    }    
    
    
//后画下半屏192*32
    WRCommandL(0x34);
    WRCommandL(0x36);
    for(j=30;j<32;j++)    //2行   画两横下边框
     {   
      WRCommandL(0x80+j);  //Y总坐标,即第几行
      WRCommandL(0x80); //X坐标，即横数第几个字节开始写起,80H为第一个字节
      for(i=0;i<12;i++) //写入一行
     {
      WRDataL(x);
      WRDataL(x);
     }
     }

  //两横左边框  
    for(j=0;j<30;j++)    //30行   
    {   //先上半屏
      WRCommandL(0x80+j);  //Y总坐标,即第几行
      WRCommandL(0x80); //X坐标，即横数第几个字节开始写起,80H为第一个字节
      WRDataL(l);
      WRDataL(0x00);
    }    
   //两横右边框  
    for(j=0;j<30;j++)    //30行   
    {   //先上半屏
      WRCommandL(0x80+j);  //Y总坐标,即第几行
      WRCommandL(0x80+11); //X坐标，即横数第几个字节开始写起,80H为第一个字节
      WRDataL(0x00);
      WRDataL(r);
    }      
     
     
     
}


//显示半宽字符及横竖点扫描测试点
void displayTEST (void)
{
 ShowNUMCharH(0x80,0x01,24);//显示半宽特殊符号
 ShowNUMCharH(0x90,0x30,24);//显示半宽0~?数字标点
 ShowNUMCharL(0x80,0x41,24);//显示半宽A~P大写
 ShowNUMCharL(0x90,0x61,24);//显示半宽a~p小写

// WaitNms(180);  //等待时间
// WaitNms(180);  //等待时间
//
// WRCommandH(0x01); //清除显示DDRAM
// WRCommandL(0x01); //清除显示DDRAM
//
// WRCGRAMH(0xff,0x00,0x40);//写入横
// WRCGRAMH(0x00,0xff,0x50);//写入横2
// WRCGRAMH(0xaa,0xaa,0x60);//写入竖
// WRCGRAMH(0x55,0x55,0x70);//写入竖2
//
// 
// WRCGRAML(0xff,0x00,0x40);//写入横
// WRCGRAML(0x00,0xff,0x50);//写入横2
// WRCGRAML(0xaa,0xaa,0x60);//写入竖
// WRCGRAML(0x55,0x55,0x70);//写入竖2
//
//    ShowCGCharH(0x80,0x00);//显示横并填满
//    ShowCGCharL(0x80,0x00);//显示横并填满
// WaitNms(180);  //等待时间
//
// WRCommandH(0x01); //清除显示DDRAM
// WRCommandL(0x01); //清除显示DDRAM
//
//    ShowCGCharH(0x80,02);//显示横2并填满
//    ShowCGCharL(0x80,02);//显示横2并填满
// WaitNms(180);  //等待时间
//
// WRCommandH(0x01); //清除显示DDRAM
// WRCommandL(0x01); //清除显示DDRAM
//
// ShowCGCharH(0x80,04);//显示竖并填满
// ShowCGCharL(0x80,04);//显示竖并填满
// WaitNms(180);  //等待时间
//
// WRCommandH(0x01); //清除显示DDRAM
// WRCommandL(0x01); //清除显示DDRAM
//
// ShowCGCharH(0x80,06);//显示竖2并填满
// ShowCGCharL(0x80,06);//显示竖2并填满
// WaitNms(180);  //等待时间
//
// WRCommandH(0x01); //清除显示DDRAM
// WRCommandL(0x01); //清除显示DDRAM
//
// WRCGRAMH(0x00,0x00,0x40);//清CGRAM1
// WRCGRAMH(0x00,0x00,0x50);//清CGRAM2
//
// WRCGRAML(0x00,0x00,0x40);//清CGRAM1
// WRCGRAML(0x00,0x00,0x50);//清CGRAM2
//
// WRCGRAMH(0xaa,0x55,0x40);//写入点
// WRCGRAMH(0x55,0xaa,0x50);//写入点2
// WRCGRAML(0xaa,0x55,0x40);//写入点
// WRCGRAML(0x55,0xaa,0x50);//写入点2
//
// ShowCGCharH(0x80,00);//显示点并填满
// ShowCGCharL(0x80,00);//显示点并填满
// WaitNms(180);  //等待时间
//
// WRCommandH(0x01); //清除显示DDRAM
// WRCommandL(0x01); //清除显示DDRAM
//
// ShowCGCharH(0x80,02);//显示点2并填满
// ShowCGCharL(0x80,02);//显示点2并填满
// WaitNms(180);  //等待时间

}
//显示汉字串
void displayCHINSE(void)
{

 // WRCommandH(0x0C); //GA  光标位置反白
 // WRCommandL(0x0C); //GA
//  WRCommandH(0x1C); //GA 
//  WRCommandL(0x1C); //GA  
  
    ShowQQCharH(0x90,uctech3,lengthCF3);
    ShowQQCharL(0x80,uctech4,lengthCF3);
    ShowQQCharH(0x80,uctech,lengthCF3);

    ShowQQCharL(0x90,uctech5,lengthCF3);
 //WRCommandH(0x34);
// WRCommandL(0x34); 
 //WRCommandL(0x05);
}

void displayCHINSE1(void)
{
    ShowQQCharH(0x95,uctech1,4);//显示'绘晶科技'
    ShowQQCharL(0x85,uctech2,4);//显示'192*64'

}

void displayCHINSE2(void)
{
  
    ShowQQCharH(0x80,uctech6,lengthCF3);//调用字库
    ShowQQCharH(0x90,uctech7,lengthCF3);
    ShowQQCharL(0x80,uctech8,lengthCF3);
    ShowQQCharL(0x90,uctech9,lengthCF3);

}
void displayEngine(void)
{
   ShowQQCharH(0x80, "状态: 怠速  这他妈的闹的", 12);
   ShowQQCharH(0x90, "转速: 713rpm/m", 7);
   ShowQQCharL(0x80, "档位: D4",4);
   ShowQQCharL(0x90, "涡轮压力: 1.45Bar ",9);

}
