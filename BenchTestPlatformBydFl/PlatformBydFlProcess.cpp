/*
 * PlatformBydFlProcess.cpp
 *
 *  Created on: July 1, 2019
 *      Author: Tester
 */

#include "PlatformBydFlProcess.h"
#include "StkciPCMPalletBoundary.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>
//
#include "mu_serial.h"
#include "mu_common.h"
#include "smartmotor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



namespace PlatformBydFl
{

PlatformBydFlProcess::PlatformBydFlProcess()
{
	bBrakeMotorEnable = false;
}

void PlatformBydFlProcess::SetConfig(const PlatformBydFlConfig config)
{
	config_params = config;

	//Open port
	sptr_ADAM_ROController_DrvPedal = std::make_shared<DBWPalletCommon::Adam_ROController>();
	sptr_ADAM_AOController_DrvSteer_Voltage = std::make_shared<DBWPalletCommon::Adam_AOController>();
	sptr_ADAM_ROController_Gear = std::make_shared<DBWPalletCommon::Adam_ROController>();
	sptr_ADAM_AOController_Fork_Ctrl = std::make_shared<DBWPalletCommon::Adam_AOController>();
	sptr_ADAM_ROController_Fork_Ctrl = std::make_shared<DBWPalletCommon::Adam_ROController>();
	//
	sptr_PcmPalletController = std::make_shared<DBWPalletCommon::PCMPalletController>();

	//
	sptr_DIODriver = std::make_shared<Platform::IsolatedDIO::IsolatedDIODriver>();


	//
	sptr_RetrieveBaseFeedback_ = std::make_shared<RetrieveBaseFeedback>("PIM");

	sptr_SmartMotorSteer = std::make_shared<DBW_Ctrl>();

	if(params_brake_config_.params_SmartMotor_Enabled.steer_enable == 1)
	{
		sptr_SmartMotorSteer->init_throttle_motor((char*)params_brake_config_.params_SmartMotor_Port.steer_motor_device.c_str());
		//msg_health_status.bConnectedSteerMotor = true;
	}





	//open vecow port
	if(!sptr_PcmPalletController->OpenVecowCanPortModule())
	{
		std::cout << "Cannot open can port !! " << std::endl;
		exit(1);
	}

	//open serial port (for brake)
//	std::string str = "/dev/ttyS1"; //serial com port 2
//	bBrakeMotorEnable = true;
//
//	if(bBrakeMotorEnable)
//	{
//		brake_motor = mu_smartmotor_connect_serial((char *)str.c_str());
//		if(brake_motor == NULL)
//			mu_kill("Failed to connect to brake motor at %s\n", (char *)str.c_str());
//		fprintf(stderr,"Connected to Brake SmartMotor\n");
//	}

//	mu_smartmotor_initialize_mt(brake_motor);

//	std::string str = "/dev/ttyS1"; //serial com port 2
//	if(mu_serial_connect(&fd, (char *)str.c_str(), 9600) > 0)
//	{
//		bBrakeMotorEnable = false;
//		std::cout << "Serial Port open failed" << std::endl;
//	}
//	else
//	{
//		bBrakeMotorEnable = true;
//		std::cout << "Serial Port open success" << std::endl;
//	}

	// Init Adam Controllers
	success_setup_adam_controller = false;
	success_setup_adam_controller = InitAdamController();
	bStart = true;
	bSteer = true;
	bStopSteer = true;
	bForkControl = false;
	bSteerControl = false;
	bActivateForkControl = false;
	bActivateSteerControl = false;
	bDeActivateValveControl = false;
	bTest = true;
	bDrive = true;
	test1 = false;


	//Set middleware publisher
	ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
	std::vector<std::string> interface_list;
	interface_list.push_back("PalletBaseFb");
	interface_list.push_back("PalletAuxCmd");
//	interface_list.push_back("TiltMotorMsg");
	ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);

	//
	//init previous paramters
	previous_in_data_dbwcmd.cmd_speed = 0.0;
	previous_in_data_dbwcmd.cmd_steer = 0.0;
	previous_gear_position = DbwGear::NEUTRAL;

	//
	previous_data_forkbasecmd.trigger_tiller = 0x00;
	previous_data_steerDBWcmd.cmd_steer = 0.0;

//	bool bchk = false;
//	bchk = sptr_DIODriver->initDIO();
//	if (bchk)
//	{
//		std::cout << "Success open Vecow DIO Port " << std::endl;
//		//msg_health_status.open_dio_port = true;
//	}
//	else
//	{
//		std::cout << "Fail to open Vecow DIO Port " << std::endl;
//		//msg_health_status.open_dio_port = false;
//		//exit(1);
//	}

	sptr_PcmPalletController->ResetLastCommandSpeed();
	if(config_params.debug.enabled==1)
	{
		sptr_PcmPalletController->SetLogger(config_params.debug.log_folder);
	}

}

bool PlatformBydFlProcess::InitAdamController()
{
	uint16_t typecode_value[4] = {0};
	float startup_value[4] = {0};
	float ao_value[4] = {0};
	uint8_t status[6] = {0};
	//
	bool bResult = false;
	//
	bool bSuccess_connect_DrvPedal = sptr_ADAM_ROController_DrvPedal->connect("10.0.0.2", 502);
	bool bSuccess_connect_DrvSteer_volt = sptr_ADAM_AOController_DrvSteer_Voltage->connect("10.0.0.3", 502);
	bool bSuccess_connect_R0_Fork_Ctrl = sptr_ADAM_ROController_Fork_Ctrl->connect("10.0.0.4" , 502);
	bool bSuccess_connect_AO_Fork_Ctrl = sptr_ADAM_AOController_Fork_Ctrl->connect("10.0.0.5", 502);
	bool bSuccess_connect_Gear = sptr_ADAM_ROController_Gear->connect("10.0.0.6", 502);

	//Open Adam controller
	//
	if (bSuccess_connect_DrvPedal)
	{
		std::cout << "\nConnection 10.0.0.2 Successful" << std::endl;
		bResult = true;
	}
	else
	{
		std::cout << "Connection 10.0.0.2 Failed\n" << std::endl;
		bResult = false;
	}
	if (bSuccess_connect_DrvPedal)
	{
		sptr_ADAM_ROController_DrvPedal->read_adam_6260_ro_status(status);
	}

	printf( "---------- Printing 10.0.0.2 Status ----------\n" );

	printf( "[Channel 0] Relay Output = 0x%X\n" , status[0] );
	printf( "[Channel 1] Relay Output = 0x%X\n" , status[1] );
	printf( "[Channel 2] Relay Output = 0x%X\n" , status[2] );
	printf( "[Channel 3] Relay Output = 0x%X\n" , status[3] );
	printf( "[Channel 4] Relay Output = 0x%X\n" , status[4] );
	printf( "[Channel 5] Relay Output = 0x%X\n" , status[5] );

	printf( "---------- Printing 10.0.0.2 Status ----------\n" );

	//
	if (bSuccess_connect_DrvSteer_volt)
	{
		std::cout << "\nConnection 10.0.0.3 Successful" << std::endl;
		bResult = true;
	}
	else
	{
		std::cout << "Connection 10.0.0.3 Failed\n" << std::endl;
		bResult = false;
	}
	if (bSuccess_connect_DrvSteer_volt)
	{
		sptr_ADAM_AOController_DrvSteer_Voltage->read_aotypecode(typecode_value);
		sptr_ADAM_AOController_DrvSteer_Voltage->read_startupvalue(startup_value);
		sptr_ADAM_AOController_DrvSteer_Voltage->read_aovalue(ao_value);
	}

	printf( "---------- Printing 10.0.0.3 Status ----------\n" );

	printf( "[Channel 0] Type Code = 0x%X\n" , typecode_value[0] );
	printf( "[Channel 1] Type Code = 0x%X\n" , typecode_value[1] );
	printf( "[Channel 2] Type Code = 0x%X\n" , typecode_value[2] );
	printf( "[Channel 3] Type Code = 0x%X\n" , typecode_value[3] );

	printf( "[Channel 0] Startup Value = %f\n" , startup_value[0] );
	printf( "[Channel 1] Startup Value = %f\n" , startup_value[1] );
	printf( "[Channel 2] Startup Value = %f\n" , startup_value[2] );
	printf( "[Channel 3] Startup Value = %f\n" , startup_value[3] );

	printf( "[Channel 0] AO Voltage = %f\n" , ao_value[0] );
	printf( "[Channel 1] AO Voltage = %f\n" , ao_value[1] );
	printf( "[Channel 2] AO Voltage = %f\n" , ao_value[2] );
	printf( "[Channel 3] AO Voltage = %f\n" , ao_value[3] );

	printf( "---------- Printing 10.0.0.3 Status ----------\n" );

	//
//	if (bSuccess_connect_R0_Fork_Ctrl)
//	{
//		std::cout << "\nConnection 10.0.0.4 Successful" << std::endl;
//		bResult = true;
//	}
//	else
//	{
//		std::cout << "Connection 10.0.0.4 Failed\n" << std::endl;
//		bResult = false;
//	}
//	if (bSuccess_connect_R0_Fork_Ctrl)
//	{
//		sptr_ADAM_ROController_Fork_Ctrl->read_adam_6260_ro_status(status);
//	}
//
//	printf( "---------- Printing 10.0.0.4 Status ----------\n" );
//
//	printf( "[Channel 0] Relay Output = 0x%X\n" , status[0] );
//	printf( "[Channel 1] Relay Output = 0x%X\n" , status[1] );
//	printf( "[Channel 2] Relay Output = 0x%X\n" , status[2] );
//	printf( "[Channel 3] Relay Output = 0x%X\n" , status[3] );
//	printf( "[Channel 4] Relay Output = 0x%X\n" , status[4] );
//	printf( "[Channel 5] Relay Output = 0x%X\n" , status[5] );
//
//	printf( "---------- Printing 10.0.0.4 Status ----------\n" );
//
//	//
//	if (bSuccess_connect_AO_Fork_Ctrl)
//	{
//		std::cout << "\nConnection 10.0.0.5 Successful" << std::endl;
//		bResult = true;
//	}
//	else
//	{
//		std::cout << "Connection 10.0.0.5 Failed\n" << std::endl;
//		bResult = false;
//	}
//	if (bSuccess_connect_AO_Fork_Ctrl)
//	{
//		sptr_ADAM_AOController_Fork_Ctrl->read_aotypecode(typecode_value);
//		sptr_ADAM_AOController_Fork_Ctrl->read_startupvalue(startup_value);
//		sptr_ADAM_AOController_Fork_Ctrl->read_aovalue(ao_value);
//	}
//
//	printf( "---------- Printing 10.0.0.5 Status ----------\n" );
//
//	printf( "[Channel 0] Type Code = 0x%X\n" , typecode_value[0] );
//	printf( "[Channel 1] Type Code = 0x%X\n" , typecode_value[1] );
//	printf( "[Channel 2] Type Code = 0x%X\n" , typecode_value[2] );
//	printf( "[Channel 3] Type Code = 0x%X\n" , typecode_value[3] );
//
//	printf( "[Channel 0] Startup Value = %f\n" , startup_value[0] );
//	printf( "[Channel 1] Startup Value = %f\n" , startup_value[1] );
//	printf( "[Channel 2] Startup Value = %f\n" , startup_value[2] );
//	printf( "[Channel 3] Startup Value = %f\n" , startup_value[3] );
//
//	printf( "[Channel 0] AO Voltage = %f\n" , ao_value[0] );
//	printf( "[Channel 1] AO Voltage = %f\n" , ao_value[1] );
//	printf( "[Channel 2] AO Voltage = %f\n" , ao_value[2] );
//	printf( "[Channel 3] AO Voltage = %f\n" , ao_value[3] );
//
//	printf( "---------- Printing 10.0.0.5 Status ----------\n" );

	//
	if (bSuccess_connect_Gear)
	{
		std::cout << "\nConnection 10.0.0.6 Successful" << std::endl;
		bResult = true;
	}
	else
	{
		std::cout << "Connection 10.0.0.6 Failed\n" << std::endl;
		bResult = false;
	}
	if (bSuccess_connect_Gear)
	{
		sptr_ADAM_ROController_Gear->read_adam_6266_ro_status(status);
	}

	printf( "---------- Printing 10.0.0.6 Status ----------\n" );

	printf( "[Channel 0] Relay Output = 0x%X\n" , status[0] );
	printf( "[Channel 1] Relay Output = 0x%X\n" , status[1] );
	printf( "[Channel 2] Relay Output = 0x%X\n" , status[2] );
	printf( "[Channel 3] Relay Output = 0x%X\n" , status[3] );

	printf( "---------- Printing 10.0.0.6 Status ----------\n" );


	//Set Drive Pedal Relay
	if (bResult)
	{
		//Set safety voltage for acceleration
	//	sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(0, 0);

		//Set relay for the acceleration
//		sptr_ADAM_ROController_DrvPedal->write_ro_status(0, false);
//		sptr_ADAM_ROController_DrvPedal->write_ro_status(2, false);
//		sptr_ADAM_ROController_DrvPedal->write_ro_status(3, false);
//		sptr_ADAM_ROController_DrvPedal->write_ro_status(4, false);
//		sptr_ADAM_ROController_DrvPedal->write_ro_status(5, false);

		//Set Safety Voltage for fork control
//		sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 2.5);
//		sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(1, 2.5);
//		sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(2, 2.5);
//		sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 2.5);

	}

	return bResult;

}

void PlatformBydFlProcess::InitSharedPtrs(std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds,
										 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds,
										 std::shared_ptr<RetreiveTiltMotorFbkMsgs> sptr_RetreiveTiltMotorFbkMsgs)
{
	sptr_RetreiveDBWCmds_ = sptr_RetreiveDBWCmds;
	sptr_RetreiveBaseCmds_= sptr_RetreiveBaseCmds;
	sptr_RetreiveTiltMotorFbkMsgs_ = sptr_RetreiveTiltMotorFbkMsgs;
}

void PlatformBydFlProcess::operator()()
{
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();
	Platform::Sensors::TiltMotorMsg tilt_motor;
	Platform::Sensors::TiltMotorFbkMsg tilt_motor_fbk;

//
//	float cmd_speed_ = 0.0;
//	float ch0_voltage = 0.0;
//	tilt_motor.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
//	tilt_motor.seq_key = 0;
//	tilt_motor.position = 90;
//	tilt_motor.speed = 30;

	//stkci messages
	Platform::Sensors::PalletDbwCmdMsg in_data_dbwcmd;
	Platform::Sensors::PalletBaseCmdMsg data_basecmd;

//	//Get Palletbase commands
//	sptr_RetreiveBaseCmds_->GetBaseCmdMsg(data_basecmd);

//	FeedbackDBWandPalletBaseStatus();
	GetDriveCmdsAndStatus();

//	ptr_stkci_pub->PubTiltMotorMsg("TiltMotorMsg", tilt_motor);
//	std::cout << "tilt motor publish" << std::endl;
//
//	if(palletbase_cmd.trigger_slowspeed == 1)
//	{
//		sptr_PcmPalletController->ServoGoToPos(90,30,1);
////		sptr_PcmPalletController->GetTiltMotorPos();
//
//////
//	while(ros::ok)
//	{
//		if(sptr_RetreiveTiltMotorFbkMsgs_->GetTiltMotorFbkMsgs(tilt_motor_fbk))
//		{
//			std::cout << "tilt_motor_position = " << tilt_motor_fbk.position << std::endl;
//		}
//	}
//
//	}

	//Get DBW commands
//	//decodeSteer();
//
//	cmd_speed_ = dbw_cmd.cmd_speed;
//
//	std::cout << "palletbase_cmd.trigger_tiller = " << palletbase_cmd.trigger_tiller << std::endl;
//
//	CommandVehSpeedChange(dbw_cmd);
//	CmdSpeedToVoltage(dbw_cmd.cmd_speed, ch0_voltage);
//	SetPIMCommands(dbw_cmd, palletbase_cmd);
//
	bool bControlFlag  = (palletbase_cmd.trigger_tiller <0x01) ? true : false;
	if(bControlFlag)
	{
		//StopDriveControl();
		stopControls();
	}
	else
	{
		if(palletbase_cmd.trigger_tiller!=0x01)
		{
			bool bSteerFlag = ((palletbase_cmd.trigger_tiller == 0x09)) ? true : false;

			if(bSteerFlag)
			{
				SteerControl(dbw_cmd);
			}
			else
			{
				ForkControl(palletbase_cmd);
			}
		}
	}
//	int abs_pos = 0;
//
//	char command[16];
//	int temp=1;
//	int count=0;
//	int digit[16],digit_num=0, digit_size;
//
//	if(abs_pos>=0){
//
//		while(temp!=0){
//			temp = abs_pos/10;
//			digit[count] = abs_pos-temp*10;
//			std::cout << "digit[" << count << "] = "  << digit[count] << std::endl;
//			abs_pos = temp;
//			count++;
//
//		}
//		digit_size = count;
//
//		while(count!=0){
//			command[digit_num+3] = digit[count-1]+0x30;
//			count--;
//			digit_num++;
//
//		}
//
//		command[0]=0x50;//P
//		command[1]=0x54;//T
//		command[2]=0x3D;//=
////		command[digit_size+3]=0x20;//SP
//
//		//fd_writen(sm, (unsigned char *)command, 10);
////		memset(command,0x00,sizeof(command));
////		usleep(10);
////		command[0]=0x47;//G
////		command[1]=0x20;//SP = space
//	}
//	else if(abs_pos<0){
//
//		abs_pos = abs(abs_pos);
//
//		while(temp!=0){
//			temp = abs_pos/10;
//			digit[count] = abs_pos-temp*10;
//			abs_pos = temp;
//			count++;
//
//		}
//		digit_size = count;
//
//		while(count!=0){
//			command[digit_num+4] = digit[count-1]+0x30;
//			count--;
//			digit_num++;
//
//		}
//
//		command[0]=0x50;//P
//		command[1]=0x54;//T
//		command[2]=0x3D;//=
//		command[3]=0x2D;//-
//		command[digit_size+4]=0x20;
//
//		//fd_writen(sm, (unsigned char *)command, 10);
////		memset(command,0x00,sizeof(command));
////		usleep(10);
////		command[0]=0x47;//G
////		command[1]=0x20;//SP
//	}
//

//	std::cout << palletbase_cmd.trigger_slowspeed << std::endl;



//	int ch;
//	int vel = 0;
//	int pos = 0;
//	//ch = getchar();
//
//	if(palletbase_cmd.trigger_slowspeed == 1)
//	{
//		sptr_SmartMotorSteer->set_motor_torque();
//		sptr_SmartMotorSteer->set_throttle_motor_torque(3500);
//		sptr_SmartMotorSteer->set_throttle_motor_torque_slope(100000);
//		vel = sptr_SmartMotorSteer->get_motor_velocity();
//		if(fabs(vel)<10000)
//		{
//			std::cout <<"done" << std::endl;
//			sptr_SmartMotorSteer->set_torque_motor_stop();
//		}
//
////		while(!test1)
////		{
////			std::cout << "enter" << std::endl;
////			vel = sptr_SmartMotorSteer->get_motor_velocity();
////			if((vel)<1500)
////			{
////				test1 = true;
////				std::cout <<"done" << std::endl;
////				sptr_SmartMotorSteer->set_torque_motor_stop();
////			}
////		}
//	}
//	else if (palletbase_cmd.trigger_slowspeed == 0)
//	{
//
//		sptr_SmartMotorSteer->set_motor_position();
//		sptr_SmartMotorSteer->set_throttle_motor_pos(0,0);
//	}

//	if(ch == 't' || ch == 'T')
//	{
//		test1 = false;
//		std::cout << "test t" << std::endl;
//		sptr_SmartMotorSteer->set_motor_torque();
//		sptr_SmartMotorSteer->set_throttle_motor_torque(3000);
//		while(!test1)
//		{
//			std::cout << "enter" << std::endl;
//			vel = sptr_SmartMotorSteer->get_motor_velocity();
//			if((vel)<1500)
//			{
//				test1 = true;
//				std::cout <<"done" << std::endl;
//				sptr_SmartMotorSteer->set_torque_motor_stop();
//			}
//		}
//	}
//
//
//	if(ch == 'p' || ch == 'P')
//	{
//		std::cout << "test p" << std::endl;
//		sptr_SmartMotorSteer->set_motor_position();
//		sptr_SmartMotorSteer->set_throttle_motor_pos(0,0);
//	}
//
//	if(ch == 'g' || ch == 'G')
//	{
//		std::cout << "test g" << std::endl;
//		pos =sptr_SmartMotorSteer->get_motor_position();
//		vel = sptr_SmartMotorSteer->get_motor_velocity();
//	}
//
//	if(ch == 'q' || ch == 'Q')
//	{
//		std::cout << "test q" << std::endl;
//		sptr_SmartMotorSteer->set_torque_motor_stop();
//	}


//	std::cout << current_timestamp-previous_timestamp
//			<< " ms main loop thread time " << std::endl;
//	previous_timestamp =  current_timestamp;
}

void PlatformBydFlProcess::SetConfigParams(DBWPalletCommon::SmartMotorSteerConfig params_brake_config)
{
	params_brake_config_ = params_brake_config;
}

//
void PlatformBydFlProcess::SetPIMCommands(const Platform::Sensors::PalletDbwCmdMsg &dbw_cmd_, const Platform::Sensors::PalletBaseCmdMsg &palletbase_cmd_)
{
//    uint8_t temp_output[8];
//    uint8_t output_DIO = 0;
//	uint8_t in_dio_feedback;


	Platform::Sensors::PalletAuxCmdMsg aux_cmd;
	aux_cmd.timestamp = Common::Utility::Time::getusecCountSinceEpoch();

	aux_cmd.trigger_left_direction_light = 0x00;
	aux_cmd.trigger_right_direction_light = 0x00;

	if(dbw_cmd.cmd_steer == 0)
	{
		aux_cmd.trigger_left_direction_light = 0x00;
		aux_cmd.trigger_right_direction_light = 0x00;
	}
	else if (dbw_cmd.cmd_steer > 0)
	{
		aux_cmd.trigger_left_direction_light = 0x01;
		aux_cmd.trigger_right_direction_light = 0x00;
	}
	else if (dbw_cmd.cmd_steer < 0)
	{
		aux_cmd.trigger_left_direction_light = 0x00;
		aux_cmd.trigger_right_direction_light = 0x01;
	}

	if(dbw_cmd.cmd_speed < 0)
	{
		aux_cmd.trigger_left_direction_light = 0x01;
		aux_cmd.trigger_right_direction_light = 0x01;
	}

//	temp_output[ 0 ] = ( aux_cmd.trigger_left_direction_light & 0x01) ;
//	temp_output[ 1 ] = ( aux_cmd.trigger_right_direction_light & 0x01 ) << 1;	// output bit 1 - right direction light
//	temp_output[ 2 ] = 0x00;		// output bit 2 - left direction light
//	temp_output[ 3 ] = 0x00;
//	temp_output[ 4 ] = 0x00;
//	temp_output[ 5 ] = 0x00;
//	temp_output[ 6 ] = 0x00;
//	temp_output[ 7 ] = 0x00;
//
//	for (int i = 0; i < 8; i++)
//	{
//		output_DIO = output_DIO | temp_output[ i ];
//	}
//
//	sptr_DIODriver->SetDIO_OutputState(output_DIO);
//	sptr_DIODriver->GetDIO_Output(&in_dio_feedback);


	ptr_stkci_pub->PubPalletAuxCmd("DBW", aux_cmd);
}

void PlatformBydFlProcess::decodeSteer()
{
	uint32_t current_steer_enc_pos_ = steer_enc_pos;

	uint32_t home_steer_enc_pos = 33545967; //33545967
	uint32_t lpos_steer_enc_pos = 33545352; //33545349
	uint32_t rpos_steer_enc_pos = 33546913; //33546921
	uint32_t left_difference = abs(home_steer_enc_pos - lpos_steer_enc_pos);
	uint32_t right_difference = abs(home_steer_enc_pos - rpos_steer_enc_pos);


	float steer_tolerance = 0.05; //0.05

	uint32_t desired_enc_pos = home_steer_enc_pos;
	//

	/*---------- decode steer start ----------*/
		if (dbw_cmd.cmd_steer < -1.0)
		{
			dbw_cmd.cmd_steer = -1.0;
		}
		else if (dbw_cmd.cmd_steer > 1.0)
		{
			dbw_cmd.cmd_steer = 1.0;
		}

		float desired_steer_percent = dbw_cmd.cmd_steer;

		if (desired_steer_percent == 0)										// home
		{
			desired_enc_pos = home_steer_enc_pos;
		}
		else if (desired_steer_percent < 0)									// left steer from home
		{
			desired_enc_pos = home_steer_enc_pos - ((home_steer_enc_pos - lpos_steer_enc_pos) * fabs(desired_steer_percent));
		}
		else if (desired_steer_percent > 0)									// right steer from home
		{
			desired_enc_pos = home_steer_enc_pos + ((rpos_steer_enc_pos - home_steer_enc_pos) * fabs(desired_steer_percent));
		}


			//if (current_steer_enc_pos_ == desired_enc_pos)
			if (current_steer_enc_pos_ > (desired_enc_pos - (left_difference * steer_tolerance)) &&
							current_steer_enc_pos_ < (desired_enc_pos + (right_difference * steer_tolerance)))
			{
				palletbase_cmd.trigger_tiller = 0x00;
				dbw_cmd.cmd_steer = 0;
			}
			else
			{
//				if(palletbase_cmd.trigger_tiller == 0x09)
//				{

				if(current_steer_enc_pos_!= 0)
				{
					palletbase_cmd.trigger_tiller = 0x09;

					// dbw_cmd.cmd_steer > 0 left turn
					// dbw_cmd.cmd_steer < 0 right turn


					if (current_steer_enc_pos_ < desired_enc_pos)
					{
						dbw_cmd.cmd_steer = -1;

						std::cout << "steering right" << std::endl;
					}
					else if (current_steer_enc_pos_ > desired_enc_pos)
					{
						dbw_cmd.cmd_steer = 1;

						std::cout << "steering left" << std::endl;
					}
				}
//				}
			}

		std::cout << "desired_enc_pos = " << desired_enc_pos << std::endl;
		std::cout << "current_steer_enc_pos_ = " << current_steer_enc_pos_ << std::endl;

		/*---------- decode steer end ----------*/
}

//set smart motor to position mode
void PlatformBydFlProcess::SetMotorMode(int acc, int vel, motor_mode mode)
{
	if (mode == position_mode)
	{
//		mu_smartmotor_initialize(brake_motor, acc, vel);
	}
	std::cout << "Enter ---------------------  " << mode << std::endl;
}

void PlatformBydFlProcess::SetMotorPos(int pos, int complete_traj)
{
	mu_smartmotor_set_abs_pos(brake_motor, pos);

	if (complete_traj)
		WaitTrajBitClear(brake_motor);

	std::cout << "Pos ---------------------  " << pos << std::endl;
}

void PlatformBydFlProcess::WaitTrajBitClear(mu_smartmotor_p brake_motor)
{
	int TrajBit;

	while (TrajBit!=0)
	{
		TrajBit = get_motor_trajBit(brake_motor);
		usleep(1000);
	};
}

int PlatformBydFlProcess::get_motor_trajBit(mu_smartmotor_p brake_motor)
{
	char buffer[1000];
	int nread;
	int TrajBit=-1;

	//pthread_mutex_lock(&motor_mutex);
	mu_smartmotor_write(brake_motor, (unsigned char*)("RBt\n"));
	nread = mu_smartmotor_readn(brake_motor, (unsigned char *)buffer, 1000);
	//pthread_mutex_unlock(&motor_mutex);

	if (nread > 0)
	{
		buffer[nread] = '\0';
		TrajBit = atoi(buffer);
		//fprintf(stderr,"nread:%d\nbuffer:%s\nTrajBit:%d\n",nread,buffer,TrajBit);
	}

	return TrajBit;
}

////set smart motor current position
//void PlatformBydFlProcess::SetMotorPos(int pos, int complete_traj)
//{
//	mu_smartmotor_set_abs_pos(brake_motor, pos);
//	std::cout << "Position ================= " << pos <<  std::endl;
//}
//
////get smart motor current position
//int PlatformBydFlProcess::GetMotorPos(mu_smartmotor_p brake_motor)
//{
//	char buffer[1000];
//	int nread;
//	int cur_pos;
//
//	char command[16];
//	memset(command,0x00,sizeof(command));
//	command[0]=0x52;
//	command[1]=0x43;
//	command[2]=0x54;
//	command[3]=0x52;
//	command[4]=0x28;
//	command[5]=0x30;
//	command[6]=0x29;
//	command[7]=0x20;
//
//	mu_smartmotor_bytes_write(brake_motor, (unsigned char*)command,8);
//
//	int i;
//	for(i=0; i<1000; i++)
//	{
//		nread = mu_smartmotor_readn(brake_motor, (unsigned char *)buffer, 1000);
//		usleep(10);
//		if(nread>0)
//		{
//			break;
//		}
//	}
//
//	printf("i = %d nread = %d \n",i, nread);
//	for(int i=0; i<nread; i++)
//	{
//		printf("%x ", buffer[i]);
//	}
//	printf("\n");
//	//
//	if (nread > 0)
//	{
//		buffer[nread] = '\0';
//		cur_pos = atoi(buffer);
//	}
//
//	return cur_pos;
//}

//Obtain absolute position of the brake motor
//void PlatformBydFlProcess::GetBrakeMotorPos()
//{
////	int cur_pos;
//
//	GetMotorPos(brake_motor);
//	std::cout << "Get Position =========================================== " << std::endl;
//
////	return cur_pos;
//}
//
//// Apply Brake
//void PlatformBydFlProcess::BrakeON(int pos)
//{
//	if (bBrakeMotorEnable)
//	{
//		SetMotorPos(pos, 0);
//	}
//	std::cout << "Brake On ---------------------------------- " << std::endl;
//}
//
//// Disengage the brake
//void PlatformBydFlProcess::BrakeOFF(void)
//{
//	if (bBrakeMotorEnable)
//	{
//		SetMotorPos(0, 0);
//	}
//}

//
void PlatformBydFlProcess::CommandVehSpeedChange(const Platform::Sensors::PalletDbwCmdMsg dbw_cmd)
{
	int32_t desired_gear_state;


	std::cout << "dbw speed = " << dbw_cmd.cmd_speed << std::endl;
	//Apply brake
	if(fabs(dbw_cmd.cmd_speed)==0.0)
	{
		//TODO:: send brake command

		desired_gear_state = DbwGear::NEUTRAL; //1
		if(desired_gear_state != previous_gear_position)
		{
			ChangeGear(desired_gear_state);
			previous_gear_position = desired_gear_state;
		}
	}//
	else if(dbw_cmd.cmd_speed>0)
	{
		desired_gear_state = DbwGear::FORWARD; //2
		if(desired_gear_state != previous_gear_position)
		{
			ChangeGear(desired_gear_state);
			previous_gear_position = desired_gear_state;
		}

		//TODO :: send speed command,
	}
	else if(dbw_cmd.cmd_speed<0)
	{
		desired_gear_state = DbwGear::REVERSE; //2
		if(desired_gear_state != previous_gear_position)
		{
			ChangeGear(desired_gear_state);
			previous_gear_position = desired_gear_state;
		}

		//TODO :: send speed command
	}
}

void PlatformBydFlProcess::ForkControl(Platform::Sensors::PalletBaseCmdMsg data_basecmd)
{
//	std::cout << "fork control" << std::endl;

	uint8_t status[6] = {0};
	if(!bForkControl)
	{
		bForkControl = true;
		previous_data_forkbasecmd.trigger_tiller = 0x00;
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, true);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, true);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, true);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, true);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, true);
	}

	if(previous_data_forkbasecmd.trigger_tiller != data_basecmd.trigger_tiller)
	{
		bActivateForkControl = true;
	}

	if(bActivateForkControl)
	{
		std::cout << "enter fork control" << std::endl;
		bActivateForkControl = false;
		if(data_basecmd.trigger_tiller == 0x05) //raise fork
		{
			sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 1.6);
		}

		else if(data_basecmd.trigger_tiller == 0x03) //lower fork
		{
			sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 3.2);
		}

		else if(data_basecmd.trigger_tiller == 0x11) //fork tilt forward
		{
			sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(2, 3.2);
		}

		else if(data_basecmd.trigger_tiller == 0x21) // fork tilt backward
		{
			sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(2, 1.6);
		}

		else if(data_basecmd.trigger_tiller == 0x81) // fork move left
		{
			sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(1, 3.2);
		}

		else if(data_basecmd.trigger_tiller == 0x41) // fork move right
		{
			sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(1, 1.6);
		}
	}

	previous_data_forkbasecmd.trigger_tiller = data_basecmd.trigger_tiller;
}

void PlatformBydFlProcess::ChangeGear(const int32_t desired_gear_position)
{
	uint8_t status[4] = {0};

	bDrive = true;

	if(desired_gear_position == 1)
	{
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, false);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(1, false);
	}
	else if(desired_gear_position == 2)
	{
		//sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, true);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, false);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(1, true);
	}
	else if(desired_gear_position == 3)
	{
		//sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, true);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(1, false);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, true);
	}

	sptr_ADAM_ROController_Gear->read_adam_6266_ro_status(status);

	printf( "---------- Printing 10.0.0.6 Status ----------\n" );

	printf( "[Channel 0] Relay Output = 0x%X\n" , status[0] );
	printf( "[Channel 1] Relay Output = 0x%X\n" , status[1] );
	printf( "[Channel 2] Relay Output = 0x%X\n" , status[2] );
	printf( "[Channel 3] Relay Output = 0x%X\n" , status[3] );

	printf( "---------- Printing 10.0.0.6 Status ----------\n" );

}

void PlatformBydFlProcess::CmdSpeedToVoltage(float cmd_speed,
		float &ch0_voltage)
{
	float ao_value[4] = {0};
	float speed_voltage = 0.0;
	cmd_speed = fabs(cmd_speed);

	if (cmd_speed > 1.0)
	{
		cmd_speed = 1.0;
	}

	if(cmd_speed == 0.0)
	{
		speed_voltage = 0.0;
	}
	else
	{
		speed_voltage = 2.0;//(cmd_speed*0.38)+1;
	}
//	float speed = cmd_speed * 4.8;

//	if(speed == 0.0)
//	{
//		speed_voltage = 0;
//	}
//	else if()

	if(cmd_speed >=0.01 && bDrive)
	{
		bTest = true;
		bDrive = false;
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(0, true);
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(2, true);
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(3, true);
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(4, true);
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(5, true);

		sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(0, speed_voltage);

		sptr_ADAM_AOController_DrvSteer_Voltage->read_aovalue(ao_value);
		printf( "[Channel 0] AO Voltage = %f\n" , ao_value[0] );

	}

}

void PlatformBydFlProcess::SteerControl(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd)
{
	float ao_value[4] = {0};
	uint8_t status[6] = {0};
	if(!bSteerControl)
	{
		bSteerControl = true;
		//bStopSteer = true;
		//previous_data_steerDBWcmd.cmd_steer = 0.0;
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(0, true); //on main valve
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(1, true); //on prop valve

		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, true);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, true);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, true);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, true);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, true);

		if(data_dbwcmd.cmd_speed == 0.0)
		{
			if(data_dbwcmd.cmd_steer >=0.01) //steer left
			{
				sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(2, 1); //set prop valve steer left voltage
				sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 1.9); //set pump voltage
			}
			else if(data_dbwcmd.cmd_steer <=-0.01)
			{
				sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(1, 1); //set prop valve steer right voltage
				sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 1.9); //set pump voltage
			}
		}
		else
		{
//			if(data_dbwcmd.cmd_steer >=0.01) //steer left
//			{
//				sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(1, 2); //set prop valve steer left voltage
//				sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 1.5); //set pump voltage
//			}
//			else if(data_dbwcmd.cmd_steer <=-0.01)
//			{
//				sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(2, 2); //set prop valve steer right voltage
//				sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 1.5); //set pump voltage
//			}
		}
	}
	std::cout << "dbw steer" << data_dbwcmd.cmd_steer << std::endl;


//	if(previous_data_steerDBWcmd.cmd_steer == data_dbwcmd.cmd_steer)
//	{
//		if(!(fabs(data_dbwcmd.cmd_steer)>0))
//		{
//			StopSteerControl();
//		}
//	}
//	else
//	{
//		if((previous_data_steerDBWcmd.cmd_steer > 0 && data_dbwcmd.cmd_steer > 0) ||
//				(previous_data_steerDBWcmd.cmd_steer < 0 && data_dbwcmd.cmd_steer < 0))
//		{
//
//		}
//	}


//	else if(fabs(data_dbwcmd.cmd_steer)<0.01)
//	{
//		//StopSteerControl();
//	}
//	previous_data_steerDBWcmd.cmd_steer = data_dbwcmd.cmd_steer;
}

void PlatformBydFlProcess::GetDriveCmdsAndStatus()
{
	Platform::Sensors::PalletDbwCmdMsg in_data_dbwcmd;
	if(sptr_RetreiveDBWCmds_->GetDBWCmdMsg(in_data_dbwcmd))
	{
		dbw_cmd = in_data_dbwcmd;
	}

	Platform::Sensors::PalletBaseCmdMsg base_cmd;
	if(sptr_RetreiveBaseCmds_->GetBaseCmdMsg(base_cmd))
	{
		palletbase_cmd = base_cmd;
	}

	Platform::Sensors::PalletDbwCmdMsg dbw_cmd_teleop_local;
//	if(sptr_RetreiveDBWCmdsTeleOps_->GetDBWCmdMsg(dbw_cmd_teleop_local))
//	{
//		dbw_cmd_teleop = dbw_cmd_teleop_local;
//		std::cout << "receive teleops data" << std::endl;
//	}
}


void PlatformBydFlProcess::StopSteerControl()
{
	//if(bStopSteer)
	//{
	//	bStopSteer = false;
		sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 2.5); //set pump voltage
		sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(1, 0); //set prop valve steer left voltage
		sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(2, 0); //set prop valve steer right voltage
		usleep(50000);
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(1, false); //off prop valve
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(0, false); //off main valve
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, false);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, false);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, false);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, false);
		sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, false);
	//}
}

void PlatformBydFlProcess::StopForkControl()
{
	//sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(1, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(2, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 2.5);

	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, false);
}

void PlatformBydFlProcess::StopDriveControl()
{
	if(bTest)
	{
		bTest = false;
		//neutral gear
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, false);
		sptr_ADAM_ROController_Gear->write_adam6266_ro_status(1, false);

		//shut down drive
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(0, false);
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(2, false);
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(3, false);
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(4, false);
		sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(5, false);

		sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(0, 0);
	}
}

void PlatformBydFlProcess::Shutdown()
{
	//set safety voltage for 10.0.0.3
//	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 2.5);
//	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(1, 2.5);
//	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(2, 2.5);
//	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 2.5);
//
//	usleep(50000);
//
//	//sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(1, false); //off prop valve
//	//sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(0, false); //off main valve
//
//	//off 10.0.0.2 fork control
//	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, false);
//	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, false);
//	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, false);
//	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, false);
//	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, false);

	//neutral gear
	sptr_ADAM_ROController_Gear->write_adam6266_ro_status(0, false);
	sptr_ADAM_ROController_Gear->write_adam6266_ro_status(1, false);

//	shut down drive
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(0, false);
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(2, false);
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(3, false);
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(4, false);
	sptr_ADAM_ROController_DrvPedal->write_adam6260_ro_status(5, false);

	sptr_ADAM_AOController_DrvSteer_Voltage->write_aovalue(0, 0);

}

void PlatformBydFlProcess::FeedbackDBWandPalletBaseStatus()
{
	//Define stkci messages
	Platform::Sensors::PalletDbwFbkMsg out_stkci_dbw_fbk; //TBD speed feedback, encoders etc
	Platform::Sensors::PalletBaseFbkMsg out_stkci_basepallet_fbk; //TBD battery

	std::unique_lock<std::mutex> can_lock (mutex_CANBus);

	out_stkci_basepallet_fbk.seq_key = 0;
	out_stkci_basepallet_fbk.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
	std::cout << "Enter feedback" << std::endl;
	uint32_t current_steer_enc_pos;

	sptr_PcmPalletController->GetPCMPalletFeedback(pcm_feedback, pcm_feedback_healthstatus);

//	Platform::Sensors::PalletBaseFbkMsg in_stkci_basepallet_fbk;
//	if(sptr_RetrieveBaseFeedback_->GetPalletBaseFbkMsg(in_stkci_basepallet_fbk))
//	{
//		out_stkci_basepallet_fbk.engagement_sensor_left = in_stkci_basepallet_fbk.engagement_sensor_left;
//		out_stkci_basepallet_fbk.engagement_sensor_right = in_stkci_basepallet_fbk.engagement_sensor_right;
//		ptr_stkci_pub->PubPalletBaseFbk("DBW", out_stkci_basepallet_fbk);
//	}

	if(sptr_PcmPalletController->GetSikoSteerEncoderFeedback(current_steer_enc_pos))
	{
		steer_enc_pos = current_steer_enc_pos;

		std::cout << "current steer encoder position = " << steer_enc_pos << std::endl;

//		sptr_SteerMotorThread_->SetCurrentSeikoEncoder(Common::Utility::Time::getusecCountSinceEpoch(),current_steer_enc_pos);
//		convert steer encoder feedback to angle in radians.
//		sptr_pcm_dbw_drive->ConvertSeikoSteerCounttoAngle(sptr_SteerMotorThread_->GetXMLEncoderHomePosition(),steer_enc_pos,steer_seiko_enc_fb_angle_radians);
	}

	uint32_t current_line_enc_pos;
	if(sptr_PcmPalletController->GetBaumerLineEncoderFeedback(current_line_enc_pos))
	{
		//		line_enc_pos = current_line_enc_pos;

		float current_height = (current_line_enc_pos/100.0) - 17.3; //in cm

		std::cout << "current line encoder position = " << current_line_enc_pos << std::endl;
		std::cout << "current line encoder position in height(cm) = " << current_height << std::endl;
	}

	uint32_t current_tilt_enc_pos;
	if(sptr_PcmPalletController->GetBaumerTiltEncoderFeedback(current_tilt_enc_pos))
	{
		//		tilt_enc_pos = current_tilt_enc_pos;
		std::cout << "current tilt encoder position = " << current_tilt_enc_pos << std::endl;
		std::cout << "current tilt encoder position in angle = " << current_tilt_enc_pos/10<< std::endl;

	}
	can_lock.unlock();
}

void PlatformBydFlProcess::stopControls()
{
	if (bForkControl && !bSteerControl)									// Previous Control was Fork
	{
		printf("1a. Deactivating Pump and Closing Fork Valves\n");

		palletbase_cmd.trigger_tiller = 0x00;
		bDeActivateValveControl = true;
		StopForkControl();												// Close Fork Valves

		//
		bForkControl = false;
	}
	else if (!bForkControl && bSteerControl)							// Previous Control was Steer
	{
		printf("1b. Deactivating Pump and Closing Steer Valves\n");

		palletbase_cmd.trigger_tiller = 0x00;
		bDeActivateValveControl = true;
		StopSteerControl();												// Close Steer Valves

		//
		bSteerControl = false;
	}
	else if (!bForkControl && !bSteerControl)							// No Previous Control
	{
		//printf("1c. Keeping Valves Closed\n");

		palletbase_cmd.trigger_tiller = 0x00;
		//		stopForkControl();												// Close Fork Valves
		//		stopSteerControl();												// Close Steer Valves

		//sptr_PcmPalletController->CloseAllValves();						// Close all valves
	}
}

PlatformBydFlProcess::~PlatformBydFlProcess()
{

}

}

