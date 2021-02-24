/*
 * TestPCMCardManager.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */
#ifndef TESTPCMCARDMANAGER_H_
#define TESTPCMCARDMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"

namespace TestPCMCard
{

class TestPCMCardInitializeController;
class TestPCMCardProcess;

class TestPCMCardManager: public Platform::Controller::BaseCSCIManager
{
	///\class TestPCMCardManager
	///\brief  A class to read a rosbag and publishes message via STKCI

	// allows TestPCMCardController to access to everything in TestPCMCardManager
    friend class TestPCMCardInitializeController;

public:

	TestPCMCardManager(int32_t argc = 0, char **argv = NULL);

	~TestPCMCardManager();

protected:

	///\brief  Clean up Specific Resources
	///\note Defines a pure virtual function from BaseCSCIManager
    void setupSpecificCSCIResources();

	///\brief  Clean up Specific Resources
    ///\note   Defines a pure virtual function from BaseCSCIManager
    void cleanupSpecificCSCIResources();

    ///\brief  Default Liveliness Lost Handler
    ///\note Overrides Definition from BaseManager
    void DefaultThreadLivelinessLostFunc(const int32_t & aThreadID);

private:

     // Used to Control Main Loop Iteration in Intialize State
     std::shared_ptr<TestPCMCardInitializeController> sptr_init_controller;
     std::shared_ptr<TestPCMCardProcess> sptr_TestPCMCardProcess;
};

}
#endif /* ifndef TESTPCMCARDMANAGER_H_ */
