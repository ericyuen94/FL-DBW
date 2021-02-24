/*
 * LogCalibSteer.cpp
 *
 *  Created on: Dec 25, 2017
 *      Author: emily
 */

#include <LogCalibSteer.h>
#include "Platform/Log/LogUtils.h"
//
namespace calibsteerEmulator
{

LogCalibSteer::LogCalibSteer()
{

}

void LogCalibSteer::SetupCalibSteerLogger(const std::string log_path)
{
	std::string current_tod;
	current_tod = Platform::Log::LogUtils::GetStringYearDayTime();

	//Create log directory
	log_folder_path_.append(log_path);
	log_folder_path_.append("/");
	log_folder_path_.append(current_tod);
	Platform::Log::LogUtils::MakeDir(log_folder_path_);

	// Create a .log file name inside the above directory
	char ac_FileName[300] = {'\0'};
	(void)sprintf(&ac_FileName[0],"%s/CalibSteer.log",log_folder_path_.c_str());

	std::cout << "SetupPCanLogger filename" << ac_FileName << std::endl;

	mp_File = fopen(&ac_FileName[0],"wb");

	if (mp_File == NULL)
	{
		std::cout << "[initDataLog] Error opening file for writing!" << std::endl;
	}
	else
	{
		std::cout << "[initDataLog] Log file opened successfully for writing" << std::endl;
	}
}

void LogCalibSteer::WriteLog(uint64_t timestamp, sSM_Status_t logdata)// To write log data of specific structure
{
	if (mp_File != NULL)
	{
			(void)fwrite(&logdata, sizeof(logdata), 1U, mp_File);
	}
	else
	{
		std::cout << "[writeLog] Error log file is not opened for writing" << std::endl;
	}
}

int64_t LogCalibSteer::GetFileSize() const
{
	return ftell(mp_File);
}

void LogCalibSteer::CloseDataLogger()
{
	if (mp_File != NULL)
	{
		(void)fclose(mp_File);
		mp_File = NULL;
	}
}


LogCalibSteer::~LogCalibSteer()
{

}

}//namespace
