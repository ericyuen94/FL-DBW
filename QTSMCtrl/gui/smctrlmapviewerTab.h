/**
 *  @file    smctrlmapviewerTab.h
 *  @author  Emily Low
 *  @date    26th July 2015
 *  @version 1.0.0
 *
 *  @brief controller for vmap viewer
 *
 *  @section DESCRIPTION
 *
 */

#ifndef smctrlmapviewerTab_H
#define smctrlmapviewerTab_H

#include <QtWidgets>
#include <QSlider>
#include <QPushButton>
#include "GetSmartMotorSteerConfiguration.h"
//
#include <memory>
//
#include "PCMPalletController.h"
#include "PcanSeikoPcanEncoder.h"
#include "DBW_Ctrl.h"
//
#include "Common/Utility/TimeUtils.h"

namespace smctrlEmulator
{


class smctrlmapviewerTab : public QWidget
{

Q_OBJECT

public:

	smctrlmapviewerTab(QWidget *parent = 0);

	~smctrlmapviewerTab();

 private slots:

 	void handle_SteerMotorCmd_job();

 	void handle_SeikoGetEncoderSteer_job();

 	void handle_button_smhome();

private:

 	 //related to QT
	 void initsmctrlmapviewerTab();

	 QGroupBox *createOne_smctrl();

private:

	 //timer
	 QTimer checkSteerMotorTimer;
	 QTimer checkSteerEncoderTimer;

	 //
	 QDoubleSpinBox *sm_motor_spinbox;
	 QPushButton *button_smhome;
	 QLabel *label_desired_steer_angle_cmd_deg;
	 QLabel *label_seiko_enc;
	 QLabel *label_seiko_enc_fb_deg;

	 //
	 DBWPalletCommon::GetSmartMotorSteerConfiguration cGetSteerMotorDBWConfiguration;
	 DBWPalletCommon::SmartMotorSteerConfig params_dbw_config;

	 //
	 std::shared_ptr<DBWPalletCommon::PCMPalletController> sptr_cPCMPalletController; //drive controller
	 std::shared_ptr<DBWPalletCommon::PcanSeikoPcanEncoder> sptr_cPcanSeikoPcanEncoder; //steer encoder
	 std::shared_ptr<DBW_Ctrl> sptr_SmartMotorSteer; //steer motor

	 //
	 float32_t max_steer_angle;

	 //
	 uint32_t current_seiko_steer_enc_pos;
	 uint64_t last_rx_timestamp_seiko_steer_enc_pos;
	 float64_t feedback_seiko_steer_angle;

	 //
	 bool status_center_steering_home;


};


}
#endif
