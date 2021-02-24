/**
 *  @file    calibsteermapviewerTab.h
 *  @author  Emily Low
 *  @date    26th July 2015
 *  @version 1.0.0
 *
 *  @brief controller for vmap viewer
 *
 *  @section DESCRIPTION
 *
 */

#ifndef calibsteermapviewerTab_H
#define calibsteermapviewerTab_H

#include <QtWidgets>
#include <QSlider>
#include <QPushButton>
#include "GetSmartMotorSteerConfiguration.h"
//
#include <memory>
//
#include "GetCalibSteerConfiguration.h"
#include "PCMPalletController.h"
#include "DBW_Ctrl.h"
#include "LogCalibSteer.h"
//
#include "Common/Utility/TimeUtils.h"
//
#include "RetrieveVehicleMeasure.h"
#include "RetrieveCovPoseMsg.h"

namespace calibsteerEmulator
{

class calibsteermapviewerTab : public QWidget
{

Q_OBJECT

public:

	calibsteermapviewerTab(QWidget *parent = 0);

	~calibsteermapviewerTab();

 private slots:

 	void handle_SteerMotorCmd_job();

 	void handle_SeikoGetEncoderSteer_job();

 	void handle_PCMDriveCmd_job();

 	void handle_PCMFeedback_job();

 	void handle_button_start_move();

 	void handle_button_smhome();

private:

 	 //related to QT
	 void initcalibsteermapviewerTab();

	 QGroupBox *createOne_calibsteer();

	 void collate_steer_calib_logged_data();

private:

	 //timer
	 QTimer checkSteerMotorTimer;
	 QTimer checkPcmDriveTimer;
	 QTimer checkSteerEncoderTimer;
	 QTimer checkPCMFeedbackTimer;

	 //
	 QDoubleSpinBox *pcm_linear_speed_spinbox;
	 QDoubleSpinBox *sm_motor_spinbox;
	 QPushButton *button_smhome;
	 QPushButton *button_start_move;
	 QLabel *label_desired_steer_angle_cmd_deg;
	 QLabel *label_seiko_enc;
	 QLabel *label_seiko_enc_fb_deg;
	 QLabel *label_platform_battery_level;

	 //
	 std::shared_ptr<GetCalibSteerConfiguration> sptr_cGetCalibSteerConfiguration; //calib steer csci config
	 std::shared_ptr<DBWPalletCommon::GetSmartMotorSteerConfiguration> sptr_cGetSmartMotorSteerConfiguration; //smart motor steer csci config
	 std::shared_ptr<DBWPalletCommon::PCMPalletController> sptr_cPCMPalletController; //drive controller
	 std::shared_ptr<DBW_Ctrl> sptr_SmartMotorSteer; //steer motor
	 std::shared_ptr<LogCalibSteer> sptr_LogCalibSteer; //logger

	 //
	 CalibSteerConfig params_calib_steer_config;
	 DBWPalletCommon::SmartMotorSteerConfig params_dbw_config;

	 //
	 float32_t max_steer_angle;

	 //
	 uint32_t current_seiko_steer_enc_pos;
	 float64_t feedback_seiko_steer_angle;
	 uint64_t last_rx_timestamp_seiko_steer_enc_pos;

	 //
	 std::mutex mutex_data_pcm_fb;
	 DBWPalletCommon::pcm_palletbase_feedback pcm_feedback;

	 //
	 bool status_center_steering_home;
	 float32_t desired_steer_angle;
	 bool read_pcm_readback_status;

	 //
	 bool button_start_move_state;
	 float64_t current_speed_percent;
	 sSM_Status_t log_calib_data;

	 //
	 std::shared_ptr<RetrieveVehicleMeasure> sptr_RetrieveVehicleMeasure_gps;
	 std::shared_ptr<RetrieveCovPoseMsg> sptr_RetrieveCovPoseMsg;
	 Platform::Sensors::GeometryMsgsVehicleMeasure stkci_vehmeas_sbg_data;
	 Platform::Sensors::GeometryMsgsPoseWithCovariance stkci_pose_data;

};


}
#endif
