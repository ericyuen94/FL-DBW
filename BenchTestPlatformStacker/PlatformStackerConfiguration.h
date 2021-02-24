/*
 * PlatformStackerConfiguration.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef PlatformStackerCONFIGURATION_H_
#define PlatformStackerCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>

namespace PlatformStacker
{

struct Debug
{
	int64_t enabled;
	std::string log_folder;
};

struct PlatformStackerConfig
{
	int64_t main_thread_period;
	std::string pcan_port;
	Debug debug;
	std::string senderid_dbwcmd;
	std::string senderid_basecmd;
};

}

#endif /* ifndef PlatformStackerCONFIGURATION_H_ */
