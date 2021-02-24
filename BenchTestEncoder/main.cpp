#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "BenchTestEncoderManager.h"

int32_t main(int32_t argc , char **argv)
{
	  //Instantiate Manager
	BenchTestEncoder::BenchTestEncoderManager cBenchTestEncoderManager(argc,argv);
	cBenchTestEncoderManager.run();

    std::cout<<"Application end" <<std::endl;
}
