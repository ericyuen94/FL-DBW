/*
 * StkciPCMCardManager.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "StkciPCMCardManager.h"
#include "StkciPCMCardInitializeController.h"
#include "StkciPCMCardProcess.h"
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"
//
namespace StkciPCMCard
{

StkciPCMCardManager::StkciPCMCardManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("PCMCard");
}

void StkciPCMCardManager::setupSpecificCSCIResources()
{
	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<StkciPCMCardInitializeController>(this);
	 sptr_StkciPCMCardProcess = std::make_shared<StkciPCMCardProcess>();

	 //
	 setInitializeController(sptr_init_controller); 		// mpInitController is used as Initialize state controller
	 setReadyController(sptr_StkciPCMCardProcess);   		// controller 0 is used as the ready state functor

}

void StkciPCMCardManager::cleanupSpecificCSCIResources()
{
	sptr_StkciPCMCardProcess->ShutdownPCM();}

void StkciPCMCardManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

StkciPCMCardManager::~StkciPCMCardManager()
{

}
}
