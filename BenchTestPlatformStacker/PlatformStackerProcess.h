#ifndef PlatformStackerPROCESS_H_
#define PlatformStackerPROCESS_H_

#include <cstdint>
#include <memory>
#include "PlatformStackerConfiguration.h"
//
#include "Platform/PCanUsb/USBPCAN.h"
#include "Platform/PCanUsb/PCanLogger.h"
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"
//
#include "Platform/PCanEmuc/PCanEmuc.h"

namespace PlatformStacker
{

class PlatformStackerProcess
{
	///\class PlatformStackerProcess
	///\brief A class to process data
public:

	PlatformStackerProcess();

	//void SetConfig();
	void SetConfig(const PlatformStackerConfig config);

	void InitSharedPtrs(std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds,
					 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds);

	void operator()();

	~PlatformStackerProcess();

private:

	void EncodeStackerMsg(TPCANMsg * const pMsgBuff, uchar_t out_byte_two, Platform::Sensors::PalletDbwCmdMsg &in_data_dbwcmd, Platform::Sensors::PalletBaseCmdMsg &in_data_basecmd);

	void ConvertUIntegerToHex( const uint32_t nValue, uint8_t ( &pBuffer )[4] );

	PlatformStackerConfig config_params;

	//
	int64_t previous_timestamp;
	int32_t analog_speed;
	int32_t fork_speed;
	bool alternative_byte;

	//
	std::shared_ptr<Platform::PCanUsb::USBPCAN> sptr_pcan;
	bool bSuccess_OpenPCANPort;
	uint32_t mu32_TPCANRdMsgSize;     // Just sizeof(TPCANRdMsg)

	//
	std::shared_ptr<Platform::PCanUsb::PCanLogger> sptr_PCanLogger;
	std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds_;
	std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds_;

	//KW_Edit - PCanEmuc pointer
	std::shared_ptr<Platform::PCanEmuc::PCanEmucDriver> sptr_pcanemuc;
};

}
#endif /* ifndef PlatformStackerPROCESS_H_ */
