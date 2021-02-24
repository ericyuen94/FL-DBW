/*
 * RetrieveVehMeasure.h
 *
 *  Created on: Jul 14, 2017
 *      Author: emily
 */

#ifndef RETRIEVEVEHMEASURE_H_
#define RETRIEVEVEHMEASURE_H_

#include "Platform/Sensors/SensorsStkciSubData.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include <mutex>
#include <list>
#include <cstdint>
#include <memory>
#include <map>

namespace DBW
{

class RetrieveVehMeasure
{

public:

	RetrieveVehMeasure();

	RetrieveVehMeasure(const std::string sender_id_name);

	void operator()();

	bool GetGeometryMsgsVehMeasureMsg(Platform::Sensors::GeometryMsgsVehicleMeasure &out_data);

	~RetrieveVehMeasure();

private:

	bool receive_status;

	std::string  sender_id_name_;
	std::shared_ptr<Platform::Sensors::SensorsStkciSubData> sptr_middleware_;

	//
	std::mutex mutex_data_vehmea_data;
	Platform::Sensors::GeometryMsgsVehicleMeasure in_data_buffer;
	int32_t veh_meas_display_count;
	uint64_t previous_timestamp;
};
}

#endif /* RETRIEVEVEHMEASURE_H_ */
