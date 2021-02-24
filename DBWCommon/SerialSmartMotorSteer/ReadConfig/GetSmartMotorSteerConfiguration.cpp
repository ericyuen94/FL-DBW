/*
 * GetSmartMotorSteerConfiguration.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: Emily Low
 */

#include "GetSmartMotorSteerConfiguration.h"

namespace DBWPalletCommon
{

GetSmartMotorSteerConfiguration::GetSmartMotorSteerConfiguration()
{

}

std::string GetSmartMotorSteerConfiguration::GetFileName()
{
    return "SmartMotorSteer.xml";
}

std::string GetSmartMotorSteerConfiguration::GetDefaultFilePath()
{
    return "../config/swmodules/dbw";
}

const SmartMotorSteerConfig* GetSmartMotorSteerConfiguration::GetSmartMotorSteerConfigData() const
{
	return ptr_steermotor_data_config;
}

void GetSmartMotorSteerConfiguration::GetSmartMotorSteerConfigData(SmartMotorSteerConfig &out_config)
{
	out_config = steermotor_data_config;
}

bool GetSmartMotorSteerConfiguration::OpenSmartMotorSteerConfiguration()
{
	bool bresult=true;

	//open configuration file
	Common::Utility::configFile cConfigFile_;
	std::string str_file_location = GetDefaultFilePath()+"/"+"SmartMotorSteer.xml";
	if(cConfigFile_.OpenFile(str_file_location) == 0)
	{
		std::cout << "Opened SmartMotorSteer Configuration\n";

		// if able to open configuration file
		bresult  = ReadSmartMotorSteerConfigfromTree(*cConfigFile_.GetCurrentReferenceNode(),steermotor_data_config);
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

void GetSmartMotorSteerConfiguration::PrintSmartMotorSteerConfigData(const SmartMotorSteerConfig &in_config)
{
	std::cout << "[SmartMotor_DataLog]" << std::endl;
	std::cout << "enabled " << in_config.params_SmartMotor_DataLog.enabled << std::endl;
	std::cout << "path " << in_config.params_SmartMotor_DataLog.path << std::endl;
	std::cout << " " << std::endl;
	//
	std::cout << "[SmartMotor_Port]" << std::endl;
	std::cout << "steer_motor_device " << in_config.params_SmartMotor_Port.steer_motor_device << std::endl;
	std::cout << " " << std::endl;
	//
	std::cout << "[SteerEncoder_Port]" << std::endl;
	std::cout << "pcan_port " << in_config.params_SteerEncoder_Port.pcan_port << std::endl;
	std::cout << " " << std::endl;
	//
	std::cout << "[SmartMotor_Enabled]" << std::endl;
	std::cout << "steer_enable " << in_config.params_SmartMotor_Enabled.steer_enable << std::endl;
	std::cout << "steer_enc_enable " << in_config.params_SmartMotor_Enabled.steer_enc_enable << std::endl;
	std::cout << "steer_enable " << in_config.params_SmartMotor_Enabled.steer_home_enable << std::endl;
	std::cout << " " << std::endl;
	//
	std::cout << "[SmartMotor_Settings]" << std::endl;
	std::cout << "steer_motor_acc " << in_config.params_SmartMotor_Settings.steer_motor_acc << std::endl;
	std::cout << "steer_motor_vel " << in_config.params_SmartMotor_Settings.steer_motor_vel << std::endl;
	std::cout << " " << std::endl;
	//
	std::cout << "[Steer_Settings]" << std::endl;
	std::cout << "steer_home_enable " << in_config.params_Steer_Settings.steer_home_enable << std::endl;
	std::cout << "steer_min_pos " << in_config.params_Steer_Settings.steer_min_pos << std::endl;
	std::cout << "steer_max_pos " << in_config.params_Steer_Settings.steer_max_pos << std::endl;
	std::cout << "max_steering " << in_config.params_Steer_Settings.max_steering << std::endl;
	std::cout << " " << std::endl;
	//
	std::cout << "[Steer_Encoder_Settings]" << std::endl;
	std::cout << "steer_encoder_home " << in_config.params_Steer_Encoder_Settings.steer_encoder_home << std::endl;
	std::cout << "steer_encoder_lpos " << in_config.params_Steer_Encoder_Settings.steer_encoder_lpos << std::endl;
	std::cout << "steer_encoder_rpos " << in_config.params_Steer_Encoder_Settings.steer_encoder_rpos << std::endl;
}

bool GetSmartMotorSteerConfiguration::ReadSmartMotorSteerConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, SmartMotorSteerConfig &aConfig)
{
	bool bresult=true;

	const Common::Entity::TreeNode *tree_node_SmartMotorSteer = aConfigurationTree.first_node("SmartMotorSteer");

	if(tree_node_SmartMotorSteer!=NULL)
	{
		//Get SmartMotor_DataLog information
		const Common::Entity::TreeNode *tree_node_sm_log  = tree_node_SmartMotorSteer->first_node("DataLog");
		if(tree_node_sm_log!=NULL)
		{
			if(tree_node_sm_log->GetChildNodeValueAs<int32_t>("enabled",aConfig.params_SmartMotor_DataLog.enabled)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_log->name() << std::endl;
			}


			if(tree_node_sm_log->GetChildNodeValueAs<std::string>("path",aConfig.params_SmartMotor_DataLog.path)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_log->name() << std::endl;
			}
		}//

		//Get SmartMotor_Port information
		const Common::Entity::TreeNode *tree_node_sm_port  = tree_node_SmartMotorSteer->first_node("SmartMotor_Port");
		if(tree_node_sm_port!=NULL)
		{
			if(tree_node_sm_port->GetChildNodeValueAs<std::string>("steer_motor_device",aConfig.params_SmartMotor_Port.steer_motor_device)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_port->name() << std::endl;
			}
		}//

		//Get SmartMotor_Enabled information
		const Common::Entity::TreeNode *tree_node_sm_enabled  = tree_node_SmartMotorSteer->first_node("SmartMotor_Enabled");
		if(tree_node_sm_enabled!=NULL)
		{
			if(tree_node_sm_enabled->GetChildNodeValueAs<int32_t>("steer_enable",aConfig.params_SmartMotor_Enabled.steer_enable)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}


			if(tree_node_sm_enabled->GetChildNodeValueAs<int32_t>("steer_enc_enable",aConfig.params_SmartMotor_Enabled.steer_enc_enable)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}

			if(tree_node_sm_enabled->GetChildNodeValueAs<int32_t>("steer_home_enable",aConfig.params_SmartMotor_Enabled.steer_home_enable)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}
		}//

		//Get SmartMotor_Settings information
		const Common::Entity::TreeNode *tree_node_sm_settings  = tree_node_SmartMotorSteer->first_node("SmartMotor_Settings");
		if(tree_node_sm_settings!=NULL)
		{
			if(tree_node_sm_settings->GetChildNodeValueAs<int32_t>("steer_motor_acc",aConfig.params_SmartMotor_Settings.steer_motor_acc)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}


			if(tree_node_sm_settings->GetChildNodeValueAs<int32_t>("steer_motor_vel",aConfig.params_SmartMotor_Settings.steer_motor_vel)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}
		}//

		//Get Steer_Settings information
		const Common::Entity::TreeNode *tree_node_sm_steer_settings  = tree_node_SmartMotorSteer->first_node("Steer_Settings");
		if(tree_node_sm_steer_settings!=NULL)
		{
			if(tree_node_sm_steer_settings->GetChildNodeValueAs<int32_t>("steer_home_enable",aConfig.params_Steer_Settings.steer_home_enable)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}

			if(tree_node_sm_steer_settings->GetChildNodeValueAs<int32_t>("steer_min_pos",aConfig.params_Steer_Settings.steer_min_pos)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}

			if(tree_node_sm_steer_settings->GetChildNodeValueAs<int32_t>("steer_max_pos",aConfig.params_Steer_Settings.steer_max_pos)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}

			if(tree_node_sm_steer_settings->GetChildNodeValueAs<int32_t>("max_steering",aConfig.params_Steer_Settings.max_steering)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}
		}//

		//Get Steer_Encoder_Port Settings information
		const Common::Entity::TreeNode *tree_node_sm_steer_port_settings  = tree_node_SmartMotorSteer->first_node("SteerEncoder_Port");
		if(tree_node_sm_steer_port_settings!=NULL)
		{
			if(tree_node_sm_steer_port_settings->GetChildNodeValueAs<std::string>("pcan_port",aConfig.params_SteerEncoder_Port.pcan_port)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}
		}

		//Get Steer_Encoder_Settings information
		const Common::Entity::TreeNode *tree_node_sm_steer_enc_settings  = tree_node_SmartMotorSteer->first_node("Steer_Encoder_Settings");
		if(tree_node_sm_steer_enc_settings!=NULL)
		{
			if(tree_node_sm_steer_enc_settings->GetChildNodeValueAs<int32_t>("steer_encoder_home",aConfig.params_Steer_Encoder_Settings.steer_encoder_home)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}

			if(tree_node_sm_steer_enc_settings->GetChildNodeValueAs<int32_t>("steer_encoder_lpos",aConfig.params_Steer_Encoder_Settings.steer_encoder_lpos)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}

			if(tree_node_sm_steer_enc_settings->GetChildNodeValueAs<int32_t>("steer_encoder_rpos",aConfig.params_Steer_Encoder_Settings.steer_encoder_rpos)==false)
			{
				bresult = false;
				std::cerr << "Missing interface_name in " << tree_node_sm_enabled->name() << std::endl;
			}
		}//
	}

	return bresult;
}

GetSmartMotorSteerConfiguration::~GetSmartMotorSteerConfiguration()
{

}

}

