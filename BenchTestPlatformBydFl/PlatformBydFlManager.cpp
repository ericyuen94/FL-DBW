/*
 * PlatformBydFlManager.cpp
 *
 *  Created on: July 1, 2019
 *      Author: Tester
 */

#include "PlatformBydFlManager.h"
#include "PlatformBydFlInitializeController.h"
#include "PlatformBydFlProcess.h"
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"
#include "RetreiveTiltMotorFbkMsgs.h"


//
namespace PlatformBydFl
{

PlatformBydFlManager::PlatformBydFlManager(int32_t argc, char **argv):
    BaseCSCIManager(argc,argv)
{
	SetCSCIName("PCMCard");
}

void PlatformBydFlManager::setupSpecificCSCIResources()
{
	 // Create all the Manager's Resources
	 sptr_init_controller = std::make_shared<PlatformBydFlInitializeController>(this);
	 sptr_PlatformBydFlProcess = std::make_shared<PlatformBydFlProcess>();

	 DBWPalletCommon::GetSmartMotorSteerConfiguration cGetSteerMotorDBWConfiguration;

	 // Configuration file related to Serial Smart motor steer and Steer encoder
	 bool open_result_dbw = cGetSteerMotorDBWConfiguration.OpenSmartMotorSteerConfiguration();
	 if(open_result_dbw)
	 {
		// Get the configuration data.
		cGetSteerMotorDBWConfiguration.GetSmartMotorSteerConfigData(config_brake_params);
		cGetSteerMotorDBWConfiguration.PrintSmartMotorSteerConfigData(config_brake_params);
	 }
	 else
	 {
		 std::cout << "Error read DBW Steer XML file " << std::endl;
	     exit(1);
	 }

	 sptr_PlatformBydFlProcess->SetConfigParams(config_brake_params);


	 //
	 setInitializeController(sptr_init_controller); 		// mpInitController is used as Initialize state controller
	 setReadyController(sptr_PlatformBydFlProcess);   		// controller 0 is used as the ready state functor

}

void PlatformBydFlManager::cleanupSpecificCSCIResources()
{
	sptr_PlatformBydFlProcess->Shutdown();
}

void PlatformBydFlManager::DefaultThreadLivelinessLostFunc(const int32_t & aThreadID)
{

}

PlatformBydFlManager::~PlatformBydFlManager()
{

}
}
