/*
 * BenchTestSteerCalibPlayerProcess.cpp
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#include "BenchTestSteerCalibPlayerProcess.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace BenchTestSteerCalibPlayer
{

BenchTestSteerCalibPlayerProcess::BenchTestSteerCalibPlayerProcess()
{
	previous_timestamp = 0;
	file_EOF = false;
}

void BenchTestSteerCalibPlayerProcess::SetConfig(const BenchTestSteerCalibPlayerConfig config)
{
	config_params = config;

	//init shared pointer
	if(config_params.debug.enabled==1)
	{
		sptr_ReadCalibSteer = std::make_shared<calibsteerEmulator::ReadCalibSteer>();
		sptr_ReadCalibSteer->SetupReadCalibSteer(config.debug.log_folder);
	}
}

void BenchTestSteerCalibPlayerProcess::operator()()
{
	calibsteerEmulator::sSM_Status_t out_datalog;
	uint32_t u32_FileReturnCount;

	if(!file_EOF)
	{
		u32_FileReturnCount = sptr_ReadCalibSteer->ReadDataLogger(out_datalog);
		if(u32_FileReturnCount == 0)
		{
			file_EOF = true;
		}

		if(!file_EOF)
		{
			sptr_ReadCalibSteer->printLogData(out_datalog);
		}
		else
		{
			exit(1);
		}
	}

	//timestamp.
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();
	//std::cout << current_timestamp-previous_timestamp << " ms main loop thread time " << std::endl;
	previous_timestamp =  current_timestamp;
}

BenchTestSteerCalibPlayerProcess::~BenchTestSteerCalibPlayerProcess()
{

}

}
