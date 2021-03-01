/*
 * VCMManager.h
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */
#ifndef VCMMANAGER_H_
#define VCMMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"
#include "VCMConfig.h"
#include "Platform/PlatformConstants.h"
//
namespace VCM
{

class VCMInitializeController;
class VCMProcess;
//
class RetreiveVIMStatus;
class RetrievePLMStatus;
class RetrieveVcmCmd;
class RetrieveDBWFeedback;

//
class VCMManager: public Platform::Controller::BaseCSCIManager
{
	///\class VCMManager
	///\brief  A class to coordinate the actions for pallet VCM

	// allows VCMController to access to everything in VCMManager
    friend class VCMInitializeController;

public:

	VCMManager(int32_t argc = 0, char **argv = NULL);

	~VCMManager();

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

    //
    void readyFunction();

private:

     // Used to Control Main Loop Iteration in Intialize State
     std::shared_ptr<VCMInitializeController> sptr_init_controller;
     std::shared_ptr<VCMProcess> sptr_VCMProcess;

     //
     std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus;
     std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMStatus;
     std::shared_ptr<RetrieveVcmCmd> sptr_RetrieveVcmCmd;
     std::shared_ptr<RetrieveVcmCmd> sptr_RetrieveVcmCmd_NTU;
     std::shared_ptr<RetrieveDBWFeedback> sptr_RetrieveDBWFeedback;

     //
     uint32_t plm_warning_status[Platform::PLM_STATUS_TOTAL_BITS];
     uint32_t plm_error_status[Platform::PLM_STATUS_TOTAL_BITS];

};

}
#endif /* ifndef VCMMANAGER_H_ */
