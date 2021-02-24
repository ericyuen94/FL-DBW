/*
 * CalibSteerStruct.h
 *
 *  Created on: Dec 25, 2017
 *      Author: emily
 */

#ifndef CALIBSTEERSTRUCT_H_
#define CALIBSTEERSTRUCT_H_

namespace calibsteerEmulator
{

struct sSM_Status_t
{
	//
	uint64_t timestamp;
	//
	double default_max_steer_angle; //degrees
	double command_steer_angle; //degrees

	//
	int default_seiko_home_pos;
	int default_seiko_extreme_left_pos;
	int default_seiko_extreme_right_pos;
	int feedback_seiko_encoder_pos;
	double fb_seiko_steer_angle;

	//
	double feedback_platform_speed;//m/s
	double feedback_platform_steer_angle; //degrees

	//
	double gps_smooth_x;
	double gps_smooth_y;
	double gps_vel;//m/s
	double gps_yaw;//rad
	double gps_yawrate;//rad/s
	double gps_aux_steer;//rad/s
	//
	double computed_gps_curvature;
	double computed_platform_curvature;
	//
	unsigned char ucBrakeTrigger;	// 0: Brake not trigger, 1: Brake is trigger
	unsigned char ucESTOP;	// 0: ESTOP not trigger, 1: ESTOP is trigger
	//
	double brake_dist;
	double brake_timestamp;
};


}//namespace

#endif /* CALIBSTEERSTRUCT_H_ */
