
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
//�����Ǵ���ʱ���Ķ�дʱ��


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
   SendByteLCDH(0xf8+(WRS<<1));//�Ĵ���ѡ��WRS
   SendByteLCDH(Wdata&0xf0);
   SendByteLCDH((Wdata<<4)&0xf0);
 }
 

void WRCommandH(Uchar CMD)
{
  RS(0);
  RS(GPIO_PIN_7);
  SPIWH(CMD,0);
  //delay10US(100);//89S52��ģ�⴮��ͨ��,����,����89S52����ʱ,
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
  //delay10US(100);//89S52��ģ�⴮��ͨ��,����,����89S52����ʱ,
  Syswait_us(90);
}




void WRDataL(Uchar Data)
{ 
  RS(0);
  RS(GPIO_PIN_7);
  SPIWL(Data,1);

}

/*******************************************************************/
//��ʼ��LCD-8λ�ӿ�
void LCDInit(void)
{  

Syswait_ms(40);

WRCommandH(0x30); //����ָ�,8λ����
WRCommandL(0x30); //����ָ�,8λ����

 WRCommandH(0x06); //��ʼ���趨���������
 WRCommandL(0x06); //��ʼ���趨���������

 WRCommandH(0x01); //�����ʾDDRAM
 WRCommandL(0x01); //�����ʾDDRAM

//20ms
Syswait_ms(20);

 WRCommandH(0x0C); //��ʾ״̬���أ�������ʾ���������ʾ�أ������ʾ���׹�
 WRCommandL(0x0C); //��ʾ״̬���أ�������ʾ���������ʾ�أ������ʾ���׹�

 WRCommandH(0x02); //��ַ���� 
 WRCommandL(0x02); //��ַ���� 
 //20ms
Syswait_ms(20);
 
}
//---------------

//������ʾ���ִ�(��ʾ�������16*16����)
//addrΪ������ʾλ��,*hanzi����ָ��;countΪ���뺺�ִ��ַ���
void ShowQQCharH(Uchar addr,Uchar *hanzi,Uchar count)
{ 
 Uchar i;
 WRCommandH(addr); //�趨DDRAM��ַ
 for(i=0;i<count;)
 { 
     WRDataH(hanzi[i*2]);
      WRDataH(hanzi[i*2+1]);
  i++;
    }
}


//������ʾ�����ִ�(��ʾ�������16*16����)
void ShowQQCharL(Uchar addr,Uchar *english,Uchar count)
{ 
 Uchar i;
 WRCommandL(addr); //�趨DDRAM��ַ
 for(i=0;i<count;)
 { 
     WRDataL(english[i*2]);
        WRDataL(english[i*2+1]);
  i++;
    }
}

//�ϰ�����ʾ�����ִ�(����ַ�16*8)
//addrΪ����ַ��׸���ַ,iΪ�׸�����ַ�����,countΪ��Ҫ�����ַ�����
void ShowNUMCharH(Uchar addr,Uchar i,Uchar count)
{
     Uchar j;
 for(j=0;j<count;)
 { 
     WRCommandH(addr); //�趨DDRAM��ַ
  WRDataH(i+j);//��Ϊ����16*8λ�ַ�ƴ��һ��16*16������ʾ
  j++;
  WRDataH(i+j);
  addr++;
  j++;
 }  
}
//�°�����ʾ�����ִ�(����ַ�)
void ShowNUMCharL(Uchar addr,Uchar i,Uchar count)
{
     Uchar j;
 for(j=0;j<count;)
 { 
     WRCommandL(addr); //�趨DDRAM��ַ
  WRDataL(i+j);
  j++;
  WRDataL(i+j);
  addr++;
  j++;
 }  
}
//�Զ����ַ�д��CGRAM
//data1�Ǹ߰�λ,data2�ǵͰ�λ,һ�����������ֽ�,��������ֽ�,�������ۼ�,��16��
//һ���Զ����ַ�Ϊ16*16����
//��һ�������ֽ�Ϊ��40H��ʼ,��4F����Ϊ��һ���Զ��庺�ַ�, ֮�������ַ��8000HΪʼ��һ��
//addrΪ����ͷ��ַ
void WRCGRAMH(Uchar data1,Uchar data2,Uchar addr)
{     
      Uchar i;
      for(i=0;i<16;)
  {
      WRCommandH(addr+i);   //�趨CGRAM��ַ
      WRDataH(data1);
   WRDataH(data1);
      i++;
   WRCommandH(addr+i);   //�趨CGRAM��ַ
   WRDataH(data2);
   WRDataH(data2);
      i++;
  }  
}
//�Զ����ַ�д��CGRAM
void WRCGRAML(Uchar data1,Uchar data2,Uchar addr)
{     
      Uchar i;
      for(i=0;i<16;)
  {
      WRCommandL(addr+i);   //�趨CGRAM��ַ
      WRDataL(data1);
   WRDataL(data1);
      i++;
   WRCommandL(addr+i);   //�趨CGRAM��ַ
   WRDataL(data2);
   WRDataL(data2);
      i++;
  }  
}
//��ʾ�ϰ����Զ�����ַ�,��������ַ�����ȫ��16*16
//addrΪ��ʾ��ַ,���Զ����ַ���һ�����ֵ���,��8000H��ʼΪ��һ��,
//8100HΪ�ڶ���,8200HΪ������,8300HΪ���ĸ�,�����ֿ�ֻ���Զ����ĸ��ַ�
//iΪ�Զ����ַ�������ַ,�������λ,�������λ
//IC����,�����ֿ���,һ��7920���ֻ����ʾ16*2������
void ShowCGCharH(Uchar addr,Uchar i)
{
     Uchar j;
 for(j=0;j<0x20;)
 { 
     WRCommandH(addr+j); //�趨DDRAM��ַ
  WRDataH(0x00);//�ַ���ַ�Ͱ�λ
     WRDataH(i);//�ַ���ַ�߰�λ
  j++;
 }  
}
//��ʾ�°����Զ�����ַ�,��������ַ�����ȫ��16*16
void ShowCGCharL(Uchar addr,Uchar i)
{
     Uchar j;
 for(j=0;j<0x20;)
 { 
     WRCommandL(addr+j); //�趨DDRAM��ַ
  WRDataL(0x00);
     WRDataL(i);
  j++;
 }  
}

//�ϰ������ͼ��������
void CLEARGDRAMH(Uchar c)
{    
       Uchar j;
       Uchar i;
    WRCommandH(0x34);
    WRCommandH(0x36);
       for(j=0;j<32;j++)
    {
      WRCommandH(0x80+j);
      WRCommandH(0x80);//X����
      for(i=0;i<12;i++)//
     {
      WRDataH(c);
      WRDataH(c);
     }
     }
  
}
//�°������ͼ��������
void CLEARGDRAML(Uchar c)
{    
       Uchar j;
       Uchar i;
    WRCommandL(0x34);
    WRCommandL(0x36);
       for(j=0;j<32;j++)
    {
      WRCommandL(0x80+j);
      WRCommandL(0x80);//X����
      for(i=0;i<12;i++)//
     {
      WRDataL(c);
      WRDataL(c);
     }
     }
  
}


//д��GDRAM ��ͼ,Y���л�ͼ����,2���ֽ�һ��,CLONG��ͼ�γ���,���ֽ�Ϊ��λ;�ֽں����һ
//HIGHT��ͼ�θ߶�,TAB��ͼ�����ݱ�.16064M��ͼ����ʾ���൱��
//����160*32�����������,
//��ͼ�ڴ�������ʱ,����ڲ����µ�����Ҫ��һЩ
//16064:clongΪ20,hingtΪ32
//19264:clongΪ24,hingtΪ32
//24064:clongΪ30,hingtΪ32
/*
void WRGDRAM(Uchar Y1,Uchar clong,Uchar hight,Uchar *TAB1)
{    
       Uint k;
       Uchar j;
       Uchar i;
    WRCommandH(0x34);//ȥ��չָ��Ĵ���
    WRCommandH(0x36);//�򿪻�ͼ����
    for(j=0;j<hight;j++)//32��
  {   //���ϰ���
       WRCommandH(Y1+j);  //Y������,���ڼ���
    WRCommandH(0x80);//X���꣬�������ڼ����ֽڿ�ʼд��,80HΪ��һ���ֽ�
       for(i=0;i<clong;i++)//д��һ��
      {
       WRDataH(TAB1[clong*j+i]);
      }
  }
//���°���
    WRCommandL(0x34);
    WRCommandL(0x36);
    for(j=0;j<hight;j++)//32
   {   
       WRCommandL(Y1+j);  //Y������,���ڼ���
    WRCommandL(0x80);//X���꣬�������ڼ����ֽڿ�ʼд��,80HΪ��һ���ֽ�
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
    
//�Ȼ��ϰ���192*32    
    WRCommandH(0x34);   //ȥ��չָ��Ĵ���
    WRCommandH(0x36);   //�򿪻�ͼ����
    //�����ϱ߿�
    for(j=0;j<2;j++)    //2��   �������ϱ߿�
    {   
      WRCommandH(0x80+j);  //Y������,���ڼ���
      WRCommandH(0x80); //X���꣬�������ڼ����ֽڿ�ʼд��,80HΪ��һ���ֽ�
      for(i=0;i<12;i++) //д��һ��
     {
      WRDataH(x);
      WRDataH(x);
     }
    }
  //������߿�  
    for(j=2;j<32;j++)    //30��   
    {   //���ϰ���
      WRCommandH(0x80+j);  //Y������,���ڼ���
      WRCommandH(0x80); //X���꣬�������ڼ����ֽڿ�ʼд��,80HΪ��һ���ֽ�
      WRDataH(l);
      WRDataH(0x00);
    }    
   //�����ұ߿�  
    for(j=2;j<32;j++)    //30��   
    {   //���ϰ���
      WRCommandH(0x80+j);  //Y������,���ڼ���
      WRCommandH(0x80+11); //X���꣬�������ڼ����ֽڿ�ʼд��,80HΪ��һ���ֽ�
      WRDataH(0x00);
      WRDataH(r);
    }    
    
    
//���°���192*32
    WRCommandL(0x34);
    WRCommandL(0x36);
    for(j=30;j<32;j++)    //2��   �������±߿�
     {   
      WRCommandL(0x80+j);  //Y������,���ڼ���
      WRCommandL(0x80); //X���꣬�������ڼ����ֽڿ�ʼд��,80HΪ��һ���ֽ�
      for(i=0;i<12;i++) //д��һ��
     {
      WRDataL(x);
      WRDataL(x);
     }
     }

  //������߿�  
    for(j=0;j<30;j++)    //30��   
    {   //���ϰ���
      WRCommandL(0x80+j);  //Y������,���ڼ���
      WRCommandL(0x80); //X���꣬�������ڼ����ֽڿ�ʼд��,80HΪ��һ���ֽ�
      WRDataL(l);
      WRDataL(0x00);
    }    
   //�����ұ߿�  
    for(j=0;j<30;j++)    //30��   
    {   //���ϰ���
      WRCommandL(0x80+j);  //Y������,���ڼ���
      WRCommandL(0x80+11); //X���꣬�������ڼ����ֽڿ�ʼд��,80HΪ��һ���ֽ�
      WRDataL(0x00);
      WRDataL(r);
    }      
     
     
     
}


//��ʾ����ַ���������ɨ����Ե�
void displayTEST (void)
{
 ShowNUMCharH(0x80,0x01,24);//��ʾ����������
 ShowNUMCharH(0x90,0x30,24);//��ʾ���0~?���ֱ��
 ShowNUMCharL(0x80,0x41,24);//��ʾ���A~P��д
 ShowNUMCharL(0x90,0x61,24);//��ʾ���a~pСд

// WaitNms(180);  //�ȴ�ʱ��
// WaitNms(180);  //�ȴ�ʱ��
//
// WRCommandH(0x01); //�����ʾDDRAM
// WRCommandL(0x01); //�����ʾDDRAM
//
// WRCGRAMH(0xff,0x00,0x40);//д���
// WRCGRAMH(0x00,0xff,0x50);//д���2
// WRCGRAMH(0xaa,0xaa,0x60);//д����
// WRCGRAMH(0x55,0x55,0x70);//д����2
//
// 
// WRCGRAML(0xff,0x00,0x40);//д���
// WRCGRAML(0x00,0xff,0x50);//д���2
// WRCGRAML(0xaa,0xaa,0x60);//д����
// WRCGRAML(0x55,0x55,0x70);//д����2
//
//    ShowCGCharH(0x80,0x00);//��ʾ�Ტ����
//    ShowCGCharL(0x80,0x00);//��ʾ�Ტ����
// WaitNms(180);  //�ȴ�ʱ��
//
// WRCommandH(0x01); //�����ʾDDRAM
// WRCommandL(0x01); //�����ʾDDRAM
//
//    ShowCGCharH(0x80,02);//��ʾ��2������
//    ShowCGCharL(0x80,02);//��ʾ��2������
// WaitNms(180);  //�ȴ�ʱ��
//
// WRCommandH(0x01); //�����ʾDDRAM
// WRCommandL(0x01); //�����ʾDDRAM
//
// ShowCGCharH(0x80,04);//��ʾ��������
// ShowCGCharL(0x80,04);//��ʾ��������
// WaitNms(180);  //�ȴ�ʱ��
//
// WRCommandH(0x01); //�����ʾDDRAM
// WRCommandL(0x01); //�����ʾDDRAM
//
// ShowCGCharH(0x80,06);//��ʾ��2������
// ShowCGCharL(0x80,06);//��ʾ��2������
// WaitNms(180);  //�ȴ�ʱ��
//
// WRCommandH(0x01); //�����ʾDDRAM
// WRCommandL(0x01); //�����ʾDDRAM
//
// WRCGRAMH(0x00,0x00,0x40);//��CGRAM1
// WRCGRAMH(0x00,0x00,0x50);//��CGRAM2
//
// WRCGRAML(0x00,0x00,0x40);//��CGRAM1
// WRCGRAML(0x00,0x00,0x50);//��CGRAM2
//
// WRCGRAMH(0xaa,0x55,0x40);//д���
// WRCGRAMH(0x55,0xaa,0x50);//д���2
// WRCGRAML(0xaa,0x55,0x40);//д���
// WRCGRAML(0x55,0xaa,0x50);//д���2
//
// ShowCGCharH(0x80,00);//��ʾ�㲢����
// ShowCGCharL(0x80,00);//��ʾ�㲢����
// WaitNms(180);  //�ȴ�ʱ��
//
// WRCommandH(0x01); //�����ʾDDRAM
// WRCommandL(0x01); //�����ʾDDRAM
//
// ShowCGCharH(0x80,02);//��ʾ��2������
// ShowCGCharL(0x80,02);//��ʾ��2������
// WaitNms(180);  //�ȴ�ʱ��

}
//��ʾ���ִ�
void displayCHINSE(void)
{

 // WRCommandH(0x0C); //GA  ���λ�÷���
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
    ShowQQCharH(0x95,uctech1,4);//��ʾ'�澧�Ƽ�'
    ShowQQCharL(0x85,uctech2,4);//��ʾ'192*64'

}

void displayCHINSE2(void)
{
  
    ShowQQCharH(0x80,uctech6,lengthCF3);//�����ֿ�
    ShowQQCharH(0x90,uctech7,lengthCF3);
    ShowQQCharL(0x80,uctech8,lengthCF3);
    ShowQQCharL(0x90,uctech9,lengthCF3);

}
void displayEngine(void)
{
   ShowQQCharH(0x80, "״̬: ����  ��������ֵ�", 12);
   ShowQQCharH(0x90, "ת��: 713rpm/m", 7);
   ShowQQCharL(0x80, "��λ: D4",4);
   ShowQQCharL(0x90, "����ѹ��: 1.45Bar ",9);

}
