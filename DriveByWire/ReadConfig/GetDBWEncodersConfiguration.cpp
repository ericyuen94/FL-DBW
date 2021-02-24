#include "GetDBWEncodersConfiguration.h"

namespace DBW
{

GetDBWEncodersConfiguration::GetDBWEncodersConfiguration()
{

}

std::string GetDBWEncodersConfiguration::GetFileName()
{
    return ".xml";
}

std::string GetDBWEncodersConfiguration::GetDefaultFilePath()
{
    return "../config/swmodules/dbw";
}

const DBWEncodersConfig* GetDBWEncodersConfiguration::GetViewerConfigData() const
{
	return ptr_dbw_encoders_config;
}

void GetDBWEncodersConfiguration::GetViewerConfigData(DBWEncodersConfig &out_config)
{
	out_config = dbw_encoders_config;
}

bool GetDBWEncodersConfiguration::OpenViewerConfiguration()
{
	bool bresult;

	//open configuration file
	Common::Utility::configFile cConfigFile_;
	std::string str_file_location = GetDefaultFilePath()+"/"+"DBWEncoders.xml";
	if(cConfigFile_.OpenFile(str_file_location) == 0)
	{
		std::cout << "Opened DBWEncoders Configuration\n";

		// if able to open configuration file
		bresult  = ReadDBWEncodersConfigfromTree(*cConfigFile_.GetCurrentReferenceNode(),dbw_encoders_config);
		ptr_dbw_encoders_config = &dbw_encoders_config;
	}
	else
	{
		std::cerr << "Cannot open DBWEncoders configuration\n";
		ptr_dbw_encoders_config = NULL;
		bresult = false;
	}

	return bresult;
}

bool GetDBWEncodersConfiguration::ReadDBWEncodersConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, DBWEncodersConfig &aConfig)
{
	bool bresult=true;

	const Common::Entity::TreeNode *tree_node_dbwencoders = aConfigurationTree.first_node("DBWEncoders");

	if(tree_node_dbwencoders!=NULL)
	{
		//Steer Encoder Settings
		const Common::Entity::TreeNode *tree_node_steer_encoder = tree_node_dbwencoders->first_node("Steer_Encoder_Settings");
		if(tree_node_steer_encoder->GetChildNodeValueAs<int32_t>("steer_home_enable",aConfig.params_Steer_Encoder_Settings.steer_home_enable)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_steer_encoder->name() << std::endl;
		}

		if(tree_node_steer_encoder->GetChildNodeValueAs<int32_t>("steer_encoder_home",aConfig.params_Steer_Encoder_Settings.steer_encoder_home)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_steer_encoder->name() << std::endl;
		}

		if(tree_node_steer_encoder->GetChildNodeValueAs<int32_t>("steer_encoder_lpos",aConfig.params_Steer_Encoder_Settings.steer_encoder_lpos)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_steer_encoder->name() << std::endl;
		}

		if(tree_node_steer_encoder->GetChildNodeValueAs<int32_t>("steer_encoder_rpos",aConfig.params_Steer_Encoder_Settings.steer_encoder_rpos)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_steer_encoder->name() << std::endl;
		}

		if(tree_node_steer_encoder->GetChildNodeValueAs<float64_t>("max_steering",aConfig.params_Steer_Encoder_Settings.max_steering)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_steer_encoder->name() << std::endl;
		}

		//Line Encoder Settings
		const Common::Entity::TreeNode *tree_node_line_encoder = tree_node_dbwencoders->first_node("Line_Encoder_Settings");
		if(tree_node_line_encoder->GetChildNodeValueAs<int32_t>("line_home_enable",aConfig.params_Line_Encoder_Settings.line_home_enable)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_line_encoder->name() << std::endl;
		}

		if(tree_node_line_encoder->GetChildNodeValueAs<int32_t>("line_encoder_home",aConfig.params_Line_Encoder_Settings.line_encoder_home)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_line_encoder->name() << std::endl;
		}

		if(tree_node_line_encoder->GetChildNodeValueAs<int32_t>("line_encoder_min",aConfig.params_Line_Encoder_Settings.line_encoder_min)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_line_encoder->name() << std::endl;
		}

		if(tree_node_line_encoder->GetChildNodeValueAs<int32_t>("line_encoder_max",aConfig.params_Line_Encoder_Settings.line_encoder_max)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_line_encoder->name() << std::endl;
		}

		//Tilt Encoder Settings
		const Common::Entity::TreeNode *tree_node_tilt_encoder = tree_node_dbwencoders->first_node("Tilt_Encoder_Settings");
		if(tree_node_tilt_encoder->GetChildNodeValueAs<int32_t>("tilt_home_enable",aConfig.params_Tilt_Encoder_Settings.tilt_home_enable)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_tilt_encoder->name() << std::endl;
		}

		if(tree_node_tilt_encoder->GetChildNodeValueAs<int32_t>("tilt_encoder_home",aConfig.params_Tilt_Encoder_Settings.tilt_encoder_home)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_tilt_encoder->name() << std::endl;
		}

		if(tree_node_tilt_encoder->GetChildNodeValueAs<int32_t>("tilt_encoder_min",aConfig.params_Tilt_Encoder_Settings.tilt_encoder_min)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_tilt_encoder->name() << std::endl;
		}

		if(tree_node_tilt_encoder->GetChildNodeValueAs<int32_t>("tilt_encoder_max",aConfig.params_Tilt_Encoder_Settings.tilt_encoder_max)==false)
		{
			bresult = false;
			std::cerr << "Missing interface_name in " << tree_node_tilt_encoder->name() << std::endl;
		}

		//
		PrintSummary();
	}

	return bresult;
}

void GetDBWEncodersConfiguration::PrintSummary()
{
	//
	std::cout << "[Steer Encoder Settings] " << std::endl;
	std::cout << " steer_home_enable = " << dbw_encoders_config.params_Steer_Encoder_Settings.steer_home_enable << std::endl;
	std::cout << " steer_encoder_home = " << dbw_encoders_config.params_Steer_Encoder_Settings.steer_encoder_home << std::endl;
	std::cout << " steer_encoder_lpos = " << dbw_encoders_config.params_Steer_Encoder_Settings.steer_encoder_lpos << std::endl;
	std::cout << " steer_encoder_rpos = " << dbw_encoders_config.params_Steer_Encoder_Settings.steer_encoder_rpos << std::endl;
	std::cout << " max_steering = " << dbw_encoders_config.params_Steer_Encoder_Settings.max_steering << std::endl;

	//
	std::cout << "[Line Encoder Settings] " << std::endl;
	std::cout << " line_home_enable = " << dbw_encoders_config.params_Line_Encoder_Settings.line_home_enable << std::endl;
	std::cout << " line_encoder_home = " << dbw_encoders_config.params_Line_Encoder_Settings.line_encoder_home << std::endl;
	std::cout << " line_encoder_min = " << dbw_encoders_config.params_Line_Encoder_Settings.line_encoder_min << std::endl;
	std::cout << " line_encoder_max = " << dbw_encoders_config.params_Line_Encoder_Settings.line_encoder_max << std::endl;

	//
	std::cout << "[Tilt Encoder Settings] " << std::endl;
	std::cout << " tilt_home_enable = " << dbw_encoders_config.params_Tilt_Encoder_Settings.tilt_home_enable << std::endl;
	std::cout << " tilt_encoder_home = " << dbw_encoders_config.params_Tilt_Encoder_Settings.tilt_encoder_home << std::endl;
	std::cout << " tilt_encoder_min = " << dbw_encoders_config.params_Tilt_Encoder_Settings.tilt_encoder_min << std::endl;
	std::cout << " tilt_encoder_max = " << dbw_encoders_config.params_Tilt_Encoder_Settings.tilt_encoder_max << std::endl;
}

GetDBWEncodersConfiguration::~GetDBWEncodersConfiguration()
{

}

}



