/*
 * ADAM_ROController.h (ADAM-6266)
 *
 *  Created on: August 20, 2018
 *      Author: Kian Wee
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "modbus/modbus.h"

namespace DBWPalletCommon
{

class Adam_ROController
{
public:

	// constructor
	Adam_ROController(){};

	// establish modbus connection
	bool connect(const char *ip_address, int port);

	// read back status
	bool read_adam_6260_ro_status(uint8_t status[6]);
	bool read_adam_6266_ro_status(uint8_t status[4]);
	bool read_ro_status(uint8_t status[4]);

	// write status
	bool write_adam6260_ro_status(int channel, uint8_t status);
	bool write_adam6266_ro_status(int channel, uint8_t status);
	bool write_ro_status(int channel, uint8_t status);


private:
	modbus_t *ctx;
};

}
