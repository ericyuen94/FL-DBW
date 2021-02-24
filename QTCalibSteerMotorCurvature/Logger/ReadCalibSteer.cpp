/*
 * ReadCalibSteer.cpp
 *
 *  Created on: Dec 25, 2017
 *      Author: emily
 */

#include <ReadCalibSteer.h>
#include "Platform/Log/LogUtils.h"
//
namespace calibsteerEmulator
{

ReadCalibSteer::ReadCalibSteer()
{

}

void ReadCalibSteer::SetupReadCalibSteer(const std::string log_path)
{
	char ac_FileName[300]; // Directory and file name
	(void)sprintf(&ac_FileName[0],"%s/CalibSteer.log",log_path.c_str());
	mp_File = fopen(&ac_FileName[0],"rb");

	if(mp_File == NULL)
	{
		std::cout << "[initReadLog] Error opening file for reading!" << std::endl;
	}
	else
	{
		std::cout << "[initReadLog] Log file opened for reading" << std::endl;
	}
}

uint32_t ReadCalibSteer::ReadDataLogger(sSM_Status_t &datalog) // To read and return the datalog to calling function
{
	uint32_t u32_FileReturnCount = 1U; // Also serve as return value

	if(mp_File == NULL)
	{
		std::cout << "[readDataLogger] Error log file not open yet! [initReadLog] not called or unsuccessful!" << std::endl;
	}
	else
	{
		const uint32_t u32_readsize = sizeof(sSM_Status_t);

		u32_FileReturnCount = fread(&datalog,u32_readsize,1U,mp_File);

		//
		if (u32_FileReturnCount == 0U)
		{
			std::cout << "[readDataLogger] EOF!" << std::endl;
		}
	} // else

	return u32_FileReturnCount;
}


void ReadCalibSteer::printLogData(sSM_Status_t &datalog)
{
	printf("%ld %0.3f %d %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f %0.3f\n",
			datalog.timestamp,
			datalog.command_steer_angle, //deg
			datalog.feedback_seiko_encoder_pos,
			datalog.feedback_platform_speed,
			datalog.feedback_platform_steer_angle, //radians
			datalog.gps_smooth_x,
			datalog.gps_smooth_y,
			datalog.gps_vel,
			datalog.gps_yaw,
			datalog.gps_yawrate,
			datalog.computed_gps_curvature,
			datalog.computed_platform_curvature);
}

void ReadCalibSteer::CloseReadLog(void)  // To close the file
{
	if(mp_File != NULL)
	{
		(void)fclose(mp_File);
		mp_File = NULL;
	}
}

ReadCalibSteer::~ReadCalibSteer()
{

}

}//namespace
