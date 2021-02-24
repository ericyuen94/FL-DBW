/*
 * PIMManager.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#include "PIMManager.h"
#include "PIMInitializeController.h"
#include "PIMProcess.h"
#include "PIMTxCommsProcess.h"
#include "RetreivePIMCmd.h"
//

namespace PIM
{

PIMManager::PIMManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("PIM");
}

void PIMManager::setupSpecificCSCIResources()
{
	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<PIMInitializeController>(this);
	 sptr_DIO = std::make_shared<Platform::IsolatedDIO::IsolatedDIODriver>();

	 //
	 sptr_RetreivePIMCmd = std::make_shared<RetreivePIMCmd>("DBW");
	 sptr_PIMProcess = std::make_shared<PIMProcess>( sptr_DIO );
	 sptr_PIMTxCommsProcess = std::make_shared<PIMTxCommsProcess>(sptr_DIO,sptr_RetreivePIMCmd);

	 //
	 setInitializeController(sptr_init_controller); 	// mpInitController is used as Initialize state controller
	 setReadyFunction(std::bind(&PIMManager::readyFunction, this));
}

void PIMManager::readyFunction()
{
	//clear the flags
	for(int32_t i=0; i<Platform::PLM_STATUS_TOTAL_BITS; i++)
	{
		plm_warning_status[i]=0;
		plm_error_status[i]=0;
	}

	//
	healthstatus_pim_fb health_healthstatus_pim_fb;
	sptr_PIMProcess->GetPIMProcessStatus(health_healthstatus_pim_fb);
	healthstatus_pim_tx health_healthstatus_pim_tx;
	sptr_PIMTxCommsProcess->GetPIMTxCommsProcessHealth(health_healthstatus_pim_tx);

	//
	if(!health_healthstatus_pim_fb.bConnectedDIO)
	{
		plm_error_status[1]=1;
	}
	//
	if(health_healthstatus_pim_fb.bConnectedDIO)
	{
		//
		if(health_healthstatus_pim_tx.bError_SetDIOCmds)
		{
			plm_error_status[2]=1;
		}
		//
		if(health_healthstatus_pim_tx.bError_RxPIMCmdMsg)
		{
			plm_error_status[3]=1;
		}
		//
		if(!health_healthstatus_pim_fb.bSuccessReadDIO)
		{
			plm_error_status[4]=1;
		}
		//
		if(!health_healthstatus_pim_fb.bFlexisoftStateOK)
		{
			plm_error_status[6]=1;
			std::cout << "[Error 6] Flexi soft state different from CPU state" << std::endl;
		}
	}

	//IES Only
	healthstatus_pim_ies health_ies;
	sptr_PIMProcess->GetIesStatus(health_ies);
	if(!health_ies.ignoreIES && !health_ies.is_ready)
	{
		plm_error_status[5]=1;
	}
	if(health_ies.ignoreIES)
	{
		plm_warning_status[1]=1;
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

void PIMManager::cleanupSpecificCSCIResources()
{

}

void PIMManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

PIMManager::~PIMManager()
{

}
}
