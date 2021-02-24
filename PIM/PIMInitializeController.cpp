/*
 * PIMInitializeController.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#include "PIMInitializeController.h"
#include "PIMManager.h"
#include "PIMProcess.h"
#include "PIMTxCommsProcess.h"
#include "RetreivePIMCmd.h"
#include "Common/Utility/TreeNodeNavigate.h"
#include <memory>

namespace PIM
{

PIMInitializeController::PIMInitializeController(PIMManager* owner):
			Platform::Controller::BaseCSCIInitializeController(owner),
			ptr_owner(owner)
{
	 AddConfigurationFile("PIM","PIM.xml","../config/swmodules/dbw");
}

Common::InitializeStageStatus PIMInitializeController::LoadSpecificParameters()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;
   	
	std::string tConfigKey;
	using Common::Utility::TreeLoader;
	using Common::Utility::TreeNavigator;

	// reference csci_config consists the address of config file
	const Common::Utility::configFile& csci_config = *ConfigFile("PIM");

	std::cout << "Load Parameters" << std::endl;

	tConfigKey = "PIM.main_thread_period";
	if (!TreeLoader::GetParam(*csci_config.GetCurrentReferenceNode(),
							  tConfigKey, config_params.main_thread_period))
	{
		ret_status = Common::InitializeStageStatus_Failed;
		AppendErrorString("Failed to Find <"+tConfigKey+">");
	}

	//display
	std::cout << "mThreadTimeoutmsec = " << config_params.main_thread_period << std::endl;

	return ret_status;
}

Common::InitializeStageStatus PIMInitializeController::SetupSpecificServices()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;

	//
	ptr_owner->setMainLoopPeriod(100);//ms
	ptr_owner->sptr_PIMProcess->SetConfigParams(config_params);

	//Set CAN Transmit thread rate 100ms
    int32_t thread_id = 11;
    ptr_owner->startThread(*ptr_owner->sptr_PIMTxCommsProcess, thread_id, 100, 10);

	//the comms threads
    thread_id++;
    ptr_owner->startThread(ptr_owner->sptr_RetreivePIMCmd, thread_id, 0, 10);

    //process
    thread_id++;
    ptr_owner->startThread(ptr_owner->sptr_PIMProcess, thread_id, config_params.main_thread_period, 10);


	return ret_status;
}

PIMInitializeController::~PIMInitializeController()
{

}

}

