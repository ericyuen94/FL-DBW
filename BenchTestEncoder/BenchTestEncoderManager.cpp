/*
 * BenchTestEncoderManager.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "BenchTestEncoderManager.h"
#include "BenchTestEncoderInitializeController.h"
#include "BenchTestEncoderProcess.h"
//

namespace BenchTestEncoder
{

BenchTestEncoderManager::BenchTestEncoderManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("PCMCard");
}

void BenchTestEncoderManager::setupSpecificCSCIResources()
{
	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<BenchTestEncoderInitializeController>(this);
	 sptr_BenchTestEncoderProcess = std::make_shared<BenchTestEncoderProcess>();

	 setInitializeController(sptr_init_controller); 		// mpInitController is used as Initialize state controller
	 setReadyController(sptr_BenchTestEncoderProcess);   		// controller 0 is used as the ready state functor

}

void BenchTestEncoderManager::cleanupSpecificCSCIResources()
{

}

void BenchTestEncoderManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

BenchTestEncoderManager::~BenchTestEncoderManager()
{

}
}
