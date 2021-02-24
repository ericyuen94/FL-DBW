#include "TeleOpsProcess.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>
//
#include "RetreiveVIMStatus.h"

namespace TeleOps
{

TeleOpsProcess::TeleOpsProcess()
{
}

TeleOpsProcess::TeleOpsProcess(std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus):
			sptr_RetreiveVIMStatus_(sptr_RetreiveVIMStatus)
{
	//init shared pointer
	sptr_Joystick = std::make_shared<joystick_generic>();

	//set publisher
	ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
	std::vector<std::string> interface_list;
	interface_list.push_back("PalletBaseCmd");
	interface_list.push_back("PalletDBWCmd");
	ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);

	//reset stkci message publisher
	previous_timestamp = 0;
	seq_key_tracker_dbw_cmd = 0;
	seq_key_tracker_palletbase_cmd = 0;

	//reset health status
	msg_health_status.error_joystick_notconnected = false;
	msg_health_status.first_receive_vimstatus_msg = false;
	msg_health_status.receive_vimstatus_msg=false;
	msg_health_status.not_receive_vimstatus_msg_count=0;
	msg_health_status.hw_estop_status=false;
	msg_health_status.unmmaned_mode=false;
	msg_health_status.lost_aux_message_heartbeat=false;

	//reset joy stick number
	for(int32_t i=0; i<JOYSTICK_AXIS; i++)
	{
		axis[i]=0;
	}
	for(int32_t i=0; i<JOYSTICK_BUTTIONS; i++)
	{
		button[i]=0;
	}
}

void TeleOpsProcess::SetConfigParams(const TeleOpsCSCI_Config &config_params)
{
	config_params_ = config_params;

	//init port for joystick
	//init port for joystick
	if(sptr_Joystick->initialize_joystick(config_params_.joystick_port) == 0)
	{
		msg_health_status.error_joystick_notconnected = false;
	}
	else
	{
		//error open joystick
		msg_health_status.error_joystick_notconnected = true;
	}

	//init values
	max_speed = config_params_.max_speed;//as percentage
	init_speed = 0.0;
	max_steer = config_params_.max_steer; //as percentage
	init_steer = 0.0;
	current_speed = 0.0;
	current_steer = init_steer;
	current_speed_percentage = 0.0;
	lower_fork = 0.0;
	max_lowerfork = config_params_.max_lowerfork;
	raise_fork = 0.0;
	max_raisefork = config_params_.max_raisefork;
}

void TeleOpsProcess::operator()()
{
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	//Get system status from stkci middleware.
	display_out_speed_percent = 0.0;
	display_out_steer_percent = 0.0;
	display_out_joystick_lower_fork_status = 0.0;
	display_out_joystick_raise_fork_status = 0.0;

	//Get STKCI incoming message status and commands
	GetSystemStatus();

    //ONLY OPERATE in MANNED switch mode.
	if(!msg_health_status.error_joystick_notconnected)
	{
		if((msg_health_status.unmmaned_mode==false) && (msg_health_status.lost_aux_message_heartbeat == false))
		{
			if(msg_health_status.hw_estop_status==false)
			{
				//Add in get joystick drive and pallet forking commands
				sptr_Joystick->get_joystick_message( axis, button );

				//Pub function in terms of percentage for DBW commands using PubTeleOpDBWCmds()
				//TODO:: Pub function for pallet base commands using PubPalletBaseCmds()
				DecodeJoyStickKeys(axis,button);
			}
			else
			{
				//set zero to stop and steer dbw cmds using PubTeleOpDBWCmds
				PubTeleOpDBWCmds(0.0, 0.0);

				//reset pallet base command
				PubPalletBaseCmds(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0);
			}
		}//if manned mode
	}//joystick connected
	else
	{
		//set zero to stop and steer dbw cmds using PubTeleOpDBWCmds
		PubTeleOpDBWCmds(0.0, 0.0);

		//reset pallet base command
		PubPalletBaseCmds(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0);
	}

	//
	PrintStatusSummary();

	//time tracker only
	std::cout << 1/((current_timestamp-previous_timestamp)/1000.0) << " Hz Tele-op thread time " << std::endl;
	previous_timestamp =  current_timestamp;
}

void TeleOpsProcess::PrintStatusSummary()
{
	//system("clear");
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	std::cout << "System Time " << current_timestamp << std::endl;
	std::cout << " " << std::endl;

	if(msg_health_status.error_joystick_notconnected)
	{
		std::cout << "[Error] Joystick not connected " << std::endl;
	}
	//
	if(msg_health_status.unmmaned_mode)
	{
		std::cout << "Autonomous Mode " << std::endl;
	}
	else
	{
		std::cout << "TeleOp Mode" << std::endl;
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
	if(msg_health_status.lost_aux_message_heartbeat)
	{
		std::cout << "Message heartbeat is Not OK " << std::endl;
	}
	else
	{
		std::cout << "Message heartbeat is OK  " << std::endl;
	}

//	std::cout << " " << std::endl;
//	if(display_out_joystick_deadman_switch_status>0)
//	{
//		std::cout << "Deadman switch is Active " << std::endl;
//	}
//	else
//	{
//		std::cout << "Deadman switch is InActive  " << std::endl;
//	}

//	std::cout << "Max Speed allowable (m/s) " << config_params_.max_speed << std::endl;
//	std::cout << "Max Steer allowable (deg) " << config_params_.max_steer << std::endl;
//	std::cout << "Current speed " << current_speed << std::endl;
	std::cout << " " << std::endl;
	std::cout << "Current DBW command speed percentage " << display_out_speed_percent << std::endl;
	std::cout << "Current DBW command steer percentage " << display_out_steer_percent << std::endl;
	std::cout << "Current DBW command brake percentage " << display_out_brake_percentage << std::endl;
	std::cout << " " << std::endl;
	std::cout << "Lower fork percentage " << display_out_joystick_lower_fork_status << std::endl;
	std::cout << "Raise fork percentage " << display_out_joystick_raise_fork_status << std::endl;
	std::cout << "Move fork left percentage " << move_fork_to_left_percentage << std::endl;
	std::cout << "Move fork right percentage " << move_fork_to_right_percentage << std::endl;
	std::cout << "Tilt fork up percentage " << tilt_fork_to_up_percentage << std::endl;
	std::cout << "Tilt fork down percentage " << tilt_fork_to_down_percentage << std::endl;

	//
//	if(display_out_joystick_lower_fork_status>0)
//	{
//		std::cout << "Lower fork is Active " << std::endl;
//	}
//	else
//	{
//		std::cout << "Lower fork is InActive  " << std::endl;
//	}
//	if(display_out_joystick_raise_fork_status>0)
//	{
//		std::cout << "Raise fork is Active " << std::endl;
//	}
//	else
//	{
//		std::cout << "Raise fork is InActive  " << std::endl;
//	}
//	if(display_out_joystick_trigger_buzzer_status>0)
//	{
//		std::cout << "Sound buzzer is Active " << std::endl;
//	}
//	else
//	{
//		std::cout << "Sound buzzer is InActive  " << std::endl;
//	}

	if(!msg_health_status.error_joystick_notconnected)
	{
		//print debug summary
		std::cout << " " << std::endl;
		std::cout << "[drive] JOYSTICK_BUTTON_DEADMAN_SWITCH = " << current_joystick_deadman_switch_status << std::endl;
		std::cout << "JOYSTICK_BUTTON_SW_ESTOP = " << current_joystick_sw_estop_status << std::endl;
		std::cout << "JOYSTICK_BUTTON_LOWER_FORK = " << current_joystick_lower_fork_status << std::endl;
		std::cout << "JOYSTICK_BUTTON_RAISE_FORK = " << current_joystick_raise_fork_status << std::endl;
		//
		std::cout << "JOYSTICK_BUTTON_MOVE_FORK_LEFT = " << current_joystick_lower_fork_move_left_status << std::endl;
		std::cout << "JOYSTICK_BUTTON_MOVE_FORK_RIGHT = " << current_joystick_raise_fork_move_right_status << std::endl;
		std::cout << "JOYSTICK_BUTTON_MOVE_FORK_TILT_UP = " << current_joystick_lower_fork_tilt_up_status << std::endl;
		std::cout << "JOYSTICK_BUTTON_MOVE_FORK_TILT_DOWN = " << current_joystick_raise_fork_tilt_down_status << std::endl;
		//
		std::cout << "JOYSTICK_BUTTON_TRIGGER_BUZZER = " << current_joystick_trigger_buzzer_status << std::endl;
	//	std::cout << "JOYSTICK_AXIS_DRIVE_CMD_RANGE = " << current_joystick_drive_cmd_range << std::endl;
	//  std::cout << "JOYSTICK_AXIS_STEER_CMD_RANGE = " << current_joystick_steer_cmd_range << std::endl;
		std::cout << " " << std::endl;
	}
}

void TeleOpsProcess::DecodeJoyStickKeys(int axis[], char button[])
{
//	printf("axis[joystick_button_RT] = ", axis[joystick_button_RT]);
//	printf("axis[JOYSTICK_AXIS_TRIGGER_FORKTILT] = ", axis[JOYSTICK_AXIS_TRIGGER_FORKTILT]);
	//To information from the joystick
	current_joystick_deadman_switch_status = static_cast<int>(button[JOYSTICK_BUTTON_DEADMAN_SWITCH]);
	current_joystick_sw_estop_status = static_cast<int>(button[JOYSTICK_BUTTON_SW_ESTOP]);
	current_joystick_lower_fork_status = static_cast<float>(button[JOYSTICK_BUTTON_LOWER_FORK]);
	current_joystick_raise_fork_status = static_cast<float>(button[JOYSTICK_BUTTON_RAISE_FORK]);
	current_joystick_lower_fork_move_left_status = static_cast<float>(button[JOYSTICK_BUTTON_MOVE_FORK_LEFT]);
	current_joystick_raise_fork_move_right_status = static_cast<float>(button[JOYSTICK_BUTTON_MOVE_FORK_RIGHT]);
	if(axis[JOYSTICK_AXIS_TRIGGER_FORKTILT]>0)
	{
		current_joystick_lower_fork_tilt_up_status = static_cast<float>(button[JOYSTICK_BUTTON_MOVE_FORK_TILT_UP]);
		current_joystick_raise_fork_tilt_down_status = static_cast<float>(button[JOYSTICK_BUTTON_MOVE_FORK_TILT_DOWN]);
		current_joystick_lower_fork_move_left_status = 0;
		current_joystick_raise_fork_move_right_status = 0;
	}
	else
	{
		current_joystick_lower_fork_tilt_up_status = 0;
		current_joystick_raise_fork_tilt_down_status = 0;
	}
	current_joystick_trigger_buzzer_status = 0;
	//
	if(axis[JOYSTICK_AXIS_TRIGGER_BUZZER]>0)
	{
		current_joystick_trigger_buzzer_status = 1;
	}
	current_joystick_drive_cmd_range = static_cast<float>(axis[JOYSTICK_AXIS_DRIVE_CMD_RANGE]);
	current_joystick_steer_cmd_range = static_cast<float>(axis[JOYSTICK_AXIS_STEER_CMD_RANGE]);

	//Determine steer commands
	if (current_joystick_steer_cmd_range < 0 && current_joystick_deadman_switch_status == 1)
	{
		current_steer = current_steer + JOYSTICK_STEER_RESOLUTION;
		if(current_steer>=max_steer)
		{
			current_steer = max_steer;
		}
	}
	else if (current_joystick_steer_cmd_range > 0 && current_joystick_deadman_switch_status == 1)
	{
		current_steer = current_steer - JOYSTICK_STEER_RESOLUTION;
		if(current_steer<-max_steer)
		{
			current_steer = -max_steer;
		}
	}
	//else if (current_joystick_deadman_switch_status==0)
	else if ((current_joystick_deadman_switch_status==0) || (current_joystick_steer_cmd_range == 0 && current_joystick_deadman_switch_status == 1))
	{
		current_steer = 0;
	}

	//Determine move fork up & down commands
	lowerfork_percentage = 0.0;
	raisefork_percentage = 0.0;
	if(current_joystick_lower_fork_status > 0 && current_joystick_deadman_switch_status == 1)
	{
//		lower_fork = lower_fork - FORK_RESOLUTION;
//		if(lower_fork<-max_lowerfork)
//		{
//			lower_fork = -max_lowerfork;
//		}
//		lowerfork_percentage = lower_fork/config_params_.max_lowerfork;
//		lowerfork_percentage = -config_params_.max_lowerfork;
		lowerfork_percentage = 1.0;
	}

	if(current_joystick_raise_fork_status > 0 && current_joystick_deadman_switch_status == 1)
	{
//		raise_fork = raise_fork + FORK_RESOLUTION;
//		if(raise_fork>=max_raisefork)
//		{
//			raise_fork = max_raisefork;
//		}
//		raisefork_percentage = raise_fork/config_params_.max_raisefork;
//		raisefork_percentage = config_params_.max_raisefork;
		raisefork_percentage = 1.0;
	}


	//Determine move fork left & right commands
	move_fork_to_left_percentage = 0.0;
	move_fork_to_right_percentage = 0.0;
	if(current_joystick_lower_fork_move_left_status > 0 && current_joystick_deadman_switch_status == 1)
	{
		move_fork_to_left_percentage = 1.0;
	}

	if(current_joystick_raise_fork_move_right_status > 0 && current_joystick_deadman_switch_status == 1)
	{
		move_fork_to_right_percentage = 1.0;
	}

	//Determine tilt fork up & down commands
	tilt_fork_to_up_percentage = 0.0;
	tilt_fork_to_down_percentage = 0.0;
	if(current_joystick_lower_fork_tilt_up_status > 0 && current_joystick_deadman_switch_status == 1)
	{
		tilt_fork_to_up_percentage = 1.0;
	}
	if(current_joystick_raise_fork_tilt_down_status > 0 && current_joystick_deadman_switch_status == 1)
	{
		tilt_fork_to_down_percentage = 1.0;
	}

	//Determine drive commands
	current_speed_percentage = 0;
	if(current_joystick_drive_cmd_range < -10000 && current_joystick_deadman_switch_status == 1)//forward
	{
		//current_speed_percentage = current_joystick_drive_cmd_range/JOYSTICK_MAX_DRIVE_CMD_RANGE;
		current_speed_percentage = config_params_.max_speed;
	}
	else if(current_joystick_drive_cmd_range > 10000 && current_joystick_deadman_switch_status == 1)//backward
	{
		//current_speed_percentage = current_joystick_drive_cmd_range/JOYSTICK_MAX_DRIVE_CMD_RANGE;
		current_speed_percentage = -config_params_.max_speed;
	}
	else
	{
		current_speed_percentage = 0;
	}
//	else if(current_joystick_deadman_switch_status==0)
//	{
//		//current_speed_percentage = -0.0/JOYSTICK_MAX_DRIVE_CMD_RANGE;
//		current_speed_percentage = 0.0;
//	}

	//release of brake
	if(current_joystick_sw_estop_status == 0 && current_joystick_deadman_switch_status == 1 )
	{
		current_brake_percentage = 0.0; //fully released
	}

	//ESTOP
//	if (current_joystick_deadman_switch_status == 1 && current_joystick_sw_estop_status == 1)
//	{
//		//current_joystick_deadman_switch_status=0;
//		current_speed_percentage = 0.0;
//		current_brake_percentage = 1.0;
//		current_steer = 0.0;
//		lowerfork_percentage = 0.0;
//		raisefork_percentage = 0.0;
//		move_fork_to_left_percentage = 0.0;
//		move_fork_to_right_percentage = 0.0;
//		tilt_fork_to_up_percentage = 0.0;
//		tilt_fork_to_down_percentage = 0.0;
//	}

	//ESTOP
	if(current_joystick_sw_estop_status == 1)
	{
		current_speed_percentage = 0.0;
		current_brake_percentage = 1.0;
		current_steer = 0.0;
		lowerfork_percentage = 0.0;
		raisefork_percentage = 0.0;
		move_fork_to_left_percentage = 0.0;
		move_fork_to_right_percentage = 0.0;
		tilt_fork_to_up_percentage = 0.0;
		tilt_fork_to_down_percentage = 0.0;
	}

	//Determine auxillary control
	else if (current_joystick_deadman_switch_status == 0)
	{
		current_speed_percentage = 0.0;
		current_brake_percentage = 0.0;
		current_steer = 0.0;
		lowerfork_percentage = 0.0;
		raisefork_percentage = 0.0;
		move_fork_to_left_percentage = 0.0;
		move_fork_to_right_percentage = 0.0;
		tilt_fork_to_up_percentage = 0.0;
		tilt_fork_to_down_percentage = 0.0;
//		current_joystick_lower_fork_status = 0;
//		current_joystick_raise_fork_status = 0;
		current_joystick_trigger_buzzer_status = 0;
	}

	//map steer commands to dbw commands in percentage
	float64_t steer_percentage;
	steer_percentage = current_steer/config_params_.max_steer; //in deg

	//map fork commands to dbw commands in percentage
//	float64_t lowerfork_percentage;
//	lowerfork_percentage = lower_fork/config_params_.max_lowerfork;
//
//
//	float64_t raisefork_percentage;
//	raisefork_percentage = raise_fork/config_params_.max_raisefork;

	//Publish current tele-op commands
	display_out_speed_percent = current_speed_percentage;
	display_out_steer_percent = steer_percentage;
	PubTeleOpDBWCmds(current_speed_percentage, steer_percentage);

	//Publish forking commands
	display_out_joystick_deadman_switch_status=current_joystick_deadman_switch_status;
	display_out_joystick_lower_fork_status=lowerfork_percentage;
	display_out_joystick_raise_fork_status=raisefork_percentage;
	display_out_joystick_fork_move_left_status = move_fork_to_left_percentage;
	display_out_joystick_fork_move_right_status = move_fork_to_right_percentage;
	display_out_joystick_fork_tilt_up_status = tilt_fork_to_up_percentage;
	display_out_joystick_fork_tilt_down_status = tilt_fork_to_down_percentage;
	display_out_joystick_trigger_buzzer_status=current_joystick_trigger_buzzer_status;
	display_out_brake_percentage = current_brake_percentage;
	PubPalletBaseCmds_StillForklift(current_joystick_deadman_switch_status,
			lowerfork_percentage, raisefork_percentage,
			move_fork_to_left_percentage, move_fork_to_right_percentage,
			tilt_fork_to_up_percentage, tilt_fork_to_down_percentage,
			current_joystick_trigger_buzzer_status,
			current_speed_percentage, steer_percentage,
			current_brake_percentage);
}

void TeleOpsProcess::PubPalletBaseCmds_StillForklift(const int32_t trigger_tiller,
		const float64_t trigger_lower_fork,
		const float64_t trigger_raise_fork,
		const float64_t trigger_move_fork_left,
		const float64_t trigger_move_fork_right,
		const float64_t trigger_tilt_fork_up,
		const float64_t trigger_tilt_fork_down,
		const int32_t trigger_buzzer,
		const float64_t drive_speed_scale,
		const float64_t steer_scale,
		const float64_t brake_scale)
{
	Platform::Sensors::PalletBaseCmdMsg out_data_BaseCmdMsg;
	out_data_BaseCmdMsg.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	//
	bool bValveEnabled = false;
	if(trigger_tiller>0)
	{
		out_data_BaseCmdMsg.trigger_tiller=0x01;
		bValveEnabled = true;
	}
	else
	{
		out_data_BaseCmdMsg.trigger_tiller=0x00;
		bValveEnabled = false;
	}


	//To lower fork
	if((trigger_lower_fork>0) && (bValveEnabled==true))
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x02;
	}

	//To raise fork
	if((trigger_raise_fork>0) && (bValveEnabled==true))
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
			out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x04;
	}

	//To tilt fork forward
	if((trigger_tilt_fork_down>0) && (bValveEnabled==true))
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x10;
	}

	//To tilt fork back
	if((trigger_tilt_fork_up>0) && (bValveEnabled==true))
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x20;
	}

	//To move fork left
	if((trigger_move_fork_left>0) && (bValveEnabled==true))
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x80;
	}

	//To move fork right
	if((trigger_move_fork_right>0) && (bValveEnabled==true))
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x40;
	}

	//For steer control
	if((fabs(steer_scale)>0) && (bValveEnabled==true))
	{
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller & 0x01;
		out_data_BaseCmdMsg.trigger_tiller = out_data_BaseCmdMsg.trigger_tiller | 0x08;
	}

	//
	out_data_BaseCmdMsg.trigger_slowspeed = brake_scale;

	//
	if(trigger_tiller<0)
	{
		out_data_BaseCmdMsg.trigger_tiller=0x00;
	}

	printf("Send final tiller commands = %x \n", out_data_BaseCmdMsg.trigger_tiller);

	//
	seq_key_tracker_palletbase_cmd++;
	if(seq_key_tracker_palletbase_cmd>UINT16_MAX)
	{
		seq_key_tracker_palletbase_cmd=0;
	}

	//
	ptr_stkci_pub->PubPalletBaseCmd(config_params_.PubStkciMsgs["PalletBaseCmdMsg"].senderid, out_data_BaseCmdMsg);

}

///\brief send the drive and steer command via Platform::Sensors::PalletDbwCmdMsg message
///\param[in] drive_scale range from -1.0 to 1.0
///\param[in] steer_scale range from -1.0 to 1.0
void TeleOpsProcess::PubTeleOpDBWCmds(float64_t drive_speed_scale, float64_t steer_scale)
{
	Platform::Sensors::PalletDbwCmdMsg out_data_DbwCmdMsg;
	out_data_DbwCmdMsg.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	out_data_DbwCmdMsg.max_speed = config_params_.max_speed;//m/s
	out_data_DbwCmdMsg.max_steer = config_params_.max_steer*3.141/180.0; //radians
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
	ptr_stkci_pub->PubPalletDbwCmd(config_params_.PubStkciMsgs["DBWCmdMsg"].senderid, out_data_DbwCmdMsg);
}

///\brief send the fork and tiller stop trigger via Platform::Sensors::PalletBaseCmdMsg message
///\param[in] trigger_tiller activated when 1 otherwise 0, drive when high
///\param[in] trigger_lower_fork activated when 1 otherwise 0
///\param[in] trigger_raise_fork activated when 1 otherwise 0
///\param[in] trigger_buzzer activated when 1 otherwise 0
void TeleOpsProcess::PubPalletBaseCmds(const int32_t trigger_tiller,
		const float64_t trigger_lower_fork,
		const float64_t trigger_raise_fork,
		const float64_t trigger_move_fork_left,
		const float64_t trigger_move_fork_right,
		const float64_t trigger_tilt_fork_up,
		const float64_t trigger_tilt_fork_down,
		const int32_t trigger_buzzer,
		const float64_t drive_speed_scale,
		const float64_t steer_scale,
		const float64_t brake_scale)
{
	Platform::Sensors::PalletBaseCmdMsg out_data_BaseCmdMsg;
	out_data_BaseCmdMsg.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	if(trigger_tiller>0)
	{
		out_data_BaseCmdMsg.trigger_tiller=0x01;
	}
	else
	{
		out_data_BaseCmdMsg.trigger_tiller=0x00;
	}
	out_data_BaseCmdMsg.trigger_slowspeed = trigger_lower_fork;
	out_data_BaseCmdMsg.trigger_belly = trigger_raise_fork;
//	if(trigger_lower_fork>0)
//	{
//		out_data_BaseCmdMsg.trigger_lower_fork=0x01;
//	}
//	else
//	{
//		out_data_BaseCmdMsg.trigger_lower_fork=0x00;
//	}
//	if(trigger_raise_fork>0)
//	{
//		out_data_BaseCmdMsg.trigger_raise_fork=0x01;
//	}
//	else
//	{
//		out_data_BaseCmdMsg.trigger_raise_fork=0x00;
//	}
	if(trigger_buzzer>0)
	{
		out_data_BaseCmdMsg.trigger_buzzer=0x01;
	}
	else
	{
		out_data_BaseCmdMsg.trigger_buzzer=0x00;
	}
	out_data_BaseCmdMsg.seq_key = seq_key_tracker_palletbase_cmd;

	//
	seq_key_tracker_palletbase_cmd++;
	if(seq_key_tracker_palletbase_cmd>UINT16_MAX)
	{
		seq_key_tracker_palletbase_cmd=0;
	}

	//
	ptr_stkci_pub->PubPalletBaseCmd(config_params_.PubStkciMsgs["PalletBaseCmdMsg"].senderid, out_data_BaseCmdMsg);
}

void TeleOpsProcess::GetPLMHealthStatus(dbw_liveness_status &status)
{
	status = msg_health_status;
}

void TeleOpsProcess::GetSystemStatus()
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
		msg_health_status.not_receive_vimstatus_msg_count=0;
	}
	else
	{
		msg_health_status.receive_vimstatus_msg=false;
		msg_health_status.not_receive_vimstatus_msg_count++;
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
	if((msg_health_status.receive_vimstatus_msg==false) && (msg_health_status.first_receive_vimstatus_msg))
	{
		msg_health_status.lost_aux_message_heartbeat=true;
	}
}

TeleOpsProcess::~TeleOpsProcess()
{

}

}

