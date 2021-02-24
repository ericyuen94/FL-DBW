#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "TestPCMCardManager.h"

int32_t main(int32_t argc , char **argv)
{
	  //Instantiate Manager
	TestPCMCard::TestPCMCardManager cTestPCMCardManager(argc,argv);
	cTestPCMCardManager.run();

    std::cout<<"Application end" <<std::endl;
}
