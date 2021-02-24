/*
 * DBWProcess.h
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */

#ifndef DBWPROCESS_H_
#define DBWPROCESS_H_

#include <cstdint>

namespace DBW
{

class DBWProcess
{
	///\class DBWProcess
	///\brief A class to process data
public:

	DBWProcess();

	void operator()();

	~DBWProcess();

private:

	int32_t count;
	int64_t previous_timestamp;

};

}
#endif /* ifndef DBWPROCESS_H_ */
