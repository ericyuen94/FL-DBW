/*
 * BenchTestPCanPlayerConfiguration.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef BENCHTESTPCANPLAYERCONFIGURATION_H_
#define BENCHTESTPCANPLAYERCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>

namespace BenchTestPCanPlayer
{

struct Debug
{
	int64_t enabled;
	std::string log_folder;
};

struct BenchTestPCanPlayerConfig
{
	int64_t main_thread_period;
	std::string pcan_port;
	Debug debug;
};

}

#endif /* ifndef BENCHTESTPCANPLAYERCONFIGURATION_H_ */
