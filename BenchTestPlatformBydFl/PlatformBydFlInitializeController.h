/*
 * PlatformBydFlInitializeController.h
 *
 *  Created on: July 1, 2019
 *      Author: Tester
 */

#ifndef PlatformBydFlINITIALIZECONTROLLER_H_
#define PlatformBydFlINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "PlatformBydFlConfiguration.h"
//

namespace PlatformBydFl
{

class PlatformBydFlManager;

class PlatformBydFlInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class PlatformBydFlInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for StkciVecowPCMCardManager
	PlatformBydFlInitializeController(PlatformBydFlManager* owner);

	///\brief Destructor
	~PlatformBydFlInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	PlatformBydFlManager *ptr_owner;

	PlatformBydFlConfig config_params;
};
}
#endif /* ifndef PlatformBydFlINITIALIZECONTROLLER_H_ */
