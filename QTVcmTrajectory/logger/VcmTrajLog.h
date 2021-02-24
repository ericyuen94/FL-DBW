/*
 * VcmTrajLog.h
 *
 *  Created on: Dec 20, 2017
 *      Author: emily
 */

#ifndef VcmTrajLog_H_
#define VcmTrajLog_H_

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
//
#include "VcmTrajStruct.h"

namespace vcmtrajdemo
{

class VcmTrajLog
{

public:

	VcmTrajLog();

	void SetupVcmTrajLog(const std::string log_path);

	void WriteLog(vcmtraj_data const logdata);

	int64_t GetFileSize(void) const; // To be used for sending file size to pub health status

	void CloseDataLogger(void);				 	 					  // To close log file

	~VcmTrajLog();

private:

	std::string log_folder_path_;

	FILE * mp_File;

	pthread_mutex_t m_MutexLogData;
};


}//namespace vcmtrajdemo

#endif /* VcmTrajLog_H_ */
