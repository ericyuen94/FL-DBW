/*
 * GetTeleOpsCSCIConfiguration.h
 *
 *  Created on: Nov 28, 2017
 *      Author: Emily Low
 */

#ifndef GETTELEOPSCSCIConfiguration_H_
#define GETTELEOPSCSCIConfiguration_H_

#include "TeleOpsCSCIConfiguration.h"
#include "Common/Utility/configFile.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

namespace TeleOps
{

class GetTeleOpsCSCIConfiguration
{

public:

	GetTeleOpsCSCIConfiguration();

	bool OpenViewerConfiguration();

	const TeleOpsCSCI_Config* GetViewerConfigData() const;

	void GetViewerConfigData(TeleOpsCSCI_Config &out_config);

	~GetTeleOpsCSCIConfiguration();

private:

	bool ReadTeleOpsCSCIConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, TeleOpsCSCI_Config &aConfig);
	//
	bool GetControlCmdThreadsSettings(const Common::Entity::TreeNode *tree_node_ctrlcmds_thread, TeleOpsCSCI_Config &aConfig);
	bool GetAControlCmdThread(const Common::Entity::TreeNode *tree_node, ThreadParams &thread_params);
	//
	bool GetSubStkciMsgsSettings(const Common::Entity::TreeNode *tree_node_substkci_msgs, TeleOpsCSCI_Config &aConfig);
	bool GetASubStkciMsg(const Common::Entity::TreeNode *tree_node, STKCIMsg &stkci_msg_params);
	//
	bool GetPubStkciMsgsSettings(const Common::Entity::TreeNode *tree_node_pubstkci_msgs, TeleOpsCSCI_Config &aConfig);
	bool GetAPubStkciMsg(const Common::Entity::TreeNode *tree_node, STKCIMsg &stkci_msg_params);

	//
	void PrintSummary();

	std::string GetFileName();
	std::string GetDefaultFilePath();

	//
	TeleOpsCSCI_Config *ptr_teleops_data_config;

	TeleOpsCSCI_Config teleops_data_config;
};


}

#endif /* ifndef GETTELEOPSCSCIConfiguration_H_ */
