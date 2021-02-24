/*
 * VCMConfig.h
 *
 *  Created on: Dec 1, 2017
 *      Author: emily
 */

#ifndef VCMCONFIG_H_
#define VCMCONFIG_H_

namespace VCM
{

struct VCMCSCI_Config
{
	int64_t main_thread_period;
	float64_t max_speed;
	float64_t max_steer;
	int vcm_trajdemo;
};

}

#endif /* VCMCONFIG_H_ */
