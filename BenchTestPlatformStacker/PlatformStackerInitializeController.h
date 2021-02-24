/*
 * PlatformStackerInitializeController.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef PlatformStackerINITIALIZECONTROLLER_H_
#define PlatformStackerINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "PlatformStackerConfiguration.h"
//

namespace PlatformStacker
{

class PlatformStackerManager;

class PlatformStackerInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class PlatformStackerInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for PlatformStackerManager
	PlatformStackerInitializeController(PlatformStackerManager* owner);

	///\brief Destructor
	~PlatformStackerInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	PlatformStackerManager *ptr_owner;

	PlatformStackerConfig config_params;
};
}
#endif /* ifndef PlatformStackerINITIALIZECONTROLLER_H_ */
