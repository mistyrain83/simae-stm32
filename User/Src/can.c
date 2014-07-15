
#include "can.h"


#define LED1_OFF()   GPIO_SetBits(GPIOC,GPIO_Pin_1);
#define LED1_ON()  GPIO_ResetBits(GPIOC,GPIO_Pin_1);


/*
 * ��������CAN_GPIO_Config
 * ����  ��CAN GPIO ��ʱ������
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);	                        											 
	
	/* CAN1 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/* Configure CAN pin: RX */									               // PB8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // ��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Configure CAN pin: TX */									               // PB9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // �����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//#define GPIO_Remap_CAN    GPIO_Remap1_CAN1 ��ʵ��û���õ���ӳ��I/O
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
}


static void CAN_Mode_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM=DISABLE;						 // ʱ�䴥��ͨ�Ž�ֹ
	CAN_InitStructure.CAN_ABOM=DISABLE;						 // �����˳������ж���λ��0���˳�
	CAN_InitStructure.CAN_AWUM=DISABLE;						 // �Զ�����ģʽ������sleep
	CAN_InitStructure.CAN_NART=DISABLE;						 // �Զ����´��ͱ��ƣ�֪�����ͳɹ�
	CAN_InitStructure.CAN_RFLM=DISABLE;						 // FIFOû���������±��ĸ��Ǿɱ���  
	CAN_InitStructure.CAN_TXFP=DISABLE;						 // ���ͱ������ȼ�ȷ������־��
	CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;	 // �ػ�ģʽ
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;				 // 1tq��BS1��BS2��ֵ���������й�
	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler=5;             // ��Ƶϵ��Ϊ5
	CAN_Init(CAN1, &CAN_InitStructure);            // ��ʼ��CAN1 
	
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
	TxMessage.StdId=0x11;				// �趨��׼��ʶ����11λ����չ��Ϊ29λ��
	TxMessage.RTR=CAN_RTR_DATA;	// ������Ϣ��֡����Ϊ����֡������Զ��֡��
	TxMessage.IDE=CAN_ID_STD;		// ��Ϣ��־��ʵ���׼��ʶ��
	TxMessage.DLC=2;					  // ������֡��һ֡8λ
	TxMessage.Data[0]=0xCA;			// ��һ֡����
	TxMessage.Data[1]=0xFE;			// �ڶ�֡����
	
	TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
	i = 0;
	// ���ڼ����Ϣ�����Ƿ�����
	while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
	{
		i++;
	}
	
	i = 0;
	// ��鷵�صĹҺŵ���Ϣ��Ŀ
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
	
	/* �жϷ��͵���Ϣ�ͽ��յ���Ϣ�Ƿ���� */
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

