/*
 * StkciPCMCardProcess.cpp
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#include "StkciPCMCardProcess.h"
#include "StkciPCMPalletBoundary.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace StkciPCMCard
{

StkciPCMCardProcess::StkciPCMCardProcess()
{
	previous_timestamp = 0;
	previous_fb_timestamp = 0;
	pub_feedback_data_count = 0;
}

void StkciPCMCardProcess::SetConfig(const StkciPCMCardConfig config)
{
	config_params = config;
	std::cout << "[PCAN PORT] = " << config_params.pcan_port << std::endl;

	//Open port
	sptr_PcmPalletController = std::make_shared<DBWPalletCommon::PCMPalletController>();
	if(!sptr_PcmPalletController->OpenCanPort(config_params.pcan_port))
	{
		std::cout << "Cannot open can port !! " << std::endl;
		exit(1);
	}
	sptr_PcmPalletController->ResetLastCommandSpeed();
	if(config_params.debug.enabled==1)
	{
		sptr_PcmPalletController->SetLogger(config_params.debug.log_folder);
	}
	//Send a startup procedure to prevent pallet from electrical shutdown
	sptr_PcmPalletController->SetStartUpProcedurePallet();

	//Set middleware publisher
	ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
	std::vector<std::string> interface_list;
	interface_list.push_back("PalletBaseFb");
	ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);
}

void StkciPCMCardProcess::InitSharedPtrs(std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds,
										 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds)
{
	sptr_RetreiveDBWCmds_ = sptr_RetreiveDBWCmds;
	sptr_RetreiveBaseCmds_= sptr_RetreiveBaseCmds;
}

void StkciPCMCardProcess::operator()()
{
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	//stkci messages
	Platform::Sensors::PalletDbwCmdMsg in_data_dbwcmd;
	Platform::Sensors::PalletBaseCmdMsg in_data_basecmd;

	//TODO:: Send Heart beat to prevent pallet from electrical shutdown
	//sptr_PcmPalletController->SetHeartBeatTiller();

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
		DBWPalletCommon::pcm_palletbase_commands pcm_commands_current;

		//Convert to STKCI to pcm commands
		std::cout << "Current speed in percent = " << in_data_dbwcmd.cmd_speed << std::endl;
		DBWPalletCommon::StkciPCMPalletBoundary::Convert_StkciDBWAndPalletBaseCmds_To_PalletPCMCmd(in_data_dbwcmd,in_data_basecmd,pcm_commands_current);

		//---------------------------------------------------------------
		//send drive pcm command message based on desired speed.
		DBWPalletCommon::pcm_palletbase_commands pcm_commands_direction_change;
		if(sptr_PcmPalletController->PCMSpeedCtrlLogic(in_data_dbwcmd.cmd_speed))
		{
			pcm_commands_direction_change = pcm_commands_current;
			pcm_commands_direction_change.analog_speed=0;
			sptr_PcmPalletController->SetPCMPalletCommands(pcm_commands_direction_change);
			//std::cout << "Send change of speed direction msg" << std::endl;
			usleep(50000); //50ms requirements
		}

		sptr_PcmPalletController->SetPCMPalletCommands(pcm_commands_current);
	}

	std::cout << current_timestamp-previous_timestamp << " ms main Set PCM loop thread time " << std::endl;
	previous_timestamp =  current_timestamp;
}

void StkciPCMCardProcess::get_pallet_feedback_thread()
{
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	//std::cout << "Send change of pallet drive msg" << std::endl;
	//Get Feedback and publish feedback for pallet base
	Platform::Sensors::PalletDbwFbkMsg out_stkci_dbw_fbk;
	Platform::Sensors::PalletBaseFbkMsg out_stkci_basepallet_fbk;
	DBWPalletCommon::pcm_palletbase_feedback pcm_feedback;
	DBWPalletCommon::PCMHealthStatusFeedback in_health_status_fb;

	if(sptr_PcmPalletController->GetPCMPalletFeedback(pcm_feedback,in_health_status_fb))
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

void StkciPCMCardProcess::ShutdownPCM()
{
	sptr_PcmPalletController->SetShutdownPCMProcedure();
	std::cout << "Shutdown PCM card done! " << std::endl;
}

StkciPCMCardProcess::~StkciPCMCardProcess()
{

}

}

