/*
 * StkciVecowPCMCardManager.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */
#ifndef StkciVecowPCMCARDMANAGER_H_
#define StkciVecowPCMCARDMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"

namespace StkciVecowPCMCard
{

class StkciVecowPCMCardInitializeController;
class StkciVecowPCMCardProcess;
//
class RetreiveBaseCmds;
class RetreiveDBWCmds;

class StkciVecowPCMCardManager: public Platform::Controller::BaseCSCIManager
{
	///\class StkciVecowPCMCardManager
	///\brief  A class to read a rosbag and publishes message via STKCI

	// allows StkciVecowPCMCardController to access to everything in StkciVecowPCMCardManager
    friend class StkciVecowPCMCardInitializeController;

public:

	StkciVecowPCMCardManager(int32_t argc = 0, char **argv = NULL);

	~StkciVecowPCMCardManager();

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
     std::shared_ptr<StkciVecowPCMCardInitializeController> sptr_init_controller;
     std::shared_ptr<StkciVecowPCMCardProcess> sptr_StkciVecowPCMCardProcess;

     //
     std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds;
     std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds;

};

}
#endif /* ifndef StkciVecowPCMCARDMANAGER_H_ */
