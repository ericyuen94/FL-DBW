/*
 * PlatformForkliftInitializeController.h
 *
 *  Created on: Aug 1, 2018
 *      Author: Kian Wee
 */

#ifndef PLATFORMFORKLIFTINITIALIZECONTROLLER_H_
#define PLATFORMFORKLIFTINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "PlatformForkliftConfiguration.h"
//

namespace PlatformForklift
{

class PlatformForkliftManager;

class PlatformForkliftInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class PlatformForkliftInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for StkciVecowPCMCardManager
	PlatformForkliftInitializeController(PlatformForkliftManager* owner);

	///\brief Destructor
	~PlatformForkliftInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	PlatformForkliftManager *ptr_owner;

	PlatformForkliftConfig config_params;
};
}
#endif /* ifndef PLATFORMFORKLIFTINITIALIZECONTROLLER_H_ */
