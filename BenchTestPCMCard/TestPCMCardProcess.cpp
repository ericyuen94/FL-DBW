/*
 * TestPCMCardProcess.cpp
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#include "TestPCMCardProcess.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace TestPCMCard
{

TestPCMCardProcess::TestPCMCardProcess()
{
	previous_timestamp = 0;
	bSuccess_OpenPCANPort = false;
	mu32_TPCANRdMsgSize = static_cast<uint32_t>(sizeof(TPCANRdMsg));
}

void TestPCMCardProcess::SetConfig(const TestPCMCardConfig config)
{
	config_params = config;
	std::cout << "[PCAN PORT] = " << config_params.pcan_port << std::endl;

	//open pcan port
	sptr_pcan=std::make_shared<Platform::PCanUsb::USBPCAN>();
	char_t c_DevNode[12];
	(void)sprintf(&c_DevNode[0], config_params.pcan_port.c_str());
	//
	int ierror=100;
	ierror = sptr_pcan->initCan(&c_DevNode[0], static_cast<uint32_t>(CAN_BAUD_125K));
	if(ierror == 0)
	{
		bSuccess_OpenPCANPort = true;
		std::cout << "Success open pcan port " << config_params.pcan_port << std::endl;
	}
	else
	{
		std::cout << "Cannot open pcan port " << config_params.pcan_port << std::endl;
	}

	//setup of logger
	if(config_params.debug.enabled==1)
	{
		sptr_PCanLogger = std::make_shared<Platform::PCanUsb::PCanLogger>();
		sptr_PCanLogger->SetupPCanLogger(config.debug.log_folder);
	}
}

void TestPCMCardProcess::operator()()
{
	TPCANRdMsg CAN_RxMsg;
    bool b_ReadTimeout;

    //receive messages via PCAN
    sptr_pcan->RXCANMsg(&CAN_RxMsg,&b_ReadTimeout);
	DecodePCMMsg(&CAN_RxMsg, static_cast<int64_t>(Common::Utility::Time::getmsCountSinceEpoch()));
	//DecodeSeikoEncoderMsg(&CAN_RxMsg, static_cast<int64_t>(Common::Utility::Time::getmsCountSinceEpoch()));

	//log data.
	if(config_params.debug.enabled==1)
	{
		sptr_PCanLogger->WriteLog(Common::Utility::Time::getusCountSinceEpoch(),CAN_RxMsg);
	}

//	//transmit messages via PCAN
//	TPCANMsg CAN_TxMsg;
//	EncodePCMMsg(&CAN_TxMsg);
//	sptr_pcan->TXCANMsg(&CAN_TxMsg,&b_ReadTimeout);

	//
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();
	std::cout << current_timestamp-previous_timestamp << " ms main loop thread time " << std::endl;
	previous_timestamp =  current_timestamp;
}

bool TestPCMCardProcess::DecodeSeikoEncoderMsg(TPCANRdMsg * const pMsgBuff, const int64_t time)
{
	TPCANRdMsg CANmsg;
	(void) memcpy(&CANmsg, pMsgBuff, mu32_TPCANRdMsgSize);

	bool bSFlag;
	//if (CANmsg.Msg.ID == 0xa60) // 0x2e0
	//{
		printf("[OUT->RXCANMsg SEIKO] ID:%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", CANmsg.Msg.ID,
				CANmsg.Msg.DATA[0],CANmsg.Msg.DATA[1],
				CANmsg.Msg.DATA[2],CANmsg.Msg.DATA[3],
				CANmsg.Msg.DATA[4],CANmsg.Msg.DATA[5],
				CANmsg.Msg.DATA[6],CANmsg.Msg.DATA[7]);
		bSFlag  = true;
	//}
	//else
	//{
	//	bSFlag  = false;
	//}

	return bSFlag;
}

bool TestPCMCardProcess::TestPCMCardProcess::DecodePCMMsg(TPCANRdMsg * const pMsgBuff, const int64_t time)
{
	TPCANRdMsg CANmsg;
	(void) memcpy(&CANmsg, pMsgBuff, mu32_TPCANRdMsgSize);

	bool bSFlag;
	if (CANmsg.Msg.ID == 0x2e0) // 0x2e0
	{
		printf("[==============> !!!!!!! OUT->RXCANMsg PCM] ID:%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", CANmsg.Msg.ID,
				CANmsg.Msg.DATA[0],CANmsg.Msg.DATA[1],
				CANmsg.Msg.DATA[2],CANmsg.Msg.DATA[3],
				CANmsg.Msg.DATA[4],CANmsg.Msg.DATA[5],
				CANmsg.Msg.DATA[6],CANmsg.Msg.DATA[7]);
		bSFlag  = true;
	}
	else
	{
		printf("[Others->RXCANMsg PCM] ID:%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", CANmsg.Msg.ID,
						CANmsg.Msg.DATA[0],CANmsg.Msg.DATA[1],
						CANmsg.Msg.DATA[2],CANmsg.Msg.DATA[3],
						CANmsg.Msg.DATA[4],CANmsg.Msg.DATA[5],
						CANmsg.Msg.DATA[6],CANmsg.Msg.DATA[7]);
		bSFlag  = false;
	}

	return bSFlag;
}

void TestPCMCardProcess::EncodePCMMsg(TPCANMsg * const pMsgBuff)
{
	const uint32_t CAN_RX_SETTINGS1_ID = 0x01D0;

	TPCANMsg CANData;
	const uint32_t copysize = sizeof(TPCANMsg);
	//
	uint8_t i;
	for (i=0U; i<8U; i++)
	{
		CANData.DATA[i] = 0U;
	}
	CANData.DATA[0] = 0xAA;

	//
	CANData.ID      = CAN_RX_SETTINGS1_ID;
	CANData.LEN     = 8U;
	CANData.MSGTYPE = 0U;

	// Copy CANData to MsgBuff
	(void) memcpy(pMsgBuff,&CANData,copysize);
}

TestPCMCardProcess::~TestPCMCardProcess()
{
	if(config_params.debug.enabled==1)
	{
		sptr_PCanLogger->CloseDataLogger();
	}
}

}
