/*
 * BenchTestPCanPlayerProcess.h
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#ifndef BENCHTESTPCANPLAYERPROCESS_H_
#define BENCHTESTPCANPLAYERPROCESS_H_

#include <cstdint>
#include <memory>
#include "BenchTestPCanPlayerConfiguration.h"
//
#include "Platform/PCanUsb/PCanLogRead.h"
#include "PCMPalletController.h"

namespace BenchTestPCanPlayer
{

class BenchTestPCanPlayerProcess
{
	///\class BenchTestPCanPlayerProcess
	///\brief A class to decode logged pcan data
public:

	BenchTestPCanPlayerProcess();

	void SetConfig(const BenchTestPCanPlayerConfig config);

	void operator()();

	~BenchTestPCanPlayerProcess();

private:

	BenchTestPCanPlayerConfig config_params;

	//
	int64_t previous_timestamp;

	//
	std::shared_ptr<Platform::PCanUsb::PCanLogRead> sptr_PCanLogRead;
	std::shared_ptr<DBWPalletCommon::PCMPalletController> sptr_PCMPalletController;

	//
	int64_t last_timestamp_rx_all;
	int64_t last_timestamp_rx_battery_msg;
};

}
#endif /* ifndef BENCHTESTPCANPLAYERPROCESS_H_ */
