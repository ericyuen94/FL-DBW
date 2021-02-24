/*
 * GetTeleOpsCSCIConfiguration.cpp
 *
 *  Created on: Nov 28, 2017
 *      Author: Emily Low
 */

#include "GetTeleOpsCSCIConfiguration.h"

namespace TeleOps
{

GetTeleOpsCSCIConfiguration::GetTeleOpsCSCIConfiguration()
{

}

std::string GetTeleOpsCSCIConfiguration::GetFileName()
{
    return "TeleOpsJoystick.xml";
}

std::string GetTeleOpsCSCIConfiguration::GetDefaultFilePath()
{
    return "../config/swmodules/dbw";
}

const TeleOpsCSCI_Config* GetTeleOpsCSCIConfiguration::GetViewerConfigData() const
{
	return ptr_teleops_data_config;
}

void GetTeleOpsCSCIConfiguration::GetViewerConfigData(TeleOpsCSCI_Config &out_config)
{
	out_config = teleops_data_config;
}

bool GetTeleOpsCSCIConfiguration::OpenViewerConfiguration()
{
	bool bresult;

	//open configuration file
	Common::Utility::configFile cConfigFile_;
	std::string str_file_location = GetDefaultFilePath()+"/"+"TeleOpsJoystick.xml";
	if(cConfigFile_.OpenFile(str_file_location) == 0)
	{
		std::cout << "Opened TeleOpsJoystick Configuration\n";

		// if able to open configuration file
		bresult  = ReadTeleOpsCSCIConfigfromTree(*cConfigFile_.GetCurrentReferenceNode(),teleops_data_config);
		ptr_teleops_data_config = &teleops_data_config;
	}
	else
	{
		std::cerr << "Cannot open TeleOpsJoystick configuration\n";
		ptr_teleops_data_config = NULL;
		bresult = false;
	}

	return bresult;
}

bool GetTeleOpsCSCIConfiguration::ReadTeleOpsCSCIConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, TeleOpsCSCI_Config &aConfig)
{
	bool result=true;

	const Common::Entity::TreeNode *tree_node_teleopscsci = aConfigurationTree.first_node("TeleOpsJoystick");

	if(tree_node_teleopscsci!=NULL)
	{
		if(tree_node_teleopscsci->GetChildNodeValueAs<int32_t>("main_thread_period",aConfig.main_thread_period)==false)
		{
			result = false;
			std::cerr << "Missing enabled in main_thread_period" << tree_node_teleopscsci->name() << std::endl;
		}

		if(tree_node_teleopscsci->GetChildNodeValueAs<std::string>("joystick_port",aConfig.joystick_port)==false)
		{
			result = false;
			std::cerr << "Missing enabled in joystick_port" << tree_node_teleopscsci->name() << std::endl;
		}


		if(tree_node_teleopscsci->GetChildNodeValueAs<float64_t>("max_speed",aConfig.max_speed)==false)
		{
			result = false;
			std::cerr << "Missing enabled in max_speed" << tree_node_teleopscsci->name() << std::endl;
		}

		if(tree_node_teleopscsci->GetChildNodeValueAs<float64_t>("max_steer",aConfig.max_steer)==false)
		{
			result = false;
			std::cerr << "Missing enabled in max_steer" << tree_node_teleopscsci->name() << std::endl;
		}
		if(tree_node_teleopscsci->GetChildNodeValueAs<float64_t>("max_lowerfork",aConfig.max_lowerfork)==false)
		{
			result = false;
			std::cerr << "Missing enabled in max_lowerfork" << tree_node_teleopscsci->name() << std::endl;
		}
		if(tree_node_teleopscsci->GetChildNodeValueAs<float64_t>("max_raisefork",aConfig.max_raisefork)==false)
		{
			result = false;
			std::cerr << "Missing enabled in max_raisefork" << tree_node_teleopscsci->name() << std::endl;
		}

		//Get SubStkciMsgs Setting information
		const Common::Entity::TreeNode *tree_node_SubStkciMsgs  = tree_node_teleopscsci->first_node("SubStkciMsgs");
		if(tree_node_SubStkciMsgs!=NULL&&
				GetSubStkciMsgsSettings(tree_node_SubStkciMsgs,aConfig))
		{
			 // continue
		}
		else
		{
			result=false;
			std::cerr << "Error reading SubStkciMsgs properties " << std::endl;
		}

		//Get PubStkciMsgs Setting information
		const Common::Entity::TreeNode *tree_node_PubStkciMsgs  = tree_node_teleopscsci->first_node("PubStkciMsgs");
		if(tree_node_PubStkciMsgs!=NULL&&
				GetPubStkciMsgsSettings(tree_node_PubStkciMsgs,aConfig))
		{
			 // continue
		}
		else
		{
			result=false;
			std::cerr << "Error reading PubStkciMsgs properties " << std::endl;
		}
		//
		PrintSummary();
	}
	return result;
}

void GetTeleOpsCSCIConfiguration::PrintSummary()
{
	std::cout << "main_thread_period = " << teleops_data_config.main_thread_period << std::endl;
	std::cout << "joystick_port = " << teleops_data_config.joystick_port << std::endl;

	//
	std::cout << "[SubStkciMsgs] " << std::endl;
	for(std::map<std::string,STKCIMsg>::iterator it=teleops_data_config.SubStkciMsgs.begin();
			it!=teleops_data_config.SubStkciMsgs.end(); it++)
	{
		std::cout << " " << it->first << " " << "| Sender id = " << it->second.senderid
				<< " | period = " << it->second.threadparams.mThreadPeriodmsec << std::endl;
	}
	//
	std::cout << "[PubStkciMsgs] " << std::endl;
	for(std::map<std::string,STKCIMsg>::iterator it=teleops_data_config.PubStkciMsgs.begin();
			it!=teleops_data_config.PubStkciMsgs.end(); it++)
	{
		std::cout << " " << it->first << " " << " | Sender id => " << it->second.senderid
				<< " | period = " << it->second.threadparams.mThreadPeriodmsec << std::endl;
	}
}

bool GetTeleOpsCSCIConfiguration::GetPubStkciMsgsSettings(const Common::Entity::TreeNode *tree_node_pubstkci_msgs, TeleOpsCSCI_Config &aConfig)
{
	bool bresult = true;

	// Get all SubStkciMsgs
	if(bresult)
	{
		const Common::Entity::TreeNode *tree_node = tree_node_pubstkci_msgs->first_node();

		while(tree_node!=NULL)
		{
			std::string name_pubmsg;
			STKCIMsg pub_stkci_params;
			name_pubmsg = tree_node->name();

			//Get the parameters of each pub msg
			bool result_single_pubmsg;
			result_single_pubmsg = GetAPubStkciMsg(tree_node,pub_stkci_params);
			if(result_single_pubmsg==false) // error in reading single pub msg
			{
				bresult = false;
				break;
			}
			else
			{
				std::pair<std::map<std::string,STKCIMsg>::iterator,bool> insert_result;
				// save a path into data buffer
				insert_result = aConfig.PubStkciMsgs.insert(std::pair<std::string,STKCIMsg>(name_pubmsg,pub_stkci_params));
				if(insert_result.second == false)
				{
					std::cerr << name_pubmsg << " is a duplicate path key " << std::endl;
					bresult = false;
					break;
				}  //  if duplicate key
			}
			//  Get the next path
			tree_node = tree_node->next_sibling();
		} // while
	}// if

	return bresult;
}

bool GetTeleOpsCSCIConfiguration::GetAPubStkciMsg(const Common::Entity::TreeNode *tree_node, STKCIMsg &stkci_msg_params)
{
	bool result=true;

	if(tree_node->GetChildNodeValueAs<std::string>("senderid",stkci_msg_params.senderid)==false)
	{
		result = false;
		std::cerr << "Missing senderid tag in " << tree_node->name() << std::endl;
	}

	if(tree_node->GetChildNodeValueAs<int32_t>("mThreadPeriodmsec",stkci_msg_params.threadparams.mThreadPeriodmsec)==false)
	{
		result = false;
		std::cerr << "Missing mThreadPeriodmsec tag in " << tree_node->name() << std::endl;
	}

	if(tree_node->GetChildNodeValueAs<int32_t>("mThreadTimeoutmsec",stkci_msg_params.threadparams.mThreadTimeoutmsec)==false)
	{
		result = false;
		std::cerr << "Missing mThreadTimeoutmsec tag in " << tree_node->name() << std::endl;
	}

	return result;
}

bool GetTeleOpsCSCIConfiguration::GetSubStkciMsgsSettings(const Common::Entity::TreeNode *tree_node_substkci_msgs, TeleOpsCSCI_Config &aConfig)
{
	bool bresult = true;

	// Get all SubStkciMsgs
	if(bresult)
	{
		const Common::Entity::TreeNode *tree_node = tree_node_substkci_msgs->first_node();

		while(tree_node!=NULL)
		{
			std::string name_submsg;
			STKCIMsg sub_stkci_params;
			name_submsg = tree_node->name();

			//Get the parameters of each submsg
			bool result_single_submsg;
			result_single_submsg = GetASubStkciMsg(tree_node,sub_stkci_params);
			if(result_single_submsg==false) // error in reading single sub msg
			{
				bresult = false;
				break;
			}
			else
			{
				std::pair<std::map<std::string,STKCIMsg>::iterator,bool> insert_result;
				// save a path into data buffer
				insert_result = aConfig.SubStkciMsgs.insert(std::pair<std::string,STKCIMsg>(name_submsg,sub_stkci_params));
				if(insert_result.second == false)
				{
					std::cerr << name_submsg << " is a duplicate path key " << std::endl;
					bresult = false;
					break;
				}  //  if duplicate key
			}
			//  Get the next path
			tree_node = tree_node->next_sibling();
		} // while
	}// if

	return bresult;
}

bool GetTeleOpsCSCIConfiguration::GetASubStkciMsg(const Common::Entity::TreeNode *tree_node, STKCIMsg &stkci_msg_params)
{
	bool result=true;

	if(tree_node->GetChildNodeValueAs<std::string>("senderid",stkci_msg_params.senderid)==false)
	{
		result = false;
		std::cerr << "Missing senderid tag in " << tree_node->name() << std::endl;
	}

	if(tree_node->GetChildNodeValueAs<int32_t>("mThreadPeriodmsec",stkci_msg_params.threadparams.mThreadPeriodmsec)==false)
	{
		result = false;
		std::cerr << "Missing mThreadPeriodmsec tag in " << tree_node->name() << std::endl;
	}

	if(tree_node->GetChildNodeValueAs<int32_t>("mThreadTimeoutmsec",stkci_msg_params.threadparams.mThreadTimeoutmsec)==false)
	{
		result = false;
		std::cerr << "Missing mThreadTimeoutmsec tag in " << tree_node->name() << std::endl;
	}

	return result;
}

GetTeleOpsCSCIConfiguration::~GetTeleOpsCSCIConfiguration()
{

}

}



