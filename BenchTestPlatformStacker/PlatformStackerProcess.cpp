#include "PlatformStackerProcess.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace PlatformStacker
{

PlatformStackerProcess::PlatformStackerProcess()
{
	previous_timestamp = 0;
	bSuccess_OpenPCANPort = false;
	mu32_TPCANRdMsgSize = static_cast<uint32_t>(sizeof(TPCANRdMsg));
	alternative_byte = false;
}

void PlatformStackerProcess::SetConfig(const PlatformStackerConfig config)
{
	config_params = config;

	std::cout << "[PCAN PORT] = " << config_params.pcan_port << std::endl;

	//open pcan port
//	sptr_pcan=std::make_shared<Platform::PCanUsb::USBPCAN>();
//	char_t c_DevNode[12];
//	(void)sprintf(&c_DevNode[0], config_params.pcan_port.c_str());
//	//
//	int ierror=100;
//	ierror = sptr_pcan->initCan(&c_DevNode[0], static_cast<uint32_t>(CAN_BAUD_125K));
//	if(ierror == 0)
//	{
//		bSuccess_OpenPCANPort = true;
//		std::cout << "Success open pcan port " << config_params.pcan_port << std::endl;
//	}
//	else
//	{
//		std::cout << "Cannot open pcan port " << config_params.pcan_port << std::endl;
//	}

	//KW_Edit - Open PcanEmuc Port
	sptr_pcanemuc = std::make_shared<Platform::PCanEmuc::PCanEmucDriver>();
	bool bSuccess = false;
	bSuccess = sptr_pcanemuc->initEmucPCAN();
	if (bSuccess == true)
	{
		bSuccess_OpenPCANPort = true;
		std::cout << "Success open pcan port " << config_params.pcan_port << std::endl;
	}
	else
	{
		std::cout << "Cannot open pcan port " << config_params.pcan_port << std::endl;
	}

}

void PlatformStackerProcess::operator()()
{
//	//TPCANRdMsg CAN_RxMsg;
//    bool b_ReadTimeout;
//
//    //
//	//transmit via PCAN
//	TPCANMsg CAN_TxMsg;
//	uchar_t out_byte;
//	if(alternative_byte)
//	{
//		out_byte = 0x00;
//		alternative_byte = false;
//	}
//	else
//	{
//		out_byte = 0x80;
//		alternative_byte = true;
//	}


	//stkci messages
	Platform::Sensors::PalletDbwCmdMsg in_data_dbwcmd;
	Platform::Sensors::PalletBaseCmdMsg in_data_basecmd;

	//Similar to PCM commands
	if(sptr_RetreiveDBWCmds_->GetDBWCmdMsg(in_data_dbwcmd))
	{
		//Get Palletbase commands
		sptr_RetreiveBaseCmds_->GetBaseCmdMsg(in_data_basecmd);

		TPCANRdMsg CAN_RxMsg;
	    bool b_ReadTimeout;

	    //
		//transmit via PCAN
		TPCANMsg CAN_TxMsg;
		uchar_t out_byte;
		if(alternative_byte)
		{
			out_byte = 0x00;
			alternative_byte = false;
		}
		else
		{
			out_byte = 0x80;
			alternative_byte = true;
		}

		EncodeStackerMsg(&CAN_TxMsg, out_byte, in_data_dbwcmd, in_data_basecmd);
		//sptr_pcan->TXCANMsg(&CAN_TxMsg,&b_ReadTimeout);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 0);		//KW_Edit - Send Command to Drive
	}

	//
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();
	std::cout << current_timestamp-previous_timestamp << " ms main loop thread time " << std::endl;
	previous_timestamp =  current_timestamp;
}

void PlatformStackerProcess::InitSharedPtrs(std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds,
										 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds)
{
	sptr_RetreiveDBWCmds_ = sptr_RetreiveDBWCmds;
	sptr_RetreiveBaseCmds_= sptr_RetreiveBaseCmds;
}

void PlatformStackerProcess::EncodeStackerMsg(TPCANMsg * const pMsgBuff, uchar_t out_byte_two, Platform::Sensors::PalletDbwCmdMsg &in_data_dbwcmd, Platform::Sensors::PalletBaseCmdMsg &in_data_basecmd)
{
	const uint32_t CAN_RX_SETTINGS1_ID = 0x1E0;

	TPCANMsg CANData;
	const uint32_t copysize = sizeof(TPCANMsg);

	if(in_data_basecmd.trigger_slowspeed < 0)		// Lower fork
	{
		fork_speed = static_cast<int32_t>(in_data_basecmd.trigger_slowspeed*4095);

		CANData.DATA[0] = 0x01;
		CANData.DATA[1] = out_byte_two;
		CANData.DATA[2] = 0x00;
		CANData.DATA[3] = 0x00;
		CANData.DATA[4] = 0x00;
		CANData.DATA[5] = 0xF0;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;

		uint8_t hex[4];
		ConvertUIntegerToHex(fork_speed,hex);
		CANData.DATA[4] = hex[0]&0xFF;
		CANData.DATA[5] = hex[1]&0xFF;
	}

	else if(in_data_basecmd.trigger_belly > 0)		// Raise fork
	{
		std::cout << "Enter raise fork " << std::endl;
		fork_speed = static_cast<int32_t>(in_data_basecmd.trigger_belly*4095);

		CANData.DATA[0] = 0x01;
		CANData.DATA[1] = out_byte_two;
		CANData.DATA[2] = 0x00;
		CANData.DATA[3] = 0x00;
		CANData.DATA[4] = 0xFF;
		CANData.DATA[5] = 0x0F;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;

		uint8_t hex[4];
		ConvertUIntegerToHex(fork_speed,hex);
		CANData.DATA[4] = hex[0]&0xFF;
		CANData.DATA[5] = hex[1]&0xFF;
	}

	else if(in_data_dbwcmd.cmd_speed > 0)
	{
		std::cout << "Move forward " << std::endl;
		analog_speed = static_cast<int32_t>(in_data_dbwcmd.cmd_speed*4095);

		CANData.DATA[0] = 0x00;
		CANData.DATA[1] = out_byte_two;
		CANData.DATA[2] = 0xFF;
		CANData.DATA[3] = 0x0F;
		CANData.DATA[4] = 0x00;
		CANData.DATA[5] = 0x00;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;

		uint8_t hex[4];
		ConvertUIntegerToHex(analog_speed,hex);
		CANData.DATA[2] = hex[0]&0xFF;
		CANData.DATA[3] = hex[1]&0xFF;
	}

//	else if(in_data_dbwcmd.cmd_speed == 0)
//	{
//		uint8_t i;
//		for (i=0U; i<8U; i++)
//		{
//			CANData.DATA[i] = 0U;
//		}
//		CANData.DATA[0] = 0x01;
//		CANData.DATA[1] = out_byte_two;
//	}

	else if(in_data_dbwcmd.cmd_speed < 0)
	{
		analog_speed = static_cast<int32_t>(in_data_dbwcmd.cmd_speed*4095);

		CANData.DATA[0] = 0x00;
		CANData.DATA[1] = out_byte_two;
		CANData.DATA[2] = 0x01; //FF
		CANData.DATA[3] = 0xF0; //0F
		CANData.DATA[4] = 0x00;
		CANData.DATA[5] = 0x00;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;

		uint8_t hex[4];
		ConvertUIntegerToHex(analog_speed,hex);
		CANData.DATA[2] = hex[0]&0xFF;
		CANData.DATA[3] = hex[1]&0xFF;
	}

	else
	{
		uint8_t i;
		for (i=0U; i<8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		CANData.DATA[0] = 0x01;
		CANData.DATA[1] = out_byte_two;
	}

	CANData.ID      = CAN_RX_SETTINGS1_ID;
	CANData.LEN     = 8U;
	CANData.MSGTYPE = 0U;

	// Copy CANData to MsgBuff
	(void) memcpy(pMsgBuff,&CANData,copysize);

}

void PlatformStackerProcess::ConvertUIntegerToHex( const uint32_t nValue, uint8_t ( &pBuffer )[4] )
{

	const uint32_t nMask             = 0x000000FFU;
	const uint32_t nBitShiftIndex[4] = { 0U, 8U, 16U, 24U };
	//
	for( uint32_t i = 0U; i < 4; i++ )
	{
		uint32_t nBuffer = 0U;
		if( nBitShiftIndex[i] <= 24U )
		{
			nBuffer = nValue >> nBitShiftIndex[i];
		}
		pBuffer[i] = static_cast<uint8_t>( nBuffer & nMask );
	}//

}

PlatformStackerProcess::~PlatformStackerProcess()
{

}

}
