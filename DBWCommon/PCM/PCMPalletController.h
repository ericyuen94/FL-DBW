#ifndef PCMPALLETCONTROLLER_H_
#define PCMPALLETCONTROLLER_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include "PCMPalletDataStruct.h"
#include "Platform/PCanUsb/USBPCAN.h"
//#include "Platform/PCanUsb/PeakCanDriver.h"
#include "Common/Utility/TimeUtils.h"
//
#include "Platform/PCanUsb/PCanLogger.h"
#include "../PcanSeikoEncoder/PcanSeikoPcanEncoder.h"
//
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include "Platform/Sensors/SensorsStkciPubData.h"
#include <ros/ros.h>


namespace DBWPalletCommon
{

namespace SPEED_MODE
{
const int32_t INVALID=0;
const int32_t TURTLE=1;
const int32_t RABBIT=2;
};

struct CANData
{
	bool 		exist;
	TPCANRdMsg 	data;
	uint64_t 	timeReceive;
};

struct PCMHealthStatusFeedback
{
	//error open can ports
	bool open_emuc_can_port; //40
	//
	bool rx_line_encoder_fb; //7
	bool rx_steer_encoder_fb; //8
	bool rx_tilt_encoder_fb;
	//
	int32_t error_rx_line_encoder_fb;
	int32_t error_rx_steer_encoder_fb;
	int32_t error_rx_tilt_encoder_fb;
	
	int32_t speed_mode;
	uint64_t speed_mode_readtime;
	uint64_t speed_mode_decodetime;
	
	uint64_t battery_readtime;
	uint64_t battery_decodetime;
	
	uint64_t speed_readtime;
	uint64_t speed_decodetime;
};

struct PCMRotorFeedback
{
	float32_t EncoderPosDegree;
	int32_t EncoderPosStep;
};


class PCMPalletController
{

public:

	PCMPalletController();

	//initialization
	bool OpenCanPort(const std::string port_name);
	bool OpenVecowCanPortModule();

	//TODO:: to implement other methods to get CAN messages
	bool GetSikoSteerEncoderFeedback(uint32_t &steer_enc_pos);
	void ConvertSikoSteerCounttoAngle(uint32_t steer_enc_home_pos, uint32_t steer_enc_pos, float64_t &steer_angle_radian);
	//
	bool GetBaumerLineEncoderFeedback(uint32_t &line_enc_pos);
	bool GetBaumerTiltEncoderFeedback(uint32_t &tilt_enc_pos);

	//pallet platform message decoder
	bool DecodeSikoEncMsg(TPCANRdMsg * const pMsgBuff, const int64_t time, uint32_t &pos);
	//
	bool DecodeBaumerLineEncMsg(TPCANRdMsg * const pMsgBuff, const int64_t time, uint32_t &line_enc_pos);
	bool DecodeBaumerTiltEncMsg(TPCANRdMsg * const pMsgBuff, const int64_t time, uint32_t &tilt_enc_pos);
	//
	

	//send commands to the PCM controller card
	bool SetPCMPalletCommands(const pcm_palletbase_commands &in_pcm_command);
	void DisplayPCM_CANData();

	//pallet truck requires a tiller on first, whenever set or change speed direction
	bool PCMSpeedCtrlLogic(float64_t current_speed);
	void ResetLastCommandSpeed();

	//to do a standard one-time startup procedure for sustain robot every 5 minutes
	void SetStartUpProcedurePallet();
	//to keep the pallet truck alive without movement every 5 minutes
	void SetHeartBeatTiller();
	//to shutdown the PCM upon exit
	void SetShutdownPCMProcedure();

	//setup logger
	void SetLogger(std::string logfolder_path);

	//KW_Edit - send commands to Forklift
	void OpenForkMainValve();
	void SetForkliftForkCmds(Platform::Sensors::PalletBaseCmdMsg &in_data_basecmd);
	//
	void OpenSteerMainValve();
	void OpenSteerPropValve();
	//
	void CloseAllValves();

	//tilt motor
	void ServoGoToPos(float PositionDegree, uchar_t Speed, uint16_t TiltMotor);
	bool DecodeTiltMotorEncMsg(TPCANRdMsg * const pMsgBuff, const int64_t time);

	void ReadPCM();
	bool DecodeModeMsg();
	bool DecodePalletSpeedFbkMsg();
	bool DecodePalletBatteryFbkMsg();
	void GetBYDCANFeedback(pcm_palletbase_feedback &pcm_feedback);
	bool GetPCMPalletFeedback(PCMHealthStatusFeedback &out_health_status_fb); 

	//de-constructor
	~PCMPalletController();

private:

	//PCM encoder messages
	bool DecodePCMMsg(TPCANRdMsg * const pMsgBuff, const int64_t time,
			pcm_palletbase_feedback &data_feedback);

	void EncodePCMMsg(TPCANMsg * const pMsgBuff,
			const pcm_palletbase_commands &data_commands);

	//decode speed messages
	float32_t ComputeForwardSpeed(uint32_t data_value);
	float32_t ComputeBackwardSpeed(uint32_t data_value);

	//steer encoder messages
	void EncodeSikoEncMsg(TPCANMsg * const pMsgBuff);

	//tools for conversion
	void ConvertUIntegerToHex( const uint32_t nValue, uint8_t ( &pBuffer )[4] );
	void ConvertHexToUInteger(const uint8_t ( &pBuffer )[4], uint32_t &nValue );
	void ConvertHexToInteger(const uint8_t ( &pBuffer )[2], int16_t &nValue );

	// Steer Encoder Setup
	void SetupSteerSikoEncoder();
	void SetupCANMsgOneSteerSikoEncoder(TPCANMsg * const pMsgBuff);
	void SetupCANMsgTwoSteerSikoEncoder(TPCANMsg * const pMsgBuff);
	void SetupCANMsgThreeSteerSikoEncoder(TPCANMsg * const pMsgBuff);
	void SetupCANMsgFourSteerSikoEncoder(TPCANMsg * const pMsgBuff);
	void SetupCANMsgFiveSteerSikoEncoder(TPCANMsg * const pMsgBuff);

	// Line Encoder Setup
	void SetupLineBaumerEncoder();
	void SetupCANMsgOneLineBaumerEncoder(TPCANMsg * const pMsgBuff);
	void SetupCANMsgTwoLineBaumerEncoder(TPCANMsg * const pMsgBuff);
	void SetupCANMsgThreeLineBaumerEncoder(TPCANMsg * const pMsgBuff);
	void SetupCANMsgFourLineBaumerEncoder(TPCANMsg * const pMsgBuff);
	void SetupCANMsgFiveLineBaumerEncoder(TPCANMsg * const pMsgBuff);

	//tilt motor setup
	void receivePositionValue(TPCANMsg * const pMsgBuff, const uint32_t CAN_RX_ID);
	void SetSpeed(uchar_t DesiredSpeed, TPCANMsg * const pMsgBuff, const uint32_t CAN_RX_ID);
	void SendControlCommand(uchar_t CAN_Command,uchar_t DeviceCAN_ID, TPCANMsg * const pMsgBuff, const uint32_t CAN_RX_ID);
	void SeActDeactCommand(uchar_t CAN_Command,uchar_t DeviceCAN_ID, TPCANMsg * const pMsgBuff, const uint32_t CAN_RX_ID);
	void SendPositionSetpointCommand(int32_t PositionStep, TPCANMsg * const pMsgBuff, const uint32_t CAN_RX_ID);
	void SetAcceleration(uchar_t DesiredAcceleration, TPCANMsg * const pMsgBuff, const uint32_t CAN_RX_ID);



private:
	//
	std::shared_ptr<Platform::PCanUsb::USBPCAN> sptr_pcan;
//	std::shared_ptr<Platform::PCanUsb::USBPCAN> sptr_pcan_seiko_enc;

	//KW_Edit - PCanEmuc pointer
	std::shared_ptr<Platform::PCanEmuc::PCanEmucDriver> sptr_pcanemuc;

	//for pcan
	bool bSuccess_OpenPCANPort;
	uint32_t mu32_TPCANRdMsgSize;     // Just sizeof(TPCANRdMsg)

	//to keep the pallet truck alive every 15 minutes
	bool heartbeat_forward_motion;
	float64_t last_cmd_speed; 		 //for PCMSpeedCtrlLogic
	int32_t count_tiller_heartbeat;

	//display purposes
	DWORD display_pcm_canid;
	BYTE display_rx_pcm_canmsg[8];
	DWORD display_speed_canid;
	DWORD display_battery_canid;
	BYTE display_rx_speed_canmsg[8];
	BYTE display_rx_battery_canmsg[8];
	BYTE display_send_cmd_byte;
	BYTE display_send_speed_byte[2];
	BYTE display_send_steer_byte[2];

	// encoder
	uint32_t m_steer_enc_pos;
	uint32_t m_line_enc_pos;
	uint32_t m_tilt_enc_pos;

	//pcan logger class
	std::shared_ptr<Platform::PCanUsb::PCanLogger> sptr_PCanLogger;
	bool logging_flag_status;

	//KW_Edit
	bool alternative_byte;
	int64_t previous_timestamp;
	int32_t analog_speed;
	int32_t fork_speed;
	//
	PCMHealthStatusFeedback health_status_fb;

	//tilt motor
	bool bTargetReached;
	bool bListenFlag;
	bool bTargetReachedFinal;
	PCMRotorFeedback stat;
	std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;

	CANData LastBYDBatteryData;
	CANData LastBYDSpeedData;
	CANData LastBYDSpeedModeData;
	pcm_palletbase_feedback out_pcm_feedback;

	std::mutex mutex_BYDBattery;
	std::mutex mutex_BYDSpeed;
	std::mutex mutex_BYDSpeedMode;

};

}

#endif /* PCMPALLETCONTROLLER_H_ */
