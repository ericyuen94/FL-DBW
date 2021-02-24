/*
 * CalibSteerConfiguration.h
 *
 *  Created on: Jult 3, 2017
 *      Author: Emily Low
 */

#ifndef CalibSteerCONFIGURATION_H_
#define CalibSteerCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>
#include "Common/typedefs.h"

namespace calibsteerEmulator
{

struct StkciMsg
{
	std::string posecov_senderid;
	std::string gps_vehiclemeasure_senderid;
	std::string platform_vehiclemeasure_senderid;
};

struct DataLog
{
	int32_t enabled;
	std::string path;
};

struct CalibSteerConfig
{
	std::string pcm_pcan_port;
	float64_t max_linear_speed;
	DataLog SteerCalibLog;
	DataLog PlaybackSteerCalibLog;
	DataLog Debug;
	StkciMsg StkciSenderId;
};

}

#endif /* ifndef CalibSteerCONFIGURATION_H_ */


