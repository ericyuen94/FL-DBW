/*
 * PlatformForkliftProcess.h
 *
 *  Created on: Aug 1, 2018
 *      Author: Kian Wee
 */

#ifndef PLATFORMFORKLIFTPROCESS_H_
#define PLATFORMFORKLIFTPROCESS_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "PlatformForkliftConfiguration.h"
//
#include "PCMPalletController.h"
#include "ADAM_AOController.h"
#include "ADAM_ROController.h"
//
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"

namespace PlatformForklift
{

class PlatformForkliftProcess
{
	///\class PlatformForkliftProcess
	///\brief A class to process data
public:

	PlatformForkliftProcess();

	void SetConfig(const PlatformForkliftConfig config);

	void InitSharedPtrs(std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds,
					 std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds);

	void InitAdamController();

	void operator()();


	// Fork and Steer Controls
	void stopControls();

	void SteerControl(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd);
	void startSteerControl();
	void stopSteerControl();

	void ForkControl(Platform::Sensors::PalletBaseCmdMsg data_basecmd);
	void startForkControl(Platform::Sensors::PalletBaseCmdMsg data_basecmd);
	void stopForkControl();
	// Fork and Steer Controls

	// Valve Controls
	void ValveControl_thread();
	void SetCurrentCommands(Platform::Sensors::PalletDbwCmdMsg data_dbwcmd, Platform::Sensors::PalletBaseCmdMsg data_basecmd);

	// Drive Controls
	void CmdSpeedToVotlage(float cmd_speed_, float &ch0_voltage_, float &ch1_voltage_);
	void CmdBrakeToVoltage(float cmd_brake_, float &cmd_brake_votlage_);

	// Relay Output Controls
	void RelayOutputControl_thread();
	void ResetROController();

	//void get_pallet_feedback_thread();

	void Shutdown();

	~PlatformForkliftProcess();

private:

	PlatformForkliftConfig config_params;

	//
	int64_t previous_timestamp;
	int64_t previous_drive_timestamp;
	int64_t previous_fb_timestamp;
	int64_t previous_ro_timestamp;

	//
	std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;

	//
	std::shared_ptr<DBWPalletCommon::PCMPalletController> sptr_PcmPalletController;
	std::shared_ptr<DBWPalletCommon::Adam_AOController> sptr_ADAM_AOController1;
	std::shared_ptr<DBWPalletCommon::Adam_AOController> sptr_ADAM_AOController2;
	std::shared_ptr<DBWPalletCommon::Adam_ROController> sptr_ADAM_ROController;

	//
	std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds_;
	std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds_;

	//
	int32_t pub_feedback_data_count;

	//
	bool bReset;

	bool bForkControl;
	bool bSteerControl;
	bool bActivateForkControl;
	bool bActivateSteerControl;
	bool bDeActivateValveControl;
	float prop_valve_voltage;
	float previous_cmd_speed_;
	float previous_cmd_brake_;
	Platform::Sensors::PalletBaseCmdMsg basecmd;
	Platform::Sensors::PalletDbwCmdMsg dbw_cmd_;
	Platform::Sensors::PalletBaseCmdMsg base_cmd_;

	//
	float previous_ro_cmd_speed_;
	bool bROchannel0;
	bool bROchannel1;
	bool bROchannel2;

};

}
#endif /* ifndef PLATFORMFORKLIFTPROCESS_H_ */

