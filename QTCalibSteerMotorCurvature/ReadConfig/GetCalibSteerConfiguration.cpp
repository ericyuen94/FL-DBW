/*
 * GetCalibSteerConfiguration.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: Emily Low
 */

#include "GetCalibSteerConfiguration.h"

namespace calibsteerEmulator
{

GetCalibSteerConfiguration::GetCalibSteerConfiguration()
{

}

std::string GetCalibSteerConfiguration::GetFileName()
{
    return "QTCalibSteerMotorCurvature.xml";
}

std::string GetCalibSteerConfiguration::GetDefaultFilePath()
{
    return "../config/swmodules/dbw";
}

const CalibSteerConfig* GetCalibSteerConfiguration::GetCalibSteerConfigData() const
{
	return ptr_steermotor_data_config;
}

void GetCalibSteerConfiguration::GetCalibSteerConfigData(CalibSteerConfig &out_config)
{
	out_config = steermotor_data_config;
}

void GetCalibSteerConfiguration::PrintCalibSteerConfigData(const CalibSteerConfig &in_config)
{
	std::cout << "[GetCalibSteerConfiguration]" << std::endl;

	std::cout << "pcm_pcan_port " << in_config.pcm_pcan_port << std::endl;
	std::cout << "max_linear_speed " << in_config.max_linear_speed << std::endl;
	std::cout << " " << std::endl;

	std::cout << "[SteerCalibLog]" << std::endl;
	std::cout << "enabled " << in_config.SteerCalibLog.enabled << std::endl;
	std::cout << "path " << in_config.SteerCalibLog.path << std::endl;
	std::cout << " " << std::endl;

	std::cout << "[Debug]" << std::endl;
	std::cout << "enabled " << in_config.Debug.enabled << std::endl;
	std::cout << "path " << in_config.Debug.path << std::endl;
	std::cout << " " << std::endl;
}

bool GetCalibSteerConfiguration::OpenCalibSteerConfiguration()
{
	bool bresult=true;

	//open configuration file
	Common::Utility::configFile cConfigFile_;
	std::string str_file_location = GetDefaultFilePath()+"/"+"QTCalibSteerMotorCurvature.xml";
	if(cConfigFile_.OpenFile(str_file_location) == 0)
	{
		std::cout << "Opened SmartMotorSteer Configuration\n";

		// if able to open configuration file
		bresult  = ReadCalibSteerConfigfromTree(*cConfigFile_.GetCurrentReferenceNode(),steermotor_data_config);
		ptr_steermotor_data_config = &steermotor_data_config;
	}
	else
	{
		std::cerr << "Cannot open SmartMotorSteer configuration\n";
		ptr_steermotor_data_config = NULL;
		bresult = false;
	}

	return bresult;
}

bool GetCalibSteerConfiguration::ReadCalibSteerConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, CalibSteerConfig &aConfig)
{
	bool bresult=true;

	const Common::Entity::TreeNode *tree_node_calibsteer = aConfigurationTree.first_node("QTCalibSteerMotorCurvature");

	if(tree_node_calibsteer!=NULL)
	{
		//Get PCAN port for PCM
		if(tree_node_calibsteer->GetChildNodeValueAs<std::string>("pcm_pcan_port",aConfig.pcm_pcan_port)==false)
		{
			bresult = false;
			std::cerr << "Missing enabled tag -> pcm_pcan_port in " << tree_node_calibsteer->name() << std::endl;
		}

		//Get max linear speed
		if(tree_node_calibsteer->GetChildNodeValueAs<float64_t>("max_linear_speed",aConfig.max_linear_speed)==false)
		{
			bresult = false;
			std::cerr << "Missing enabled tag -> max_linear_speed in " << tree_node_calibsteer->name() << std::endl;
		}

		//Get SteerCalibLog information
		const Common::Entity::TreeNode *tree_node_steerlog  = tree_node_calibsteer->first_node("SteerCalibLog");
		if(tree_node_steerlog!=NULL && bresult == true)
		{
			if(tree_node_steerlog->GetChildNodeValueAs<int32_t>("enabled",aConfig.SteerCalibLog.enabled)==false)
			{
				bresult = false;
				std::cerr << "Missing enabled tag -> enabled in " << tree_node_steerlog->name() << std::endl;
			}

			if(tree_node_steerlog->GetChildNodeValueAs<std::string>("log_folder",aConfig.SteerCalibLog.path)==false)
			{
				bresult = false;
				std::cerr << "Missing tag log_folder in " << tree_node_steerlog->name() << std::endl;
			}
		}//if(tree_node_steerlog!=NULL)

		//Get Debug information
		const Common::Entity::TreeNode *tree_node_debug  = tree_node_calibsteer->first_node("Debug");
		if(tree_node_debug!=NULL && bresult == true)
		{
			if(tree_node_debug->GetChildNodeValueAs<int32_t>("enabled",aConfig.Debug.enabled)==false)
			{
				bresult = false;
				std::cerr << "Missing enabled tag -> enabled in " << tree_node_debug->name() << std::endl;
			}

			if(tree_node_debug->GetChildNodeValueAs<std::string>("log_folder",aConfig.Debug.path)==false)
			{
				bresult = false;
				std::cerr << "Missing tag log_folder in " << tree_node_debug->name() << std::endl;
			}
		}//if(tree_node_debug!=NULL)

		//Get Stkci information
		const Common::Entity::TreeNode *tree_node_stkci  = tree_node_calibsteer->first_node("StkciMsg");
		if(tree_node_stkci!=NULL && bresult == true)
		{
			if(tree_node_stkci->GetChildNodeValueAs<std::string>("posecov_senderid",aConfig.StkciSenderId.posecov_senderid)==false)
			{
				bresult = false;
				std::cerr << "Missing enabled tag -> posecov_senderid in " << tree_node_debug->name() << std::endl;
			}

			if(tree_node_stkci->GetChildNodeValueAs<std::string>("gps_vehiclemeasure_senderid",aConfig.StkciSenderId.gps_vehiclemeasure_senderid)==false)
			{
				bresult = false;
				std::cerr << "Missing enabled tag -> gps_vehiclemeasure_senderid in " << tree_node_debug->name() << std::endl;
			}

			if(tree_node_stkci->GetChildNodeValueAs<std::string>("platform_vehiclemeasure_senderid",aConfig.StkciSenderId.platform_vehiclemeasure_senderid)==false)
			{
				bresult = false;
				std::cerr << "Missing enabled tag -> platform_vehiclemeasure_senderid in " << tree_node_debug->name() << std::endl;
			}
		}//
	}

	return bresult;
}

GetCalibSteerConfiguration::~GetCalibSteerConfiguration()
{

}

}

