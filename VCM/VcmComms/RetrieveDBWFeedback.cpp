/*
 * RetrieveDBWFeedback.cpp
 *
 *  Created on: Nov 24, 2017
 *      Author: emily
 */

#include "RetrieveDBWFeedback.h"
#include <cstring>

namespace VCM
{

RetrieveDBWFeedback::RetrieveDBWFeedback()
{

}

RetrieveDBWFeedback::RetrieveDBWFeedback(const std::string sender_id_name)
{
	sptr_middleware_ = std::make_shared<Platform::Sensors::SensorsStkciSubData>();
	std::vector<std::string> sub_interface_names;
	sub_interface_names.push_back("PalletDBWFb");
	sptr_middleware_->SetUpMiddleWareInterfaces(sub_interface_names);
	dbw_data_display_count = 0;
	sender_id_name_ = sender_id_name;
}

bool RetrieveDBWFeedback::GetPalletDBWFbkMsg(Platform::Sensors::PalletDbwFbkMsg &out_data)
{
	bool success=false;

	//std::unique_lock<std::mutex> lock(mutex_data_dbwfeedback_data);
	Platform::Sensors::PalletDbwFbkMsg out_datas;

	receive_status = false;

	//Retrieve data from stkci
	if(sptr_middleware_->SubPalletDbwFbk(sender_id_name_,out_datas))
	{
		in_data_buffer = out_datas;
		receive_status = true;
	}

	//
	if(receive_status)
	{
		out_data = in_data_buffer;
		dbw_data_display_count++;
		if((dbw_data_display_count>5) && (out_data.timestamp>previous_timestamp))
		{
			previous_timestamp = out_data.timestamp;
//			std::cout << sender_id_name_
//				<< " | time = " << out_data.timestamp
//				<< " | seq key = " << out_data.seq_key
//				<< std::endl;
			dbw_data_display_count=0;
		}
		success = true;
	}
	else
	{
		out_data.speed=0;
		out_data.steer=0;
		out_data.left_drive_encoder_count=0;
		out_data.right_drive_encoder_count=0;
		out_data.steer_encoder_count=0;
		out_data.drive_motor_status=0;
		out_data.steer_motor_status=0;
		out_data.drive_motor_temp=0;
		out_data.steer_motor_temp=0;
	}

	return success;
}

RetrieveDBWFeedback::~RetrieveDBWFeedback()
{

}
}
