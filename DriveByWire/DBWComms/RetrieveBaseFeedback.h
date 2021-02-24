/*
 * RetrieveBaseFeedback.h
 *
 *  Created on: Nov 24, 2017
 *      Author: emily
 */

#ifndef RETRIEVEBASEFEEDBACK_H_
#define RETRIEVEBASEFEEDBACK_H_

#include "Platform/Sensors/SensorsStkciSubData.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include <mutex>
#include <list>
#include <cstdint>
#include <memory>
#include <map>

namespace DBW
{

class RetrieveBaseFeedback
{

public:

	RetrieveBaseFeedback();

	RetrieveBaseFeedback(const std::string sender_id_name);

	void operator()();

	bool GetPalletBaseFbkMsg(Platform::Sensors::PalletBaseFbkMsg &out_data);

	bool GetLiveliness();

	~RetrieveBaseFeedback();

private:

	bool receive_status;

	std::string  sender_id_name_;
	std::shared_ptr<Platform::Sensors::SensorsStkciSubData> sptr_middleware_;

	std::mutex mutex_data_basefeedback_data;
	Platform::Sensors::PalletBaseFbkMsg in_data_buffer;
	uint64_t previous_timestamp;
	int32_t basefbk_data_display_count;

	uint64_t last_rx_timestamp;
};

}
#endif /* RETRIEVEBASEFEEDBACK_H_ */
