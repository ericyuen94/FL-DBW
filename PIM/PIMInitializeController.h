/*
 * PIMInitializeController.h
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#ifndef PIMINITIALIZECONTROLLER_H_
#define PIMINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "PIMConfig.h"
//

namespace PIM
{

class PIMManager;

class PIMInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class PIMInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for PIMManager
	PIMInitializeController(PIMManager* owner);

	///\brief Destructor
	~PIMInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	PIMManager *ptr_owner;
	PIMCSCI_Config config_params;
	
};
}
#endif /* ifndef PIMINITIALIZECONTROLLER_H_ */
