/*
 * VcmTrajRead.h
 *
 *  Created on: Dec 20, 2017
 *      Author: emily
 */

#ifndef VcmTrajREAD_H_
#define VcmTrajREAD_H_

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
//
#include <iostream>
#include <unistd.h>
#include <cstdlib>
//
#include "VcmTrajStruct.h"
//

namespace vcmtrajdemo
{

class VcmTrajRead
{

public:

	VcmTrajRead();

	bool SetupVcmTrajRead(const std::string log_path);

	uint32_t ReadDataLogger(vcmtraj_data &datalog); // To read and return the datalog to calling function

	void CloseReadLog(void);  // To close the file

	~VcmTrajRead();

private:

	FILE * mp_File;
};


}//vcmtrajdemo



#endif /* VcmTrajREAD_H_ */
