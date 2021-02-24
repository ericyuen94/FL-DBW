/*
 * BenchTestEncoderProcess.cpp
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#include "BenchTestEncoderProcess.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace BenchTestEncoder
{

BenchTestEncoderProcess::BenchTestEncoderProcess()
{
	previous_timestamp = 0;
}

void BenchTestEncoderProcess::SetConfig(const BenchTestEncoderConfig config)
{
	config_params = config;
	std::cout << "Can Port = " << config_params.pcan_port << std::endl;

	//init shared pointer
	sptr_PcanSeikoPcanEncoder = std::make_shared<DBWPalletCommon::PcanSeikoPcanEncoder>();

	//open pcan port
	sptr_PcanSeikoPcanEncoder->OpenCanPort(config_params.pcan_port);
}

void BenchTestEncoderProcess::operator()()
{
	TPCANRdMsg CAN_RxMsg;
    bool b_ReadTimeout;

    //std::cout << "BenchTestEncoderProcess " << " " << Common::Utility::Time::getmsCountSinceEpoch() << std::endl;

    //get encoder count
	uint32_t encoder_count;
	if(sptr_PcanSeikoPcanEncoder->GetSeikoSteerFeedback(encoder_count))
	{
		std::cout << "!!! Get encoder count = " << encoder_count << std::endl;
	}
	else
	{
		//std::cout << "Polling for seiko encoder readings" << std::endl;
	}

    //
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();
	//std::cout << current_timestamp-previous_timestamp << " ms main loop thread time " << std::endl;
	previous_timestamp =  current_timestamp;
}

BenchTestEncoderProcess::~BenchTestEncoderProcess()
{

}

}
