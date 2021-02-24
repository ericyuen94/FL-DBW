/*
 * joystick_generic.cpp
 *
 *  Created on: Mar 26, 2013
 *      Author: autopave
 */

#include "joystick_generic.h"

namespace TeleOps
{

joystick_generic::joystick_generic ()
{

	joystick_file_descriptor = 0;

	number_of_axis           = 0;

	number_of_buttons        = 0;

	axis_value               = NULL;

	buttons_state            = NULL;
}

joystick_generic::~joystick_generic() {

}

int joystick_generic::initialize_joystick( std::string file_name ) {

	if( ( joystick_file_descriptor = open( file_name.c_str(), O_RDONLY ) ) == -1 ) {

		std::cerr << "Fail to open joy stick..." << std::endl;

		return -1;
	}


	ioctl( joystick_file_descriptor, JSIOCGAXES     , &number_of_axis    );

	ioctl( joystick_file_descriptor, JSIOCGBUTTONS  , &number_of_buttons );

	ioctl( joystick_file_descriptor, JSIOCGNAME(80) , &name_of_joystick  );


	axis_value    = ( int *  )calloc( number_of_axis   , sizeof( int  ) );
	if( axis_value == NULL     ) {

		std::cerr << "Fail to open allocate memory..." << std::endl;

		return -3;
	}

	buttons_state = ( char * )calloc( number_of_buttons, sizeof( char ) );
	if( buttons_state == NULL ) {

		std::cerr << "Fail to open allocate memory..." << std::endl;

		return -3;
	}

//	std::cout << "Joy stick detected :: " << name_of_joystick  << "   "
//			  << "Number of axis     :: " << number_of_axis    << "   "
//			  << "Number of buttons  :: " << number_of_buttons << std::endl;

	fcntl( joystick_file_descriptor, F_SETFL , O_NONBLOCK );

	return 0;
}

void joystick_generic::deinitialize_joystick() {

	close( joystick_file_descriptor    );

	return;
}

void  joystick_generic::get_joystick_parameters( int &no_of_axis, int &no_of_buttons ) {

	no_of_axis		= number_of_axis;

	no_of_buttons	= number_of_buttons;

	return;
}

int  joystick_generic::get_joystick_message( int axis[], char buttons[] ) {

	if( read( joystick_file_descriptor, &js , sizeof( struct js_event ) ) < 0 ) 	return -4;

	switch( js.type & ~JS_EVENT_INIT ) {

		case JS_EVENT_AXIS 		:	axis_value   [ js.number ] = js.value;		break;

		case JS_EVENT_BUTTON	:	buttons_state[ js.number ] = js.value;		break;
	}

	for( int axis_no = 0; axis_no < number_of_axis; axis_no++ ) {

		axis[axis_no] = axis_value[axis_no];
		//std::cout << "Axis " 	<< axis_no << " = " 	<< (float)axis[axis_no] 	<< std::endl;
	}

	for( int button_no = 0; button_no < number_of_buttons; button_no++ ) {

		buttons[button_no] = buttons_state[button_no];
		//std::cout << "Joy Button " 	<< button_no << " = " 	<< (int)buttons[button_no] 	<< std::endl;
	}

	return 0;
}
}
