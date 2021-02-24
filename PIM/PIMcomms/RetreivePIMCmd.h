/*
 * RetreivePIMCmd.h
 *
 *  Created on: Nov 28, 2017
 *      Author: emily
 */

#ifndef RetreivePIMCmd_H_
#define RetreivePIMCmd_H_

#include "Platform/Sensors/SensorsStkciSubData.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include "Common/Utility/TimeUtils.h"
#include <mutex>
#include <list>
#include <cstdint>
#include <memory>
#include <map>

namespace PIM
{

class RetreivePIMCmd
{

public:

	RetreivePIMCmd();

	RetreivePIMCmd(const std::string sender_id_name);

	void operator()();

	bool GetPIMDataMsg(Platform::Sensors::PalletAuxCmdMsg &out_data);

	bool GetLiveliness();

	~RetreivePIMCmd();

private:

	std::string  sender_id_name_;
	std::shared_ptr<Platform::Sensors::SensorsStkciSubData> sptr_middleware_;

	std::mutex mutex_data_PIM_data;
	Platform::Sensors::PalletAuxCmdMsg in_data_buffer;
	uint64_t previous_timestamp;
	int32_t csci_data_display_count;

	//check message aliveness
	bool msg_activeness;
	uint64_t last_rx_timestamp;
};

}
#endif /* RetreivePIMCmd_H_ */
