#include "main.h"
#include "stm32f10x_Init.h"
#include "PCD.h"
#include "string.h"
#include "ISO14443.h"
#include "History.h"
#include "usart.h"


//------------------------------------------------------------------------------
// Functions Define
//------------------------------------------------------------------------------
tpd_Card g_sCard;						// 射频卡
unsigned char  g_cBuffer[256];			// 通信缓存
unsigned short g_iLength;

unsigned char  g_cATS[32];
unsigned char  g_cATSLen;

const unsigned char g_cM1KEY[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int snr = 17;    // 扇区号17


//******************************************************************************
//    PARAM: void
//   RETURN: void
//  COMMENT: 主程序
//  		
//******************************************************************************
int main(void)
{
	unsigned char cStatus;
	
	// 初始化系统
	InitializeSystem();
	
	// 初始化设备
	InitDevice();
	
	uart_init(115200);
	
	printf("init..........\r\n\r\n");
	
	do
	{
		printf("running..........\r\n\r\n");
		//卡片复位
		PiccReset();
		
		// ISO14443A寻卡 判断卡类型
		// 选卡，防碰撞，
		// 寻卡
		cStatus = PiccRequest(PICC_REQALL, g_sCard.cATQ);   // PICC_REQALL:寻天线区内全部卡,g_sCard.cATQ:接收卡片类型代码
		if (cStatus != ERR_SUCCESS)
		{
			cStatus = PiccRequest(PICC_REQALL, g_sCard.cATQ);
		}
	
		if (cStatus == ERR_SUCCESS)
		{
			// 选卡，防碰撞
			cStatus = PiccAnticollSelect(&g_sCard.cLen, g_sCard.cUID, &g_sCard.cSAK);
			// 卡休眠
			cStatus = PiccHaltA();		// 正常返回 00
			
			cStatus = PiccRequest(PICC_REQIDL, g_sCard.cATQ);	// 正常返回 FF 无卡
			
			cStatus = PiccRequest(PICC_REQALL, g_sCard.cATQ);	// 正常返回 00 
			if (cStatus == ERR_SUCCESS)
			{
				cStatus = PiccAnticollSelect(&g_sCard.cLen, g_sCard.cUID, &g_sCard.cSAK);
			}
			LED_GRE(ON);
		}
		else
		{
			LED_GRE(OFF);
		}

		// ISO14443-4 Type A 测试流程
		if (cStatus == ERR_SUCCESS)
		{
			if (g_sCard.cSAK & 0x20)
			{
				// ISO14443-4 Type A CPU 测试流程
					// 复位命令
					// 选应用
					// 取随机数
				cStatus = ISO14443A_Rst(&g_cATSLen, g_cATS);
				if (cStatus == ERR_SUCCESS)
				{
//					memcpy(g_cBuffer, "\x00\xA4\x00\x00\x02\x3F\x00", 7);
//					g_iLength = 0x07;
//					cStatus = ISO14443_COS(&g_iLength, g_cBuffer);
					
					memcpy(g_cBuffer, "\x00\x84\x00\x00\x08", 5);
					g_iLength = 0x05;
					cStatus = ISO14443_COS(&g_iLength, g_cBuffer);
					LED_YEL(ON);
				}
				else
				{
					LED_YEL(OFF);
				}
			}
			else
			{
				// Mifare卡 测试流程
					// 认证扇区
					// 读取数据块1
					// 写入数据块1	清空
					// 读取数据块1
					// 写入数据块1	写值
			
				cStatus = PiccAuthState(15*4+3, 0x60, 0x00, (unsigned char *)g_cM1KEY);
				printf("piccAuthState: 0x%x\n", cStatus);
				if (cStatus == ERR_SUCCESS)
				{
					// 读取0扇区，第1块数据
					cStatus = PiccRead(15*4+1, g_cBuffer);
					printf("read 18 sector!\n");
					printf("value = : %s [%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x]\r\n\r\n",
							g_cBuffer,
							g_cBuffer[0],
							g_cBuffer[1],
							g_cBuffer[2],
							g_cBuffer[3],
							g_cBuffer[4],
							g_cBuffer[5],
							g_cBuffer[6],
							g_cBuffer[7],
							g_cBuffer[8],
							g_cBuffer[9],
							g_cBuffer[10],
							g_cBuffer[11],
							g_cBuffer[12],
							g_cBuffer[13],
							g_cBuffer[14],
							g_cBuffer[15]
					);
				}

//				if (cStatus == ERR_SUCCESS)
//				{
//					memset(g_cBuffer, 0xA5, 16);
//					// 写0扇区，第1块数据
//					cStatus = PiccWrite(16*4+1, "123456aaaaabbbbb");
//					
//				}

//				if (cStatus == ERR_SUCCESS)
//				{
//					cStatus = PiccRead(16*6+1, g_cBuffer);
//					printf("read 18 sector!\n");
//					printf("value = :%s\n",g_cBuffer);
//				}

//				if (cStatus == ERR_SUCCESS)
//				{
//					memset(g_cBuffer, 0x00, 16);
//					cStatus = PiccWrite(16*4+1, "123456aaaaabbbbb");
//				}
				
				
			}
		}
		else
		{
			LED_YEL(OFF);
			LED_BLU(OFF);
		}
		
		// ISO14443-4 Type B 测试流程
		
		cStatus = IDCard_Reqest(&g_iLength, g_cATS);
		
		if (cStatus == ERR_SUCCESS)
		{
			g_iLength = sizeof("\x00\x36\x00\x00\x08") - 1;
			memcpy(g_cBuffer, "\x00\x36\x00\x00\x08", g_iLength);
			cStatus = IDCard_APDU(&g_iLength, g_cBuffer);
			
			if (cStatus == ERR_SUCCESS)
			{
				LED_GRE(ON);
				printf("read finish!\n");
				printf("读到的值是:%s\n",g_cBuffer);
			}
		}
		else
		{
			LED_GRE(OFF);
		}


	} while (1);
}
//******************************************************************************
//	 	IN: void
//	RETURN: void
// Comment: 
//
//******************************************************************************
void InitDevice(void)
{
	unsigned char cStatus;
	LED_RED(ON);
	LED_GRE(ON);
	LED_YEL(ON);
	LED_BLU(ON);
	BEEP(ON);
	Delay100uS(2000);		// 延时200ms
	LED_RED(OFF);
	LED_GRE(OFF);
	LED_YEL(OFF);
	LED_BLU(OFF);
	BEEP(OFF);
	
	// LED_RED:	射频正常启动亮
	// LED_GRE:	有卡指示灯，单独亮为M1卡
	// LED_YEL:	Type A CPU Card
	// LED_BLU:	Type B CPU Card
	


	RC_SEL0(OFF);
	RC_SEL1(ON);			// 设置射频芯片为SPI模式
	
	cStatus = PcdReset();   // 复位并初始化RC663
	if (cStatus == ERR_SUCCESS)
	{
		PcdConfig(PCD_TYPE_14443A);
		LED_RED(ON);		// 射频初始化成功，RED LED 点亮。
	}

	return;
}

//******************************************************************************
//	 	IN: unsigned int iTimes
//	RETURN: void
// Comment: 使用Timer4做等待延时,单位时间100us
//
//******************************************************************************
void Delay100uS(unsigned int iTimes)
{	
	Timer4_Init(TIME_UNIT_100US);
	
	Timer4_Wait(iTimes);
	
	Timer4_Stop();
	return;
}

void Timer4_Init(unsigned char cMode)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
	uint16_t PrescalerValue;
	uint16_t PeriodValue;
	
	switch (cMode)
	{
		case TIME_UNIT_100US:
			PrescalerValue = 0;				// 系统时钟1分频
			PeriodValue    = SystemCoreClock/10000;
			break;
		
		case TIME_UNIT_1MS:
			PrescalerValue = 1;				// 系统时钟2分频
			PeriodValue    = SystemCoreClock/2000;
			break;
		
		case TIME_UNIT_200MS:
			PrescalerValue = 575;			// 系统时钟576分频
			PeriodValue    = SystemCoreClock/2880;
			break;
		
		default:
			break;
	}

	TIM_TimeBaseStructure.TIM_Period = PeriodValue;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_SetCounter(TIM4, 0x0000);
	TIM_ITConfig(TIM4, TIM_IT_Update , ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	return;
}
void Timer4_Wait(unsigned short sTime)
{
	do
	{
		sTime--;
		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		while(TIM_GetITStatus(TIM4, TIM_IT_Update) == RESET);
		
	} while (sTime);
	return;
}
void Timer4_Stop(void)
{
	TIM_Cmd(TIM4, DISABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update , DISABLE);
	return;
}


