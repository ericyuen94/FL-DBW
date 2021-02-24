/*
 * PlatformBydFlInitializeController.cpp
 *
 *  Created on: July 1, 2019
 *      Author: Tester
 */

#include "PlatformBydFlInitializeController.h"
#include "PlatformBydFlManager.h"
#include "PlatformBydFlProcess.h"
#include "Common/Utility/TreeNodeNavigate.h"
#include <memory>
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"

namespace PlatformBydFl
{

PlatformBydFlInitializeController::PlatformBydFlInitializeController(PlatformBydFlManager* owner):
			Platform::Controller::BaseCSCIInitializeController(owner),
			ptr_owner(owner)
{
	 AddConfigurationFile("PlatformBydFlConfig","BenchTestPlatformBydFl.xml","../config/swmodules/dbw");
}

Common::InitializeStageStatus PlatformBydFlInitializeController::LoadSpecificParameters()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;
   	std::string tConfigKey;
	using Common::Utility::TreeLoader;
	using Common::Utility::TreeNavigator;

	// reference csci_config consists the address of config file
	const Common::Utility::configFile& csci_config = *ConfigFile("PlatformBydFlConfig");

	tConfigKey = "BenchTestPlatformBydFl.main_thread_period";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.main_thread_period))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestPlatformBydFl.feedback_thread_period";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.feedback_thread_period))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestPlatformBydFl.drive_thread_period";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.drive_thread_period))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestPlatformBydFl.pcan_port";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.pcan_port))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestPlatformBydFl.Debug.enabled";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.debug.enabled))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestPlatformBydFl.Debug.log_folder";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.debug.log_folder))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestPlatformBydFl.senderid_dbwcmd";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.senderid_dbwcmd))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestPlatformBydFl.senderid_publisher";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.senderid_publisher))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestPlatformBydFl.senderid_basecmd";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.senderid_basecmd))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	//
	tConfigKey = "BenchTestPlatformBydFl.max_forward_speed";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.max_forward_speed))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	//
	tConfigKey = "BenchTestPlatformBydFl.max_steering";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.max_steering))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}


	//display
	std::cout << "mThreadTimeoutmsec = " << config_params.main_thread_period << std::endl;
	std::cout << "pcan_port = " << config_params.pcan_port << std::endl;
	std::cout << "senderid_dbwcmd = " << config_params.senderid_dbwcmd << std::endl;
	std::cout << "senderid_basecmd = " << config_params.senderid_basecmd << std::endl;
	std::cout << "max_forward_speed = " << config_params.max_forward_speed << std::endl;
	std::cout << "max_steering = " << config_params.max_steering << std::endl;
	//
	return ret_status;
}

Common::InitializeStageStatus PlatformBydFlInitializeController::SetupSpecificServices()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;

	ptr_owner->setMainLoopPeriod(config_params.main_thread_period);//ms

	//
	ptr_owner->sptr_RetreiveDBWCmds = std::make_shared<RetreiveDBWCmds>("TeleOps");//(config_params.senderid_dbwcmd);
	ptr_owner->sptr_RetreiveBaseCmds = std::make_shared<RetreiveBaseCmds>("DBW");//(config_params.senderid_basecmd);
	ptr_owner->sptr_RetreiveTiltMotorFbkMsgs = std::make_shared<RetreiveTiltMotorFbkMsgs>("TiltMotorFbkMsg");//(config_params.senderid_basecmd);
	ptr_owner->sptr_PlatformBydFlProcess->InitSharedPtrs(ptr_owner->sptr_RetreiveDBWCmds,
			ptr_owner->sptr_RetreiveBaseCmds, ptr_owner->sptr_RetreiveTiltMotorFbkMsgs);
	ptr_owner->sptr_PlatformBydFlProcess->SetConfig(config_params);

	//the comms threads
	int32_t thread_id=11;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveDBWCmds,thread_id,0,10);
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveBaseCmds,thread_id,0,10);
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveTiltMotorFbkMsgs,thread_id,0,10);
	thread_id++;
	ptr_owner->startThread(std::bind(&PlatformBydFlProcess::FeedbackDBWandPalletBaseStatus,
			ptr_owner->sptr_PlatformBydFlProcess.get()),
			thread_id,
			0,10);

//	ptr_owner->startThread(std::bind(&PlatformBydFlProcess::ValveControl_thread,
//				ptr_owner->sptr_PlatformBydFlProcess.get()),
//				thread_id,
//				config_params.drive_thread_period,10);
//
//	thread_id++;
//
//	ptr_owner->startThread(std::bind(&PlatformBydFlProcess::RelayOutputControl_thread,
//				ptr_owner->sptr_PlatformBydFlProcess.get()),
//				thread_id,
//				200,10);


	return ret_status;
}

PlatformBydFlInitializeController::~PlatformBydFlInitializeController()
{

}

}
