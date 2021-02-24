/*
 * RetreiveDBWCmds.h
 *
 *  Created on: Nov 28, 2017
 *      Author: emily
 */

#ifndef RETREIVEDBWCMDS_H_
#define RETREIVEDBWCMDS_H_

#include "Platform/Sensors/SensorsStkciSubData.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include "Common/Utility/TimeUtils.h"
#include <mutex>
#include <list>
#include <cstdint>
#include <memory>
#include <map>

namespace StkciVecowPCMCard
{

class RetreiveDBWCmds
{

public:

	RetreiveDBWCmds();

	RetreiveDBWCmds(const std::string sender_id_name);

	void operator()();

	bool GetDBWCmdMsg(Platform::Sensors::PalletDbwCmdMsg &out_data);

	bool GetLiveliness();

	~RetreiveDBWCmds();

private:

	std::string  sender_id_name_;
	std::shared_ptr<Platform::Sensors::SensorsStkciSubData> sptr_middleware_;

	//
	std::mutex mutex_data_dbw_data;
	Platform::Sensors::PalletDbwCmdMsg in_data_buffer;
	uint64_t previous_timestamp;
	int32_t csci_data_display_count;

	//check message aliveness
	bool msg_activeness;
	uint64_t last_rx_timestamp;
};

}
#endif /* RETREIVEDBWCMDS_H_ */
