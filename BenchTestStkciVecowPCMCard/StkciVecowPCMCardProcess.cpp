/*
 * StkciVecowPCMCardProcess.cpp
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#include "StkciVecowPCMCardProcess.h"
#include "StkciPCMPalletBoundary.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace StkciVecowPCMCard
{

StkciVecowPCMCardProcess::StkciVecowPCMCardProcess()
{
	previous_timestamp = 0;
	previous_fb_timestamp = 0;
	pub_feedback_data_count = 0;
}

void StkciVecowPCMCardProcess::SetConfig(const StkciVecowPCMCardConfig config)
{
	config_params = config;

	//Open port
	sptr_PcmPalletController = std::make_shared<DBWPalletCommon::PCMPalletController>();
	sptr_IsolatedDIODriver = std::make_shared<Platform::IsolatedDIO::IsolatedDIODriver>();

	//
	if(!sptr_PcmPalletController->OpenVecowCanPortModule())
	{
		std::cout << "Cannot open can port !! " << std::endl;
		exit(1);
	}

	//open DIO port
	bool bchk1 = false;
	bchk1 = sptr_IsolatedDIODriver->initDIO();
	if (bchk1)
	{
		std::cout << "Success open Vecow DIO Port " << std::endl;
	}
	else
	{
		std::cout << "Fail to open Vecow DIO Port " << std::endl;
		exit(1);
	}

	sptr_PcmPalletController->ResetLastCommandSpeed();
	if(config_params.debug.enabled==1)
	{
		sptr_PcmPalletController->SetLogger(config_params.debug.log_folder);
	}

	//Send a startup procedure to prevent pallet from electrical shutdown
	//sptr_PcmPalletController->SetStartUpProcedurePallet();

	//Set middleware publisher
	ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
	std::vector<std::string> interface_list;
	interface_list.push_back("PalletBaseFb");
	ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);
}

void StkciVecowPCMCardProcess::InitSharedPtrs(std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds,
										 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds)
{
	sptr_RetreiveDBWCmds_ = sptr_RetreiveDBWCmds;
	sptr_RetreiveBaseCmds_= sptr_RetreiveBaseCmds;
}

void StkciVecowPCMCardProcess::operator()()
{
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	//stkci messages
	Platform::Sensors::PalletDbwCmdMsg in_data_dbwcmd;
	Platform::Sensors::PalletBaseCmdMsg in_data_basecmd;

	//TODO:: Send Heart beat to prevent pallet from electrical shutdown
	sptr_PcmPalletController->SetHeartBeatTiller();

	//Get DBW commands
	if(sptr_RetreiveDBWCmds_->GetDBWCmdMsg(in_data_dbwcmd))
	{
		//Get Palletbase commands
		sptr_RetreiveBaseCmds_->GetBaseCmdMsg(in_data_basecmd);

		//the tiller is default to be on now.
//		if(in_data_basecmd.trigger_tiller==0)
//		{
//			sptr_PcmPalletController->ResetLastCommandSpeed();
//		}

		//Send to PCM controller
		DBWPalletCommon::pcm_trucktwo_palletbase_commands pcm_commands_current;

		//Convert to STKCI to pcm commands
		std::cout << "Current speed in percent = " << in_data_dbwcmd.cmd_speed << std::endl;
		DBWPalletCommon::StkciPCMPalletBoundary::Convert_StkciDBWAndPalletBaseCmds_To_TruckTwoPalletPCMCmd(in_data_dbwcmd,in_data_basecmd,pcm_commands_current);

		//For Pallet truck two.
		uint8_t out_dio_cmd;
		sptr_PcmPalletController->GetVecowDIOCommands(pcm_commands_current,out_dio_cmd);
		sptr_IsolatedDIODriver->SetDIO_OutputState(out_dio_cmd);
		sptr_PcmPalletController->SetPCMTruckTwoPalletCommands(pcm_commands_current);


		//---------------------------------------------------------------
		//send drive pcm command message based on desired speed.
//		DBWPalletCommon::pcm_palletbase_commands pcm_commands_direction_change;
//		if(sptr_PcmPalletController->PCMSpeedCtrlLogic(in_data_dbwcmd.cmd_speed))
//		{
//			pcm_commands_direction_change = pcm_commands_current;
//			pcm_commands_direction_change.analog_speed=0;
//			sptr_PcmPalletController->SetPCMPalletCommands(pcm_commands_direction_change);
//			//std::cout << "Send change of speed direction msg" << std::endl;
//			usleep(50000); //50ms requirements
//		}
//		sptr_PcmPalletController->SetPCMPalletCommands(pcm_commands_current);
	}

	std::cout << current_timestamp-previous_timestamp << " ms main Set PCM loop thread time " << std::endl;
	previous_timestamp =  current_timestamp;
}

void StkciVecowPCMCardProcess::get_pallet_feedback_thread()
{
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	//std::cout << "Send change of pallet drive msg" << std::endl;
	//Get Feedback and publish feedback for pallet base
	Platform::Sensors::PalletDbwFbkMsg out_stkci_dbw_fbk;
	Platform::Sensors::PalletBaseFbkMsg out_stkci_basepallet_fbk;
	DBWPalletCommon::pcm_palletbase_feedback pcm_feedback;

	//
	if(sptr_PcmPalletController->GetPCMPalletFeedback(pcm_feedback))
	{
		DBWPalletCommon::StkciPCMPalletBoundary::Convert_PalletPCMFbk_To_StkciDBWAndPalletBaseFbk(pcm_feedback,out_stkci_dbw_fbk,out_stkci_basepallet_fbk);

		//pub_feedback_data_count++;
		//if(pub_feedback_data_count>10)
		{
			ptr_stkci_pub->PubPalletBaseFbk(config_params.senderid_publisher, out_stkci_basepallet_fbk);
			//pub_feedback_data_count = 0;
		}
	}

	std::cout << current_timestamp-previous_fb_timestamp << " ms pallet Get PCM feedback loop thread time " << std::endl;
	previous_fb_timestamp =  current_timestamp;
}

void StkciVecowPCMCardProcess::ShutdownPCM()
{
	//Reset to home state
	uchar_t out_dio=0x00;
	uint8_t out_dio_vecow = static_cast<uint8_t>(out_dio);
	for(int32_t i=0; i<10; i++)//2s //60 //50
	{
		sptr_IsolatedDIODriver->SetDIO_OutputState(out_dio_vecow);
		usleep(50000); //50ms requirements
	}

	sptr_PcmPalletController->SetShutdownPCMProcedure();
	std::cout << "Shutdown PCM card and DIO module done! " << std::endl;
}

StkciVecowPCMCardProcess::~StkciVecowPCMCardProcess()
{

}

}

