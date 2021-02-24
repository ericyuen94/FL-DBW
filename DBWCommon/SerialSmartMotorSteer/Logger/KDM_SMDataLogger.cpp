/**************************************************************************************
* KDM_SMDataLogger.cpp code by Loh Yong Hua, USC, KDM.
***************************************************************************************/
#include "KDM_SMDataLogger.h"

/*
 * This function KDM_ESTOPDataLogger() is the constructor of the class.
 */
KDM_SMDataLogger::KDM_SMDataLogger()
{
	iDataLoggerFlag = 0;
}

/*
 * This function initDataLogger is to init the file logger.
 */
void KDM_SMDataLogger::initDataLogger(const std::string DirName)
{
	char cLogFileName[200]; //File name

	//create a directory based on the system time.
	getDayDateTimeYear();
	std::string pathlog;
	pathlog="";
	pathlog.append(DirName);
	pathlog.append("/");
	pathlog.append(cLogFileDir);
	umask(0);
	mkdir(pathlog.c_str(), 0777);

	//
	sprintf(cLogFileName,"%s/SMDataLog.log",pathlog.c_str());
	fptrLogFile = fopen(cLogFileName,"wb");

	//
	if (fptrLogFile == NULL)
		printf("Error Opening File for Writing!\n");
	else
		iDataLoggerFlag = 1;
	fprintf(stderr,"[initDataLogger]\n");
	std::cout << "cLogFileName = " << cLogFileName << std::endl;
}

/*
 * This function writeLog write data to respective file.
 */
void KDM_SMDataLogger::writeLog(sSM_Status_t sDataLog)
{
	if (iDataLoggerFlag)
	{
		pthread_mutex_lock(&log_data_mutex);
		fwrite(&sDataLog, sizeof(sDataLog), 1, fptrLogFile);
		pthread_mutex_unlock(&log_data_mutex);
	}
}

/*
 * This function deInitDataLogger is to de-init the file logger.
 */
void KDM_SMDataLogger::deInitDataLogger()
{
	if (fptrLogFile != NULL)
	{
		fclose(fptrLogFile);
		fptrLogFile = NULL;
	}
	iDataLoggerFlag = 0;
}

/*This function get the computer system time and date and create
 *a directory based on the system time and date.
 */
void KDM_SMDataLogger::getDayDateTimeYear()
{
	time_t rawtime;
	time(&rawtime);

	struct tm *timeinfo = localtime(&rawtime);
	std::string s = asctime(timeinfo);
	sprintf(cLogFileDir,"Log_Date_d%d_m%d_Time_h%d_m%d_s%d",timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
	umask(0);
	mkdir(cLogFileDir, 0777);
}

/*
 * This function ~KDM_DataLogger() is the de-constructor of the class.
 */
KDM_SMDataLogger::~KDM_SMDataLogger()
{
	if (fptrLogFile != NULL)
	{
		fclose(fptrLogFile);
		fptrLogFile = NULL;
	}
}
