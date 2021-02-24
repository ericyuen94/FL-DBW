/*
 * TeleOpsInitializeController.h
 *
 *  Created on: Nov 28, 2017
 *      Author: Emily Low
 */

#ifndef TELEOPSINITIALIZECONTROLLER_H_
#define TELEOPSINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "TeleOpsCSCIConfiguration.h"
//

namespace TeleOps
{

class TeleOpsManager;

class TeleOpsInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class TeleOpsInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for TeleOpsManager
	TeleOpsInitializeController(TeleOpsManager* owner);

	///\brief Destructor
	~TeleOpsInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	TeleOpsManager *ptr_owner;
	
};
}
#endif /* ifndef TELEOPSINITIALIZECONTROLLER_H_ */
