/*
 * BenchTestEncoderProcess.h
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#ifndef BENCHTESTENCODERPROCESS_H_
#define BENCHTESTENCODERPROCESS_H_

#include <cstdint>
#include <memory>
#include "BenchTestEncoderConfiguration.h"
#include "PcanSeikoPcanEncoder.h"

//

namespace BenchTestEncoder
{

class BenchTestEncoderProcess
{
	///\class BenchTestEncoderProcess
	///\brief A class to process data
public:

	BenchTestEncoderProcess();

//	void SetConfig();
	void SetConfig(const BenchTestEncoderConfig config);

	void operator()();

	~BenchTestEncoderProcess();

private:

	BenchTestEncoderConfig config_params;

	//
	int64_t previous_timestamp;

	//
	std::shared_ptr<DBWPalletCommon::PcanSeikoPcanEncoder> sptr_PcanSeikoPcanEncoder;
};

}
#endif /* ifndef BenchTestEncoderPROCESS_H_ */
