/*
 * PCMPalletDataStruct.h
 *
 *  Created on: Nov 30, 2017
 *      Author: emily
 */

#ifndef PCMPALLETDATASTRUCT_H_
#define PCMPALLETDATASTRUCT_H_

#include "Common/typedefs.h"

namespace DBWPalletCommon
{

struct pcm_palletbase_feedback
{
	bool pallet_detection_sensor_left; //true is active
	bool pallet_detection_sensor_right; //true is active
	bool pallet_height_sensor_top; //true is active
	bool pallet_height_sensor_bottom; //true is active
	bool collision_detection_sensor_left; //true is active
    bool collision_detection_sensor_right; //true is active
    bool pcm_controller_error_status; //true is error
    bool cmd_drive_error_status; //true is error
    bool cmd_fork_control_error_status; //true is error
    bool cmd_speed_value_error_status; //true is error
    bool CAN_communication_error_status; // true is error
    float32_t current_platform_speed;
    float32_t current_platform_batterylevel;
};

struct pcm_palletbase_commands
{
	bool move_forward; //true is active
	bool move_backward; //true is active
	bool fork_lowering; //true is active
	bool fork_lifting; //true is active
	float belly; //true is active
	float slow_speed; //true is active
	bool tiller; //true is active
	bool trigger_buzzer; //true is active
	float cmd_speed;
	int32_t analog_speed; //0 to 4095
};

struct pcm_trucktwo_palletbase_commands
{
	pcm_palletbase_commands pcm_drive_cmd;
	int32_t analog_steer; //0 to 4095
};

}

#endif /* PCMPALLETDATASTRUCT_H_ */
