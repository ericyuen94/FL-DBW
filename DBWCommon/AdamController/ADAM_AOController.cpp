/*
 * ADAM_AOController.cpp (ADAM-6224)
 *
 *  Created on: June 14, 2018
 *      Author: Kian Wee
 */

#include "ADAM_AOController.h"
#include "ADAM_Definitions.h"

namespace DBWPalletCommon
{

bool Adam_AOController::connect(const char *ip_address, int port)
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
    	printf("Connection to ADAM-6224 %s:%d is Successful\n", ip_address, port);
    	bSuccess_modbus_connect = true;
    }

    return bSuccess_modbus_connect;
}

bool Adam_AOController::read_aovalue(float value[4])
{
	int rc;
	uint16_t aovalue[4] = {0};
	uint16_t typecode[4] = {0};
	bool bSuccess_read;

	// read ao value
	rc = modbus_read_registers(ctx, ADAM6224_AOCHANNEL0_VALUE, 4, aovalue);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_read = false;
	}
	else
	{
		printf("Reading of AO Value Successful\n");
		bSuccess_read = true;
	}

	// read ao typecode
	rc = modbus_read_registers(ctx, ADAM6224_AOCHANNEL0_TYPECODE, 4, typecode);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_read = false;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			if (typecode[i] == 0x0148)	// Range : 0 ~ 10 V
			{
				value[i] = ( aovalue[i] / 4095.0 ) * 10.0;
			}
			else if (typecode[i] == 0x0143) // Range : +/- 10 V
			{
				value[i] = (( aovalue[i] / 4095.0 ) * 20.0 ) - 10.0;
			}
			else if (typecode[i] == 0x0147) // Range : 0 ~ 5 V
			{
				value[i] = ( aovalue[i] / 4095.0 ) * 5.0;
			}
		}
		bSuccess_read = true;
	}

	return bSuccess_read;
}

bool Adam_AOController::read_aotypecode(uint16_t value[4])
{
	int rc;
	bool bSuccess_read;

	// read ao typecode
	rc = modbus_read_registers(ctx, ADAM6224_AOCHANNEL0_TYPECODE, 4, value);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_read = false;
	}
	else
	{
		printf("Reading of AO Type Code Successful\n");
		bSuccess_read = true;
	}

	return bSuccess_read;
}

bool Adam_AOController::read_startupvalue(float value[4])
{
	int rc;
	uint16_t startupvalue[4] = {0};
	uint16_t typecode[4] = {0};
	bool bSuccess_read;

	// read startup value
	rc = modbus_read_registers(ctx, ADAM6224_AOCHANNEL0_STARTUPVALUE, 4, startupvalue);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_read = false;
	}
	else
	{
		printf("Reading of Startup Values Successful\n");
		bSuccess_read = true;
	}

	// read ao typecode
	rc = modbus_read_registers(ctx, ADAM6224_AOCHANNEL0_TYPECODE, 4, typecode);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_read = false;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{//printf("Printing value of %d : %d \n", i , startupvalue[i]);
		//printf("Printing type of %d : %x \n", i , typecode[i]);
			if (typecode[i] == 0x0148)	// Range : 0 ~ 10 V
			{
				value[i] = ( startupvalue[i] / 4095.0 ) * 10.0;
			}
			else if (typecode[i] == 0x0143) // Range : +/- 10 V
			{	//printf("Printing value of %d : %d", i , startupvalue[i]);
				value[i] = (( startupvalue[i] / 4095.0 ) * 20.0 ) - 10.0;
			}
			else if (typecode[i] == 0x0147)
			{
				value[i] = startupvalue[i] / 4095.0 * 5.0;
			}
		}
		bSuccess_read = true;
	}

	return bSuccess_read;
}

bool Adam_AOController::write_aovalue(int channel, float value)
{
	int address_to_write, typecode_address, rc, raw_data;
	uint16_t typecode[4];
	bool bSuccess_write;

	switch (channel)
	{
		case 0:
			address_to_write = ADAM6224_AOCHANNEL0_VALUE;
			typecode_address = ADAM6224_AOCHANNEL0_TYPECODE;
			break;
		case 1:
			address_to_write = ADAM6224_AOCHANNEL1_VALUE;
			typecode_address = ADAM6224_AOCHANNEL1_TYPECODE;
			break;
		case 2:
			address_to_write = ADAM6224_AOCHANNEL2_VALUE;
			typecode_address = ADAM6224_AOCHANNEL2_TYPECODE;
			break;
		case 3:
			address_to_write = ADAM6224_AOCHANNEL3_VALUE;
			typecode_address = ADAM6224_AOCHANNEL3_TYPECODE;
			break;
	}

	// read channel type code
	modbus_read_registers(ctx, typecode_address, 1, typecode);
	if (typecode[0] == 0x0148)	// Range : 0 ~ 10 V
	{
		raw_data = ( value / 10.0 ) * 4095.0;
	}
	else if (typecode[0] == 0x0143) // Range : +/- 10 V
	{
		raw_data = ( (value + 10.0) / 20.0) * 4095.0;
	}
	else if (typecode[0] == 0x0147) // Range : 0 ~ 5 V
	{
		raw_data =  ( value / 5.0 ) * 4095.0;
	}

	// write ao values
	rc = modbus_write_register(ctx, address_to_write, raw_data);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_write = false;
	}
	else
	{
		//printf("Write of AO Value of %f to Channel %d Successful\n", value, channel);
		bSuccess_write = true;
	}

	return bSuccess_write;
}

bool Adam_AOController::write_aotypecode(int channel, int typecode)
{
	int address_to_write, rc;
	bool bSuccess_write;

	switch (channel)
	{
		case 0:
			address_to_write = ADAM6224_AOCHANNEL0_TYPECODE;
			break;
		case 1:
			address_to_write = ADAM6224_AOCHANNEL1_TYPECODE;
			break;
		case 2:
			address_to_write = ADAM6224_AOCHANNEL2_TYPECODE;
			break;
		case 3:
			address_to_write = ADAM6224_AOCHANNEL3_TYPECODE;
			break;
	}

	// write ao typecode
	rc = modbus_write_register(ctx, address_to_write, typecode);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_write = false;
	}
	else
	{
		bSuccess_write = true;
		printf("Write of AO Type Code of %d to Channel %d Successful\n", typecode, channel);
	}

	return bSuccess_write;
}

bool Adam_AOController::write_startupvalue(int channel, float startupvalue)
{
	int address_to_write, typecode_address, raw_data, rc;
	uint16_t typecode[4] = {0};
	bool bSuccess_write;

	switch (channel)
	{
		case 0:
			address_to_write = ADAM6224_AOCHANNEL0_STARTUPVALUE;
			typecode_address = ADAM6224_AOCHANNEL0_TYPECODE;
			break;
		case 1:
			address_to_write = ADAM6224_AOCHANNEL1_STARTUPVALUE;
			typecode_address = ADAM6224_AOCHANNEL1_TYPECODE;
			break;
		case 2:
			address_to_write = ADAM6224_AOCHANNEL2_STARTUPVALUE;
			typecode_address = ADAM6224_AOCHANNEL2_TYPECODE;
			break;
		case 3:
			address_to_write = ADAM6224_AOCHANNEL3_STARTUPVALUE;
			typecode_address = ADAM6224_AOCHANNEL3_TYPECODE;
			break;
	}

	// read channel type code
	modbus_read_registers(ctx, typecode_address, 1, typecode);
	if (typecode[0] == 0x0148)	// Range : 0 ~ 10 V
	{
		raw_data = ( startupvalue / 10.0 ) * 4095.0;
	}
	else if (typecode[0] == 0x0143) // Range : +/- 10 V
	{
		raw_data = ( (startupvalue + 10.0) / 20.0) * 4095.0;
	}
	else if (typecode[0] == 0x0147) // Range : +/- 10 V
	{
		raw_data = ( startupvalue / 5.0 ) * 4095.0;
	}

	// write startup value
	rc = modbus_write_register(ctx, address_to_write, raw_data);
	if (rc == -1)
	{
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		modbus_free(ctx);
		bSuccess_write = false;
	}
	else
	{
		bSuccess_write = true;
		printf("Write of Startup Value of %f to Channel %d Successful\n", startupvalue, channel);
	}

	return bSuccess_write;
}

} // namespace DBWPalletCommon
