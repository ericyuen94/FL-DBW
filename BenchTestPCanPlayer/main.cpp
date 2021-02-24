#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "BenchTestPCanPlayerManager.h"

int32_t main(int32_t argc , char **argv)
{
	//Instantiate Manager
	BenchTestPCanPlayer::BenchTestPCanPlayerManager cBenchTestPCanPlayerManager(argc,argv);
	cBenchTestPCanPlayerManager.run();

    std::cout<<"Application end" <<std::endl;
}
