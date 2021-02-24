/*
 * RetrieveCovPoseMsg.cpp
 *
 *  Created on: Jul 5, 2017
 *      Author: emily
 */

#include <RetrieveCovPoseMsg.h>

namespace calibsteerEmulator
{

RetrieveCovPoseMsg::RetrieveCovPoseMsg()
{

}

RetrieveCovPoseMsg::RetrieveCovPoseMsg(const std::string sender_id_name):
		sender_id_name_(sender_id_name)
{
	sptr_middleware_ = std::make_shared<Platform::Sensors::SensorsStkciSubData>();
	std::vector<std::string> sub_interface_names;
	sub_interface_names.push_back("CovariancePos");
	sptr_middleware_->SetUpMiddleWareInterfaces(sub_interface_names);
	receive_status = false;
	pos_cov_display_count = 0;
	previous_timestamp = 0;
}

bool RetrieveCovPoseMsg::GetGeometryMsgsPoseWithCovarianceMsg(Platform::Sensors::GeometryMsgsPoseWithCovariance &out_data)
{
	bool success=false;

	//std::unique_lock<std::mutex> lock(mutex_data_posecov_data);

	std::vector<Platform::Sensors::GeometryMsgsPoseWithCovariance> out_datas;
	//Retrieve data from stkci
	if(sptr_middleware_->SubCovPos(sender_id_name_,out_datas,20))
	{
		//store the data
		if(out_datas.size()>0)
		{
			in_data_buffer = out_datas.back();
			receive_status = true;
		}
	}

	if(receive_status)
	{
		out_data = in_data_buffer;
		pos_cov_display_count++;
		if((pos_cov_display_count>5) && (out_data.timestamp>previous_timestamp))
		{
			previous_timestamp = out_data.timestamp;
			std::cout << sender_id_name_
				<< " | time = " << out_data.timestamp
				<< " | x = " << out_data.pose.position.x
				<< " | y = " << out_data.pose.position.y
				<< " | yaw = " << out_data.pose.altitude.yaw
				<< std::endl;
			pos_cov_display_count=0;
		}
		success = true;
	}

	return success;
}

RetrieveCovPoseMsg::~RetrieveCovPoseMsg()
{
	sptr_middleware_.reset();
}

}
