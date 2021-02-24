//******************************************************************************************************
// KDM_SMDataLogger.h code by modified by Loh Yong Hua, USC, KDM.
//******************************************************************************************************

#ifndef KDM_SMDATALOGGER_H
#define KDM_SMDATALOGGER_H

#include "cv.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "SM_Struct.h"

class KDM_SMDataLogger
{
public:
	KDM_SMDataLogger();
	void initDataLogger(const std::string DirName);
	void writeLog(sSM_Status_t sDataLog);
	void deInitDataLogger();
	void getDayDateTimeYear();
	~KDM_SMDataLogger();

private:
	char cLogFileDir[100]; //Directory name
	int iDataLoggerFlag;
	FILE *fptrLogFile;
	pthread_mutex_t log_data_mutex;

public:
	
};

#endif
