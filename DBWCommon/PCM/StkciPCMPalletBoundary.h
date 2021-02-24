/*
 * StkciPCMPalletBoundary.h
 *
 *  Created on: Nov 30, 2017
 *      Author: emily
 */

#ifndef STKCIPCMPALLETBOUNDARY_H_
#define STKCIPCMPALLETBOUNDARY_H_

#include <cstdint>
#include <memory>
#include <cmath>
#include "PCMPalletDataStruct.h"
#include "Common/Utility/TimeUtils.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"

namespace DBWPalletCommon
{

class StkciPCMPalletBoundary
{

public:

	StkciPCMPalletBoundary();

	~StkciPCMPalletBoundary();

	static void ResetPalletPCMCommad(pcm_palletbase_commands &pcm_base_cmds);

	static void Convert_StkciDBWAndPalletBaseCmds_To_PalletPCMCmd(Platform::Sensors::PalletDbwCmdMsg stkci_dbw_cmd,
			Platform::Sensors::PalletBaseCmdMsg stkci_basepallet_cmd,
			pcm_palletbase_commands &pcm_base_cmds);

	static void Convert_PalletPCMFbk_To_StkciDBWAndPalletBaseFbk(pcm_palletbase_feedback pcm_base_fbk,
			Platform::Sensors::PalletDbwFbkMsg &stkci_dbw_fbk,
			Platform::Sensors::PalletBaseFbkMsg &stkci_basepallet_fbk);

	static void Convert_StkciDBWAndPalletBaseCmds_To_TruckTwoPalletPCMCmd(
				Platform::Sensors::PalletDbwCmdMsg stkci_dbw_cmd,
				Platform::Sensors::PalletBaseCmdMsg stkci_basepallet_cmd,
				pcm_trucktwo_palletbase_commands &pcm_base_cmds);

	static void Convert_Speed_To_PalletPCMCmd(float64_t current_speed,
				pcm_palletbase_commands &pcm_base_cmds);

};

void StkciPCMPalletBoundary::ResetPalletPCMCommad(pcm_palletbase_commands &pcm_base_cmds)
{
  	pcm_base_cmds.move_forward=false;
	pcm_base_cmds.move_backward=false;
	pcm_base_cmds.fork_lowering=false;
	pcm_base_cmds.fork_lifting=false;
	pcm_base_cmds.belly=false;
	pcm_base_cmds.slow_speed=false;
	pcm_base_cmds.tiller=true; //tiller is always true
	pcm_base_cmds.trigger_buzzer=false;
	pcm_base_cmds.analog_speed=0;
}

void StkciPCMPalletBoundary::Convert_PalletPCMFbk_To_StkciDBWAndPalletBaseFbk(pcm_palletbase_feedback pcm_base_fbk,
		Platform::Sensors::PalletDbwFbkMsg &stkci_dbw_fbk,
		Platform::Sensors::PalletBaseFbkMsg &stkci_basepallet_fbk)
{

	//Pallet dbw feedback
	stkci_dbw_fbk.timestamp = Common::Utility::Time::getusecCountSinceEpoch();


	//---------------------------------------------------
	//Pallet base feedback
	stkci_basepallet_fbk.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	if(pcm_base_fbk.collision_detection_sensor_left)
	{
		stkci_basepallet_fbk.collision_sensor_left=0x01;
		//std::cout << "collision_detection_sensor_left " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.collision_sensor_left=0x00;
	}
	//
	if(pcm_base_fbk.collision_detection_sensor_right)
	{
		stkci_basepallet_fbk.collision_sensor_right=0x01;
		//std::cout << "collision_detection_sensor_right " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.collision_sensor_right=0x00;
	}
	//
	if(pcm_base_fbk.pallet_detection_sensor_left)
	{
		stkci_basepallet_fbk.engagement_sensor_left=0x01;
		//std::cout << "engagement_sensor_left " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.engagement_sensor_left=0x00;
	}
	//
	if(pcm_base_fbk.pallet_detection_sensor_right)
	{
		stkci_basepallet_fbk.engagement_sensor_right=0x01;
		//std::cout << "engagement_sensor_right " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.engagement_sensor_right=0x00;
	}
	//
	if(pcm_base_fbk.pallet_height_sensor_top)
	{
		stkci_basepallet_fbk.height_sensor_top=0x01;
		//std::cout << "pallet_height_sensor_top " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.height_sensor_top=0x00;
	}
	//
	if(pcm_base_fbk.pallet_height_sensor_bottom)
	{
		stkci_basepallet_fbk.height_sensor_base=0x01;
		//std::cout << "pallet_height_sensor_bottom " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.height_sensor_base=0x00;
	}
	//
	if(pcm_base_fbk.pcm_controller_error_status)
	{
		stkci_basepallet_fbk.pcm_controller_bit_error=0x01;
		//std::cout << "pcm_controller_bit_error " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.pcm_controller_bit_error=0x00;
	}
	//
	if(pcm_base_fbk.cmd_drive_error_status)
	{
		stkci_basepallet_fbk.drive_cmd_error=0x01;
		//std::cout << "drive_cmd_error " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.drive_cmd_error=0x00;
	}
	//
	if(pcm_base_fbk.cmd_fork_control_error_status)
	{
		stkci_basepallet_fbk.fork_cmd_error=0x01;
		//std::cout << "fork_cmd_error " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.fork_cmd_error=0x00;
	}
	//
	if(pcm_base_fbk.cmd_speed_value_error_status)
	{
		stkci_basepallet_fbk.speed_range_error=0x01;
		//std::cout << "speed_range_error " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.speed_range_error=0x00;
	}
	//
	if(pcm_base_fbk.CAN_communication_error_status)
	{
		stkci_basepallet_fbk.can_comms_error=0x01;
		//std::cout << "can_comms_error " << std::endl;
	}
	else
	{
		stkci_basepallet_fbk.can_comms_error=0x00;
	}
	//
	stkci_basepallet_fbk.batterylevel = pcm_base_fbk.current_platform_batterylevel;//TBD
}

void StkciPCMPalletBoundary::Convert_StkciDBWAndPalletBaseCmds_To_TruckTwoPalletPCMCmd(
			Platform::Sensors::PalletDbwCmdMsg stkci_dbw_cmd,
			Platform::Sensors::PalletBaseCmdMsg stkci_basepallet_cmd,
			pcm_trucktwo_palletbase_commands &pcm_base_cmds)
{
	//
	Convert_StkciDBWAndPalletBaseCmds_To_PalletPCMCmd(stkci_dbw_cmd,stkci_basepallet_cmd,pcm_base_cmds.pcm_drive_cmd);

	//compute the command for steering angle, max steering is fixed at 90 degrees
	//Send commands to steer motor.
	float32_t steer_percent;
	float32_t steer_percent_count;
	int32_t steer_count=1936;
	if(stkci_dbw_cmd.cmd_steer<0.0) //turn right, Right hand convention
	{
		steer_percent = stkci_dbw_cmd.cmd_steer;
		if(steer_percent<-1.0) //lower bound
		{
			steer_percent = -1.0;
		}
		steer_percent_count = steer_count*1.0 + steer_percent*1040.0;
		steer_count = static_cast<int32_t>(steer_percent_count);
	}
	else//turn left, Right hand convention
	{
		steer_percent = stkci_dbw_cmd.cmd_steer;
		if(steer_percent>1.0)//upper bound
		{
			steer_percent = 1.0;
		}
		steer_percent_count = steer_count*1.0 + steer_percent*986.0;
		steer_count = static_cast<int32_t>(steer_percent_count);
	}
	//
	pcm_base_cmds.analog_steer = steer_count;

}

void StkciPCMPalletBoundary::Convert_StkciDBWAndPalletBaseCmds_To_PalletPCMCmd(
			Platform::Sensors::PalletDbwCmdMsg stkci_dbw_cmd,
			Platform::Sensors::PalletBaseCmdMsg stkci_basepallet_cmd,
			pcm_palletbase_commands &pcm_base_cmds)
{
	//reset the direction of travel.
	pcm_base_cmds.move_forward=false;
	pcm_base_cmds.move_backward=false;

	//determine forward of backward motion.
	//for pallet
	if(stkci_dbw_cmd.cmd_speed>0) //pallet ve speed is reverse
	{
		pcm_base_cmds.move_forward=false;
		pcm_base_cmds.move_backward=true;
		//std::cout << "[MOVE Backward] " << std::endl;
	}
	else if(stkci_dbw_cmd.cmd_speed<0) //pallet -ve speed is forward
	{
		pcm_base_cmds.move_forward=true;
		pcm_base_cmds.move_backward=false;
		//std::cout << "[MOVE Forward] " << std::endl;
	}

	//compute speed percentage
	pcm_base_cmds.analog_speed = static_cast<int32_t>(fabs(stkci_dbw_cmd.cmd_speed)*4095);

	//for safety, off the tiller when speed is zero
	//In other words, to drive pallet truck, always do with tiller is high
	//Tiller will be automatically off when speed is zero
	if(pcm_base_cmds.analog_speed == 0)
	{
		pcm_base_cmds.tiller=false;
	}

//	std::cout << std::dec << "command speed factor " <<  stkci_dbw_cmd.cmd_speed
//			<< " pcm_base_cmds.analog_speed = " << pcm_base_cmds.analog_speed  << std::endl;
//	std::cout << std::hex
//				<< "std out Hex.analog_speed = " << pcm_base_cmds.analog_speed  << std::endl;

	//trigger_tiller_stop
	if(stkci_basepallet_cmd.trigger_tiller==true)
	{
		pcm_base_cmds.tiller=true;
	}
	else
	{
		pcm_base_cmds.tiller=false;
		//pcm_base_cmds.analog_speed = 0;
	}

	//tiller is always on by default
	pcm_base_cmds.tiller=true;

	//fork_lowering
	if(stkci_basepallet_cmd.trigger_lower_fork==true)
	{
		pcm_base_cmds.fork_lowering=true;
	}
	else
	{
		pcm_base_cmds.fork_lowering=false;
	}

	//trigger_raise_fork
	if(stkci_basepallet_cmd.trigger_raise_fork==true)
	{
		pcm_base_cmds.fork_lifting=true;
	}
	else
	{
		pcm_base_cmds.fork_lifting=false;
	}

	//trigger_buzzer
	if(stkci_basepallet_cmd.trigger_buzzer==true)
	{
		pcm_base_cmds.trigger_buzzer=true;
	}
	else
	{
		pcm_base_cmds.trigger_buzzer=false;
	}

	pcm_base_cmds.belly=false;
	pcm_base_cmds.slow_speed=false;

	//trigger_belly
//	if(stkci_basepallet_cmd.trigger_belly==true)
//	{
//		pcm_base_cmds.belly=true;
//	}
//	else
//	{
//		pcm_base_cmds.belly=false;
//	}
//
//	//trigger_slow_speed
//	if(stkci_basepallet_cmd.trigger_slowspeed==true)
//	{
//		pcm_base_cmds.slow_speed=true;
//	}
//	else
//	{
//		pcm_base_cmds.slow_speed=false;
//	}
//
//

}

void StkciPCMPalletBoundary::Convert_Speed_To_PalletPCMCmd(float64_t current_speed,
			pcm_palletbase_commands &pcm_base_cmds)
{
	//reset the direction of travel.
	pcm_base_cmds.move_forward=false;
	pcm_base_cmds.move_backward=false;

	//determine forward of backward motion.
	//for pallet
	if(current_speed>0) //pallet ve speed is reverse
	{
		pcm_base_cmds.move_forward=false;
		pcm_base_cmds.move_backward=true;
		//std::cout << "[MOVE Backward] " << std::endl;
	}
	else if(current_speed<0) //pallet -ve speed is forward
	{
		pcm_base_cmds.move_forward=true;
		pcm_base_cmds.move_backward=false;
		//std::cout << "[MOVE Forward] " << std::endl;
	}

	//compute speed percentage
	pcm_base_cmds.analog_speed = static_cast<int32_t>(fabs(current_speed)*4095);

	//for safety, off the tiller when speed is zero
	//In other words, to drive pallet truck, always do with tiller is high
	//Tiller will be automatically off when speed is zero
	if(pcm_base_cmds.analog_speed == 0)
	{
		pcm_base_cmds.tiller=false;
	}
	else
	{
		pcm_base_cmds.tiller=true;
	}

	//fork_lowering
	pcm_base_cmds.fork_lowering=false;

	//trigger_raise_fork
	pcm_base_cmds.fork_lifting=false;

	//trigger_buzzer
	pcm_base_cmds.trigger_buzzer=false;

	pcm_base_cmds.belly=false;
	pcm_base_cmds.slow_speed=false;
}

}


#endif /* STKCIPCMPALLETBOUNDARY_H_ */
