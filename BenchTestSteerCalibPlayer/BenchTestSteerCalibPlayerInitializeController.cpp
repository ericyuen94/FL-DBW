/*
 * BenchTestSteerCalibPlayerInitializeController.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "BenchTestSteerCalibPlayerInitializeController.h"
#include "BenchTestSteerCalibPlayerManager.h"
#include "BenchTestSteerCalibPlayerProcess.h"
#include "Common/Utility/TreeNodeNavigate.h"
#include <memory>

namespace BenchTestSteerCalibPlayer
{

BenchTestSteerCalibPlayerInitializeController::BenchTestSteerCalibPlayerInitializeController(BenchTestSteerCalibPlayerManager* owner):
			Platform::Controller::BaseCSCIInitializeController(owner),
			ptr_owner(owner)
{
	 AddConfigurationFile("BenchTestSteerCalibPlayerConfig","BenchTestSteerCalibPlayer.xml","../config/swmodules/dbw");
}

Common::InitializeStageStatus BenchTestSteerCalibPlayerInitializeController::LoadSpecificParameters()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;
   	std::string tConfigKey;
	using Common::Utility::TreeLoader;
	using Common::Utility::TreeNavigator;

	// reference csci_config consists the address of config file
	const Common::Utility::configFile& csci_config = *ConfigFile("BenchTestSteerCalibPlayerConfig");

	tConfigKey = "BenchTestSteerCalibPlayer.main_thread_period";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.main_thread_period))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestSteerCalibPlayer.Debug.enabled";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.debug.enabled))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "BenchTestSteerCalibPlayer.Debug.log_folder";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.debug.log_folder))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	//display
	std::cout << "mThreadTimeoutmsec = " << config_params.main_thread_period << std::endl;
	std::cout << "debug enabled  = " << config_params.debug.enabled << std::endl;
	std::cout << "debug log_folder  = " << config_params.debug.log_folder << std::endl;

	//
	return ret_status;
}

Common::InitializeStageStatus BenchTestSteerCalibPlayerInitializeController::SetupSpecificServices()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;

	ptr_owner->setMainLoopPeriod(config_params.main_thread_period);//ms
	ptr_owner->sptr_BenchTestSteerCalibPlayerProcess->SetConfig(config_params);
	return ret_status;
}

BenchTestSteerCalibPlayerInitializeController::~BenchTestSteerCalibPlayerInitializeController()
{

}

}
