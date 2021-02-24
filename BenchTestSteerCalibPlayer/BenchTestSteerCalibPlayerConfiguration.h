/*
 * BenchTestSteerCalibPlayerConfiguration.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef BenchTestSteerCalibPlayerCONFIGURATION_H_
#define BenchTestSteerCalibPlayerCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>

namespace BenchTestSteerCalibPlayer
{

struct Debug
{
	int64_t enabled;
	std::string log_folder;
};

struct BenchTestSteerCalibPlayerConfig
{
	int64_t main_thread_period;
	Debug debug;
};

}

#endif /* ifndef BenchTestSteerCalibPlayerCONFIGURATION_H_ */
