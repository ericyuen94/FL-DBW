/*
 * BenchTestPCanPlayerManager.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */
#ifndef BENCHTESTPCANPLAYERMANAGER_H_
#define BENCHTESTPCANPLAYERMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"

namespace BenchTestPCanPlayer
{

class BenchTestPCanPlayerInitializeController;
class BenchTestPCanPlayerProcess;

class BenchTestPCanPlayerManager: public Platform::Controller::BaseCSCIManager
{
	///\class BenchTestPCanPlayerManager
	///\brief  A class to read a rosbag and publishes message via STKCI

	// allows BenchTestPCanPlayerController to access to everything in BenchTestPCanPlayerManager
    friend class BenchTestPCanPlayerInitializeController;

public:

	BenchTestPCanPlayerManager(int32_t argc = 0, char **argv = NULL);

	~BenchTestPCanPlayerManager();

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
     std::shared_ptr<BenchTestPCanPlayerInitializeController> sptr_init_controller;
     std::shared_ptr<BenchTestPCanPlayerProcess> sptr_BenchTestPCanPlayerProcess;
};

}
#endif /* ifndef BenchTestPCanPlayerMANAGER_H_ */
