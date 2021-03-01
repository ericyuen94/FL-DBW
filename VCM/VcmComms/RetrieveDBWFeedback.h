/*
 * RetrieveDBWFeedback.h
 *
 *  Created on: Nov 24, 2017
 *      Author: emily
 */

#ifndef RETRIEVEDBWFEEDBACK_H_
#define RETRIEVEDBWFEEDBACK_H_

#include "Platform/Sensors/SensorsStkciSubData.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include <mutex>
#include <list>
#include <cstdint>
#include <memory>
#include <map>

namespace VCM
{

class RetrieveDBWFeedback
{

public:

	RetrieveDBWFeedback();

	RetrieveDBWFeedback(const std::string sender_id_name);

	bool GetPalletDBWFbkMsg(Platform::Sensors::PalletDbwFbkMsg &out_data);

	~RetrieveDBWFeedback();

private:

	bool receive_status;

	std::string  sender_id_name_;
	std::shared_ptr<Platform::Sensors::SensorsStkciSubData> sptr_middleware_;

	std::mutex mutex_data_dbwfeedback_data;
	Platform::Sensors::PalletDbwFbkMsg in_data_buffer;
	uint64_t previous_timestamp;
	int32_t dbw_data_display_count;
};

}
#endif /* RETRIEVEDBWFEEDBACK_H_ */
