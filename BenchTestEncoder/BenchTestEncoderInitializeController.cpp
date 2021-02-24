/*
 * BenchTestEncoderInitializeController.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "BenchTestEncoderInitializeController.h"
#include "BenchTestEncoderManager.h"
#include "BenchTestEncoderProcess.h"
#include "Common/Utility/TreeNodeNavigate.h"
#include <memory>

namespace BenchTestEncoder
{

BenchTestEncoderInitializeController::BenchTestEncoderInitializeController(BenchTestEncoderManager* owner):
			Platform::Controller::BaseCSCIInitializeController(owner),
			ptr_owner(owner)
{
	 AddConfigurationFile("BenchTestEncoderConfig","BenchTestEncoder.xml","../config/swmodules/dbw");
}

Common::InitializeStageStatus BenchTestEncoderInitializeController::LoadSpecificParameters()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;
   	std::string tConfigKey;
	using Common::Utility::TreeLoader;
	using Common::Utility::TreeNavigator;

	// reference csci_config consists the address of config file
	const Common::Utility::configFile& csci_config = *ConfigFile("BenchTestEncoderConfig");

	tConfigKey = "BenchTestEncoder.main_thread_period";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.main_thread_period))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestEncoder.pcan_port";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.pcan_port))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	//display
	std::cout << "mThreadTimeoutmsec = " << config_params.main_thread_period << std::endl;
	std::cout << "pcan_port = " << config_params.pcan_port << std::endl;
	//
	return ret_status;
}

Common::InitializeStageStatus BenchTestEncoderInitializeController::SetupSpecificServices()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;

	ptr_owner->setMainLoopPeriod(config_params.main_thread_period);//ms
	ptr_owner->sptr_BenchTestEncoderProcess->SetConfig(config_params);
	return ret_status;
}

BenchTestEncoderInitializeController::~BenchTestEncoderInitializeController()
{

}

}
