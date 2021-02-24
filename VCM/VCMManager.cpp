/*
 * VCMManager.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#include "VCMManager.h"
#include "VCMInitializeController.h"
#include "VCMProcess.h"
//
#include "RetreiveVIMStatus.h"
#include "RetrievePLMStatus.h"
#include "RetrieveVcmCmd.h"

namespace VCM
{

VCMManager::VCMManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("VCM");
}

void VCMManager::setupSpecificCSCIResources()
{
	// Create all the Manager's Resources
	sptr_init_controller = std::make_shared<VCMInitializeController>(this);
	sptr_RetreiveVIMStatus = std::make_shared<RetreiveVIMStatus>("PIM");
	sptr_RetrievePLMStatus = std::make_shared<RetrievePLMStatus>("PLM");
	sptr_RetrieveVcmCmd = std::make_shared<RetrieveVcmCmd>("PathFollower");
	sptr_RetrieveVcmCmd_NTU = std::make_shared<RetrieveVcmCmd>("NTUTrigger");
	sptr_VCMProcess = std::make_shared<VCMProcess>(sptr_RetreiveVIMStatus,sptr_RetrievePLMStatus,sptr_RetrieveVcmCmd,sptr_RetrieveVcmCmd_NTU);

	//
	setInitializeController(sptr_init_controller); 	// mpInitController is used as Initialize state controller
	setReadyFunction(std::bind(&VCMManager::readyFunction, this));
}

void VCMManager::readyFunction()
{
	//clear the flags
	for(int32_t i=0; i<Platform::PLM_STATUS_TOTAL_BITS; i++)
	{
		plm_warning_status[i]=0;
		plm_error_status[i]=0;
	}

	//
	platform_liveness_status status;
	sptr_VCMProcess->GetPLMHealthStatus(status);
	//
	if((!status.receive_plmstatus_msg) && (status.error_receive_plmstatus_msg>10))
	{
		plm_error_status[1]=1;
	}
	//
	if((!status.receive_vimstatus_msg) && (status.error_receive_vimstatus_msg_count>10))
	{
		plm_error_status[2]=1;
	}
	//
	if(status.error_receive_pfm_msg_count_automode>10)
	{
		plm_error_status[3]=1;
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

void VCMManager::cleanupSpecificCSCIResources()
{

}

void VCMManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

VCMManager::~VCMManager()
{

}
}
