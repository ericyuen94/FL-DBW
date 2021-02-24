#include <iostream>
#include <cstdint>

#include "Common/Utility/TimeUtils.h"
#include "PIMManager.h"

//v0.1.0 PIM Baseline - ITAP and Biz Night and ST Aero demos - 30 Oct 2018 (Kianwee)
//v0.2.0 with safety health status - 9 Nov 2018 (Emily)
//v0.3.0 add in fork collision sensors (Darren) - 14 Aug 2019
//v0.3.1 updated warning and error msg (Darren) - 24 Sep 2019
//v0.3.2 new error/warning msgs display - 30 Oct 2019 (Darren)

int32_t main(int32_t argc , char **argv)
{
	std::cout << "===========================" << std::endl;
	std::cout << "PIM v0.3.2" << std::endl;
	std::cout << "===========================" << std::endl;

	//Instantiate Manager
	PIM::PIMManager cPIMManager(argc,argv);
	cPIMManager.run();

	//
	std::cout<<"Application end" <<std::endl;
}
