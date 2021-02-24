#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "BenchTestSteerCalibPlayerManager.h"

int32_t main(int32_t argc , char **argv)
{
	//Instantiate Manager
	BenchTestSteerCalibPlayer::BenchTestSteerCalibPlayerManager cBenchTestSteerCalibPlayerManager(argc,argv);
	cBenchTestSteerCalibPlayerManager.run();

    std::cout<<"Application end" <<std::endl;
}
