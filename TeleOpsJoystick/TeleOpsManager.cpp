/*
 * TeleOpsManager.cpp
 *
 *  Created on: Nov 28, 2017
 *      Author: Emily Low
 */

#include "TeleOpsManager.h"
#include "TeleOpsInitializeController.h"
#include "TeleOpsProcess.h"
//
#include "RetreiveVIMStatus.h"

//
namespace TeleOps
{

TeleOpsManager::TeleOpsManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("TeleOps");
	read_config_success = false;
}

void TeleOpsManager::setupSpecificCSCIResources()
{
	 // Get configuration data
	 GetTeleOpsCSCIConfiguration cGetTeleOpsConfig;
	 bool open_result = cGetTeleOpsConfig.OpenViewerConfiguration();
	 if(open_result)
	 {
		 cGetTeleOpsConfig.GetViewerConfigData(config_params);
		 read_config_success = true;
	 }

	 //Set shared pointers
	 sptr_RetreiveVIMStatus = std::make_shared<RetreiveVIMStatus>(config_params.SubStkciMsgs["PIMStatusMsg"].senderid);

	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<TeleOpsInitializeController>(this);
	 sptr_TeleOpsProcess = std::make_shared<TeleOpsProcess>(sptr_RetreiveVIMStatus);
	 sptr_TeleOpsProcess->SetConfigParams(config_params);

	 //
	 setInitializeController(sptr_init_controller); 	// mpInitController is used as Initialize state controller
	 setReadyFunction(std::bind(&TeleOpsManager::readyFunction, this));
}

void TeleOpsManager::readyFunction()
{
	//clear the flags
	for(int32_t i=0; i<Platform::PLM_STATUS_TOTAL_BITS; i++)
	{
		plm_warning_status[i]=0;
		plm_error_status[i]=0;
	}

	//
	dbw_liveness_status health_status;
	sptr_TeleOpsProcess->GetPLMHealthStatus(health_status);

	//
	if(health_status.error_joystick_notconnected)
	{
		plm_error_status[1]=1;
	}

	//
	if((health_status.receive_vimstatus_msg == false) && (health_status.not_receive_vimstatus_msg_count>10))
	{
		plm_error_status[2]=1;
	}

	//Send to PLM
	ResetWarning();
	ResetError();
	for(int32_t i=0; i<Platform::PLM_STATUS_TOTAL_BITS; i++)
	{
		if(plm_warning_status[i]==1)
		{
			UpdateWarning(i,true);
		}
		if(plm_error_status[i]==1)
		{
			UpdateError(i,true);
		}
	}
}

void TeleOpsManager::cleanupSpecificCSCIResources()
{

}

void TeleOpsManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

TeleOpsManager::~TeleOpsManager()
{

}
}
