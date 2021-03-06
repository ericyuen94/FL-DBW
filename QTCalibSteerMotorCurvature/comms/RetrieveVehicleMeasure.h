/*
 * RetrieveVehicleMeasure.h
 *
 *  Created on: Jun 14, 2017
 *      Author: emily
 */

#ifndef RETRIEVEVEHICLEMEASURE_H_
#define RETRIEVEVEHICLEMEASURE_H_
#include "Platform/Sensors/SensorsStkciSubData.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include <mutex>
#include <list>
#include <cstdint>
#include <memory>
#include <map>

namespace calibsteerEmulator
{

class RetrieveVehicleMeasure
{

public:

	RetrieveVehicleMeasure(const std::string sender_id_name);

	bool GetVehicleMeasureData(Platform::Sensors::GeometryMsgsVehicleMeasure &output);

	~RetrieveVehicleMeasure();

private:

	std::string  sender_id_name_;
	std::shared_ptr<Platform::Sensors::SensorsStkciSubData> sptr_middleware_;
	//
	std::mutex mutex_data_vehmeasure_data;
	std::list<Platform::Sensors::GeometryMsgsVehicleMeasure> in_data_buffer;
};


}

#endif /* RETRIEVEVEHICLEMEASURE_H_ */
