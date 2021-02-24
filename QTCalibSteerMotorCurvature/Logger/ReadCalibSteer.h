/*
 * ReadCalibSteer.h
 *
 *  Created on: Dec 25, 2017
 *      Author: emily
 */

#ifndef READCALIBSTEER_H_
#define READCALIBSTEER_H_
//
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
//
#include <iostream>
#include <unistd.h>
#include <cstdlib>
//
#include "CalibSteerStruct.h"

namespace calibsteerEmulator
{

class ReadCalibSteer
{

public:

	ReadCalibSteer();

	void SetupReadCalibSteer(const std::string log_path);

	uint32_t ReadDataLogger(sSM_Status_t &datalog); // To read and return the datalog to calling function

	void printLogData(sSM_Status_t &datalog);

	void CloseReadLog(void);  // To close the file

	~ReadCalibSteer();

private:

	FILE * mp_File;
};

}//namespace

#endif /* READCALIBSTEER_H_ */
