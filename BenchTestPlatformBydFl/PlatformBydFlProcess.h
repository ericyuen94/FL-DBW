/*
 * PlatformBydFlProcess.h
 *
 *  Created on: July 1, 2019
 *      Author: Tester
 */

#ifndef PlatformBydFlPROCESS_H_
#define PlatformBydFlPROCESS_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "PlatformBydFlConfiguration.h"
//
#include "PCMPalletController.h"
#include "ADAM_AOController.h"
#include "ADAM_ROController.h"
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"
#include "RetreiveTiltMotorFbkMsgs.h"

//
#include "RetrieveBaseFeedback.h"

#include "Platform/IsolatedDIO/IsolatedDIO.h"

#include "DBW_Ctrl.h"
#include "SmartMotorSteerConfiguration.h"


//
#include "mu_serial.h"
#include "smartmotor.h"

namespace PlatformBydFl
{

namespace DbwGear
{
const int32_t NEUTRAL=1;
const int32_t FORWARD=2;
const int32_t REVERSE=3;
};

enum motor_mode
{
   position_mode
};

//
class PlatformBydFlProcess
{
	///\class PlatformBydFlProcess
	///\brief A class to process data
public:

	PlatformBydFlProcess();

	void SetConfig(const PlatformBydFlConfig config);

	void SetConfigParams(DBWPalletCommon::SmartMotorSteerConfig params_brake_config);

	void InitSharedPtrs(std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds,
					 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds,
					 std::shared_ptr<RetreiveTiltMotorFbkMsgs> sptr_RetreiveTiltMotorFbkMsgs);

	bool InitAdamController();

	void operator()();

	void SetMotorMode(int acc, int vel, motor_mode mode);

	void SetMotorPos(int pos, int complete_traj);

	void WaitTrajBitClear(mu_smartmotor_p brake_motor);

	int get_motor_trajBit(mu_smartmotor_p brake_motor);

	//
	void SetPIMCommands(const Platform::Sensors::PalletDbwCmdMsg &dbw_cmd_, const Platform::Sensors::PalletBaseCmdMsg &palletbase_cmd_);

	void ResetEncoder();

	void FeedbackDBWandPalletBaseStatus();

	void Shutdown();

	~PlatformBydFlProcess();

private:

	void CommandVehSpeedChange(const Platform::Sensors::PalletDbwCmdMsg dbw_cmd);

	void CmdSpeedToVoltage(float cmd_speed, float &ch0_voltage);

	void ChangeGear(const int32_t desired_gear_position);

	void ForkControl(Platform::Sensors::PalletBaseCmdMsg data_basecmd);

	void SteerControl(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd);

	void StopSteerControl();

	void GetDriveCmdsAndStatus();

	void SetCurrentCommands(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd, Platform::Sensors::PalletBaseCmdMsg data_basecmd);

	void stopControls();

	void StopForkControl();

	void decodeSteer();

	void StopDriveControl();

private:

	PlatformBydFlConfig config_params;

	//
	std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;

	//
	std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds_;
	std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds_;
	std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmdsTeleOps_;
	std::shared_ptr<RetreiveTiltMotorFbkMsgs> sptr_RetreiveTiltMotorFbkMsgs_;

	std::shared_ptr<Platform::IsolatedDIO::IsolatedDIODriver> sptr_DIODriver;





	//
	std::shared_ptr<DBWPalletCommon::Adam_ROController> sptr_ADAM_ROController_DrvPedal;
	std::shared_ptr<DBWPalletCommon::Adam_AOController> sptr_ADAM_AOController_DrvSteer_Voltage;
	std::shared_ptr<DBWPalletCommon::Adam_ROController> sptr_ADAM_ROController_Gear;
	std::shared_ptr<DBWPalletCommon::Adam_AOController> sptr_ADAM_AOController_Fork_Ctrl;
	std::shared_ptr<DBWPalletCommon::Adam_ROController> sptr_ADAM_ROController_Fork_Ctrl;

	//
	std::shared_ptr<DBWPalletCommon::PCMPalletController> sptr_PcmPalletController;

	//
	std::shared_ptr<RetrieveBaseFeedback> sptr_RetrieveBaseFeedback_;

	//
	DBWPalletCommon::PCMHealthStatusFeedback pcm_feedback_healthstatus;

	//
	DBWPalletCommon::pcm_palletbase_feedback pcm_feedback;

	//
	Platform::Sensors::PalletBaseCmdMsg previous_data_forkbasecmd;

	Platform::Sensors::PalletDbwCmdMsg previous_data_steerDBWcmd;

	//
	int64_t previous_timestamp;
	bool success_setup_adam_controller;
	bool bStart;
	bool bSteer;
	bool bStopSteer;
	uint32_t steer_enc_pos;
	bool bForkControl;
	bool bSteerControl;
	bool bActivateForkControl;
	bool bActivateSteerControl;
	bool bDeActivateValveControl;
	bool bTest;
	bool bDrive;
	bool test1;

	//
	Platform::Sensors::PalletDbwCmdMsg previous_in_data_dbwcmd;
	int32_t previous_gear_position;

	Platform::Sensors::PalletBaseCmdMsg palletbase_cmd;
	Platform::Sensors::PalletDbwCmdMsg dbw_cmd;
	Platform::Sensors::PalletDbwCmdMsg dbw_cmd_teleop;

	//
	std::mutex mutex_CANBus;

	//for brake
	bool bBrakeMotorEnable;
	int fd;
	mu_smartmotor_p brake_motor;

	std::shared_ptr<DBW_Ctrl> sptr_SmartMotorSteer;
	DBWPalletCommon::SmartMotorSteerConfig params_brake_config_;



};

}
#endif /* ifndef PlatformBydFlPROCESS_H_ */

