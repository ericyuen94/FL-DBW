/*
 * DBWManager.cpp
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */

#include "DBWManager.h"
#include "DBWInitializeController.h"
#include "DBWProcess.h"
//
#include "RetrieveVehMeasure.h"
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"
#include "RetreiveVIMStatus.h"
#include "RetrievePLMStatus.h"
#include "RetrieveBaseFeedback.h"
//
#include "BydFLDriveSteerForkThread.h"

//
namespace DBW
{

DBWManager::DBWManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("DBW");
	read_config_success = false;
}

void DBWManager::setupSpecificCSCIResources()
{
	// Create all the Manager's Resources
	sptr_init_controller = std::make_shared<DBWInitializeController>(this);
	sptr_DBWProcess = std::make_shared<DBWProcess>();

	// Get configuration data
 	GetDBWCSCIConfiguration cGetDBWConfig;
 	DBWPalletCommon::GetSmartMotorSteerConfiguration cGetSteerMotorDBWConfiguration;

	// Configuration file related to DBW CSCI and PCM card
	bool open_result = cGetDBWConfig.OpenViewerConfiguration();
	if(open_result)
	{
		cGetDBWConfig.GetViewerConfigData(config_params);
	 	read_config_success = true;
    }
	else
	{
		std::cout << "Error read DBW CSCI XML file " << std::endl;
		exit(1);
	}

	 // Configuration file related to Serial Smart motor steer and Steer encoder
	 bool open_result_dbw = cGetSteerMotorDBWConfiguration.OpenSmartMotorSteerConfiguration();
	 if(open_result_dbw)
	 {
		// Get the configuration data.
		cGetSteerMotorDBWConfiguration.GetSmartMotorSteerConfigData(config_brake_params);
		cGetSteerMotorDBWConfiguration.PrintSmartMotorSteerConfigData(config_brake_params);
	 }
	 else
	 {
		 std::cout << "Error read DBW Steer XML file " << std::endl;
	     exit(1);
	 }

	// Get configuration data
	GetDBWEncodersConfiguration cGetDBWEncodersConfig;

	bool open_encoder_config = cGetDBWEncodersConfig.OpenViewerConfiguration();
	if (open_encoder_config)
	{
		cGetDBWEncodersConfig.GetViewerConfigData(config_encoder_params);
		read_encoder_config_success = true;
	}
	else
	{
		std::cout << "Error read DBW Decoder XML file " << std::endl;
		exit(1);
	}

	if (config_params.logging.enabled == 1)
	{
		SetupHealthDataLog();
	}

	//Set shared pointers
	sptr_RetrieveVehMeasure =  std::make_shared<RetrieveVehMeasure>(config_params.SubStkciMsgs["GPSVehMeasureMsg"].senderid);
	sptr_RetreiveDBWCmds_TeleOp = std::make_shared<RetreiveDBWCmds>(config_params.SubStkciMsgs["TeleOpsCmdMsg"].senderid);
	sptr_RetreiveDBWCmds_VCM = std::make_shared<RetreiveDBWCmds>(config_params.SubStkciMsgs["VCMCmdMsg"].senderid);
	sptr_RetreiveBaseCmds = std::make_shared<RetreiveBaseCmds>(config_params.SubStkciMsgs["PalletBaseCmdMsg"].senderid);
	sptr_RetreiveVIMStatus = std::make_shared<RetreiveVIMStatus>(config_params.SubStkciMsgs["PIMStatusMsg"].senderid);
	sptr_RetrievePLMStatus = std::make_shared<RetrievePLMStatus>("PLM");
	sptr_RetrievePLMErrorStatus = std::make_shared<RetrievePLMStatus>("PLM_Status");
	sptr_RetreiveTiltMotorCmds = std::make_shared<RetreiveTiltMotorCmds>("TiltMotorMsg");
	sptr_RetrieveBaseFeedback = std::make_shared<RetrieveBaseFeedback>("PIM");


	//
	sptr_BydFLDriveSteerForkThread = std::make_shared<BydFLDriveSteerForkThread>(sptr_RetrieveVehMeasure,
			sptr_RetreiveDBWCmds_TeleOp,
			sptr_RetreiveDBWCmds_VCM,
			sptr_RetreiveBaseCmds,
			sptr_RetreiveVIMStatus,
			sptr_RetrievePLMStatus,
			sptr_RetrievePLMErrorStatus,
			sptr_RetreiveTiltMotorCmds,
			sptr_RetrieveBaseFeedback);

	//set config and forklift controller card
	sptr_BydFLDriveSteerForkThread->SetConfigParams(config_params, config_encoder_params, config_brake_params);

	//
	setInitializeController(sptr_init_controller); 	// mpInitController is used as Initialize state controller
	setReadyFunction(std::bind(&DBWManager::readyFunction, this));
}

void DBWManager::SetupHealthDataLog()
{
	std::string current_tod;
	current_tod = Platform::Log::LogUtils::GetStringYearDayTime();
	//
	//Create log directory
	std::string log_folder_path_;
	log_folder_path_.append("/proj/pallet/Tatooine/Logs");
	log_folder_path_.append("/LogDBW_");
	log_folder_path_.append(current_tod);
	Platform::Log::LogUtils::MakeDir(log_folder_path_);
	std::string chmod_cmd = "chmod 777 -R ";
	chmod_cmd.append(log_folder_path_);
	std::system(chmod_cmd.c_str());
	global_log_folder_path_ = log_folder_path_;
	//
	std::string filename_txt;
	filename_txt.append(log_folder_path_);
	filename_txt.append("/dbw_health.log");
	//
	global_filename1_txt = filename_txt;
	ptrFileText_logger_health.open(filename_txt.c_str(),std::fstream::out | std::fstream::binary);
	//
	filename_txt.clear();
	filename_txt.append(log_folder_path_);
	filename_txt.append("/dbw_fsmdata.log");
	global_filename2_txt = filename_txt;
	ptrFileText_logger_fsm_data.open(filename_txt.c_str(),std::fstream::out | std::fstream::binary);
}

void DBWManager::UpdateDir()
{
	uint64_t file_size = Platform::Log::LogUtils::get_file_size(global_filename1_txt) + Platform::Log::LogUtils::get_file_size(global_filename2_txt);
	if (file_size >= max_file_size)
	{
		ptrFileText_logger_health.close();
		ptrFileText_logger_fsm_data.close();

		std::string chmod_cmd = "chmod 777 -R ";
		chmod_cmd.append(global_log_folder_path_);
		std::system(chmod_cmd.c_str());

		DBWManager::SetupHealthDataLog();
	}
}

void DBWManager::WriteDBWDataLogText(std::string msg)
{
	if(ptrFileText_logger_health != NULL)
	{
		ptrFileText_logger_health << std::setiosflags(std::ios::fixed) << Common::Utility::Time::getusCountSinceEpoch()
		<< " " << msg << std::endl;
	}
}

void DBWManager::WriteFSMDataLogText(const dbw_liveness_status &health)
{
	if(ptrFileText_logger_fsm_data != NULL)
	{
		ptrFileText_logger_fsm_data << std::setiosflags(std::ios::fixed) << Common::Utility::Time::getusCountSinceEpoch()
		<< " " << health.current_drive_cmd.cmd_speed
		<< " " << health.current_drive_cmd.cmd_steer
		<< " " << health.current_platform_forkheight
		<< " " << health.current_palletbase_cmd.trigger_belly
		<< " " << health.current_palletbase_cmd.trigger_slowspeed
		<< " " << health.current_palletbase_cmd.fork_height
		<< std::endl;
	}
}

void DBWManager::readyFunction()
{
	//clear the flags
	for(int32_t i=0; i<Platform::PLM_STATUS_TOTAL_BITS; i++)
	{
		plm_warning_status[i]=0;
		plm_error_status[i]=0;
	}

	//Update health status
	dbw_liveness_status current_msg_health_status;
	sptr_BydFLDriveSteerForkThread->GetPLMHealthStatus(current_msg_health_status);

	//Logging
	//WriteFSMDataLogText(current_msg_health_status);
	//UpdateDir();
	//Platform::Log::LogUtils::MaxDirectory("/proj/pallet/Tatooine/Logs", (char *)"LogDBW", "6");

	//
	int32_t threshold_lost_comms_count = 20;
	//
	if (current_msg_health_status.pcm_feedback_healthstatus.open_emuc_can_port)
	{
		if(current_msg_health_status.bConnectedSteerMotor == false)
		{
			plm_error_status[2]=1;
			WriteDBWDataLogText("error_open_smart_motor_port");
		}
		if(!current_msg_health_status.connect_AO1 || !current_msg_health_status.connect_AO2
				|| !current_msg_health_status.connect_RO1 || !current_msg_health_status.connect_RO2 || !current_msg_health_status.connect_RO3)
		{
			plm_error_status[3]=1;
			WriteDBWDataLogText("error_connect_adam");
		}
		if(current_msg_health_status.error_plmstatus_count>threshold_lost_comms_count || current_msg_health_status.error_plmerrorstatus_count > threshold_lost_comms_count)
		{
			plm_error_status[4]=1;
			WriteDBWDataLogText("error_plmstatus_count");
		}
		//
		if(current_msg_health_status.error_vimstatus_count>threshold_lost_comms_count)
		{
			plm_error_status[5]=1;
			WriteDBWDataLogText("error_vimstatus_count");
		}
		//
		if(current_msg_health_status.error_rxdrvcmd_tele_count>threshold_lost_comms_count)
		{
			plm_error_status[6]=1;
			WriteDBWDataLogText("error_rxdrvcmd_tele_count");
		}
		//
		if(current_msg_health_status.error_rxdrvcmd_vcm_count>threshold_lost_comms_count)
		{
			plm_error_status[7]=1;
			WriteDBWDataLogText("error_rxdrvcmd_vcm_count");
		}
//		if(current_msg_health_status.error_rxpalletbasecmd_count>threshold_lost_comms_count)
//		{
//			plm_error_status[8]=1;
//			WriteDBWDataLogText("error_rxpalletbasecmd_count");
//		}
		//
		if(current_msg_health_status.error_rxpalletbasefb_count>threshold_lost_comms_count)
		{
			plm_error_status[9]=1;
			WriteDBWDataLogText("error_rxpalletbasefb_count");
		}
		if(current_msg_health_status.pcm_feedback_healthstatus.rx_line_encoder_fb == false)
		{
			plm_error_status[10]=1;
			WriteDBWDataLogText("error_rx_line_encoder_fb");
		}
		//
		if(current_msg_health_status.pcm_feedback_healthstatus.rx_steer_encoder_fb == false)
		{
			plm_error_status[11]=1;
			WriteDBWDataLogText("error_rx_steer_encoder_fb");
		}
		//
		if(current_msg_health_status.pcm_feedback_healthstatus.rx_tilt_encoder_fb == false)
		{
			plm_error_status[12]=1;
			WriteDBWDataLogText("error_rx_tilt_motor_encoder_fb");
		}
		//
		if(current_msg_health_status.pcm_feedback_healthstatus.speed_mode != DBWPalletCommon::SPEED_MODE::TURTLE)
		{
			plm_error_status[13]=1;
			WriteDBWDataLogText("error_vehicle_in_rabbit_mode");
		}
		if((Common::Utility::Time::getusCountSinceEpoch() - current_msg_health_status.pcm_feedback_healthstatus.speed_mode_readtime) > 300000)
		{
			plm_error_status[14]=1;
			WriteDBWDataLogText("error_exceed_speedmode_readtime");
		}
		if((Common::Utility::Time::getusCountSinceEpoch() - current_msg_health_status.pcm_feedback_healthstatus.speed_mode_decodetime) > 300000)
		{
			plm_error_status[15]=1;
			WriteDBWDataLogText("error_exceed_speedmode_decodetime");
		}
		if((Common::Utility::Time::getusCountSinceEpoch() - current_msg_health_status.pcm_feedback_healthstatus.speed_readtime) > 300000)
		{
			plm_error_status[16]=1;
			WriteDBWDataLogText("error_exceed_speed_readtime");
		}
		if((Common::Utility::Time::getusCountSinceEpoch() - current_msg_health_status.pcm_feedback_healthstatus.speed_decodetime) > 300000)
		{
			plm_error_status[17]=1;
			WriteDBWDataLogText("error_exceed_speed_decodetime");
		}
		if((Common::Utility::Time::getusCountSinceEpoch() - current_msg_health_status.pcm_feedback_healthstatus.battery_readtime) > 300000)
		{
			plm_error_status[18]=1;
			WriteDBWDataLogText("error_exceed_battery_readtime");
		}
		if((Common::Utility::Time::getusCountSinceEpoch() - current_msg_health_status.pcm_feedback_healthstatus.battery_decodetime) > 300000)
		{
			plm_error_status[19]=1;
			WriteDBWDataLogText("error_exceed_battery_decodetime");
		}
		if(current_msg_health_status.error_temperaturelimit_count>threshold_lost_comms_count)
		{
			plm_error_status[20]=1;
			WriteDBWDataLogText("error_exceed_normal_temperature_limit");
		}
		//
//		if(current_msg_health_status.exceeded_speed)
//		{
//			plm_error_status[13]=1;
//			WriteDBWDataLogText("error_exceed_speed");
//		}
//
//		if(current_msg_health_status.exceeded_steer)
//		{
//			plm_error_status[14]=1;
//			WriteDBWDataLogText("error_exceed_steer");
//		}
//
//		if(current_msg_health_status.exceeded_forkheight)
//		{
//			plm_error_status[15]=1;
//			WriteDBWDataLogText("error_exceed_forkheight");
//		}
//
//		if(current_msg_health_status.exceeded_battlevel)
//		{
//			plm_error_status[16]=1;
//			WriteDBWDataLogText("error_exceed_batterylevel");
//		}
//


		//Warning
		if(current_msg_health_status.send_stop_cmd_dbw)
		{
//			plm_error_status[33]=1;
			plm_warning_status[33]=1;
			WriteDBWDataLogText("warning_send_stop_cmd_dbw");
		}

		if(current_msg_health_status.temperature_warning == true)
		{
			plm_warning_status[1]=1;
			WriteDBWDataLogText("warning_cpu_higher_temperatue_than_normal");
		}
	}
	else
	{
		//
		if(current_msg_health_status.pcm_feedback_healthstatus.open_emuc_can_port == false)
		{
			plm_error_status[1]=1;
			WriteDBWDataLogText("error_open_emuc_can_port");
		}
	}

	//Send to PLM
	ResetWarning();
	ResetError();
	for(int32_t i=0; i<Platform::PLM_STATUS_TOTAL_BITS; i++)
	{
		if(plm_warning_status[i]==1)
		{
			UpdateWarning(i,true);
		}
		if(plm_error_status[i]==1)
		{
			UpdateError(i,true);
		}
	}
}

void DBWManager::cleanupSpecificCSCIResources()
{
	if(config_params.logging.enabled == 1)
	{
		ptrFileText_logger_health.close();
		ptrFileText_logger_fsm_data.close();
	}
	//
	sptr_BydFLDriveSteerForkThread->ShutdownDBWDriveAndSteer();
}

void DBWManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

DBWManager::~DBWManager()
{

}
}
