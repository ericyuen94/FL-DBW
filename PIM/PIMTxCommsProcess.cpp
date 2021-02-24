/*
 * PIMTxCommsProcess.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#include <PIMTxCommsProcess.h>

namespace PIM
{

PIMTxCommsProcess::PIMTxCommsProcess(std::shared_ptr<Platform::IsolatedDIO::IsolatedDIODriver> dev,
		std::shared_ptr<RetreivePIMCmd> sptr_RetreivePIMCmd) :
		sptr_DIO(dev),
        sptr_RetreivePIMCmd_(sptr_RetreivePIMCmd)

{
	temp_output[8] = {0};
	output_DIO = 0;

	startTime = Common::Utility::Time::getusecCountSinceEpoch() ;
	//
	health_status.bError_RxPIMCmdMsg = false;
	health_status.bError_SetDIOCmds = false;
	health_status.error_count_RxPIMCmdMsg = 0;
	health_status.error_count_SetDIOCmds = 0;

	pip_pip=0;
}

void PIMTxCommsProcess::GetPIMTxCommsProcessHealth(healthstatus_pim_tx &health)
{
	health = health_status;
}

void PIMTxCommsProcess::operator()()
{

//	  For testing only TODO: Remove after test
//    static int sCounter = 0;
//	  static struct PalletAuxCmdMsg testCmd;
//    if ( ++sCounter > 49 ) //5s
//    {
//        sCounter = 0;
//        testCmd.trigger_warning_light ^= 0x01;
//        testCmd.trigger_left_direction_light ^= 0x01;
//        testCmd.trigger_right_direction_light ^= 0x01;
//        UpdateIOXPCmdMsg( testCmd );
//    }
//end of testing

    struct PalletAuxCmdMsg pim_cmd;
    Platform::Sensors::PalletAuxCmdMsg stkci_cmd;
    if(sptr_RetreivePIMCmd_->GetPIMDataMsg(stkci_cmd))
    {
    	std::cout << "test" << std::endl;
    		pim_cmd.trigger_left_direction_light	= stkci_cmd.trigger_left_direction_light;
    		pim_cmd.trigger_right_direction_light	= stkci_cmd.trigger_right_direction_light;
    		pim_cmd.trigger_buzzer					= 0;//stkci_cmd.trigger_buzzer;
    		health_status.bError_RxPIMCmdMsg = false;
    		health_status.error_count_RxPIMCmdMsg = 0;
    }
    else
    {
    	pim_cmd.trigger_left_direction_light = 0;
    	pim_cmd.trigger_right_direction_light = 0;
    	pim_cmd.trigger_buzzer	= 0;
    	health_status.error_count_RxPIMCmdMsg++;
    	if(health_status.error_count_RxPIMCmdMsg>20)
    	{
    		health_status.bError_RxPIMCmdMsg = true;
    	}
    }

    if (stkci_cmd.trigger_buzzer == 1 /*|| !dbw_aliveness*/)
    	ErrorSound(pim_cmd);
    else if (stkci_cmd.trigger_buzzer == 0)
    	NormalSound(pim_cmd);
    else if (stkci_cmd.trigger_buzzer == 2)
    	pim_cmd.trigger_buzzer = 0;

    //send out commands to pcan
    UpdateDIOMsg( pim_cmd );
    if(SetDIOMsg())
    {
    	health_status.bError_SetDIOCmds=false;
     	health_status.error_count_SetDIOCmds = 0;
    }
    else
    {
    	health_status.error_count_SetDIOCmds++;
    	if(health_status.error_count_SetDIOCmds>20)
    	{
    		health_status.bError_SetDIOCmds=true;
    	}
    }

    uint64_t looperTime = Common::Utility::Time::getusecCountSinceEpoch() -  startTime;
    std::cout << "[LOOPER TIME] = " << looperTime/1000.00 << std::endl;
    startTime = Common::Utility::Time::getusecCountSinceEpoch();
}

void PIMTxCommsProcess::NormalSound(PalletAuxCmdMsg &cmd)
{
    pip_pip++;
    if(pip_pip%10 <= 4  )
    	cmd.trigger_buzzer	= 1;
    else
    	cmd.trigger_buzzer	= 0;
}

void PIMTxCommsProcess::ErrorSound(PalletAuxCmdMsg &cmd)
{
    cmd.trigger_buzzer	= 1;
}

void PIMTxCommsProcess::UpdateDIOMsg( const struct PalletAuxCmdMsg setCmd )
{
	output_DIO = 0;

//	temp_output[ 0 ] = 0x00;
//	temp_output[ 1 ] = ( setCmd.trigger_right_direction_light & 0x01 ) << 1;	// output bit 1 - right direction light
//	temp_output[ 2 ] = ( setCmd.trigger_left_direction_light & 0x01 ) << 2;		// output bit 2 - left direction light
//	temp_output[ 3 ] = ( setCmd.trigger_buzzer & 0x01 ) << 3;					// output bit 3 - buzzer
//	temp_output[ 4 ] = 0x00;
//	temp_output[ 5 ] = 0x00;
//	temp_output[ 6 ] = 0x00;
//	temp_output[ 7 ] = 0x00;

	temp_output[ 0 ] =  setCmd.trigger_left_direction_light & 0x01;// output bit 1 - right direction light
	temp_output[ 1 ] = ( setCmd.trigger_right_direction_light & 0x01 ) << 1;// output bit 2 - left direction light
	if(heartbeat_high == true)
	{
		temp_output[ 2 ] = 0x01 << 2;// output bit 3 - heartbeat
		std::cout << "heartbeat = " << (bool)heartbeat_high << std::endl;
		std::cout << "heartbeat = " << "1" << std::endl;
	}
	else
	{
		temp_output[ 2 ] = 0x00;
		std::cout << "heartbeat = " << (bool)heartbeat_high << std::endl;
		std::cout << "heartbeat = " << "0" << std::endl;
	}

	if (heartbeat_high>0) heartbeat_high = 0;
	else if (heartbeat_high<=0) heartbeat_high = 1;
	//heartbeat_high = !heartbeat_high;


	temp_output[ 3 ] = (setCmd.trigger_buzzer & 0x01) << 3;
	temp_output[ 4 ] = 0x00;
	temp_output[ 5 ] = 0x00;
	temp_output[ 6 ] = 0x00;
	temp_output[ 7 ] = 0x00;

	//
	for (int i = 0; i < 8; i++)
	{
		output_DIO = output_DIO | temp_output[ i ];
	}
}

bool PIMTxCommsProcess::SetDIOMsg()
{

	std::cout << "output_DIO = " << (int)output_DIO << std::endl;
	std::cout << "output_DIO = " << (int)output_DIO << std::endl;
	std::cout << "output_DIO = " << (int)output_DIO << std::endl;
	std::cout << "output_DIO = " << (int)output_DIO << std::endl;
	bool result = sptr_DIO->SetDIO_OutputState(output_DIO);

	return result;
}

PIMTxCommsProcess::~PIMTxCommsProcess()
{

}

}

