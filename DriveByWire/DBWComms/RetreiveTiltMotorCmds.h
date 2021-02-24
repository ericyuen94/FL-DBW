/*
 * RetreiveTiltMotorCmds.h
 *
 *  Created on: Sept 20, 2019
 *      Author: chunkiat
 */

#ifndef RETREIVETILTMOTORCMDS_H_
#define RETREIVETILTMOTORCMDS_H_

#include "Platform/Sensors/SensorsStkciSubData.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include "Common/Utility/TimeUtils.h"
#include <mutex>
#include <list>
#include <cstdint>
#include <memory>
#include <map>

namespace DBW
{

class RetreiveTiltMotorCmds
{

public:

	RetreiveTiltMotorCmds();

	RetreiveTiltMotorCmds(const std::string sender_id_name);

	void operator()();

	bool GetTiltMotorMsgs(Platform::Sensors::TiltMotorMsg &out_data);

	bool GetLiveliness();

	~RetreiveTiltMotorCmds();

private:

	std::string  sender_id_name_;
	std::shared_ptr<Platform::Sensors::SensorsStkciSubData> sptr_middleware_;

	std::mutex mutex_data_base_data;
	Platform::Sensors::TiltMotorMsg in_data_buffer;
	uint64_t previous_timestamp;
	int32_t csci_data_display_count;

	//check message aliveness
	bool msg_activeness;
	uint64_t last_rx_timestamp;
};

}
#endif /* RETREIVETILTMOTORCMDS_H_ */
