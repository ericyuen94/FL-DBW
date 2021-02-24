/*
 * GetSmartMotorSteerConfiguration.h
 *
 *  Created on: July 3, 2017
 *      Author: Emily Low
 */

#ifndef GETSMARTMOTORSTEERCONFIGURATION_H_
#define GETSMARTMOTORSTEERCONFIGURATION_H_

#include "SmartMotorSteerConfiguration.h"
#include "Common/Utility/configFile.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

//
namespace DBWPalletCommon
{

class GetSmartMotorSteerConfiguration
{

public:

	GetSmartMotorSteerConfiguration();

	bool OpenSmartMotorSteerConfiguration();

	const SmartMotorSteerConfig* GetSmartMotorSteerConfigData() const;

	void GetSmartMotorSteerConfigData(SmartMotorSteerConfig &out_config);

	void PrintSmartMotorSteerConfigData(const SmartMotorSteerConfig &in_config);

    ~GetSmartMotorSteerConfiguration();

private:

    std::string GetFileName();

   	std::string GetDefaultFilePath();

  	bool ReadSmartMotorSteerConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, SmartMotorSteerConfig &aConfig);

  	SmartMotorSteerConfig *ptr_steermotor_data_config;
  	SmartMotorSteerConfig steermotor_data_config;
 };

}

#endif /* ifndef GetSmartMotorSteerCONFIGURATION_H_ */
