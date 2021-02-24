/*
 * ADAM_AOController.h (ADAM-6224)
 *
 *  Created on: June 14, 2018
 *      Author: Kian Wee
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "modbus/modbus.h"

namespace DBWPalletCommon
{

class Adam_AOController
{
public:

	// constructor
	Adam_AOController(){};

	// establish modbus connection
	bool connect(const char *ip_address, int port);

	// read back values
	bool read_aovalue(float value[4]);
	bool read_aotypecode(uint16_t value[4]);
	bool read_startupvalue(float value[4]);

	// write values
	bool write_aovalue(int channel, float ao_value);
	bool write_aotypecode(int channel, int ao_typecode);
	bool write_startupvalue(int channel, float startupvalue);


private:
	modbus_t *ctx;
};

}
