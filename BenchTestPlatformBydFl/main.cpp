#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "PlatformBydFlManager.h"

int32_t main(int32_t argc , char **argv)
{
	  //Instantiate Manager
	PlatformBydFl::PlatformBydFlManager cPlatformForkliftManager(argc,argv);
	cPlatformForkliftManager.run();

    std::cout<<"Application end" <<std::endl;
}
