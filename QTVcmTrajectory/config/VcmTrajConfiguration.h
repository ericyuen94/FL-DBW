/*
 * VcmTrajConfiguration.h
 *
 *  Created on: Jult 3, 2017
 *      Author: Emily Low
 */

#ifndef VcmTrajCONFIGURATION_H_
#define VcmTrajCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>
#include "Common/typedefs.h"

namespace vcmtrajdemo
{

struct StkciMsg
{
	std::string platform_vehiclemeasure_senderid;
};

struct DataLog
{
	int32_t enabled;
	std::string path;
};

struct VcmTrajConfig
{
	float64_t max_linear_speed;
	float64_t max_steer;
	DataLog VcmCalibLog;
	StkciMsg StkciSenderId;
};

}

#endif /* ifndef VcmTrajCONFIGURATION_H_ */


