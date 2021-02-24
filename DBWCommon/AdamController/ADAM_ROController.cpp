/*
 * ADAM_ROController.cpp (ADAM-6266)
 *
 *  Created on: August 20, 2018
 *      Author: Kian Wee
 */

#include "ADAM_ROController.h"
#include "ADAM_Definitions.h"

namespace DBWPalletCommon
{

bool Adam_ROController::connect(const char *ip_address, int port)
{
	bool bSuccess_modbus_connect;

    /* Establish TCP Connection */
    ctx = modbus_new_tcp(ip_address, port);

    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        bSuccess_modbus_connect = false;
    }
    else
    {
    	printf("Connection to ADAM-6266 %s:%d is Successful\n", ip_address, port);
    	bSuccess_modbus_connect = true;
    }

    return bSuccess_modbus_connect;
}

bool Adam_ROController::read_adam_6266_ro_status(uint8_t status[4])
{
	int rc;
	bool bSuccess_read = false;

	// read Relay Output value
	rc = modbus_read_bits(ctx, ADAM6266_D0CHANNEL0_VALUE, 4, status);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_read = false;
	}
	else
	{
		printf("Reading of Relay Output Values Successful\n");
		bSuccess_read = true;
	}


	return bSuccess_read;
}

bool Adam_ROController::read_ro_status(uint8_t status[4])
{
	int rc;
	bool bSuccess_read = false;

	// read Relay Output value
	rc = modbus_read_bits(ctx, ADAM6266_D0CHANNEL0_VALUE, 4, status);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_read = false;
	}
	else
	{
		printf("Reading of Relay Output Values Successful\n");
		bSuccess_read = true;
	}


	return bSuccess_read;
}

bool Adam_ROController::read_adam_6260_ro_status(uint8_t status[6])
{
	int rc;
	bool bSuccess_read = false;

	// read Relay Output value
	rc = modbus_read_bits(ctx, ADAM6260_D0CHANNEL0_VALUE, 6, status);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_read = false;
	}
	else
	{
	//	printf("Reading of Relay Output Values Successful\n");
		bSuccess_read = true;
	}


	return bSuccess_read;
}

bool Adam_ROController::write_ro_status(int channel, uint8_t status)
{
	int address_to_write, rc;
	bool bSuccess_write;

	switch (channel)
	{
		case 0:
			address_to_write = ADAM6266_D0CHANNEL0_VALUE;
			break;
		case 1:
			address_to_write = ADAM6266_D0CHANNEL1_VALUE;
			break;
		case 2:
			address_to_write = ADAM6266_D0CHANNEL2_VALUE;
			break;
		case 3:
			address_to_write = ADAM6266_D0CHANNEL3_VALUE;
			break;
	}

	// write Relay Output value
	rc = modbus_write_bit(ctx, address_to_write, status);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_write = false;
	}
	else
	{
	//	printf("Write of Relay Output Value of %d to Channel %d Successful\n", status, channel);
		bSuccess_write = true;
	}

	return bSuccess_write;
}

bool Adam_ROController::write_adam6266_ro_status(int channel, uint8_t status)
{
	int address_to_write, rc;
	bool bSuccess_write;

	switch (channel)
	{
		case 0:
			address_to_write = ADAM6266_D0CHANNEL0_VALUE;
			break;
		case 1:
			address_to_write = ADAM6266_D0CHANNEL1_VALUE;
			break;
		case 2:
			address_to_write = ADAM6266_D0CHANNEL2_VALUE;
			break;
		case 3:
			address_to_write = ADAM6266_D0CHANNEL3_VALUE;
			break;
	}

	// write Relay Output value
	rc = modbus_write_bit(ctx, address_to_write, status);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_write = false;
	}
	else
	{
		//printf("Write of Relay Output Value of %d to Channel %d Successful\n", status, channel);
		bSuccess_write = true;
	}

	return bSuccess_write;
}

bool Adam_ROController::write_adam6260_ro_status(int channel, uint8_t status)
{
	int address_to_write, rc;
	bool bSuccess_write;

	switch (channel)
	{
		case 0:
			address_to_write = ADAM6260_D0CHANNEL0_VALUE;
			break;
		case 1:
			address_to_write = ADAM6260_D0CHANNEL1_VALUE;
			break;
		case 2:
			address_to_write = ADAM6260_D0CHANNEL2_VALUE;
			break;
		case 3:
			address_to_write = ADAM6260_D0CHANNEL3_VALUE;
			break;
		case 4:
			address_to_write = ADAM6260_D0CHANNEL4_VALUE;
			break;
		case 5:
			address_to_write = ADAM6260_D0CHANNEL5_VALUE;
			break;
	}

	// write Relay Output value
	rc = modbus_write_bit(ctx, address_to_write, status);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_write = false;
	}
	else
	{
		//printf("Write of Relay Output Value of %d to Channel %d Successful\n", status, channel);
		bSuccess_write = true;
	}

	return bSuccess_write;
}


} // namespace DBWPalletCommon
