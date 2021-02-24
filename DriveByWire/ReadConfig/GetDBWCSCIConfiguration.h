/*
 * GetDBWCSCIConfiguration.h
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */

#ifndef GetDBWCSCIConfiguration_H_
#define GetDBWCSCIConfiguration_H_

#include "DBWCSCIConfiguration.h"
#include "Common/Utility/configFile.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

namespace DBW
{

class GetDBWCSCIConfiguration
{

public:

	GetDBWCSCIConfiguration();

	bool OpenViewerConfiguration();

	const DBWCSCI_Config* GetViewerConfigData() const;

	void GetViewerConfigData(DBWCSCI_Config &out_config);

	~GetDBWCSCIConfiguration();

private:

	bool ReadDBWCSCIConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, DBWCSCI_Config &aConfig);
	//
	bool GetControlCmdThreadsSettings(const Common::Entity::TreeNode *tree_node_ctrlcmds_thread, DBWCSCI_Config &aConfig);
	bool GetAControlCmdThread(const Common::Entity::TreeNode *tree_node, ThreadParams &thread_params);
	//
	bool GetSubStkciMsgsSettings(const Common::Entity::TreeNode *tree_node_substkci_msgs, DBWCSCI_Config &aConfig);
	bool GetASubStkciMsg(const Common::Entity::TreeNode *tree_node, STKCIMsg &stkci_msg_params);
	//
	bool GetPubStkciMsgsSettings(const Common::Entity::TreeNode *tree_node_pubstkci_msgs, DBWCSCI_Config &aConfig);
	bool GetAPubStkciMsg(const Common::Entity::TreeNode *tree_node, STKCIMsg &stkci_msg_params);

	//
	void PrintSummary();

	std::string GetFileName();
	std::string GetDefaultFilePath();

	//
	DBWCSCI_Config *ptr_dbw_data_config;

	DBWCSCI_Config dbw_data_config;
};


}

#endif /* ifndef GetDBWCSCIConfiguration_H_ */
