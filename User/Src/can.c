
#include "can.h"


#define LED1_OFF()   GPIO_SetBits(GPIOC,GPIO_Pin_1);
#define LED1_ON()  GPIO_ResetBits(GPIOC,GPIO_Pin_1);


/*
 * 函数名：CAN_GPIO_Config
 * 描述  ：CAN GPIO 和时钟配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);	                        											 
	
	/* CAN1 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/* Configure CAN pin: RX */									               // PB8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // 上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Configure CAN pin: TX */									               // PB9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // 复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//#define GPIO_Remap_CAN    GPIO_Remap1_CAN1 本实验没有用到重映射I/O
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
}


static void CAN_Mode_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM=DISABLE;						 // 时间触发通信禁止
	CAN_InitStructure.CAN_ABOM=DISABLE;						 // 离线退出是在中断置位清0后退出
	CAN_InitStructure.CAN_AWUM=DISABLE;						 // 自动唤醒模式：清零sleep
	CAN_InitStructure.CAN_NART=DISABLE;						 // 自动重新传送豹纹，知道发送成功
	CAN_InitStructure.CAN_RFLM=DISABLE;						 // FIFO没有锁定，新报文覆盖旧报文  
	CAN_InitStructure.CAN_TXFP=DISABLE;						 // 发送报文优先级确定：标志符
	CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;	 // 回环模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;				 // 1tq、BS1、BS2的值跟波特率有关
	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler=5;             // 分频系数为5
	CAN_Init(CAN1, &CAN_InitStructure);            // 初始化CAN1 
	
}


static void CAN_Filter_Config(void)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
		
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	//CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

TestStatus CAN_Polling(void)
{
	CanTxMsg TxMessage;
	CanRxMsg RxMessage;
	uint32_t i = 0;
	uint8_t TransmitMailbox = 0;
	/* transmit */
	TxMessage.StdId=0x11;				// 设定标准标识符（11位，扩展的为29位）
	TxMessage.RTR=CAN_RTR_DATA;	// 传输消息的帧类型为数据帧（还有远程帧）
	TxMessage.IDE=CAN_ID_STD;		// 消息标志符实验标准标识符
	TxMessage.DLC=2;					  // 发送两帧，一帧8位
	TxMessage.Data[0]=0xCA;			// 第一帧数据
	TxMessage.Data[1]=0xFE;			// 第二帧数据
	
	TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
	i = 0;
	// 用于检查消息传输是否正常
	while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
	{
		i++;
	}
	
	i = 0;
	// 检查返回的挂号的信息数目
	while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i != 0xFF))
	{
		i++;
	}
	
	/* receive */
	RxMessage.StdId=0x00;
	RxMessage.IDE=CAN_ID_STD;
	RxMessage.DLC=0;
	RxMessage.Data[0]=0x00;
	RxMessage.Data[1]=0x00;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	
	
	if (RxMessage.StdId!=0x11)
	{
		return FAILED;  
	}
	
	if (RxMessage.IDE!=CAN_ID_STD)
	{
		return FAILED;
	}
	
	if (RxMessage.DLC!=2)
	{
		return FAILED;  
	}
	
	/* 判断发送的信息和接收的信息是否相等 */
	if ((RxMessage.Data[0]<<8|RxMessage.Data[1])!=0xCAFE)
	{
		return FAILED;
	}
	
	//printf("receive data:0X%X,0X%X",RxMessage.Data[0], RxMessage.Data[1]);  
	
	return PASSED; /* Test Passed */
}

void USER_CAN_Init(void)
{
	
	CAN_GPIO_Config();
	CAN_Mode_Config();
	CAN_Filter_Config();
}

void USER_CAN_Test(void)
{
	TestStatus TestRx;
	/* CAN transmit at 100Kb/s and receive by polling in loopback mode */
	TestRx = CAN_Polling();
	
	if (TestRx == FAILED)
	{    
		LED1_OFF();	// LED1 OFF 		
	}
	else
	{    
		LED1_ON();	  // LED1 ON;		
	}
}

