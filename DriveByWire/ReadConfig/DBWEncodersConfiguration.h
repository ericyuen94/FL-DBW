#ifndef DBWENCODERSCONFIGURATION_H_
#define DBWENCODERSCONFIGURATION_H_

#include <cstring>
#include <vector>
#include <cstdlib>
#include <string>
#include <map>
#include "Common/typedefs.h"

namespace DBW
{

struct Steer_Encoder_Settings
{
	int32_t steer_home_enable;
	int32_t steer_encoder_home;
	int32_t steer_encoder_lpos;
	int32_t steer_encoder_rpos;
	float64_t max_steering;
};

struct Line_Encoder_Settings
{
	int32_t line_home_enable;
	int32_t line_encoder_home;
	int32_t line_encoder_min;
	int32_t line_encoder_max;
};

struct Tilt_Encoder_Settings
{
	int32_t tilt_home_enable;
	int32_t tilt_encoder_home;
	int32_t tilt_encoder_min;
	int32_t tilt_encoder_max;
};

struct DBWEncodersConfig
{
	Steer_Encoder_Settings params_Steer_Encoder_Settings;
	Line_Encoder_Settings params_Line_Encoder_Settings;
	Tilt_Encoder_Settings params_Tilt_Encoder_Settings;
};

}

#endif /* ifndef DBWENCODERSCONFIGURATION_H_ */
