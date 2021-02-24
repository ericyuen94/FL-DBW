#include <iostream>
#include <cstdint>

// v1.0 tele-operate the vehicle - Shell Jurong Island - 27 Dec 2018
// v1.1 Fixed drive latching onto commands - 29 Apr 2019 - Darren

#include "Common/Utility/TimeUtils.h"
#include "TeleOpsManager.h"

int32_t main(int32_t argc , char **argv)
{
	std::cout << "===========================" << std::endl;
	std::cout << "TeleOps v1.0" << std::endl;
	std::cout << "===========================" << std::endl;

	//Instantiate Manager
	TeleOps::TeleOpsManager cTeleOps(argc,argv);
	cTeleOps.run();

	std::cout<<"Application end" <<std::endl;
}
