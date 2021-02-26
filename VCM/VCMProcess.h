/*
 * VCMProcess.h
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#ifndef VCMPROCESS_H_
#define VCMPROCESS_H_

#include <cstdint>
#include <memory>
#include <map>
#include <string>
#include <fstream>
//
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "Platform/Utility/GetVehConfiguration.h"
#include "Platform/PlatformDefinition.h"
#include "VCMConfig.h"
//
#include "RetreiveVIMStatus.h"
#include "RetrievePLMStatus.h"
#include "RetrieveVcmCmd.h"

#include "../DBWCommon/FuzzyLogicController/FuzzyController.h"


//
namespace VCM
{

struct platform_liveness_status
{
	long system_state;
	//
	bool first_receive_vimstatus_msg;
	bool receive_vimstatus_msg;
	int32_t error_receive_vimstatus_msg_count;
	//
	bool first_receive_plmstatus_msg;
	bool receive_plmstatus_msg;
	int32_t error_receive_plmstatus_msg;
	//
	bool lost_aux_message_heartbeat;
	//
	bool hw_estop_status;
	bool unmmaned_mode;
	//
	int32_t error_receive_pfm_msg_count_automode;

	//
	bool rx_pfm_commands;
	float64_t desired_speed_percent;
	float64_t desired_steer_radians;
	float64_t cmd_dbw_speed_percent;
	float64_t cmd_dbw_steer_percent;
};

class VCMProcess
{
	///\class VCMProcess
	///\brief A class to process data
public:

	VCMProcess();

	VCMProcess(std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus,
			std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMStatus,
			std::shared_ptr<RetrieveVcmCmd> sptr_RetrieveVcmCmd,
			std::shared_ptr<RetrieveVcmCmd> sptr_RetrieveVcmCmd_NTU);

	void SetConfigParams(const VCMCSCI_Config &config_params);

	void operator()();

	void GetPLMHealthStatus(platform_liveness_status &status);

	~VCMProcess();

private:

	void GetSystemStatus();

	void PubDBWCmds(float64_t drive_speed_scale, float64_t steer_scale);

	void ComputePathFollowCmdtoDBWCmd(float64_t desired_speed, float64_t current_curvature);

	void ComputeTrajDemoCmdtoDBWCmd(float64_t current_speed_percent, float64_t current_steer_radians_percent);

	void PrintVCM_DBWStatus();

	void GetBoundaryCheckLinearSpeed(float64_t current_speed, float64_t &new_speed);

	void SpeedController();

	float rule_table(std::string spd_error, std::string acc_error);

	float64_t PID(
			const float64_t Pgain, 
			const float64_t Igain, 
			const float64_t Dgain, 
			const float64_t dt, 
			const float64_t min,
			const float64_t max,
			const float64_t des,
			const float64_t cur,
			float64_t prev,
			float64_t Ierror);

	float64_t Velocity_Intergral_error;
	float64_t Steer_Intergral_error;
	float64_t Prev_velo;
	float64_t Prev_Steer;
	//
	VCMCSCI_Config config_params_;
	int64_t previous_timestamp;
	float64_t default_max_steer_radians;
	float64_t default_max_speed_percent;
	float64_t default_vehicle_length;


	//
	std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus_;
	std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMStatus_;
	std::shared_ptr<RetrieveVcmCmd> sptr_RetrieveVcmCmd_;
	std::shared_ptr<RetrieveVcmCmd> sptr_RetrieveVcmCmd_NTU_;

	//stkci messages
	Platform::Sensors::PalletAuxFbkMsg vim_status;
	Platform::Sensors::SystemMsgsUGVPlatformStatus plm_status_;
	STKCI::VcmCmdMsg data_vcm_cmd;
	STKCI::VcmCmdMsg data_vcm_cmd_NTU;

	//publish stkci messages
	std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;
	uint16_t seq_key_tracker_dbw_cmd;

	//Status
	platform_liveness_status msg_health_status;


	//Fuzzy Controller

	//rule txt
};

}
#endif /* ifndef VCMPROCESS_H_ */
