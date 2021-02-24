/*
 * PlatformForkliftConfiguration.h
 *
 *  Created on: Aug 1, 2018
 *      Author: Kian Wee
 */

#ifndef PLATFORMFORKLIFTCONFIGURATION_H_
#define PLATFORMFORKLIFTCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>

namespace PlatformForklift
{

struct Debug
{
	int64_t enabled;
	std::string log_folder;
};

struct PlatformForkliftConfig
{
	int64_t main_thread_period;
	int64_t drive_thread_period;
	int64_t feedback_thread_period;
	std::string pcan_port;
	std::string senderid_publisher;
	std::string senderid_dbwcmd;
	std::string senderid_basecmd;
	Debug debug;
};

}

#endif /* ifndef PLATFORMFORKLIFTCONFIGURATION_H_ */
