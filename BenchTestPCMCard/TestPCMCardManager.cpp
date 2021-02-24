/*
 * TestPCMCardManager.cpp
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#include "TestPCMCardManager.h"
#include "TestPCMCardInitializeController.h"
#include "TestPCMCardProcess.h"
//

namespace TestPCMCard
{

TestPCMCardManager::TestPCMCardManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("PCMCard");
}

void TestPCMCardManager::setupSpecificCSCIResources()
{
	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<TestPCMCardInitializeController>(this);
	 sptr_TestPCMCardProcess = std::make_shared<TestPCMCardProcess>();

	 setInitializeController(sptr_init_controller); 	// mpInitController is used as Initialize state controller
	 setReadyController(sptr_TestPCMCardProcess);   		// controller 0 is used as the ready state functor

}

void TestPCMCardManager::cleanupSpecificCSCIResources()
{

}

void TestPCMCardManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

TestPCMCardManager::~TestPCMCardManager()
{

}
}
