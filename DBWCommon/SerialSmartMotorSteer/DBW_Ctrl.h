/*
 * DBW_Ctrl.h
 *
 *  Created on: Jul 19, 2012
 *      Author: Loh Yong Hua (KDM/USC)
 */

#ifndef DBW_CTRL_H
#define DBW_CTRL_H

#include <sys/types.h>
#include <iostream>
#include <cstdint>
#include <unistd.h>
#include <pthread.h>
//
#include "mu_common.h"
#include "smartmotor.h"
#include "motrona_IV251_Interface.h"
//
#include "../../PcanSeikoEncoder/PcanSeikoPcanEncoder.h"

//#include "DBW_Struct.h"
#define EXTRA_POS 1000
#define MAX_STEER 30.0
using namespace std;

enum enumGearShift
{
   Gear_Park,
   Gear_Reverse,
   Gear_Neutral,
   Gear_Drive,
   Gear_Invalid
};
enum motor_mode
{
   position_mode,
   velocity_mode
};

class DBW_Ctrl
{
public:
	DBW_Ctrl();
	~DBW_Ctrl();

	void init_steer_encoder(char * devname);
	void init_steer_motor(char * devname);
	bool init_throttle_motor(char * devname);
	void init_gear_motor(char * devname);

	void set_steer_motor_parameters(int min_pos, int max_pos);
	void set_throttle_motor_parameters(int min_pos, int max_pos);
	void set_gear_motor_parameters(int dist_P_R, int dist_R_N, int dist_N_D);

	//
	void set_motor_torque(void);
	void set_motor_position(void);
	void set_throttle_motor_torque(int torque);
	void set_throttle_motor_torque_slope(int torque_slope);
	void set_torque_motor_stop(void);
	int get_motor_velocity(void);
	int get_motor_vel(mu_smartmotor_p sm);
	int get_motor_position(void);
	bool get_brake_motor_aliveness(void);

	void set_steer_motor_mode(int acc, int vel, motor_mode mode);
	void set_throttle_motor_mode(int acc, int vel, motor_mode mode);
	void set_gear_motor_mode(int acc, int vel, motor_mode mode);

	void set_pallet_truck_desired_steer_angle_motor_pos(float steer_angle_deg);
	void set_steer_motor_pos(int pos, int complete_traj);
	void set_throttle_motor_pos(int pos, int complete_traj);
	void set_gear_motor_pos(int pos, int complete_traj);
	void change_gear(enumGearShift cur, enumGearShift next);

	void set_throttle_motor_origin (void);
	void set_steer_motor_origin (void);
	void set_gear_motor_origin (void);

	void Center_SteerMotor(unsigned int encoder_home, uint32_t current_steer_enc_pos);
	void Get_HomeMotorPosition(int &home_motor_position);

	void stop_steer_motor(void);
	void stop_throttle_motor(void);
	void stop_gear_motor(void);
	enumGearShift report_current_gear(void);

	int get_steer_motor_pos(void);
	int get_throttle_motor_pos(void);
	int get_gear_motor_pos(void);
	int get_steer_encoder_pos(void);

	int get_steer_motor_over_current_Status(void);
	double get_steer_motor_voltage(void);
	double get_steer_motor_current(void);
	int get_throttle_motor_over_current_Status(void);
	double get_throttle_motor_voltage(void);
	double get_throttle_motor_current(void);
	int get_gear_motor_over_current_Status(void);
	double get_gear_motor_voltage(void);
	double get_gear_motor_current(void);

	unsigned int get_steer_motor_status(void);
	unsigned int get_throttle_motor_status(void);
	unsigned int get_gear_motor_status(void);

	void BrakeON(int pos);
	void BrakeOFF(void);

	void set_steer_motor_off(void);
	void set_throttle_motor_off(void);
	void set_gear_motor_off(void);

	void WakeupThrottleActuator(void);
	void release_motorsNencoder(void);
	void disconnect_throttle_motor();

private:
	void set_gear_motor (enumGearShift cur, enumGearShift next);
	int get_motor_pos(mu_smartmotor_p sm);
	int get_motor_trajBit(mu_smartmotor_p sm);
	unsigned int get_motor_Status(mu_smartmotor_p sm);
	int get_motor_over_current_Status(mu_smartmotor_p sm);
	double get_motor_Current(mu_smartmotor_p sm);
	double get_motor_Voltage(mu_smartmotor_p sm);
	void WaitTrajBitClear(mu_smartmotor_p sm);
	void motor_off(mu_smartmotor_p sm);
	void WakeupActuator(mu_smartmotor_p sm);

	bool steer_motor_enable;
	bool throttle_motor_enable;
	bool gear_motor_enable;
	bool steer_encoder_enable;

	mu_smartmotor_p steer_motor;
	mu_smartmotor_p throttle_motor;
	mu_smartmotor_p gear_motor;
	MOTRONA_IV251_INTERFACE cSSI;
	//
	DBWPalletCommon::PcanSeikoPcanEncoder cPcanSeikoEncoder;

	bool bmotor_released;

	pthread_mutex_t throttle_motor_mutex;
	pthread_mutex_t steer_motor_mutex;
	pthread_mutex_t gear_motor_mutex;


	int throttle_motor_acc, throttle_motor_vel;
	int steer_motor_acc, steer_motor_vel;
	int gear_motor_acc, gear_motor_vel;

	int gear_Dist_P_R, gear_Dist_R_N, gear_Dist_N_D;
	int throttle_min_pos, throttle_max_pos, throttle_startpos;
	int steer_min_pos, steer_max_pos;

	enumGearShift current_Gear;

	//Pallet truck steer DBW
	int init_desired_motor_abs_enc_home_position; //pcan seiko encoder defined in config file
	int init_sm_motor_enc_home_position;  //internal smart motor encoder
	int init_motor_pos_cmd_home_position;

};

#endif
