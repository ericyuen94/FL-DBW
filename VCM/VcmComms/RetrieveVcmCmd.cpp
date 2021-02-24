/*
 * RetrieveVcmCmd.cpp
 *
 *  Created on: Jul 16, 2017
 *      Author: emily
 */

#include <RetrieveVcmCmd.h>
#include "Platform/PlatformConstants.h"

namespace VCM
{

RetrieveVcmCmd::RetrieveVcmCmd()
{

}

RetrieveVcmCmd::RetrieveVcmCmd(const std::string sender_id_name):
		sender_id_name_(sender_id_name)
{
	std::string relative_path_senderid_config = "../config/swmodules/comms/";
	std::string	interface_config_file = Platform::UGV_INTERFACE_FILE;
	if(stkci_sub_vcm.setup(relative_path_senderid_config + interface_config_file,"VCMCommand")==STKCI::InterfaceRetCode_OK)
	{
		std::cout << "Setup VCMCommand interface correctly " << std::endl;
	}
	else
	{
		std::cout << "Setup VCMCommand interface error " << std::endl;
		exit(1);
	}

	sptr_configfile = std::make_shared<Common::Utility::configFile>();
	std::string sender_id_file_location;
	sender_id_file_location.append(relative_path_senderid_config);
	sender_id_file_location.append(Platform::UGV_SENDER_ID_FILE);
	if(sptr_configfile->OpenFile(sender_id_file_location) == 0)
	{
		std::cout << "Opened sender ID data configuration\n";
	}
	else
	{
		std::cerr << "Cannot open sender ID data configuration\n";
		exit(1);
	}

	//
	vcm_cmd_display_count = 0;
	previous_timestamp = 0;

	//
	msg_activeness = false;
}

void RetrieveVcmCmd::operator()()
{
	//
	int16_t sender_id;
	sender_id = sptr_configfile->GetItem<int16_t>("CSCISenderID", sender_id_name_);
	STKCI::VcmCmdMsg key;
	key.header.senderID = sender_id;
	key.header.msgID = 0;

	//
	STKCI::VcmCmdMsg out_data;

	//Retrieve data from
	if(stkci_sub_vcm.takeLastKeyedwithTimeout(out_data,key,20)== STKCI::InterfaceRetCode_OK)
	{
		//store the data
		std::unique_lock<std::mutex> lock(mutex_data_vcm_data);

		in_data_buffer = out_data;

		//
		msg_activeness = true;
		last_rx_timestamp = in_data_buffer.timestamp;
//
//			std::cout << "Get VCM commands " <<  in_data_buffer.timestamp
//					<< " Speed = " << in_data_buffer.velocity
//					<< " Steer = " << in_data_buffer.steer.mValue
//					<< std::endl;
	}
}

bool RetrieveVcmCmd::GetVcmCmd(STKCI::VcmCmdMsg &out_data)
{
	bool success=false;

	if(GetLiveliness())
	{
		std::unique_lock<std::mutex> lock(mutex_data_vcm_data);
		out_data = in_data_buffer;
		vcm_cmd_display_count++;
		if((vcm_cmd_display_count>5) && (out_data.timestamp>previous_timestamp))
		{
			previous_timestamp = out_data.timestamp;
			std::cout << sender_id_name_
					<< " | time = " << out_data.timestamp
					<< std::setprecision(3)
					<< " | speed = " << out_data.velocity
					<< " | steer = " << out_data.steer.mValue
					<< std::endl;
			vcm_cmd_display_count = 0;
		}
		success = true;
	}

	return success;
}

bool RetrieveVcmCmd::GetLiveliness()
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

RetrieveVcmCmd::~RetrieveVcmCmd()
{

}

}
