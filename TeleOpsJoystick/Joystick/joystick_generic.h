/*
 * joystick_generic.h
 *
 *  Created on: Mar 26, 2013
 *      Author: autopave
 */
#pragma once
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

namespace TeleOps
{

const int MAXIMUM_JOYSTICK_AXIS    = 10;
const int MAXIMUM_JOYSTICK_BUTTONS = 20;
//
const int32_t joystick_button_a = 0;
const int32_t joystick_button_b = 1;
const int32_t joystick_button_x = 2;
const int32_t joystick_button_y = 3;
const int32_t joystick_button_LB = 4;
const int32_t joystick_button_RB = 5;
const int32_t joystick_button_LT = 6;
const int32_t joystick_button_RT = 7;
//
const int32_t joystick_axis_left_right = 0;
const int32_t joystick_axis_up_down = 1;
const int32_t joystick_axis_ranalog = 2;
const int32_t joystick_axis_rightanalog = 3;
const int32_t joystick_axis_lanalog = 4;
const int32_t joystick_axis_leftanalog = 5;

//
const int32_t JOYSTICK_BUTTON_DEADMAN_SWITCH = joystick_button_LB;
const int32_t JOYSTICK_BUTTON_SW_ESTOP = joystick_button_RB;
const int32_t JOYSTICK_BUTTON_RAISE_FORK = joystick_button_y;
const int32_t JOYSTICK_BUTTON_LOWER_FORK = joystick_button_a;
const int32_t JOYSTICK_BUTTON_MOVE_FORK_LEFT = joystick_button_x;
const int32_t JOYSTICK_BUTTON_MOVE_FORK_RIGHT = joystick_button_b;
//
const int32_t JOYSTICK_AXIS_TRIGGER_FORKTILT = joystick_axis_leftanalog;
const int32_t JOYSTICK_BUTTON_MOVE_FORK_TILT_UP = joystick_button_x;
const int32_t JOYSTICK_BUTTON_MOVE_FORK_TILT_DOWN = joystick_button_b;
//
const int32_t JOYSTICK_AXIS_TRIGGER_BUZZER = joystick_axis_ranalog;
const int32_t JOYSTICK_AXIS_DRIVE_CMD_RANGE = joystick_axis_lanalog;
const int32_t JOYSTICK_AXIS_STEER_CMD_RANGE = joystick_axis_left_right;

class joystick_generic {

	public  :

		joystick_generic ();

		~joystick_generic();


		int  initialize_joystick  ( std::string file_name );

		void deinitialize_joystick();



		int  get_joystick_message   ( int axis[], char buttons[] );

		void get_joystick_parameters( int &no_of_axis, int &no_of_buttons );

	private :

		int  joystick_file_descriptor;

		int  number_of_axis;

		int  number_of_buttons;

		int  *axis_value;

		char *buttons_state;

		char name_of_joystick[80];


		struct js_event js;
};

}


