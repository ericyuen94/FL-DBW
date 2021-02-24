/*
 * SM_Struct.h
 *
 *  Created on: Aug 15, 2012
 *      Author: Loh Yong Hua
 */

#ifndef SM_STRUCT_H_
#define SM_STRUCT_H_


struct sSM_Status_t
{
	unsigned char ucBrakeTrigger;	// 0: Brake not trigger, 1: Brake is trigger
	unsigned char ucESTOP;	// 0: ESTOP not trigger, 1: ESTOP is trigger
	double smooth_x;
	double smooth_y;
	int enc_pos;
	int steer_mot_pos;
	double gps_vel;
	double djrvs_vel;
	double brake_dist;
	double brake_timestamp;
	double yaw;
	double yawrate;
	double gps_curvature;
	double djrvs_curvature;
	double timestamp;
};

#endif /* SM_STRUCT_H_ */
