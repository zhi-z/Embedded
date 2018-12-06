#include "stm32f10x.h"
#include "stm32f10x_Init.h"



//******************************************************************************
// FUNCTION: RCC时钟配置
//    PARAM: void
//   RETURN: void
//******************************************************************************
void RCC_Configuration(void)
{
	RCC_APB1PeriphClockCmd((RCC_APB1Periph_SPI2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4), ENABLE);
	
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO), ENABLE);

	return;
}
//******************************************************************************
// FUNCTION: GPID初始化配置
//    PARAM: void
//   RETURN: void
//******************************************************************************
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure SPI2 pins: CSS, SCK, MISO and MOSI ---------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// RC663 GPIO Configure
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;					// RC_RST
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;					// for RC_IRQ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;	// for RC_SEL0, RC_SEL1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// LED1, LED2, LED3, LED4 Configure
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// BEEP Configure
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	return;
}

void TIME_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t PrescalerValue = 0;

	// TIM3 as PWM Beep Out put control
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 2000000) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 800;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIM3, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	//TIM_ARRPreloadConfig(TIM3, ENABLE);

	/* TIM3 enable counter */
	//TIM_Cmd(TIM3, ENABLE);
}
//******************************************************************************
// FUNCTION: NVIC配置
//    PARAM: void
//   RETURN: void
//******************************************************************************
void NVIC_Configuration(void)
{
//	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	/* Enable the USART1 Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);


//	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	return;
}

//******************************************************************************
// FUNCTION: SPI_Configuration
//    PARAM: void
//   RETURN: void
//  COMMENT: Comment
//  		
//******************************************************************************
void SPI_Configuration(void)
{
	SPI_InitTypeDef   SPI_InitStructure;
	
	/* SPI2 Config -------------------------------------------------------------*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);

	
	return;
}

void InitializeSystem(void)
{   
	/* System Clocks Configuration */
	RCC_Configuration();
	
	/* Configure the GPIO ports */
	GPIO_Configuration();
	
	SPI_Configuration();
	
	TIME_Configuration();
	   
	/* NVIC configuration */
	NVIC_Configuration();

	
	return;
}

void BEEP(unsigned char a)
{
	unsigned int iTemp;
	iTemp = GPIOB->CRL;
	iTemp &= 0xFF0FFFFF;
	if (a)
	{
		GPIOB->CRL = iTemp | 0x00B00000;  // PB5 模拟输出		
		TIM_Cmd(TIM3, ENABLE);
	}
	else
	{
		TIM_Cmd(TIM3, DISABLE);
		GPIOB->CRL = iTemp | 0x00400000;  // PB5 漏极开路

		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
	}
	return;
}
