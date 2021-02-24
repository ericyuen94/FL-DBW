/*
 * RetrieveVehicleMeasure.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: emily
 */

#include "RetrieveVehicleMeasure.h"

namespace calibsteerEmulator
{

RetrieveVehicleMeasure::RetrieveVehicleMeasure(const std::string sender_id_name):
		sender_id_name_(sender_id_name)
{
	sptr_middleware_ = std::make_shared<Platform::Sensors::SensorsStkciSubData>();
	std::vector<std::string> sub_interface_names;
	sub_interface_names.push_back("VehicleMeasure");
	sptr_middleware_->SetUpMiddleWareInterfaces(sub_interface_names);
}

bool RetrieveVehicleMeasure::GetVehicleMeasureData(Platform::Sensors::GeometryMsgsVehicleMeasure &output)
{
	bool success=false;

	std::vector<Platform::Sensors::GeometryMsgsVehicleMeasure> out_datas;

	//Retrieve list of data from stkci
	if(sptr_middleware_->SubVehicleMeasure(sender_id_name_,out_datas,20))
	{
		//store the data
		std::unique_lock<std::mutex> lock(mutex_data_vehmeasure_data);
		in_data_buffer.insert(in_data_buffer.end(),out_datas.begin(),out_datas.end());

		success = true;
	}

	//
	std::unique_lock<std::mutex> lock(mutex_data_vehmeasure_data);
	if(success)
	{
		output= in_data_buffer.back();
		in_data_buffer.clear();
	}

	return success;
}

RetrieveVehicleMeasure::~RetrieveVehicleMeasure()
{

}

}

