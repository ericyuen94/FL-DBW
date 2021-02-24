/*
 * TestPCMCardProcess.h
 *
 *  Created on: Oct 1, 2017
 *      Author: Emily Low
 */

#ifndef TESTPCMCARDPROCESS_H_
#define TESTPCMCARDPROCESS_H_

#include <cstdint>
#include <memory>
#include "TestPCMCardConfiguration.h"
//
#include "Platform/PCanUsb/USBPCAN.h"
#include "Platform/PCanUsb/PCanLogger.h"

namespace TestPCMCard
{

class TestPCMCardProcess
{
	///\class TestPCMCardProcess
	///\brief A class to process data
public:

	TestPCMCardProcess();

	void SetConfig(const TestPCMCardConfig config);

	void operator()();

	~TestPCMCardProcess();

private:

	bool DecodePCMMsg(TPCANRdMsg * const pMsgBuff, const int64_t time);

	bool DecodeSeikoEncoderMsg(TPCANRdMsg * const pMsgBuff, const int64_t time);

	bool TransmitPCMMsg(TPCANRdMsg * const pMsgBuff, const int64_t time);

	void EncodePCMMsg(TPCANMsg * const pMsgBuff);

	TestPCMCardConfig config_params;

	//
	int64_t previous_timestamp;

	//
	std::shared_ptr<Platform::PCanUsb::USBPCAN> sptr_pcan;
	bool bSuccess_OpenPCANPort;
	uint32_t mu32_TPCANRdMsgSize;     // Just sizeof(TPCANRdMsg)

	//
	std::shared_ptr<Platform::PCanUsb::PCanLogger> sptr_PCanLogger;

};

}
#endif /* ifndef TESTPCMCARDPROCESS_H_ */
