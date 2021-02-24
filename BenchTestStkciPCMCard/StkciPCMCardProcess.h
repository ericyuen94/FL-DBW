/*
 * StkciPCMCardProcess.h
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#ifndef STKCIPCMCARDPROCESS_H_
#define STKCIPCMCARDPROCESS_H_

#include <cstdint>
#include <memory>
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "StkciPCMCardConfiguration.h"
//
#include "PCMPalletController.h"

//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"

namespace StkciPCMCard
{

class StkciPCMCardProcess
{
	///\class StkciPCMCardProcess
	///\brief A class to process data
public:

	StkciPCMCardProcess();

	void SetConfig(const StkciPCMCardConfig config);

	void InitSharedPtrs(std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds,
					 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds);

	void operator()();

	void get_pallet_feedback_thread();

	void ShutdownPCM();

	~StkciPCMCardProcess();

private:

	StkciPCMCardConfig config_params;

	//
	int64_t previous_timestamp;
	int64_t previous_fb_timestamp;

	//
	std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;

	//
	std::shared_ptr<DBWPalletCommon::PCMPalletController> sptr_PcmPalletController;
	std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds_;
	std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds_;

	//
	int32_t pub_feedback_data_count;


};

}
#endif /* ifndef STKCIPCMCARDPROCESS_H_ */

