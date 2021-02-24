/*
 * RetrieveCovPoseMsg.h
 *
 *  Created on: Jul 5, 2017
 *      Author: emily
 */

#ifndef RETRIEVECOVPOSEMSG_H_
#define RETRIEVECOVPOSEMSG_H_

#include "Platform/Sensors/SensorsStkciSubData.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include <mutex>
#include <list>
#include <cstdint>
#include <memory>
#include <map>

namespace calibsteerEmulator
{

class RetrieveCovPoseMsg
{
public:

	RetrieveCovPoseMsg();

	RetrieveCovPoseMsg(const std::string sender_id_name);

	bool GetGeometryMsgsPoseWithCovarianceMsg(Platform::Sensors::GeometryMsgsPoseWithCovariance &out_data);

	~RetrieveCovPoseMsg();

private:

	bool receive_status;

	std::string  sender_id_name_;
	std::shared_ptr<Platform::Sensors::SensorsStkciSubData> sptr_middleware_;

	std::mutex mutex_data_posecov_data;
	Platform::Sensors::GeometryMsgsPoseWithCovariance in_data_buffer;
	uint64_t previous_timestamp;
	int32_t pos_cov_display_count;
};

}
#endif /* RETRIEVECOVPOSEMSG_H_ */
