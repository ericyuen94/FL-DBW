/*
 * PIMProcess.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#include "PIMProcess.h"
#include "Common/Utility/TimeUtils.h"
#include <iostream>

namespace PIM
{

PIMProcess::PIMProcess( std::shared_ptr<Platform::IsolatedDIO::IsolatedDIODriver> dev ) :
		sptr_DIO( dev )
{
    bSuccess_initDIO = false;

    unmanned_switch = false;
    estop_status = false;
    left_engagement_sensor = false;
    right_engagement_sensor = false;
    left_straight_collision_sensor = false;
    left_cross_collision_sensor = false;
    right_straight_collision_sensor = false;
    right_cross_collision_sensor = false;
	piston_status = false;
	solenoid_status = false;
    //
    healthstatus_pim.bConnectedDIO = false;
    healthstatus_pim.bSuccessReadDIO = false;
    healthstatus_pim.error_count_ReadDIO = 0;
    healthstatus_pim.bFlexisoftStateOK = false;
    healthstatus_pim.error_count_FS = 0;

	last_receive_time = Common::Utility::Time::getusecCountSinceEpoch();
}

PIMProcess::~PIMProcess()
{

}

void PIMProcess::GetPIMProcessStatus(healthstatus_pim_fb &health_pim)
{
	health_pim = healthstatus_pim;
}

void PIMProcess::GetIesStatus(healthstatus_pim_ies &health_ies)
{
	//Reset Boolean 
	health_ies.is_ready			= false;
	health_ies.is_fresh_data	= false;
	health_ies.ignoreIES		= false;

	//Data validation
	uint64_t current_time 		= Common::Utility::Time::getusecCountSinceEpoch();
	uint64_t data_lifetime 		= (current_time - last_receive_time) / 1000000.0;
	health_ies.is_fresh_data 	= (data_lifetime < 0.1) ? true : false;

	//Logic 1 (Data fresh and manual ===> ignore )
	if(health_ies.is_fresh_data && !unmanned_switch)
	{
		health_ies.ignoreIES	= true;
	}
	else
	{
		health_ies.ignoreIES	= false;
	}
	
	//Logic 2 (Data fresh , solenoid and piston ===> ies ready )
	if(health_ies.is_fresh_data && solenoid_status && piston_status)
	{
		health_ies.is_ready		= true;
	}
	else
	{
		health_ies.is_ready		= false;
	}
} 

void PIMProcess::SetConfigParams( const PIMCSCI_Config config_params )
{
	bSuccess_initDIO = sptr_DIO->initDIO();

    if ( !bSuccess_initDIO )
    {
        //exit( -1 );
        healthstatus_pim.bConnectedDIO =  false;
    }
    else
    {
    	healthstatus_pim.bConnectedDIO = true;
    }

	//set publisher
	ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
	std::vector<std::string> interface_list;
	interface_list.push_back("PalletAuxFb");

	//
	interface_list.push_back("PalletBaseFb");
	ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);
}

void PIMProcess::operator()()
{
	if(healthstatus_pim.bConnectedDIO)
	{
		//read DIO
		bSuccess_readDIO = sptr_DIO->GetDIO_Input( &input_DIO );

		//
		if ( bSuccess_readDIO )
		{
			last_receive_time = Common::Utility::Time::getusecCountSinceEpoch();

			healthstatus_pim.bSuccessReadDIO = true;
			healthstatus_pim.error_count_ReadDIO = 0;
			//
			printf("Input = 0x%X\n", input_DIO);
			std::bitset<8> binaryInput(input_DIO);
			std::cout << "Input binary          : " << binaryInput << std::endl;

			//publish stkci PIM message
			Platform::Sensors::PalletAuxFbkMsg pallet_auxcmd;
			pallet_auxcmd.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
			pallet_auxcmd.seq_key = 0;

			DecodeMsg( input_DIO );

			//check if the manned/unmanned switch is the same as Flexisoft state

			if(unmanned_switch == flexisoft_state)
			{
				healthstatus_pim.bFlexisoftStateOK = true;
				healthstatus_pim.error_count_FS = 0;
			}
			else
			{
				 healthstatus_pim.error_count_FS ++;
				 if (healthstatus_pim.error_count_FS >= 20)
				 {
					 healthstatus_pim.bFlexisoftStateOK = false;
					 healthstatus_pim.error_count_FS = 20;
				 }
			}

			//
//			pallet_auxcmd.estop_status = Platform::PIM::SWITCH_ESTOP_CLEAR;

			if(!unmanned_switch)
			{
				pallet_auxcmd.manned_unmanned_switch = Platform::PIM::SWITCH_MANNED;
			}
			else
			{
				pallet_auxcmd.manned_unmanned_switch = Platform::PIM::SWITCH_UNMANNED;
			}
			//
			if(!estop_status)
			{
				pallet_auxcmd.estop_status = Platform::PIM::SWITCH_ESTOP_CLEAR;
			}
			else
			{
				pallet_auxcmd.estop_status = Platform::PIM::SWITCH_ESTOP_ACTIVATE;
//				pallet_auxcmd.estop_status = Platform::PIM::SWITCH_ESTOP_CLEAR;	//changed to bypass Estop
			}
			//
			if(solenoid_status)
			{
				pallet_auxcmd.solenoid_status = Platform::PIM::SWITCH_SOLENOID_ACTIVATE;
			}
			else
			{
				pallet_auxcmd.solenoid_status = Platform::PIM::SWITCH_SOLENOID_DEACTIVATE;
			}
			//
			if(piston_status)
			{
				pallet_auxcmd.piston_status = Platform::PIM::SWITCH_PISTON_ACTIVATE;
			}
			else
			{
				pallet_auxcmd.piston_status = Platform::PIM::SWITCH_PISTON_DEACTIVATE;
			}
			ptr_stkci_pub->PubPalletAuxFbk("PIM", pallet_auxcmd);

			//
			Platform::Sensors::PalletBaseFbkMsg pallet_basefbk;
			pallet_basefbk.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
			pallet_basefbk.seq_key = 0;
			if(!left_engagement_sensor)
			{
				pallet_basefbk.engagement_sensor_left = 0x00;
			}
			else
			{
				pallet_basefbk.engagement_sensor_left = 0x01;
			}

			if(!right_engagement_sensor)
			{
				pallet_basefbk.engagement_sensor_right = 0x00;
			}
			else
			{
				pallet_basefbk.engagement_sensor_right = 0x01;
			}
			ptr_stkci_pub->PubPalletBaseFbk("PIM", pallet_basefbk);
		}
		else
		{
			healthstatus_pim.error_count_ReadDIO++;
			if(healthstatus_pim.error_count_ReadDIO>20)
			{
				healthstatus_pim.bSuccessReadDIO = false;
			}
		}
	}//healthstatus_pim.bConnectedDIO is true.
}

void PIMProcess::DecodeMsg( uint8_t input )
{
	// DI0 Not Used for Pallet Stacker
	//original
	//----------- INPUT 0 -----------------

	//----------- INPUT 1 -----------------
	if ((input_DIO & 0x02) == 0x02)			//DI4 - right pallet engagement sensor - normally low
	{
		std::cout << "Right Engaged         : 1" << std::endl;
		right_engagement_sensor = true;		// Engaged
	}
	else
	{
		std::cout << "Right Engaged         : 0" << std::endl;
		right_engagement_sensor = false;	// Not Engaged
	}
	//----------- INPUT 2 -----------------
	if ((input_DIO & 0x04) == 0x04)			// DI3 - left pallet engagement sensor - normally low
	{
		std::cout << "Left Engaged          : 1" << std::endl;
		left_engagement_sensor = true;		// Engaged
	}
	else
	{
		std::cout << "Left Engaged          : 0" << std::endl;
		left_engagement_sensor = false;		// Not Engaged
	}
	//----------- INPUT 3 -----------------
	if ((input_DIO & 0x08) == 0x08)			// DI1 - manned_unmmanned_switch - normally low
	{
		unmanned_switch = true;				// Unmanned
		std::cout << "Autonomous            : 1" << std::endl;
	}
	else
	{
		unmanned_switch = false;			// Manned
		std::cout << "Autonomous            : 0" << std::endl;
	}
	//----------- INPUT 4 -----------------
	if((input_DIO & 0x10) == 0x10)
	{
		estop_status = false;				// Estop Clear
		std::cout << "E-Stop                : 0" << std::endl;
	}
	else
	{
		estop_status = true;				// Estop Activated
		std::cout << "E-Stop                : 1" << std::endl;
	}
	//----------- INPUT 5 -----------------
	if((input_DIO & 0x20) == 0x20)
	{
		piston_status = true;
		std::cout << "Piston                : 1" << std::endl;
	}
	else
	{
		piston_status = false;
		std::cout << "Piston                : 0" << std::endl;
	}
	//----------- INPUT 6 -----------------
	if((input_DIO & 0x40) == 0x40)
	{
		solenoid_status = true;
		std::cout << "Solenoid              : 1" << std::endl;
	}
	else
	{
		solenoid_status = false;
		std::cout << "Solenoid              : 0" << std::endl;
	}
	//----------- INPUT 7 -----------------
	if((input_DIO & 0x80) == 0x80)			// For checking Flexisoft is also in the same manned/Autonomous state
	{
		flexisoft_state = true;				// unmanned AUTONOMOUS
		std::cout << "flexisoft_state       : 1 (Autonomous)" << std::endl;
	}
	else
	{
		flexisoft_state = false;
		std::cout << "flexisoft_state       : 0 (Manned) " << std::endl;
	}
	/* DI5 to DI7 to be determined */
}

}
