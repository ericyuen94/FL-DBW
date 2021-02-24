/*
 * DBW_Ctrl.cpp
 *
 *  Created on: Jul 19, 2012
 *      Author: Loh Yong Hua (KDM/USC)
 */

#include "DBW_Ctrl.h"

/* Constructor */
DBW_Ctrl::DBW_Ctrl()
{
	steer_motor_enable = false;
	throttle_motor_enable = false;
	gear_motor_enable = false;
	steer_encoder_enable = false;

	bmotor_released = false;

	current_Gear = Gear_Park;

	pthread_mutex_init(&steer_motor_mutex,NULL);
	pthread_mutex_init(&throttle_motor_mutex,NULL);
	pthread_mutex_init(&gear_motor_mutex,NULL);
}

/* Destructor */
DBW_Ctrl::~DBW_Ctrl()
{
	pthread_mutex_destroy(&throttle_motor_mutex);
	pthread_mutex_destroy(&steer_motor_mutex);
	pthread_mutex_destroy(&gear_motor_mutex);
}

/* Connect to Motrona IV251 */
void DBW_Ctrl::init_steer_encoder(char *devname)
{
	int err;

	steer_encoder_enable = true;
	if (steer_encoder_enable)
	{
		err = cSSI.mu_motrona_connect_serial((char*)devname);
		if(err==0)
			printf("Connection to Motrona IV251 established.\n");
		else
		{
			printf("Failed to connect to Motrona IV251.\n");
			exit(0);
		}
	}
}

// Initialise smart motor for steering */
void DBW_Ctrl::init_steer_motor(char *devname)
{
	steer_motor_enable = true;

	pthread_mutex_lock(&steer_motor_mutex);
	if (steer_motor_enable)
	{
		steer_motor = mu_smartmotor_connect_serial((char*)devname);
		if(steer_motor == NULL)
			mu_kill("Failed to connect to steer motor at %s\n", devname);
		fprintf(stderr,"Connected to Steering SmartMotor\n");

		//mu_smartmotor_set_abs_pos(steer_motor, 0);
	}

	pthread_mutex_unlock(&steer_motor_mutex);
}

// Initialise smart motor for throttle */
bool DBW_Ctrl::init_throttle_motor(char *devname)
{
	throttle_motor_enable = true;
	bool init_motor = false;

	pthread_mutex_lock(&throttle_motor_mutex);
	if (throttle_motor_enable)
	{
		throttle_motor = mu_smartmotor_connect_serial((char*)devname);
		if(throttle_motor == NULL)
		{
			mu_kill("Failed to connect to throttle motor at %s\n", devname);
		}
		else
		{
			fprintf(stderr,"Connected to Throttle SmartMotor\n");
			init_motor = true;
			//init motor
			mu_smartmotor_initialize(throttle_motor);
		}
	}
	pthread_mutex_unlock(&throttle_motor_mutex);
	return init_motor;
}
// Disconnect smart motor for throttle
void DBW_Ctrl::disconnect_throttle_motor()
{
	pthread_mutex_lock(&throttle_motor_mutex);
	mu_smartmotor_disconnect(throttle_motor);
	throttle_motor_enable = false;
	pthread_mutex_unlock(&throttle_motor_mutex);
}


// Initialise smart motor for gear */
void DBW_Ctrl::init_gear_motor(char *devname)
{
	gear_motor_enable = true;

	pthread_mutex_lock(&gear_motor_mutex);
	if (gear_motor_enable)
	{
		gear_motor = mu_smartmotor_connect_serial((char*)devname);
		if(gear_motor == NULL)
			mu_kill("Failed to connect to gear motor at %s\n", devname);
		usleep(1000);
		fprintf(stderr,"Connected to Gear SmartMotor\n");
	}
	pthread_mutex_unlock(&gear_motor_mutex);
}

/* Set the steer motor parameters */
void DBW_Ctrl::set_steer_motor_parameters(int min_pos, int max_pos)
{
	steer_min_pos = min_pos;
	steer_max_pos = max_pos;
}

/* Set the throttle motor parameters */
void DBW_Ctrl::set_throttle_motor_parameters(int min_pos, int max_pos)
{
	throttle_min_pos = min_pos;
	std::cout << "throttle_min_pos ====================== " << throttle_min_pos << std::endl;
	throttle_max_pos = max_pos;
	std::cout << "throttle_max_pos ====================== " << throttle_max_pos << std::endl;
}

/*Set motor to torque mode*/
void DBW_Ctrl::set_motor_torque()
{
	pthread_mutex_lock(&throttle_motor_mutex);
	mu_smartmotor_set_torque_mode(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);
}

/*Set motor to position mode*/
void DBW_Ctrl::set_motor_position()
{
	pthread_mutex_lock(&throttle_motor_mutex);
	mu_smartmotor_set_position_mode(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);
}

/*Set torque motor value*/
void DBW_Ctrl::set_throttle_motor_torque(int torque)
{
	pthread_mutex_lock(&throttle_motor_mutex);
	mu_smartmotor_set_torque_val(throttle_motor,torque);
	pthread_mutex_unlock(&throttle_motor_mutex);
}

/*Set torque slope value*/
void DBW_Ctrl::set_throttle_motor_torque_slope(int torque_slope)
{
	pthread_mutex_lock(&throttle_motor_mutex);
	mu_smartmotor_set_torque_slope_val(throttle_motor,torque_slope);
	pthread_mutex_unlock(&throttle_motor_mutex);
}

/*Stop torque motor*/
void DBW_Ctrl::set_torque_motor_stop()
{
	pthread_mutex_lock(&throttle_motor_mutex);
	mu_smartmotor_stop(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);
}

/* Obtain velocity of the motor */
int DBW_Ctrl::get_motor_velocity(void)
{
	int cur_vel;

	pthread_mutex_lock(&throttle_motor_mutex);
	cur_vel = get_motor_vel(throttle_motor);
	std::cout << "Current velocity is ============= " << cur_vel << std::endl;
	pthread_mutex_unlock(&throttle_motor_mutex);

	return cur_vel;
}

/* Obtain velocity of the motor */
int DBW_Ctrl::get_motor_position(void)
{
	int cur_pos;

	pthread_mutex_lock(&throttle_motor_mutex);
	cur_pos = get_motor_pos(throttle_motor);
	std::cout << "Current pos is ============= " << cur_pos << std::endl;
	pthread_mutex_unlock(&throttle_motor_mutex);

	return cur_pos;
}


/* Obtain absolute position of the motor */
int DBW_Ctrl::get_motor_vel(mu_smartmotor_p sm)
{
	char buffer[1000];
	int nread;
	int cur_pos;
	int i;

//	pthread_mutex_lock(&throttle_motor_mutex);
	char command[16];
	memset(command,0x00,sizeof(command));
	command[0]=0x52;//R
	command[1]=0x56;//V
	command[2]=0x41;//A
	command[3]=0x20;//SP
	mu_smartmotor_bytes_write(sm, (unsigned char*)command,8);

	for(i=0; i<1000; i++)
	{
		//std::cout << "i = " << i << std::endl;
		nread = mu_smartmotor_readn(sm, (unsigned char *)buffer, 1000);
		if(nread>0)
		{
			break;
		}
	}
//	pthread_mutex_unlock(&throttle_motor_mutex);

	if (nread > 0)
	{
		buffer[nread] = '\0';
		cur_pos = atoi(buffer);
	}

	return cur_pos;
}


/* Set the gear motor parameters */
void DBW_Ctrl::set_gear_motor_parameters(int dist_P_R, int dist_R_N, int dist_N_D)
{
	gear_Dist_P_R = dist_P_R;
	gear_Dist_R_N = dist_R_N;
	gear_Dist_N_D = dist_N_D;
}

/* Set steer motor to velocity mode */
//void DBW_Ctrl::set_steer_motor_mode(int acc, int vel, motor_mode mode)
//{
//	pthread_mutex_lock(&steer_motor_mutex);
//	if (mode == position_mode)
//	{
//		mu_smartmotor_initialize(steer_motor, acc, vel);
//		std::cout << "Mode is =================== " << std::endl;
//	}
//	else if (mode == velocity_mode)
//		mu_smartmotor_initialize_mv(steer_motor, acc, vel);
//	pthread_mutex_unlock(&steer_motor_mutex);
//}

/* Set throttle motor to velocity mode */
//void DBW_Ctrl::set_throttle_motor_mode(int acc, int vel, motor_mode mode)
//{
//	pthread_mutex_lock(&throttle_motor_mutex);
//	if (mode == position_mode)
//	{
//		mu_smartmotor_initialize(throttle_motor, acc, vel);
//		std::cout << "Mode is =================== " << std::endl;
//	}
//	else if (mode == velocity_mode)
//		mu_smartmotor_initialize_mv(throttle_motor, acc, vel);
//	pthread_mutex_unlock(&throttle_motor_mutex);
//}

/* Set gear motor to velocity mode */
//void DBW_Ctrl::set_gear_motor_mode(int acc, int vel, motor_mode mode)
//{
//	pthread_mutex_lock(&gear_motor_mutex);
//	if (mode == position_mode)
//		mu_smartmotor_initialize(gear_motor, acc, vel);
//	else if (mode == velocity_mode)
//		mu_smartmotor_initialize_mv(gear_motor, acc, vel);
//	pthread_mutex_unlock(&gear_motor_mutex);
//}

void DBW_Ctrl::set_pallet_truck_desired_steer_angle_motor_pos(float steer_angle_deg)
{
	int initial_home_motor_position;
	int send_motor_position;

	//left is pos, right is negative
	float steer_angle_deg_opposite = steer_angle_deg*-1.0;
	//
	Get_HomeMotorPosition(initial_home_motor_position);
	//
	send_motor_position = initial_home_motor_position-(steer_angle_deg_opposite*1228.0702);
	set_steer_motor_pos(send_motor_position, 0);

	//
	//std::cout << "Send init_motor_pos_cmd_home_position "  << initial_home_motor_position << std::endl;
	//std::cout << "Send motor commands "  << send_motor_position << std::endl;
}

/* Set the smartmotor that control the steering to the desired position.
 * If complete_traj is set to '1', function will only return to caller
 * when motor complete the trajectory.
 * */
void DBW_Ctrl::set_steer_motor_pos(int pos, int complete_traj)
{
	pthread_mutex_lock(&steer_motor_mutex);
	mu_smartmotor_set_abs_pos(steer_motor, pos);
	pthread_mutex_unlock(&steer_motor_mutex);

	if (complete_traj)
		WaitTrajBitClear(steer_motor);
}

/* Set the smart motor that control the brake & throttle to the desired position.
 * If complete_traj is set to '1', function will only return to caller
 * when motor complete the trajectory.
 * */
void DBW_Ctrl::set_throttle_motor_pos(int pos, int complete_traj)
{
	pthread_mutex_lock(&throttle_motor_mutex);
	mu_smartmotor_set_abs_pos(throttle_motor, pos);
	//std::cout << "Absolute position =================== " << pos << std::endl;
	pthread_mutex_unlock(&throttle_motor_mutex);

	if (complete_traj)
		WaitTrajBitClear(throttle_motor);
}


/* Set the smart motor that control the gear to the desired position.
 * If complete_traj is set to '1', function will only return to caller
 * when motor complete the trajectory.
 * */
void DBW_Ctrl::set_gear_motor_pos(int pos, int complete_traj)
{
	pthread_mutex_lock(&gear_motor_mutex);
	mu_smartmotor_set_abs_pos(gear_motor, pos);
	pthread_mutex_unlock(&gear_motor_mutex);

	if (complete_traj)
		WaitTrajBitClear(gear_motor);
}

/* Shift gear from current gear to a specific gear,
 * function will wait the desired gear is reached
 * */
void DBW_Ctrl::change_gear(enumGearShift cur, enumGearShift next)
{
	int TrajBit;

	fprintf(stderr,"change_gear in\n");
	if (gear_motor_enable)
	{
		set_gear_motor(cur,next);

		pthread_mutex_lock(&gear_motor_mutex);
		mu_smartmotor_wait(gear_motor);
		TrajBit = get_motor_trajBit(gear_motor);

		while (TrajBit!=0)
		{
			TrajBit = get_motor_trajBit(gear_motor);
			usleep(1000);
		};
		pthread_mutex_unlock(&gear_motor_mutex);

		current_Gear = next;
	}
	//fprintf(stderr,"change_gear out\n");
}

void DBW_Ctrl::set_throttle_motor_origin (void)
{
	pthread_mutex_lock(&throttle_motor_mutex);
	mu_smartmotor_set_origin(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);
}

/* Set Steer motor origin */
void DBW_Ctrl::set_steer_motor_origin (void)
{
	pthread_mutex_lock(&steer_motor_mutex);
	mu_smartmotor_set_origin(steer_motor);
	pthread_mutex_unlock(&steer_motor_mutex);
}

/* Set Gear motor origin */
void DBW_Ctrl::set_gear_motor_origin (void)
{
	pthread_mutex_lock(&gear_motor_mutex);
	mu_smartmotor_set_origin(gear_motor);
	pthread_mutex_unlock(&gear_motor_mutex);
	current_Gear = Gear_Park;
}

/* Shift gear from current gear to a specific gear */
void DBW_Ctrl::set_gear_motor (enumGearShift cur, enumGearShift next)
{
	//fprintf(stderr,"[set_gear_motor] START\n");
	pthread_mutex_lock(&gear_motor_mutex);
	switch (next)
	{
		case Gear_Park:		fprintf(stderr,"[set_gear_motor] Park\n");
							mu_smartmotor_set_abs_pos(gear_motor, 0);
							break;

		case Gear_Reverse:	fprintf(stderr,"[set_gear_motor] Reverse\n");
							if (cur == Gear_Park)
								mu_smartmotor_set_abs_pos(gear_motor, gear_Dist_P_R);
							else
								mu_smartmotor_set_abs_pos(gear_motor, gear_Dist_P_R - EXTRA_POS);
							break;

		case Gear_Neutral:	fprintf(stderr,"[set_gear_motor] Neutral\n");
							mu_smartmotor_set_abs_pos(gear_motor, gear_Dist_P_R + gear_Dist_R_N);
							break;

		case Gear_Drive:	fprintf(stderr,"[set_gear_motor] Drive\n");
							mu_smartmotor_set_abs_pos(gear_motor, gear_Dist_P_R + gear_Dist_R_N + gear_Dist_N_D);
							break;
	}
	pthread_mutex_unlock(&gear_motor_mutex);
	//fprintf(stderr,"[set_gear_motor] END\n");
}

void DBW_Ctrl::Get_HomeMotorPosition(int &home_motor_position)
{
	home_motor_position = init_motor_pos_cmd_home_position;
	std::cout << "Home motor position =================================  " << home_motor_position << std::endl;
}

/* Center the Steering Wheel to Home Position */
void DBW_Ctrl::Center_SteerMotor(unsigned int encoder_home, uint32_t current_steer_enc_pos)
{
	int steer_motor_pos=0;
	float cur_diff;
	int step;
	//uint32_t steer_encoder_pos;
	float send_delta_motor_cmd;
	int send_new_motor_cmd;

	//home position for smart motor encoder
	int startup_sm_motor_encoder_pos;
	startup_sm_motor_encoder_pos = get_steer_motor_pos();
	init_sm_motor_enc_home_position = startup_sm_motor_encoder_pos;

	//home position for pcan encoder, xml file
	init_desired_motor_abs_enc_home_position = encoder_home;

	std::cout << " " << std::endl;
	std::cout << "========================================= " << std::endl;
	std::cout << " Centering Steering Wheel in progress" << std::endl;
	std::cout << "========================================= " << std::endl;
	//
	int delta = current_steer_enc_pos - init_desired_motor_abs_enc_home_position;
	//gear ratio for truck 1
//	cur_diff = (delta/42.48)*829.63;
	//gear ratio for stacker
	cur_diff = (delta/41.9181)*1228.0702;
	send_delta_motor_cmd = init_sm_motor_enc_home_position + cur_diff;
	send_new_motor_cmd = static_cast<int>(send_delta_motor_cmd);
	mu_smartmotor_set_abs_pos(steer_motor, send_new_motor_cmd);
	init_motor_pos_cmd_home_position = send_new_motor_cmd;

	//
	std::cout << "init_desired_motor_abs_enc_home_position = " << init_desired_motor_abs_enc_home_position << std::endl;
	std::cout << "init_sm_motor_enc_home_position = " << init_sm_motor_enc_home_position << std::endl;
	std::cout << "seiko steer_encoder_pos = " << current_steer_enc_pos << std::endl;

	std::cout << "cur_diff = " << cur_diff << std::endl;
	std::cout << "send_new_motor_cmd = " << send_new_motor_cmd << std::endl;
	std::cout << "init_motor_pos_cmd_home_position = " << init_motor_pos_cmd_home_position << std::endl;

}


/* Stop Steer smart motor */
void DBW_Ctrl::stop_steer_motor(void)
{
	pthread_mutex_lock(&steer_motor_mutex);
	mu_smartmotor_abruptstop(steer_motor);
	pthread_mutex_unlock(&steer_motor_mutex);
}

/* Stop throttle smart motor */
void DBW_Ctrl::stop_throttle_motor(void)
{
	pthread_mutex_lock(&throttle_motor_mutex);
	mu_smartmotor_abruptstop(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);
}

/* Stop Gear smart motor */
void DBW_Ctrl::stop_gear_motor(void)
{
	pthread_mutex_lock(&gear_motor_mutex);
	mu_smartmotor_abruptstop(gear_motor);
	pthread_mutex_unlock(&gear_motor_mutex);
}

/* Obtain absolute position of the motor */
//int DBW_Ctrl::get_motor_pos(mu_smartmotor_p sm)
//{
//	char buffer[1000];
//	int nread;
//	int cur_pos;
//
//	//pthread_mutex_lock(&motor_mutex);
//	mu_smartmotor_write(sm, (unsigned char*)("RP\n"));
//	nread = mu_smartmotor_readn(sm, (unsigned char *)buffer, 1000);
//	//pthread_mutex_unlock(&motor_mutex);
//
//	if (nread > 0)
//	{
//		buffer[nread] = '\0';
//		cur_pos = atoi(buffer);
//	}
//
//	return cur_pos;
//}

int DBW_Ctrl::get_motor_pos(mu_smartmotor_p sm)
{
	char buffer[1000];
	int nread;
	int cur_pos;

	//printf("\n DBW_Ctrl::get_motor_pos \n");
	//
	char command[16];
	memset(command,0x00,sizeof(command));
	command[0]=0x52;//R
	command[1]=0x56;//P
	command[2]=0x41;//A
	command[3]=0x20;//SP

	printf("Write bytes \n");
	//pthread_mutex_lock(&motor_mutex);
	mu_smartmotor_bytes_write(sm, (unsigned char*)command,4);
//	mu_smartmotor_write(sm, (unsigned char*)command);

	printf("Read N \n");
	int i;
	for(i=0; i<1000; i++)
	{

		nread = mu_smartmotor_readn(sm, (unsigned char *)buffer, 1000);
		printf("Read %d : %d\n", i, nread);
		if(nread>0)
		{
			break;
		}
	}
	//pthread_mutex_unlock(&motor_mutex);
	printf("i = %d nread = %d \n",i, nread);
	for(int i=0; i<nread; i++)
	{
		printf("%x ", buffer[i]);
	}
	printf("\n");
	//
	if (nread > 0)
	{
		buffer[nread] = '\0';
		cur_pos = atoi(buffer);
	}

	return cur_pos;
}

/* Obtain absolute position of the Steer motor */
int DBW_Ctrl::get_steer_motor_pos(void)
{
	int cur_pos;

	pthread_mutex_lock(&steer_motor_mutex);
	cur_pos = get_motor_pos(steer_motor);
	std::cout << "Current steer position is ============= " << cur_pos << std::endl;
	pthread_mutex_unlock(&steer_motor_mutex);

	return cur_pos;
}

/* Obtain absolute position of the Steer motor */
int DBW_Ctrl::get_throttle_motor_pos(void)
{
	int cur_pos;

	pthread_mutex_lock(&throttle_motor_mutex);
	cur_pos = get_motor_pos(throttle_motor);
	std::cout << "Current position is ============= " << cur_pos << std::endl;
	pthread_mutex_unlock(&throttle_motor_mutex);

	return cur_pos;
}

/* Obtain absolute position of the Steer motor */
int DBW_Ctrl::get_gear_motor_pos(void)
{
	int cur_pos;

	pthread_mutex_lock(&gear_motor_mutex);
	cur_pos = get_motor_pos(gear_motor);
	pthread_mutex_unlock(&gear_motor_mutex);

	return cur_pos;
}

/* Report current Gear */
enumGearShift DBW_Ctrl::report_current_gear(void)
{
	int pos_Park = EXTRA_POS;
	int pos_Reverse = gear_Dist_P_R-EXTRA_POS;
	int pos_Neutral = gear_Dist_P_R + gear_Dist_R_N;
	int pos_Drive = gear_Dist_N_D + pos_Neutral;

	int gear_pos = get_gear_motor_pos();
	//fprintf(stderr,"[report_current_gear] gear_pos:%d\n",gear_pos);

	if (gear_pos >= -500 && gear_pos <= 500)
	{
		//fprintf(stderr,"[report_current_gear] PARK\n");
		return Gear_Park;
	}
	else if ((gear_pos >= pos_Reverse-500) && (gear_pos <= pos_Reverse + EXTRA_POS + 500))
	{
		//fprintf(stderr,"[report_current_gear] REVERSE\n");
		return Gear_Reverse;
	}
	else if (gear_pos >= pos_Neutral-500 && gear_pos <= pos_Neutral + 500)
	{
		//fprintf(stderr,"[report_current_gear] NEUTRAL\n");
		return Gear_Neutral;
	}
	else if (gear_pos >= pos_Drive-500)// && gear_pos <= pos_Drive + 500 )
	{
		//fprintf(stderr,"[report_current_gear] DRIVE\n");
		return Gear_Drive;
	}
	else
	{
		//fprintf(stderr,"[report_current_gear] INVALID\n");
		return Gear_Invalid;
	}
}

/* Obtain steer encoder position */
int DBW_Ctrl::get_steer_encoder_pos(void)
{
	return cSSI.mu_motrona_enc_pos_read();
}

/* Get trajectory bit of the motor
 * Function will return:	'1', if motor is in motion,
 * 							'0', if motor is stationary.
 */
int DBW_Ctrl::get_motor_trajBit(mu_smartmotor_p sm)
{
	char buffer[1000];
	int nread;
	int TrajBit=-1;

	//pthread_mutex_lock(&motor_mutex);
	mu_smartmotor_write(sm, (unsigned char*)("RBt\n"));
	nread = mu_smartmotor_readn(sm, (unsigned char *)buffer, 1000);
	//pthread_mutex_unlock(&motor_mutex);

	if (nread > 0)
	{
		buffer[nread] = '\0';
		TrajBit = atoi(buffer);
		//fprintf(stderr,"nread:%d\nbuffer:%s\nTrajBit:%d\n",nread,buffer,TrajBit);
	}

	return TrajBit;
}

/* Get the status of the motor
 * Function will return:	'status', return motor status
 * 							'0', otherwise.
 */
unsigned int DBW_Ctrl::get_motor_Status(mu_smartmotor_p sm)
{
	char buffer[1000];
	int nread;
	unsigned int status = 0;

	//pthread_mutex_lock(&motor_mutex);
	mu_smartmotor_write(sm, (unsigned char*)("RW\n"));
	nread = mu_smartmotor_readn(sm, (unsigned char *)buffer, 1000);
	//pthread_mutex_unlock(&motor_mutex);

	if (nread > 0)
	{
		buffer[nread] = '\0';
		status = atoi(buffer);
	}

	return status;
}

/* Get the over current status of the motor
 * Function will return:	'1', if the motor encounter over current
 * 							'0', otherwise.
 */
int DBW_Ctrl::get_motor_over_current_Status(mu_smartmotor_p sm)
{
	char buffer[1000];
	int nread;
	int over_curr_status = 0;

	//pthread_mutex_lock(&motor_mutex);
	mu_smartmotor_write(sm, (unsigned char*)("RBa\n"));
	nread = mu_smartmotor_readn(sm, (unsigned char *)buffer, 1000);
	//pthread_mutex_unlock(&motor_mutex);

	if (nread > 0)
	{
		buffer[nread] = '\0';
		over_curr_status = atoi(buffer);
	}

	return over_curr_status;
}

/* Get SmartMotor Current in Amps
 * Function will return:	SmartMotor Current in Amps.
 * 							'-1', when error occurred.
 */
double DBW_Ctrl::get_motor_Current(mu_smartmotor_p sm)
{
	char buffer[1000];
	int nread;
	int icurr_mA = 0;
	double dcurr_A = 0.0;

	//pthread_mutex_lock(&motor_mutex);
	mu_smartmotor_write(sm, (unsigned char*)("z=UIA\n"));
	mu_smartmotor_write(sm, (unsigned char*)("Rz\n"));
	nread = mu_smartmotor_readn(sm, (unsigned char *)buffer, 1000);
	//pthread_mutex_unlock(&motor_mutex);

	if (nread > 0)
	{
		buffer[nread] = '\0';
		icurr_mA = atoi(buffer);
		dcurr_A = ((double)icurr_mA)*0.01;
	}
	else
		dcurr_A = -1.0;

	return dcurr_A;
}


/* Get SmartMotor Voltage
 * Function will return:	SmartMotor Voltages.
 * 							'-1', when error occurred.
 */
double DBW_Ctrl::get_motor_Voltage(mu_smartmotor_p sm)
{
	char buffer[1000];
	int nread;
	int ivolts_tenths = 0;
	double dvolts= 0.0;

	//pthread_mutex_lock(&motor_mutex);
	mu_smartmotor_write(sm, (unsigned char*)("z=UJA\n"));
	mu_smartmotor_write(sm, (unsigned char*)("Rz\n"));
	nread = mu_smartmotor_readn(sm, (unsigned char *)buffer, 1000);
	//pthread_mutex_unlock(&motor_mutex);

	if (nread > 0)
	{
		buffer[nread] = '\0';
		ivolts_tenths = atoi(buffer);
		dvolts = ((double)ivolts_tenths)*0.1;
	}
	else
		dvolts = -1.0;

	return dvolts;
}

/* Get the over current status of the steer motor
 * Function will return:	'1', if the motor encounter over current
 * 							'0', otherwise.
 */
int DBW_Ctrl::get_steer_motor_over_current_Status(void)
{
	return get_motor_over_current_Status(steer_motor);
}

/* Get the voltage level of the steer motor
 * Function will return:	'1', if the motor encounter over current
 * 							'0', otherwise.
 */
double DBW_Ctrl::get_steer_motor_voltage(void)
{
	double mot_voltage;

	pthread_mutex_lock(&steer_motor_mutex);
	mot_voltage = get_motor_Voltage(steer_motor);
	pthread_mutex_unlock(&steer_motor_mutex);

	return mot_voltage;
}

/* Get the current level of the steer motor
 * Function will return:	'1', if the motor encounter over current
 * 							'0', otherwise.
 */
double DBW_Ctrl::get_steer_motor_current(void)
{
	double mot_current;

	pthread_mutex_lock(&steer_motor_mutex);
	mot_current = get_motor_Current(steer_motor);
	pthread_mutex_unlock(&steer_motor_mutex);

	return mot_current;
}

/* Get the over current status of the throttle motor
 * Function will return:	'1', if the motor encounter over current
 * 							'0', otherwise.
 */
int DBW_Ctrl::get_throttle_motor_over_current_Status(void)
{
	int status;

	pthread_mutex_lock(&throttle_motor_mutex);
	status = get_motor_over_current_Status(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);

	return status;
}

/* Get the voltage level of the throttle motor
 * Function will return:	'1', if the motor encounter over current
 * 							'0', otherwise.
 */
double DBW_Ctrl::get_throttle_motor_voltage(void)
{
	double mot_voltage;

	pthread_mutex_lock(&throttle_motor_mutex);
	mot_voltage = get_motor_Voltage(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);

	return mot_voltage;
}

/* Get the current level of the throttle motor
 * Function will return:	'1', if the motor encounter over current
 * 							'0', otherwise.
 */
double DBW_Ctrl::get_throttle_motor_current(void)
{
	double mot_current;

	pthread_mutex_lock(&throttle_motor_mutex);
	mot_current = get_motor_Current(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);

	return mot_current;
}

/* Get the over current status of the gear motor
 * Function will return:	'1', if the motor encounter over current
 * 							'0', otherwise.
 */
int DBW_Ctrl::get_gear_motor_over_current_Status(void)
{
	int status;

	pthread_mutex_lock(&gear_motor_mutex);
	status = get_motor_over_current_Status(gear_motor);
	pthread_mutex_unlock(&gear_motor_mutex);

	return status;
}

/* Get the voltage level of the gear motor
 * Function will return:	'1', if the motor encounter over current
 * 							'0', otherwise.
 */
double DBW_Ctrl::get_gear_motor_voltage(void)
{
	double mot_voltage;

	pthread_mutex_lock(&gear_motor_mutex);
	mot_voltage = get_motor_Voltage(gear_motor);
	pthread_mutex_unlock(&gear_motor_mutex);

	return mot_voltage;
}

/* Get the current level of the gear motor
 * Function will return:	'1', if the motor encounter over current
 * 							'0', otherwise.
 */
double DBW_Ctrl::get_gear_motor_current(void)
{
	double mot_current;

	pthread_mutex_lock(&gear_motor_mutex);
	mot_current = get_motor_Current(gear_motor);
	pthread_mutex_unlock(&gear_motor_mutex);

	return mot_current;
}


/* Get the status of steer motor
 * Function will return:	'0-255', 16-bit of steer motor status
 * 							'0', otherwise.
 */
unsigned int DBW_Ctrl::get_steer_motor_status(void)
{
	unsigned int mot_status;

	pthread_mutex_lock(&steer_motor_mutex);
	mot_status = get_motor_Status(steer_motor);
	pthread_mutex_unlock(&steer_motor_mutex);

	return mot_status;
}

/* Get the status of throttle motor
 * Function will return:	'0-255', 16-bit of throttle motor status
 * 							'0', otherwise.
 */
unsigned int DBW_Ctrl::get_throttle_motor_status(void)
{
	unsigned int mot_status;

	pthread_mutex_lock(&throttle_motor_mutex);
	mot_status = get_motor_Status(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);

	return mot_status;
}

/* Get the status of gear motor
 * Function will return:	'0-255', 16-bit of gear motor status
 * 							'0', otherwise.
 */
unsigned int DBW_Ctrl::get_gear_motor_status(void)
{
	unsigned int mot_status;

	pthread_mutex_lock(&gear_motor_mutex);
	mot_status = get_motor_Status(gear_motor);
	pthread_mutex_unlock(&gear_motor_mutex);

	return mot_status;
}

/* Function will return when the trajectory bit of the motor is cleared.
 * (or when the motor stop moving)
 * */
void DBW_Ctrl::WaitTrajBitClear(mu_smartmotor_p sm)
{
	int TrajBit;

	while (TrajBit!=0)
	{
		TrajBit = get_motor_trajBit(sm);
		usleep(1000);
	};
}

/* Apply Brake */
void DBW_Ctrl::BrakeON (int pos)
{
	if (throttle_motor_enable)
	{
		if (pos <= (throttle_min_pos * 0.8) )
		{
			set_throttle_motor_pos(pos, 0);
			std::cout << "Brake position 1 ============ " << pos << std::endl;
		}
		else
		{
			set_throttle_motor_pos(throttle_min_pos, 0);
			std::cout << "Brake position 2 ============ " << pos << std::endl;
		}
	}
}

/* Disengage the brake */
void DBW_Ctrl::BrakeOFF (void)
{
	if (throttle_motor_enable)
	{
		set_throttle_motor_pos(0, 0);
	}
}

/* Off the desired motor */
void DBW_Ctrl::motor_off(mu_smartmotor_p sm)
{
	//pthread_mutex_lock(&motor_mutex);
	mu_smartmotor_off(sm);
	//pthread_mutex_unlock(&motor_mutex);
}

// Off the smart motor that control the steering
void DBW_Ctrl::set_steer_motor_off(void)
{
	pthread_mutex_lock(&steer_motor_mutex);
	motor_off(steer_motor);
	pthread_mutex_unlock(&steer_motor_mutex);
}

// Off the smart motor that control the brake & throttle
void DBW_Ctrl::set_throttle_motor_off(void)
{
	pthread_mutex_lock(&throttle_motor_mutex);
	motor_off(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);
}

// Off the smart motor that control the gear
void DBW_Ctrl::set_gear_motor_off(void)
{
	pthread_mutex_lock(&gear_motor_mutex);
	motor_off(gear_motor);
	pthread_mutex_unlock(&gear_motor_mutex);
}

// Wakeup Throttle actuator
void DBW_Ctrl::WakeupActuator(mu_smartmotor_p sm)
{
	//pthread_mutex_lock(&motor_mutex);
	mu_smartmotor_wake(sm);
	//pthread_mutex_unlock(&motor_mutex);
}

// Wakeup Throttle actuator
void DBW_Ctrl::WakeupThrottleActuator(void)
{
	pthread_mutex_lock(&throttle_motor_mutex);
	WakeupActuator(throttle_motor);
	pthread_mutex_unlock(&throttle_motor_mutex);
}

/* Release the motor & encoder handlers */
void DBW_Ctrl::release_motorsNencoder(void)
{
	if (!bmotor_released)
	{
		//pthread_mutex_lock(&motor_mutex);
		if (throttle_motor_enable==1)
		{
			set_throttle_motor_pos(0,0);
		}

		if (steer_motor_enable==1)
		{
			set_steer_motor_pos(0, 0);
			set_steer_motor_off();
		}
		//pthread_mutex_unlock(&motor_mutex);

		if (gear_motor_enable==1)
		{
			change_gear(current_Gear, Gear_Park);
			set_gear_motor_off();
		}

		if (steer_encoder_enable ==1)
		{
			cSSI.mu_motrona_disconnect();
		}

		bmotor_released = true;
	}
}

bool DBW_Ctrl::get_brake_motor_aliveness(void)
{
	char buffer[1000];
	int nread;
	int cur_pos;

	//printf("\n DBW_Ctrl::get_motor_pos \n");
	//
	char command[16];
	memset(command,0x00,sizeof(command));
	command[0]=0x52;//R
	command[1]=0x56;//P
	command[2]=0x41;//A
	command[3]=0x20;//SP

	//pthread_mutex_lock(&motor_mutex);
	mu_smartmotor_bytes_write(throttle_motor, (unsigned char*)command,4);
//	mu_smartmotor_write(sm, (unsigned char*)command);

	int i;
	for(i=0; i<20; i++)
	{
		nread = mu_smartmotor_readn(throttle_motor, (unsigned char *)buffer, 1000);
		if(nread>0)
		{
			break;
		}
	}
	//pthread_mutex_unlock(&motor_mutex);
	printf("i = %d nread = %d This is Aliveness\n",i, nread);
	for(int i=0; i<nread; i++)
	{
		printf("%x ", buffer[i]);
	}
	printf("\n");
	//
	if (nread > 0)
	{
		return true;
	}
	else
	{
		return false;
	}

}
