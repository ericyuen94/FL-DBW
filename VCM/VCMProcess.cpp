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
	SpeedController();
	return;

	//
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

	FuzzyController::FuzzyFunction *FuzzySpdErrorSet[5];

	FuzzySpdErrorSet[0] = new FuzzyController::FuzTrapezoid;
	FuzzySpdErrorSet[0]->setName("XS speed error");
	FuzzySpdErrorSet[0]->setInterval(0.0,0.3);
	FuzzySpdErrorSet[0]->setMiddle(0.0,0.1);

	FuzzySpdErrorSet[1] = new FuzzyController::FuzTriangle;
	FuzzySpdErrorSet[1]->setName("SM speed error");
	FuzzySpdErrorSet[1]->setInterval(0.1,0.5);
	FuzzySpdErrorSet[1]->setMiddle(0.3,0.3);

	FuzzySpdErrorSet[2] = new FuzzyController::FuzTriangle;
	FuzzySpdErrorSet[2]->setName("ME speed error");
	FuzzySpdErrorSet[2]->setInterval(0.3,0.7);
	FuzzySpdErrorSet[2]->setMiddle(0.5,0.5);

	FuzzySpdErrorSet[3] = new FuzzyController::FuzTriangle;
	FuzzySpdErrorSet[3]->setName("LG speed error");
	FuzzySpdErrorSet[3]->setInterval(0.5,0.9);
	FuzzySpdErrorSet[3]->setMiddle(0.7,0.7);

	FuzzySpdErrorSet[4] = new FuzzyController::FuzTrapezoid;
	FuzzySpdErrorSet[4]->setName("XL speed error");
	FuzzySpdErrorSet[4]->setInterval(0.7,1.0);
	FuzzySpdErrorSet[4]->setMiddle(0.9,1.0);

	std::vector<FuzzyController::FuzzySolution> Spd_output;

	double speed_error = 0.88;
	int j = 0;

	for (int i = 0; i < 5; i++)
	{
		if(FuzzySpdErrorSet[i]->isDotinInterval(speed_error))
		{
			std::string n;
			FuzzySpdErrorSet[i]->GetName(n);
//			std::cout << "Name of fuzzy set : " << n << std::endl;
			// do string compare to cross check rule table
			FuzzyController::FuzzySolution spdout;
			spdout.SetValue(FuzzySpdErrorSet[i]->GetValue(speed_error));
			spdout.setName(n);
			Spd_output.push_back(spdout);
		}
	}

	//

	FuzzyController::FuzzyFunction *FuzzyAccErrorSet[5];

	FuzzyAccErrorSet[0] = new FuzzyController::FuzTrapezoid;
	FuzzyAccErrorSet[0]->setName("XS acc error");
	FuzzyAccErrorSet[0]->setInterval(0.0,0.3);
	FuzzyAccErrorSet[0]->setMiddle(0.0,0.1);

	FuzzyAccErrorSet[1] = new FuzzyController::FuzTriangle;
	FuzzyAccErrorSet[1]->setName("SM acc error");
	FuzzyAccErrorSet[1]->setInterval(0.1,0.5);
	FuzzyAccErrorSet[1]->setMiddle(0.3,0.3);

	FuzzyAccErrorSet[2] = new FuzzyController::FuzTriangle;
	FuzzyAccErrorSet[2]->setName("ME acc error");
	FuzzyAccErrorSet[2]->setInterval(0.3,0.7);
	FuzzyAccErrorSet[2]->setMiddle(0.5,0.5);

	FuzzyAccErrorSet[3] = new FuzzyController::FuzTriangle;
	FuzzyAccErrorSet[3]->setName("LG acc error");
	FuzzyAccErrorSet[3]->setInterval(0.5,0.9);
	FuzzyAccErrorSet[3]->setMiddle(0.7,0.7);

	FuzzyAccErrorSet[4] = new FuzzyController::FuzTrapezoid;
	FuzzyAccErrorSet[4]->setName("XL acc error");
	FuzzyAccErrorSet[4]->setInterval(0.7,1.0);
	FuzzyAccErrorSet[4]->setMiddle(0.9,1.0);

	std::vector<FuzzyController::FuzzySolution> Acc_output;

	double acc_error = 0.6;
	int k = 0;

	for (int i = 0; i < 5; i++)
	{
		if(FuzzyAccErrorSet[i]->isDotinInterval(acc_error))
		{
			std::string n;
			FuzzyAccErrorSet[i]->GetName(n);
			// do string compare to cross check rule table
			FuzzyController::FuzzySolution accout;
			accout.SetValue(FuzzyAccErrorSet[i]->GetValue(acc_error));
			accout.setName(n);
			Acc_output.push_back(accout);
		}
	}

	//Check which rule is applicable
	std::string a, b;
	float outdata[2];
	double weighted_sum=0;
	for(int i=0;i<Acc_output.size() && i<Spd_output.size();i++)
	{
		Spd_output[i].GetName(a);
		Acc_output[i].GetName(b);
		std::cout << "speed	output = " << Spd_output[i].out_data << std::endl;
		std::cout << "acc	output = " << Acc_output[i].out_data << std::endl;
		std::cout << a << std::endl << b <<std::endl;
		outdata[i] = std::min(Spd_output[i].out_data,Acc_output[i].out_data);
		weighted_sum+=outdata[i];
		std::cout << "Min = " << outdata[i] << std::endl;
		outdata[i] = outdata[i] * rule_table(a,b);
 	}
	//
	std::cout <<"weighted sum = " << weighted_sum <<std::endl;
	double final_output = (outdata[0] + outdata[1]) / weighted_sum;
	std::cout << "final output = " << final_output << std::endl;

}


float VCMProcess::rule_table(std::string spd_error, std::string acc_error)
{
	std::string line;
	std::ifstream myRule;
	myRule.open("/home/pallet/Desktop/Rules.txt", std::ifstream::in);
	if(myRule.is_open())
	{
		while(getline(myRule, line))
		{
			//(e.g line) "XL speed error + XL acc error = high"
			//(e.g line) "XS speed error + XS acc error = low"
			if(line.compare(0,14,spd_error) == 0 && line.compare(17,12,acc_error) == 0)
			{
				if(line.compare(32,4,"high")==0)
				{
					//return high speed factor
					std::cout << "HIGH "<< line <<std::endl;
					return 0.7;
				}
				else if(line.compare(32,3,"low")==0)
				{
					//return low speed factor
					std::cout << "LOW " << line <<std::endl;
					return 0.3;
				}
			}
		}
		myRule.close();
		return 0;
	}
	else
	{
		//unable to open files
		std::cout <<"Unable to Open File" <<std::endl;
		return 0;
	}
}

VCMProcess::~VCMProcess()
{
	PubDBWCmds(0.0,0.0);

}

}
