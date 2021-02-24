/*
 * VcmTrajLog.cpp
 *
 *  Created on: Dec 20, 2017
 *      Author: emily
 */

#include "VcmTrajLog.h"
#include "Platform/Log/LogUtils.h"

namespace vcmtrajdemo
{

VcmTrajLog::VcmTrajLog()
{

}

void VcmTrajLog::SetupVcmTrajLog(const std::string log_path)
{
	std::string current_tod;
	current_tod = Platform::Log::LogUtils::GetStringYearDayTime();

	//Create log directory
	log_folder_path_.append(log_path);
//	log_folder_path_.append("/");
//	log_folder_path_.append(current_tod);
	Platform::Log::LogUtils::MakeDir(log_folder_path_);

	// Create a .log file name inside the above directory
	char ac_FileName[300] = {'\0'};
	(void)sprintf(&ac_FileName[0],"%s/vcmtraj.log",log_folder_path_.c_str());

	std::cout << "SetupVcmTrajLog filename" << ac_FileName << std::endl;

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

void VcmTrajLog::WriteLog(vcmtraj_data const logdata)
{
	if (mp_File != NULL)
	{
		(void)pthread_mutex_lock(&m_MutexLogData);
		(void)fwrite(&logdata, sizeof(logdata), 1U, mp_File);
		(void)pthread_mutex_unlock(&m_MutexLogData);
	}
	else
	{
		std::cout << "[writeLog] Error log file is not opened for writing" << std::endl;
	}
}

int64_t VcmTrajLog::GetFileSize() const
{
	return ftell(mp_File);
}

void VcmTrajLog::CloseDataLogger()
{
	if (mp_File != NULL)
	{
		(void)fclose(mp_File);
		mp_File = NULL;
	}
}

VcmTrajLog::~VcmTrajLog()
{

}

}//namespace vcmtrajdemo
