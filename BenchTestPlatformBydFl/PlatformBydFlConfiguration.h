/*
 * PlatformBydFlConfiguration.h
 *
 *  Created on: July 1, 2019
 *      Author: Tester
 */

#ifndef PlatformBydFlConfiguration_H_
#define PlatformBydFlConfiguration_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>

namespace PlatformBydFl
{

struct Debug
{
	int64_t enabled;
	std::string log_folder;
};

struct PlatformBydFlConfig
{
	int64_t main_thread_period;
	int64_t drive_thread_period;
	int64_t feedback_thread_period;
	std::string pcan_port;
	std::string senderid_publisher;
	std::string senderid_dbwcmd;
	std::string senderid_basecmd;
	float64_t max_forward_speed;
	float64_t max_steering;
	Debug debug;
};

}

#endif /* ifndef PlatformBydFlConfiguration_H_ */
