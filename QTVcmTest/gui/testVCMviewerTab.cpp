/**
 *  @file    testVCMviewerTab.cpp
 *  @author  Emily Low
 *  @date    26th July 2015
 *  @version 1.0.0
 *
 *  @brief controller for vmap viewer
 *
 *  @section DESCRIPTION
 *
 */

#include "testVCMviewerTab.h"
#include "Common/Utility/TimeUtils.h"
#include "Platform/PlatformDefinition.h"
//

namespace QtTestVCM
{

testVCMviewerTab::testVCMviewerTab(QWidget *parent)
: QWidget(parent)
{
	inittestVCMviewerTab();
}

void testVCMviewerTab::inittestVCMviewerTab()
{
	//set main layout.
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(createTwo_PalletBaseMsg());
	mainLayout->addWidget(createFour_PalletVCMCmdMsgInputs());

	mainLayout->addStretch(1);
	//
	//set timer job.
	checkCheckBoxStatusTimer.setInterval(50); //10Hz
	connect(&checkCheckBoxStatusTimer, SIGNAL (timeout()), this, SLOT (handle_checkCheckBoxStatusTimer_job()));
	checkCheckBoxStatusTimer.start();
	//
	setLayout(mainLayout);

	//set publisher
	ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
	std::vector<std::string> interface_list;
	interface_list.push_back("PalletBaseCmd");
	ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);

	//
	//setup VCM stkci publisher
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


	seq_key_tracker = 0;
	bEStop_Activated = false;
}

void testVCMviewerTab::handle_button_SW_ESTOP()
{
	if(bEStop_Activated)
	{
		bEStop_Activated = false;
		QPalette pal_green = button_SW_ESTOP->palette();
		pal_green.setColor(QPalette::Button, QColor(Qt::green));
		button_SW_ESTOP->setAutoFillBackground(true);
		button_SW_ESTOP->setPalette(pal_green);
		button_SW_ESTOP->update();
		checkBox_PalletBaseMsg[BaseSensor::trigger_valve]->setChecked(true);
		doubleSpinBox_dbw_brake->setValue(0.0); //release brake
	}
	else
	{
		bEStop_Activated = true;
		QPalette pal_red = button_SW_ESTOP->palette();
		pal_red.setColor(QPalette::Button, QColor(Qt::red));
		button_SW_ESTOP->setAutoFillBackground(true);
		button_SW_ESTOP->setPalette(pal_red);
		button_SW_ESTOP->update();
		checkBox_PalletBaseMsg[BaseSensor::trigger_valve]->setChecked(false);
		doubleSpinBox_dbw_brake->setValue(1.0); //full brake
		doubleSpinBox_dbw_cmd_speed->setValue(0.0); //zero speed
		doubleSpinBox_dbw_cmd_steer->setValue(0.0); //home steer
	}

}

void testVCMviewerTab::handle_checkCheckBoxStatusTimer_job()
{
	std::cout << "Running Path Follower Trigger" << std::endl;	

	//--------------------------------------------
	//Fill up the PalletBaseCmdMsg message
	//--------------------------------------------
	bool bValveEnabled = false;
	Platform::Sensors::PalletBaseCmdMsg out_data_BaseCmdMsg;
	out_data_BaseCmdMsg.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	if(checkBox_PalletBaseMsg[BaseSensor::trigger_valve]->isChecked()==1)
	{
		out_data_BaseCmdMsg.trigger_tiller=0x01;
		bValveEnabled = true;
	}
	else
	{
		out_data_BaseCmdMsg.trigger_tiller=0x00;
		bValveEnabled = false;
	}

	if((radio_PalletControl[ValveControl::neutral]->isChecked()==1) && bValveEnabled==true)
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
	}

	if((radio_PalletControl[ValveControl::forklift_fork_lower]->isChecked()==1) && bValveEnabled==true)
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x02;
	}

	if((radio_PalletControl[ValveControl::forklift_fork_raise]->isChecked()==1) && bValveEnabled==true)
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x04;
	}

	if((radio_PalletControl[ValveControl::forklift_fork_tiltfwd]->isChecked()==1) && bValveEnabled==true)
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x10;
	}

	if((radio_PalletControl[ValveControl::forklift_fork_tiltbk]->isChecked()==1) && bValveEnabled==true)
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x20;
	}

	if((radio_PalletControl[ValveControl::forklift_fork_moveright]->isChecked()==1) && bValveEnabled==true)
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x40;
	}

	if((radio_PalletControl[ValveControl::forklift_fork_moveleft]->isChecked()==1) && bValveEnabled==true)
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x80;
	}

	if((radio_PalletControl[ValveControl::forklift_steer]->isChecked()==1) && bValveEnabled==true)
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x08;
	}

	out_data_BaseCmdMsg.trigger_slowspeed = doubleSpinBox_dbw_brake->value();

	printf("out_data_BaseCmdMsg.trigger_tiller = 0x%X\n", out_data_BaseCmdMsg.trigger_tiller);
	printf("out_data_BaseCmdMsg.trigger_tiller = %f\n", out_data_BaseCmdMsg.trigger_slowspeed);

	out_data_BaseCmdMsg.seq_key = seq_key_tracker;
	//ptr_stkci_pub->PubPalletBaseCmd("PalletBehavior", out_data_BaseCmdMsg);

	
	//Publish VCM commands
	PublishVCMCmds(doubleSpinBox_dbw_cmd_speed->value(),doubleSpinBox_dbw_cmd_steer->value());
	//
	seq_key_tracker++;
	if(seq_key_tracker>UINT16_MAX)
	{
		seq_key_tracker=0;
	}
}

void testVCMviewerTab::PublishVCMCmds(const float64_t commandedVelocity, const float64_t commandedSteer)
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

QGroupBox *testVCMviewerTab::createFour_PalletVCMCmdMsgInputs()
{
	 QGroupBox *spinBoxesGroup = new QGroupBox(tr("DBW Cmds"));;

	 //
	 QLabel *doubleSpinBox_dbw_max_speed_label = new QLabel(tr("Max speed (m/s)"));
	 doubleSpinBox_dbw_max_speed = new QDoubleSpinBox();
	 doubleSpinBox_dbw_max_speed->setRange(-2.76, 2.76);
	 doubleSpinBox_dbw_max_speed->setSingleStep(0.2);
	 doubleSpinBox_dbw_max_speed->setValue(0);
	 //
	 QLabel *doubleSpinBox_dbw_max_steer_label = new QLabel(tr("Max steer (deg)"));
	 doubleSpinBox_dbw_max_steer = new QDoubleSpinBox();
	 doubleSpinBox_dbw_max_steer->setRange(-90, 90);
	 doubleSpinBox_dbw_max_steer->setSingleStep(0.5);
	 doubleSpinBox_dbw_max_steer->setValue(0);
	 //
	 QLabel *doubleSpinBox_dbw_cmd_speed_label = new QLabel(tr("Cmd speed percent"));
	 doubleSpinBox_dbw_cmd_speed = new QDoubleSpinBox();
	 doubleSpinBox_dbw_cmd_speed->setRange(-0.4, 0.4);
	 doubleSpinBox_dbw_cmd_speed->setSingleStep(0.1);
	 doubleSpinBox_dbw_cmd_speed->setValue(0);
	 //
	 QLabel *doubleSpinBox_dbw_cmd_steer_label = new QLabel(tr("Cmd steer percent"));
	 doubleSpinBox_dbw_cmd_steer = new QDoubleSpinBox();
	 doubleSpinBox_dbw_cmd_steer->setRange(-1, 1);
	 doubleSpinBox_dbw_cmd_steer->setSingleStep(0.1);
	 doubleSpinBox_dbw_cmd_steer->setValue(0);
	 //
	 //
	 QLabel *doubleSpinBox_dbw_brake_label = new QLabel(tr("Cmd brake percent"));
	 doubleSpinBox_dbw_brake = new QDoubleSpinBox();
	 doubleSpinBox_dbw_brake->setRange(0, 1);
	 doubleSpinBox_dbw_brake->setSingleStep(0.2);
	 doubleSpinBox_dbw_brake->setValue(0);
	 //
	 QHBoxLayout *spinBoxLayout_maxspeed = new QHBoxLayout;
	 spinBoxLayout_maxspeed->addWidget(doubleSpinBox_dbw_max_speed_label);
	 spinBoxLayout_maxspeed->addWidget(doubleSpinBox_dbw_max_speed);
	 QHBoxLayout *spinBoxLayout_maxsteer = new QHBoxLayout;
	 spinBoxLayout_maxsteer->addWidget(doubleSpinBox_dbw_max_steer_label);
	 spinBoxLayout_maxsteer->addWidget(doubleSpinBox_dbw_max_steer);
	 QHBoxLayout *spinBoxLayout_cmdspeed = new QHBoxLayout;
	 spinBoxLayout_cmdspeed->addWidget(doubleSpinBox_dbw_cmd_speed_label);
	 spinBoxLayout_cmdspeed->addWidget(doubleSpinBox_dbw_cmd_speed);
	 QHBoxLayout *spinBoxLayout_cmdsteer = new QHBoxLayout;
	 spinBoxLayout_cmdsteer->addWidget(doubleSpinBox_dbw_cmd_steer_label);
	 spinBoxLayout_cmdsteer->addWidget(doubleSpinBox_dbw_cmd_steer);
	 //
	 //
	 QHBoxLayout *spinBoxLayout_cmdbrake = new QHBoxLayout;
	 spinBoxLayout_cmdbrake->addWidget(doubleSpinBox_dbw_brake_label);
	 spinBoxLayout_cmdbrake->addWidget(doubleSpinBox_dbw_brake);


	 //
	 QVBoxLayout *spinBoxLayout = new QVBoxLayout;
	 spinBoxLayout->addLayout(spinBoxLayout_maxspeed);
	 spinBoxLayout->addLayout(spinBoxLayout_maxsteer);
	 spinBoxLayout->addLayout(spinBoxLayout_cmdspeed);
	 spinBoxLayout->addLayout(spinBoxLayout_cmdsteer);
	 //
	 spinBoxLayout->addLayout(spinBoxLayout_cmdbrake);

	 //
	 spinBoxesGroup->setLayout(spinBoxLayout);
	 return spinBoxesGroup;
}

QGroupBox *testVCMviewerTab::createTwo_PalletBaseMsg()
{
	//Main Grp
	QGroupBox *checkBoxGrp_PalletBaseMsgs;
	checkBoxGrp_PalletBaseMsgs = new QGroupBox(tr("Valve Controls"));

	//Feedback
	checkBox_PalletBaseMsg[BaseSensor::collision_sensor_left] = new QCheckBox(tr("LeftCollision"));
	checkBox_PalletBaseMsg[BaseSensor::collision_sensor_left]->setChecked(false);
	checkBox_PalletBaseMsg[BaseSensor::collision_sensor_right] = new QCheckBox(tr("RightCollision"));
	checkBox_PalletBaseMsg[BaseSensor::collision_sensor_right]->setChecked(false);
	checkBox_PalletBaseMsg[BaseSensor::engagement_sensor_left] = new QCheckBox(tr("LeftEngage"));
	checkBox_PalletBaseMsg[BaseSensor::engagement_sensor_left]->setChecked(false);
	checkBox_PalletBaseMsg[BaseSensor::engagement_sensor_right] = new QCheckBox(tr("RightEngage"));
	checkBox_PalletBaseMsg[BaseSensor::engagement_sensor_right]->setChecked(false);
	checkBox_PalletBaseMsg[BaseSensor::height_sensor_top] = new QCheckBox(tr("TopHeight"));
	checkBox_PalletBaseMsg[BaseSensor::height_sensor_top]->setChecked(false);
	checkBox_PalletBaseMsg[BaseSensor::height_sensor_base] = new QCheckBox(tr("BaseHeight"));
	checkBox_PalletBaseMsg[BaseSensor::height_sensor_base]->setChecked(false);
	//-----
	QHBoxLayout *hlayout_Feedback_Base = new QHBoxLayout;
	hlayout_Feedback_Base->addWidget(checkBox_PalletBaseMsg[BaseSensor::collision_sensor_left]);
	hlayout_Feedback_Base->addWidget(checkBox_PalletBaseMsg[BaseSensor::collision_sensor_right]);
	hlayout_Feedback_Base->addWidget(checkBox_PalletBaseMsg[BaseSensor::engagement_sensor_left]);
	hlayout_Feedback_Base->addWidget(checkBox_PalletBaseMsg[BaseSensor::engagement_sensor_right]);
	hlayout_Feedback_Base->addWidget(checkBox_PalletBaseMsg[BaseSensor::height_sensor_top]);
	hlayout_Feedback_Base->addWidget(checkBox_PalletBaseMsg[BaseSensor::height_sensor_base]);

//	//Commands
//	checkBox_PalletBaseMsg[BaseSensor::trigger_tiller] = new QCheckBox(tr("On Tiller"));
//	checkBox_PalletBaseMsg[BaseSensor::trigger_tiller]->setChecked(false);
//	checkBox_PalletBaseMsg[BaseSensor::trigger_lower_fork] = new QCheckBox(tr("On LowerFork"));
//	checkBox_PalletBaseMsg[BaseSensor::trigger_lower_fork]->setChecked(false);
//	checkBox_PalletBaseMsg[BaseSensor::trigger_raise_fork] = new QCheckBox(tr("On RaiseFork"));
//	checkBox_PalletBaseMsg[BaseSensor::trigger_raise_fork]->setChecked(false);
//	checkBox_PalletBaseMsg[BaseSensor::trigger_sound_buzzer] = new QCheckBox(tr("On ReverseBuzzer"));
//	checkBox_PalletBaseMsg[BaseSensor::trigger_sound_buzzer]->setChecked(false);
//	//-----
//	QHBoxLayout *hlayout_Commands_Base = new QHBoxLayout;
//	hlayout_Commands_Base->addWidget(checkBox_PalletBaseMsg[BaseSensor::trigger_tiller]);
//	hlayout_Commands_Base->addWidget(checkBox_PalletBaseMsg[BaseSensor::trigger_lower_fork]);
//	hlayout_Commands_Base->addWidget(checkBox_PalletBaseMsg[BaseSensor::trigger_raise_fork]);
//	hlayout_Commands_Base->addWidget(checkBox_PalletBaseMsg[BaseSensor::trigger_sound_buzzer]);

	//Valve Activation
	checkBox_PalletBaseMsg[BaseSensor::trigger_valve] = new QCheckBox(tr("Activate Valves"));
	checkBox_PalletBaseMsg[BaseSensor::trigger_valve]->setChecked(true);
	//--
	QHBoxLayout *hlayout_ValveEnable = new QHBoxLayout;
	hlayout_ValveEnable->addWidget(checkBox_PalletBaseMsg[BaseSensor::trigger_valve]);

	//Valve Controls
	radio_PalletControl[ValveControl::neutral] = new QRadioButton(tr("Neutral"));
	radio_PalletControl[ValveControl::neutral]->setChecked(false);
	radio_PalletControl[ValveControl::forklift_fork_raise] = new QRadioButton(tr("Fork Raise"));
	radio_PalletControl[ValveControl::forklift_fork_raise]->setChecked(false);
	radio_PalletControl[ValveControl::forklift_fork_lower] = new QRadioButton(tr("Fork Lower"));
	radio_PalletControl[ValveControl::forklift_fork_lower]->setChecked(false);
	radio_PalletControl[ValveControl::forklift_fork_moveleft] = new QRadioButton(tr("Fork Move Left"));
	radio_PalletControl[ValveControl::forklift_fork_moveleft]->setChecked(false);
	radio_PalletControl[ValveControl::forklift_fork_moveright] = new QRadioButton(tr("Fork Move Right"));
	radio_PalletControl[ValveControl::forklift_fork_moveright]->setChecked(false);
	radio_PalletControl[ValveControl::forklift_fork_tiltfwd] = new QRadioButton(tr("Fork TiltFwd"));
	radio_PalletControl[ValveControl::forklift_fork_tiltfwd]->setChecked(false);
	radio_PalletControl[ValveControl::forklift_fork_tiltbk] = new QRadioButton(tr("Fork TiltBk"));
	radio_PalletControl[ValveControl::forklift_fork_tiltbk]->setChecked(false);
	radio_PalletControl[ValveControl::forklift_steer] = new QRadioButton(tr("Steer Control"));
	radio_PalletControl[ValveControl::forklift_steer]->setChecked(false);
	//--
	QHBoxLayout *hlayout_ForkControl1 = new QHBoxLayout;
	hlayout_ForkControl1->addWidget(radio_PalletControl[ValveControl::neutral]);
	//--
	QHBoxLayout *hlayout_ForkControl2 = new QHBoxLayout;
	hlayout_ForkControl2->addWidget(radio_PalletControl[ValveControl::forklift_fork_lower]);
	hlayout_ForkControl2->addWidget(radio_PalletControl[ValveControl::forklift_fork_raise]);
	//--
	QHBoxLayout *hlayout_ForkControl3 = new QHBoxLayout;
	hlayout_ForkControl3->addWidget(radio_PalletControl[ValveControl::forklift_fork_tiltfwd]);
	hlayout_ForkControl3->addWidget(radio_PalletControl[ValveControl::forklift_fork_tiltbk]);
	//--
	QHBoxLayout *hlayout_ForkControl4 = new QHBoxLayout;
	hlayout_ForkControl4->addWidget(radio_PalletControl[ValveControl::forklift_fork_moveleft]);
	hlayout_ForkControl4->addWidget(radio_PalletControl[ValveControl::forklift_fork_moveright]);

	QHBoxLayout *hlayout_SteerControl = new QHBoxLayout;
	hlayout_SteerControl->addWidget(radio_PalletControl[ValveControl::forklift_steer]);

	//
	button_SW_ESTOP = new QPushButton(tr("SW ESTOP"));
	button_SW_ESTOP->setMaximumWidth(150);
	connect(button_SW_ESTOP, SIGNAL (clicked()), this, SLOT (handle_button_SW_ESTOP())); //events.
	QPalette pal = button_SW_ESTOP->palette();
	pal.setColor(QPalette::Button, QColor(Qt::green));
	button_SW_ESTOP->setAutoFillBackground(true);
	button_SW_ESTOP->setPalette(pal);
	button_SW_ESTOP->update();

	//Combine
	QVBoxLayout *hlayout_Combined = new QVBoxLayout;
	//hlayout_Combined->addLayout(hlayout_Feedback_Base);
	hlayout_Combined->addLayout(hlayout_ValveEnable);
	hlayout_Combined->addLayout(hlayout_ForkControl1);
	hlayout_Combined->addLayout(hlayout_ForkControl2);
	hlayout_Combined->addLayout(hlayout_ForkControl3);
	hlayout_Combined->addLayout(hlayout_ForkControl4);
	hlayout_Combined->addLayout(hlayout_SteerControl);
	hlayout_Combined->addWidget(button_SW_ESTOP);

	//
	checkBoxGrp_PalletBaseMsgs->setLayout(hlayout_Combined);
	//
	return checkBoxGrp_PalletBaseMsgs;
}

testVCMviewerTab::~testVCMviewerTab()
{

}

}
