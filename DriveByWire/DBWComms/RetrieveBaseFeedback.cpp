/*
 * RetrieveBaseFeedback.cpp
 *
 *  Created on: Nov 24, 2017
 *      Author: emily
 */

#include <cstring>
#include "Common/Utility/TimeUtils.h"
#include "RetrieveBaseFeedback.h"

namespace DBW
{

RetrieveBaseFeedback::RetrieveBaseFeedback():
		receive_status(false),
		previous_timestamp(0),
		basefbk_data_display_count(0),
		last_rx_timestamp(0)
{

}

RetrieveBaseFeedback::RetrieveBaseFeedback(const std::string sender_id_name):
		sender_id_name_(sender_id_name),
		receive_status(false),
		previous_timestamp(0),
		basefbk_data_display_count(0),
		last_rx_timestamp(0)
{
	sptr_middleware_ = std::make_shared<Platform::Sensors::SensorsStkciSubData>();
	std::vector<std::string> sub_interface_names;
	sub_interface_names.push_back("PalletBaseFb");
	sptr_middleware_->SetUpMiddleWareInterfaces(sub_interface_names);
}

void RetrieveBaseFeedback::operator ()()
{
	Platform::Sensors::PalletBaseFbkMsg out_datas;

	//Retrieve data from stkci
	if(sptr_middleware_->SubPalletBaseFbk(sender_id_name_,out_datas,200))
	{
		std::unique_lock<std::mutex> lock(mutex_data_basefeedback_data);
		in_data_buffer = out_datas;
		receive_status = true;
		last_rx_timestamp = out_datas.timestamp;
	}
}

bool RetrieveBaseFeedback::GetPalletBaseFbkMsg(Platform::Sensors::PalletBaseFbkMsg &out_data)
{
	bool success = false;
	std::unique_lock<std::mutex> lock(mutex_data_basefeedback_data);

	if(GetLiveliness())
	{
		out_data = in_data_buffer;
		basefbk_data_display_count++;
//		if((basefbk_data_display_count>5) && (out_data.timestamp>previous_timestamp))
//		{
//			previous_timestamp = out_data.timestamp;
//			std::cout << sender_id_name_
//					<< " | time = " << out_data.timestamp
//					<< " | seq key = " << out_data.seq_key
//					<< std::endl;
//			basefbk_data_display_count = 0;
//		}
		success = true;
	}
	return success;
}

bool RetrieveBaseFeedback::GetLiveliness()
{
	bool aliveness = false;
	uint64_t current_time = Common::Utility::Time::getusCountSinceEpoch();
	float32_t delta;
	delta = abs(current_time-last_rx_timestamp)/1000000.0;
	if(delta<2.0 && receive_status==true)//2 seconds
	{
		aliveness = true;
	}
	return aliveness;
}
//bool RetrieveBaseFeedback::GetPalletBaseFbkMsg(Platform::Sensors::PalletBaseFbkMsg &out_data)
//{
//	bool success=false;
//
//	//std::unique_lock<std::mutex> lock(mutex_data_basefeedback_data);
//	Platform::Sensors::PalletBaseFbkMsg out_datas;
//
//	//Retrieve data from stkci
//	if(sptr_middleware_->SubPalletBaseFbk(sender_id_name_,out_datas,200))
//	{
//
//		std::cout<< std::endl<< std::endl << "HERERER HRHERERE " << std::endl;
//		in_data_buffer = out_datas;
//		receive_status = true;
//	}
//
//	//
//	if(receive_status)
//	{
//		out_data = in_data_buffer;
//		basefbk_data_display_count++;
//		if((basefbk_data_display_count>5) && (out_data.timestamp>previous_timestamp))
//		{
//			previous_timestamp = out_data.timestamp;
//			std::cout << sender_id_name_
//				<< " | time = " << out_data.timestamp
//				<< " | seq key = " << out_data.seq_key
//				<< std::endl;
//			basefbk_data_display_count=0;
//		}
//		success = true;
//	}
//	else
//	{
//		out_data.batterylevel=0;
//	}
//
//	return success;
//}


RetrieveBaseFeedback::~RetrieveBaseFeedback()
{
	sptr_middleware_.reset();
}

}
