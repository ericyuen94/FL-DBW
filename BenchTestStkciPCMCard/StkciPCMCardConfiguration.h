/*
 * StkciPCMCardConfiguration.h
 *
 *  Created on: Oct 8, 2017
 *      Author: Emily Low
 */

#ifndef STKCIPCMCARDCONFIGURATION_H_
#define STKCIPCMCARDCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>

namespace StkciPCMCard
{

struct Debug
{
	int64_t enabled;
	std::string log_folder;
};

struct StkciPCMCardConfig
{
	int64_t main_thread_period;
	int64_t feedback_thread_period;
	std::string pcan_port;
	std::string senderid_publisher;
	std::string senderid_dbwcmd;
	std::string senderid_basecmd;
	Debug debug;
};

}

#endif /* ifndef STKCIPCMCARDCONFIGURATION_H_ */
