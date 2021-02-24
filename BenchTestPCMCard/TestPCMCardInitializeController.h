/*
 * TestPCMCardInitializeController.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef TESTPCMCARDINITIALIZECONTROLLER_H_
#define TESTPCMCARDINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "TestPCMCardConfiguration.h"
//

namespace TestPCMCard
{

class TestPCMCardManager;

class TestPCMCardInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class TestPCMCardInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for TestPCMCardManager
	TestPCMCardInitializeController(TestPCMCardManager* owner);

	///\brief Destructor
	~TestPCMCardInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	TestPCMCardManager *ptr_owner;

	TestPCMCardConfig config_params;
};
}
#endif /* ifndef TESTPCMCARDINITIALIZECONTROLLER_H_ */
