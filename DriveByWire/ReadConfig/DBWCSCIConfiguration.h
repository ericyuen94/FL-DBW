/*
 * DBWCSCIConfiguration.h
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */

#ifndef DBWCSCIConfiguration_H_
#define DBWCSCIConfiguration_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>
#include "Common/typedefs.h"

namespace DBW
{

struct Logging
{
	int64_t enabled;
};

struct Debug
{
	int64_t enabled;
	std::string log_folder;
};

struct PCMParams
{
	std::string port;
	int32_t baudrate;
};

struct ThreadParams
{
	int32_t mThreadPeriodmsec;
	int32_t mThreadTimeoutmsec;
};

struct STKCIMsg
{
	std::string senderid;
	ThreadParams threadparams;
};

struct DBWCSCI_Config
{
	int32_t main_thread_period;
	float32_t warning_temperature;
	float32_t error_temperature;
	Logging logging;
	Debug debug;
	PCMParams PCMSettings;
	std::map<std::string,ThreadParams> ControlCmdThreads;
	std::map<std::string,STKCIMsg> SubStkciMsgs;
	std::map<std::string,STKCIMsg> PubStkciMsgs;
};

}

#endif /* ifndef DBWCSCIConfiguration_H_ */
