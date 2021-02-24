/*
 * RetrieveVehMeasure.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: emily
 */

#include <RetrieveVehMeasure.h>

namespace DBW
{

RetrieveVehMeasure::RetrieveVehMeasure()
{

}

RetrieveVehMeasure::RetrieveVehMeasure(const std::string sender_id_name):
		sender_id_name_(sender_id_name)
{
	sptr_middleware_ = std::make_shared<Platform::Sensors::SensorsStkciSubData>();
	std::vector<std::string> sub_interface_names;
	sub_interface_names.push_back("VehicleMeasure");
	sptr_middleware_->SetUpMiddleWareInterfaces(sub_interface_names);
	receive_status = false;
	previous_timestamp = 0;
	veh_meas_display_count = 0;
}

void RetrieveVehMeasure::operator()()
{
	std::vector<Platform::Sensors::GeometryMsgsVehicleMeasure> out_datas;

	//Retrieve data from stkci
	if(sptr_middleware_->SubVehicleMeasure(sender_id_name_,out_datas,20))
	{
		//store the data
		std::unique_lock<std::mutex> lock(mutex_data_vehmea_data);
		if(out_datas.size()>0)
		{
			in_data_buffer = out_datas.back();
			receive_status = true;
		}
	}
}

bool RetrieveVehMeasure::GetGeometryMsgsVehMeasureMsg(Platform::Sensors::GeometryMsgsVehicleMeasure &out_data)
{
	bool success=false;

	if(receive_status)
	{
		std::unique_lock<std::mutex> lock(mutex_data_vehmea_data);
		out_data = in_data_buffer;
		veh_meas_display_count++;
		if((veh_meas_display_count>5) && (out_data.timestamp>previous_timestamp))
		{
			previous_timestamp = out_data.timestamp;
			std::cout << sender_id_name_
					<< " | time = " << out_data.timestamp
					<< std::setprecision(3)
					<< " | speed = " << out_data.speed
					<< " | steer = " << out_data.steering
					<< " | yawrate = " << out_data.yaw_rate
					<< std::endl;
			veh_meas_display_count = 0;
		}
		success = true;
	}

	return success;
}

RetrieveVehMeasure::~RetrieveVehMeasure()
{
	sptr_middleware_.reset();
}

}
