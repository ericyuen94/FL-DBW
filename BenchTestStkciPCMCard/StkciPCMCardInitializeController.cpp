/*
 * StkciPCMCardInitializeController.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "StkciPCMCardInitializeController.h"
#include "StkciPCMCardManager.h"
#include "StkciPCMCardProcess.h"
#include "Common/Utility/TreeNodeNavigate.h"
#include <memory>
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"

namespace StkciPCMCard
{

StkciPCMCardInitializeController::StkciPCMCardInitializeController(StkciPCMCardManager* owner):
			Platform::Controller::BaseCSCIInitializeController(owner),
			ptr_owner(owner)
{
	 AddConfigurationFile("StkciPCMCardConfig","BenchTestStkciPCMCard.xml","../config/swmodules/dbw");
}

Common::InitializeStageStatus StkciPCMCardInitializeController::LoadSpecificParameters()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;
   	std::string tConfigKey;
	using Common::Utility::TreeLoader;
	using Common::Utility::TreeNavigator;

	// reference csci_config consists the address of config file
	const Common::Utility::configFile& csci_config = *ConfigFile("StkciPCMCardConfig");

	tConfigKey = "BenchTestStkciPCMCard.main_thread_period";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.main_thread_period))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestStkciPCMCard.feedback_thread_period";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.feedback_thread_period))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestStkciPCMCard.pcan_port";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.pcan_port))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestStkciPCMCard.Debug.enabled";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.debug.enabled))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestStkciPCMCard.Debug.log_folder";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.debug.log_folder))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestStkciPCMCard.senderid_dbwcmd";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.senderid_dbwcmd))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestStkciPCMCard.senderid_publisher";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.senderid_publisher))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestStkciPCMCard.senderid_basecmd";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.senderid_basecmd))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	//display
	std::cout << "mThreadTimeoutmsec = " << config_params.main_thread_period << std::endl;
	std::cout << "pcan_port = " << config_params.pcan_port << std::endl;
	std::cout << "senderid_dbwcmd = " << config_params.senderid_dbwcmd << std::endl;
	std::cout << "senderid_basecmd = " << config_params.senderid_basecmd << std::endl;

	//
	return ret_status;
}

Common::InitializeStageStatus StkciPCMCardInitializeController::SetupSpecificServices()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;

	ptr_owner->setMainLoopPeriod(config_params.main_thread_period);//ms

	//
	ptr_owner->sptr_RetreiveDBWCmds = std::make_shared<RetreiveDBWCmds>(config_params.senderid_dbwcmd);
	ptr_owner->sptr_RetreiveBaseCmds = std::make_shared<RetreiveBaseCmds>(config_params.senderid_basecmd);
	ptr_owner->sptr_StkciPCMCardProcess->InitSharedPtrs(ptr_owner->sptr_RetreiveDBWCmds,ptr_owner->sptr_RetreiveBaseCmds);
	ptr_owner->sptr_StkciPCMCardProcess->SetConfig(config_params);

	//the comms threads
	int32_t thread_id=11;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveDBWCmds,thread_id,0,10);
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveBaseCmds,thread_id,0,10);

	//
	thread_id++;
	ptr_owner->startThread(std::bind(&StkciPCMCardProcess::get_pallet_feedback_thread,
			ptr_owner->sptr_StkciPCMCardProcess.get()),
			thread_id,
			config_params.feedback_thread_period,10);
	//
	return ret_status;
}

StkciPCMCardInitializeController::~StkciPCMCardInitializeController()
{

}

}
