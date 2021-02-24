#ifndef TELEOPSPROCESS_H_
#define TELEOPSPROCESS_H_

#include <cstdint>
#include <memory>
#include <map>
//
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "Platform/PlatformDefinition.h"
#include "TeleOpsCSCIConfiguration.h"

//
#include "joystick_generic.h"

namespace TeleOps
{

const int32_t JOYSTICK_AXIS=7;
const int32_t JOYSTICK_BUTTIONS=12;
const float64_t JOYSTICK_MAX_DRIVE_CMD_RANGE = -32767.0;
const float32_t JOYSTICK_STEER_RESOLUTION = 1.0;
const float32_t FORK_RESOLUTION = 0.1;

struct dbw_liveness_status
{
	bool error_joystick_notconnected;
	//
	bool first_receive_vimstatus_msg;
	bool receive_vimstatus_msg;
	int32_t not_receive_vimstatus_msg_count;
	//
	bool hw_estop_status;
	bool unmmaned_mode;
	//
	bool lost_aux_message_heartbeat;
};

class RetreiveVIMStatus;
class RetrievePLMStatus;

class TeleOpsProcess
{
	///\class TeleOpsProcess
	///\brief A class to process data
public:

	TeleOpsProcess();

	TeleOpsProcess(std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus);

	void operator()();

	void SetConfigParams(const TeleOpsCSCI_Config &config_params);

	void GetPLMHealthStatus(dbw_liveness_status &status);

	~TeleOpsProcess();

private:

	void GetSystemStatus();

	void PubTeleOpDBWCmds(float64_t drive_speed_scale, float64_t steer_scale);

	void PubPalletBaseCmds(const int32_t trigger_tiller,
			const float64_t trigger_lower_fork,
			const float64_t trigger_raise_fork,
			const float64_t trigger_move_fork_left,
			const float64_t trigger_move_fork_right,
			const float64_t trigger_tilt_fork_up,
			const float64_t trigger_tilt_fork_down,
			const int32_t trigger_buzzer,
			const float64_t drive_speed_scale,
			const float64_t steer_scale,
			const float64_t brake_scale);

	void PubPalletBaseCmds_StillForklift(const int32_t trigger_tiller,
			const float64_t trigger_lower_fork,
			const float64_t trigger_raise_fork,
			const float64_t trigger_move_fork_left,
			const float64_t trigger_move_fork_right,
			const float64_t trigger_tilt_fork_up,
			const float64_t trigger_tilt_fork_down,
			const int32_t trigger_buzzer,
			const float64_t drive_speed_scale,
			const float64_t steer_scale,
			const float64_t brake_scale);


	void PrintStatusSummary();

	void DecodeJoyStickKeys(int axis[], char button[]);

private:

	//
	TeleOpsCSCI_Config config_params_;
	int64_t previous_timestamp;

	//joystick
	std::shared_ptr<joystick_generic> sptr_Joystick;

	//sub stkci messages
	std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus_;
	dbw_liveness_status msg_health_status;
	Platform::Sensors::SystemMsgsUGVPlatformStatus plm_status;
	Platform::Sensors::PalletAuxFbkMsg vim_status;

	//publish stkci messages
	std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;
	uint16_t seq_key_tracker_dbw_cmd;
	uint16_t seq_key_tracker_palletbase_cmd;

	//Joystick commands
	int  axis[JOYSTICK_AXIS];
	char button[JOYSTICK_BUTTIONS];

	//
	//init values
	float32_t max_speed;
	float32_t init_speed;
	float32_t max_steer;
	float32_t init_steer;
	float32_t current_speed; //m/s
	float32_t current_steer;//in deg
	float32_t current_speed_percentage;
	float32_t current_brake_percentage;
	float32_t lower_fork;
	float32_t max_lowerfork;
	float32_t raise_fork;
	float32_t max_raisefork;
	float64_t lowerfork_percentage;
	float64_t raisefork_percentage;
	float64_t move_fork_to_left_percentage;
	float64_t move_fork_to_right_percentage;
	float64_t tilt_fork_to_up_percentage;
	float64_t tilt_fork_to_down_percentage;

	//float32_t trigger_tiller_estop;
	//float32_t trigger_lower_fork;
	//float32_t trigger_raise_fork;
	//float32_t trigger_collision_sensors;

	//status
	float64_t display_out_speed_percent;
	float64_t display_out_steer_percent;
	float64_t display_out_brake_percentage;

	int32_t display_out_joystick_deadman_switch_status;
	float64_t display_out_joystick_lower_fork_status;
	float64_t display_out_joystick_raise_fork_status;
	float64_t display_out_joystick_fork_move_left_status;
	float64_t display_out_joystick_fork_move_right_status;
	float64_t display_out_joystick_fork_tilt_up_status;
	float64_t display_out_joystick_fork_tilt_down_status;
	int32_t display_out_joystick_trigger_buzzer_status;

	//
	int32_t current_joystick_deadman_switch_status;
	int32_t current_joystick_sw_estop_status;
	float64_t current_joystick_lower_fork_status;
	float64_t current_joystick_raise_fork_status;
	float64_t current_joystick_lower_fork_move_left_status;
	float64_t current_joystick_raise_fork_move_right_status;
	float64_t current_joystick_lower_fork_tilt_up_status;
	float64_t current_joystick_raise_fork_tilt_down_status;
	int32_t current_joystick_trigger_buzzer_status;
	float64_t current_joystick_drive_cmd_range;
	float64_t current_joystick_steer_cmd_range;
};

}
#endif /* ifndef TELEOPSPROCESS_H_ */
