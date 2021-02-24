/*
 * StkciVecowPCMCardInitializeController.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef StkciVecowPCMCARDINITIALIZECONTROLLER_H_
#define StkciVecowPCMCARDINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "StkciVecowPCMCardConfiguration.h"
//

namespace StkciVecowPCMCard
{

class StkciVecowPCMCardManager;

class StkciVecowPCMCardInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class StkciVecowPCMCardInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for StkciVecowPCMCardManager
	StkciVecowPCMCardInitializeController(StkciVecowPCMCardManager* owner);

	///\brief Destructor
	~StkciVecowPCMCardInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	StkciVecowPCMCardManager *ptr_owner;

	StkciVecowPCMCardConfig config_params;
};
}
#endif /* ifndef StkciVecowPCMCARDINITIALIZECONTROLLER_H_ */
