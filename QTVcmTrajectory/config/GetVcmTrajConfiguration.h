/*
 * GetVcmTrajConfiguration.h
 *
 *  Created on: July 3, 2017
 *      Author: Emily Low
 */

#ifndef GetVcmTrajCONFIGURATION_H_
#define GetVcmTrajCONFIGURATION_H_

#include "VcmTrajConfiguration.h"
#include "Common/Utility/configFile.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

//
namespace vcmtrajdemo
{

class GetVcmTrajConfiguration
{

public:

	GetVcmTrajConfiguration();

	bool OpenVcmTrajConfiguration();

	const VcmTrajConfig* GetVcmTrajConfigData() const;

	void GetVcmTrajConfigData(VcmTrajConfig &out_config);

	void PrintVcmTrajConfigData(const VcmTrajConfig &in_config);

    ~GetVcmTrajConfiguration();

private:

    std::string GetFileName();

   	std::string GetDefaultFilePath();

  	bool ReadVcmTrajConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, VcmTrajConfig &aConfig);

  	VcmTrajConfig *ptr_vcmtraj_data_config;
  	VcmTrajConfig vcmtraj_data_config;
 };

}

#endif /* ifndef GetVcmTrajCONFIGURATION_H_ */
