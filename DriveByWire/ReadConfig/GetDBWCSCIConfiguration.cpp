/*
 * GetDBWCSCIConfiguration.cpp
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */

#include "GetDBWCSCIConfiguration.h"

namespace DBW
{

GetDBWCSCIConfiguration::GetDBWCSCIConfiguration()
{

}

std::string GetDBWCSCIConfiguration::GetFileName()
{
    return "DBWCSCI.xml";
}

std::string GetDBWCSCIConfiguration::GetDefaultFilePath()
{
    return "../config/swmodules/dbw";
}

const DBWCSCI_Config* GetDBWCSCIConfiguration::GetViewerConfigData() const
{
	return ptr_dbw_data_config;
}

void GetDBWCSCIConfiguration::GetViewerConfigData(DBWCSCI_Config &out_config)
{
	out_config = dbw_data_config;
}

bool GetDBWCSCIConfiguration::OpenViewerConfiguration()
{
	bool bresult;

	//open configuration file
	Common::Utility::configFile cConfigFile_;
	std::string str_file_location = GetDefaultFilePath()+"/"+"DBWCSCI.xml";
	if(cConfigFile_.OpenFile(str_file_location) == 0)
	{
		std::cout << "Opened DBWCSCI Configuration\n";

		// if able to open configuration file
		bresult  = ReadDBWCSCIConfigfromTree(*cConfigFile_.GetCurrentReferenceNode(),dbw_data_config);
		ptr_dbw_data_config = &dbw_data_config;
	}
	else
	{
		std::cerr << "Cannot open DBWCSCI configuration\n";
		ptr_dbw_data_config = NULL;
		bresult = false;
	}

	return bresult;
}

bool GetDBWCSCIConfiguration::ReadDBWCSCIConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, DBWCSCI_Config &aConfig)
{
	bool result=true;

	const Common::Entity::TreeNode *tree_node_dbwcsci = aConfigurationTree.first_node("DBWCSCI");

	if(tree_node_dbwcsci!=NULL)
	{
		if(tree_node_dbwcsci->GetChildNodeValueAs<int32_t>("main_thread_period",aConfig.main_thread_period)==false)
		{
			result = false;
			std::cerr << "Missing enabled in main_thread_period" << tree_node_dbwcsci->name() << std::endl;
		}

		if(tree_node_dbwcsci->GetChildNodeValueAs<float32_t>("warning_temperature",aConfig.warning_temperature)==false)
		{
			result = false;
			std::cerr << "Missing enabled in warning temperature" << tree_node_dbwcsci->name() << std::endl;
		}

		if(tree_node_dbwcsci->GetChildNodeValueAs<float32_t>("error_temperature",aConfig.error_temperature)==false)
		{
			result = false;
			std::cerr << "Missing enabled in error temperature" << tree_node_dbwcsci->name() << std::endl;
		}

		//Logging
		const Common::Entity::TreeNode *tree_node_logging = tree_node_dbwcsci->first_node("Logging");
		if(tree_node_logging->GetChildNodeValueAs<int64_t>("enabled",aConfig.logging.enabled)==false)
		{
			result = false;
			std::cerr << "Missing enabled in enabled" << tree_node_logging->name() << std::endl;
		}

		//Debug
		const Common::Entity::TreeNode *tree_node_debug = tree_node_dbwcsci->first_node("Debug");
		if(tree_node_debug->GetChildNodeValueAs<int64_t>("enabled",aConfig.debug.enabled)==false)
		{
			result = false;
			std::cerr << "Missing enabled in enabled" << tree_node_debug->name() << std::endl;
		}
		if(tree_node_debug->GetChildNodeValueAs<std::string>("log_folder",aConfig.debug.log_folder)==false)
		{
			result = false;
			std::cerr << "Missing enabled in log_folder" << tree_node_debug->name() << std::endl;
		}

		//pcm
		const Common::Entity::TreeNode *tree_node_pcm = tree_node_dbwcsci->first_node("PCMSettings");
		if(tree_node_pcm->GetChildNodeValueAs<std::string>("port",aConfig.PCMSettings.port)==false)
		{
			result = false;
			std::cerr << "Missing enabled in port" << tree_node_dbwcsci->name() << std::endl;
		}
		if(tree_node_pcm->GetChildNodeValueAs<int32_t>("baudrate",aConfig.PCMSettings.baudrate)==false)
		{
			result = false;
			std::cerr << "Missing enabled in baudrate" << tree_node_dbwcsci->name() << std::endl;
		}

		//Get ControlCmdThreads Setting information
		const Common::Entity::TreeNode *tree_node_ControlCmdThreads  = tree_node_dbwcsci->first_node("ControlCmdThreads");
		if(tree_node_ControlCmdThreads!=NULL&&
				GetControlCmdThreadsSettings(tree_node_ControlCmdThreads,aConfig))
		{
			 // continue
		}
		else
		{
			result=false;
			std::cerr << "Error reading ControlCmdThreads properties " << std::endl;
		}

		//Get SubStkciMsgs Setting information
		const Common::Entity::TreeNode *tree_node_SubStkciMsgs  = tree_node_dbwcsci->first_node("SubStkciMsgs");
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
		const Common::Entity::TreeNode *tree_node_PubStkciMsgs  = tree_node_dbwcsci->first_node("PubStkciMsgs");
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

void GetDBWCSCIConfiguration::PrintSummary()
{
	std::cout << "main_thread_period = " << dbw_data_config.main_thread_period << std::endl;
	//
	std::cout << "[PCMSettings] " << std::endl;
	std::cout << " port = " << dbw_data_config.PCMSettings.port << std::endl;
	std::cout << " baudrate = " << dbw_data_config.PCMSettings.baudrate << std::endl;
	//
	std::cout << "[ControlCmdThreads] " << std::endl;
	for(std::map<std::string,ThreadParams>::iterator it=dbw_data_config.ControlCmdThreads.begin();
			it!=dbw_data_config.ControlCmdThreads.end(); it++)
	{
		std::cout << " " << it->first << " " << " | period = " << it->second.mThreadPeriodmsec << std::endl;
	}
	//
	std::cout << "[SubStkciMsgs] " << std::endl;
	for(std::map<std::string,STKCIMsg>::iterator it=dbw_data_config.SubStkciMsgs.begin();
			it!=dbw_data_config.SubStkciMsgs.end(); it++)
	{
		std::cout << " " << it->first << " " << "| Sender id = " << it->second.senderid
				<< " | period = " << it->second.threadparams.mThreadPeriodmsec << std::endl;
	}
	//
	std::cout << "[PubStkciMsgs] " << std::endl;
	for(std::map<std::string,STKCIMsg>::iterator it=dbw_data_config.PubStkciMsgs.begin();
			it!=dbw_data_config.PubStkciMsgs.end(); it++)
	{
		std::cout << " " << it->first << " " << " | Sender id => " << it->second.senderid
				<< " | period = " << it->second.threadparams.mThreadPeriodmsec << std::endl;
	}
}

bool GetDBWCSCIConfiguration::GetPubStkciMsgsSettings(const Common::Entity::TreeNode *tree_node_pubstkci_msgs, DBWCSCI_Config &aConfig)
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

bool GetDBWCSCIConfiguration::GetAPubStkciMsg(const Common::Entity::TreeNode *tree_node, STKCIMsg &stkci_msg_params)
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

bool GetDBWCSCIConfiguration::GetSubStkciMsgsSettings(const Common::Entity::TreeNode *tree_node_substkci_msgs, DBWCSCI_Config &aConfig)
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

bool GetDBWCSCIConfiguration::GetASubStkciMsg(const Common::Entity::TreeNode *tree_node, STKCIMsg &stkci_msg_params)
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

bool GetDBWCSCIConfiguration::GetControlCmdThreadsSettings(const Common::Entity::TreeNode *tree_node_ctrlcmds_thread, DBWCSCI_Config &aConfig)
{
	bool bresult = true;

	// Get all ControlCmdThreads
	if(bresult)
	{
		const Common::Entity::TreeNode *tree_node = tree_node_ctrlcmds_thread->first_node();

		while(tree_node!=NULL)
		{
			std::string name_ctrlcmd;
			ThreadParams thread_params;
			name_ctrlcmd = tree_node->name();

			//Get the parameters of each thread
			bool result_single_threadcmd;
			result_single_threadcmd = GetAControlCmdThread(tree_node,thread_params);
			if(result_single_threadcmd==false) // error in reading single thread
			{
				bresult = false;
				break;
			}
			else
			{
				std::pair<std::map<std::string,ThreadParams>::iterator,bool> insert_result;
				// save a path into data buffer
				insert_result = aConfig.ControlCmdThreads.insert(std::pair<std::string,ThreadParams>(name_ctrlcmd,thread_params));
				if(insert_result.second == false)
				{
					std::cerr << name_ctrlcmd << " is a duplicate path key " << std::endl;
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

bool GetDBWCSCIConfiguration::GetAControlCmdThread(const Common::Entity::TreeNode *tree_node, ThreadParams &thread_params)
{
	bool result=true;

	if(tree_node->GetChildNodeValueAs<int32_t>("mThreadPeriodmsec",thread_params.mThreadPeriodmsec)==false)
	{
		result = false;
		std::cerr << "Missing mThreadPeriodmsec tag in " << tree_node->name() << std::endl;
	}

	if(tree_node->GetChildNodeValueAs<int32_t>("mThreadTimeoutmsec",thread_params.mThreadTimeoutmsec)==false)
	{
		result = false;
		std::cerr << "Missing mThreadTimeoutmsec tag in " << tree_node->name() << std::endl;
	}

	return result;
}

GetDBWCSCIConfiguration::~GetDBWCSCIConfiguration()
{

}

}



