#ifndef __19264__

#define __19264__
typedef unsigned char Uchar;
typedef unsigned int Uint;
static const Uchar lengthCF3=12;
static const Uchar LCDdelay=167;  //延时时间常数
static unsigned char uctech[] = {"真是他麻痹的奇了怪了啊哈"};
static unsigned char uctech3[] = {"广东省深圳市宝安区石岩镇"};
static unsigned char uctech4[] = {"麻布第二工业区4 栋2 楼"};
static unsigned char uctech5[] = {"可实现任意行单独反白显示"};

static unsigned char uctech6[] = {"汇                    精"};
static unsigned char uctech7[] = {"聚      绘晶科技      品"};
static unsigned char uctech8[] = {"焦       192*64       至"};
static unsigned char uctech9[] = {"点 www.huijinglcm.com 上"};

static unsigned char uctech1[] = {"绘晶科技"};
static unsigned char uctech2[] = {" 192*64"};


void E2(int i);
void E1(int i);
void RW(int i);
void RS(int i);
void REST(int i);


//这个是在串口时指令和数据之间的延时

void delay10US(Uchar x);

static void Wait1ms(void);//延迟1 ms

//延迟n ms 
void WaitNms(int n);

/////////////////////////////////////////////////////////////////////////////////
//以下是串口时开的读写时序


void SendByteLCDH(Uchar WLCDData);
void SPIWH(Uchar Wdata,Uchar WRS);
void WRCommandH(Uchar CMD);
void WRDataH(Uchar Data);
void SendByteLCDL(Uchar WLCDData);
void SPIWL(Uchar Wdata,Uchar WRS);
void WRCommandL(Uchar CMD);
void WRDataL(Uchar Data);


/*******************************************************************/
//初始化LCD-8位接口
void LCDInit(void);

//上屏显示汉字串(显示半宽字型16*16点阵)
//addr为汉字显示位置,*hanzi汉字指针;count为输入汉字串字符数
void ShowQQCharH(Uchar addr,Uchar *hanzi,Uchar count);

//下屏显示汉字字串(显示半宽字型16*16点阵)
void ShowQQCharL(Uchar addr,Uchar *english,Uchar count);


//上半屏显示连续字串(半宽字符16*8)
//addr为半宽字符首个地址,i为首个半宽字符代码,count为需要输入字符个数
void ShowNUMCharH(Uchar addr,Uchar i,Uchar count);

//下半屏显示连续字串(半宽字符)
void ShowNUMCharL(Uchar addr,Uchar i,Uchar count);

//自定义字符写入CGRAM
//data1是高八位,data2是低八位,一存必须存两个字节,横向存两字节,后纵向累加,共16行
//一个自定义字符为16*16点阵
//第一个存入字节为从40H开始,到4F结束为第一个自定义汉字符, 之后调出地址从8000H为始第一个
//addr为存入头地址
void WRCGRAMH(Uchar data1,Uchar data2,Uchar addr);

//自定义字符写入CGRAM
void WRCGRAML(Uchar data1,Uchar data2,Uchar addr);

//显示上半屏自定义的字符,并把这个字符填满全屏16*16
//addr为显示地址,把自定义字符当一个汉字调出,从8000H开始为第一个,
//8100H为第二个,8200H为第三个,8300H为第四个,中文字库只能自定义四个字符
//i为自定义字符调出地址,先输入低位,再输入高位
//IC决定,中文字库类,一个7920最多只能显示16*2个汉字
void ShowCGCharH(Uchar addr,Uchar i);

//显示下半屏自定义的字符,并把这个字符填满全屏16*16
void ShowCGCharL(Uchar addr,Uchar i);

//上半屏清除图形区数据
void CLEARGDRAMH(Uchar c);

//下半屏清除图形区数据
void CLEARGDRAML(Uchar c);
void WRGDRAM1(Uchar x,Uchar l,Uchar r );

//显示半宽字符及横竖点扫描测试点
void displayTEST (void);

//显示汉字串
void displayCHINSE(void);
void displayCHINSE1(void);
void displayCHINSE2(void);
void displayEngine(void);
void Syswait_ms(int Count);
void Syswait_us(int Count);
#endif
