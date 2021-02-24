/*
 * GetVcmTrajConfiguration.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: Emily Low
 */

#include "GetVcmTrajConfiguration.h"

namespace vcmtrajdemo
{

GetVcmTrajConfiguration::GetVcmTrajConfiguration()
{

}

std::string GetVcmTrajConfiguration::GetFileName()
{
    return "QTVcmTrajectory.xml";
}

std::string GetVcmTrajConfiguration::GetDefaultFilePath()
{
    return "../config/swmodules/dbw";
}

const VcmTrajConfig* GetVcmTrajConfiguration::GetVcmTrajConfigData() const
{
	return ptr_vcmtraj_data_config;
}

void GetVcmTrajConfiguration::GetVcmTrajConfigData(VcmTrajConfig &out_config)
{
	out_config = vcmtraj_data_config;
}

void GetVcmTrajConfiguration::PrintVcmTrajConfigData(const VcmTrajConfig &in_config)
{
	std::cout << "[GetVcmTrajConfiguration]" << std::endl;

	std::cout << "max_linear_speed " << in_config.max_linear_speed << std::endl;
	std::cout << "max_steer " << in_config.max_steer << std::endl;
	std::cout << " " << std::endl;

	std::cout << "[VcmCalibLog]" << std::endl;
	std::cout << "enabled " << in_config.VcmCalibLog.enabled << std::endl;
	std::cout << "path " << in_config.VcmCalibLog.path << std::endl;
	std::cout << " " << std::endl;

	std::cout << "[StkciMsg]" << std::endl;
	std::cout << "platform_vehiclemeasure_senderid = " << in_config.StkciSenderId.platform_vehiclemeasure_senderid << std::endl;
}

bool GetVcmTrajConfiguration::OpenVcmTrajConfiguration()
{
	bool bresult=true;

	//open configuration file
	Common::Utility::configFile cConfigFile_;
	std::string str_file_location = GetDefaultFilePath()+"/"+"QTVcmTrajectory.xml";
	if(cConfigFile_.OpenFile(str_file_location) == 0)
	{
		std::cout << "Opened QTVcmTrajectory Configuration\n";

		// if able to open configuration file
		bresult  = ReadVcmTrajConfigfromTree(*cConfigFile_.GetCurrentReferenceNode(),vcmtraj_data_config);
		ptr_vcmtraj_data_config = &vcmtraj_data_config;
	}
	else
	{
		std::cerr << "Cannot open QTVcmTrajectory configuration\n";
		ptr_vcmtraj_data_config = NULL;
		bresult = false;
	}

	return bresult;
}

bool GetVcmTrajConfiguration::ReadVcmTrajConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, VcmTrajConfig &aConfig)
{
	bool bresult=true;

	const Common::Entity::TreeNode *tree_node_VcmTraj = aConfigurationTree.first_node("QTVcmTrajectory");

	if(tree_node_VcmTraj!=NULL)
	{
		//Get max linear speed
		if(tree_node_VcmTraj->GetChildNodeValueAs<float64_t>("max_linear_speed",aConfig.max_linear_speed)==false)
		{
			bresult = false;
			std::cerr << "Missing enabled tag -> max_linear_speed in " << tree_node_VcmTraj->name() << std::endl;
		}

		//Get max steer
		if(tree_node_VcmTraj->GetChildNodeValueAs<float64_t>("max_steer",aConfig.max_steer)==false)
		{
			bresult = false;
			std::cerr << "Missing enabled tag -> max_steer in " << tree_node_VcmTraj->name() << std::endl;
		}

		//Get VcmCalibLog information
		const Common::Entity::TreeNode *tree_node_steerlog  = tree_node_VcmTraj->first_node("VcmCalibLog");
		if(tree_node_steerlog!=NULL && bresult == true)
		{
			if(tree_node_steerlog->GetChildNodeValueAs<int32_t>("enabled",aConfig.VcmCalibLog.enabled)==false)
			{
				bresult = false;
				std::cerr << "Missing enabled tag -> enabled in " << tree_node_steerlog->name() << std::endl;
			}

			if(tree_node_steerlog->GetChildNodeValueAs<std::string>("log_folder",aConfig.VcmCalibLog.path)==false)
			{
				bresult = false;
				std::cerr << "Missing tag log_folder in " << tree_node_steerlog->name() << std::endl;
			}
		}//if(tree_node_steerlog!=NULL)


		//Get Stkci information
		const Common::Entity::TreeNode *tree_node_stkci  = tree_node_VcmTraj->first_node("StkciMsg");
		if(tree_node_stkci!=NULL && bresult == true)
		{
			if(tree_node_stkci->GetChildNodeValueAs<std::string>("platform_vehiclemeasure_senderid",aConfig.StkciSenderId.platform_vehiclemeasure_senderid)==false)
			{
				bresult = false;
				std::cerr << "Missing enabled tag -> platform_vehiclemeasure_senderid in " << tree_node_stkci->name() << std::endl;
			}
		}//
	}

	return bresult;
}

GetVcmTrajConfiguration::~GetVcmTrajConfiguration()
{

}

}

