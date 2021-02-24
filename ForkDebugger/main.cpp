#include <iostream>
#include <cstdint>
#include "ForkDebugger.h"

int32_t main(int32_t argc , char **argv)
{
	std::cout << "===============================" << std::endl;
	std::cout << "  DBW Fork Debugger V1.0      " << std::endl;
	std::cout << "===============================" << std::endl;

	//ros::init(argc, argv, "DBW Debugger Tool");

	ForkDebugger node;

	node.run();

}
