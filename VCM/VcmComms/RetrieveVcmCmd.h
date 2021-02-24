/*
 * RetrieveVcmCmd.h
 *
 *  Created on: Jul 16, 2017
 *      Author: emily
 */

#ifndef RETRIEVEVCMCMD_H_
#define RETRIEVEVCMCMD_H_

#include "Interfaces/VcmCmdMsgSubInterface.h"
#include "Platform/Sensors/SensorsStkciSubData.h"
#include "Platform/Sensors/SensorsUnifiedMiddlewareStruct.h"
#include "Common/Utility/TimeUtils.h"
#include "Common/Utility/configFile.h"
#include <mutex>
#include <list>
#include <cstdint>
#include <memory>
#include <map>

namespace VCM
{

class RetrieveVcmCmd
{

public:

	RetrieveVcmCmd();

	RetrieveVcmCmd(const std::string sender_id_name);

	void operator()();

	bool GetVcmCmd(STKCI::VcmCmdMsg &out_data);

	bool GetLiveliness();

	~RetrieveVcmCmd();

private:

	std::string  sender_id_name_;
	STKCI::VcmCmdMsgSubInterface stkci_sub_vcm;
	STKCI::VcmCmdMsg in_data_buffer;
	//
	std::mutex mutex_data_vcm_data;
	int32_t vcm_cmd_display_count;
	uint64_t previous_timestamp;

	//check message aliveness
	bool msg_activeness;
	uint64_t last_rx_timestamp;

	//
	std::shared_ptr<Common::Utility::configFile> sptr_configfile;

};

}


#endif /* RETRIEVEVCMCMD_H_ */

