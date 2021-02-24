/*
 * PcanSeikoPcanEncoder.cpp
 *
 *  Created on: Dec 19, 2017
 *      Author: emily
 */

#include "PcanSeikoPcanEncoder.h"

//
namespace DBWPalletCommon
{

PcanSeikoPcanEncoder::PcanSeikoPcanEncoder()
{
	bSuccess_OpenPCANPort = false;
	mu32_TPCANRdMsgSize = static_cast<uint32_t>(sizeof(TPCANRdMsg));
}

bool PcanSeikoPcanEncoder::OpenCanPort(const std::string port_name)
{
	//init variables
	bSuccess_OpenPCANPort = false;

	//open pcan port
//	sptr_pcan_encoder=std::make_shared<Platform::PCanUsb::USBPCAN>();
//	char_t c_DevNode[12];
//	(void)sprintf(&c_DevNode[0], port_name.c_str());
//	//
//	int ierror=100;
//	ierror = sptr_pcan_encoder->initCan(&c_DevNode[0], static_cast<uint32_t>(CAN_BAUD_125K));
//	if(ierror == 0)
//	{
//		SetupDefaultSeikoEncoder();
//		bSuccess_OpenPCANPort = true;
//		std::cout << "Success open pcan port " << port_name << std::endl;
//	}
//	else
//	{
//		std::cout << "Cannot open pcan port " << port_name << std::endl;
//	}

	//KW_Edit - Open PcanEmuc Port
	sptr_pcanemuc_encoder = std::make_shared<Platform::PCanEmuc::PCanEmucDriver>();
	bool bSuccess = false;
	bSuccess = sptr_pcanemuc_encoder->initEmucPCAN();
	if (bSuccess == true)
	{
		SetupDefaultSeikoEncoder();
		bSuccess_OpenPCANPort = true;
		std::cout << "Success open pcan port " << port_name << std::endl;
	}
	else
	{
		std::cout << "Cannot open pcan port " << port_name << std::endl;
	}


	return bSuccess_OpenPCANPort;
}

//bool PcanSeikoPcanEncoder::OpenCanPort(const std::string port_name)
//{
//	//init variables
//	bSuccess_OpenPCANPort = false;
//
//	//open pcan port
//	sptr_pcan_encoder=std::make_shared<Platform::PCanUsb::USBPCAN>();
//
//	//check for correct device number, PCM card device number is 11
//	char_t dev_node[100];
//	char_t c_DevNode[100];
//	uint64_t deviceNo;
//	bool found_device_number=false;
//	std::string pcan_port_path;
//
//	for(int i=0; i<2; i++)//assume 2 usb can port connect
//	{
//		usleep(5000U);
//		//(void)sprintf(&dev_node[0], "/dev/pcanusb%d",i+32);
//		pcan_port_path="";
//		pcan_port_path.append("/dev/pcan");
//		std::stringstream port_number;
//		port_number<<i+32;
//		pcan_port_path.append(port_number.str());
//		std::cout << " " << std::endl;
//		std::cout << "testing i = " << i << " = " << pcan_port_path << std::endl;
//		//
//		(void)sprintf(&c_DevNode[0],"%s",pcan_port_path.c_str());
//		int ierror=100;
//		ierror = sptr_pcan_encoder->initCan(&c_DevNode[0], static_cast<uint32_t>(CAN_BAUD_125K));
//		if(ierror == 0) //connected
//		{
//			uint64_t irq_number;
//			if(sptr_pcan_encoder->GetIRQNumber(irq_number))
//			{
//				if(irq_number == 22)
//				{
//					std::cout << "Found Encoder CAN port " << std::endl;
//					SetupDefaultSeikoEncoder();
//					bSuccess_OpenPCANPort = true;
//					break;
//					//break;
//				}
//				else
//				{
//					std::cout << "irq_number = " << irq_number << std::endl;
//					sptr_pcan_encoder->stopCan();
//				}
//			}//get irq number
//		}
//	}//for.
//
//	return bSuccess_OpenPCANPort;
//}

void PcanSeikoPcanEncoder::SetupDefaultSeikoEncoder()
{
	bool b_ReadTimeout;
	TPCANMsg CAN_TxMsg;
	//

	//KW_Edit - Setup Seiko Encoder Start

	SetupCANMsgOneSeikoEncoder(&CAN_TxMsg);
	//sptr_pcan_encoder->TXCANMsg(&CAN_TxMsg,&b_ReadTimeout);
	sptr_pcanemuc_encoder->TxEmucCANMsg(&CAN_TxMsg, 0); //sptr_pcanemuc_encoder(&CAN_TxMsg, {0: CAN_1, 1: CAN_2});
	usleep(1000);
	//
	SetupCANMsgTwoSeikoEncoder(&CAN_TxMsg);
	//sptr_pcan_encoder->TXCANMsg(&CAN_TxMsg,&b_ReadTimeout);
	sptr_pcanemuc_encoder->TxEmucCANMsg(&CAN_TxMsg, 0);
	usleep(1000);
	//
	SetupCANMsgThreeSeikoEncoder(&CAN_TxMsg);
	//sptr_pcan_encoder->TXCANMsg(&CAN_TxMsg,&b_ReadTimeout);
	sptr_pcanemuc_encoder->TxEmucCANMsg(&CAN_TxMsg, 0);
	usleep(1000);
	//
	SetupCANMsgFourSeikoEncoder(&CAN_TxMsg);
	//sptr_pcan_encoder->TXCANMsg(&CAN_TxMsg,&b_ReadTimeout);
	sptr_pcanemuc_encoder->TxEmucCANMsg(&CAN_TxMsg, 0);
	usleep(1000);
	//
	SetupCANMsgFiveSeikoEncoder(&CAN_TxMsg);
	//sptr_pcan_encoder->TXCANMsg(&CAN_TxMsg,&b_ReadTimeout);
	sptr_pcanemuc_encoder->TxEmucCANMsg(&CAN_TxMsg, 0);
	usleep(50000);

	std::cout << " SetupSteerSikoEncoder Completed" << std::endl;


	//KW_Edit - Setup Seiko Encoder End
}

bool PcanSeikoPcanEncoder::GetSeikoSteerFeedback(uint32_t &encoder_position)
{
	TPCANRdMsg CAN_RxMsg;
    bool b_ReadTimeout;
//    std::cout << "Retreive encoders  ...." << std::endl;
// 	//transmit via PCAN
//	TPCANMsg CAN_TxMsg;
//	EncodeSeikoEncMsg(&CAN_TxMsg);
//	sptr_pcan_encoder->TXCANMsg(&CAN_TxMsg,&b_ReadTimeout);

	//receive via PCAN
	bool result=false;
	//sptr_pcan_encoder->RXCANMsg(&CAN_RxMsg,&b_ReadTimeout);
//	std::cout << "get seiko steer feedback" << std::endl;
	sptr_pcanemuc_encoder->RxEmucCANMsg(&CAN_RxMsg, 1);		//KW_Edit - Reading Seiko Encoder Feedback
//	std::cout << "get seiko steer feedback 1" << std::endl;

    result = DecodeSeikoEncMsg(&CAN_RxMsg, static_cast<int64_t>(Common::Utility::Time::getmsCountSinceEpoch()),encoder_position);
    if(result)
    {
    	std::cout << "-------------------------------------[Rx->Enc Position] " << encoder_position << std::endl;
    }
    //
    return result;
}

void PcanSeikoPcanEncoder::ConvertSeikoSteerCounttoAngle(uint32_t steer_enc_home_pos, uint32_t steer_enc_pos, float64_t &steer_angle_radian)
{
	float64_t feedback_seiko_steer_angle;
	int32_t delta_enc_pos;
	//
	delta_enc_pos = steer_enc_home_pos-steer_enc_pos;
	//for truck 1
//	feedback_seiko_steer_angle = delta_enc_pos/42.48;
	//for stacker
	feedback_seiko_steer_angle = delta_enc_pos/41.9181;
	//
	steer_angle_radian = feedback_seiko_steer_angle*3.141/180.0;
}

bool PcanSeikoPcanEncoder::DecodeSeikoEncMsg(TPCANRdMsg * const pMsgBuff, const int64_t time, uint32_t &pos)
{
	TPCANRdMsg CANmsg;
	(void) memcpy(&CANmsg, pMsgBuff, mu32_TPCANRdMsgSize);

	bool bSFlag;
	//std::cout << "can msg id = " << CANmsg.Msg.ID << std::endl;
	printf("[OUT->RXCANMsg Seiko Encoder] ID:%x\n", CANmsg.Msg.ID);
	if (CANmsg.Msg.ID == 0x183) // 0x583
	{
		std::cout << "decode seiko encoder" << std::endl;
//		printf("[OUT->RXCANMsg Seiko Encoder] ID:%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", CANmsg.Msg.ID,
//				CANmsg.Msg.DATA[0],CANmsg.Msg.DATA[1],
//				CANmsg.Msg.DATA[2],CANmsg.Msg.DATA[3],
//				CANmsg.Msg.DATA[4],CANmsg.Msg.DATA[5],
//				CANmsg.Msg.DATA[6],CANmsg.Msg.DATA[7]);

		uint32_t PositionBuffer[4];
		uint32_t nPosition = 0U;

		PositionBuffer[0] = CANmsg.Msg.DATA[0];
		PositionBuffer[1] = CANmsg.Msg.DATA[1];
		PositionBuffer[2] = CANmsg.Msg.DATA[2];
		PositionBuffer[3] = CANmsg.Msg.DATA[3];

		PositionBuffer[1] = PositionBuffer[1] <<8;
		PositionBuffer[2] = PositionBuffer[2] <<16;
		PositionBuffer[3] = PositionBuffer[3] <<24;

		nPosition = PositionBuffer[0] | PositionBuffer[1] | PositionBuffer[2] | PositionBuffer[3];
		pos = nPosition;

		//
		bSFlag  = true;
	}
	else
	{
		bSFlag  = false;
	}

	return bSFlag;
}

void PcanSeikoPcanEncoder::SetupCANMsgOneSeikoEncoder(TPCANMsg * const pMsgBuff)
{
	const uint32_t CAN_RX_SETTINGS1_ID = 0;

	TPCANMsg CANData;
	const uint32_t copysize = sizeof(TPCANMsg);
	//
	uint8_t i;
	for (i=0U; i<8U; i++)
	{
		CANData.DATA[i] = 0U;
	}
	CANData.DATA[0] = 0x80;
	CANData.DATA[1] = 0x03;

	//
	CANData.ID      = CAN_RX_SETTINGS1_ID;
	CANData.LEN     = 8U;
	CANData.MSGTYPE = 0U;

	// Copy CANData to MsgBuff
	(void) memcpy(pMsgBuff,&CANData,copysize);
}

void PcanSeikoPcanEncoder::SetupCANMsgTwoSeikoEncoder(TPCANMsg * const pMsgBuff)
{
	const uint32_t CAN_RX_SETTINGS1_ID = 0x0603;

	TPCANMsg CANData;
	const uint32_t copysize = sizeof(TPCANMsg);
	//
	CANData.DATA[0] = 0x22;
	CANData.DATA[1] = 0x00;
	CANData.DATA[2] = 0x18;
	CANData.DATA[3] = 0x02;
	CANData.DATA[4] = 0xFE;
	CANData.DATA[5] = 0x00;
	CANData.DATA[6] = 0x00;
	CANData.DATA[7] = 0x00;

	//
	CANData.ID      = CAN_RX_SETTINGS1_ID;
	CANData.LEN     = 8U;
	CANData.MSGTYPE = 0U;

	// Copy CANData to MsgBuff
	(void) memcpy(pMsgBuff,&CANData,copysize);
}

void PcanSeikoPcanEncoder::SetupCANMsgThreeSeikoEncoder(TPCANMsg * const pMsgBuff)
{
	const uint32_t CAN_RX_SETTINGS1_ID = 0x0603;

	TPCANMsg CANData;
	const uint32_t copysize = sizeof(TPCANMsg);
	//
	CANData.DATA[0] = 0x22;
	CANData.DATA[1] = 0x00;
	CANData.DATA[2] = 0x18;
	CANData.DATA[3] = 0x05;
	CANData.DATA[4] = 0x64;
	CANData.DATA[5] = 0x00;
	CANData.DATA[6] = 0x00;
	CANData.DATA[7] = 0x00;

	//
	CANData.ID      = CAN_RX_SETTINGS1_ID;
	CANData.LEN     = 8U;
	CANData.MSGTYPE = 0U;

	// Copy CANData to MsgBuff
	(void) memcpy(pMsgBuff,&CANData,copysize);
}

void PcanSeikoPcanEncoder::SetupCANMsgFourSeikoEncoder(TPCANMsg * const pMsgBuff)
{
	const uint32_t CAN_RX_SETTINGS1_ID = 0x0603;

	TPCANMsg CANData;
	const uint32_t copysize = sizeof(TPCANMsg);
	//
	CANData.DATA[0] = 0x22;
	CANData.DATA[1] = 0x10;
	CANData.DATA[2] = 0x10;
	CANData.DATA[3] = 0x01;
	CANData.DATA[4] = 0x73;
	CANData.DATA[5] = 0x61;
	CANData.DATA[6] = 0x76;
	CANData.DATA[7] = 0x65;

	//
	CANData.ID      = CAN_RX_SETTINGS1_ID;
	CANData.LEN     = 8U;
	CANData.MSGTYPE = 0U;

	// Copy CANData to MsgBuff
	(void) memcpy(pMsgBuff,&CANData,copysize);
}

void PcanSeikoPcanEncoder::SetupCANMsgFiveSeikoEncoder(TPCANMsg * const pMsgBuff)
{
	const uint32_t CAN_RX_SETTINGS1_ID = 0x00;

	TPCANMsg CANData;
	const uint32_t copysize = sizeof(TPCANMsg);
	//
	uint8_t i;
	for (i=0U; i<8U; i++)
	{
		CANData.DATA[i] = 0U;
	}
	//
	CANData.DATA[0] = 0x01;
	CANData.DATA[1] = 0x03;
	//
	CANData.ID      = CAN_RX_SETTINGS1_ID;
	CANData.LEN     = 8U;
	CANData.MSGTYPE = 0U;

	// Copy CANData to MsgBuff
	(void) memcpy(pMsgBuff,&CANData,copysize);
}

void PcanSeikoPcanEncoder::EncodeSeikoEncMsg(TPCANMsg * const pMsgBuff)
{
	const uint32_t CAN_RX_SETTINGS1_ID = 0x0603;

	TPCANMsg CANData;
	const uint32_t copysize = sizeof(TPCANMsg);
	//
	uint8_t i;
	for (i=0U; i<8U; i++)
	{
		CANData.DATA[i] = 0U;
	}
	CANData.DATA[0] = 0x40;
	CANData.DATA[1] = 0x04;
	CANData.DATA[2] = 0x60;

	//
	CANData.ID      = CAN_RX_SETTINGS1_ID;
	CANData.LEN     = 8U;
	CANData.MSGTYPE = 0U;

	// Copy CANData to MsgBuff
	(void) memcpy(pMsgBuff,&CANData,copysize);
}

PcanSeikoPcanEncoder::~PcanSeikoPcanEncoder()
{

}

}

