/*
 * BenchTestEncoderInitializeController.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef BENCHTESTENCODERINITIALIZECONTROLLER_H_
#define BENCHTESTENCODERINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "BenchTestEncoderConfiguration.h"
//

namespace BenchTestEncoder
{

class BenchTestEncoderManager;

class BenchTestEncoderInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class BenchTestEncoderInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for BenchTestEncoderManager
	BenchTestEncoderInitializeController(BenchTestEncoderManager* owner);

	///\brief Destructor
	~BenchTestEncoderInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	BenchTestEncoderManager *ptr_owner;

	BenchTestEncoderConfig config_params;
};
}
#endif /* ifndef BenchTestEncoderINITIALIZECONTROLLER_H_ */
