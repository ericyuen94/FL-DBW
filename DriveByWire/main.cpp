#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "DBWManager.h"

// v1.0 vcm baseline the vehicle - Shell Jurong Island - 27 Dec 2018
// v2.0 increase steering pump voltage, fixed bug on veh braking

int32_t main(int32_t argc , char **argv)
{
	  std::cout << "===========================" << std::endl;
	  std::cout << "DBW v2.0" << std::endl;
	  std::cout << "===========================" << std::endl;

      //Instantiate Manager
      DBW::DBWManager cDBW(argc,argv);
      cDBW.run();

      std::cout<<"Application end" <<std::endl;
}
