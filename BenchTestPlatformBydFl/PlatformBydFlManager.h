/*
 * PlatformBydFlManager.h
 *
 *  Created on: July 1, 2019
 *      Author: Tester
 */
#ifndef PlatformBydFlMANAGER_H_
#define PlatformBydFlMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"
#include "GetSmartMotorSteerConfiguration.h"


namespace PlatformBydFl
{

class PlatformBydFlInitializeController;
class PlatformBydFlProcess;
//
class RetreiveBaseCmds;
class RetreiveDBWCmds;
class RetreiveTiltMotorFbkMsgs;

class PlatformBydFlManager: public Platform::Controller::BaseCSCIManager
{
	///\class PlatformBydFlManager
	///\brief  A class to read a rosbag and publishes message via STKCI

	// allows PlatformBydFlController to access to everything in SPlatformBydFlManager
    friend class PlatformBydFlInitializeController;

public:

    PlatformBydFlManager(int32_t argc = 0, char **argv = NULL);

	~PlatformBydFlManager();

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
     std::shared_ptr<PlatformBydFlInitializeController> sptr_init_controller;
     std::shared_ptr<PlatformBydFlProcess> sptr_PlatformBydFlProcess;

     //
     std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds;
     std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds;
     std::shared_ptr<RetreiveTiltMotorFbkMsgs> sptr_RetreiveTiltMotorFbkMsgs;

     //
     DBWPalletCommon::SmartMotorSteerConfig config_brake_params;
     //
     bool read_steermotor_config_success;



};

}
#endif /* ifndef PlatformBydFlMANAGER_H_ */
