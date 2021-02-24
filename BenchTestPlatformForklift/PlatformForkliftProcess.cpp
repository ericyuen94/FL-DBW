/*
 * PlatformForkliftProcess.cpp
 *
 *  Created on: Aug 1, 2018
 *      Author: Kian Wee
 */

#include "PlatformForkliftProcess.h"
#include "StkciPCMPalletBoundary.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace PlatformForklift
{

PlatformForkliftProcess::PlatformForkliftProcess()
{
	previous_timestamp = 0;
	previous_fb_timestamp = 0;
	previous_drive_timestamp = 0;
	previous_ro_timestamp = 0;
	pub_feedback_data_count = 0;

	bReset = false;

	bForkControl = false;
	bSteerControl = false;
	bActivateForkControl = false;
	bActivateSteerControl = false;
	bDeActivateValveControl = false;
	prop_valve_voltage = 0;
	previous_cmd_speed_ = 0;
	previous_cmd_brake_ = 0;
	basecmd.trigger_tiller = 0x00;

	//
	previous_ro_cmd_speed_ = 0;
	bROchannel0 = false;
	bROchannel1 = false;
	bROchannel2 = false;
}

void PlatformForkliftProcess::SetConfig(const PlatformForkliftConfig config)
{
	config_params = config;

	//Open port
	sptr_PcmPalletController = std::make_shared<DBWPalletCommon::PCMPalletController>();
	sptr_ADAM_AOController1 = std::make_shared<DBWPalletCommon::Adam_AOController>();
	sptr_ADAM_AOController2 = std::make_shared<DBWPalletCommon::Adam_AOController>();
	sptr_ADAM_ROController = std::make_shared<DBWPalletCommon::Adam_ROController>();

	//
	if(!sptr_PcmPalletController->OpenVecowCanPortModule())
	{
		std::cout << "Cannot open can port !! " << std::endl;
		exit(1);
	}

	// Init Adam Controllers
	InitAdamController();

	sptr_PcmPalletController->ResetLastCommandSpeed();
	if(config_params.debug.enabled==1)
	{
		sptr_PcmPalletController->SetLogger(config_params.debug.log_folder);
	}

	//Send a startup procedure to prevent pallet from electrical shutdown
	//sptr_PcmPalletController->SetStartUpProcedurePallet();

	//Set middleware publisher
	ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
	std::vector<std::string> interface_list;
	interface_list.push_back("PalletBaseFb");
	ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);
}

void PlatformForkliftProcess::InitSharedPtrs(std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds,
										 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds)
{
	sptr_RetreiveDBWCmds_ = sptr_RetreiveDBWCmds;
	sptr_RetreiveBaseCmds_= sptr_RetreiveBaseCmds;
}

void PlatformForkliftProcess::InitAdamController()
{
	uint16_t typecode_value[4] = {0};
	float startup_value[4] = {0};
	float ao_value[4] = {0};
	uint8_t status[4] = {0};

	bool bSuccess_connect1 = sptr_ADAM_AOController1->connect("10.0.0.2", 502);
	bool bSuccess_connect2 = sptr_ADAM_AOController2->connect("10.0.0.3", 502);
	bool bSuccess_connect3 = sptr_ADAM_ROController->connect("10.0.0.4", 502);

	if (bSuccess_connect1)
	{
		std::cout << "Connection 10.0.0.2 Successful" << std::endl;
	}
	else
	{
		std::cout << "Connection 10.0.0.2 Failed" << std::endl;
	}

	if (bSuccess_connect1)
	{
		sptr_ADAM_AOController1->read_aotypecode(typecode_value);
		sptr_ADAM_AOController1->read_startupvalue(startup_value);
		sptr_ADAM_AOController1->read_aovalue(ao_value);
	}

	printf( "---------- Printing 10.0.0.2 Status ----------\n" );

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

	printf( "---------- Printing 10.0.0.2 Status ----------\n" );

	if (bSuccess_connect2)
	{
		std::cout << "Connection 10.0.0.3 Successful" << std::endl;
	}
	else
	{
		std::cout << "Connection 10.0.0.3 Failed" << std::endl;
	}

	if (bSuccess_connect2)
	{
		sptr_ADAM_AOController2->read_aotypecode(typecode_value);
		sptr_ADAM_AOController2->read_startupvalue(startup_value);
		sptr_ADAM_AOController2->read_aovalue(ao_value);
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

	printf( "---------- Printing 10.0.0.3 Status ----------" );

	if (bSuccess_connect3)
	{
		std::cout << "Connection 10.0.0.4 Successful" << std::endl;
	}
	else
	{
		std::cout << "Connection 10.0.0.4 Failed" << std::endl;
	}

	if (bSuccess_connect3)
	{
		sptr_ADAM_ROController->read_ro_status(status);
	}

	printf( "---------- Printing 10.0.0.4 Status ----------\n" );

	printf( "[Channel 0] Relay Output = 0x%X\n" , status[0] );
	printf( "[Channel 1] Relay Output = 0x%X\n" , status[1] );
	printf( "[Channel 2] Relay Output = 0x%X\n" , status[2] );
	printf( "[Channel 3] Relay Output = 0x%X\n" , status[3] );

	printf( "---------- Printing 10.0.0.4 Status ----------" );

	sptr_ADAM_ROController->write_ro_status(0, true);
	bROchannel0 = true;
	usleep(500000);
}

void PlatformForkliftProcess::operator()()
{
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	float ao_value[4] = {0};

	//stkci messages
	Platform::Sensors::PalletDbwCmdMsg in_data_dbwcmd;
	Platform::Sensors::PalletBaseCmdMsg in_data_basecmd;

//	//TODO:: Send Heart beat to prevent pallet from electrical shutdown
//	//sptr_PcmPalletController->SetHeartBeatTiller();

	//Get DBW commands
	if(sptr_RetreiveDBWCmds_->GetDBWCmdMsg(in_data_dbwcmd))
	{
		bReset = false;

		//Get Palletbase commands
		sptr_RetreiveBaseCmds_->GetBaseCmdMsg(in_data_basecmd);

		SetCurrentCommands(in_data_dbwcmd, in_data_basecmd);

		printf("in_data_basecmd.trigger_slowspeed = %f\n", in_data_basecmd.trigger_slowspeed);

		//
		float cmd_speed_ = 0.0;
		float ch0_voltage = 0.0;
		float ch1_voltage = 0.0;
		//
		float cmd_brake_ = 0.0;
		float cmd_brake_voltage = 0.0;
		//
		if (bActivateForkControl)
		{
			sptr_ADAM_AOController1->write_aovalue(0, 4.15);			// Activate Pump

			bActivateForkControl = false;
		}

		if (bActivateSteerControl)
		{
			sptr_ADAM_AOController2->write_aovalue(3, prop_valve_voltage);// Activate Steer Prop Valve
			sptr_ADAM_AOController1->write_aovalue(0, 4.15);			// Activate Pump

			bActivateSteerControl = false;
		}

		if (bDeActivateValveControl)
		{
			sptr_ADAM_AOController1->write_aovalue(0, 3.90);			// DeActivate Pump
			sptr_ADAM_AOController2->write_aovalue(3, 0.0);				// DeActivate Steer Prop Valve

			bDeActivateValveControl = false;
		}

		//
		cmd_speed_ = in_data_dbwcmd.cmd_speed;
		cmd_brake_ = in_data_basecmd.trigger_slowspeed;

		//
		CmdSpeedToVotlage(cmd_speed_, ch0_voltage, ch1_voltage);
		CmdBrakeToVoltage(cmd_brake_, cmd_brake_voltage);

		if (previous_cmd_speed_ != cmd_speed_)
		{
			previous_cmd_speed_ = cmd_speed_;

			sptr_ADAM_AOController2->write_aovalue(0, ch0_voltage);		// Set Acceleration Voltage
			sptr_ADAM_AOController2->write_aovalue(1, ch1_voltage);		// Set Acceleration Voltage
		}

		if (previous_cmd_brake_ != cmd_brake_)
		{
			previous_cmd_brake_ = cmd_brake_;

			sptr_ADAM_AOController2->write_aovalue(2, cmd_brake_voltage);// Set Brake Voltage
		}

		std::cout << "cmd_speed = " << in_data_dbwcmd.cmd_speed << std::endl;
		std::cout << "ch0_voltage = " << ch0_voltage << ", ch1_voltage = " << ch1_voltage << std::endl;

		std::cout << "cmd_brake = " << in_data_basecmd.trigger_slowspeed << std::endl;
		std::cout << "cmd_brake_voltage = " << cmd_brake_voltage << std::endl;
	}
	else
	{
		printf("4. Stop Controls\n");

		if (!bReset)
		{
			// Stop Controls
			sptr_ADAM_AOController1->write_aovalue(0, 3.90);				// DeActivate Pump
			sptr_ADAM_AOController2->write_aovalue(3, 0.0);					// DeActivate Steer Prop Valve
			sptr_ADAM_AOController2->write_aovalue(0, 5.500);				// Set 0 Acceleration Voltage
			sptr_ADAM_AOController2->write_aovalue(1, 6.730);				// Set 0 Acceleration Voltage
			sptr_ADAM_ROController->write_ro_status(0, true);
			usleep(500000);
			sptr_ADAM_ROController->write_ro_status(0, false);
			sptr_ADAM_ROController->write_ro_status(1, false);
			sptr_ADAM_ROController->write_ro_status(2, false);
			for(int32_t i=0; i<20; i++)//1s
			{
				sptr_PcmPalletController->CloseAllValves();					// Close all valves
				usleep(50000); //50ms requirements
			}
			bForkControl = false;
			bSteerControl = false;
			bActivateForkControl = false;
			bActivateSteerControl = false;
			bDeActivateValveControl = false;
			prop_valve_voltage = 0;
			previous_cmd_speed_ = 0;
			previous_cmd_brake_ = 0;
			basecmd.trigger_tiller = 0x00;

			bReset = true;
		}
	}

	std::cout << current_timestamp-previous_timestamp << " ms main loop thread time " << std::endl;
	previous_timestamp =  current_timestamp;
}

void PlatformForkliftProcess::stopControls()
{
	if (bForkControl && !bSteerControl)									// Previous Control was Fork
	{
		printf("1a. Deactivating Pump and Closing Fork Valves\n");

		basecmd.trigger_tiller = 0x00;
		bDeActivateValveControl = true;
		stopForkControl();												// Close Fork Valves

		//
		bForkControl = false;
	}
	else if (!bForkControl && bSteerControl)							// Previous Control was Steer
	{
		printf("1b. Deactivating Pump and Closing Steer Valves\n");

		basecmd.trigger_tiller = 0x00;
		bDeActivateValveControl = true;
		stopSteerControl();												// Close Steer Valves

		//
		bSteerControl = false;
	}
	else if (!bForkControl && !bSteerControl)							// No Previous Control
	{
		printf("1c. Keeping Valves Closed\n");

		basecmd.trigger_tiller = 0x00;
		sptr_PcmPalletController->CloseAllValves();						// Close all valves
	}
}

void PlatformForkliftProcess::SteerControl(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd)
{
	if (bForkControl)
	{
		printf("3a. Deactivating Pump and Closing Fork Valves, Transiting\n");

		basecmd.trigger_tiller = 0x00;

		// Stop Fork Control
		bDeActivateValveControl = true;
		stopForkControl();

		//
		bForkControl = false;
	}
	else if (!bSteerControl)
	{
		float prop_valve_voltage_ = (data_dbwcmd.cmd_steer) * 2.0;
		prop_valve_voltage = prop_valve_voltage_;
		printf("steer = %f\n", data_dbwcmd.cmd_steer);
		printf("voltage = %f\n", prop_valve_voltage);
		if (fabs(prop_valve_voltage_) > 0)
		{
			printf("3b1. Opening Steer Valves and Activating Pump\n");

			startSteerControl();
			bActivateSteerControl = true;
		}
		else
		{
			printf("3b2. Keeping Steer Valves Closed\n");

			bDeActivateValveControl = true;
			sptr_PcmPalletController->CloseAllValves();					// Close all valves
		}
		//
		bSteerControl = true;
	}
	else
	{
		bool bChk;
		float prop_valve_voltage_ = (data_dbwcmd.cmd_steer) * 2.0;

		bChk = (prop_valve_voltage == prop_valve_voltage_) ? true : false;
		if (bChk)
		{
			if (fabs(prop_valve_voltage_) > 0)
			{
				printf("3c1. Keeping Steer Valves Opened\n");

				sptr_PcmPalletController->OpenSteerPropValve();			// Open Steer prop valve
			}
			else
			{
				printf("3c2. Keeping Steer Valves Closed\n");

				sptr_PcmPalletController->CloseAllValves();				// Close all valves
			}
		}
		else
		{
			printf("3d. Deactivating Pump, Transiting\n");

			prop_valve_voltage = 0;

			// Stop Steer Control
			bDeActivateValveControl = true;
			stopSteerControl();

			//
			bSteerControl = false;
		}

		printf("steer = %f\n", data_dbwcmd.cmd_steer);
		printf("voltage = %f\n", prop_valve_voltage);
	}
}

void PlatformForkliftProcess::startSteerControl()
{
	// Start Steer Control
	for(int32_t i=0; i<20; i++)//1s
	{
		sptr_PcmPalletController->OpenSteerPropValve();					// Open Steer prop valve
		usleep(50000); //50ms requirements
	}
}

void PlatformForkliftProcess::stopSteerControl()
{
	// Stop Steer Control
	for(int32_t i=0; i<20; i++)//1s
	{
		sptr_PcmPalletController->CloseAllValves();						// Close all valves
		usleep(50000); //50ms requirements
	}
}

void PlatformForkliftProcess::ForkControl(Platform::Sensors::PalletBaseCmdMsg data_basecmd)
{
	if (bSteerControl)
	{
		printf("2a. Deactivating Pump and Closing Steer Valves, Transiting\n");

		basecmd.trigger_tiller = 0x00;

		bDeActivateValveControl = true;
		stopSteerControl();

		//
		bSteerControl = false;
	}
	else if (!bForkControl)
	{
		printf("2b. Opening Fork Valves and Activating Pump\n");

		basecmd.trigger_tiller = data_basecmd.trigger_tiller;

		startForkControl(data_basecmd);

		if (data_basecmd.trigger_tiller != 0x03 && data_basecmd.trigger_tiller != 0x11)
		{// Don't Activate Pump for Fork Lower and Fork Tilt Forward

			bActivateForkControl = true;
		}

		//
		bForkControl = true;
	}
	else
	{
		if (basecmd.trigger_tiller == data_basecmd.trigger_tiller)
		{
			printf("2c. Keeping Fork Valves Opened\n");

			sptr_PcmPalletController->SetForkliftForkCmds(data_basecmd);// Open valve + movement
		}
		else
		{
			printf("2d. Deactivating Pump and Closing Fork Valves, Transiting\n");

			basecmd.trigger_tiller = 0x00;

			// Stop Fork Control
			bDeActivateValveControl = true;
			stopForkControl();

			//
			bForkControl = false;
		}
	}
}

void PlatformForkliftProcess::startForkControl(Platform::Sensors::PalletBaseCmdMsg data_basecmd)
{
	// Start Fork Control
	for(int32_t i=0; i<20; i++)//1s
	{
		sptr_PcmPalletController->OpenForkMainValve();					// Open Fork main valve
		usleep(50000); //50ms requirements
	}
	for(int32_t i=0; i<20; i++)//1s
	{
		sptr_PcmPalletController->SetForkliftForkCmds(data_basecmd);	// Open Fork movement valve
		usleep(50000); //50ms requirements
	}
}

void PlatformForkliftProcess::stopForkControl()
{
	// Stop Fork Control
	for(int32_t i=0; i<20; i++)//1s
	{
		sptr_PcmPalletController->OpenForkMainValve();					// Close Fork movement valve
		usleep(50000); //50ms requirements
	}
	for(int32_t i=0; i<20; i++)//1s
	{
		sptr_PcmPalletController->CloseAllValves();						// Close all valves
		usleep(50000); //50ms requirements
	}
}

void PlatformForkliftProcess::ValveControl_thread()
{
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	//
	Platform::Sensors::PalletDbwCmdMsg in_data_dbwcmd;
	Platform::Sensors::PalletBaseCmdMsg in_data_basecmd;

	in_data_dbwcmd = dbw_cmd_;
	in_data_basecmd = base_cmd_;

	printf("in_data_basecmd.trigger_tiller = 0x%X\n", in_data_basecmd.trigger_tiller);

	//
	bool bControlFlag = ((in_data_basecmd.trigger_tiller > 0x01)) ? true : false;

	if (!bControlFlag)
	{
		stopControls();
	}
	else
	{
		bool bSteerFlag = ((in_data_basecmd.trigger_tiller == 0x09)) ? true : false;

		if (!bSteerFlag)
		{
			ForkControl(in_data_basecmd);
		}
		else
		{
			SteerControl(in_data_dbwcmd);
		}
	}

	std::cout << current_timestamp-previous_drive_timestamp << " ms valve control loop thread time " << std::endl;
	previous_drive_timestamp =  current_timestamp;
}

void PlatformForkliftProcess::SetCurrentCommands(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd, Platform::Sensors::PalletBaseCmdMsg data_basecmd)
{
	dbw_cmd_ = data_dbwcmd;
	base_cmd_ = data_basecmd;
}

void PlatformForkliftProcess::CmdSpeedToVotlage(float cmd_speed_, float &ch0_voltage_, float &ch1_voltage_)
{
	cmd_speed_ = fabs(cmd_speed_);

	if (cmd_speed_ == 0.0f)
	{
		ch0_voltage_ = 5.500;
		ch1_voltage_ = 6.730;
	}
	else if (cmd_speed_ == 0.1f)
	{
		ch0_voltage_ = 6.500;
		ch1_voltage_ = 5.517;
	}
	else if (cmd_speed_ == 0.2f)
	{
		ch0_voltage_ = 7.500;
		ch1_voltage_ = 4.424;
	}
	else if (cmd_speed_ == 0.3f)
	{
		ch0_voltage_ = 8.500;
		ch1_voltage_ = 3.537;
	}
	else if (cmd_speed_ == 0.4f)
	{
		ch0_voltage_ = 9.500;
		ch1_voltage_ = 2.584;
	}
}

void PlatformForkliftProcess::CmdBrakeToVoltage(float cmd_brake_, float &cmd_brake_voltage_)
{
	float no_brake_voltage = 1.186;
	float full_brake_voltage = 6.197;
	//
	float break_voltage_diff = full_brake_voltage - no_brake_voltage;

	cmd_brake_voltage_ = no_brake_voltage + (break_voltage_diff * cmd_brake_);
}

void PlatformForkliftProcess::RelayOutputControl_thread()
{
	int64_t current_timestamp;
	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();

	ResetROController();

	float cmd_speed_ = dbw_cmd_.cmd_speed;
	float prev_cmd_speed_ = previous_ro_cmd_speed_;

	printf("cmd_speed = %f\n", cmd_speed_);
	printf("prev_cmd_speed_ = %f\n", prev_cmd_speed_);

	if ((cmd_speed_ > 0.0f && prev_cmd_speed_ < 0.0f) || (cmd_speed_ > 0.0f && prev_cmd_speed_ == 0.0f))
	{
		sptr_ADAM_ROController->write_ro_status(0, true);
		sptr_ADAM_ROController->write_ro_status(1, true);

		bROchannel0 = true;
		bROchannel1 = true;

		std::cout << "-------------------------------------------------------------------_Set_ 0 ------------------------------------------------------------------" << std::endl;
		std::cout << "-------------------------------------------------------------------_Set_ 1 ------------------------------------------------------------------" << std::endl;
	}
	else if ((cmd_speed_ < 0.0f && prev_cmd_speed_ > 0.0f) || (cmd_speed_ < 0.0f && prev_cmd_speed_ == 0.0f))
	{
		sptr_ADAM_ROController->write_ro_status(0, true);
		sptr_ADAM_ROController->write_ro_status(2, true);

		bROchannel0 = true;
		bROchannel2 = true;

		std::cout << "-------------------------------------------------------------------_Set_ 0 ------------------------------------------------------------------" << std::endl;
		std::cout << "-------------------------------------------------------------------_Set_ 2 ------------------------------------------------------------------" << std::endl;
	}
	else if (cmd_speed_ == 0.0f && prev_cmd_speed_ != 0.0f)
	{
		sptr_ADAM_ROController->write_ro_status(0, true);

		bROchannel0 = true;

		std::cout << "-------------------------------------------------------------------_Set_ 0 ------------------------------------------------------------------" << std::endl;
	}

	previous_ro_cmd_speed_ = cmd_speed_;

	std::cout << current_timestamp-previous_ro_timestamp << " ms relay output loop thread time " << std::endl;
	previous_ro_timestamp = current_timestamp;
}

void PlatformForkliftProcess::ResetROController()
{
	if (bROchannel0)
	{
		sptr_ADAM_ROController->write_ro_status(0, false);
		bROchannel0 = false;

		std::cout << "-------------------------------------------------------------------_Reset_ 0 ------------------------------------------------------------------" << std::endl;
	}

	if (bROchannel1)
	{
		sptr_ADAM_ROController->write_ro_status(1, false);
		bROchannel1 = false;

		std::cout << "-------------------------------------------------------------------_Reset_ 1 ------------------------------------------------------------------" << std::endl;
	}

	if (bROchannel2)
	{
		sptr_ADAM_ROController->write_ro_status(2, false);
		bROchannel2 = false;

		std::cout << "-------------------------------------------------------------------_Reset_ 2 ------------------------------------------------------------------" << std::endl;
	}
}

//void PlatformForkliftProcess::get_pallet_feedback_thread()
//{
//	int64_t current_timestamp;
//	current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();
//
//	//std::cout << "Send change of pallet drive msg" << std::endl;
//	//Get Feedback and publish feedback for pallet base
//	Platform::Sensors::PalletDbwFbkMsg out_stkci_dbw_fbk;
//	Platform::Sensors::PalletBaseFbkMsg out_stkci_basepallet_fbk;
//	DBWPalletCommon::pcm_palletbase_feedback pcm_feedback;
//
//	//
//	if(sptr_PcmPalletController->GetPCMPalletFeedback(pcm_feedback))
//	{
//		DBWPalletCommon::StkciPCMPalletBoundary::Convert_PalletPCMFbk_To_StkciDBWAndPalletBaseFbk(pcm_feedback,out_stkci_dbw_fbk,out_stkci_basepallet_fbk);
//
//		//pub_feedback_data_count++;
//		//if(pub_feedback_data_count>10)
//		{
//			ptr_stkci_pub->PubPalletBaseFbk(config_params.senderid_publisher, out_stkci_basepallet_fbk);
//			//pub_feedback_data_count = 0;
//		}
//	}
//
//	std::cout << current_timestamp-previous_fb_timestamp << " ms pallet Get PCM feedback loop thread time " << std::endl;
//	previous_fb_timestamp =  current_timestamp;
//}

void PlatformForkliftProcess::Shutdown()
{
	std::cout << "Shutting down! " << std::endl;
	// Stop Controls
	sptr_ADAM_AOController1->write_aovalue(0, 3.90);					// DeActivate Pump
	sptr_ADAM_AOController2->write_aovalue(3, 0.0);						// DeActivate Steer Prop Valve
	sptr_ADAM_AOController2->write_aovalue(0, 5.500);					// Set 0 Acceleration Voltage
	sptr_ADAM_AOController2->write_aovalue(1, 6.730);					// Set 0 Acceleration Voltage
	sptr_ADAM_ROController->write_ro_status(0, true);
	usleep(500000);
	sptr_ADAM_ROController->write_ro_status(0, false);
	sptr_ADAM_ROController->write_ro_status(1, false);
	sptr_ADAM_ROController->write_ro_status(2, false);
	for(int32_t i=0; i<20; i++)//1s
	{
		sptr_PcmPalletController->CloseAllValves();						// Close all valves
		usleep(50000); //50ms requirements
	}

	sptr_PcmPalletController->SetShutdownPCMProcedure();				// shutdown
	std::cout << "Shutdown done! " << std::endl;
}

PlatformForkliftProcess::~PlatformForkliftProcess()
{

}

}

