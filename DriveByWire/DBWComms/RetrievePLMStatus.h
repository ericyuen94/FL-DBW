/*
 * RetrievePLMStatus.h
 *
 *  Created on: Nov 26, 2017
 *      Author: emily
 */

#ifndef RetrievePLMStatus_H_
#define RetrievePLMStatus_H_

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

class RetrievePLMStatus
{

public:

	RetrievePLMStatus();

	RetrievePLMStatus(const std::string sender_id_name);

	void operator()();

	bool GetPLMDataMsg(Platform::Sensors::SystemMsgsUGVPlatformStatus &out_data);

	bool GetLiveliness();

	~RetrievePLMStatus();

private:

	std::string  sender_id_name_;
	std::shared_ptr<Platform::Sensors::SensorsStkciSubData> sptr_middleware_;

	std::mutex mutex_data_csci_data;
	Platform::Sensors::SystemMsgsUGVPlatformStatus in_data_buffer;
	uint64_t previous_timestamp;
	int32_t csci_data_display_count;

	//check message aliveness
	bool msg_activeness;
	uint64_t last_rx_timestamp;
};
}
#endif /* RetrievePLMStatus_H_ */
