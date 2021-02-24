/*
 * SmartMotorSteerConfiguration.h
 *
 *  Created on: Jult 3, 2017
 *      Author: Emily Low
 */

#ifndef SMARTMOTORSTEERCONFIGURATION_H_
#define SMARTMOTORSTEERCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>
#include "Common/typedefs.h"

namespace DBWPalletCommon
{

struct SmartMotor_DataLog
{
	int32_t enabled;
	std::string path;
};

struct SmartMotor_Port
{
	std::string steer_motor_device;
};

struct SteerEncoder_Port
{
	std::string pcan_port;
};

struct SmartMotor_Enabled
{
	int32_t steer_enable;
	int32_t steer_enc_enable;
	int32_t steer_home_enable;
};

struct SmartMotor_Settings
{
	int32_t steer_motor_acc;
	int32_t steer_motor_vel;
};

struct Steer_Settings
{
	int32_t steer_home_enable;
	int32_t steer_min_pos;
	int32_t steer_max_pos;
	int32_t max_steering;
};

struct Steer_Encoder_Settings
{
	int32_t steer_encoder_home;
	int32_t steer_encoder_lpos;
	int32_t steer_encoder_rpos;
};

struct SmartMotorSteerConfig
{
	SmartMotor_DataLog params_SmartMotor_DataLog;
	SmartMotor_Port params_SmartMotor_Port;
	SteerEncoder_Port params_SteerEncoder_Port;
	SmartMotor_Enabled params_SmartMotor_Enabled;
	SmartMotor_Settings params_SmartMotor_Settings;
	Steer_Settings params_Steer_Settings;
	Steer_Encoder_Settings params_Steer_Encoder_Settings;
};

}

#endif /* ifndef SmartMotorSteerCONFIGURATION_H_ */


