/*
 * VCMProcess.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#include "VCMProcess.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace VCM
{

VCMProcess::VCMProcess()
{

}

VCMProcess::VCMProcess(std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus,
		std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMStatus,
		std::shared_ptr<RetrieveVcmCmd> sptr_RetrieveVcmCmd,
		std::shared_ptr<RetrieveVcmCmd> sptr_RetrieveVcmCmd_NTU):
		sptr_RetreiveVIMStatus_(sptr_RetreiveVIMStatus),
		sptr_RetrievePLMStatus_(sptr_RetrievePLMStatus),
		sptr_RetrieveVcmCmd_(sptr_RetrieveVcmCmd),
		sptr_RetrieveVcmCmd_NTU_(sptr_RetrieveVcmCmd_NTU)
{
	//reset health status
	msg_health_status.system_state = Platform::ePLM_Status::PLMStatus_Invalid;
	msg_health_status.first_receive_vimstatus_msg=false;
	msg_health_status.receive_vimstatus_msg=false;
	msg_health_status.error_receive_vimstatus_msg_count=0;
	msg_health_status.first_receive_plmstatus_msg = false;
	msg_health_status.receive_plmstatus_msg=false;
	msg_health_status.error_receive_plmstatus_msg=0;
	msg_health_status.lost_aux_message_heartbeat=false;
	msg_health_status.error_receive_pfm_msg_count_automode = 0;
	msg_health_status.rx_pfm_commands = false;
	msg_health_status.desired_speed_percent=0.0;
	msg_health_status.desired_steer_radians=0.0;
	msg_health_status.cmd_dbw_speed_percent=0.0;
	msg_health_status.cmd_dbw_steer_percent=0.0;

	//set publisher
	ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
	std::vector<std::string> interface_list;
	interface_list.push_back("PalletDBWCmd");
	ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);

	Velocity_Intergral_error	= 0;
	Prev_velo					= 0;
	Steer_Intergral_error		= 0;
	Prev_Steer					= 0;		
}

void VCMProcess::SetConfigParams(const VCMCSCI_Config &config_params)
{
	config_params_ = config_params;
	default_max_steer_radians = config_params_.max_steer*3.141592/180.0;
	default_max_speed_percent = config_params_.max_speed;

	//
	Platform::Utility::GetVehConfiguration cVehConfigurationConfig;
	const Platform::Utility::VehicleConfigurationData *ptr_veh_xml_data;
	if(cVehConfigurationConfig.OpenVehConfiguration())
	{
		ptr_veh_xml_data = cVehConfigurationConfig.GetVehConfigurationData();
		default_vehicle_length = ptr_veh_xml_data->chassis.wheel_base;
	}

	//
	std::cout << "[VCMProcess] = SetConfigParams max_steer =  " << config_params_.max_steer
			 << " max_speed =  " << config_params_.max_speed
			 << " veh_length =  " << default_vehicle_length
			 << std::endl;
}

void VCMProcess::PrintVCM_DBWStatus()
{
	//system("clear");
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	//
	std::cout << "System Time " << current_timestamp << std::endl;
	std::cout << " " << std::endl;
	std::cout << "[XML] = SetConfigParams max_steer =  " << config_params_.max_steer
			 << " max_speed =  " << config_params_.max_speed
			 << std::endl;

	//
	if(msg_health_status.unmmaned_mode)
	{
		std::cout << "Unmanned Switch " << std::endl;
	}
	else
	{
		std::cout << "Manned Switch" << std::endl;
	}
	//
	if(msg_health_status.system_state == Platform::ePLM_Status::PLMStatus_Waypoint)
	{
		std::cout << "Autonomous Waypoint State" << std::endl;
	}
	else
	{
		std::cout << "Not Autonomous Waypoint State" << std::endl;
	}
	std::cout << " " << std::endl;

	//
	if(msg_health_status.hw_estop_status)
	{
		std::cout << "Hardware ESTOP = Activated " << std::endl;
	}
	else
	{
		std::cout << "Hardware ESTOP = Clear " << std::endl;
	}

	//
	if(msg_health_status.receive_vimstatus_msg)
	{
		std::cout << "Receive VIM Message = Yes " << std::endl;
	}
	else
	{
		std::cout << "Receive VIM Message = No " << std::endl;
	}
	if(msg_health_status.receive_plmstatus_msg)
	{
		std::cout << "receive PLM msg = True " << std::endl;
	}
	else
	{
		std::cout << "receive PLM msg = False " << std::endl;
	}
	std::cout << " " << std::endl;

	//
	if(msg_health_status.rx_pfm_commands)
	{
		std::cout << "receive Path Follower commands = True " << std::endl;
	}
	else
	{
		std::cout << "receive Path Follower commands = False " << std::endl;
	}
	std::cout << " " << std::endl;
	//
	std::cout << "Desired PFM speed = " << msg_health_status.desired_speed_percent << std::endl;
	std::cout << "Desired PFM steer = " << msg_health_status.desired_steer_radians << std::endl;
	std::cout << " " << std::endl;
	//
	std::cout << "Commanded DBW speed = " << msg_health_status.cmd_dbw_speed_percent << std::endl;
	std::cout << "Commanded DBW steer = " << msg_health_status.cmd_dbw_steer_percent << std::endl;

}

void VCMProcess::GetPLMHealthStatus(platform_liveness_status &status)
{
	status = msg_health_status;
}

void VCMProcess::operator()()
{
	//time tracker only
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	//Get STKCI incoming message status and commands
	GetSystemStatus();

	//Operate in waypoint mode only
	bool send_zero_cmds=true;

	if(msg_health_status.lost_aux_message_heartbeat == true)
	{
		std::cout << "!! Lost heartbeat " << std::endl;
	}
	if(msg_health_status.hw_estop_status == true)
	{
		std::cout << "!! ESTOP " << std::endl;
	}
	if(msg_health_status.unmmaned_mode == false)
	{
		std::cout << "!! Not unmanned " << std::endl;
	}
	if(msg_health_status.system_state != Platform::ePLM_Status::PLMStatus_Waypoint)
	{
		std::cout << "!! Not waypoint mode " <<  msg_health_status.system_state << std::endl;
	}
		std::cout << msg_health_status.lost_aux_message_heartbeat << ","
	<< msg_health_status.hw_estop_status << ","
	<< msg_health_status.unmmaned_mode
	<< std::endl;
	//
	if((msg_health_status.lost_aux_message_heartbeat==false)
			&& (msg_health_status.hw_estop_status == false)
			&& (msg_health_status.unmmaned_mode)
			&& (plm_status_.system_state == Platform::ePLM_Status::PLMStatus_Waypoint))
	{
		//speed is a percentage.
		//steer is in radians
		if(sptr_RetrieveVcmCmd_->GetVcmCmd(data_vcm_cmd)) //From PathFollower
		{
			float64_t pfm_speed = data_vcm_cmd.velocity; //percentage
			float64_t pfm_steer = data_vcm_cmd.steer.mValue; //radians
			if(config_params_.vcm_trajdemo==0)
			{
				ComputePathFollowCmdtoDBWCmd(pfm_speed,pfm_steer); //pfm_steer which is curvature to steer
			}
			else
			{
				ComputeTrajDemoCmdtoDBWCmd(pfm_speed,pfm_steer); //curvature
			}
			send_zero_cmds = false;
		}//Get VCM
		else
		{
			send_zero_cmds = true;
			std::cout << " !! Did not receive PFM cmds !!! " << std::endl;
		}

		//
		if(send_zero_cmds)
		{
			msg_health_status.error_receive_pfm_msg_count_automode++;
		}
		else
		{
			msg_health_status.error_receive_pfm_msg_count_automode=0;
		}
	}//if.

	//send zero commands
	if(send_zero_cmds)
	{
		//set zero to stop and steer dbw cmds using PubDBWCmds
		PubDBWCmds(0.0, 0.0);
		//for display
		msg_health_status.desired_speed_percent=0.0;
		msg_health_status.desired_steer_radians=0.0;
		msg_health_status.cmd_dbw_speed_percent=0.0;
		msg_health_status.cmd_dbw_steer_percent=0.0;
		msg_health_status.rx_pfm_commands = false;
	}
	else
	{
		msg_health_status.rx_pfm_commands = true;
	}

	previous_timestamp =  current_timestamp;
}

void VCMProcess::ComputePathFollowCmdtoDBWCmd(float64_t desired_speed, float64_t current_curvature)
{
	//ensure within max linear speed percentage
	float64_t speed_mpersec;
	//Convert speed input from m/s to percent
	speed_mpersec = PID(	1.0, 						//P gain
							0.0, 						//I gain
							0.0, 						//D gain
							0.01, 						//dt
							-0.81,						//min (3km/h) reverse
							2.77,						//max (10km/h)
							desired_speed,				//Desired
							0.0,						//current feedback (DBW Speed feedback)
							Prev_velo,					//prev feedback
							Velocity_Intergral_error);	//Integral error
							
	//
	// GetBoundaryCheckLinearSpeed(desired_speed,speed_mpersec);
	//Steer is already in angle
	float64_t converted_steer_angle = atan(current_curvature*1.36); // 1.36 = wheel base of forklift
	std::cout << "Steering angle = " << converted_steer_angle << std::endl;

	//compute cmd steer in percentage
	float64_t steer_radpersec;
	steer_radpersec = PID(	1.8, 						//P gain
						0.0, 						//I gain
						0.0, 						//D gain
						0.01, 						//dt
						-M_PI_2,					//min
						M_PI_2,						//max
						current_curvature,			//Desired angle(rad)
						0.0,						//current feedback (DBW steer feedback)
						Prev_Steer,					//prev feedback angle(rad)
						Steer_Intergral_error);		//Integral error;
	//publish stkci
	std::cout << "VCM - Speed = " << speed_mpersec << std::endl;
	std::cout << "VCM - Steer = " << steer_radpersec << std::endl;

	PubDBWCmds(speed_mpersec,steer_radpersec);

	//for display
	msg_health_status.desired_speed_percent=desired_speed;
	msg_health_status.desired_steer_radians=converted_steer_angle;
	msg_health_status.cmd_dbw_speed_percent=speed_mpersec;
	msg_health_status.cmd_dbw_steer_percent=steer_radpersec;
}

void VCMProcess::ComputeTrajDemoCmdtoDBWCmd(float64_t current_speed_percent, float64_t current_steer_radians_percent)
{
	std::cout << "Check for = ComputeTrajDemoCmdtoDBWCmd" << std::endl;
	//ensure within max linear speed percentage
	float64_t percent_speed;
	GetBoundaryCheckLinearSpeed(current_speed_percent,percent_speed);

	//compute cmd steer in percentage
	float64_t percent_steer;
	//percent_steer = current_steer_radians/default_max_steer_radians;
	std::cout << "[AUTO TrajDemo] default_max_steer_radians = " << default_max_steer_radians
			<< " percent_steer = " <<  current_steer_radians_percent
			<< std::endl;

	//publish stkci
	PubDBWCmds(percent_speed, current_steer_radians_percent);

	//for display
	msg_health_status.desired_speed_percent=current_speed_percent;
	msg_health_status.desired_steer_radians=current_steer_radians_percent;
	msg_health_status.cmd_dbw_speed_percent=percent_speed;
	msg_health_status.cmd_dbw_steer_percent=current_steer_radians_percent;
}

void VCMProcess::GetBoundaryCheckLinearSpeed(float64_t current_speed, float64_t &new_speed)
{

	//update formula
//	new_speed = 0.543641109 * pow(current_speed, 3) - 1.216053406 * pow(current_speed, 2) + 1.024657709 * current_speed + 0.385845871;
//	if(current_speed == 0)
//	{
//		new_speed = 0.0;
//	}
//
//	if(new_speed<=-(new_speed * default_max_speed_percent))
//	{
//		new_speed = -(new_speed * default_max_speed_percent);
//	}
//
//
//	if(new_speed>=(new_speed * default_max_speed_percent))
//	{
//		new_speed = (new_speed * default_max_speed_percent);
//	}

	//updated method
//	new_speed = current_speed;
//	if(current_speed<=-(default_max_speed_percent*current_speed))
//	{
//		new_speed = -(default_max_speed_percent*current_speed);
//	}
//
//	if(current_speed>=(default_max_speed_percent*current_speed))
//	{
//		new_speed = (default_max_speed_percent*current_speed);
//	}

	//old method
	new_speed = current_speed;
	if(current_speed<=-default_max_speed_percent)
	{
		new_speed = -default_max_speed_percent;
	}

	if(current_speed>=default_max_speed_percent)
	{
		new_speed = default_max_speed_percent;
	}
}

///\brief send the drive and steer command via Platform::Sensors::PalletDbwCmdMsg message
///\param[in] drive_scale range from -1.0 to 1.0
///\param[in] steer_scale range from -1.0 to 1.0
void VCMProcess::PubDBWCmds(float64_t drive_speed_scale, float64_t steer_scale)
{
	Platform::Sensors::PalletDbwCmdMsg out_data_DbwCmdMsg;
	out_data_DbwCmdMsg.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	out_data_DbwCmdMsg.max_speed = config_params_.max_speed;//m/s
	out_data_DbwCmdMsg.max_steer = default_max_steer_radians; //radians
	out_data_DbwCmdMsg.cmd_speed = drive_speed_scale;
	out_data_DbwCmdMsg.cmd_steer = steer_scale;
	out_data_DbwCmdMsg.seq_key = seq_key_tracker_dbw_cmd;

	//
	seq_key_tracker_dbw_cmd++;
	if(seq_key_tracker_dbw_cmd>UINT16_MAX)
	{
		seq_key_tracker_dbw_cmd=0;
	}

	//
	ptr_stkci_pub->PubPalletDbwCmd("VCM", out_data_DbwCmdMsg);
}

void VCMProcess::GetSystemStatus()
{
	//Get VIM status
	Platform::Sensors::PalletAuxFbkMsg vim_status_local;
	if(sptr_RetreiveVIMStatus_->GetVIMDataMsg(vim_status_local))
	{
		vim_status=vim_status_local;
		msg_health_status.receive_vimstatus_msg=true;
		if(msg_health_status.first_receive_vimstatus_msg == false)
		{
			msg_health_status.first_receive_vimstatus_msg = true;
		}
		msg_health_status.error_receive_vimstatus_msg_count = 0;
	}
	else
	{
		msg_health_status.receive_vimstatus_msg=false;
		msg_health_status.error_receive_vimstatus_msg_count++;
		std::cout << " !! Did not rx VIM status " << std::endl;
	}

	//Get PLM status
	Platform::Sensors::SystemMsgsUGVPlatformStatus plm_status_local;
	if(sptr_RetrievePLMStatus_->GetPLMDataMsg(plm_status_local))
	{
		plm_status_=plm_status_local;
		msg_health_status.system_state = plm_status_.system_state;
		msg_health_status.receive_plmstatus_msg=true;
		if(msg_health_status.first_receive_plmstatus_msg == false)
		{
			msg_health_status.first_receive_plmstatus_msg = true;
		}
		msg_health_status.error_receive_plmstatus_msg=0;
	}
	else
	{
		msg_health_status.receive_plmstatus_msg=false;
		msg_health_status.error_receive_plmstatus_msg++;
		std::cout << " !! Did not rx PLM status " << std::endl;
	}

	if(msg_health_status.receive_vimstatus_msg == true)
	{
		//Determine current system state
		if(vim_status.manned_unmanned_switch==Platform::PIM::SWITCH_MANNED)//tele-opes
		{
			msg_health_status.unmmaned_mode=false;
		}
		if(vim_status.manned_unmanned_switch==Platform::PIM::SWITCH_UNMANNED)//autonomous
		{
			msg_health_status.unmmaned_mode=true;
		}

		//Check ESTOP status
		if(vim_status.estop_status==Platform::PIM::SWITCH_ESTOP_CLEAR)
		{
			msg_health_status.hw_estop_status=false;
		}
		else
		{
			msg_health_status.hw_estop_status=true;
		}
	}

	//Critical error when failed to receive VIM messages
	msg_health_status.lost_aux_message_heartbeat=false;
	if((msg_health_status.receive_plmstatus_msg==false)
			&& (msg_health_status.receive_vimstatus_msg==false)
			&& (msg_health_status.first_receive_plmstatus_msg))
	{
		msg_health_status.lost_aux_message_heartbeat=true;
		std::cout << " !! Did not rx Lost_aux_message_heartbeat " << std::endl;
	}
}

float64_t VCMProcess::PID(
			const float64_t Pgain, 
			const float64_t Igain, 
			const float64_t Dgain, 
			const float64_t dt, 
			const float64_t min,
			const float64_t max,
			const float64_t des,
			const float64_t cur,
			float64_t prev,
			float64_t Ierror)
{

		float64_t output = 0;
		float64_t steer_error = des - cur;		// Calculate error
		float32_t Pout = Pgain * steer_error;				// Proportional term
		Ierror += steer_error * dt;	// Integral term
		// if(fabs(Ierror) >= 100.0)
		// 	Ierror = 100 * (fabs(Ierror)/Ierror);	//Limits integral term 
		float32_t Iout = Igain * Ierror;
		float32_t derivative = (steer_error - prev) / dt;	// Derivative term
		float32_t Dout = Dgain * derivative;

		// Calculate total output
		output = Pout + Iout + Dout;
		
		// Restrict to max/min
		if (output > max)
			output = max;
		else if (output < min)
			output = min;

		prev = cur;

		return output;
}

void VCMProcess::SpeedController()
{
	/*
	 * Input: 	Speed feedback
	 * 			Desired Speed
	 * 			Previous speed error
	 *
	 * 	speed error = Desired speed - speed feedback
	 * 	Speed error delta = speed error - Previous speed error
	 * 	//small change in speed error =
	 *
	 * 	Desired acceleration/deceleration = 0.54m/s^2
	 *
	 * 	output: Brake torque(0 ~ 1.0)
	 *
	 *	brake =
	 */

	 float speed_error = desired_speed - Speed_feedback;
	 float acc_error = desired_acc - acc;
	 //if speed_error < 0 its deceleration
	//Assume 3 ranges (s,m,l) small, medium, large

	//rules
	bool speed_level[3];
	bool acceleration_level[3];

	float small,medium,large;
	float smallacc,mediumacc,largeacc;

	if(speed_error <= 0.3)
	{
		speed_level[0] = true;
	}
	else if(speed_error <= 0.5)
	{
		speed_level[0] = true;
		speed_level[1] = true;

	}
	else if(speed_error <= 0.7)
	{
		speed_level[1] = true;
		speed_level[2] = true;
	}
	else
	{
		speed_level[2] = true;
	}

	
	if(acc_error <= 0.3)
	{
		acceleration_level[0] = true;
	}
	else if(acc_error <= 0.5)
	{
		acceleration_level[0] = true;
		acceleration_level[1] = true;

	}
	else if(acc_error <= 0.7)
	{
		acceleration_level[1] = true;
		acceleration_level[2] = true;
	}
	else
	{
		acceleration_level[2] = true;
	}

	std::vector<float> output;
	output.clear();
	//check which rules are applicable
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			if(speed_level[i] && acceleration_level[j])
			{
				//this rule is applicable
				float output1 = 1e6;
				float output2 = 1e6;
				if(speed_error <= 0.3)
				{
					speed_error1 = 1.0;
				}
				else if(speed_error <= 0.5)
				{
					speed_error1 = speed_error*-gradient + find_Constant(0,0.5,-gradient);
					speed_error2 = speed_error*gradient + find_Constant(0,0.3,gradient);
				}
				else if(speed_error <= 0.7)
				{
					speed_error1 = speed_error*-gradient + find_Constant(1,0.5,-gradient);
					speed_error2 = speed_error*gradient + find_Constant(0,0.5,gradient);
				}
				else
				{
					speed_error1 = 1.0;
				}
				//
				if(acc_error <= 0.3)
				{
					acc_error1 = 1.0;
				}
				else if(acc_error <= 0.5)
				{
					acc_error1 = acc_error*-gradient + find_Constant(0,0.5,-gradient);
					acc_error2 = acc_error*gradient + find_Constant(0,0.3,gradient);
				}
				else if(speed_error <= 0.7)
				{
					acc_error1 = acc_error*-gradient + find_Constant(1,0.5,-gradient);
					acc_error2 = acc_error*gradient + find_Constant(0,0.5,gradient);
				}
				else
				{
					acc_error1 = 1.0;
				}
				//
				int slvl = rule_table(i,j);
				float res_1 = min(speed_error1, acc_error1); //determine which speed/acc level to use
				float res_2 = min(speed_error2, acc_error2); //determine which speed/acc level to use
				res_1 = res_1 * slvl; //determine high/low speed
				res_2 = res_2 * slvl; //determine high/low speed
				if(slvl <= 0.3) //low output
				{
					output.push_back(res_1);	
				}
				else //high output
				{
					output.push_back(res_2);
				}
			}
		}
	}

}

float VCMProcess::find_Constant(float y, float x, float m)
{
	float constant = y-(m*x);
	return constant;
}

float VCMProcess::rule_table(int i, int j)
{
	float ret = 0;
	float low_factor = 0.3;
	float high_factor = 0.7;
	//i = 0 and j = 0, speed = low
	//i = 1 and j = 0, speed = low
	if(i == 0)
	{
		ret = low_factor;
	}
	else if(i == 1 && j == 0)
	{
		ret = low_factor;
	}
	else if(i == 1 && j != 0)
	{
		ret = high_factor;
	}
	else
	{
		ret = high_factor;
	}
	return ret;
}

VCMProcess::~VCMProcess()
{
	PubDBWCmds(0.0,0.0);

}

}
