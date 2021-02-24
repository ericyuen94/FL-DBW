/*
 * DBWProcess.cpp
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */

#include "DBWProcess.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace DBW
{

DBWProcess::DBWProcess()
{
	count = 0;
	previous_timestamp = 0;
}

void DBWProcess::operator()()
{
	count++;
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();
	//std::cout << count << " " << current_timestamp-previous_timestamp << " ms main loop thread time " << std::endl;
	previous_timestamp =  current_timestamp;
}

DBWProcess::~DBWProcess()
{

}

}
