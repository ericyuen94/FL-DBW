#include "ForkDebugger.h"

ForkDebugger::ForkDebugger()
{
    SetupMiddleWare();
}

ForkDebugger::~ForkDebugger()
{
    sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 2.5);

	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, false);	

}

void ForkDebugger::SetupMiddleWare()
{
    ///////////////////////////
	sptr_ADAM_AOController_Fork_Ctrl = std::make_shared<DBWPalletCommon::Adam_AOController>();
	sptr_ADAM_ROController_Fork_Ctrl = std::make_shared<DBWPalletCommon::Adam_ROController>();
	bool bSuccess_connect_AO_Fork_Ctrl = sptr_ADAM_AOController_Fork_Ctrl->connect("10.0.0.5", 502);
	bool bSuccess_connect_R0_Fork_Ctrl = sptr_ADAM_ROController_Fork_Ctrl->connect("10.0.0.4", 502);
	if (bSuccess_connect_AO_Fork_Ctrl && bSuccess_connect_R0_Fork_Ctrl)
		{ std::cout << "Successfully Connected to both 10.0.0.4 and 10.0.0.5" << std::endl; }
	else
		{ std::cout << "Cant connect" << std::endl; exit(0) ; }

	uint8_t thisstatus[6] = {0};
	sptr_ADAM_ROController_Fork_Ctrl->read_adam_6260_ro_status(thisstatus);
	printf( "[Channel 0] thisstatus RO  = %d\n" , thisstatus[0] );
	printf( "[Channel 1] thisstatus RO  = %d\n" , thisstatus[1] );
	printf( "[Channel 2] thisstatus RO  = %d\n" , thisstatus[2] );
	printf( "[Channel 3] thisstatus RO  = %d\n" , thisstatus[3] );
	printf( "[Channel 4] thisstatus RO  = %d\n" , thisstatus[4] );
	printf( "[Channel 5] thisstatus RO  = %d\n" , thisstatus[5] );

	float ao_value[4] = {0};
	sptr_ADAM_AOController_Fork_Ctrl->read_aovalue(ao_value);
	printf("[Channel 0] AO Voltage = %f\n", ao_value[0]);
	printf("[Channel 1] AO Voltage = %f\n", ao_value[1]);
	printf("[Channel 2] AO Voltage = %f\n", ao_value[2]);
	printf("[Channel 3] AO Voltage = %f\n", ao_value[3]);
	
	//sleep(2);

	//Set Safety Voltage for fork control
	//
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, false);

	sptr_ADAM_ROController_Fork_Ctrl->read_adam_6260_ro_status(thisstatus);
	printf( "[Channel 0] thisstatus RO  = %d\n" , thisstatus[0] );
	printf( "[Channel 1] thisstatus RO  = %d\n" , thisstatus[1] );
	printf( "[Channel 2] thisstatus RO  = %d\n" , thisstatus[2] );
	printf( "[Channel 3] thisstatus RO  = %d\n" , thisstatus[3] );
	printf( "[Channel 4] thisstatus RO  = %d\n" , thisstatus[4] );
	printf( "[Channel 5] thisstatus RO  = %d\n" , thisstatus[5] );

	//sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 2.5);
	//sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 2.5);

	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(1, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(2, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 2.5);

}

void ForkDebugger::run()
{

    while (true)
    {
        std::string name;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Enter ""l"" to simulate LowerFork" << std::endl;
        std::cout << "Enter ""r"" to simulate RaiseFOrk" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::getline(std::cin, name);

        if (!name.length())
        {
            continue;
        }

        if(std::tolower(name.at(0)) == 'l')
        {
            doLowerFork();
        }
        else if(std::tolower(name.at(0)) == 'r')
        {
            doRaiseFork();
        }
        else if(std::tolower(name.at(0)) == 's')
        {
            doStopFork();
        }
	    else if(std::tolower(name.at(0)) == 'o')
        {
            doReadFork();
        }

    }
}

void ForkDebugger::doLowerFork() // lower fork
{
    sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, true);

	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 3.4);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 3.5);
}

void ForkDebugger::doRaiseFork()
{
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, true);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, true);

	//sleep(1);

	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 1.3);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 3.5);
}

void ForkDebugger::doStopFork()
{
    sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(0, 2.5);
	sptr_ADAM_AOController_Fork_Ctrl->write_aovalue(3, 2.5);

	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(1, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(2, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(3, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(4, false);
	sptr_ADAM_ROController_Fork_Ctrl->write_adam6260_ro_status(5, false);
}

void ForkDebugger::doReadFork()
{
	uint8_t thisstatus[6] = {0};
	sptr_ADAM_ROController_Fork_Ctrl->read_adam_6260_ro_status(thisstatus);
	printf( "[Channel 0] thisstatus RO  = %d\n" , thisstatus[0] );
	printf( "[Channel 1] thisstatus RO  = %d\n" , thisstatus[1] );
	printf( "[Channel 2] thisstatus RO  = %d\n" , thisstatus[2] );
	printf( "[Channel 3] thisstatus RO  = %d\n" , thisstatus[3] );
	printf( "[Channel 4] thisstatus RO  = %d\n" , thisstatus[4] );
	printf( "[Channel 5] thisstatus RO  = %d\n" , thisstatus[5] );

	float ao_value[4] = {0};
	sptr_ADAM_AOController_Fork_Ctrl->read_aovalue(ao_value);
	printf("[Channel 0] AO Voltage = %f\n", ao_value[0]);
	printf("[Channel 1] AO Voltage = %f\n", ao_value[1]);
	printf("[Channel 2] AO Voltage = %f\n", ao_value[2]);
	printf("[Channel 3] AO Voltage = %f\n", ao_value[3]);
}
