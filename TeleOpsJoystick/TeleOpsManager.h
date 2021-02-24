/*
 * TeleOpsManager.h
 *
 *  Created on: Nov 28, 2017
 *      Author: Emily Low
 */
#ifndef TELEOPSMANAGER_H_
#define TELEOPSMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"
#include "GetTeleOpsCSCIConfiguration.h"
#include "Platform/PlatformConstants.h"
//
namespace TeleOps
{

class TeleOpsInitializeController;
class TeleOpsProcess;
//
class RetreiveVIMStatus;

//
class TeleOpsManager: public Platform::Controller::BaseCSCIManager
{
	///\class TeleOpsManager
	///\brief  A class to coordinate the actions for pallet TeleOps

	// allows TeleOpsController to access to everything in TeleOpsManager
    friend class TeleOpsInitializeController;

public:

	TeleOpsManager(int32_t argc = 0, char **argv = NULL);

	~TeleOpsManager();

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

    void readyFunction();

private:

     // Used to Control Main Loop Iteration in Intialize State
     std::shared_ptr<TeleOpsInitializeController> sptr_init_controller;
     std::shared_ptr<TeleOpsProcess> sptr_TeleOpsProcess;

     //STKCI
     std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus;

     //
     TeleOpsCSCI_Config config_params;
     bool read_config_success;

     //
     uint32_t plm_warning_status[Platform::PLM_STATUS_TOTAL_BITS];
     uint32_t plm_error_status[Platform::PLM_STATUS_TOTAL_BITS];
};

}
#endif /* ifndef TELEOPSMANAGER_H_ */
