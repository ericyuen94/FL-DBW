/*
 * BenchTestPCanPlayerManager.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "BenchTestPCanPlayerManager.h"
#include "BenchTestPCanPlayerInitializeController.h"
#include "BenchTestPCanPlayerProcess.h"
//

namespace BenchTestPCanPlayer
{

BenchTestPCanPlayerManager::BenchTestPCanPlayerManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("PCMCard");
}

void BenchTestPCanPlayerManager::setupSpecificCSCIResources()
{
	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<BenchTestPCanPlayerInitializeController>(this);
	 sptr_BenchTestPCanPlayerProcess = std::make_shared<BenchTestPCanPlayerProcess>();

	 setInitializeController(sptr_init_controller); 		// mpInitController is used as Initialize state controller
	 setReadyController(sptr_BenchTestPCanPlayerProcess);   		// controller 0 is used as the ready state functor

}

void BenchTestPCanPlayerManager::cleanupSpecificCSCIResources()
{

}

void BenchTestPCanPlayerManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

BenchTestPCanPlayerManager::~BenchTestPCanPlayerManager()
{

}
}
