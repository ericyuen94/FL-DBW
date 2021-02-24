/*
 * VCMInitializeController.h
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#ifndef VCMINITIALIZECONTROLLER_H_
#define VCMINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "VCMConfig.h"
//

namespace VCM
{

class VCMManager;

class VCMInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class VCMInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for VCMManager
	VCMInitializeController(VCMManager* owner);

	///\brief Destructor
	~VCMInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	VCMManager *ptr_owner;
    //
	VCMCSCI_Config config_params;
	
};
}
#endif /* ifndef VCMINITIALIZECONTROLLER_H_ */
