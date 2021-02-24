/*
 * BenchTestPCanPlayerProcess.cpp
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#include "BenchTestPCanPlayerProcess.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace BenchTestPCanPlayer
{

BenchTestPCanPlayerProcess::BenchTestPCanPlayerProcess()
{
	previous_timestamp = 0;
}

void BenchTestPCanPlayerProcess::SetConfig(const BenchTestPCanPlayerConfig config)
{
	config_params = config;

	//init shared pointer
	if(config_params.debug.enabled==1)
	{
		sptr_PCanLogRead = std::make_shared<Platform::PCanUsb::PCanLogRead>();
		sptr_PCanLogRead->SetupPCanLogRead(config.debug.log_folder);
	}
	sptr_PCMPalletController = std::make_shared<DBWPalletCommon::PCMPalletController>();
}

void BenchTestPCanPlayerProcess::operator()()
{
	//data
	uint64_t out_timestamp;
	TPCANRdMsg out_datalog;
	uint32_t result;

	//pcan data
	std::vector<char_t> read_candata;

	//read data out.
	if(config_params.debug.enabled==1)
	{
		result = sptr_PCanLogRead->ReadDataLogger(out_timestamp,out_datalog);

		if(result == 0)
		{
			exit(1);
		}

		//
		float32_t platform_current_speed;
		float32_t platform_current_batt_level;
		uint32_t pos_encoder;
		sptr_PCMPalletController->DecodePalletSpeedFbkMsg(&out_datalog,out_timestamp,platform_current_speed);
		sptr_PCMPalletController->DecodePalletBatteryFbkMsg(&out_datalog,out_timestamp,platform_current_batt_level);
		sptr_PCMPalletController->DecodeSeikoEncMsg(&out_datalog,out_timestamp,pos_encoder);

		//DecodePalletBatteryFbkMsg(out_datalog.Msg,out_timestamp,platform_current_batt_level);

		//
		float32_t freq;
		freq = 1/((out_timestamp-last_timestamp_rx_all)/1000000.0);
		last_timestamp_rx_all = out_timestamp;
		std::cout << "Hz = " << freq << std::endl;

	}

	//timestamp.
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();
	//std::cout << current_timestamp-previous_timestamp << " ms main loop thread time " << std::endl;
	previous_timestamp =  current_timestamp;
}


BenchTestPCanPlayerProcess::~BenchTestPCanPlayerProcess()
{

}

}
