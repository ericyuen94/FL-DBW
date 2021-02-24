/*
 * PlatformStackerManager.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */
#ifndef PlatformStackerMANAGER_H_
#define PlatformStackerMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"

namespace PlatformStacker
{

class PlatformStackerInitializeController;
class PlatformStackerProcess;
class RetreiveBaseCmds;
class RetreiveDBWCmds;

class PlatformStackerManager: public Platform::Controller::BaseCSCIManager
{
	///\class PlatformStackerManager
	///\brief  A class to read a rosbag and publishes message via STKCI

	// allows PlatformStackerController to access to everything in PlatformStackerManager
    friend class PlatformStackerInitializeController;

public:

	PlatformStackerManager(int32_t argc = 0, char **argv = NULL);

	~PlatformStackerManager();

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
     std::shared_ptr<PlatformStackerInitializeController> sptr_init_controller;
     std::shared_ptr<PlatformStackerProcess> sptr_PlatformStackerProcess;

     //
     std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds;
     std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds;
};

}
#endif /* ifndef PlatformStackerMANAGER_H_ */
