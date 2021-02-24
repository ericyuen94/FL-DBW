/*
 * DBWInitializeController.h
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */

#ifndef DBWINITIALIZECONTROLLER_H_
#define DBWINITIALIZECONTROLLER_H_

#include "Platform/Controller/BaseCSCIInitializeController.h"
#include "DBWCSCIConfiguration.h"
//

namespace DBW
{

class DBWManager;

class DBWInitializeController: public Platform::Controller::BaseCSCIInitializeController
{

	///\class DBWInitializeController
	///\brief Read CSCI config file and set services

public:

	///\brief Constructor
	///\param[in] aOwner is the pointer for DBWManager
	DBWInitializeController(DBWManager* owner);

	///\brief Destructor
	~DBWInitializeController();

protected:

	///\brief Retrieve Configuration Values from CSCI configuration file
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus LoadSpecificParameters();

	///\brief Setup CSCI Specific Services using loaded parameters
	///\note Inherited from BaseInitializeController and defined here
	Common::InitializeStageStatus SetupSpecificServices();

private:

	DBWManager *ptr_owner;
	
};
}
#endif /* ifndef DBWINITIALIZECONTROLLER_H_ */
