/*
 * TeleOpsCSCIConfiguration.h
 *
 *  Created on: Nov 28, 2017
 *      Author: Emily Low
 */

#ifndef TELEOPSCSCIConfiguration_H_
#define TELEOPSCSCIConfiguration_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>
#include "Common/typedefs.h"

namespace TeleOps
{

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

struct TeleOpsCSCI_Config
{
	int32_t main_thread_period;
	float64_t max_speed; //in km/h
	float64_t max_steer; //in degrees
	float64_t max_lowerfork;
	float64_t max_raisefork;
	std::string joystick_port;
	std::map<std::string,STKCIMsg> SubStkciMsgs;
	std::map<std::string,STKCIMsg> PubStkciMsgs;
};

}

#endif /* ifndef TELEOPSCSCIConfiguration_H_ */
