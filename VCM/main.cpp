#include <iostream>
#include <cstdint>

// v1.0 vcm baseline the vehicle - Shell Jurong Island - 27 Dec 2018
// v2.0 removed vcm ntu cmds - Darren

#include "Common/Utility/TimeUtils.h"
#include "VCMManager.h"

int32_t main(int32_t argc , char **argv)
{
	std::cout << "===========================" << std::endl;
	std::cout << "VCM v2.0" << std::endl;
	std::cout << "===========================" << std::endl;

    //Instantiate Manager
	VCM::VCMManager cVCMManager(argc,argv);
	cVCMManager.run();

    std::cout<<"Application end" <<std::endl;
}
