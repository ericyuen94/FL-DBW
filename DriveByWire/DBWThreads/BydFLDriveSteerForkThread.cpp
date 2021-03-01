/*
 * BydFLDriveSteerForkThread.h
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */

#include "BydFLDriveSteerForkThread.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include "Platform/PlatformDefinition.h"
//
namespace DBW
{

BydFLDriveSteerForkThread::BydFLDriveSteerForkThread()
{
}

BydFLDriveSteerForkThread::BydFLDriveSteerForkThread(std::shared_ptr<RetrieveVehMeasure> sptr_RetrieveVehMeasure,
													 std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmdsTeleOps,
													 std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmdsVCM,
													 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds,
													 std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus,
													 std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMStatus,
													 std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMErrorStatus,
													 std::shared_ptr<RetreiveTiltMotorCmds> sptr_RetreiveTiltMotorCmds,
													 std::shared_ptr<RetrieveBaseFeedback> sptr_RetrieveBaseFeedback) : sptr_RetrieveVehMeasure_(sptr_RetrieveVehMeasure),
																														sptr_RetreiveDBWCmdsTeleOps_(sptr_RetreiveDBWCmdsTeleOps),
																														sptr_RetreiveDBWCmdsVCM_(sptr_RetreiveDBWCmdsVCM),
																														sptr_RetreiveBaseCmds_(sptr_RetreiveBaseCmds),
																														sptr_RetreiveVIMStatus_(sptr_RetreiveVIMStatus),
																														sptr_RetrievePLMStatus_(sptr_RetrievePLMStatus),
																														sptr_RetrievePLMErrorStatus_(sptr_RetrievePLMErrorStatus),
																														sptr_RetreiveTiltMotorCmds_(sptr_RetreiveTiltMotorCmds),
																														sptr_RetrieveBaseFeedback_(sptr_RetrieveBaseFeedback)
{
	//init conditions
	msg_health_status.receive_dbwcmd_msg_tele = false;
	msg_health_status.receive_dbwcmd_msg_vcm = false;
	msg_health_status.receive_basecmd_msg = false;
	msg_health_status.receive_vimstatus_msg = false;
	msg_health_status.receive_plmstatus_msg = false;
	msg_health_status.hw_estop_status = false;
	msg_health_status.unmmaned_mode = false;
	msg_health_status.last_unmmaned_mode = false;
	msg_health_status.send_stop_cmd_dbw = false;
	msg_health_status.error_plmstatus_count = 0;							 //1
	msg_health_status.error_vimstatus_count = 0;							 //2
	msg_health_status.error_rxdrvcmd_tele_count = 0;						 //3
	msg_health_status.error_rxdrvcmd_vcm_count = 0;							 //4
	msg_health_status.error_rxpalletbasecmd_count = 0;						 //5
	msg_health_status.error_rxpalletbasefb_count = 0;						 //6
	msg_health_status.pcm_feedback_healthstatus.rx_line_encoder_fb = false;	 //7
	msg_health_status.pcm_feedback_healthstatus.rx_steer_encoder_fb = false; //8
	msg_health_status.pcm_feedback_healthstatus.open_emuc_can_port = false;	 //40
	msg_health_status.connect_AO1 = false;									 //41
	msg_health_status.connect_AO2 = false;									 //42
	msg_health_status.connect_RO1 = false;									 //43
	msg_health_status.connect_RO2 = false;
	msg_health_status.connect_RO3 = false;
	msg_health_status.pcm_feedback_healthstatus.rx_line_encoder_fb = false;
	msg_health_status.pcm_feedback_healthstatus.rx_steer_encoder_fb = false;
	msg_health_status.pcm_feedback_healthstatus.rx_tilt_encoder_fb = false;
	//
	msg_health_status.exceeded_speed = false;
	msg_health_status.exceeded_steer = false;
	msg_health_status.exceeded_forkheight = false;
	msg_health_status.exceeded_battlevel = false;
	msg_health_status.exceeded_temperature = false;
	//
	vim_status.manned_unmanned_switch = Platform::PIM::SWITCH_MANNED;

	//
	msg_health_status.bConnectedSteerMotor = false;
	msg_health_status.open_dio_port = false;

	//init forklift controller parameters
	previous_timestamp = 0;
	previous_fb_timestamp = 0;
	previous_drive_timestamp = 0;
	previous_ro_timestamp = 0;
	pub_feedback_data_count = 0;
	bReset = false;
	bForkControl = false;
	bSteerControl = false;
	bActivateForkControl = false;
	bActivateSteerControl = false;
	bDeActivateValveControl = false;
	prop_valve_voltage = 0;
	previous_cmd_speed_ = 0;
	previous_cmd_brake_ = 0;
	basecmd.trigger_tiller = 0x00;
	//
	bHomeSteer = false;
	bHomeFork = false;
	bHomeInit = false;
	//
	transitionManned = false;
	//
	bActiveGear = false;

	//
	RelayOutputReset_Counter = 0;
	previous_ro_cmd_speed_ = 0;
	bROchannel0 = false;
	bROchannel1 = false;
	bROchannel2 = false;
	//
	last_desired_fork_height = 0.06;
	last_desired_tilt_angle = 270;
	ForkHeightCounter = 0;
	TiltAngleCounter = 0;
	bForkHeight = false;
	bTiltAngle = false;
	bTransitFork = false;
	bTransitTilt = false;
	//
	bSteerRight = false;
	bSteerLeft = false;
	previous_gear_position = DbwGear::NEUTRAL;
	bRaiseFork = false;
	bTiltFork = false;
	bSteerAction = false;
	bForkMoving = false;

	volt_test = 0.0;

	//set publishers
	ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
	std::vector<std::string> interface_list;
	interface_list.push_back("PalletBaseFb");
	interface_list.push_back("PalletDBWFb");
	interface_list.push_back("PalletAuxCmd");
	interface_list.push_back("VehicleMeasure");
	ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);

	//
	ptrFileText_decodevcm_Logger.open("./logger_decodevcm.txt", std::fstream::out | std::fstream::binary);
	//TODO::
	//DBWPalletCommon::StkciPCMPalletBoundary::ResetPalletPCMCommad(pcm_commands);
	//sptr_pcm_dbw_drive = std::make_shared<DBWPalletCommon::PCMPalletController>();
	//sptr_pcm_dbw_drive->ResetLastCommandSpeed();

	steer_enc_pos = 0;

	steer_error = 0.0;
	steer_error_previous = 0.0;
	steer_error_integral = 0.0;

	tilt_count = 0;

	bSafetyLidar = false;
}

void BydFLDriveSteerForkThread::GetPLMHealthStatus(dbw_liveness_status &out_msg_health_status)
{
	out_msg_health_status = msg_health_status;
}

void BydFLDriveSteerForkThread::PrintDBWStatus()
{
	system("clear");
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	//
	std::cout << "System Time " << current_timestamp << std::endl;
	std::cout << " " << std::endl;

	//
	if ((msg_health_status.pcm_feedback_healthstatus.open_emuc_can_port) && (msg_health_status.connect_AO1) && (msg_health_status.connect_AO2) && (msg_health_status.connect_RO1))
	{
		//
		if (msg_health_status.receive_vimstatus_msg)
		{
			std::cout << "Received VIM Status Msg = True " << std::endl;
		}
		else
		{
			std::cout << "Received VIM Status Msg = False " << std::endl;
		}

		//
		if (msg_health_status.receive_plmstatus_msg)
		{
			std::cout << "Receive PLM Status Msg = True " << std::endl;
		}
		else
		{
			std::cout << "Receive PLM Status Msg = False " << std::endl;
		}

		if (msg_health_status.receive_dbwcmd_msg_tele)
		{
			std::cout << "Received TeleOps Commands = True " << std::endl;
		}
		else
		{
			std::cout << "Received TeleOps Commands = False " << std::endl;
		}

		//
		if (msg_health_status.receive_dbwcmd_msg_vcm)
		{
			std::cout << "Received VCM Commands = True " << std::endl;
		}
		else
		{
			std::cout << "Received VCM Commands = False " << std::endl;
		}

		//
		if (msg_health_status.receive_basecmd_msg)
		{
			std::cout << "Received BaseCmd Msg = True " << std::endl;
		}
		else
		{
			std::cout << "Received BaseCmd Msg = False " << std::endl;
		}

		//
		std::cout << " " << std::endl;
		if (msg_health_status.unmmaned_mode)
		{
			std::cout << "Drive Cmds from VCM " << std::endl;
		}
		else
		{
			std::cout << "Drive Cmds from TeleOps" << std::endl;
		}

		if (msg_health_status.hw_estop_status)
		{
			std::cout << "HW ESTOP Status = Activated " << std::endl;
		}
		else
		{
			std::cout << "HW ESTOP Status = NOT Activated " << std::endl;
		}

		if (msg_health_status.send_stop_cmd_dbw)
		{
			std::cout << "Send Stop Cmd DBW = True " << std::endl;
		}
		else
		{
			std::cout << "Send Stop Cmd DBW = False " << std::endl;
		}

		std::cout << " " << std::endl;
		std::cout << "Desired Speed = " << msg_health_status.current_drive_cmd.cmd_speed << std::endl;
		std::cout << "Desired Steer = " << msg_health_status.current_drive_cmd.cmd_steer << std::endl;
		//		std::cout << "Desired Fork Height = " << msg_health_status.desired_platform_forkheight << std::endl;
		//		std::cout << "Current Fork Height = " << msg_health_status.current_platform_forkheight << std::endl;
	}
	else
	{
		if (msg_health_status.pcm_feedback_healthstatus.open_emuc_can_port == false)
		{
			std::cout << "[ERROR] Connection to CAN PORTS " << std::endl;
		}
		if (msg_health_status.connect_AO1 == false)
		{
			std::cout << "[ERROR] Connection to Adam Analog Ouput Controller 1" << std::endl;
		}
		if (msg_health_status.connect_AO2 == false)
		{
			std::cout << "[ERROR] Connection to Adam Analog Ouput Controller 2" << std::endl;
		}
		if (msg_health_status.connect_RO1 == false)
		{
			std::cout << "[ERROR] Connection to Adam Relay Ouput Controller 1" << std::endl;
		}
		if (msg_health_status.connect_RO2 == false)
		{
			std::cout << "[ERROR] Connection to Adam Relay Ouput Controller 2" << std::endl;
		}
		if (msg_health_status.connect_RO3 == false)
		{
			std::cout << "[ERROR] Connection to Adam Relay Ouput Controller 3" << std::endl;
		}
	}

	//TODO::
	std::cout << " " << std::endl;
	//sptr_pcm_dbw_drive->DisplayPCM_CANData();
}

void BydFLDriveSteerForkThread::SetConfigParams(DBWCSCI_Config config, DBWEncodersConfig encoder_config, DBWPalletCommon::SmartMotorSteerConfig params_brake_config)
{
	config_params = config;
	encoder_config_params = encoder_config;
	params_brake_config_ = params_brake_config;

	//
	std::cout << "[BydFLDriveSteerForkThread - Steer Encoder Settings] " << std::endl;
	std::cout << " steer_home_enable = " << encoder_config_params.params_Steer_Encoder_Settings.steer_home_enable << std::endl;
	std::cout << " steer_encoder_home = " << encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_home << std::endl;
	std::cout << " steer_encoder_lpos = " << encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_lpos << std::endl;
	std::cout << " steer_encoder_rpos = " << encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_rpos << std::endl;
	std::cout << " max_steering = " << encoder_config_params.params_Steer_Encoder_Settings.max_steering << std::endl;

	//
	std::cout << "[BydFLDriveSteerForkThread - Line Encoder Settings] " << std::endl;
	std::cout << " line_home_enable = " << encoder_config_params.params_Line_Encoder_Settings.line_home_enable << std::endl;
	std::cout << " line_encoder_home = " << encoder_config_params.params_Line_Encoder_Settings.line_encoder_home << std::endl;
	std::cout << " line_encoder_min = " << encoder_config_params.params_Line_Encoder_Settings.line_encoder_min << std::endl;
	std::cout << " line_encoder_max = " << encoder_config_params.params_Line_Encoder_Settings.line_encoder_max << std::endl;

	//
	std::cout << "[BydFLDriveSteerForkThread - Tilt Encoder Settings] " << std::endl;
	std::cout << " tilt_home_enable = " << encoder_config_params.params_Tilt_Encoder_Settings.tilt_home_enable << std::endl;
	std::cout << " tilt_encoder_home = " << encoder_config_params.params_Tilt_Encoder_Settings.tilt_encoder_home << std::endl;
	std::cout << " tilt_encoder_min = " << encoder_config_params.params_Tilt_Encoder_Settings.tilt_encoder_min << std::endl;
	std::cout << " tilt_encoder_max = " << encoder_config_params.params_Tilt_Encoder_Settings.tilt_encoder_max << std::endl;

	//Open port
	sptr_PcmPalletController = std::make_shared<DBWPalletCommon::PCMPalletController>();

	//
	sptr_ADAM_ROController_DrvPedal = std::make_shared<DBWPalletCommon::Adam_ROController>();
	sptr_ADAM_AOController_DrvSteer_Voltage = std::make_shared<DBWPalletCommon::Adam_AOController>();
	sptr_ADAM_ROController_Gear = std::make_shared<DBWPalletCommon::Adam_ROController>();
	sptr_ADAM_AOController_Fork_Ctrl = std::make_shared<DBWPalletCommon::Adam_AOController>();
	sptr_ADAM_ROController_Fork_Ctrl = std::make_shared<DBWPalletCommon::Adam_ROController>();

	//
	sptr_SmartMotorSteer = std::make_shared<DBW_Ctrl>();
	//	sptr_DIODriver = std::make_shared<Platform::IsolatedDIO::IsolatedDIODriver>();

	//
	if (params_brake_config_.params_SmartMotor_Enabled.steer_enable == 1)
	{
		//First Initialization
		bool bSmartMotor 			= sptr_SmartMotorSteer->init_throttle_motor((char *)params_brake_config_.params_SmartMotor_Port.steer_motor_device.c_str());
		bool bSmartMotorCalibrated	= false;
		bSmartMotorCalibrated = (bSmartMotor) ? CalibrateBrake() : false ;
		if (bSmartMotor && bSmartMotorCalibrated)
		{
			//sptr_SmartMotorSteer->set_throttle_motor_origin();
			sptr_SmartMotorSteer->disconnect_throttle_motor();
			bSmartMotor = sptr_SmartMotorSteer->init_throttle_motor((char *)params_brake_config_.params_SmartMotor_Port.steer_motor_device.c_str()); // reinitialization of motor after calibration
		}

		if(bSmartMotor && bSmartMotorCalibrated)
		{
			msg_health_status.bConnectedSteerMotor = true;
		}
		else
		{
			msg_health_status.bConnectedSteerMotor = false;
		}
		std::cout << "======bSmartMotor 			=" << bSmartMotor << std::endl;
		std::cout << "======bSmartMotorCalibrated 	=" << bSmartMotorCalibrated << std::endl;
		std::cout << "======bConnectedSteerMotor 	=" << msg_health_status.bConnectedSteerMotor << "vs" << (bSmartMotor && bSmartMotorCalibrated) << std::endl;
	}

	//
	if (!sptr_PcmPalletController->OpenVecowCanPortModule())
	{
		std::cout << "[EMUC CAN]Cannot open can port !! " << std::endl;
		msg_health_status.pcm_feedback_healthstatus.open_emuc_can_port = false;
		//exit(1);
	}
	else
	{
		std::cout << "open can port" << std::endl;
		msg_health_status.pcm_feedback_healthstatus.open_emuc_can_port = true;
	}

	//open DIO port
	//	bool bchk = false;
	//	bchk = sptr_DIODriver->initDIO();
	//	if (bchk)
	//	{
	//		std::cout << "Success open Vecow DIO Port " << std::endl;
	//		msg_health_status.open_dio_port = true;
	//	}
	//	else
	//	{
	//		std::cout << "Fail to open Vecow DIO Port " << std::endl;
	//		msg_health_status.open_dio_port = false;
	//		//exit(1);
	//	}

	// Init Adam Controllers
	InitAdamController();

	// PID Stuff
	steer_max_deg = 85.253;//90.0; //90.0
	steer_encoder_resolution = 360.000/4096.000;//(steer_max_deg * 2) / (encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_rpos - encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_lpos); // degree per encoder pulse

	sptr_PcmPalletController->ResetLastCommandSpeed();
	if (config_params.debug.enabled == 1)
	{
		sptr_PcmPalletController->SetLogger(config_params.debug.log_folder);
	}
}

void BydFLDriveSteerForkThread::Thread_MainControl()
{
	//
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();


	/*****************************************/
	/*
	 * ADAM Controller Comms Check
	 */
	std::unique_lock<std::mutex> ao_lock(mutex_AOController);
	uint16_t typecode_value[4] = {0};
	float startup_value[4] = {0};
	float ao_value[4] = {0};
	uint8_t status[6] = {0};
	sptr_ADAM_ROController_DrvPedal->read_adam_6260_ro_status(status);
	sptr_ADAM_AOController_DrvSteer_Voltage->read_aotypecode(typecode_value);
	sptr_ADAM_AOController_DrvSteer_Voltage->read_startupvalue(startup_value);
	sptr_ADAM_AOController_DrvSteer_Voltage->read_aovalue(ao_value);
	sptr_ADAM_ROController_Fork_Ctrl->read_adam_6260_ro_status(status);
	sptr_ADAM_AOController_Fork_Ctrl->read_aotypecode(typecode_value);
	sptr_ADAM_AOController_Fork_Ctrl->read_startupvalue(startup_value);
	sptr_ADAM_AOController_Fork_Ctrl->read_aovalue(ao_value);
	sptr_ADAM_ROController_Gear->read_adam_6266_ro_status(status);
	ao_lock.unlock();

	/*****************************************/

	//Get current status and DBW commands
	GetDriveCmdsAndStatus();

//	std::cout << "plm system state = " << plm_status.system_state << std::endl;
	//Send command to drive, steer and fork controller
	bool stop_drive_motor = false;
	float cmd_speed_ = 0.0;
	float ch0_voltage = 0.0;
	float ch1_voltage = 0.0;
	//
	float cmd_brake_ = 0.0;
	float cmd_brake_voltage = 0.0;
	float final_volt;
	std::cout<<"msg_health_status.hw_estop_status = "<< msg_health_status.hw_estop_status <<std::endl;
	std::cout<<"msg_health_status.detected_lost_auxillary_msgs = "<< msg_health_status.detected_lost_auxillary_msgs <<std::endl;

	if ((msg_health_status.hw_estop_status == false) && (msg_health_status.detected_lost_auxillary_msgs == false))
	{
		if ((msg_health_status.receive_dbwcmd_msg_tele == true) || (msg_health_status.receive_dbwcmd_msg_vcm == true))
		{
			//
			bReset = false;

			if(msg_health_status.unmmaned_mode)
			{
				/*---------- decode speed start ----------*/
				if (fabs(dbw_cmd.cmd_speed) <= 0.0001)
				{
					palletbase_cmd.trigger_slowspeed = 1;
					//std::cout << "BRAKES ACTIVATED " << std::endl;
				}
				else
				{
					palletbase_cmd.trigger_slowspeed = 0;
					//std::cout << "BRAKES NOT ACTIVATED " << std::endl;
				}
				/*---------- decode speed end ----------*/
			}

			//-------------------------DS-------------------------------
			//Fixed to move after fast obs
			if (msg_health_status.receive_dbwcmd_msg_vcm && !msg_health_status.receive_dbwcmd_msg_tele
					&& plm_status.system_state == Platform::ePLM_Status::PLMStatus_Waypoint)
			{
				if(bSafetyLidar == true)
				{
					//std::cout << "AFTER ESTOP STATE ============ AFTER ESTOP STATE" << std::endl;
					dbw_cmd.cmd_steer = 0;
					dbw_cmd.cmd_speed = 0;
					sleep(1);
					bSafetyLidar = false;
				}
				else
				{
					DecodeVCM();
				}
			}

			//armrest to be able to use fork
			if(msg_health_status.receive_dbwcmd_msg_tele
					&& plm_status.system_state == Platform::ePLM_Status::PLMStatus_Manned)
			{
				//std::cout << "ENTER =================== ENTER " << std::endl;
				if(palletbase_cmd.trigger_tiller == 0x00)
				{
					//std::cout << "ENTER 1 =================== ENTER 1" << std::endl;
					stopForkControl();
				}
			}
//			else
//			{
//				bTransitFork = false;
//				bTransitTilt = false;
//			}
			//-----------------------DS--------------------------
			//
			SetCurrentCommands(dbw_cmd, palletbase_cmd);

			msg_health_status.current_drive_cmd = dbw_cmd;
			msg_health_status.current_palletbase_cmd = palletbase_cmd;
			//

			//
			cmd_speed_ = dbw_cmd.cmd_speed;
			cmd_brake_ = palletbase_cmd.trigger_slowspeed;

			//check if brake died
			if (msg_health_status.bConnectedSteerMotor == false)
			{
				cmd_speed_ = 0.0;
			}

//						std::cout <<"============================dbw_cmd.cmd_steer = " << dbw_cmd.cmd_steer << std::endl;
			//
			CommandVehSpeedChange(dbw_cmd);
			CmdSpeedToVotlage(cmd_speed_, ch0_voltage);

			//			//printing acc and brake voltage
//						std::cout << "cmd_speed = " << cmd_speed_ << " | ch0 voltage = " << ch0_voltage << std::endl;
//						std::cout << "cmd_brake = " << cmd_brake_ << std::endl;

			std::unique_lock<std::mutex> ao_lock(mutex_AOController);
			float ao_value[4] = {0};
			if (bActivateForkControl)
			{
				//sptr_ADAM_AOController1->write_aovalue(0, 4.15/*4.5*//*4.15*/);			// Activate Pump
				if (palletbase_cmd.trigger_tiller == 0x05) //raise fork
				{

					sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 1.3);

				}

				else if (palletbase_cmd.trigger_tiller == 0x03) //lower fork
				{
					sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 3.4);
				}

				else if (palletbase_cmd.trigger_tiller == 0x11) //fork tilt forward
				{
					sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(2, 3.4);
				}

				else if (palletbase_cmd.trigger_tiller == 0x21) // fork tilt backward
				{
					sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(2, 1.2);
				}

				//				else if(palletbase_cmd.trigger_tiller == 0x81) // fork move left
				//				{
				//					sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(1, 3.8);
				//				}
				//
				//				else if(palletbase_cmd.trigger_tiller == 0x41) // fork move right
				//				{
				//					sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(1, 1.3);
				//				}

				bActivateForkControl = false;
			}

			if (msg_health_status.receive_dbwcmd_msg_tele)
			{
				if (bActivateSteerControl)
				{
					if (bSteerRight)
					{
						std::cout << "steer right" << std::endl;
						bSteerRight = false;
						sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(2, -1.5); //set prop valve steer right voltage,6
					}
					else if (bSteerLeft)
					{
						std::cout << "steer left" << std::endl;
						bSteerLeft = false;
						sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(2, 1.5); //set prop valve steer left voltage,9
					}

					sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 3.5); //set pump voltage

					bActivateSteerControl = false;
				}
			}

			//			sptr_ADAM_AOController_DrvSteer_Voltage->read_aovalue(ao_value);
			uint8_t thisstatus[6] = {0};
			sptr_ADAM_ROController_Fork_Ctrl->read_adam_6260_ro_status(thisstatus);
			//
						printf( "[Channel 0] thisstatus RO  = %d\n" , thisstatus[0] );
						printf( "[Channel 1] thisstatus RO  = %d\n" , thisstatus[1] );
						printf( "[Channel 2] thisstatus RO  = %d\n" , thisstatus[2] );
						printf( "[Channel 3] thisstatus RO  = %d\n" , thisstatus[3] );
						printf( "[Channel 4] thisstatus RO  = %f\n" , thisstatus[4] );
						printf( "[Channel 5] thisstatus RO  = %f\n" , thisstatus[5] );

			if (previous_cmd_speed_ != cmd_speed_)
			{
				previous_cmd_speed_ = cmd_speed_;

				if (cmd_speed_ == 0)
				{
					sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(0, false);
					sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(2, false);
					sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(3, false);
					sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(4, false);
					sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(5, false);
				}
				else
				{
					sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(0, true);
					sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(2, true);
					sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(3, true);
					sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(4, true);
					sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(5, true);
				}
				sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(0, ch0_voltage);
			}
			//
			//			sptr_SmartMotorSteer->set_throttle_motor_pos(0,0);
			if (previous_cmd_brake_ != cmd_brake_)
			{
				std::cout << "enter the brake function" << std::endl;
				previous_cmd_brake_ = cmd_brake_;
				CmdBrake(cmd_brake_);
			}

			SetPIMCommands(dbw_cmd, palletbase_cmd, stop_drive_motor);

			ao_lock.unlock();

			//
			msg_health_status.send_stop_cmd_dbw = false;
		}
		else
		{
			stop_drive_motor = true;
		}
	} //if
	else
	{
		stop_drive_motor = true;
	}

	//message failure or EStop switch detected.
	if (stop_drive_motor)
	{
		//Reset commands
		dbw_cmd.cmd_steer = 0;
		dbw_cmd.cmd_speed = 0;
		SetPIMCommands(dbw_cmd, palletbase_cmd, stop_drive_motor);

		//--------------DS---------------
		bSafetyLidar = true;
		//std::cout << "ESTOP STATE ============ ESTOP STATE" << std::endl;
		//------------DS---------------

		//std::cout << "4. Stop Controls " << std::endl;
		if (!bReset)
		{
			std::cout << "brake" << std::endl;
			stopForkliftControl();
			TorqueBrake();

			// Stop Controls
			bForkControl = false;
			bSteerControl = false;
			bActivateForkControl = false;
			bActivateSteerControl = false;
			bDeActivateValveControl = false;
			prop_valve_voltage = 0;
			previous_cmd_speed_ = 0;
			previous_cmd_brake_ = 1;
			basecmd.trigger_tiller = 0x00;
			//
			bReset = true;
		}
		//
		msg_health_status.send_stop_cmd_dbw = true;
	} //if(stop_drive_motor)

	//std::cout << current_timestamp-previous_timestamp << " ms main loop thread time " << std::endl;
	previous_timestamp = current_timestamp;
	//
	//PrintDBWStatus();
}

//Get the current PLM mode
//Get the current DBW commands either from tele-op or VCM
//Update the set steer commands
//Update the PLM mode to steer class
void BydFLDriveSteerForkThread::Thread_ValveControl()
{
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();
	//
	Platform::Sensors::PalletDbwCmdMsg in_data_dbwcmd;
	Platform::Sensors::PalletBaseCmdMsg in_data_basecmd;
	in_data_dbwcmd = dbw_cmd_;
	in_data_basecmd = base_cmd_;

	//
	bool stop_drive_motor = false;
	if ((msg_health_status.hw_estop_status == false) && (msg_health_status.detected_lost_auxillary_msgs == false))
	{
		if ((msg_health_status.receive_dbwcmd_msg_tele == true) || (msg_health_status.receive_dbwcmd_msg_vcm == true))
		{
			stop_drive_motor = false;
		}
		else
		{
			stop_drive_motor = true;
		}
	}
	else
	{
		stop_drive_motor = true;
	}

	//
	if (stop_drive_motor)
	{
		in_data_basecmd.trigger_tiller = 0x00;
		//	std::cout << "[!! ERROR] Thread_ValveControl trigger_tiller is stop_drive_motor " << std::endl;
	}
	//printf("in_data_basecmd.trigger_tiller = 0x%X\n", in_data_basecmd.trigger_tiller);

	//
	bool bControlFlag = ((in_data_basecmd.trigger_tiller > 0x01)) ? true : false;
	if (!bControlFlag && !bRaiseFork && !bTiltFork)
	{
		stopControls();
	}
	else
	{
		bool bSteerFlag = ((in_data_basecmd.trigger_tiller == 0x09)) ? true : false;

		if (!bSteerFlag)
		{
			ForkControl(in_data_basecmd);
		}
		else
		{
			SteerControl(in_data_dbwcmd);
		}
	}

	//
	//std::cout << current_timestamp-previous_drive_timestamp << " ms valve control loop thread time " << std::endl;
	previous_drive_timestamp = current_timestamp;
}

void BydFLDriveSteerForkThread::Thread_ReadPCM()
{
	sptr_PcmPalletController->ReadPCM();
}
void BydFLDriveSteerForkThread::Thread_DecodeBattery()
{
	bool good = sptr_PcmPalletController->DecodePalletBatteryFbkMsg();
	if (good) cout << "[Succesfully decode battery message]" << endl;
}
void BydFLDriveSteerForkThread::Thread_DecodeSpeed()
{
	bool good = sptr_PcmPalletController->DecodePalletSpeedFbkMsg();
	if (good) cout << "[Successfully decode speed message]" << endl;
}
void BydFLDriveSteerForkThread::Thread_DecodeSpeedMode()
{
	bool good = sptr_PcmPalletController->DecodeModeMsg();
	if (good) cout << "[Successfully decode speed mode message]" << endl;
}

//TODO:: Get update from steer commands
//TODO:: Pub the pallet base and dbw feedbacks
void BydFLDriveSteerForkThread::Thread_FeedbackDBWandPalletBaseStatus()
{
	//Define stkci messages
	Platform::Sensors::PalletDbwFbkMsg out_stkci_dbw_fbk;		  //TBD speed feedback, encoders etc
	Platform::Sensors::PalletBaseFbkMsg out_stkci_basepallet_fbk; //TBD battery

	//TODO::Get feedback from drive,speed and steer DBW
	//IMPORTANT:: Missing feedback parameters to be determined

	//std::cout << "enter feedback data" << std::endl;

	std::unique_lock<std::mutex> can_lock(mutex_CANBus);

	sptr_PcmPalletController->GetPCMPalletFeedback(msg_health_status.pcm_feedback_healthstatus);
	sptr_PcmPalletController->GetBYDCANFeedback(pcm_feedback);
	//DBWPalletCommon::StkciPCMPalletBoundary::Convert_PalletPCMFbk_To_StkciDBWAndPalletBaseFbk(pcm_feedback,out_stkci_dbw_fbk,out_stkci_basepallet_fbk);

	msg_health_status.current_platform_battlevel = pcm_feedback.current_platform_batterylevel;
	msg_health_status.current_platform_speed = (pcm_feedback.current_platform_speed * 0.278);

	//TODO:: Get steer encoder feedback upon request
	uint32_t current_steer_enc_pos;
	if (sptr_PcmPalletController->GetSikoSteerEncoderFeedback(current_steer_enc_pos))
	{
		msg_health_status.pcm_feedback_healthstatus.rx_steer_encoder_fb = true;
		std::cout << "get steer encoder" << std::endl;
		steer_enc_pos = current_steer_enc_pos;
		std::cout << "current steer encoder position = " << steer_enc_pos << std::endl;
	}
	else
	{
		msg_health_status.pcm_feedback_healthstatus.rx_steer_encoder_fb = false;
	}

	out_stkci_basepallet_fbk.seq_key = 0;
	out_stkci_basepallet_fbk.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	out_stkci_basepallet_fbk.batterylevel = pcm_feedback.current_platform_batterylevel;
	if (pcm_feedback.current_platform_batterylevel > 100.0)
	{
		msg_health_status.exceeded_battlevel = true;
	}
	else
	{
		msg_health_status.exceeded_battlevel = false;
	}

	uint32_t current_line_enc_pos;
	if (sptr_PcmPalletController->GetBaumerLineEncoderFeedback(current_line_enc_pos))
	{
//		msg_health_status.pcm_feedback_healthstatus.rx_line_encoder_fb = true;
		line_enc_pos = current_line_enc_pos;
		current_height = (line_enc_pos / 100.0) - 6.2;								//in cm
		out_stkci_basepallet_fbk.fork_height_fb = (line_enc_pos / 10000.0) - 0.062; //in m
		std::cout << "current line encoder position = " << line_enc_pos << std::endl;
		std::cout << "current line encoder position in height(cm) = " << current_height << std::endl;
	}
	else
	{
//		msg_health_status.pcm_feedback_healthstatus.rx_line_encoder_fb = false;
		current_height = (line_enc_pos / 100.0) - 6.2;	//6.2;
		out_stkci_basepallet_fbk.fork_height_fb =(line_enc_pos / 10000.0) - 0.062;// 0.062;
	}

	msg_health_status.current_platform_forkheight = out_stkci_basepallet_fbk.fork_height_fb;
	out_stkci_basepallet_fbk.forkbase_height_left = last_desired_fork_height;
	//	std::cout << "current line encoder position in height(m) = " << out_stkci_basepallet_fbk.fork_height_fb << std::endl;
	if (!msg_health_status.unmmaned_mode)
	{
		msg_health_status.desired_platform_forkheight = msg_health_status.current_platform_forkheight;
	}
	if (out_stkci_basepallet_fbk.fork_height_fb > (out_stkci_basepallet_fbk.fork_height_fb + out_stkci_basepallet_fbk.fork_height_fb * 0.07))
	{
		msg_health_status.exceeded_forkheight = true;
	}
	else
	{
		msg_health_status.exceeded_forkheight = false;
	}

	// DIO Sensor Feedback and publish base feedback messages -CK
	Platform::Sensors::PalletBaseFbkMsg in_stkci_basepallet_fbk;
	if (sptr_RetrieveBaseFeedback_->GetPalletBaseFbkMsg(in_stkci_basepallet_fbk))
	{
		msg_health_status.error_rxpalletbasefb_count = 0;
		out_stkci_basepallet_fbk.engagement_sensor_left = in_stkci_basepallet_fbk.engagement_sensor_left;
		out_stkci_basepallet_fbk.engagement_sensor_right = in_stkci_basepallet_fbk.engagement_sensor_right;
	}
	else
	{
		msg_health_status.error_rxpalletbasefb_count++;
	}

	out_stkci_basepallet_fbk.reserved_two = 0x00;
	if (bForkHeight)
	{
		//	std::cout << "[STATUS ] bForkHeight is true " << std::endl;
	}
	else
	{
		//	std::cout << "[STATUS ] bForkHeight is false " << std::endl;
	}
	if (msg_health_status.receive_basecmd_msg)
	{
		//	std::cout << "[STATUS ] msg_health_status.receive_basecmd_msg is true " << std::endl;
	}
	else
	{
		//	std::cout << "[STATUS ] msg_health_status.receive_basecmd_msg is false " << std::endl;
	}

	if (bForkHeight && msg_health_status.receive_basecmd_msg)
	{
		out_stkci_basepallet_fbk.reserved_two = 0x01;

		if(palletbase_cmd.seq_key == 999)
		{
			bForkHeight = false;
		}
	}

	uint32_t current_tilt_enc_pos;
	if (sptr_PcmPalletController->GetBaumerTiltEncoderFeedback(current_tilt_enc_pos))
	{
//		msg_health_status.pcm_feedback_healthstatus.rx_tilt_encoder_fb = true;
		tilt_enc_pos = current_tilt_enc_pos;
		std::cout << "current tilt encoder position = " << tilt_enc_pos << std::endl;
		std::cout << "current tilt encoder position in angle = " << tilt_enc_pos / 10 << std::endl;
	}
	else
	{
//		msg_health_status.pcm_feedback_healthstatus.rx_tilt_encoder_fb = false;
	}

	out_stkci_basepallet_fbk.reserved_three = 0x00;

	if (bTiltAngle && msg_health_status.receive_basecmd_msg)
	{
		out_stkci_basepallet_fbk.reserved_three = 0x01;
		if(palletbase_cmd.seq_key == 999)
		{
			bTiltAngle = false;
		}
	}

	can_lock.unlock();

	//-------------------CPU Temperature-----------------------
	const char *n[] = {"/sys/class/hwmon/hwmon1/temp2_input",
					   "/sys/class/hwmon/hwmon1/temp3_input",
					   "/sys/class/hwmon/hwmon1/temp4_input",
					   "/sys/class/hwmon/hwmon1/temp5_input"};

	total = 0;
	for (int i = 0; i <= (CORES - 1); i++)
	{
		int ref = 0;

		if ((f = fopen(n[i], "r")))
		{

			ref = fscanf(f, "%lf", &value);
			if (ref != 1)
			{
				std::cout << "Error " << std::endl;
			}
			ref = fclose(f);

			value /= 1000;
			total += value;
		}
	}
	average = total / CORES;

	out_stkci_basepallet_fbk.cpu_temperature = average;
	//	std::cout << "Temperature ====================================== " << out_stkci_basepallet_fbk.cpu_temperature << std::endl;

	if (out_stkci_basepallet_fbk.cpu_temperature >= config_params.warning_temperature)
	{
		msg_health_status.temperature_warning = true;
	}
	else
	{
		msg_health_status.temperature_warning = false;
	}
	if (out_stkci_basepallet_fbk.cpu_temperature >= config_params.error_temperature)
	{
		msg_health_status.exceeded_temperature = true;
		msg_health_status.error_temperaturelimit_count++;
	}
	else
	{
		msg_health_status.exceeded_temperature = false;
		msg_health_status.error_temperaturelimit_count = 0;
	}
	//-------------------CPU Temperature-----------------------

	if (dbw_cmd.cmd_steer > 1.0)
	{
		msg_health_status.exceeded_steer = true;
	}
	else
	{
		msg_health_status.exceeded_steer = false;
	}

	if (dbw_cmd.cmd_speed > (dbw_cmd.cmd_speed + (dbw_cmd.cmd_speed * 0.25)))
	{
		msg_health_status.exceeded_speed = true;
	}
	else
	{
		msg_health_status.exceeded_speed = false;
	}

	//	if(msg_health_status.current_platform_speed > (msg_health_status.current_platform_speed*1.30))
	//	{
	//		msg_health_status.exceeded_speed=true;
	//	}
	//	else
	//	{
	//		msg_health_status.exceeded_speed=false;
	//	}

	//format the drive by wire feedback.
	out_stkci_dbw_fbk.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	out_stkci_dbw_fbk.seq_key = 0;
	out_stkci_dbw_fbk.speed = (pcm_feedback.current_platform_speed * 0.278);
	std::cout << "Speed ===================    " << out_stkci_dbw_fbk.speed << std::endl;
	//	out_stkci_dbw_fbk.steer = steer_seiko_enc_fb_angle_radians;
	out_stkci_dbw_fbk.left_drive_encoder_count = 0;
	out_stkci_dbw_fbk.right_drive_encoder_count = 0;
	out_stkci_dbw_fbk.steer_encoder_count = steer_enc_pos;
	out_stkci_dbw_fbk.drive_motor_status = 0;
	out_stkci_dbw_fbk.steer_motor_status = 0;
	out_stkci_dbw_fbk.drive_motor_temp = 0.0;
	out_stkci_dbw_fbk.steer_motor_temp = 0.0;

	//Get GPS vehicle measure
	sptr_RetrieveVehMeasure_->GetGeometryMsgsVehMeasureMsg(gps_vehmeas_data);

	//format platform vehicle measure
	platform_vehmeas.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	platform_vehmeas.seq_key = 0;
	//TODO::
	platform_vehmeas.speed = (pcm_feedback.current_platform_speed * 0.278);
	//	platform_vehmeas.steering = steer_seiko_enc_fb_angle_radians;
	platform_vehmeas.yaw_rate = gps_vehmeas_data.yaw_rate; //sbg gps yaw rate
	ptr_stkci_pub->PubVehicleMeasure("VehicleMeasure", platform_vehmeas);

	//Publish feedback messages
	ptr_stkci_pub->PubPalletBaseFbk("DBW", out_stkci_basepallet_fbk);
	ptr_stkci_pub->PubPalletDbwFbk("DBW", out_stkci_dbw_fbk);
	//std::cout << "PubPalletDbwFbk " << std::endl;

	//std::cout << "3 Thread FeedbackDBWandPalletBaseStatus time (s) = " << (Common::Utility::Time::getusecCountSinceEpoch()-lasttime_thread_base_pallet_status)/1000000.0 << std::endl;
	lasttime_thread_base_pallet_status = Common::Utility::Time::getusecCountSinceEpoch();
}

//Handle incoming message status and safety mechanism check
void BydFLDriveSteerForkThread::GetDriveCmdsAndStatus()
{
	//Get PLM status
	Platform::Sensors::SystemMsgsUGVPlatformStatus plm_status_local;
	if (sptr_RetrievePLMStatus_->GetPLMDataMsg(plm_status_local))
	{
		plm_status = plm_status_local;
		msg_health_status.receive_plmstatus_msg = true;
		msg_health_status.error_plmstatus_count = 0;
	}
	else
	{
		msg_health_status.receive_plmstatus_msg = false;
		msg_health_status.error_plmstatus_count++;
	}

	//Gt PLM Error Status
	Platform::Sensors::SystemMsgsUGVPlatformStatus plm_error_status_local;
	if (sptr_RetrievePLMErrorStatus_->GetPLMDataMsg(plm_error_status_local))
	{
		plm_error_status = plm_error_status_local;
		msg_health_status.receive_plmerrorstatus_msg = true;
		msg_health_status.error_plmerrorstatus_count = 0;
	}
	else
	{
		msg_health_status.receive_plmerrorstatus_msg = false;
		msg_health_status.error_plmerrorstatus_count++;
	}
//	std::cout << "plm error = " << plm_error_status.bCSCI_Error << std::endl;
//	std::cout << "plm alive = " << plm_error_status.bCSCI_Alive << std::endl;



	//-----------------------------------------------------------------------
	//Get VIM status
	Platform::Sensors::PalletAuxFbkMsg vim_status_local;
	if (sptr_RetreiveVIMStatus_->GetVIMDataMsg(vim_status_local))
	{
		vim_status = vim_status_local;
		msg_health_status.receive_vimstatus_msg = true;
		msg_health_status.error_vimstatus_count = 0;
	}
	else
	{
		msg_health_status.receive_vimstatus_msg = false;
		msg_health_status.error_vimstatus_count++;
	}

	//Determine DBW commands from the correct source
	if (vim_status.manned_unmanned_switch == Platform::PIM::SWITCH_MANNED) //tele-opes
	{
		msg_health_status.unmmaned_mode = false;
	}
	if (vim_status.manned_unmanned_switch == Platform::PIM::SWITCH_UNMANNED) //autonomous
	{
		msg_health_status.unmmaned_mode = true;
	}
	//reset last command speed upon unmanned mode change
	if (msg_health_status.unmmaned_mode != msg_health_status.last_unmmaned_mode)
	{
		msg_health_status.last_unmmaned_mode = msg_health_status.unmmaned_mode;
		//sptr_pcm_dbw_drive->ResetLastCommandSpeed();
	}

	//Check hardware ESTOP status
	if (vim_status.estop_status == Platform::PIM::SWITCH_ESTOP_CLEAR)
	{
		msg_health_status.hw_estop_status = false;
	}
	if (vim_status.estop_status == Platform::PIM::SWITCH_ESTOP_ACTIVATE)
	{
		msg_health_status.hw_estop_status = true;
	}

	//Check IES solenoid status
	if (vim_status.solenoid_status == Platform::PIM::SWITCH_SOLENOID_ACTIVATE)
	{
		msg_health_status.solenoid_status = true;
	}
	if (vim_status.solenoid_status == Platform::PIM::SWITCH_SOLENOID_DEACTIVATE)
	{
		msg_health_status.solenoid_status = false;
	}

	//Check IES piston status
	if (vim_status.piston_status == Platform::PIM::SWITCH_PISTON_ACTIVATE)
	{
		msg_health_status.piston_status = true;
	}
	if (vim_status.piston_status == Platform::PIM::SWITCH_PISTON_DEACTIVATE)
	{
		msg_health_status.piston_status = false;
	}

	//Critical error when failed to receive PLM and VIM messages
	msg_health_status.detected_lost_auxillary_msgs = false;
	if (msg_health_status.receive_plmstatus_msg == false)
	{
		msg_health_status.detected_lost_auxillary_msgs = true;
		std::cout<<"Test 1"<<std::endl;

	}
	if (msg_health_status.receive_vimstatus_msg == false)
	{
		msg_health_status.detected_lost_auxillary_msgs = true;
		std::cout<<"Test 2"<<std::endl;

	}
	if (msg_health_status.receive_plmerrorstatus_msg == false)
	{
		msg_health_status.detected_lost_auxillary_msgs = true;
		std::cout<<"Test 3"<<std::endl;

	}
	if(msg_health_status.unmmaned_mode)
	{
		if (plm_error_status.bCSCI_Alive)
		{
			msg_health_status.detected_lost_auxillary_msgs = true;
			std::cout<<"Test 4"<<std::endl;

		}
		if (plm_error_status.bCSCI_Error)
		{
			msg_health_status.detected_lost_auxillary_msgs = true;
			std::cout<<"Test 5"<<std::endl;

		}
	}

	//un-comment when IES is ready
	//Critical error when did not receive IES signal during autonomous
	// if (msg_health_status.unmmaned_mode)
	// {
	// 	if (msg_health_status.piston_status==false)
	// 	{
	// 		msg_health_status.detected_lost_auxillary_msgs = true;
	// 	}
	// 	if(msg_health_status.solenoid_status==false)
	// 	{
	// 		msg_health_status.detected_lost_auxillary_msgs = true;
	// 	}
	// }

	// check brake motor aliveness
	msg_health_status.bConnectedSteerMotor = sptr_SmartMotorSteer->get_brake_motor_aliveness();

	//-----------------------------------------------------------------------
	//Get DBW commands from Teleops or VCM
	Platform::Sensors::PalletDbwCmdMsg dbw_cmd_teleop_local;
	Platform::Sensors::PalletDbwCmdMsg dbw_cmd_vcm_local;
	if (msg_health_status.unmmaned_mode == false)
	{
		transitionManned = true;
		//		std::cout << "---------------- In Manned Mode !!! -------------------" << std::endl;
		if (sptr_RetreiveDBWCmdsTeleOps_->GetDBWCmdMsg(dbw_cmd_teleop_local))
		{
			msg_health_status.receive_dbwcmd_msg_tele = true;
			dbw_cmd = dbw_cmd_teleop_local;
			msg_health_status.error_rxdrvcmd_tele_count = 0;
		}
		else
		{
			msg_health_status.receive_dbwcmd_msg_tele = false;
			msg_health_status.error_rxdrvcmd_tele_count++;
		}
		//
		msg_health_status.error_rxdrvcmd_vcm_count = 0;
		msg_health_status.receive_dbwcmd_msg_vcm = false;
	}
	else
	{
		if (transitionManned)
		{
			transitionManned = false;
			if (sptr_RetreiveDBWCmdsVCM_->GetDBWCmdMsg(dbw_cmd_vcm_local))
			{
				msg_health_status.receive_dbwcmd_msg_vcm = true;
				dbw_cmd = dbw_cmd_vcm_local;
				msg_health_status.error_rxdrvcmd_vcm_count = 0;
			}
		}

		//		std::cout << "----------------  In UNManned Mode !!! -------------------" << std::endl;
		if (sptr_RetreiveDBWCmdsVCM_->GetDBWCmdMsg(dbw_cmd_vcm_local))
		//						&& (plm_status.system_state==Platform::ePLM_Status::PLMStatus_Waypoint))
		{
			msg_health_status.receive_dbwcmd_msg_vcm = true;
			dbw_cmd = dbw_cmd_vcm_local;
			msg_health_status.error_rxdrvcmd_vcm_count = 0;
		}
		else
		{
			if (!sptr_RetreiveDBWCmdsVCM_->GetDBWCmdMsg(dbw_cmd_vcm_local))
			{
				msg_health_status.receive_dbwcmd_msg_vcm = false;
			}
			msg_health_status.error_rxdrvcmd_vcm_count++;
		}

		//when veh is in idle mode
		if (plm_status.system_state == Platform::ePLM_Status::PLMStatus_Idle)
		{
			dbw_cmd.cmd_speed = 0.0;
		}

		//
		msg_health_status.error_rxdrvcmd_tele_count = 0;
		msg_health_status.receive_dbwcmd_msg_tele = false;
	}

	//-----------------------------------------------------------------------
	//Get pallet base commands  last_desired_fork_height
	//To drive always tiller must be set to high
	//Tiller will be set to zero automatically when speed is zero
	if (sptr_RetreiveBaseCmds_->GetBaseCmdMsg(palletbase_cmd))
	{
		std::cout<<"==================================RECIEVED"<<std::endl;
		msg_health_status.receive_basecmd_msg = true;
		msg_health_status.error_rxpalletbasecmd_count = 0;
	}
	else
	{
		std::cout<<"===================================FAIL"<<std::endl;
		msg_health_status.receive_basecmd_msg = false;
		bTiltAngle = false;
		bForkHeight = false;
		msg_health_status.error_rxpalletbasecmd_count++;
	}
}

void BydFLDriveSteerForkThread::InitAdamController()
{
	uint16_t typecode_value[4] = {0};
	float startup_value[4] = {0};
	float ao_value[4] = {0};
	uint8_t status[6] = {0};
	//
	//
	bool bSuccess_connect_DrvPedal = sptr_ADAM_ROController_DrvPedal->connect("10.0.0.2", 502);
	bool bSuccess_connect_DrvSteer_volt = sptr_ADAM_AOController_DrvSteer_Voltage->connect("10.0.0.3", 502);
	bool bSuccess_connect_R0_Fork_Ctrl = sptr_ADAM_ROController_Fork_Ctrl->connect("10.0.0.4", 502);
	bool bSuccess_connect_AO_Fork_Ctrl = sptr_ADAM_AOController_Fork_Ctrl->connect("10.0.0.5", 502);
	bool bSuccess_connect_Gear = sptr_ADAM_ROController_Gear->connect("10.0.0.6", 502);

	//
	msg_health_status.connect_AO1 = bSuccess_connect_AO_Fork_Ctrl;
	msg_health_status.connect_AO2 = bSuccess_connect_DrvSteer_volt;
	msg_health_status.connect_RO1 = bSuccess_connect_R0_Fork_Ctrl;
	msg_health_status.connect_RO2 = bSuccess_connect_DrvPedal;
	msg_health_status.connect_RO3 = bSuccess_connect_Gear;

	if (bSuccess_connect_DrvPedal)
	{
		std::cout << "\nConnection 10.0.0.2 Successful" << std::endl;
	}
	else
	{
		std::cout << "Connection 10.0.0.2 Failed\n"
				  << std::endl;
	}
	if (bSuccess_connect_DrvPedal)
	{
		sptr_ADAM_ROController_DrvPedal->read_adam_6260_ro_status(status);
	}

	printf("---------- Printing 10.0.0.2 Status ----------\n");

	printf("[Channel 0] Relay Output = 0x%X\n", status[0]);
	printf("[Channel 1] Relay Output = 0x%X\n", status[1]);
	printf("[Channel 2] Relay Output = 0x%X\n", status[2]);
	printf("[Channel 3] Relay Output = 0x%X\n", status[3]);
	printf("[Channel 4] Relay Output = 0x%X\n", status[4]);
	printf("[Channel 5] Relay Output = 0x%X\n", status[5]);

	printf("---------- Printing 10.0.0.2 Status ----------\n");

	//
	if (bSuccess_connect_DrvSteer_volt)
	{
		std::cout << "\nConnection 10.0.0.3 Successful" << std::endl;
	}
	else
	{
		std::cout << "Connection 10.0.0.3 Failed\n"
				  << std::endl;
	}
	if (bSuccess_connect_DrvSteer_volt)
	{
		sptr_ADAM_AOController_DrvSteer_Voltage->read_aotypecode(typecode_value);
		sptr_ADAM_AOController_DrvSteer_Voltage->read_startupvalue(startup_value);
		sptr_ADAM_AOController_DrvSteer_Voltage->read_aovalue(ao_value);
	}

	printf("---------- Printing 10.0.0.3 Status ----------\n");

	printf("[Channel 0] Type Code = 0x%X\n", typecode_value[0]);
	printf("[Channel 1] Type Code = 0x%X\n", typecode_value[1]);
	printf("[Channel 2] Type Code = 0x%X\n", typecode_value[2]);
	printf("[Channel 3] Type Code = 0x%X\n", typecode_value[3]);

	printf("[Channel 0] Startup Value = %f\n", startup_value[0]);
	printf("[Channel 1] Startup Value = %f\n", startup_value[1]);
	printf("[Channel 2] Startup Value = %f\n", startup_value[2]);
	printf("[Channel 3] Startup Value = %f\n", startup_value[3]);

	printf("[Channel 0] AO Voltage = %f\n", ao_value[0]);
	printf("[Channel 1] AO Voltage = %f\n", ao_value[1]);
	printf("[Channel 2] AO Voltage = %f\n", ao_value[2]);
	printf("[Channel 3] AO Voltage = %f\n", ao_value[3]);

	printf("---------- Printing 10.0.0.3 Status ----------\n");

	//Open Adam controller
	if (bSuccess_connect_R0_Fork_Ctrl)
	{
		std::cout << "\nConnection 10.0.0.4 Successful" << std::endl;
	}
	else
	{
		std::cout << "Connection 10.0.0.4 Failed\n"
				  << std::endl;
	}
	if (bSuccess_connect_R0_Fork_Ctrl)
	{
		sptr_ADAM_ROController_Fork_Ctrl->read_adam_6260_ro_status(status);
	}

	printf("---------- Printing 10.0.0.4 Status ----------\n");

	printf("[Channel 0] Relay Output = 0x%X\n", status[0]);
	printf("[Channel 1] Relay Output = 0x%X\n", status[1]);
	printf("[Channel 2] Relay Output = 0x%X\n", status[2]);
	printf("[Channel 3] Relay Output = 0x%X\n", status[3]);
	printf("[Channel 4] Relay Output = 0x%X\n", status[4]);
	printf("[Channel 5] Relay Output = 0x%X\n", status[5]);

	printf("---------- Printing 10.0.0.4 Status ----------\n");

	//
	if (bSuccess_connect_AO_Fork_Ctrl)
	{
		std::cout << "\nConnection 10.0.0.5 Successful" << std::endl;
	}
	else
	{
		std::cout << "Connection 10.0.0.5 Failed\n"
				  << std::endl;
	}
	if (bSuccess_connect_AO_Fork_Ctrl)
	{
		sptr_ADAM_AOController_Fork_Ctrl->read_aotypecode(typecode_value);
		sptr_ADAM_AOController_Fork_Ctrl->read_startupvalue(startup_value);
		sptr_ADAM_AOController_Fork_Ctrl->read_aovalue(ao_value);
	}

	printf("---------- Printing 10.0.0.5 Status ----------\n");

	printf("[Channel 0] Type Code = 0x%X\n", typecode_value[0]);
	printf("[Channel 1] Type Code = 0x%X\n", typecode_value[1]);
	printf("[Channel 2] Type Code = 0x%X\n", typecode_value[2]);
	printf("[Channel 3] Type Code = 0x%X\n", typecode_value[3]);

	printf("[Channel 0] Startup Value = %f\n", startup_value[0]);
	printf("[Channel 1] Startup Value = %f\n", startup_value[1]);
	printf("[Channel 2] Startup Value = %f\n", startup_value[2]);
	printf("[Channel 3] Startup Value = %f\n", startup_value[3]);

	printf("[Channel 0] AO Voltage = %f\n", ao_value[0]);
	printf("[Channel 1] AO Voltage = %f\n", ao_value[1]);
	printf("[Channel 2] AO Voltage = %f\n", ao_value[2]);
	printf("[Channel 3] AO Voltage = %f\n", ao_value[3]);

	printf("---------- Printing 10.0.0.5 Status ----------\n");
	//

	//
	if (bSuccess_connect_Gear)
	{
		std::cout << "\nConnection 10.0.0.6 Successful" << std::endl;
	}
	else
	{
		std::cout << "Connection 10.0.0.6 Failed\n"
				  << std::endl;
	}
	if (bSuccess_connect_Gear)
	{
		sptr_ADAM_ROController_Gear->read_adam_6266_ro_status(status);
	}

	printf("---------- Printing 10.0.0.6 Status ----------\n");

	printf("[Channel 0] Relay Output = 0x%X\n", status[0]);
	printf("[Channel 1] Relay Output = 0x%X\n", status[1]);
	printf("[Channel 2] Relay Output = 0x%X\n", status[2]);
	printf("[Channel 3] Relay Output = 0x%X\n", status[3]);

	printf("---------- Printing 10.0.0.6 Status ----------\n");
}

//void BydFLDriveSteerForkThread::CheckHome()
//{
//	// reset speed and steer
//	dbw_cmd.cmd_speed = 0;
//	dbw_cmd.cmd_steer = 0;
//	palletbase_cmd.trigger_tiller = 0x00;
//
//	//
//	uint32_t current_steer_enc_pos_ = steer_enc_pos;
//	uint32_t current_line_enc_pos_ = line_enc_pos;
//
//	uint32_t home_steer_enc_pos = encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_home;
//	uint32_t lpos_steer_enc_pos = encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_lpos;
//	uint32_t rpos_steer_enc_pos = encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_rpos;
//	uint32_t left_difference = abs(home_steer_enc_pos - lpos_steer_enc_pos);
//	uint32_t right_difference = abs(home_steer_enc_pos - rpos_steer_enc_pos);
//
//	float tolerance = 0.05; //0.05
//
//	if (!bHomeSteer && encoder_config_params.params_Steer_Encoder_Settings.steer_home_enable == 1)
//	{
//		// check current position of steer encoder
//		if (current_steer_enc_pos_ > (home_steer_enc_pos - (left_difference * tolerance)) &&
//				current_steer_enc_pos_ < (home_steer_enc_pos + (right_difference * tolerance)))
//		{
//			bHomeSteer = true;
//			palletbase_cmd.trigger_tiller = 0x00;
//			dbw_cmd.cmd_steer = 0;
//		}
//		else
//		{
//			bHomeSteer = false;
//			palletbase_cmd.trigger_tiller = 0x09;							// activate only steer control
//
//			// dbw_cmd.cmd_steer > 0 left turn
//			// dbw_cmd.cmd_steer < 0 right turn
//
//			if (current_steer_enc_pos_ < home_steer_enc_pos)				// wheel steered left
//			{
//				// steer right to home
//				dbw_cmd.cmd_steer = -1; //1
//
//				std::cout << "steering right" << std::endl;
//			}
//			else if (current_steer_enc_pos_ > home_steer_enc_pos)			// wheel steered right
//			{
//				// steer left to home
//				dbw_cmd.cmd_steer = 1; //1
//
//				std::cout << "steering left" << std::endl;
//			}
//		}
//	}
//	else
//	{
//		palletbase_cmd.trigger_tiller = 0x00;
//		bHomeSteer = true;
//	}
//
//
//	uint32_t home_line_enc_pos = encoder_config_params.params_Line_Encoder_Settings.line_encoder_home;
//	uint32_t min_line_enc_pos = encoder_config_params.params_Line_Encoder_Settings.line_encoder_min;
//	uint32_t max_line_enc_pos = encoder_config_params.params_Line_Encoder_Settings.line_encoder_max;
//
//	float raise_line_tolerance = 0.8/*0.8*/;
//	float lower_line_tolerance = 1; //4,1
//
//	uint32_t interval = 204; 		//change in encoder per 1cm change in fork height
//	uint32_t home_height =  4.5; 	//home height for fork in cm
//
//	if (!bHomeSteer)
//	{
//		bHomeFork = false;
//	}
//	else
//	{
//		if (!bHomeFork && encoder_config_params.params_Line_Encoder_Settings.line_home_enable == 1)
//		{
//			uint32_t desired_fork_height = last_desired_fork_height * 100; // desired_fork_height in cm, palletbase_cmd.fork_height in m
//			uint32_t desired_line_enc_pos = (desired_fork_height + 17.3) * 100.0;
//
//			// check current position of line encoder
//			uint32_t linetolerance = desired_line_enc_pos*0.05;
//			if (current_line_enc_pos_ > (desired_line_enc_pos - linetolerance) &&
//					current_line_enc_pos_ < (desired_line_enc_pos + linetolerance))
//
//			{
//				palletbase_cmd.trigger_tiller = 0x00;
//				bHomeFork = true;
//			}
//			else
//			{
//				bHomeFork = false;
//				if ( current_line_enc_pos_ <  desired_line_enc_pos)
//				{
//					palletbase_cmd.trigger_tiller = 0x05;
//
//					std::cout << "raising fork" << std::endl;
//				}
//				else if ( current_line_enc_pos_ >  desired_line_enc_pos)
//				{
//					palletbase_cmd.trigger_tiller = 0x03;
//
//					std::cout << "lowering fork" << std::endl;
//				}
//			}
//		}
//		else
//		{
//			palletbase_cmd.trigger_tiller = 0x00;
//			bHomeFork = true;
//		}
//	}
//
//	if (bHomeSteer && bHomeFork)
//	{
//		bHomeInit = true;
//	}
//
//	//
//	if(bHomeSteer)
//	{
//		bHomeInit = true;
//	}
//}

void BydFLDriveSteerForkThread::SetPIMCommands(const Platform::Sensors::PalletDbwCmdMsg &dbw_cmd_, const Platform::Sensors::PalletBaseCmdMsg &palletbase_cmd_, bool bError)
{
	Platform::Sensors::PalletAuxCmdMsg aux_cmd;
	aux_cmd.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	static int lightCounter = 0;
	static bool bLight = false;
	std::cout << "send pim command" << std::endl;

	aux_cmd.trigger_left_direction_light = 0x00;
	aux_cmd.trigger_right_direction_light = 0x00;
	aux_cmd.trigger_buzzer = 0x00;

	if(bError)
	{
		aux_cmd.trigger_left_direction_light = 0x01;
		aux_cmd.trigger_right_direction_light = 0x01;
	}
	else if (dbw_cmd.cmd_steer == 0)
	{
		aux_cmd.trigger_left_direction_light = 0x00;
		aux_cmd.trigger_right_direction_light = 0x00;
	}
	else if (dbw_cmd.cmd_steer > 0)
	{
		aux_cmd.trigger_left_direction_light = 0x01;
		aux_cmd.trigger_right_direction_light = 0x00;
	}
	else if (dbw_cmd.cmd_steer < 0)
	{
		aux_cmd.trigger_left_direction_light = 0x00;
		aux_cmd.trigger_right_direction_light = 0x01;
	}

	if(dbw_cmd.cmd_speed > 0 || dbw_cmd.cmd_speed < 0)
	{
		aux_cmd.trigger_buzzer = 0x01;
	}

	if(plm_status.system_state==Platform::ePLM_Status::PLMStatus_Manned)
	{aux_cmd.trigger_buzzer = 0x02;}
	else if (plm_status.system_state==Platform::ePLM_Status::PLMStatus_Waypoint)
	{aux_cmd.trigger_buzzer = 0x00;}
	else if (plm_status.system_state==Platform::ePLM_Status::PLMStatus_Idle && 1)//NoError))
	{aux_cmd.trigger_buzzer = 0x00;}
	else
	{
		aux_cmd.trigger_buzzer = 0x01;
		aux_cmd.trigger_left_direction_light = 0x01;
		aux_cmd.trigger_right_direction_light = 0x01;
	}

//	if (dbw_cmd.cmd_speed < 0)
//	{
//		aux_cmd.trigger_left_direction_light = 0x01;
//		aux_cmd.trigger_right_direction_light = 0x01;
//	}

	ptr_stkci_pub->PubPalletAuxCmd("DBW", aux_cmd);
}

void BydFLDriveSteerForkThread::stopControls()
{
	if (bForkControl && !bSteerControl) // Previous Control was Fork
	{
		printf("1a. Deactivating Pump and Closing Fork Valves\n");

		basecmd.trigger_tiller = 0x00;
		bDeActivateValveControl = true;
		stopForkControl(); // Close Fork Valves

		//
		bForkControl = false;
	}
	else if (!bForkControl && bSteerControl) // Previous Control was Steer
	{
		printf("1b. Deactivating Pump and Closing Steer Valves\n");

		basecmd.trigger_tiller = 0x00;
		bDeActivateValveControl = true;
		volt_test = 0.0;
		stopSteerControl(); // Close Steer Valves

		//
		bSteerControl = false;
	}
	else if (!bForkControl && !bSteerControl) // No Previous Control
	{
		//printf("1c. Keeping Valves Closed\n");

		basecmd.trigger_tiller = 0x00;
		//		stopForkControl();												// Close Fork Valves
		//		stopSteerControl();												// Close Steer Valves

		//sptr_PcmPalletController->CloseAllValves();						// Close all valves
	}
}

void BydFLDriveSteerForkThread::SteerControl(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd)
{
	if (bForkControl)
	{
		printf("3a. Deactivating Pump and Closing Fork Valves, Transiting\n");

		basecmd.trigger_tiller = 0x00;

		// Stop Fork Control
		bDeActivateValveControl = true;
		stopForkControl();

		//
		bForkControl = false;
	}
	else if (!bSteerControl)
	{
		// dbw_cmd.cmd_steer > 0 left turn
		// dbw_cmd.cmd_steer < 0 right turn

		float previous_cmd_steer = data_dbwcmd.cmd_steer;
		previous_cmd_steer_ = previous_cmd_steer;
		std::cout << "previous cmd steer = " << previous_cmd_steer_ << std::endl;
		if (previous_cmd_steer_ < 0) // steer right
		{
			bSteerRight = true;
			bSteerLeft = false;
			//			prop_valve_voltage = 5.0f; //5.0f
		}
		else if (previous_cmd_steer_ > 0)
		{
			bSteerLeft = true;
			bSteerRight = false;
			//			prop_valve_voltage = -5.0f; //-5.0f									// steer left
		}

		//printf("steer = %f\n", data_dbwcmd.cmd_steer);
		//printf("voltage = %f\n", prop_valve_voltage);
		if (fabs(previous_cmd_steer) > 0)
		{
			printf("3b1. Opening Steer Valves and Activating Pump\n");

			startSteerControl();
			bActivateSteerControl = true;
		}
		else
		{
			printf("3b2. Keeping Steer Valves Closed\n");

			bDeActivateValveControl = true;
			stopSteerControl();
			//sptr_PcmPalletController->CloseAllValves();					// Close all valves
		}

		//
		bSteerControl = true;
	}
	else
	{
		bool bChk;
		float previous_cmd_steer = data_dbwcmd.cmd_steer; //current steer
		//
		std::cout << "previous cmd steer1 = " << previous_cmd_steer_ << std::endl;
		std::cout << "previous cmd steer2 = " << previous_cmd_steer << std::endl;
		bChk = (previous_cmd_steer_ == previous_cmd_steer) ? true : false;
		if (bChk)
		{
			if (fabs(previous_cmd_steer) > 0)
			{
				printf("3c1. Keeping Steer Valves Opened\n");

				startSteerControl();

				//sptr_PcmPalletController->OpenSteerPropValve();			// Open Steer prop valve
			}
			else
			{
				printf("3c2. Keeping Steer Valves Closed\n");

				stopSteerControl();

				//sptr_PcmPalletController->CloseAllValves();				// Close all valves
			}
		}
		else
		{
			if ((previous_cmd_steer_ < 0 && previous_cmd_steer < 0) || (previous_cmd_steer_ > 0 && previous_cmd_steer > 0))
			{
				printf("3d1. Keeping Steer Valves Opened\n");

				startSteerControl();
				//				sptr_PcmPalletController->OpenSteerPropValve();			// Open Steer prop valve
			}
			else
			{
				printf("3d2. Deactivating Pump, Transiting\n");

				//				prop_valve_voltage = 0;

				// Stop Steer Control
				bDeActivateValveControl = true;
				stopSteerControl();

				//
				bSteerControl = false;
			}
		}

		//		printf("steer = %f\n", data_dbwcmd.cmd_steer);
		//		printf("voltage = %f\n", prop_valve_voltage);
	}
}

void BydFLDriveSteerForkThread::startSteerControl()
{

	std::unique_lock<std::mutex> ao_lock(mutex_AOController);

	//	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(0, true); //on main valve
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(1, true); //on prop valve

	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, true);

	bActiveGear = true;

	ao_lock.unlock();

	// Start Steer Control
	//	for(int32_t i=0; i<5; i++)//250ms
	//	{
	//		sptr_PcmPalletController->OpenSteerPropValve();					// Open Steer prop valve
	//		usleep(50000); //50ms requirements
	//	}
}

void BydFLDriveSteerForkThread::stopSteerControl()
{
	std::unique_lock<std::mutex> ao_lock(mutex_AOController);

	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 2.5);			 //set pump voltage
	sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(1, 0.0);		 //set prop valve steer left voltage
	sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(2, 0.0);		 //set prop valve steer right voltage,7.5
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(1, false); //off prop valve
	//sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(0, false); //off main valve
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, false);

	bActiveGear = false;

	ao_lock.unlock();

	//	// Stop Steer Control
	//	for(int32_t i=0; i<5; i++)//250ms
	//	{
	//		sptr_PcmPalletController->CloseAllValves();						// Close all valves
	//		usleep(50000); //50ms requirements
	//	}
}

void BydFLDriveSteerForkThread::ForkControl(Platform::Sensors::PalletBaseCmdMsg data_basecmd)
{
	std::cout << "Fork commands ========================= " << bForkControl << std::endl;

	if (bSteerControl)
	{
		printf("2a. Deactivating Pump and Closing Steer Valves, Transiting\n");

		basecmd.trigger_tiller = 0x00;

		bDeActivateValveControl = true;
		stopSteerControl();

		//
		bSteerControl = false;
	}
	else if (!bForkControl)
	{
		printf("2b. Opening Fork Valves and Activating Pump\n");

		basecmd.trigger_tiller = data_basecmd.trigger_tiller;

		startForkControl(data_basecmd);

		bActivateForkControl = true;
		std::cout << "else if !bForkControl xxxxxxxxxxxx@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

		//
		bForkControl = true;
	}
	else
	{
		if (basecmd.trigger_tiller == data_basecmd.trigger_tiller)
		{
			printf("2c. Keeping Fork Valves Opened\n");

			//sptr_PcmPalletController->SetForkliftForkCmds(data_basecmd);// Open valve + movement
		}
		else
		{
			printf("2d. Deactivating Pump and Closing Fork Valves, Transiting\n");

			basecmd.trigger_tiller = 0x00;

			// Stop Fork Control
			bDeActivateValveControl = true;
			stopForkControl();

			//
			bForkControl = false;
		}
	}
}

void BydFLDriveSteerForkThread::startForkControl(Platform::Sensors::PalletBaseCmdMsg data_basecmd)
{
	// Start Fork Control
	std::unique_lock<std::mutex> ao_lock(mutex_AOController);

	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, true);

	ao_lock.unlock();
}

void BydFLDriveSteerForkThread::stopForkControl()
{
	// Stop Fork Control
	std::unique_lock<std::mutex> ao_lock(mutex_AOController);

	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 2.5); //maybe dont need
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(1, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(2, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 2.5);

	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, false);

	ao_lock.unlock();
}

void BydFLDriveSteerForkThread::SetCurrentCommands(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd, Platform::Sensors::PalletBaseCmdMsg data_basecmd)
{
	dbw_cmd_ = data_dbwcmd;
	base_cmd_ = data_basecmd;
}

void BydFLDriveSteerForkThread::CmdSpeedToVotlage(float cmd_speed_, float &ch0_voltage_)
{
	cmd_speed_ = fabs(cmd_speed_);

	if (cmd_speed_ > 1.0)
	{
		cmd_speed_ = 1.0;
	}

	if (cmd_speed_ == 0.0)
	{
		ch0_voltage_ = 0.0;
	}
	else
	{
		//		ch0_voltage_ = (cmd_speed_*3.344)+1.456;
		ch0_voltage_ = ((cmd_speed_ * 3.6784) + 1.10216) - 0.5;
		//		ch0_voltage_ = (0.01*3.6784)+1.1216;
	}
}

bool BydFLDriveSteerForkThread::CalibrateBrake()
{
	bool localRetCode = false;
	uint64_t startCalibrationTime = Common::Utility::Time::getusecCountSinceEpoch() ; //microseconds
	double	 deltaTime			  = (Common::Utility::Time::getusecCountSinceEpoch() - startCalibrationTime)/1000000.00; //seconds
	double 	 windowTime			  = 1.5;
	
	sptr_SmartMotorSteer->set_motor_torque();
	sptr_SmartMotorSteer->set_throttle_motor_torque(-3000);
	sptr_SmartMotorSteer->set_throttle_motor_torque_slope(-5000);
	int vel = 100001;

	std::cout << "Calibrating Brake......................" << std::endl;
	do{
		//std::cout << "Calibrating Brake......................" << std::endl;
		deltaTime = (Common::Utility::Time::getusecCountSinceEpoch() - startCalibrationTime)/1000000.00;
		//std::cout << "Delta Time ( increase , max 10s )     : " << deltaTime << std::endl;
	} while (deltaTime < windowTime );
	std::cout << "Calibrating Brake  Finish.............." << std::endl;

	sleep(1);
	int pos = -99999;
	pos = sptr_SmartMotorSteer->get_throttle_motor_pos();
	std::cout << "Motor Pos :"  << pos << endl;
	std::cout << "Motor Pos :"  << pos << endl;

	vel = sptr_SmartMotorSteer->get_motor_velocity();
	if (fabs(vel) < 100000 )
	{
		localRetCode = true;
	}
	std::cout << "Calibration finished with return 		: " << localRetCode  << std::endl;
	return localRetCode;
}

void BydFLDriveSteerForkThread::CmdBrake(float cmd_brake_)
{

	if (cmd_brake_ == 1)
	{
		TorqueBrake();
		std::cout << " BRAKES ACTIVATED " << std::endl;
	}
	else if (cmd_brake_ == 0)
	{
		std::cout << "release brake" << std::endl;
//		sptr_SmartMotorSteer->set_motor_position();
//		sptr_SmartMotorSteer->set_throttle_motor_pos(0, 0);
		sptr_SmartMotorSteer->set_motor_torque();
		sptr_SmartMotorSteer->set_throttle_motor_torque(-3000);
		sptr_SmartMotorSteer->set_throttle_motor_torque_slope(-5000);

	}
}

void BydFLDriveSteerForkThread::TorqueBrake()
{
	int vel = 0;

	sptr_SmartMotorSteer->set_motor_torque();
	sptr_SmartMotorSteer->set_throttle_motor_torque(6000);			// Max torque
	sptr_SmartMotorSteer->set_throttle_motor_torque_slope(100000); 	//how fast it goes to desired torque
	vel = sptr_SmartMotorSteer->get_motor_velocity();
	if (fabs(vel) < 100000)
	{
		sptr_SmartMotorSteer->set_torque_motor_stop();
	}
}
void BydFLDriveSteerForkThread::TorqueControl(float64_t torque)
{
	int vel = 0;
	torque = torque * 6000;
	sptr_SmartMotorSteer->set_motor_torque();
	sptr_SmartMotorSteer->set_throttle_motor_torque(torque);
	sptr_SmartMotorSteer->set_throttle_motor_torque_slope(100000);
	vel = sptr_SmartMotorSteer->get_motor_velocity();
	if (fabs(vel) < 100000)
	{
		sptr_SmartMotorSteer->set_torque_motor_stop();
	}
}

float32_t BydFLDriveSteerForkThread::ComputeSteerDeg(int32_t encoder_pos)
{
	float32_t SteerDeg;

	SteerDeg = (encoder_pos - encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_home) * steer_encoder_resolution;
	std::cout << "resolution = " << steer_encoder_resolution << std::endl;
	return SteerDeg;
}

void BydFLDriveSteerForkThread::DecodeVCM()
{
	// reset controls
	palletbase_cmd.trigger_tiller = 0x00;

	// assign current encoder values
	uint32_t current_steer_enc_pos_ = steer_enc_pos;
	uint32_t current_line_enc_pos_ = line_enc_pos;
	uint32_t current_tilt_enc_pose_ = tilt_enc_pos;

	// assign steer encoder presets
	uint32_t home_steer_enc_pos = encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_home;
	uint32_t lpos_steer_enc_pos = encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_lpos;
	uint32_t rpos_steer_enc_pos = encoder_config_params.params_Steer_Encoder_Settings.steer_encoder_rpos;
	uint32_t left_difference = abs(lpos_steer_enc_pos - home_steer_enc_pos);
	uint32_t right_difference = abs(home_steer_enc_pos - rpos_steer_enc_pos);

	float steer_tolerance = 0.03; //0.05

	uint32_t desired_enc_pos = home_steer_enc_pos;
	//

	// assign line encoder presets
	uint32_t home_line_enc_pos = encoder_config_params.params_Line_Encoder_Settings.line_encoder_home;
	uint32_t min_line_enc_pos = encoder_config_params.params_Line_Encoder_Settings.line_encoder_min;
	uint32_t max_line_enc_pos = encoder_config_params.params_Line_Encoder_Settings.line_encoder_max;

	float raise_line_tolerance = 0.8; //0.8
	float lower_line_tolerance = 4;	  //4

	//

	// assign tilt encoder presets
	uint32_t home_tilt_enc_pos = encoder_config_params.params_Tilt_Encoder_Settings.tilt_encoder_home;
	uint32_t min_tilt_enc_pos = encoder_config_params.params_Tilt_Encoder_Settings.tilt_encoder_min;
	uint32_t max_tilt_enc_pos = encoder_config_params.params_Tilt_Encoder_Settings.tilt_encoder_max;

	float tilt_tolerance = 0.15;

	std::cout << "CK - bTiltFork = " << bTiltFork << std::endl;
	std::cout << "CK - bRaiseFork = " << bRaiseFork << std::endl;
	std::cout << "CK - bSteerAction = " << bSteerAction << std::endl;
	std::cout << "CK - trigger_tiller = " << palletbase_cmd.trigger_tiller << std::endl;
	printf("palletbase_cmd.trigger_tiller = %x \n", palletbase_cmd.trigger_tiller);

	/*---------- decode steer end ----------*/

	if (msg_health_status.pcm_feedback_healthstatus.rx_steer_encoder_fb)
	{
		float32_t steer_current_deg = ComputeSteerDeg(steer_enc_pos);
		std::cout << "steer enc value = " << steer_enc_pos << std::endl;
		std::cout << "Steer ========== " << dbw_cmd.cmd_steer << std::endl;

		// Max/Min Voltage for steering?? 1.5/-1.5
		dbw_cmd.cmd_steer = dbw_cmd.cmd_steer/M_PI_2 * 1.5; //linear conversion of angle to voltage
		//

		if (!bForkMoving)
		{
			if (dbw_cmd.cmd_steer == 0)
			{
				bSteerAction = false;
				palletbase_cmd.trigger_tiller = 0x00;
				dbw_cmd.cmd_steer = 0;
				steer_error_integral = 0;
			}
			else
			{
				bSteerAction = true;
				palletbase_cmd.trigger_tiller = 0x09;
			}

			// compensate for deadband
			if(dbw_cmd.cmd_steer >= -0.175 && dbw_cmd.cmd_steer < 0)
			{
				dbw_cmd.cmd_steer = -0.175;
			}
			else if(dbw_cmd.cmd_steer <= 0.175 && dbw_cmd.cmd_steer > 0)
			{
				dbw_cmd.cmd_steer = 0.175;
			}
			//

			if (bActiveGear && (palletbase_cmd.trigger_tiller == 0x09))
			{
				std::unique_lock<std::mutex> ao_lock(mutex_AOController);
				sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(2, dbw_cmd.cmd_steer); //set prop valve steer right voltage
				sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 3.4);				   //set pump voltage
				ao_lock.unlock();
			}
		}
	}

	/*---------- decode steer end ----------*/
	std::cout << "1CK - bTiltFork = " << bTiltFork << std::endl;
	std::cout << "1CK - bRaiseFork = " << bRaiseFork << std::endl;
	std::cout << "1CK - bSteerAction = " << bSteerAction << std::endl;
	printf("palletbase_cmd.trigger_tiller = %x \n", palletbase_cmd.trigger_tiller);
	std::cout << "1CK - msg_health_status.receive_basecmd_msg = " << msg_health_status.receive_basecmd_msg << std::endl;
	std::cout << "1CK - dbw_cmd.cmd_speed = " << dbw_cmd.cmd_speed << std::endl;
	std::cout << "1CK - palletbase_cmd.seq_key = " << palletbase_cmd.seq_key << std::endl;
	std::cout << "1CK - msg_health_status.unmmaned_mode = " << msg_health_status.unmmaned_mode << std::endl;

	/*---------- decode line start ----------*/
	if (/*bTransitFork &&*/ msg_health_status.receive_basecmd_msg && palletbase_cmd.seq_key == 1)
	{
		last_desired_fork_height = palletbase_cmd.fork_height;
		//-------comment if not working------------
		bSteerAction = false;
		palletbase_cmd.trigger_tiller = 0x00;
		//-----------------------------------------
	}

	//	if (!bTransitFork)
	//	{
	//		float current_height = (current_line_enc_pos_/100.0) - 6.2; //in cm
	//		last_desired_fork_height = current_height / 100.0; //in m
	//		bTransitFork = true;
	//	}

	uint32_t desired_fork_height = last_desired_fork_height * 100; // desired_fork_height in cm, palletbase_cmd.fork_height in m
	uint32_t desired_line_enc_pos = (desired_fork_height + 6.2) * 100.0;

	//
	msg_health_status.desired_platform_forkheight = last_desired_fork_height;
	//
	//
//	printf("desired_fork_height = %d cm \n", desired_fork_height);
//	printf("desired_line_enc_pos = %d\n", desired_line_enc_pos);
//	printf("current_line_enc_pos_ = %d\n", current_line_enc_pos_);
//	printf("trigger_tiller = %x\n", palletbase_cmd.trigger_tiller);
//	std::cout << "bTiltFork = " << bTiltFork << std::endl;
//	std::cout << "bSteerAction = " << bSteerAction << std::endl;
//	std::cout << "raise fork seq key = " << palletbase_cmd.seq_key << std::endl;

	if (palletbase_cmd.trigger_tiller == 0x00 && !bTiltFork
			&& !bSteerAction && dbw_cmd.cmd_speed == 0
			&& palletbase_cmd.seq_key == 1
			&& msg_health_status.unmmaned_mode == true)
	{
		//	std::cout << "trigger_tiller = 0 " << std::endl;

		if (desired_line_enc_pos > max_line_enc_pos)
		{
			desired_line_enc_pos = max_line_enc_pos;
		}
		else if (desired_line_enc_pos < min_line_enc_pos)
		{
			desired_line_enc_pos = min_line_enc_pos;
		}

		//		if (current_line_enc_pos_ > (desired_line_enc_pos - (interval * raise_line_tolerance)) &&
		//				current_line_enc_pos_ < (desired_line_enc_pos + (interval * lower_line_tolerance)))
		uint32_t linetolerance = (max_line_enc_pos - min_line_enc_pos) * 0.005; //0.05
		if (current_line_enc_pos_ >= (desired_line_enc_pos - linetolerance) &&
			current_line_enc_pos_ <= (desired_line_enc_pos + linetolerance))
		{
			palletbase_cmd.trigger_tiller = 0x00;
			bForkHeight = true;
			bForkMoving = false;
			bRaiseFork = false;
			std::cout << "reached fork desired" << std::endl;

		}
		else
		{
			ForkHeightCounter = 0;
			bForkHeight = false;
			bForkMoving = true;
			bRaiseFork = true;
			if (current_line_enc_pos_ < desired_line_enc_pos)
			{
				palletbase_cmd.trigger_tiller = 0x05;

				std::cout << "raising fork" << std::endl;
			}
			else if (current_line_enc_pos_ > desired_line_enc_pos)
			{
				palletbase_cmd.trigger_tiller = 0x03;

				std::cout << "lowering fork" << std::endl;
			}
		}
	}
	else
	{
		bRaiseFork = false;
	}
	//	/*---------- decode line end ----------*/
	//
	//	/*---------- decode tilt angle start ----------*/
	if (/*bTransitTilt &&*/ msg_health_status.receive_basecmd_msg && palletbase_cmd.seq_key == 2)
	{
		last_desired_tilt_angle = palletbase_cmd.tilt_angle;
		//-------comment if not working------------
		bSteerAction = false;
		palletbase_cmd.trigger_tiller = 0x00;
		//-----------------------------------------
	}
	//
	//	if (!bTransitTilt)
	//	{
	//		last_desired_tilt_angle = current_tilt_enc_pose_ / 10.0; //in degree
	//		bTransitTilt = true;
	//	}

	uint32_t desired_tilt_enc_pos = last_desired_tilt_angle * 10;

//	printf("desired_tilt_enc_pos = %d\n", desired_tilt_enc_pos);
//	printf("current_tilt_enc_pose_ = %d\n", current_tilt_enc_pose_);
//	printf("trigger_tiller = %x\n", palletbase_cmd.trigger_tiller);
//	std::cout << "bRaiseFork = " << bRaiseFork << std::endl;
//	std::cout << "bSteerAction = " << bSteerAction << std::endl;
//	std::cout << "tilt seq key = " << palletbase_cmd.seq_key << std::endl;

	if (palletbase_cmd.trigger_tiller == 0x00 && !bRaiseFork && !bSteerAction
			&& dbw_cmd.cmd_speed == 0 && palletbase_cmd.seq_key == 2
			&& msg_health_status.unmmaned_mode == true)
	{
		//	std::cout << "trigger_tiller = 0 " << std::endl;

		if (desired_tilt_enc_pos > max_tilt_enc_pos)
		{
			desired_tilt_enc_pos = max_tilt_enc_pos;
		}
		else if (desired_tilt_enc_pos < min_tilt_enc_pos)
		{
			desired_tilt_enc_pos = min_tilt_enc_pos;
		}

		uint32_t tilttolerance = 10; //desired_tilt_enc_pos*tilt_tolerance;
		if (current_tilt_enc_pose_ >= (desired_tilt_enc_pos - tilttolerance) &&
			current_tilt_enc_pose_ <= (desired_tilt_enc_pos + tilttolerance))
		{
			tilt_count++;
			palletbase_cmd.trigger_tiller = 0x00;
			if(tilt_count >= 50)
			{
			palletbase_cmd.trigger_tiller = 0x00;
			bTiltAngle = true;
			bForkMoving = false;
			bTiltFork = false;
			tilt_count = 0;
			}
		}
		else
		{
			tilt_count = 0;
			TiltAngleCounter = 0;
			bTiltFork = true;
			bTiltAngle = false;
			bForkMoving = true;
			if (current_tilt_enc_pose_ < desired_tilt_enc_pos)
			{
				palletbase_cmd.trigger_tiller = 0x11;

				std::cout << "tilt down" << std::endl;
			}
			else if (current_tilt_enc_pose_ > desired_tilt_enc_pos)
			{
				palletbase_cmd.trigger_tiller = 0x21;

				std::cout << "tilt up" << std::endl;
			}
		}
	}
	else
	{
		bTiltFork = false;
	}

	bForkMoving = bTiltFork || bRaiseFork ;
	/*---------- decode tilt angle end ----------*/
}

void BydFLDriveSteerForkThread::Thread_TiltMotorControl()
{
	Platform::Sensors::TiltMotorMsg tilt_motor;
	if (sptr_RetreiveTiltMotorCmds_->GetTiltMotorMsgs(tilt_motor))
	{
		std::cout << "receive tilt motor msgs" << std::endl;
		sptr_PcmPalletController->ServoGoToPos(tilt_motor.position, tilt_motor.speed, 1);
	}
}

void BydFLDriveSteerForkThread::ShutdownDBWDriveAndSteer()
{
	std::cout << "Shutting down! " << std::endl;
	// Stop Controls

	stopForkliftControl();
	//sptr_SmartMotorSteer->set_motor_position();
	//sptr_SmartMotorSteer->set_throttle_motor_pos(0, 0);
	sptr_SmartMotorSteer->set_motor_torque();
	sptr_SmartMotorSteer->set_throttle_motor_torque(-3000);
	sptr_SmartMotorSteer->set_throttle_motor_torque_slope(-5000);
	//sleep(3);
	//	for(int32_t i=0; i<20; i++)//1s
	//	{
	//		sptr_PcmPalletController->CloseAllValves();						// Close all valves
	//		usleep(50000); //50ms requirements
	//	}
	//
	// sptr_PcmPalletController->SetShutdownPCMProcedure();				// shutdown
	std::cout << "Shutdown done! " << std::endl;
}

void BydFLDriveSteerForkThread::CommandVehSpeedChange(const Platform::Sensors::PalletDbwCmdMsg dbw_cmd)
{
	int32_t desired_gear_state;

	//	std::cout << "dbw speed = " << dbw_cmd.cmd_speed << std::endl;
	//Apply brake
	if (fabs(dbw_cmd.cmd_speed) == 0.0)
	{
		//TODO:: send brake command

		desired_gear_state = DbwGear::NEUTRAL; //1
		if (desired_gear_state != previous_gear_position)
		{
			ChangeGear(desired_gear_state);
			previous_gear_position = desired_gear_state;
		}
	} //
	else if (dbw_cmd.cmd_speed > 0.0)
	{
		desired_gear_state = DbwGear::FORWARD; //2
		if (desired_gear_state != previous_gear_position)
		{
			ChangeGear(desired_gear_state);
			previous_gear_position = desired_gear_state;
		}

		//TODO :: send speed command,
	}
	else if (dbw_cmd.cmd_speed < 0.0)
	{
		desired_gear_state = DbwGear::REVERSE; //3
		if (desired_gear_state != previous_gear_position)
		{
			ChangeGear(desired_gear_state);
			previous_gear_position = desired_gear_state;
		}

		//TODO :: send speed command
	}
}

void BydFLDriveSteerForkThread::ChangeGear(const int32_t desired_gear_position)
{
	uint8_t status[4] = {0};

	std::unique_lock<std::mutex> ao_lock(mutex_AOController);

	if (desired_gear_position == 1)
	{
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, false);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(1, false);
	}
	else if (desired_gear_position == 2)
	{
		//sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, true);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, false);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(1, true);
	}
	else if (desired_gear_position == 3)
	{
		//sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, true);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(1, false);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, true);
	}
	//
	//	sptr_ADAM_ROController_Gear->read_adam_6266_ro_status(status);
	//
	//	printf( "---------- Printing 10.0.0.6 Status ----------\n" );
	//
	//	printf( "[Channel 0] Relay Output = 0x%X\n" , status[0] );
	//	printf( "[Channel 1] Relay Output = 0x%X\n" , status[1] );
	//	printf( "[Channel 2] Relay Output = 0x%X\n" , status[2] );
	//	printf( "[Channel 3] Relay Output = 0x%X\n" , status[3] );
	//
	//	printf( "---------- Printing 10.0.0.6 Status ----------\n" );

	ao_lock.unlock();
}

void BydFLDriveSteerForkThread::stopForkliftControl()
{
	//set safety voltage for 10.0.0.3

	std::unique_lock<std::mutex> ao_lock(mutex_AOController);

	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(1, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(2, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 2.5);

	usleep(50000);

	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(1, false); //off prop valve
	//sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(0, false); //off main valve

	//off 10.0.0.2 fork control
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, false);

	//neutral gear
	sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, false);
	sptr_ADAM_ROController_Gear->write_adam6266_ro_status(1, false);

	//shut down drive
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(0, false);
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(2, false);
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(3, false);
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(4, false);
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(5, false);

	sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(0, 0);
	sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(1, 0.0);
	sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(2, 0.0); //7.5

	ao_lock.unlock();
}

BydFLDriveSteerForkThread::~BydFLDriveSteerForkThread()
{
	ShutdownDBWDriveAndSteer();
	ptrFileText_decodevcm_Logger.close();
}

} // namespace DBW
