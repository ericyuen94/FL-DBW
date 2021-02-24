#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "PlatformStackerManager.h"

int32_t main(int32_t argc , char **argv)
{
	//Instantiate Manager
	PlatformStacker::PlatformStackerManager cPlatformStackerManager(argc,argv);
	cPlatformStackerManager.run();

    std::cout<<"Application end" <<std::endl;
}
