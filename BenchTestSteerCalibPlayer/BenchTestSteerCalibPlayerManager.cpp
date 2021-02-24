/*
 * BenchTestSteerCalibPlayerManager.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "BenchTestSteerCalibPlayerManager.h"
#include "BenchTestSteerCalibPlayerInitializeController.h"
#include "BenchTestSteerCalibPlayerProcess.h"
//

namespace BenchTestSteerCalibPlayer
{

BenchTestSteerCalibPlayerManager::BenchTestSteerCalibPlayerManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("PCMCard");
}

void BenchTestSteerCalibPlayerManager::setupSpecificCSCIResources()
{
	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<BenchTestSteerCalibPlayerInitializeController>(this);
	 sptr_BenchTestSteerCalibPlayerProcess = std::make_shared<BenchTestSteerCalibPlayerProcess>();

	 setInitializeController(sptr_init_controller); 		// mpInitController is used as Initialize state controller
	 setReadyController(sptr_BenchTestSteerCalibPlayerProcess);   		// controller 0 is used as the ready state functor

}

void BenchTestSteerCalibPlayerManager::cleanupSpecificCSCIResources()
{

}

void BenchTestSteerCalibPlayerManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

BenchTestSteerCalibPlayerManager::~BenchTestSteerCalibPlayerManager()
{

}
}
