#ifndef GETDBWENCODERSCONFIGURATION_H_
#define GETDBWENCODERSCONFIGURATION_H_

#include "DBWEncodersConfiguration.h"
#include "Common/Utility/configFile.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

namespace DBW
{

class GetDBWEncodersConfiguration
{

public:

	GetDBWEncodersConfiguration();

	bool OpenViewerConfiguration();

	const DBWEncodersConfig* GetViewerConfigData() const;

	void GetViewerConfigData(DBWEncodersConfig &out_config);

	~GetDBWEncodersConfiguration();

private:

	bool ReadDBWEncodersConfigfromTree(const Common::Entity::TreeNode &aConfigurationTree, DBWEncodersConfig &aConfig);

	//
	void PrintSummary();

	std::string GetFileName();
	std::string GetDefaultFilePath();

	//
	DBWEncodersConfig *ptr_dbw_encoders_config;

	DBWEncodersConfig dbw_encoders_config;
};


}

#endif /* ifndef GETDBWENCODERSCONFIGURATION_H_ */
