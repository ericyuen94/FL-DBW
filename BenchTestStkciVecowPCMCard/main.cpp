#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "StkciVecowPCMCardManager.h"

int32_t main(int32_t argc , char **argv)
{
	  //Instantiate Manager
	StkciVecowPCMCard::StkciVecowPCMCardManager cStkciVecowPCMCardManager(argc,argv);
	cStkciVecowPCMCardManager.run();

    std::cout<<"Application end" <<std::endl;
}
