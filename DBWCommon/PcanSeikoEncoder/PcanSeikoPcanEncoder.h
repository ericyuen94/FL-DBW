#ifndef PCANSEIKOPCANENCODER_H_
#define PCANSEIKOPCANENCODER_H_

#include <iostream>
#include <sstream>
#include <cstdint>
#include <memory>
#include "Platform/PCanUsb/USBPCAN.h"
#include "Common/Utility/TimeUtils.h"

#include "Platform/PCanEmuc/PCanEmuc.h"

namespace DBWPalletCommon
{

class PcanSeikoPcanEncoder
{

public:

	PcanSeikoPcanEncoder();

	bool OpenCanPort(const std::string port_name);

	bool GetSeikoSteerFeedback(uint32_t &encoder_position);

	void ConvertSeikoSteerCounttoAngle(uint32_t steer_enc_home_pos, uint32_t steer_enc_pos, float64_t &steer_angle_radian);

	bool DecodeSeikoEncMsg(TPCANRdMsg * const pMsgBuff, const int64_t time, uint32_t &pos);

	void EncodeSeikoEncMsg(TPCANMsg * const pMsgBuff);

	~PcanSeikoPcanEncoder();

private:

	void SetupDefaultSeikoEncoder();

	void SetupCANMsgOneSeikoEncoder(TPCANMsg * const pMsgBuff);

	void SetupCANMsgTwoSeikoEncoder(TPCANMsg * const pMsgBuff);

	void SetupCANMsgThreeSeikoEncoder(TPCANMsg * const pMsgBuff);

	void SetupCANMsgFourSeikoEncoder(TPCANMsg * const pMsgBuff);

	void SetupCANMsgFiveSeikoEncoder(TPCANMsg * const pMsgBuff);

private:

	std::shared_ptr<Platform::PCanUsb::USBPCAN> sptr_pcan_encoder;

	//KW_Edit - PCanEmuc pointer
	std::shared_ptr<Platform::PCanEmuc::PCanEmucDriver> sptr_pcanemuc_encoder;


	bool bSuccess_OpenPCANPort;
	uint32_t mu32_TPCANRdMsgSize;     // Just sizeof(TPCANRdMsg)
};

}

#endif /* PCANSEIKOPCANENCODER_H_ */
