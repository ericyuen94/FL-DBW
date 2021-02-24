/*
 * LogCalibSteer.h
 *
 *  Created on: Dec 25, 2017
 *      Author: emily
 */

#ifndef LOGCALIBSTEER_H_
#define LOGCALIBSTEER_H_
//
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
//
#include "CalibSteerStruct.h"
//
namespace calibsteerEmulator
{

class LogCalibSteer
{

public:

	LogCalibSteer();

	void SetupCalibSteerLogger(const std::string log_path);

	void WriteLog(uint64_t timestamp, sSM_Status_t logdata);     // To write log data of specific structure

	int64_t GetFileSize(void) const; // To be used for sending file size to pub health status

	void CloseDataLogger(void);	 // To close log file

	~LogCalibSteer();

private:

	std::string log_folder_path_;

	FILE * mp_File;

};

}//namespace
#endif /* LOGCALIBSTEER_H_ */
