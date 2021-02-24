/*
 * BenchTestSteerCalibPlayerProcess.h
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#ifndef BenchTestSteerCalibPlayerPROCESS_H_
#define BenchTestSteerCalibPlayerPROCESS_H_

#include <cstdint>
#include <memory>
#include "BenchTestSteerCalibPlayerConfiguration.h"
#include "ReadCalibSteer.h"

namespace BenchTestSteerCalibPlayer
{

class BenchTestSteerCalibPlayerProcess
{
	///\class BenchTestSteerCalibPlayerProcess
	///\brief A class to decode logged pcan data
public:

	BenchTestSteerCalibPlayerProcess();

	void SetConfig(const BenchTestSteerCalibPlayerConfig config);

	void operator()();

	~BenchTestSteerCalibPlayerProcess();

private:

	BenchTestSteerCalibPlayerConfig config_params;

	//
	int64_t previous_timestamp;

	//
	std::shared_ptr<calibsteerEmulator::ReadCalibSteer> sptr_ReadCalibSteer;
	bool file_EOF;

};

}
#endif /* ifndef BenchTestSteerCalibPlayerPROCESS_H_ */
