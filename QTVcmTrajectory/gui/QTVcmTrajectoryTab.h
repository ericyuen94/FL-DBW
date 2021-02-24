/**
 *  @file    QTVcmTrajectoryTab.h
 *  @author  Emily Low
 *  @date    26th July 2015
 *  @version 1.0.0
 *
 *  @brief controller for vmap viewer
 *
 *  @section DESCRIPTION
 *
 */

#ifndef QTVcmTrajectoryTab_H
#define QTVcmTrajectoryTab_H

#include <QtWidgets>
#include <QSlider>
#include <QPushButton>
//
#include <memory>
//
#include "Common/Utility/TimeUtils.h"
#include "GetVcmTrajConfiguration.h"
#include "VcmTrajLog.h"
#include "VcmTrajRead.h"
//
#include "RetrieveVehicleMeasure.h"
#include "Interfaces/VcmCmdMsgPubInterface.h"
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "Platform/PlatformConstants.h"
#include "Common/Utility/configFile.h"
//
namespace vcmtrajdemo
{

class QTVcmTrajectoryTab : public QWidget
{

Q_OBJECT

public:

	QTVcmTrajectoryTab(QWidget *parent = 0);

	~QTVcmTrajectoryTab();

 private slots:

 	void handle_pushbtn_start_traj_log_job();

 	void handle_pushbtn_playback_traj_log_job();

 	void handle_pushbtn_play_fixedProfile();

 	void handle_timer_traj_log_job();

 	void handle_timer_traj_player_job();

private:

 	 //related to QT
	 void initQTVcmTrajectoryTab();

	 QGroupBox *createOne_log_trajdemo();

	 void PublishVCMCmds(const float64_t commandedVelocity, const float64_t commandedSteer);

	 void PubPalletBaseCmds(const int32_t trigger_tiller,
	 		const int32_t trigger_lower_fork,
	 		const int32_t trigger_raise_fork,
	 		const int32_t trigger_buzzer);

	 void playback_profile_one();

	 //void playback_profile_dhl();

private:

	 //timer
	 QTimer logTrajTimer;
	 QTimer playlogTrajTimer;

	 //
	 QPushButton *button_log_traj;
	 QPushButton *button_playback_traj;
	 QPushButton *button_playback_fixedprofile;

	 //
	 bool open_read_file_status;
	 bool start_traj_log_status;
	 bool start_traj_logplay_status;
	 bool start_motionprofile_status;

	 //
	 std::shared_ptr<GetVcmTrajConfiguration> sptr_cGetVcmTrajConfiguration; //calib traj csci config
	 std::shared_ptr<VcmTrajLog> sptr_cVcmTrajLog;
	 std::shared_ptr<VcmTrajRead> sptr_cVcmTrajRead;

	 //config data
	 VcmTrajConfig params_VcmTrajConfig;
	 float32_t max_speed;
	 float32_t max_steer;
	 bool readEOF_status;

	 //stkci shared ptr
	 std::shared_ptr<RetrieveVehicleMeasure> sptr_RetrieveVehicleMeasure_platform;
	 std::list<Platform::Sensors::GeometryMsgsVehicleMeasure> output_vehmeas_stkci_msg;

	 //Setup stkci pub
	 std::shared_ptr<Common::Utility::configFile> sptr_configfile;
	 STKCI::VcmCmdMsgPubInterface stkci_pub_vcm;
	 STKCI::VcmCmdMsg stkci_data_vcm;
	 std::string relative_path_senderid_config;
	 std::string	interface_config_file;

	//publish other stkci messages
	std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;


};


}
#endif
