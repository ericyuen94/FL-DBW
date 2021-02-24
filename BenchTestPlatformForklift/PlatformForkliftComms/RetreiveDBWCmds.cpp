/*
 * RetreiveDBWCmds.cpp
 *
 *  Created on: Nov 28, 2017
 *      Author: emily
 */

#include <RetreiveDBWCmds.h>

namespace PlatformForklift
{

RetreiveDBWCmds::RetreiveDBWCmds()
{

}

RetreiveDBWCmds::RetreiveDBWCmds(const std::string sender_id_name)
{
	sptr_middleware_ = std::make_shared<Platform::Sensors::SensorsStkciSubData>();
	std::vector<std::string> sub_interface_names;
	sub_interface_names.push_back("PalletDBWCmd");
	sptr_middleware_->SetUpMiddleWareInterfaces(sub_interface_names);
	csci_data_display_count = 0;
	sender_id_name_ = sender_id_name;
	msg_activeness = false;
	previous_timestamp = 0;
}

void RetreiveDBWCmds::operator()()
{
	Platform::Sensors::PalletDbwCmdMsg out_datas;

	//Retrieve data from stkci
	if(sptr_middleware_->SubPalletDbwCmd(sender_id_name_,out_datas))
	{
		std::unique_lock<std::mutex> lock(mutex_data_dbw_data);
		in_data_buffer = out_datas;
		//
		msg_activeness = true;
		last_rx_timestamp = out_datas.timestamp;
	}
}

bool RetreiveDBWCmds::GetDBWCmdMsg(Platform::Sensors::PalletDbwCmdMsg &out_data)
{
	bool success=false;

	std::unique_lock<std::mutex> lock(mutex_data_dbw_data);

	if(GetLiveliness())
	{
		out_data = in_data_buffer;
		csci_data_display_count++;
		if((csci_data_display_count>5) && (out_data.timestamp>previous_timestamp))
		{
			previous_timestamp = out_data.timestamp;
			std::cout << sender_id_name_
				<< " | time = " << out_data.timestamp
				<< " | seq key = " << out_data.seq_key
				<< std::endl;
			csci_data_display_count=0;
		}

		//
		success = true;
	}

	return success;
}

bool RetreiveDBWCmds::GetLiveliness()
{
	bool aliveness = false;
	uint64_t current_time = Common::Utility::Time::getusecCountSinceEpoch();

	float32_t delta;
	delta = abs(current_time-last_rx_timestamp)/1000000.0;

	if(delta<2.0 && msg_activeness==true)//2 seconds
	{
		aliveness = true;
	}
	return aliveness;
}

RetreiveDBWCmds::~RetreiveDBWCmds()
{

}

}

