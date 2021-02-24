/*
 * StkciPCMCardManager.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */
#ifndef STKCIPCMCARDMANAGER_H_
#define STKCIPCMCARDMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"

namespace StkciPCMCard
{

class StkciPCMCardInitializeController;
class StkciPCMCardProcess;
//
class RetreiveBaseCmds;
class RetreiveDBWCmds;

class StkciPCMCardManager: public Platform::Controller::BaseCSCIManager
{
	///\class StkciPCMCardManager
	///\brief  A class to read a rosbag and publishes message via STKCI

	// allows StkciPCMCardController to access to everything in StkciPCMCardManager
    friend class StkciPCMCardInitializeController;

public:

	StkciPCMCardManager(int32_t argc = 0, char **argv = NULL);

	~StkciPCMCardManager();

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
     std::shared_ptr<StkciPCMCardInitializeController> sptr_init_controller;
     std::shared_ptr<StkciPCMCardProcess> sptr_StkciPCMCardProcess;

     //
     std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds;
     std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds;

};

}
#endif /* ifndef STKCIPCMCARDMANAGER_H_ */
