/*
 * BydFLDriveSteerForkThread.h
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */

#ifndef BydFLDriveSteerForkThread_H_
#define BydFLDriveSteerForkThread_H_

#include <cstdint>
#include <memory>
#include <mutex>
//
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "Common/Utility/TimeUtils.h"
//
#include "GetDBWCSCIConfiguration.h"
//
#include "GetDBWEncodersConfiguration.h"
//
#include "PCMPalletController.h"
#include "ADAM_AOController.h"
#include "ADAM_ROController.h"
//
#include "RetrieveVehMeasure.h"
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"
#include "RetreiveVIMStatus.h"
#include "RetrievePLMStatus.h"
#include "RetreiveTiltMotorCmds.h"
#include "RetrieveBaseFeedback.h"
//
#include "DBW_Ctrl.h"
#include "SmartMotorSteerConfiguration.h"
//
#include "Platform/IsolatedDIO/IsolatedDIO.h"

#define CORES 4

namespace DBW
{

namespace DbwGear
{
const int32_t NEUTRAL=1;
const int32_t FORWARD=2;
const int32_t REVERSE=3;
};

struct dbw_liveness_status
{
	bool receive_dbwcmd_msg_tele;
	bool receive_dbwcmd_msg_vcm;
	//
	bool receive_basecmd_msg;
	bool receive_vimstatus_msg;
	bool receive_plmstatus_msg;
	bool receive_plmerrorstatus_msg;
	bool detected_lost_auxillary_msgs;
	//
	bool hw_estop_status;
	bool unmmaned_mode;
	bool solenoid_status;
	bool piston_status;
	bool last_unmmaned_mode;
	bool exceeded_speed;
	bool exceeded_steer;
	bool exceeded_forkheight;
	bool exceeded_battlevel;
	bool exceeded_temperature;

	//====From here on is for warning and error plm status===========
	//
	bool send_stop_cmd_dbw; //warning 1
	bool temperature_warning;
	//stkci message comms status
	int32_t error_plmstatus_count; //1
	int32_t error_plmerrorstatus_count; //
	int32_t error_vimstatus_count; //2
	int32_t error_rxdrvcmd_tele_count; //3
	int32_t error_rxdrvcmd_vcm_count; //4
	int32_t error_rxpalletbasecmd_count; //5
	int32_t error_rxpalletbasefb_count; //6
	int32_t error_temperaturelimit_count; //7

	//pcm feedback status
	DBWPalletCommon::PCMHealthStatusFeedback pcm_feedback_healthstatus;

	bool connect_AO1; //41
	bool connect_AO2; //42
	bool connect_RO1; //43
	bool connect_RO2;
	bool connect_RO3;

	bool bConnectedSteerMotor;
	bool open_dio_port;

	//
	Platform::Sensors::PalletBaseCmdMsg current_palletbase_cmd;
	Platform::Sensors::PalletDbwCmdMsg current_drive_cmd;
	float desired_platform_forkheight;
	float current_platform_forkheight;
	float64_t current_platform_speed;
	float64_t current_platform_battlevel;
	float64_t current_platform_steer;
};

class BydFLDriveSteerForkThread
{

	///\ brief This class is a controller to command the Drive motor HWCI
	///\ note 

public:

	BydFLDriveSteerForkThread();

	BydFLDriveSteerForkThread(std::shared_ptr<RetrieveVehMeasure> sptr_RetrieveVehMeasure,
					std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmdsTeleOps,
					std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmdsVCM,
					std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds,
					std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus,
					std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMStatus,
					std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMErrorStatus,
					std::shared_ptr<RetreiveTiltMotorCmds> sptr_RetreiveTiltMotorCmds,
					std::shared_ptr<RetrieveBaseFeedback> sptr_RetrieveBaseFeedback);

	void SetConfigParams(DBWCSCI_Config config, DBWEncodersConfig encoder_config, DBWPalletCommon::SmartMotorSteerConfig params_brake_config);

	void Thread_MainControl();

	void Thread_ValveControl();

	void Thread_TiltMotorControl();

	void Thread_FeedbackDBWandPalletBaseStatus();

	void ShutdownDBWDriveAndSteer();

	void GetPLMHealthStatus(dbw_liveness_status &out_msg_health_status);

	void Thread_ReadPCM();
	void Thread_DecodeBattery();
	void Thread_DecodeSpeed();
	void Thread_DecodeSpeedMode();
	void TorqueControl_Thread();


	~BydFLDriveSteerForkThread();

private:
	
	void GetDriveCmdsAndStatus();

	void PrintDBWStatus();

private:

	void InitAdamController();

	void CheckHome();

	// Fork and Steer Controls
	void stopControls();
	//
	void SteerControl(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd);
	void startSteerControl();
	void stopSteerControl();
	//
	void ForkControl(Platform::Sensors::PalletBaseCmdMsg data_basecmd);
	void startForkControl(Platform::Sensors::PalletBaseCmdMsg data_basecmd);
	void stopForkControl();
	//
	void SetCurrentCommands(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd, Platform::Sensors::PalletBaseCmdMsg data_basecmd);

	// Drive Controls
	void CmdSpeedToVotlage(float cmd_speed_, float &ch0_voltage_);
	void CmdBrake(float cmd_brake_);

	//
	void DecodeVCM();

	//ck - stop forklift control
	void stopForkliftControl();

	//ck - change gear
	void CommandVehSpeedChange(const Platform::Sensors::PalletDbwCmdMsg dbw_cmd);

	//ck - change gear
	void ChangeGear(const int32_t desired_gear_position);

	//ck
	void SetPIMCommands(const Platform::Sensors::PalletDbwCmdMsg &dbw_cmd_, const Platform::Sensors::PalletBaseCmdMsg &palletbase_cmd_, bool bError);

	//
	bool CalibrateBrake();
	void TorqueBrake(void);

private:

	//
	DBWCSCI_Config config_params;
	//
	DBWEncodersConfig encoder_config_params;
	//
	std::shared_ptr<RetrieveVehMeasure> sptr_RetrieveVehMeasure_;
	std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmdsTeleOps_;
	std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmdsVCM_;
	std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds_;
	std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus_;
	std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMStatus_;
	std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMErrorStatus_;
	std::shared_ptr<RetreiveTiltMotorCmds> sptr_RetreiveTiltMotorCmds_;
	std::shared_ptr<RetrieveBaseFeedback> sptr_RetrieveBaseFeedback_;


	//
	std::mutex mutex_system_state;
	std::mutex mutex_drive_commands;
	std::mutex mutex_base_pallet_commands;
	
	//
	std::mutex mutex_AOController;
	std::mutex mutex_CANBus;

	//
	uint64_t lasttime_thread_base_pallet_status;

	//
	std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;

	//
	std::shared_ptr<DBWPalletCommon::PCMPalletController> sptr_PcmPalletController;
	std::shared_ptr<DBWPalletCommon::Adam_AOController> sptr_ADAM_AOController1;
	std::shared_ptr<DBWPalletCommon::Adam_AOController> sptr_ADAM_AOController2;
	std::shared_ptr<DBWPalletCommon::Adam_ROController> sptr_ADAM_ROController;

	//
	//
	std::shared_ptr<DBWPalletCommon::Adam_ROController> sptr_ADAM_ROController_DrvPedal;
	std::shared_ptr<DBWPalletCommon::Adam_AOController> sptr_ADAM_AOController_DrvSteer_Voltage;
	std::shared_ptr<DBWPalletCommon::Adam_ROController> sptr_ADAM_ROController_Gear;
	std::shared_ptr<DBWPalletCommon::Adam_AOController> sptr_ADAM_AOController_Fork_Ctrl;
	std::shared_ptr<DBWPalletCommon::Adam_ROController> sptr_ADAM_ROController_Fork_Ctrl;

	//
	std::shared_ptr<Platform::IsolatedDIO::IsolatedDIODriver> sptr_DIODriver;

	//
	std::shared_ptr<DBW_Ctrl> sptr_SmartMotorSteer;

	//
	DBWPalletCommon::SmartMotorSteerConfig params_brake_config_;

	//
	dbw_liveness_status msg_health_status;
	Platform::Sensors::GeometryMsgsVehicleMeasure gps_vehmeas_data;
	Platform::Sensors::SystemMsgsUGVPlatformStatus plm_status;
	Platform::Sensors::SystemMsgsUGVPlatformStatus plm_error_status;
	Platform::Sensors::PalletAuxFbkMsg vim_status;
	Platform::Sensors::PalletBaseCmdMsg palletbase_cmd;
	Platform::Sensors::PalletDbwCmdMsg dbw_cmd;
	Platform::Sensors::GeometryMsgsVehicleMeasure platform_vehmeas;

	//Forklift controller parameters
	int64_t previous_timestamp;
	int64_t previous_drive_timestamp;
	int64_t previous_fb_timestamp;
	int32_t pub_feedback_data_count;
	int64_t previous_ro_timestamp;
	//
	bool bHomeSteer;
	bool bHomeFork;
	bool bHomeInit;
	//
	bool bReset;
	bool bForkControl;
	bool bSteerControl;
	bool bActivateForkControl;
	bool bActivateSteerControl;
	bool bDeActivateValveControl;
	float prop_valve_voltage;
	float previous_cmd_steer_;
	float previous_cmd_speed_;
	float previous_cmd_brake_;
	//
	float previous_ro_cmd_speed_;
	bool bROchannel0;
	bool bROchannel1;
	bool bROchannel2;
	//
	int RelayOutputReset_Counter;
	//
	float last_desired_fork_height;
	float last_desired_tilt_angle;
	bool bForkHeight;
	bool bTiltAngle;
	uint32_t ForkHeightCounter;
	uint32_t TiltAngleCounter;
	bool bTransitFork;
	bool bTransitTilt;
	bool bForkMoving;
	int tilt_count;
	//
	bool transitionManned;
	//
	bool bSteerLeft;
	bool bSteerRight;
	//
	bool bActiveGear;
	bool bRaiseFork;
	bool bTiltFork;
	bool bSteerAction;
	//
	float volt_test;
	float current_height;
	//
	Platform::Sensors::PalletBaseCmdMsg basecmd;
	Platform::Sensors::PalletDbwCmdMsg dbw_cmd_;
	Platform::Sensors::PalletBaseCmdMsg base_cmd_;
	//
	DBWPalletCommon::pcm_palletbase_feedback pcm_feedback;
	uint32_t steer_enc_pos;
	uint32_t line_enc_pos;
	uint32_t tilt_enc_pos;

	//
	int32_t previous_gear_position;

//
	std::ofstream ptrFileText_decodevcm_Logger;

	//
	uint8_t in_dio_feedback;

	//TODO::
//	std::shared_ptr<DBWPalletCommon::PCMPalletController> sptr_pcm_dbw_drive;
//	DBWPalletCommon::pcm_palletbase_commands pcm_commands;
//	DBWPalletCommon::pcm_palletbase_feedback pcm_feedback;
//	uint32_t steer_enc_pos;
//	float64_t steer_seiko_enc_fb_angle_radians;


	// PID Stuff
	float32_t steer_max_deg;
	float32_t steer_encoder_resolution; // degree per encoder pulse
	float32_t steer_error;
	float32_t steer_error_previous;
	float32_t steer_error_integral;

	//for CPU current temperature
	double value=0;
	double total=0;
	double average=0;
	FILE *f;

	void DecodeVCMnComputeSteerPIDOutput();
	float32_t ComputeSteerDeg(int32_t encoder_pos);

	bool bSafetyLidar;

	double Prev_torque;
};

}//BydFLDriveSteerForkThread

#endif /* BydFLDriveSteerForkThread_H_ */

