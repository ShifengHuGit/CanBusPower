#ifndef __19264__

#define __19264__
typedef unsigned char Uchar;
typedef unsigned int Uint;
static const Uchar lengthCF3=12;
static const Uchar LCDdelay=167;  //��ʱʱ�䳣��
static unsigned char uctech[] = {"��������Ե����˹��˰���"};
static unsigned char uctech3[] = {"�㶫ʡ�����б�����ʯ����"};
static unsigned char uctech4[] = {"�鲼�ڶ���ҵ��4 ��2 ¥"};
static unsigned char uctech5[] = {"��ʵ�������е���������ʾ"};

static unsigned char uctech6[] = {"��                    ��"};
static unsigned char uctech7[] = {"��      �澧�Ƽ�      Ʒ"};
static unsigned char uctech8[] = {"��       192*64       ��"};
static unsigned char uctech9[] = {"�� www.huijinglcm.com ��"};

static unsigned char uctech1[] = {"�澧�Ƽ�"};
static unsigned char uctech2[] = {" 192*64"};


void E2(int i);
void E1(int i);
void RW(int i);
void RS(int i);
void REST(int i);


//������ڴ���ʱָ�������֮�����ʱ

void delay10US(Uchar x);

static void Wait1ms(void);//�ӳ�1 ms

//�ӳ�n ms 
void WaitNms(int n);

/////////////////////////////////////////////////////////////////////////////////
//�����Ǵ���ʱ���Ķ�дʱ��


void SendByteLCDH(Uchar WLCDData);
void SPIWH(Uchar Wdata,Uchar WRS);
void WRCommandH(Uchar CMD);
void WRDataH(Uchar Data);
void SendByteLCDL(Uchar WLCDData);
void SPIWL(Uchar Wdata,Uchar WRS);
void WRCommandL(Uchar CMD);
void WRDataL(Uchar Data);


/*******************************************************************/
//��ʼ��LCD-8λ�ӿ�
void LCDInit(void);

//������ʾ���ִ�(��ʾ�������16*16����)
//addrΪ������ʾλ��,*hanzi����ָ��;countΪ���뺺�ִ��ַ���
void ShowQQCharH(Uchar addr,Uchar *hanzi,Uchar count);

//������ʾ�����ִ�(��ʾ�������16*16����)
void ShowQQCharL(Uchar addr,Uchar *english,Uchar count);


//�ϰ�����ʾ�����ִ�(����ַ�16*8)
//addrΪ����ַ��׸���ַ,iΪ�׸�����ַ�����,countΪ��Ҫ�����ַ�����
void ShowNUMCharH(Uchar addr,Uchar i,Uchar count);

//�°�����ʾ�����ִ�(����ַ�)
void ShowNUMCharL(Uchar addr,Uchar i,Uchar count);

//�Զ����ַ�д��CGRAM
//data1�Ǹ߰�λ,data2�ǵͰ�λ,һ�����������ֽ�,��������ֽ�,�������ۼ�,��16��
//һ���Զ����ַ�Ϊ16*16����
//��һ�������ֽ�Ϊ��40H��ʼ,��4F����Ϊ��һ���Զ��庺�ַ�, ֮�������ַ��8000HΪʼ��һ��
//addrΪ����ͷ��ַ
void WRCGRAMH(Uchar data1,Uchar data2,Uchar addr);

//�Զ����ַ�д��CGRAM
void WRCGRAML(Uchar data1,Uchar data2,Uchar addr);

//��ʾ�ϰ����Զ�����ַ�,��������ַ�����ȫ��16*16
//addrΪ��ʾ��ַ,���Զ����ַ���һ�����ֵ���,��8000H��ʼΪ��һ��,
//8100HΪ�ڶ���,8200HΪ������,8300HΪ���ĸ�,�����ֿ�ֻ���Զ����ĸ��ַ�
//iΪ�Զ����ַ�������ַ,�������λ,�������λ
//IC����,�����ֿ���,һ��7920���ֻ����ʾ16*2������
void ShowCGCharH(Uchar addr,Uchar i);

//��ʾ�°����Զ�����ַ�,��������ַ�����ȫ��16*16
void ShowCGCharL(Uchar addr,Uchar i);

//�ϰ������ͼ��������
void CLEARGDRAMH(Uchar c);

//�°������ͼ��������
void CLEARGDRAML(Uchar c);
void WRGDRAM1(Uchar x,Uchar l,Uchar r );

//��ʾ����ַ���������ɨ����Ե�
void displayTEST (void);

//��ʾ���ִ�
void displayCHINSE(void);
void displayCHINSE1(void);
void displayCHINSE2(void);
void displayEngine(void);
void Syswait_ms(int Count);
void Syswait_us(int Count);
#endif
