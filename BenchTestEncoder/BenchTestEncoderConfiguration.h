/*
 * BenchTestEncoderConfiguration.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef BENCHTESTENCODERCONFIGURATION_H_
#define BENCHTESTENCODERCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>

namespace BenchTestEncoder
{

struct BenchTestEncoderConfig
{
	int64_t main_thread_period;
	std::string pcan_port;
};

}

#endif /* ifndef BENCHTESTENCODERCONFIGURATION_H_ */
