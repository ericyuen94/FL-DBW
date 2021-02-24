/**
 *  @file    QTVcmTrajectoryTab.cpp
 *  @author  Emily Low
 *  @date    26th July 2015
 *  @version 1.0.0
 *
 *  @brief controller for vmap viewer
 *
 *  @section DESCRIPTION
 *
 */

#include "QTVcmTrajectoryTab.h"
#include "Common/Utility/TimeUtils.h"
#include "Platform/PlatformDefinition.h"
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "Interfaces/VcmCmdMsgPubInterface.h"
//
#include <cmath>

namespace vcmtrajdemo
{

QTVcmTrajectoryTab::QTVcmTrajectoryTab(QWidget *parent)
: QWidget(parent)
{
	initQTVcmTrajectoryTab();

	open_read_file_status = false;
	start_traj_log_status = false;
	start_traj_logplay_status = false;
	start_motionprofile_status = false;
	readEOF_status = false;
}

void QTVcmTrajectoryTab::initQTVcmTrajectoryTab()
{
	//
	sptr_cGetVcmTrajConfiguration = std::make_shared<GetVcmTrajConfiguration>();
	if(!sptr_cGetVcmTrajConfiguration->OpenVcmTrajConfiguration())
	{
		std::cout << "Error read QTVcmTrajectory XML file " << std::endl;
		exit(1);
	}
	else
	{
		//Get the configuration data.
		sptr_cGetVcmTrajConfiguration->GetVcmTrajConfigData(params_VcmTrajConfig);
		sptr_cGetVcmTrajConfiguration->PrintVcmTrajConfigData(params_VcmTrajConfig);
		max_speed = params_VcmTrajConfig.max_linear_speed;
		max_steer = params_VcmTrajConfig.max_steer;
	}

	//init stkci messages
	sptr_RetrieveVehicleMeasure_platform = std::make_shared<RetrieveVehicleMeasure>(params_VcmTrajConfig.StkciSenderId.platform_vehiclemeasure_senderid);

	//setup stkci publisher
	relative_path_senderid_config = "../config/swmodules/comms/";
	std::string sender_id_file_location;
	sender_id_file_location.append(relative_path_senderid_config);
	sender_id_file_location.append(Platform::UGV_SENDER_ID_FILE);
	sptr_configfile = std::make_shared<Common::Utility::configFile>();
	if(!sptr_configfile->OpenFile(sender_id_file_location) == 0)
	{
		std::cerr << "Cannot open sender ID data configuration\n";
		exit(1);
	}
	//setup publisher
	interface_config_file = Platform::UGV_INTERFACE_FILE;
	stkci_pub_vcm.setup(relative_path_senderid_config + interface_config_file,"VCMCommand");

	//set publisher
	ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
	std::vector<std::string> interface_list;
	interface_list.push_back("PalletBaseCmd");
	ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);

	//set main layout.
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(createOne_log_trajdemo());
	mainLayout->addStretch(1);

	//
	//set trajectory log
	logTrajTimer.setInterval(40); //25Hz
	connect(&logTrajTimer, SIGNAL (timeout()), this, SLOT (handle_timer_traj_log_job()));
	logTrajTimer.start();

	//set player trajectory log as path planner command
	playlogTrajTimer.setInterval(100); //10Hz
	connect(&playlogTrajTimer, SIGNAL (timeout()), this, SLOT (handle_timer_traj_player_job()));
	playlogTrajTimer.start();

	//
	setLayout(mainLayout);
}

void QTVcmTrajectoryTab::handle_timer_traj_log_job()
{
	vcmtraj_data log_data;
	sptr_RetrieveVehicleMeasure_platform->GetVehicleMeasureData(output_vehmeas_stkci_msg);

	//TODO:: to subscribe to vehicle measure information.
	if(start_traj_log_status)
	{
		for(std::list<Platform::Sensors::GeometryMsgsVehicleMeasure>::iterator it=output_vehmeas_stkci_msg.begin();
					it!=output_vehmeas_stkci_msg.end(); ++it)
		{
			log_data.timestamp = it->timestamp;
			log_data.speed = it->speed;
			log_data.steering = it->steering;
			log_data.yaw_rate = it->yaw_rate;
			sptr_cVcmTrajLog->WriteLog(log_data);
			std::cout << "Logging in progress " << std::endl;
		}//for
	}//if
}

void QTVcmTrajectoryTab::handle_timer_traj_player_job()
{
	//minic as a path follower command
	vcmtraj_data current_data;

	//
	if(start_traj_logplay_status == true && readEOF_status == false)
	{
		if(sptr_cVcmTrajRead->ReadDataLogger(current_data)>0)
		{
			//PublishVCMCmds(current_data.speed,current_data.steering); //at current is percentage
			PublishVCMCmds(params_VcmTrajConfig.max_linear_speed,current_data.steering); //at speed is a percentage, steer is angle in radians
//			if(fabs(current_data.steering)>0.01)
//			{
//				PublishVCMCmds(params_VcmTrajConfig.max_linear_speed,current_data.steering); //at speed is a percentage, steer is angle in radians
//			}
//			else
//			{
//				PublishVCMCmds(0.0,current_data.steering); //at speed is a percentage, steer is angle in radians
//			}
		}
		else
		{
			readEOF_status = true;
			std::cout << "Stop reading from data file " << std::endl;
		}
	}//
}

void QTVcmTrajectoryTab::handle_pushbtn_start_traj_log_job()
{
	if(start_traj_log_status == false && params_VcmTrajConfig.VcmCalibLog.enabled==1)
	{
		sptr_cVcmTrajLog = std::make_shared<VcmTrajLog>();
		sptr_cVcmTrajLog->SetupVcmTrajLog(params_VcmTrajConfig.VcmCalibLog.path);
		button_log_traj->setText("Stop Log");
		start_traj_log_status = true;
	}
	else if(start_traj_log_status == true && params_VcmTrajConfig.VcmCalibLog.enabled==1)
	{
		sptr_cVcmTrajLog->CloseDataLogger();
		sptr_cVcmTrajLog.reset();
		button_log_traj->setText("Start Log");
		button_log_traj->setEnabled(false);
		start_traj_log_status = false;
		std::cout << "Stop data logging done !" << std::endl;
	}
}

void QTVcmTrajectoryTab::handle_pushbtn_playback_traj_log_job()
{
	if(start_traj_logplay_status == false)
	{
		start_traj_logplay_status = true;
		sptr_cVcmTrajRead = std::make_shared<VcmTrajRead>();
		sptr_cVcmTrajRead->SetupVcmTrajRead(params_VcmTrajConfig.VcmCalibLog.path);
		readEOF_status = false;
		button_playback_traj->setText("Stop");
		std::cout << " ========================== " << std::endl;
	}
	else
	{
		sptr_cVcmTrajRead->CloseReadLog();
		sptr_cVcmTrajRead.reset();
		start_traj_logplay_status = false;
		button_playback_traj->setText("Start");
		std::cout << " " << std::endl;
	}
}

void QTVcmTrajectoryTab::handle_pushbtn_play_fixedProfile()
{
	if(start_motionprofile_status == false)
	{
		start_motionprofile_status = true;
		readEOF_status = false;
		button_playback_fixedprofile->setText("Stop");
		playback_profile_one();
		//playback_profile_dhl();
	}
	else
	{
		start_motionprofile_status = false;
		button_playback_fixedprofile->setText("Start");
		std::cout << " " << std::endl;
	}
}

void QTVcmTrajectoryTab::playback_profile_one()
{
	//X seconds of
	int32_t duration_one_seconds;
	duration_one_seconds = static_cast<int32_t>(1.0/0.02);
	int32_t duration_three_seconds;
	duration_three_seconds = static_cast<int32_t>(3.0/0.02);
	int32_t duration_five_seconds;
	duration_five_seconds = static_cast<int32_t>(5.0/0.02);
	int32_t duration_ten_seconds;
	duration_ten_seconds = static_cast<int32_t>(10.0/0.02);
	int32_t duration_15_seconds;
	duration_15_seconds = static_cast<int32_t>(15.0/0.02);
	int32_t duration_7_seconds;
	duration_7_seconds = static_cast<int32_t>(7.0/0.02);

	float32_t linear_speed;
	linear_speed = 0.1;

	//backward motion
	//for(int32_t i=0; i<duration_7_seconds; i++)
	/*for(int32_t i=0; i<duration_three_seconds; i++)
	{
		PublishVCMCmds(-linear_speed,0.0);
		PubPalletBaseCmds(1,0,0,0);
		std::cout << "1/6 running backward motion " << i << " / " << duration_three_seconds << std::endl;
		usleep(20000);
	}

	//do a fake pickup
	for(int32_t i=0; i<duration_three_seconds; i++)
	{
		PublishVCMCmds(0,0.0); //0 degree
		PubPalletBaseCmds(1,0,1,0); //pickup
		std::cout << "2/6 running fake pickup " << i << " / " << duration_three_seconds << std::endl;
		usleep(20000);
	}*/

	// steering
	//for(int32_t i=0; i<duration_three_seconds; i++)

	float steer_angle = -20;
	for(int32_t i=0; i<duration_three_seconds; i++)
	{
		//PublishVCMCmds(linear_speed,0.0);
		PublishVCMCmds(0,steer_angle*(M_PI/180));
		//PubPalletBaseCmds(1,0,0,0);
		std::cout << "1/2 running steer motion " << i << " / duration_three_seconds" << std::endl;
		std::cout << "steering angle = " << steer_angle << "(degrees), " << steer_angle*(M_PI/180) << "(radians)" <<  std::endl;
		usleep(20000);
	}

	//forward motion
	for(int32_t i=0; i<9999999999; i++)
	{
		//PublishVCMCmds(linear_speed,0.0);
		PublishVCMCmds(0.2,steer_angle*(M_PI/180));
		//PubPalletBaseCmds(1,0,0,0);
		std::cout << "2/2 running forward motion " << i << " / " << 9999999999 << std::endl;
		usleep(20000);
	}

	//backward motion
	/*for(int32_t i=0; i<duration_7_seconds; i++)
	{
		PublishVCMCmds(-linear_speed,0.0);
		PubPalletBaseCmds(1,0,0,0);
		std::cout << "4/6 running backward motion " << i << " / " << duration_7_seconds << std::endl;
		usleep(20000);
	}

	//do a fake drop down
	for(int32_t i=0; i<duration_three_seconds; i++)
	{
		PublishVCMCmds(0,0.0); //0 degree
		PubPalletBaseCmds(1,1,0,0); //drop
		std::cout << "5/6 running fake drop down " << i << " / " << duration_three_seconds << std::endl;
		usleep(20000);
	}

	//forward motion
	for(int32_t i=0; i<duration_7_seconds; i++)
	{
		PublishVCMCmds(-linear_speed,0.0);
		PubPalletBaseCmds(1,0,0,0);
		std::cout << "6/6 running forward motion " << i << " / " << duration_7_seconds << std::endl;
		usleep(20000);
	}*/
}

/*
void QTVcmTrajectoryTab::playback_profile_one()
{
	//X seconds of
	int32_t duration_three_seconds;
	duration_three_seconds = static_cast<int32_t>(3.0/0.02);
	int32_t duration_five_seconds;
	duration_five_seconds = static_cast<int32_t>(5.0/0.02);
	int32_t duration_ten_seconds;
	duration_ten_seconds = static_cast<int32_t>(10.0/0.02);
	int32_t duration_15_seconds;
	duration_15_seconds = static_cast<int32_t>(15.0/0.02);
	int32_t duration_7_seconds;
	duration_7_seconds = static_cast<int32_t>(7.0/0.02);

	float32_t linear_speed;
	linear_speed = 0.1;

	//forward motion
	for(int32_t i=0; i<duration_7_seconds; i++)
	{
		PublishVCMCmds(linear_speed,0.0);
		PubPalletBaseCmds(1,0,0,0);
		std::cout << "running forward straight motion " << i << " / " << duration_five_seconds << std::endl;
		usleep(20000);
	}

	//forward right motion, max is 44 degrees at this moment
	for(int32_t i=0; i<duration_15_seconds; i++)
	{
		PublishVCMCmds(linear_speed,0.43625); //25 degrees in radians
		PubPalletBaseCmds(1,0,0,0);
		std::cout << "running turn right motion " << i << " / " << duration_ten_seconds << std::endl;
		usleep(20000);
	}

	//do a fake pickup
	for(int32_t i=0; i<duration_three_seconds; i++)
	{
		PublishVCMCmds(0,0.0); //0 degree
		PubPalletBaseCmds(1,0,1,0); //pickup
		std::cout << "running fake pickup " << i << " / " << duration_ten_seconds << std::endl;
		usleep(20000);
	}

	//do a fake drop down
	for(int32_t i=0; i<duration_three_seconds; i++)
	{
		PublishVCMCmds(0,0.0); //0 degree
		PubPalletBaseCmds(1,1,0,0); //drop
		std::cout << "running fake drop down " << i << " / " << duration_ten_seconds << std::endl;
		usleep(20000);
	}

	//reverse right motion, max is 44 degrees at this moment
	for(int32_t i=0; i<duration_15_seconds; i++)
	{
		PublishVCMCmds(-linear_speed,0.43625); //25 degrees in radians
		PubPalletBaseCmds(1,0,0,0);
		std::cout << "running reverse right motion " << i << " / " << duration_ten_seconds << std::endl;
		usleep(20000);
	}

	//reverse motion
	for(int32_t i=0; i<duration_five_seconds; i++)
	{
		PublishVCMCmds(-linear_speed,0.0);
		PubPalletBaseCmds(1,0,0,0);
		std::cout << "running reverse straight motion " << i << " / " << duration_five_seconds << std::endl;
		usleep(20000);
	}
}*/

void QTVcmTrajectoryTab::PublishVCMCmds(const float64_t commandedVelocity, const float64_t commandedSteer)
{
	//send pfm commands via stkci
	int16_t sender_id;
	sender_id = sptr_configfile->GetItem<int16_t>("CSCISenderID", "PathFollower");
	stkci_data_vcm.steer.mKey = 0;
	stkci_data_vcm.steer.mValue = commandedSteer; //steer is angle in radians
	stkci_data_vcm.velocity = commandedVelocity; //percentage -1 to 1
	stkci_data_vcm.header.msgID = 0;
	stkci_data_vcm.header.senderID = sender_id;
	stkci_data_vcm.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	stkci_pub_vcm.publish(stkci_data_vcm);
}

///\brief send the fork and tiller stop trigger via Platform::Sensors::PalletBaseCmdMsg message
///\param[in] trigger_tiller activated when 1 otherwise 0, drive when high
///\param[in] trigger_lower_fork activated when 1 otherwise 0
///\param[in] trigger_raise_fork activated when 1 otherwise 0
///\param[in] trigger_buzzer activated when 1 otherwise 0
void QTVcmTrajectoryTab::PubPalletBaseCmds(const int32_t trigger_tiller,
		const int32_t trigger_lower_fork,
		const int32_t trigger_raise_fork,
		const int32_t trigger_buzzer)
{
	Platform::Sensors::PalletBaseCmdMsg out_data_BaseCmdMsg;
	out_data_BaseCmdMsg.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	if(trigger_tiller>0)
	{
		out_data_BaseCmdMsg.trigger_tiller=0x01;
	}
	else
	{
		out_data_BaseCmdMsg.trigger_tiller=0x00;
	}
	if(trigger_lower_fork>0)
	{
		out_data_BaseCmdMsg.trigger_lower_fork=0x01;
	}
	else
	{
		out_data_BaseCmdMsg.trigger_lower_fork=0x00;
	}
	if(trigger_raise_fork>0)
	{
		out_data_BaseCmdMsg.trigger_raise_fork=0x01;
	}
	else
	{
		out_data_BaseCmdMsg.trigger_raise_fork=0x00;
	}
	if(trigger_buzzer>0)
	{
		out_data_BaseCmdMsg.trigger_buzzer=0x01;
	}
	else
	{
		out_data_BaseCmdMsg.trigger_buzzer=0x00;
	}
	out_data_BaseCmdMsg.seq_key = 0;

	//
	ptr_stkci_pub->PubPalletBaseCmd("DBW", out_data_BaseCmdMsg);
}

QGroupBox *QTVcmTrajectoryTab::createOne_log_trajdemo()
{
	 QGroupBox *grouplogtrajbox = new QGroupBox(tr("trajdemo"));

	 //
	 QLabel *pushbtn_logtraj_label = new QLabel(tr("Log Traj"));
	 button_log_traj = new QPushButton("Start Log");
	 QObject::connect(button_log_traj, SIGNAL(clicked()),this, SLOT(handle_pushbtn_start_traj_log_job()));
	 QHBoxLayout *hlayout_button_smhome = new QHBoxLayout;
	 hlayout_button_smhome->addWidget(pushbtn_logtraj_label);
	 hlayout_button_smhome->addWidget(button_log_traj);
	 if(params_VcmTrajConfig.VcmCalibLog.enabled==0)
	 {
		button_log_traj->setEnabled(false);
	 }

	 //
	 QLabel *pushbtn_playback_traj_label = new QLabel(tr("Play Log"));
	 button_playback_traj = new QPushButton("Start");
	 QObject::connect(button_playback_traj, SIGNAL(clicked()),this, SLOT(handle_pushbtn_playback_traj_log_job()));
	 QHBoxLayout *hlayout_button_playback = new QHBoxLayout;
	 hlayout_button_playback->addWidget(pushbtn_playback_traj_label);
	 hlayout_button_playback->addWidget(button_playback_traj);

	 //
	 QLabel *pushbtn_button_playback_fixedprofile = new QLabel(tr("Play Profile One"));
	 button_playback_fixedprofile = new QPushButton("Start");
	 QObject::connect(button_playback_fixedprofile, SIGNAL(clicked()),this, SLOT(handle_pushbtn_play_fixedProfile()));
	 QHBoxLayout *hlayout_button_fix_one = new QHBoxLayout;
	 hlayout_button_fix_one->addWidget(pushbtn_button_playback_fixedprofile);
	 hlayout_button_fix_one->addWidget(button_playback_fixedprofile);

	 //
	 QVBoxLayout *vbox = new QVBoxLayout;
	 //vbox->addLayout(hlayout_button_smhome);
	 //vbox->addLayout(hlayout_button_playback);
	 vbox->addLayout(hlayout_button_fix_one);
	 grouplogtrajbox->setLayout(vbox);
	 return grouplogtrajbox;
}

QTVcmTrajectoryTab::~QTVcmTrajectoryTab()
{

}

}
