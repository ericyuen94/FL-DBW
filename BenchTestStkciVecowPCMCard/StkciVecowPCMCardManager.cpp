/*
 * StkciVecowPCMCardManager.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "StkciVecowPCMCardManager.h"
#include "StkciVecowPCMCardInitializeController.h"
#include "StkciVecowPCMCardProcess.h"
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"
//
namespace StkciVecowPCMCard
{

StkciVecowPCMCardManager::StkciVecowPCMCardManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("PCMCard");
}

void StkciVecowPCMCardManager::setupSpecificCSCIResources()
{
	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<StkciVecowPCMCardInitializeController>(this);
	 sptr_StkciVecowPCMCardProcess = std::make_shared<StkciVecowPCMCardProcess>();

	 //
	 setInitializeController(sptr_init_controller); 		// mpInitController is used as Initialize state controller
	 setReadyController(sptr_StkciVecowPCMCardProcess);   		// controller 0 is used as the ready state functor

}

void StkciVecowPCMCardManager::cleanupSpecificCSCIResources()
{
	sptr_StkciVecowPCMCardProcess->ShutdownPCM();}

void StkciVecowPCMCardManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

StkciVecowPCMCardManager::~StkciVecowPCMCardManager()
{

}
}
