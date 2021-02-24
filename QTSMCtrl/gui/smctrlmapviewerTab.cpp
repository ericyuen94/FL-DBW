/**
 *  @file    smctrlmapviewerTab.cpp
 *  @author  Emily Low
 *  @date    26th July 2015
 *  @version 1.0.0
 *
 *  @brief controller for vmap viewer
 *
 *  @section DESCRIPTION
 *
 */

#include "smctrlmapviewerTab.h"
#include "Common/Utility/TimeUtils.h"
#include "Platform/PlatformDefinition.h"
//
#include <cmath>

namespace smctrlEmulator
{

smctrlmapviewerTab::smctrlmapviewerTab(QWidget *parent)
: QWidget(parent)
{
	initsmctrlmapviewerTab();
	current_seiko_steer_enc_pos = 0;
	last_rx_timestamp_seiko_steer_enc_pos = 0;
	status_center_steering_home = false;
}

void smctrlmapviewerTab::initsmctrlmapviewerTab()
{
	//
	sptr_cPCMPalletController = std::make_shared<DBWPalletCommon::PCMPalletController>();
//	sptr_cPcanSeikoPcanEncoder = std::make_shared<DBWPalletCommon::PcanSeikoPcanEncoder>();
	sptr_SmartMotorSteer = std::make_shared<DBW_Ctrl>();

	//Read config file.
	if(!cGetSteerMotorDBWConfiguration.OpenSmartMotorSteerConfiguration())
	{
		std::cout << "Error read DBW XML file " << std::endl;
		exit(1);
	}
	else
	{
		//Get the configuration data.
		cGetSteerMotorDBWConfiguration.GetSmartMotorSteerConfigData(params_dbw_config);
		cGetSteerMotorDBWConfiguration.PrintSmartMotorSteerConfigData(params_dbw_config);

		//Open drive PCM port
		//sptr_cPCMPalletController->OpenCanPort(params_dbw_config.params_SteerEncoder_Port.pcan_port);

		//Open Seiko Pcan port
//		sptr_cPcanSeikoPcanEncoder->OpenCanPort(params_dbw_config.params_SteerEncoder_Port.pcan_port);

		//Open Steer motor port
		sptr_SmartMotorSteer->init_steer_motor((char*)params_dbw_config.params_SmartMotor_Port.steer_motor_device.c_str());
	}

	//
	max_steer_angle = params_dbw_config.params_Steer_Settings.max_steering;

	//set main layout.
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(createOne_smctrl());
	mainLayout->addStretch(1);

	//
	//set command steer motor timer job.
	checkSteerMotorTimer.setInterval(50); //20Hz
	connect(&checkSteerMotorTimer, SIGNAL (timeout()), this, SLOT (handle_SteerMotorCmd_job()));
	checkSteerMotorTimer.start();

	//set seiko steer encoder time job
	checkSteerEncoderTimer.setInterval(10); //20Hz
	connect(&checkSteerEncoderTimer, SIGNAL (timeout()), this, SLOT (handle_SeikoGetEncoderSteer_job()));
	checkSteerEncoderTimer.start();

	//
	setLayout(mainLayout);
}

void smctrlmapviewerTab::handle_SteerMotorCmd_job()
{
	//Do steering home center at the startup
	if(status_center_steering_home == false)
	{
		bool seiko_encoder_valid=false;
		uint64_t delta_time_encoder;
		delta_time_encoder = Common::Utility::Time::getusecCountSinceEpoch() - last_rx_timestamp_seiko_steer_enc_pos;

		std::cout << "delta_time_encoder = " << delta_time_encoder << std::endl;
		//check validity of encoder reading
		if(delta_time_encoder<200000)//100ms
		{
			seiko_encoder_valid = true;
		}

		//start steering home centering
		if(seiko_encoder_valid)
		{
			std::cout << "*****************************"<< std::endl;
			std::cout << "Begin Steer Home Procedure " << std::endl;
			std::cout << "*****************************"<< std::endl;
			sptr_SmartMotorSteer->Center_SteerMotor(params_dbw_config.params_Steer_Encoder_Settings.steer_encoder_home,current_seiko_steer_enc_pos);
			status_center_steering_home = true;
			//exit(1);
		}
	}

	//Get current desired steer angle
	float32_t desired_steer_angle=0;
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
		// steer boundary start (min -90, max 90)
		int current_motor_position, target_motor_position;
		float neg_desired_steer_angle = desired_steer_angle*-1.0;
		sptr_SmartMotorSteer->Get_HomeMotorPosition(current_motor_position);

		target_motor_position = current_motor_position-(neg_desired_steer_angle*1228.0702);
		if (target_motor_position < params_dbw_config.params_Steer_Settings.steer_min_pos)
		{
			// if target pos < min pos, target pos = min pos
			// desired angle = difference of min and current
			target_motor_position = params_dbw_config.params_Steer_Settings.steer_min_pos;
			desired_steer_angle = ((target_motor_position - current_motor_position) / 1228.0702);
		}
		else if (target_motor_position > params_dbw_config.params_Steer_Settings.steer_max_pos)
		{
			// if target pos > max pos, target pos = max pos
			// desired angle = difference of max and current
			target_motor_position = params_dbw_config.params_Steer_Settings.steer_max_pos;
			desired_steer_angle = ((target_motor_position - current_motor_position) / 1228.0702);
		}

		sptr_SmartMotorSteer->set_pallet_truck_desired_steer_angle_motor_pos(desired_steer_angle);
	}

	//display to GUI
	std::stringstream steer_angle;
	steer_angle << desired_steer_angle;
	label_desired_steer_angle_cmd_deg->setText(QString::fromStdString(steer_angle.str()));

}

void smctrlmapviewerTab::handle_SeikoGetEncoderSteer_job()
{
	uint32_t current_steer_enc_pos=0;
	if(sptr_cPcanSeikoPcanEncoder->GetSeikoSteerFeedback(current_steer_enc_pos))
	{
		last_rx_timestamp_seiko_steer_enc_pos = Common::Utility::Time::getusecCountSinceEpoch();
		current_seiko_steer_enc_pos = current_steer_enc_pos;
		//
		sptr_cPcanSeikoPcanEncoder->ConvertSeikoSteerCounttoAngle(params_dbw_config.params_Steer_Encoder_Settings.steer_encoder_home,current_seiko_steer_enc_pos,feedback_seiko_steer_angle);
		//
		std::stringstream enc_count;
		enc_count << current_steer_enc_pos;
		label_seiko_enc->setText(QString::fromStdString(enc_count.str()));
		//
		float64_t feedback_seiko_steer_angle_deg;
		feedback_seiko_steer_angle_deg = feedback_seiko_steer_angle*180/3.141;
		//std::cout << "feedback_seiko_steer_angle_deg = " << feedback_seiko_steer_angle_deg << std::endl;
		std::stringstream enc_fb_angle;
		enc_fb_angle << feedback_seiko_steer_angle_deg;
		label_seiko_enc_fb_deg->setText(QString::fromStdString(enc_fb_angle.str()));
	}
}

void smctrlmapviewerTab::handle_button_smhome()
{
	 sm_motor_spinbox->setValue(0);
	 //send command to motor after steer home centering
	if(status_center_steering_home)
	{
		sptr_SmartMotorSteer->set_pallet_truck_desired_steer_angle_motor_pos(0.0);
	}
}

QGroupBox *smctrlmapviewerTab::createOne_smctrl()
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
	 QVBoxLayout *vbox = new QVBoxLayout;
	 vbox->addLayout(hlayout_spinbox_smhome);
	 vbox->addLayout(hlayout_button_smhome);
	 vbox->addLayout(hlayout_steer_ang);
	 vbox->addLayout(hlayout_seiko);
	 vbox->addLayout(hlayout_seiko_angle);
	 groupsmctrlBox->setLayout(vbox);
	 return groupsmctrlBox;
}

smctrlmapviewerTab::~smctrlmapviewerTab()
{

}

}
