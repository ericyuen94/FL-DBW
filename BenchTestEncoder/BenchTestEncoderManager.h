/*
 * BenchTestEncoderManager.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */
#ifndef BENCHTESTENCODERMANAGER_H_
#define BENCHTESTENCODERMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"

namespace BenchTestEncoder
{

class BenchTestEncoderInitializeController;
class BenchTestEncoderProcess;

class BenchTestEncoderManager: public Platform::Controller::BaseCSCIManager
{
	///\class BenchTestEncoderManager
	///\brief  A class to read a rosbag and publishes message via STKCI

	// allows BenchTestEncoderController to access to everything in BenchTestEncoderManager
    friend class BenchTestEncoderInitializeController;

public:

	BenchTestEncoderManager(int32_t argc = 0, char **argv = NULL);

	~BenchTestEncoderManager();

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
     std::shared_ptr<BenchTestEncoderInitializeController> sptr_init_controller;
     std::shared_ptr<BenchTestEncoderProcess> sptr_BenchTestEncoderProcess;
};

}
#endif /* ifndef BenchTestEncoderMANAGER_H_ */
