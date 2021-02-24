/*
 * PlatformForkliftManager.cpp
 *
 *  Created on: Aug 1, 2018
 *      Author: Kian Wee
 */

#include "PlatformForkliftManager.h"
#include "PlatformForkliftInitializeController.h"
#include "PlatformForkliftProcess.h"
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"
//
namespace PlatformForklift
{

PlatformForkliftManager::PlatformForkliftManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("PCMCard");
}

void PlatformForkliftManager::setupSpecificCSCIResources()
{
	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<PlatformForkliftInitializeController>(this);
	 sptr_PlatformForkliftProcess = std::make_shared<PlatformForkliftProcess>();

	 //
	 setInitializeController(sptr_init_controller); 		// mpInitController is used as Initialize state controller
	 setReadyController(sptr_PlatformForkliftProcess);   		// controller 0 is used as the ready state functor

}

void PlatformForkliftManager::cleanupSpecificCSCIResources()
{
	sptr_PlatformForkliftProcess->Shutdown();}

void PlatformForkliftManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

PlatformForkliftManager::~PlatformForkliftManager()
{

}
}
