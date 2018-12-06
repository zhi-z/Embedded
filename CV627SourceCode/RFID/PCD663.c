#include "main.h"
#include "PCD.h"

#ifdef _PCD663_

//#include "String.h"


tpd_pcdpara g_sPcdPara;
tpd_pcdtrsv g_sPcdTrsv;


//******************************************************************************
// Function: 复位并初始化RC663
//		 IN: void
//		RET: unsigned char 		= MI_OK
//                     			= MI_RESETERR
//******************************************************************************
unsigned char PcdReset(void)
{
	unsigned char cStatus = MI_OK;
	unsigned char n;
	unsigned int i = 0xFFFF;

	RC_RST(ON);				// 高电平复位
	Delay100uS(50);
	RC_RST(OFF);			// 低电平运行

	do {
		n = ReadRawRC(0x00);
		i--;
	} while (i != 0 && n != 0x40);

	if (i == 0)
	{
		cStatus = MI_RESETERR;     // 复位错误
	}
	else
	{
		n = ReadRawRC(PHHAL_HW_RC663_REG_VERSION);  // 寄存器读写测试
		
	}
	return cStatus;
}

//******************************************************************************
// Function: 读寄存器,指定寄存器地址,返回寄存器数据,无返回值.
//		 IN: unsigned char Address
//		RET: unsigned char
//******************************************************************************
unsigned char ReadRawRC(unsigned char cAddress)
{
	unsigned short iTemp = 0;
	
	RC_NSS(OFF);
	
	iTemp  = (cAddress << 9) | 0x0100;
	
	while ((SPI2->SR & SPI_I2S_FLAG_TXE) == RESET);		// while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);,等待发射区空
	SPI2->DR = iTemp;									// SPI_I2S_SendData(SPI2, iTemp); 通过外设 SPIx 发送一个数据
	
	while ((SPI2->SR & SPI_I2S_FLAG_RXNE) == RESET);	// while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);等待接收完一个byte
	iTemp = SPI2->DR;									// iTemp = SPI_I2S_ReceiveData(SPI2); 读取数据
	
	RC_NSS(ON);

	return (unsigned char)(iTemp);
}
//******************************************************************************
//	 	IN:	unsigned char cAddress
//			unsigned char cValue
//	   RET: void
// Comment: 写寄存器,指定寄存器地址,输出数据,
//******************************************************************************
void WriteRawRC(unsigned char cAddress, unsigned char cValue)
{
	unsigned short iTemp;
	RC_NSS(OFF);

	iTemp  = (cAddress << 9) | cValue;
	iTemp &= 0xFEFF;

	while ((SPI2->SR & SPI_I2S_FLAG_TXE) == RESET);		// while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);	// 等待状态位	
	SPI2->DR = iTemp;									// SPI_I2S_SendData(SPI2, iTemp);
	
	while ((SPI2->SR & SPI_I2S_FLAG_RXNE) == RESET);	// while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	iTemp = SPI2->DR;									// iTemp = SPI_I2S_ReceiveData(SPI2);
	
	RC_NSS(ON);
	return;
}

//******************************************************************************
//       IN: unsigned char reg
//           unsigned char mask
//      OUT: void
//   RETURN: void
//******************************************************************************
void SetBitMask(unsigned char reg,unsigned char mask)
{
	unsigned char tmp = ReadRawRC(reg);
	WriteRawRC(reg, tmp | mask);		// set bit mask
	return;
}
//******************************************************************************
//       IN: unsigned char reg
//           unsigned char mask
//      OUT: void
//   RETURN: void
//  COMMENT: 清RC500寄存器位
//******************************************************************************
void ClearBitMask(unsigned char reg,unsigned char mask)
{
	unsigned char tmp = ReadRawRC(reg);
	WriteRawRC(reg, tmp & ~mask);		// clear bit mask
	return;
}


//******************************************************************************
//       IN: unsigned char tmoLength
//      OUT: void
//   RETURN: void
//  COMMENT: 设置RC663定时器
//******************************************************************************
void PcdSetTmo(unsigned char cFWI)
{
	unsigned int cClock0 = 4096;
	unsigned int cClock1;
	unsigned char cTpcd = 2;

	// FWT+ΔFWT+ΔTpcd		// ΔFWT = 49152; ΔTpcd = 16.4ms = 222384;
	// cClock1 = (FWT+ΔFWT+ΔTpcd) / 4096;
	
	cClock1 = (1<<cFWI) + 12 + cTpcd;	// 计算方法

	WriteRawRC(PHHAL_HW_RC663_REG_T0RELOADHI,(unsigned char)(cClock0>>8));
	WriteRawRC(PHHAL_HW_RC663_REG_T0RELOADLO,(unsigned char)cClock0);
	WriteRawRC(PHHAL_HW_RC663_REG_T1RELOADHI,(unsigned char)(cClock1>>8));
	WriteRawRC(PHHAL_HW_RC663_REG_T1RELOADLO,(unsigned char)cClock1);
//	WriteRawRC(PHHAL_HW_RC663_REG_T0CONTROL, 0x98);		// 时钟源为13.56
//	WriteRawRC(PHHAL_HW_RC663_REG_T1CONTROL, 0x9A);		// 时钟源为Timer0
	WriteRawRC(PHHAL_HW_RC663_REG_TCONTROL,0x33);   //PHHAL_HW_RC663_REG_TCONTROL:Timer Registers
	
	return;
}
//******************************************************************************
//    PARAM: unsigned char RxPtl
//			 unsigned char TxPtl
//   RETURN: void
//  COMMENT: CLRC663 Datasheet 7.10.3.13 LoadProtocol command; Table 45;
//  		
//******************************************************************************
unsigned char phhalHw_Rc663_Cmd_LoadProtocol(unsigned char RxPtl, unsigned char TxPtl)
{
	unsigned char cStatus = MI_OK;
	
	PcdSetTmo(0);  // 设置RC663定时器

	g_sPcdTrsv.cData[0] = RxPtl;
	g_sPcdTrsv.cData[1] = TxPtl;
  g_sPcdTrsv.iLength = 2;   // 数据长度

	//PHHAL_HW_RC663_CMD_LOADPROTOCOL(0x0DU):从EEPROM读取数据并初始化协议更改所需的寄存器。
	cStatus = PcdTransceive(PHHAL_HW_RC663_CMD_LOADPROTOCOL); 
	
	return cStatus;
}
//******************************************************************************
//      IN: unsigned char cType
//  RETURN: void
// COMMENT: 配置基站操作卡类型

//******************************************************************************
unsigned char PcdConfig(unsigned char cType)
{
	unsigned char cStatus = ERR_FAILURE;

	if (cType == PCD_TYPE_14443A)			//Config Type A
	{
//		WriteRawRC(PHHAL_HW_RC663_REG_TCONTROL,0x30);
//		WriteRawRC(PHHAL_HW_RC663_REG_FIFOCONTROL, 0X10);
		
		//--------------- LoadProtocol( bTxProtocol=0, bRxProtocol=0)
		cStatus = phhalHw_Rc663_Cmd_LoadProtocol(0x00, 0x00);	// 570us
		if (cStatus != 0x00)
		{
			// 当前次的寻卡失败后，协议配置可能失败，在此重新配置，此配置有效，但是会返回失败。B卡相同。
			cStatus = phhalHw_Rc663_Cmd_LoadProtocol(0x00, 0x00);
		}

		//----------Apply RegisterSet
		// 写一个寄存器时间3.3us   PHHAL_HW_RC663_REG_T0CONTROL:Timer0 Control Register
		WriteRawRC(PHHAL_HW_RC663_REG_T0CONTROL, 0X98); 		// Input clock is 13.56MHz
		WriteRawRC(PHHAL_HW_RC663_REG_T1CONTROL, 0X92); 		// Input clock is Timer0
//		WriteRawRC(PHHAL_HW_RC663_REG_T2CONTROL, 0X20);
//		WriteRawRC(PHHAL_HW_RC663_REG_T2RELOADHI, 0X03);
//		WriteRawRC(PHHAL_HW_RC663_REG_T2RELOADLO, 0XFF);
		
		WriteRawRC(PHHAL_HW_RC663_REG_FIFOCONTROL, 0x10);			// confirmed
		WriteRawRC(PHHAL_HW_RC663_REG_WATERLEVEL, _WATER_LEVEL_);	// confirmed
		WriteRawRC(PHHAL_HW_RC663_REG_RXBITCTRL, 0x88);				// confirmed
		WriteRawRC(PHHAL_HW_RC663_REG_DRVMODE, 0X8D);
		WriteRawRC(PHHAL_HW_RC663_REG_TXAMP, 0X80);			// 
		WriteRawRC(PHHAL_HW_RC663_REG_DRVCON, 0X01);
		WriteRawRC(PHHAL_HW_RC663_REG_TXI, 0x05);
		WriteRawRC(PHHAL_HW_RC663_REG_RXSOFD, 0X00);
		WriteRawRC(PHHAL_HW_RC663_REG_RCV, 0X12);
////////////////////////////////////////////////////////////////////////////////////////////////
		WriteRawRC(PHHAL_HW_RC663_REG_TXCRCCON, 0x18);		
		WriteRawRC(PHHAL_HW_RC663_REG_RXCRCCON, 0x18); 
		WriteRawRC(PHHAL_HW_RC663_REG_TXDATANUM, 0x08);
		WriteRawRC(PHHAL_HW_RC663_REG_TXMODWIDTH, 0x27);
		WriteRawRC(PHHAL_HW_RC663_REG_TXSYM10BURSTLEN, 0x00);
		WriteRawRC(PHHAL_HW_RC663_REG_TXWAITCTRL, 0xc0);
		WriteRawRC(PHHAL_HW_RC663_REG_TXWAITLO, 0x12);
		WriteRawRC(PHHAL_HW_RC663_REG_FRAMECON, 0xCF);
		WriteRawRC(PHHAL_HW_RC663_REG_RXCTRL, 0x04);
		WriteRawRC(PHHAL_HW_RC663_REG_RXTHRESHOLD, 0x7F);	// tested value: 0x7F; recommanded value: 0x32
		WriteRawRC(PHHAL_HW_RC663_REG_RXANA, 0x0F);			// tested value: 0x0F; recommanded value: 0x0A
		WriteRawRC(PHHAL_HW_RC663_REG_RXWAIT, 0x90);     
		
		//WriteRawRC(PHHAL_HW_RC663_REG_SIGOUT, 0x19);
		

//		WriteRawRC(PHHAL_HW_RC663_REG_DRVMODE, 0x82);
//		WriteRawRC(PHHAL_HW_RC663_REG_STATUS, 0x00);
//		WriteRawRC(PHHAL_HW_RC663_REG_DRVMODE, 0x8A);	//FieldOn

	}
	else if (cType == PCD_TYPE_14443B)		// Config Type B
	{
		//----- LoadProtocol( bTxProtocol=04, bRxProtocol=04)
		cStatus = phhalHw_Rc663_Cmd_LoadProtocol(0x04, 0x04);
		if (cStatus != 0x00)
		{
			cStatus = phhalHw_Rc663_Cmd_LoadProtocol(0x04, 0x04);
		}

		//----------Apply RegisterSet
		WriteRawRC(PHHAL_HW_RC663_REG_T0CONTROL, 0X98); 	// Input clock is 13.56MHz
		WriteRawRC(PHHAL_HW_RC663_REG_T1CONTROL, 0X92); 	// Input clock is Timer0
//		WriteRawRC(PHHAL_HW_RC663_REG_T2CONTROL, 0X20);
//		WriteRawRC(PHHAL_HW_RC663_REG_T2RELOADHI, 0X03);
//		WriteRawRC(PHHAL_HW_RC663_REG_T2RELOADLO, 0XFF);
		
		WriteRawRC(PHHAL_HW_RC663_REG_FIFOCONTROL, 0X10);
		WriteRawRC(PHHAL_HW_RC663_REG_WATERLEVEL, _WATER_LEVEL_);
		WriteRawRC(PHHAL_HW_RC663_REG_RXBITCTRL, 0X80);
		WriteRawRC(PHHAL_HW_RC663_REG_DRVMODE, 0X8F);
		WriteRawRC(PHHAL_HW_RC663_REG_TXAMP, 0X8F);			//  0xCF:	0CM 13,8  4CM 11.2
		WriteRawRC(PHHAL_HW_RC663_REG_DRVCON, 0X01);
		WriteRawRC(PHHAL_HW_RC663_REG_TXI, 0X05);
		WriteRawRC(PHHAL_HW_RC663_REG_RXSOFD, 0X00);
		WriteRawRC(PHHAL_HW_RC663_REG_RCV, 0X12);

////////////////////////////////////////////////////////////////////////////////////////////////
		WriteRawRC(PHHAL_HW_RC663_REG_TXCRCCON, 0x7B);
		WriteRawRC(PHHAL_HW_RC663_REG_RXCRCCON, 0x7B); 
		WriteRawRC(PHHAL_HW_RC663_REG_TXDATANUM, 0x08);
		WriteRawRC(PHHAL_HW_RC663_REG_TXMODWIDTH, 0x00);
		WriteRawRC(PHHAL_HW_RC663_REG_TXSYM10BURSTLEN, 0x00);
		WriteRawRC(PHHAL_HW_RC663_REG_TXWAITCTRL, 0x01);
		//WriteRawRC(PHHAL_HW_RC663_REG_TXWAITLO, 0x0B);
		WriteRawRC(PHHAL_HW_RC663_REG_FRAMECON, 0x05);
		WriteRawRC(PHHAL_HW_RC663_REG_RXSOFD, 0x00);
		WriteRawRC(PHHAL_HW_RC663_REG_RXCTRL, 0x34);
		WriteRawRC(PHHAL_HW_RC663_REG_RXTHRESHOLD, 0x7F);		//tested value: 0x7F; recommanded value: 0x3F
		WriteRawRC(PHHAL_HW_RC663_REG_RXANA, 0x06);				//tested value: 0x06; recommanded value: 0x0A
		WriteRawRC(PHHAL_HW_RC663_REG_RXWAIT, 0x9F);

//		WriteRawRC(PHHAL_HW_RC663_REG_DRVMODE, 0x8F); 	//FieldOn

	}
	else
	{
		return 0xFF;
	}
	Delay100uS(30);
	return cStatus;
}
//******************************************************************************
//       IN: unsigned char cCommand
//   RETURN: unsigned char
//  COMMENT: PCD and PICC transcive
//******************************************************************************
unsigned char PcdTransceive(unsigned char cCommand)
{
	unsigned char recebyte = 0;
	unsigned char cStatus = 0x01;
	unsigned char irq0En = 0x00;
	unsigned char irq1En = 0x00;
	unsigned char cIrq0waitFor;
	unsigned char cIrq1waitFor;
	unsigned char n1, n2, j;
	unsigned short i;
	unsigned char cError;

	n2 = 0x00;
	cIrq0waitFor = 0x00;
	cIrq1waitFor = 0x00;

	switch (cCommand)
	{
		case PHHAL_HW_RC663_CMD_WRITEE2:
			irq0En = 0x10;
			irq1En = 0x40;
			cIrq0waitFor = 0x00;
			cIrq1waitFor = 0x40;
			break;
		case PHHAL_HW_RC663_CMD_READE2:
			irq0En = 0x10;
			irq1En = 0x40;
			cIrq0waitFor = 0x00;
			cIrq1waitFor = 0x40;
			recebyte=1;
			break;
		case PHHAL_HW_RC663_CMD_LOADPROTOCOL:
			irq0En = 0x10;   // 设置中断使能寄存器
			irq1En = 0x42;
			cIrq0waitFor = 0x10;
			cIrq1waitFor = 0x02;
			break;
		case PHHAL_HW_RC663_CMD_LOADKEY:
			irq0En = 0x18;
			irq1En = 0x42;
			cIrq0waitFor = 0x00;
			cIrq1waitFor = 0x40;
			break;
		case PHHAL_HW_RC663_CMD_MFAUTHENT:
			irq0En = 0x18;//0x14;
			irq1En = 0x42;
			cIrq0waitFor = 0x00;
			cIrq1waitFor = 0x40;
			break;
		case PHHAL_HW_RC663_CMD_TRANSMIT:
			irq0En = 0x18;//0x0A;
			irq1En = 0x42;
			cIrq0waitFor = 0x08;
			cIrq1waitFor = 0x42;
			break;
		case PHHAL_HW_RC663_CMD_TRANSCEIVE:
			irq0En = 0x14;
			irq1En = 0x42;
			cIrq0waitFor = 0x10;
			cIrq1waitFor = 0x02;
			recebyte=1;
			break;
		default:
			cCommand = MI_UNKNOWN_COMMAND;
			break;
	}
	if (cCommand != MI_UNKNOWN_COMMAND)
	{
		WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_IDLE);
		WriteRawRC(PHHAL_HW_RC663_REG_IRQ0, 0x7F);		//clear all IRQ0 flags   0111 1111 ==>>0:writing a 1 to a bit position 6~0 clears the interrupt request
		WriteRawRC(PHHAL_HW_RC663_REG_IRQ1, 0x7F);		//clear all IRQ1 flags
		WriteRawRC(PHHAL_HW_RC663_REG_IRQ0EN, 0x00);	//clear all IRQ0 source
		WriteRawRC(PHHAL_HW_RC663_REG_IRQ1EN, 0x00);	//clear all IRQ1 source 
		WriteRawRC(PHHAL_HW_RC663_REG_IRQ0EN, irq0En);
		WriteRawRC(PHHAL_HW_RC663_REG_IRQ1EN, irq1En);
		SetBitMask(PHHAL_HW_RC663_REG_FIFOCONTROL, 0x10);	//FlushFIFO();
		for (i=0; i<g_sPcdTrsv.iLength; i++)
		{  
			WriteRawRC(PHHAL_HW_RC663_REG_FIFODATA, g_sPcdTrsv.cData[i]);   //FIFO-Data Register
		}
		WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, cCommand);   //Command Register

		i =50000;
		do
		{
			n1 = ReadRawRC(PHHAL_HW_RC663_REG_IRQ0);  //PHHAL_HW_RC663_REG_IRQ0：IRQ Registers
			n2 = ReadRawRC(PHHAL_HW_RC663_REG_IRQ1);  //PHHAL_HW_RC663_REG_IRQ1: IRQ1 Register
			i--;
		} while ((i != 0) && !(n1 & cIrq0waitFor) && !(n2 & cIrq1waitFor));
		
		if ((i != 0) && !(n2 & 0x02))
		{
			cError = ReadRawRC(PHHAL_HW_RC663_REG_ERROR);  //Error Register.
			if(0 == cError)
			{
				cStatus = MI_OK;
			}
			else if (cError & 0x02)
			{
				cStatus = MI_FRAMINGERR;
			}
			else if (cError & 0x04)
			{
				cStatus = MI_COLLERR;   // 碰撞错误
			}
			else if (cError & 0x01)
			{
				cStatus = MI_CRCERR;
			}
			else
			{
				cStatus = MI_FATAL_ERR;
			}
			if (recebyte)
			{
				n1 = ReadRawRC(PHHAL_HW_RC663_REG_FIFOLENGTH);
				n2 = ReadRawRC(PHHAL_HW_RC663_REG_RXBITCTRL) & 0x07;
				if (n2 != 0)
				{
					g_sPcdTrsv.iLength = (n1 - 1) * 8 + n2;
				}
				else
				{
					g_sPcdTrsv.iLength = n1;
				}

				for (j = 0; j < n1; j++)
				{// 读取队列寄存器中的数据
					g_sPcdTrsv.cData[j] = ReadRawRC(PHHAL_HW_RC663_REG_FIFODATA);
				}
			}
		}
		else if (n2 & 0x02)
		{
			cStatus = MI_NOTAGERR;
		}
		else
		{
			cStatus = MI_COM_ERR;
		}
	}
	WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_IDLE);
	WriteRawRC(PHHAL_HW_RC663_REG_IRQ0, 0x7F);		//clear all IRQ0 flags
	WriteRawRC(PHHAL_HW_RC663_REG_IRQ1, 0x7F); 		//clear all IRQ1 flags
	WriteRawRC(PHHAL_HW_RC663_REG_IRQ0EN, 0x00);	//clear all IRQ0 source
	WriteRawRC(PHHAL_HW_RC663_REG_IRQ1EN, 0x00); 	//clear all IRQ1 source 
	WriteRawRC(PHHAL_HW_RC663_REG_TCONTROL, 0x33);		// stop timer now
	SetBitMask(PHHAL_HW_RC663_REG_T0CONTROL, 0x80);	// set timer auto stop
	SetBitMask(PHHAL_HW_RC663_REG_T1CONTROL, 0x80);	// set timer auto stop
	
	Delay100uS(5);			// FDT PCD,MIN
	return cStatus;
}

// M531PiccRequestB:	单字节无CRC回复，要求有返回数据。

// CRC，Framing, Parity错误，清空FIFO,继续接收，直到超时。

// 返回: 	MI_OK
//			MI_FATAL_ERR
//			MI_NOTAGERR
//			MI_COM_ERR
//unsigned char PcdComTransceiveAPDU(void)
//{
//	unsigned char recebyte = 0;
//	unsigned char cStatus = 0x00;
//	unsigned char irq0En = 0x00;
//	unsigned char irq1En = 0x00;
////	unsigned char cIrq0waitFor;
////	unsigned char cIrq1waitFor;
//	unsigned char n1, n2, j;
//	unsigned char k, n;
//	unsigned long lCnt;
//	unsigned char cError;
//	unsigned short sDataCnt;
//	unsigned short sFIFOLength;
//	unsigned short sCnt;
//	unsigned char cRxWait;

//	n2 = 0x00;
////	cIrq0waitFor = 0x00;
////	cIrq1waitFor = 0x00;

//	switch (g_sPcdTrsv.cCommand)
//	{
//		case PHHAL_HW_RC663_CMD_TRANSCEIVE:
//			irq0En = 0x14;
//			irq1En = 0x42;
////			cIrq0waitFor = 0x10;
////			cIrq1waitFor = 0x02;
//			recebyte=1;
//			break;
//		default:
//			g_sPcdTrsv.cCommand = MI_UNKNOWN_COMMAND;
//			break;
//	}
//	if (g_sPcdTrsv.cCommand != MI_UNKNOWN_COMMAND)
//	{
//		cRxWait = ReadRawRC(PHHAL_HW_RC663_REG_RXWAIT);
//		WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_IDLE);
//		WriteRawRC(PHHAL_HW_RC663_REG_IRQ0, 0x7F);		//clear all IRQ0 flags
//		WriteRawRC(PHHAL_HW_RC663_REG_IRQ1, 0x7F);		//clear all IRQ1 flags
//		WriteRawRC(PHHAL_HW_RC663_REG_IRQ0EN, 0x00);	//clear all IRQ0 source
//		WriteRawRC(PHHAL_HW_RC663_REG_IRQ1EN, 0x00);	//clear all IRQ1 source 
//		WriteRawRC(PHHAL_HW_RC663_REG_IRQ0EN, irq0En);
//		WriteRawRC(PHHAL_HW_RC663_REG_IRQ1EN, irq1En);
//		SetBitMask(PHHAL_HW_RC663_REG_FIFOCONTROL, 0x10);	// FlushFIFO();
//		ClearBitMask(PHHAL_HW_RC663_REG_T0CONTROL, 0x80);	// set timer auto start but no stop
//		ClearBitMask(PHHAL_HW_RC663_REG_T1CONTROL, 0x80);	// set timer auto start but no stop
////		SetBitMask(PHHAL_HW_RC663_REG_RXCTRL, 0x08);		// Enables the EMD suppression
//		
//		j = (g_sPcdTrsv.iLength < _WATER_LEVEL_) ? g_sPcdTrsv.iLength : _WATER_LEVEL_;
//		for(sDataCnt = 0; sDataCnt <j; sDataCnt++)
//		{
//			WriteRawRC(PHHAL_HW_RC663_REG_FIFODATA, g_sPcdTrsv.cData[sDataCnt]);
//		}
//		WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, g_sPcdTrsv.cCommand);
//		for(; sDataCnt < g_sPcdTrsv.iLength; sDataCnt++)
//		{
//			do
//			{
//				n1 = ReadRawRC(PHHAL_HW_RC663_REG_FIFOLENGTH);
//			} while (n1 > _WATER_LEVEL_);
//			WriteRawRC(PHHAL_HW_RC663_REG_FIFODATA, g_sPcdTrsv.cData[sDataCnt]);
//		}
//		do
//		{
//			n1 = ReadRawRC(PHHAL_HW_RC663_REG_STATUS);
//		} while ((n1 & 0x07) > 0x00 && (n1 & 0x07) < 0x06 );

//		lCnt =0x200000;
//		sDataCnt = 0;
//		do
//		{
//			j = ReadRawRC(PHHAL_HW_RC663_REG_STATUS);
//			if ((j & 0x07) == 0)
//			{
//				k = ReadRawRC(PHHAL_HW_RC663_REG_RXBITCTRL);
//				if (0 != (k & 0x07))
//				{
//					k = ReadRawRC(PHHAL_HW_RC663_REG_FIFOLENGTH);
//					if (k <= 4)
//					{
//						WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_IDLE); //??
//						WriteRawRC(PHHAL_HW_RC663_REG_RXWAIT, 0x88);				// set no RxWait
//						SetBitMask(PHHAL_HW_RC663_REG_FIFOCONTROL, 0x10);	//FlushFIFO();
//						WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_RECEIVE);	// start receive
//						sDataCnt = 0;
//						do
//						{
//							n = ReadRawRC(PHHAL_HW_RC663_REG_STATUS);
//						} while ((n & 0x07) > 0x00 && (n & 0x07) < 0x06 );
//					}
//					else
//					{
//						WriteRawRC(PHHAL_HW_RC663_REG_TCONTROL, 0x33);		//
//						cStatus = MI_RESIDUAL_BIT_ERR;
//						break;
//					}
//				}
//				else
//				{
//					j = ReadRawRC(PHHAL_HW_RC663_REG_ERROR);
//					k = ReadRawRC(PHHAL_HW_RC663_REG_FIFOLENGTH);
//					if (0 != (j & 0x01))
//					{
//						if (k < 4)
//						{
//							WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_IDLE); //??
//							WriteRawRC(PHHAL_HW_RC663_REG_RXWAIT, 0x88);				// set no RxWait
//							SetBitMask(PHHAL_HW_RC663_REG_FIFOCONTROL, 0x10);	//FlushFIFO();
//							WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_RECEIVE);	// start receive
//							sDataCnt = 0;
//							do
//							{
//								n = ReadRawRC(PHHAL_HW_RC663_REG_STATUS);
//							} while ((n & 0x07) > 0x00 && (n & 0x07) < 0x06 );
//						}
//						else
//						{
//							WriteRawRC(PHHAL_HW_RC663_REG_TCONTROL, 0x33);		//
//							cStatus = MI_CRCERR;
//							break;
//						}
//					}
//					else if (0 != (j & 0x02))
//					{
//						if (k < 4)
//						{
//							WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_IDLE); //??
//							WriteRawRC(PHHAL_HW_RC663_REG_RXWAIT, 0x88);				// set no RxWait
//							SetBitMask(PHHAL_HW_RC663_REG_FIFOCONTROL, 0x10);	//FlushFIFO();
//							WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_RECEIVE);	// start receive
//							sDataCnt = 0;
//							do
//							{
//								n = ReadRawRC(PHHAL_HW_RC663_REG_STATUS);
//							} while ((n & 0x07) > 0x00 && (n & 0x07) < 0x06 );

//						}
//						else
//						{
//							WriteRawRC(PHHAL_HW_RC663_REG_TCONTROL, 0x33);		//
//							cStatus = MI_FRAMINGERR;
//							break;
//						}
//					}
//					else if (0 != (j & 0x10))
//					{
//						if (k < 4)
//						{
//							WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_IDLE); //??
//							WriteRawRC(PHHAL_HW_RC663_REG_RXWAIT, 0x88);				// set no RxWait
//							SetBitMask(PHHAL_HW_RC663_REG_FIFOCONTROL, 0x10);	//FlushFIFO();
//							WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_RECEIVE);	// start receive
//							sDataCnt = 0;
//							do
//							{
//								n = ReadRawRC(PHHAL_HW_RC663_REG_STATUS);
//							} while ((n & 0x07) > 0x00 && (n & 0x07) < 0x06 );

//						}
//						else
//						{
//							WriteRawRC(PHHAL_HW_RC663_REG_TCONTROL, 0x33);		//
//							cStatus = MI_FRAMINGERR;
//							break;
//						}
//					}
//					else
//					{
//						WriteRawRC(PHHAL_HW_RC663_REG_TCONTROL, 0x33);		// transaction finished, stop timer now
//						cStatus = MI_OK;
//						break;
//					}
//				}
//			}
//			else if ((j & 0x07) == 0x07)
//			{
//				k = ReadRawRC(PHHAL_HW_RC663_REG_FIFOLENGTH);
//				if (k > _WATER_LEVEL_)
//				{
//					if (sDataCnt <= _READER_FSD_ - _WATER_LEVEL_)
//					{
//						for (j=0; j < _WATER_LEVEL_; j++)
//						{
//							g_sPcdTrsv.cData[sDataCnt++] = ReadRawRC(PHHAL_HW_RC663_REG_FIFODATA);
//						}
//					}
//					else
//					{
//						cStatus = MI_FATAL_ERR;
//						break;
//					}
//				}
//			}
//			else
//			{
//				k = ReadRawRC(PHHAL_HW_RC663_REG_TCONTROL);
//				if (0 == (k & 0x20))
//				{
//					j = ReadRawRC(PHHAL_HW_RC663_REG_STATUS);
//					if ((j & 0x07) == 0x06)
//					{
//						cStatus = MI_NOTAGERR;
//						break;
//					}
//				}
//			}
//			lCnt--;
//		}
//		while (lCnt != 0);

//		if (MI_OK == cStatus)
//		{
//			if (0 == lCnt)
//			{
//				cStatus = MI_COM_ERR;
//			}
//			else
//			{
//				cError = ReadRawRC(PHHAL_HW_RC663_REG_ERROR);
//				if(0 == cError)
//				{
//					cStatus = MI_OK;
//				}
//				else if (cError & 0x02)
//				{
//					cStatus = MI_FRAMINGERR;
//				}
//				else if (cError & 0x04)
//				{
//					cStatus = MI_COLLERR;
//				}
//				else if (cError & 0x01)
//				{
//					cStatus = MI_CRCERR;
//				}
//				else if (cError & 0x10)
//				{
//					cStatus = MI_FRAMINGERR;
//				}
//				else
//				{
//					cStatus = MI_FATAL_ERR;
//				}
//				if (recebyte)
//				{
//					sFIFOLength = (0x03 & ReadRawRC(PHHAL_HW_RC663_REG_FIFOCONTROL)) * 256 + ReadRawRC(PHHAL_HW_RC663_REG_FIFOLENGTH);
//					n2 = ReadRawRC(PHHAL_HW_RC663_REG_RXBITCTRL) & 0x07;
//					if (n2 != 0)
//					{
//						g_sPcdTrsv.iLength = (sFIFOLength + sDataCnt - 1) * 8 + n2;
//					}
//					else
//					{
//						g_sPcdTrsv.iLength = (sFIFOLength + sDataCnt) * 8;
//					}
//					if (sDataCnt + sFIFOLength <= _READER_FSD_)
//					{
//						for (sCnt = 0; sCnt < sFIFOLength; sCnt++)
//						{
//							g_sPcdTrsv.cData[sDataCnt++] = ReadRawRC(PHHAL_HW_RC663_REG_FIFODATA);
//						}
//					}
//					else
//					{
//						cStatus = MI_FATAL_ERR;
//					}
//				}
//			}
//		}
//		WriteRawRC(PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_IDLE);
//		WriteRawRC(PHHAL_HW_RC663_REG_IRQ0, 0x7F);		//clear all IRQ0 flags
//		WriteRawRC(PHHAL_HW_RC663_REG_IRQ1, 0x7F); 		//clear all IRQ1 flags
//		WriteRawRC(PHHAL_HW_RC663_REG_IRQ0EN, 0x00);	//clear all IRQ0 source
//		WriteRawRC(PHHAL_HW_RC663_REG_IRQ1EN, 0x00); 	//clear all IRQ1 source 
//		WriteRawRC(PHHAL_HW_RC663_REG_TCONTROL, 0x33);		// stop timer now
//		SetBitMask(PHHAL_HW_RC663_REG_T0CONTROL, 0x80);	// set timer auto stop
//		SetBitMask(PHHAL_HW_RC663_REG_T1CONTROL, 0x80);	// set timer auto stop
//		WriteRawRC(PHHAL_HW_RC663_REG_RXWAIT, cRxWait);
//	//	ClearBitMask(PHHAL_HW_RC663_REG_RXCTRL, 0x08);	// Disables the EMD suppression
//	}
//	else
//	{
//		cStatus = MI_COM_ERR;
//	}
//	Delay100uS(5);			// FDT PCD,MIN
//	return cStatus;
//}
//******************************************************************************
//      IN: unsigned short iTime
//  RETURN: void
// COMMENT: 
//  		
//******************************************************************************
void M500PcdRfReset(unsigned short iTime)
{
	if (iTime != 0)
	{
		ClearBitMask(PHHAL_HW_RC663_REG_DRVMODE, 0x08);
		Delay100uS(iTime * 10);            // Delay for 1 ms
		SetBitMask(PHHAL_HW_RC663_REG_DRVMODE, 0x08);
	}
	else
	{
		ClearBitMask(PHHAL_HW_RC663_REG_DRVMODE, 0x08);
	}
	return;
}
//******************************************************************************
//      IN: unsigned char cFSDI
//  RETURN: void
// COMMENT: FSDI转换为FSD
//  		
//******************************************************************************
void PcdSetFSD(unsigned char cFSDI)
{
	switch (cFSDI)
	{
		case 0:					// 14
		case 1:					// 22
		case 2:					// 30
		case 3:					// 38
		case 4:					// 46
			g_sPcdPara.cFSD = 14 + cFSDI * 8;
			break;
		case 5:
			g_sPcdPara.cFSD = 62;
			break;
		case 6:
			g_sPcdPara.cFSD = 94;
			break;
		case 7:
			g_sPcdPara.cFSD = 126;
			break;
		default:
			g_sPcdPara.cFSD = 254;
			break;
	}
	return;
}


// PCD支持的函数
//******************************************************************************
//      IN: unsigned char cSta			// 0 = Disable; else = Enable;
//     OUT: void
//  RETURN: void
// COMMENT: Enable or Disable RegTxControl 天线控制
//  		
//******************************************************************************
void PcdAntenna(unsigned char cSta)
{
	unsigned char cTemp = ReadRawRC(PHHAL_HW_RC663_REG_DRVMODE);
	if (cSta)
	{
		WriteRawRC(PHHAL_HW_RC663_REG_DRVMODE, cTemp | 0x08);
	}
	else
	{
		WriteRawRC(PHHAL_HW_RC663_REG_DRVMODE, cTemp & (~0x08));
	}
}

////******************************************************************************
////    PARAM: void
////   RETURN: void
////******************************************************************************
//void PcdIdle(void)
//{
//	//WriteRawRC(JREG_COMMAND, JCMD_IDLE);
//	return;
//}





#endif

