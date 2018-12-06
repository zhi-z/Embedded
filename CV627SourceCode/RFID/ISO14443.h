/*******************************************************************************
 * FileName: ISO14443.h
 *   Author: Layne Wen
 *     Date: 2017.02.22
 *  COMMENT: 版本日期更新
 ******************************************************************************/
#ifndef ISO14443_H
#define ISO14443_H

//------------------------------------------------------------------------------
// ISO14443-3 Command
//------------------------------------------------------------------------------
#define PICC_REQIDL			0x26		// 寻天线区内未进入休眠状态的卡
#define PICC_REQALL			0x52		// 寻天线区内全部卡
#define PICC_ANTICOLL1		0x93		// 一级防冲撞
#define PICC_ANTICOLL2		0x95		// 二级防冲撞
#define PICC_ANTICOLL3		0x97		// 三级防冲撞
#define PICC_HALT			0x50		// 停止

//------------------------------------------------------------------------------
// ISO14443-4 Command
//------------------------------------------------------------------------------
#define PICC_RESET			0xE0		// 复位功能
#define PICC_PPSS			0xD0		// 

//------------------------------------------------------------------------------
// MFIRE卡命令字定义
//------------------------------------------------------------------------------
#define PICC_AUTHENT1A		0x60		// 验证A密钥
#define PICC_AUTHENT1B		0x61		// 验证B密钥
#define PICC_READ			0x30		// 读块
#define PICC_WRITE			0xA0		// 写块
#define PICC_DECREMENT		0xC0		// 扣款
#define PICC_INCREMENT		0xC1		// 充值
#define PICC_RESTORE		0xC2		// 调钱包到缓冲区
#define PICC_TRANSFER		0xB0		// 保存缓冲区中数据

//------------------------------------------------------------------------------
// Mifare密钥来源定义
//------------------------------------------------------------------------------
#define KEY_CURRENT			0x01
#define KEY_E2PROM			0x02

//------------------------------------------------------------------------------
// Type Define
//------------------------------------------------------------------------------
typedef struct
{
	unsigned char cUID[11];
	unsigned char cATQ[2];
	unsigned char cSAK;
	unsigned char cLen;
} tpd_Card;

//------------------------------------------------------------------------------
// Extern Functions Define
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ISO14443-3 Type A functions define
//------------------------------------------------------------------------------
void          PiccReset(void);
unsigned char PiccRequest(unsigned char cReq,unsigned char *cATQA);						// 寻卡
unsigned char PiccAnticoll(unsigned char cLev, unsigned char *cUID);					// 防冲撞
unsigned char PiccSelect(unsigned char cLev, unsigned char *cUID,unsigned char *cSAK);	// 选定一张卡
unsigned char PiccHaltA(void);															// 卡休眠
unsigned char PiccAnticollSelect(unsigned char *cLen, unsigned char *cUID, unsigned char *cSAK);

//------------------------------------------------------------------------------
// ISO14443-3 Type B functions define
//------------------------------------------------------------------------------
unsigned char PiccRequestB(unsigned char cReq,unsigned char AFI,unsigned char N,unsigned char *cpATQB);
unsigned char PiccAttrib(unsigned char *PUPI, unsigned char PARAM3,unsigned char cFSDI, unsigned char *answer);
unsigned char PiccHaltB(unsigned char *PUPI);

//------------------------------------------------------------------------------
// ISO14443-4 Type A&B CPU card functions define
//------------------------------------------------------------------------------
unsigned char PiccTypeA_Rst(unsigned char *cpLen, unsigned char *cpData);

unsigned char ISO14443A_Rst(unsigned char *cpLen, unsigned char *cpRATS);

unsigned char ISO14443_COS(unsigned short *pLen, unsigned char *pAPDU);



//--------------------------------------------------------------------------------------------------
// Mifare S50/S70 card functions define
//--------------------------------------------------------------------------------------------------
unsigned char PiccAuthState(unsigned char cBlk, unsigned char cMode, unsigned char cKType, unsigned char *cKey);// 验证密钥

unsigned char PiccRead(unsigned char cAddr, unsigned char *cpData);	// 读块
unsigned char PiccWrite(unsigned char cBlk, unsigned char *cData);	// 写块
unsigned char PiccValue(unsigned char cFct,unsigned char cBlk, unsigned char *cValue);	// 充值扣款
unsigned char PiccRestore(unsigned char cBlk);											// 备份
unsigned char PiccTransfer(unsigned char cBlk);											// 操作


//------------------------------------------------------------------------------
// ID Card COS
//------------------------------------------------------------------------------
unsigned char IDCard_Reqest(unsigned short *ipLen, unsigned char *pATQB);
unsigned char IDCard_APDU(unsigned short *ipLen, unsigned char *cpAPDU);


#endif

