/*
 * BenchTestSteerCalibPlayerManager.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */
#ifndef BenchTestSteerCalibPlayerMANAGER_H_
#define BenchTestSteerCalibPlayerMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"

namespace BenchTestSteerCalibPlayer
{

class BenchTestSteerCalibPlayerInitializeController;
class BenchTestSteerCalibPlayerProcess;

class BenchTestSteerCalibPlayerManager: public Platform::Controller::BaseCSCIManager
{
	///\class BenchTestSteerCalibPlayerManager
	///\brief  A class to read a rosbag and publishes message via STKCI

	// allows BenchTestSteerCalibPlayerController to access to everything in BenchTestSteerCalibPlayerManager
    friend class BenchTestSteerCalibPlayerInitializeController;

public:

	BenchTestSteerCalibPlayerManager(int32_t argc = 0, char **argv = NULL);

	~BenchTestSteerCalibPlayerManager();

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
     std::shared_ptr<BenchTestSteerCalibPlayerInitializeController> sptr_init_controller;
     std::shared_ptr<BenchTestSteerCalibPlayerProcess> sptr_BenchTestSteerCalibPlayerProcess;
};

}
#endif /* ifndef BenchTestSteerCalibPlayerMANAGER_H_ */
