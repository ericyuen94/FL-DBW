/**
 *  @file    testVCMviewerTab.h
 *  @author  Emily Low
 *  @date    26th July 2015
 *  @version 1.0.0
 *
 *  @brief controller for vmap viewer
 *
 *  @section DESCRIPTION
 *
 */

#ifndef testVCMviewerTab_H
#define testVCMviewerTab_H

#include <QtWidgets>
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "Interfaces/VcmCmdMsgPubInterface.h"
#include "Platform/PlatformConstants.h"
#include "Common/Utility/configFile.h"
//
namespace QtTestVCM
{

const int32_t TOTAL_PALLET_AUX_SENSOR=6;
const int32_t TOTAL_PALLET_BASE_SENSOR=11;
const int32_t TOTAL_PALLET_FORK_CONTROL=8;

enum AuxSensor
{
	manned_switch=0,
	hw_estop,
	trigger_warning_light,
	trigger_left_direction_light,
	trigger_right_direction_light,
	trigger_buzzer
};

enum BaseSensor
{
	batterylevel=0,
	collision_sensor_left,
	collision_sensor_right,
	engagement_sensor_left,
	engagement_sensor_right,
	height_sensor_top,
	height_sensor_base,
	trigger_valve,
	trigger_lower_fork,
	trigger_raise_fork,
	trigger_sound_buzzer,
};

enum ValveControl
{
	neutral = 0,
	forklift_fork_raise,
	forklift_fork_lower,
	forklift_fork_moveleft,
	forklift_fork_moveright,
	forklift_fork_tiltfwd,
	forklift_fork_tiltbk,
	forklift_steer
};

class testVCMviewerTab : public QWidget
{

Q_OBJECT

public:

	testVCMviewerTab(QWidget *parent = 0);

	~testVCMviewerTab();

 private slots:

 	void handle_checkCheckBoxStatusTimer_job();

	void handle_button_SW_ESTOP(); //edit SW ESTOP


private:

 	 //related to QT
	 void inittestVCMviewerTab();

	 QGroupBox *createTwo_PalletBaseMsg();

	 QGroupBox *createFour_PalletVCMCmdMsgInputs();

	 void PublishVCMCmds(const float64_t commandedVelocity, const float64_t commandedSteer);
	

private:

	 //timer
	 QTimer checkCheckBoxStatusTimer;

	 //Spinboxes
	 QDoubleSpinBox *doubleSpinBox_batterylevel;
	 QDoubleSpinBox *doubleSpinBox_dbw_max_speed;
	 QDoubleSpinBox *doubleSpinBox_dbw_max_steer;
	 QDoubleSpinBox *doubleSpinBox_dbw_cmd_speed;
	 QDoubleSpinBox *doubleSpinBox_dbw_cmd_steer;
	 //
	 QDoubleSpinBox *doubleSpinBox_dbw_brake;
	 //
//	 QDoubleSpinBox *doubleSpinBox_dbw_fb_speed;
//	 QDoubleSpinBox *doubleSpinBox_dbw_fb_steer;
//	 QSpinBox *SpinBox_dbw_fb_left_drive_encoder_count;
//	 QSpinBox *SpinBox_dbw_fb_right_drive_encoder_count;
//	 QSpinBox *SpinBox_dbw_fb_steer_encoder_count;
//	 QSpinBox *SpinBox_dbw_fb_drive_motor_status;
//	 QSpinBox *SpinBox_dbw_fb_steer_motor_status;
//	 QDoubleSpinBox *doubleSpinBox_drive_motor_temp;
//	 QDoubleSpinBox *doubleSpinBox_steer_motor_temp;

	 //checkbox
	 QCheckBox *checkBox_PalletBaseMsg[TOTAL_PALLET_BASE_SENSOR];

	 //radiobutton
	 QRadioButton *radio_PalletControl[TOTAL_PALLET_FORK_CONTROL];

	 //push btn
	 QPushButton *button_SW_ESTOP;
	 bool bEStop_Activated;

	 //stkci pub data
	 std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;
	 uint16_t seq_key_tracker;

	 //Setup VCM stkci pub
	 STKCI::VcmCmdMsg stkci_data_vcm;
	 std::shared_ptr<Common::Utility::configFile> sptr_configfile;
	 STKCI::VcmCmdMsgPubInterface stkci_pub_vcm;
	 std::string relative_path_senderid_config;
	 std::string	interface_config_file;
};


}
#endif
