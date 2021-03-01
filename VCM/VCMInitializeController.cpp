/*
 * VCMInitializeController.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#include "VCMInitializeController.h"
#include "VCMManager.h"
#include "VCMProcess.h"
#include "Common/Utility/TreeNodeNavigate.h"
#include <memory>
//
#include "RetreiveVIMStatus.h"
#include "RetrievePLMStatus.h"
#include "RetrieveVcmCmd.h"
#include "VCMProcess.h"

namespace VCM
{

VCMInitializeController::VCMInitializeController(VCMManager* owner):
			Platform::Controller::BaseCSCIInitializeController(owner),
			ptr_owner(owner)
{
	 AddConfigurationFile("VCM","VCM.xml","../config/swmodules/dbw");
}

Common::InitializeStageStatus VCMInitializeController::LoadSpecificParameters()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;
   	
	std::string tConfigKey;
	using Common::Utility::TreeLoader;
	using Common::Utility::TreeNavigator;

	// reference csci_config consists the address of config file
	const Common::Utility::configFile& csci_config = *ConfigFile("VCM");

	tConfigKey = "VCM.main_thread_period";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.main_thread_period))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "VCM.max_speed";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.max_speed))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "VCM.max_steer";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.max_steer))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	tConfigKey = "VCM.vcm_trajdemo";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey,config_params.vcm_trajdemo))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	//display
	std::cout << "mThreadTimeoutmsec = " << config_params.main_thread_period << std::endl;
	std::cout << "max_speed = " << config_params.max_speed << std::endl;
	std::cout << "max_steer = " << config_params.max_steer << std::endl;
	std::cout << "vcm_trajdemo = " << config_params.vcm_trajdemo << std::endl;
    //
    ptr_owner->sptr_VCMProcess->SetConfigParams(config_params);

	return ret_status;
}

Common::InitializeStageStatus VCMInitializeController::SetupSpecificServices()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;

	//readyFunction
	ptr_owner->setMainLoopPeriod(100);//ms
	//
	int32_t thread_id=11;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveVIMStatus,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetrievePLMStatus,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetrieveVcmCmd,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetrieveVcmCmd_NTU,thread_id,0,10);
	//vcm process thread
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_VCMProcess,thread_id,config_params.main_thread_period,10);
	thread_id++;
	ptr_owner->startThread(std::bind(&VCMProcess::GetUserInput_Thread,ptr_owner->sptr_VCMProcess.get()),thread_id,config_params.main_thread_period,10);
	//
	return ret_status;
}

VCMInitializeController::~VCMInitializeController()
{

}

}

