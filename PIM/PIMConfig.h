/*
 * PIMConfig.h
 *
 *  Created on: Dec 1, 2017
 *      Author: emily
 */

#ifndef PIMCONFIG_H_
#define PIMCONFIG_H_

#include <cstring>

namespace PIM
{

struct PIMCSCI_Config
{
	int64_t main_thread_period;
	int64_t comm_tx_thread_period; //set the can bus message transmit rate.
	std::string pcan_port;
};

}

#endif /* PIMCONFIG_H_ */
