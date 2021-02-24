#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "PlatformForkliftManager.h"

int32_t main(int32_t argc , char **argv)
{
	  //Instantiate Manager
	PlatformForklift::PlatformForkliftManager cPlatformForkliftManager(argc,argv);
	cPlatformForkliftManager.run();

    std::cout<<"Application end" <<std::endl;
}
