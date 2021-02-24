/*
 * PlatformStackerManager.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "PlatformStackerManager.h"
#include "PlatformStackerInitializeController.h"
#include "PlatformStackerProcess.h"
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"
//

namespace PlatformStacker
{

PlatformStackerManager::PlatformStackerManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("PCMCard");
}

void PlatformStackerManager::setupSpecificCSCIResources()
{
	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<PlatformStackerInitializeController>(this);
	 sptr_PlatformStackerProcess = std::make_shared<PlatformStackerProcess>();

	 setInitializeController(sptr_init_controller); 	// mpInitController is used as Initialize state controller
	 setReadyController(sptr_PlatformStackerProcess);   		// controller 0 is used as the ready state functor

}

void PlatformStackerManager::cleanupSpecificCSCIResources()
{

}

void PlatformStackerManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

PlatformStackerManager::~PlatformStackerManager()
{

}
}
