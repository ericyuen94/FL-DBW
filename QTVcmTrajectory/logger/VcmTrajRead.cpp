/*
 * VcmTrajRead.cpp
 *
 *  Created on: Dec 20, 2017
 *      Author: emily
 */

#include "VcmTrajRead.h"
#include "Platform/Log/LogUtils.h"

namespace vcmtrajdemo
{

VcmTrajRead::VcmTrajRead()
{

}

bool VcmTrajRead::SetupVcmTrajRead(const std::string log_path)
{
	bool result=false;
	char ac_FileName[300]; // Directory and file name
	(void)sprintf(&ac_FileName[0],"%s/vcmtraj.log",log_path.c_str());
	mp_File = fopen(&ac_FileName[0],"rb");

	std::cout << " " << std::endl;
	if(mp_File == NULL)
	{
		std::cout << "[initReadLog] Error opening file for reading!" << std::endl;
	}
	else
	{
		std::cout << "[initReadLog] Log file opened for reading" << std::endl;
		result = true;
	}

	return result;
}

uint32_t VcmTrajRead::ReadDataLogger(vcmtraj_data &datalog) // To read and return the datalog to calling function
{
	uint32_t u32_FileReturnCount = 1U; // Also serve as return value

	if(mp_File == NULL)
	{
		std::cout << "[readDataLogger] Error log file not open yet! [initReadLog] not called or unsuccessful!" << std::endl;
	}
	else
	{
		const uint32_t u32_readsize = sizeof(datalog);

		u32_FileReturnCount = fread(&datalog,u32_readsize,1U,mp_File);
		//
		if (u32_FileReturnCount == 0U)
		{
			std::cout << "[readDataLogger] EOF!" << std::endl;
		}
		else
		{
			std::cout << "time = " << datalog.timestamp
					  << " speed (m/s) = " << datalog.speed
					  << " steer (deg) = " << datalog.steering*180.0/3.141 << std::endl;
		}

	} // else

	return u32_FileReturnCount;
}

void VcmTrajRead::CloseReadLog(void)  // To close the file
{
	if(mp_File != NULL)
	{
		(void)fclose(mp_File);
		mp_File = NULL;
	}
}

VcmTrajRead::~VcmTrajRead()
{

}

}//vcmtrajdemo
