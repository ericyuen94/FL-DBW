/*
 * RetrieveVehicleMeasure.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: emily
 */

#include "RetrieveVehicleMeasure.h"

namespace vcmtrajdemo
{

RetrieveVehicleMeasure::RetrieveVehicleMeasure(const std::string sender_id_name):
		sender_id_name_(sender_id_name)
{
	sptr_middleware_ = std::make_shared<Platform::Sensors::SensorsStkciSubData>();
	std::vector<std::string> sub_interface_names;
	sub_interface_names.push_back("VehicleMeasure");
	sptr_middleware_->SetUpMiddleWareInterfaces(sub_interface_names);
}

void RetrieveVehicleMeasure::GetVehicleMeasureData(std::list<Platform::Sensors::GeometryMsgsVehicleMeasure> &output)
{
	std::vector<Platform::Sensors::GeometryMsgsVehicleMeasure> out_datas;
	output.clear();

	//Retrieve list of data from stkci
	if(sptr_middleware_->SubVehicleMeasure(sender_id_name_,out_datas,20))
	{
		//store the data
		in_data_buffer.insert(in_data_buffer.end(),out_datas.begin(),out_datas.end());
		//
		output.insert(output.begin(),in_data_buffer.begin(),in_data_buffer.end());
		in_data_buffer.clear();
	}
}

RetrieveVehicleMeasure::~RetrieveVehicleMeasure()
{

}

}

