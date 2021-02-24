/*
 * TeleOpsInitializeController.cpp
 *
 *  Created on: Nov 28, 2017
 *      Author: Emily Low
 */

#include "TeleOpsInitializeController.h"
#include "TeleOpsManager.h"
#include "TeleOpsProcess.h"
#include "Common/Utility/TreeNodeNavigate.h"
#include <memory>

//
#include "RetreiveVIMStatus.h"

namespace TeleOps
{

TeleOpsInitializeController::TeleOpsInitializeController(TeleOpsManager* owner):
			Platform::Controller::BaseCSCIInitializeController(owner),
			ptr_owner(owner)
{
	
}

Common::InitializeStageStatus TeleOpsInitializeController::LoadSpecificParameters()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;
   	

	return ret_status;
}

Common::InitializeStageStatus TeleOpsInitializeController::SetupSpecificServices()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;

	//readyFunction
	ptr_owner->setMainLoopPeriod(100);//ms

	//the comms threads
	int32_t thread_id=11;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveVIMStatus,thread_id,0,10);

	//teleops process thread
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_TeleOpsProcess,thread_id,ptr_owner->config_params.main_thread_period,10);

	//
	return ret_status;
}

TeleOpsInitializeController::~TeleOpsInitializeController()
{

}

}

