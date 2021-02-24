/*
 * StkciPCMCardInitializeController.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef STKCIPCMCARDINITIALIZECONTROLLER_H_
#define STKCIPCMCARDINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "StkciPCMCardConfiguration.h"
//

namespace StkciPCMCard
{

class StkciPCMCardManager;

class StkciPCMCardInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class StkciPCMCardInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for StkciPCMCardManager
	StkciPCMCardInitializeController(StkciPCMCardManager* owner);

	///\brief Destructor
	~StkciPCMCardInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	StkciPCMCardManager *ptr_owner;

	StkciPCMCardConfig config_params;
};
}
#endif /* ifndef STKCIPCMCARDINITIALIZECONTROLLER_H_ */
