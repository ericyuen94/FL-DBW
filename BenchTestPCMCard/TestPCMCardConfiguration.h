/*
 * TestPCMCardConfiguration.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef TESTPCMCARDCONFIGURATION_H_
#define TESTPCMCARDCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>

namespace TestPCMCard
{

struct Debug
{
	int64_t enabled;
	std::string log_folder;
};

struct TestPCMCardConfig
{
	int64_t main_thread_period;
	std::string pcan_port;
	Debug debug;
};

}

#endif /* ifndef TESTPCMCARDCONFIGURATION_H_ */
