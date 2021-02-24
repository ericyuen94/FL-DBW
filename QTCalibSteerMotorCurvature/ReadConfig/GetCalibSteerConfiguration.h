/*
 * GetCalibSteerConfiguration.h
 *
 *  Created on: July 3, 2017
 *      Author: Emily Low
 */

#ifndef GetCalibSteerCONFIGURATION_H_
#define GetCalibSteerCONFIGURATION_H_

#include "CalibSteerConfiguration.h"
#include "Common/Utility/configFile.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

//
namespace calibsteerEmulator
{

class GetCalibSteerConfiguration
{

public:

	GetCalibSteerConfiguration();

	bool OpenCalibSteerConfiguration();

	const CalibSteerConfig* GetCalibSteerConfigData() const;

	void GetCalibSteerConfigData(CalibSteerConfig &out_config);

	void PrintCalibSteerConfigData(const CalibSteerConfig &in_config);

    ~GetCalibSteerConfiguration();

private:

    std::string GetFileName();

   	std::string GetDefaultFilePath();

  	bool ReadCalibSteerConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, CalibSteerConfig &aConfig);

  	CalibSteerConfig *ptr_steermotor_data_config;
  	CalibSteerConfig steermotor_data_config;
 };

}

#endif /* ifndef GetCalibSteerCONFIGURATION_H_ */
