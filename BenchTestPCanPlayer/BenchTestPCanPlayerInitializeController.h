/*
 * BenchTestPCanPlayerInitializeController.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef BENCHTESTPCANPLAYERINITIALIZECONTROLLER_H_
#define BENCHTESTPCANPLAYERINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "BenchTestPCanPlayerConfiguration.h"
//

namespace BenchTestPCanPlayer
{

class BenchTestPCanPlayerManager;

class BenchTestPCanPlayerInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class BenchTestPCanPlayerInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for BenchTestPCanPlayerManager
	BenchTestPCanPlayerInitializeController(BenchTestPCanPlayerManager* owner);

	///\brief Destructor
	~BenchTestPCanPlayerInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	BenchTestPCanPlayerManager *ptr_owner;

	BenchTestPCanPlayerConfig config_params;
};
}
#endif /* ifndef BenchTestPCanPlayerINITIALIZECONTROLLER_H_ */
