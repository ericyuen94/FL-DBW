#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "StkciPCMCardManager.h"

int32_t main(int32_t argc , char **argv)
{
	  //Instantiate Manager
	StkciPCMCard::StkciPCMCardManager cStkciPCMCardManager(argc,argv);
	cStkciPCMCardManager.run();

    std::cout<<"Application end" <<std::endl;
}
