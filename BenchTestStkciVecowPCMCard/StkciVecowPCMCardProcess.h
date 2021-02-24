/*
 * StkciVecowPCMCardProcess.h
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#ifndef StkciVecowPCMCARDPROCESS_H_
#define StkciVecowPCMCARDPROCESS_H_

#include <cstdint>
#include <memory>
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "StkciVecowPCMCardConfiguration.h"
//
#include "PCMPalletController.h"
#include "Platform/IsolatedDIO/IsolatedDIO.h"
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"

namespace StkciVecowPCMCard
{

class StkciVecowPCMCardProcess
{
	///\class StkciVecowPCMCardProcess
	///\brief A class to process data
public:

	StkciVecowPCMCardProcess();

	void SetConfig(const StkciVecowPCMCardConfig config);

	void InitSharedPtrs(std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds,
					 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds);

	void operator()();

	void get_pallet_feedback_thread();

	void ShutdownPCM();

	~StkciVecowPCMCardProcess();

private:

	StkciVecowPCMCardConfig config_params;

	//
	int64_t previous_timestamp;
	int64_t previous_fb_timestamp;

	//
	std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;

	//
	std::shared_ptr<DBWPalletCommon::PCMPalletController> sptr_PcmPalletController;
	std::shared_ptr<Platform::IsolatedDIO::IsolatedDIODriver> sptr_IsolatedDIODriver;

	//
	std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds_;
	std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds_;

	//
	int32_t pub_feedback_data_count;


};

}
#endif /* ifndef StkciVecowPCMCARDPROCESS_H_ */

