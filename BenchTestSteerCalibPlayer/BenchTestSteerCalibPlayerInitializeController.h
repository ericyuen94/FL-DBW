/*
 * BenchTestSteerCalibPlayerInitializeController.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef BenchTestSteerCalibPlayerINITIALIZECONTROLLER_H_
#define BenchTestSteerCalibPlayerINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "BenchTestSteerCalibPlayerConfiguration.h"
//

namespace BenchTestSteerCalibPlayer
{

class BenchTestSteerCalibPlayerManager;

class BenchTestSteerCalibPlayerInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class BenchTestSteerCalibPlayerInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for BenchTestSteerCalibPlayerManager
	BenchTestSteerCalibPlayerInitializeController(BenchTestSteerCalibPlayerManager* owner);

	///\brief Destructor
	~BenchTestSteerCalibPlayerInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	BenchTestSteerCalibPlayerManager *ptr_owner;

	BenchTestSteerCalibPlayerConfig config_params;
};
}
#endif /* ifndef BenchTestSteerCalibPlayerINITIALIZECONTROLLER_H_ */
