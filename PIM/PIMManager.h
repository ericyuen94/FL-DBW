/*
 * PIMManager.h
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */
#ifndef PIMMANAGER_H_
#define PIMMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"
#include "Platform/IsolatedDIO/IsolatedDIO.h"
#include "Platform/PlatformConstants.h"
//
namespace PIM
{

class PIMInitializeController;
class PIMProcess;
class PIMTxCommsProcess;
class RetreivePIMCmd;

//
class PIMManager: public Platform::Controller::BaseCSCIManager
{
	///\class PIMManager
	///\brief  A class to coordinate the actions for pallet PIM

	// allows PIMController to access to everything in PIMManager
    friend class PIMInitializeController;

public:

	PIMManager(int32_t argc = 0, char **argv = NULL);

	~PIMManager();

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
     std::shared_ptr<PIMInitializeController> sptr_init_controller;
     std::shared_ptr<PIMProcess> sptr_PIMProcess;

     // CAN Transmit Control
     std::shared_ptr<PIMTxCommsProcess> sptr_PIMTxCommsProcess;

     // Isolated DIO pointer
     std::shared_ptr<Platform::IsolatedDIO::IsolatedDIODriver> sptr_DIO;

     //
     std::shared_ptr<RetreivePIMCmd> sptr_RetreivePIMCmd;

     //
     uint32_t plm_warning_status[Platform::PLM_STATUS_TOTAL_BITS];
     uint32_t plm_error_status[Platform::PLM_STATUS_TOTAL_BITS];
};

}
#endif /* ifndef PIMMANAGER_H_ */
