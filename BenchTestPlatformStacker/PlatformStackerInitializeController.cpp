/*
 * PlatformStackerInitializeController.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "PlatformStackerInitializeController.h"
#include "PlatformStackerManager.h"
#include "PlatformStackerProcess.h"
#include "Common/Utility/TreeNodeNavigate.h"
#include <memory>
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"

namespace PlatformStacker
{

PlatformStackerInitializeController::PlatformStackerInitializeController(PlatformStackerManager* owner):
			Platform::Controller::BaseCSCIInitializeController(owner),
			ptr_owner(owner)
{
	 AddConfigurationFile("PlatformStackerConfig","BenchTestPlatformStacker.xml","../config/swmodules/dbw");
}

Common::InitializeStageStatus PlatformStackerInitializeController::LoadSpecificParameters()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;
   	std::string tConfigKey;
	using Common::Utility::TreeLoader;
	using Common::Utility::TreeNavigator;

	// reference csci_config consists the address of config file
	const Common::Utility::configFile& csci_config = *ConfigFile("PlatformStackerConfig");

	tConfigKey = "BenchPlatformStacker.main_thread_period";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.main_thread_period))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

//	tConfigKey = "BenchPlatformStacker.pcan_port";
//	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
//							  tConfigKey,config_params.pcan_port))
//	{
//		ret_status = Common::InitializeStageStatus_Failed;
//		AppendErrorString("Failed to Find <"+tConfigKey+">");
//	}

	tConfigKey = "BenchPlatformStacker.Debug.enabled";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.debug.enabled))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchPlatformStacker.Debug.log_folder";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.debug.log_folder))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchPlatformStacker.senderid_dbwcmd";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.senderid_dbwcmd))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchPlatformStacker.senderid_basecmd";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.senderid_basecmd))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	//display
	std::cout << "mThreadTimeoutmsec = " << config_params.main_thread_period << std::endl;
	std::cout << "pcan_port = " << config_params.pcan_port << std::endl;
	std::cout << "debug enabled  = " << config_params.debug.enabled << std::endl;
	std::cout << "debug log_folder  = " << config_params.debug.log_folder << std::endl;
	std::cout << "senderid_dbwcmd  = " << config_params.senderid_dbwcmd << std::endl;
	std::cout << "senderid_basecmd  = " << config_params.senderid_basecmd << std::endl;

	//
	return ret_status;
}

Common::InitializeStageStatus PlatformStackerInitializeController::SetupSpecificServices()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;

	ptr_owner->setMainLoopPeriod(config_params.main_thread_period);//ms
	//ptr_owner->sptr_PlatformStackerProcess->SetConfig();
	ptr_owner->sptr_PlatformStackerProcess->SetConfig(config_params);
	ptr_owner->sptr_RetreiveDBWCmds = std::make_shared<RetreiveDBWCmds>(config_params.senderid_dbwcmd);
	ptr_owner->sptr_RetreiveBaseCmds = std::make_shared<RetreiveBaseCmds>(config_params.senderid_basecmd);
	ptr_owner->sptr_PlatformStackerProcess->InitSharedPtrs(ptr_owner->sptr_RetreiveDBWCmds,ptr_owner->sptr_RetreiveBaseCmds);

	//the comms threads
	int32_t thread_id=11;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveDBWCmds,thread_id,0,10);
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveBaseCmds,thread_id,0,10);

	return ret_status;
}

PlatformStackerInitializeController::~PlatformStackerInitializeController()
{

}

}
