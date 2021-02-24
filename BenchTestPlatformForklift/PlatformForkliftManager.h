/*
 * PlatformForkliftManager.h
 *
 *  Created on: Aug 1, 2018
 *      Author: Kian Wee
 */
#ifndef PLATFORMFORKLIFTMANAGER_H_
#define PLATFORMFORKLIFTMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"

namespace PlatformForklift
{

class PlatformForkliftInitializeController;
class PlatformForkliftProcess;
//
class RetreiveBaseCmds;
class RetreiveDBWCmds;

class PlatformForkliftManager: public Platform::Controller::BaseCSCIManager
{
	///\class PlatformForkliftManager
	///\brief  A class to read a rosbag and publishes message via STKCI

	// allows PlatformForkliftController to access to everything in SPlatformForkliftManager
    friend class PlatformForkliftInitializeController;

public:

    PlatformForkliftManager(int32_t argc = 0, char **argv = NULL);

	~PlatformForkliftManager();

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
     std::shared_ptr<PlatformForkliftInitializeController> sptr_init_controller;
     std::shared_ptr<PlatformForkliftProcess> sptr_PlatformForkliftProcess;

     //
     std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds;
     std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds;

};

}
#endif /* ifndef PLATFORMFORKLIFTMANAGER_H_ */
