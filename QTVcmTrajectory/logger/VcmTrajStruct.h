/*
 * VcmTrajStruct.h
 *
 *  Created on: Dec 28, 2017
 *      Author: emily
 */

#ifndef VCMTRAJSTRUCT_H_
#define VCMTRAJSTRUCT_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <map>
#include "Common/typedefs.h"

namespace vcmtrajdemo
{

struct vcmtraj_data
{
	uint64_t timestamp;
	float64_t speed;
	float64_t steering;
	float64_t yaw_rate;

};

}

#endif /* VCMTRAJSTRUCT_H_ */

