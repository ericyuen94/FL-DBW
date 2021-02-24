/**
 *  @file    calibsteermapviewerTab.cpp
 *  @author  Emily Low
 *  @date    26th July 2015
 *  @version 1.0.0
 *
 *  @brief controller for vmap viewer
 *
 *  @section DESCRIPTION
 *
 */

#include "calibsteermapviewerTab.h"
#include "Common/Utility/TimeUtils.h"
#include "Platform/PlatformDefinition.h"
#include "StkciPCMPalletBoundary.h"
//
#include <cmath>

namespace calibsteerEmulator
{

calibsteermapviewerTab::calibsteermapviewerTab(QWidget *parent)
: QWidget(parent)
{
	initcalibsteermapviewerTab();
	current_seiko_steer_enc_pos = 0;
	feedback_seiko_steer_angle = 0.0;
	current_speed_percent = 0.0;
	last_rx_timestamp_seiko_steer_enc_pos = 0;
	status_center_steering_home = false;
	button_start_move_state = false;
	read_pcm_readback_status = false;
}

void calibsteermapviewerTab::initcalibsteermapviewerTab()
{
	//Init shared ptrs
	sptr_cGetCalibSteerConfiguration = std::make_shared<GetCalibSteerConfiguration>();
	sptr_cGetSmartMotorSteerConfiguration = std::make_shared<DBWPalletCommon::GetSmartMotorSteerConfiguration>();
	sptr_cPCMPalletController = std::make_shared<DBWPalletCommon::PCMPalletController>();
	sptr_SmartMotorSteer = std::make_shared<DBW_Ctrl>();
	sptr_LogCalibSteer = std::make_shared<LogCalibSteer>();
	//

	//Read calib steer csci settings
	if(!sptr_cGetCalibSteerConfiguration->OpenCalibSteerConfiguration())
	{
		std::cout << "Error read QTCalibSteerMotorCurvature XML file " << std::endl;
		exit(1);
	}
	else
	{
		//Get the configuration data.
		sptr_cGetCalibSteerConfiguration->GetCalibSteerConfigData(params_calib_steer_config);
		sptr_cGetCalibSteerConfiguration->PrintCalibSteerConfigData(params_calib_steer_config);
	}

	//Read smart motor steer config file.
	if(!sptr_cGetSmartMotorSteerConfiguration->OpenSmartMotorSteerConfiguration())
	{
		std::cout << "Error read SmartMotorSteer XML file " << std::endl;
		exit(1);
	}
	else
	{
		//Get the configuration data.
		sptr_cGetSmartMotorSteerConfiguration->GetSmartMotorSteerConfigData(params_dbw_config);
		sptr_cGetSmartMotorSteerConfiguration->PrintSmartMotorSteerConfigData(params_dbw_config);

		//Open Drive PCM port
		sptr_cPCMPalletController->OpenCanPort(params_dbw_config.params_SteerEncoder_Port.pcan_port);
		sptr_cPCMPalletController->ResetLastCommandSpeed();
		//Send a startup procedure to prevent pallet from electrical shutdown
		sptr_cPCMPalletController->SetStartUpProcedurePallet();

		//Open Steer motor port
		sptr_SmartMotorSteer->init_steer_motor((char*)params_dbw_config.params_SmartMotor_Port.steer_motor_device.c_str());
	}

	//Open Calibration Steer log file
	if(params_calib_steer_config.SteerCalibLog.enabled==1)
	{
		sptr_LogCalibSteer->SetupCalibSteerLogger(params_calib_steer_config.SteerCalibLog.path);
	}

	//Open PCM Log Can Message file
	if(params_calib_steer_config.Debug.enabled==1)
	{
		sptr_cPCMPalletController->SetLogger(params_calib_steer_config.Debug.path);
	}

	//init stkci messages
	sptr_RetrieveVehicleMeasure_gps = std::make_shared<RetrieveVehicleMeasure>(params_calib_steer_config.StkciSenderId.gps_vehiclemeasure_senderid);
	sptr_RetrieveCovPoseMsg = std::make_shared<RetrieveCovPoseMsg>(params_calib_steer_config.StkciSenderId.posecov_senderid);

	//Get the max steering calibration angle.
	max_steer_angle = params_dbw_config.params_Steer_Settings.max_steering;

	//set main layout.
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(createOne_calibsteer());
	mainLayout->addStretch(1);

	//Set threading
	//set command steer motor timer job.
	checkSteerMotorTimer.setInterval(100); //10Hz
	connect(&checkSteerMotorTimer, SIGNAL (timeout()), this, SLOT (handle_SteerMotorCmd_job()));
	checkSteerMotorTimer.start();

	//Set threading
	//set seiko steer encoder time job
	checkSteerEncoderTimer.setInterval(10); //50Hz
	connect(&checkSteerEncoderTimer, SIGNAL (timeout()), this, SLOT (handle_SeikoGetEncoderSteer_job()));
	checkSteerEncoderTimer.start();

	//Set threading
	//set PCM drive time job
	checkPcmDriveTimer.setInterval(50); //100Hz
	connect(&checkPcmDriveTimer, SIGNAL (timeout()), this, SLOT (handle_PCMDriveCmd_job()));
	checkPcmDriveTimer.start();

	//Set threading
	//set PCM Feedback time job
	checkPCMFeedbackTimer.setInterval(10); //100Hz
	connect(&checkPCMFeedbackTimer, SIGNAL (timeout()), this, SLOT (handle_PCMFeedback_job()));
	checkPCMFeedbackTimer.start();

	//
	setLayout(mainLayout);
}

void calibsteermapviewerTab::handle_SteerMotorCmd_job()
{
	//Do steering home center at the startup
	if(status_center_steering_home == false)
	{
		bool seiko_encoder_valid=false;
		uint64_t delta_time_encoder;
		delta_time_encoder = Common::Utility::Time::getusecCountSinceEpoch() - last_rx_timestamp_seiko_steer_enc_pos;

		//check validity of encoder reading
		if(delta_time_encoder<200000)//200ms
		{
			seiko_encoder_valid = true;
			std::cout << "delta_time_encoder = " << delta_time_encoder << std::endl;
		}

		//start steering home centering
		if(seiko_encoder_valid)
		{
			std::cout << "********************************************"<< std::endl;
			std::cout << "Begin Steer Home Procedure " << std::endl;
			std::cout << "********************************************"<< std::endl;
			sptr_SmartMotorSteer->Center_SteerMotor(params_dbw_config.params_Steer_Encoder_Settings.steer_encoder_home,current_seiko_steer_enc_pos);
			status_center_steering_home = true;
			std::cout << "*************** Steer Home Done ************"<< std::endl;
			//exit(1);
		}
	}

	//Get current desired steer angle
	if(sm_motor_spinbox->value()<0.0) //turn right, Right hand convention
	{
		desired_steer_angle = sm_motor_spinbox->value()*max_steer_angle;
	}
	else//turn left, Right hand convention
	{
		desired_steer_angle = sm_motor_spinbox->value()*max_steer_angle;
	}
	//
	if(fabs(sm_motor_spinbox->value())<0.01)
	{
		desired_steer_angle = 0.0;
	}

	//Send command to motor after steer home centering
	if(status_center_steering_home)
	{
		sptr_SmartMotorSteer->set_pallet_truck_desired_steer_angle_motor_pos(desired_steer_angle);
	}

	//display to GUI
	std::stringstream steer_angle;
	steer_angle << desired_steer_angle;
	label_desired_steer_angle_cmd_deg->setText(QString::fromStdString(steer_angle.str()));
}

void calibsteermapviewerTab::handle_SeikoGetEncoderSteer_job()
{
	uint32_t current_steer_enc_pos=0;
	if(sptr_cPCMPalletController->GetSeikoSteerEncoderFeedback(current_steer_enc_pos) && read_pcm_readback_status == true)
	{
		last_rx_timestamp_seiko_steer_enc_pos = Common::Utility::Time::getusecCountSinceEpoch();
		current_seiko_steer_enc_pos = current_steer_enc_pos;
		//
		sptr_cPCMPalletController->ConvertSeikoSteerCounttoAngle(params_dbw_config.params_Steer_Encoder_Settings.steer_encoder_home,current_seiko_steer_enc_pos,feedback_seiko_steer_angle);
		std::stringstream enc_count;
		enc_count << current_steer_enc_pos;
		label_seiko_enc->setText(QString::fromStdString(enc_count.str()));
		std::stringstream enc_fb_angle;
		enc_fb_angle << feedback_seiko_steer_angle*180.0/3.141;
		label_seiko_enc_fb_deg->setText(QString::fromStdString(enc_fb_angle.str()));
	}

	//Retrieve stkci messages
	if(sptr_RetrieveVehicleMeasure_gps->GetVehicleMeasureData(stkci_vehmeas_sbg_data))
	{
		std::cout << "[Receive] stkci_vehmeas_sbg_data msg !!! " << std::endl;
	}
	else
	{
		std::cout << "Did not receive stkci_vehmeas_sbg_data msg ??? " << std::endl;
	}

	//
	if(sptr_RetrieveCovPoseMsg->GetGeometryMsgsPoseWithCovarianceMsg(stkci_pose_data))
	{
		std::cout << "[Receive] pose cov msg !!! " << std::endl;
	}
	else
	{
		std::cout << "Did not receive pose cov msg ??? " << std::endl;
	}

	//Log data for calibration steer
	//TODO:: fill up log_calib_data
	if(params_calib_steer_config.SteerCalibLog.enabled==1)
	{
		if(button_start_move_state)//start moving
		{
			collate_steer_calib_logged_data();
			sptr_LogCalibSteer->WriteLog(Common::Utility::Time::getusecCountSinceEpoch(),log_calib_data);
		}
	}//log enabled
}

void calibsteermapviewerTab::collate_steer_calib_logged_data()
{
	log_calib_data.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	log_calib_data.default_max_steer_angle = max_steer_angle;
	log_calib_data.command_steer_angle = desired_steer_angle;
	//
	log_calib_data.default_seiko_home_pos = params_dbw_config.params_Steer_Encoder_Settings.steer_encoder_home;
	log_calib_data.default_seiko_extreme_left_pos = params_dbw_config.params_Steer_Encoder_Settings.steer_encoder_lpos;
	log_calib_data.default_seiko_extreme_right_pos = params_dbw_config.params_Steer_Encoder_Settings.steer_encoder_rpos;
	log_calib_data.feedback_seiko_encoder_pos = current_seiko_steer_enc_pos;
	log_calib_data.fb_seiko_steer_angle = feedback_seiko_steer_angle;

	std::unique_lock<std::mutex> lock(mutex_data_pcm_fb);
	//TODO:: to get from sender ID vehicle measure not IMAR...All info collate in the DBWs
	log_calib_data.feedback_platform_speed = pcm_feedback.current_platform_speed;
	log_calib_data.feedback_platform_steer_angle = feedback_seiko_steer_angle;
	log_calib_data.gps_smooth_x = stkci_pose_data.pose.position.x;
	log_calib_data.gps_smooth_y = stkci_pose_data.pose.position.y;
	log_calib_data.gps_vel = stkci_vehmeas_sbg_data.speed;
	log_calib_data.gps_yaw = stkci_pose_data.pose.altitude.yaw;
	log_calib_data.gps_yawrate = stkci_vehmeas_sbg_data.yaw_rate;//TBD..remember to switch in sensor fusion
	log_calib_data.gps_aux_steer = stkci_vehmeas_sbg_data.steering;//TBD..remember to switch in sensor fusion

	//compute gps curvature
	double computed_gps_curvature_;
	if(fabs(stkci_vehmeas_sbg_data.speed)>0.01)
	{
		computed_gps_curvature_ = stkci_vehmeas_sbg_data.yaw_rate/stkci_vehmeas_sbg_data.speed;
	}
	else
	{
		computed_gps_curvature_ = 0.0;
	}
	log_calib_data.computed_gps_curvature = computed_gps_curvature_;

	//compute platform curvature
	double computed_platform_curvature_;
	if(fabs(pcm_feedback.current_platform_speed)>0.01)
	{
		computed_platform_curvature_ = stkci_vehmeas_sbg_data.yaw_rate/pcm_feedback.current_platform_speed;
	}
	else
	{
		computed_platform_curvature_ = 0.0;
	}
	log_calib_data.computed_platform_curvature = computed_platform_curvature_;

	//other data
	log_calib_data.ucBrakeTrigger = 0;
	log_calib_data.ucESTOP = 0;
	log_calib_data.brake_dist = 0;
	log_calib_data.brake_timestamp = 0;
}

void calibsteermapviewerTab::handle_PCMDriveCmd_job()
{
	//Send to PCM controller
	DBWPalletCommon::pcm_palletbase_commands pcm_commands_current;
	DBWPalletCommon::pcm_palletbase_commands pcm_commands_direction_change;

	//Send Heart beat to prevent pallet from electrical shutdown
	sptr_cPCMPalletController->SetHeartBeatTiller();

	//Convert to STKCI to pcm commands
	DBWPalletCommon::StkciPCMPalletBoundary::Convert_Speed_To_PalletPCMCmd(current_speed_percent,pcm_commands_current);
	if(sptr_cPCMPalletController->PCMSpeedCtrlLogic(current_speed_percent))
	{
		pcm_commands_direction_change = pcm_commands_current;
		pcm_commands_direction_change.analog_speed=0;
		sptr_cPCMPalletController->SetPCMPalletCommands(pcm_commands_direction_change);
		//std::cout << "Send change of speed direction msg" << std::endl;
		usleep(50000); //50ms requirements
	}
	sptr_cPCMPalletController->SetPCMPalletCommands(pcm_commands_current);
}

void calibsteermapviewerTab::handle_PCMFeedback_job()
{
	//Define stkci messages
	Platform::Sensors::PalletDbwFbkMsg out_stkci_dbw_fbk; //TBD speed feedback, encoders etc
	Platform::Sensors::PalletBaseFbkMsg out_stkci_basepallet_fbk; //TBD battery

	std::unique_lock<std::mutex> lock(mutex_data_pcm_fb);

	sptr_cPCMPalletController->GetPCMPalletFeedback(pcm_feedback);
	read_pcm_readback_status = true;
	DBWPalletCommon::StkciPCMPalletBoundary::Convert_PalletPCMFbk_To_StkciDBWAndPalletBaseFbk(pcm_feedback,out_stkci_dbw_fbk,out_stkci_basepallet_fbk);
	//load in the battery feedback
	out_stkci_basepallet_fbk.batterylevel = pcm_feedback.current_platform_batterylevel;
	//std::cout << "batterylevel = " << out_stkci_basepallet_fbk.batterylevel << std::endl;
	//
	std::stringstream batt_level;
	batt_level << out_stkci_basepallet_fbk.batterylevel;
	label_platform_battery_level->setText(QString::fromStdString(batt_level.str()));
}

void calibsteermapviewerTab::handle_button_start_move()
{
	//Control the start and stop movement
	if(button_start_move_state==false && status_center_steering_home == true)
	{
		std::cout << "GO button depressed " << std::endl;
		button_start_move->setText("Stop");
		button_start_move_state = true;
		current_speed_percent = pcm_linear_speed_spinbox->value();
		if(fabs(pcm_linear_speed_spinbox->value())<0.01)
		{
			current_speed_percent = 0.0;
		}
	}
	else if(button_start_move_state==true && status_center_steering_home == true)
	{
		std::cout << "Stop button depressed " << std::endl;
		button_start_move->setText("Go");
		button_start_move_state = false;
		current_speed_percent = 0.0;
	}
}

void calibsteermapviewerTab::handle_button_smhome()
{
	sm_motor_spinbox->setValue(0);
	//send command to motor after steer home centering
	if(status_center_steering_home)
	{
		sptr_SmartMotorSteer->set_pallet_truck_desired_steer_angle_motor_pos(0);
	}
}

QGroupBox *calibsteermapviewerTab::createOne_calibsteer()
{
	 QGroupBox *groupsmctrlBox = new QGroupBox(tr("SteerMotorController"));

	 //
	 QLabel *doubleSpinBox_steermotor_label = new QLabel(tr("Steer percent"));
	 sm_motor_spinbox = new QDoubleSpinBox();
	 sm_motor_spinbox->setRange(-1, 1);
	 sm_motor_spinbox->setSingleStep(0.1);
	 sm_motor_spinbox->setValue(0);
	 QHBoxLayout *hlayout_spinbox_smhome = new QHBoxLayout;
	 hlayout_spinbox_smhome->addWidget(doubleSpinBox_steermotor_label);
	 hlayout_spinbox_smhome->addWidget(sm_motor_spinbox);

	 //
	 QLabel *pushbtn_steermotor_label = new QLabel(tr("Home Motor Pos"));
	 button_smhome = new QPushButton("Home Steer Motor");
	 QObject::connect(button_smhome, SIGNAL(clicked()),this, SLOT(handle_button_smhome()));
	 QHBoxLayout *hlayout_button_smhome = new QHBoxLayout;
	 hlayout_button_smhome->addWidget(pushbtn_steermotor_label);
	 hlayout_button_smhome->addWidget(button_smhome);

	 //
	 QLabel *local_label_seiko_enc= new QLabel(tr("Seiko Enc = "));
	 label_seiko_enc = new QLabel(tr("0"));
	 QHBoxLayout *hlayout_seiko = new QHBoxLayout;
	 hlayout_seiko->addWidget(local_label_seiko_enc);
	 hlayout_seiko->addWidget(label_seiko_enc);

	 //
	 QLabel *local_label_seiko_angle= new QLabel(tr("Steer fb (deg) = "));
	 label_seiko_enc_fb_deg = new QLabel(tr("0"));
	 QHBoxLayout *hlayout_seiko_angle = new QHBoxLayout;
	 hlayout_seiko_angle->addWidget(local_label_seiko_angle);
	 hlayout_seiko_angle->addWidget(label_seiko_enc_fb_deg);

	 //
	 QLabel *local_label_desired_steer_angle_cmd_deg = new QLabel(tr("Desired Steer (deg) = "));
	 label_desired_steer_angle_cmd_deg = new QLabel(tr("0"));
	 QHBoxLayout *hlayout_steer_ang = new QHBoxLayout;
	 hlayout_steer_ang->addWidget(local_label_desired_steer_angle_cmd_deg);
	 hlayout_steer_ang->addWidget(label_desired_steer_angle_cmd_deg);

	 //
	 QLabel *pcm_linear_speed_spinbox_label = new QLabel(tr("Speed percent"));
	 pcm_linear_speed_spinbox = new QDoubleSpinBox();
	 pcm_linear_speed_spinbox->setRange(-params_calib_steer_config.max_linear_speed, params_calib_steer_config.max_linear_speed);
	 pcm_linear_speed_spinbox->setSingleStep(0.1);
	 pcm_linear_speed_spinbox->setValue(0);
	 QHBoxLayout *hlayout_spinbox_linearspeed = new QHBoxLayout;
	 hlayout_spinbox_linearspeed->addWidget(pcm_linear_speed_spinbox_label);
	 hlayout_spinbox_linearspeed->addWidget(pcm_linear_speed_spinbox);

	 //
	 QLabel *pushbtn_start_move_label = new QLabel(tr("Pallet Move"));
	 button_start_move = new QPushButton("Go");
	 QObject::connect(button_start_move, SIGNAL(clicked()),this, SLOT(handle_button_start_move()));
	 QHBoxLayout *hlayout_button_startmove = new QHBoxLayout;
	 hlayout_button_startmove->addWidget(pushbtn_start_move_label);
	 hlayout_button_startmove->addWidget(button_start_move);

	 //
	 QLabel *local_label_batt= new QLabel(tr("Battery level = "));
	 label_platform_battery_level = new QLabel(tr("0"));
	 QHBoxLayout *hlayout_battlevel = new QHBoxLayout;
	 hlayout_battlevel->addWidget(local_label_batt);
	 hlayout_battlevel->addWidget(label_platform_battery_level);

	 //
	 QVBoxLayout *vbox = new QVBoxLayout;
	 vbox->addLayout(hlayout_spinbox_smhome);
	 vbox->addLayout(hlayout_button_smhome);
	 vbox->addLayout(hlayout_steer_ang);
	 vbox->addLayout(hlayout_seiko);
	 vbox->addLayout(hlayout_seiko_angle);
	 vbox->addLayout(hlayout_battlevel);

	 //
	 vbox->addLayout(hlayout_spinbox_linearspeed);
	 vbox->addLayout(hlayout_button_startmove);

	 //
	 groupsmctrlBox->setLayout(vbox);

	 return groupsmctrlBox;
}

calibsteermapviewerTab::~calibsteermapviewerTab()
{
	if(params_calib_steer_config.SteerCalibLog.enabled==1)
	{
		sptr_LogCalibSteer->CloseDataLogger();
	}
}

}
