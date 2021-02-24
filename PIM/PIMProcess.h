/*
 * PIMProcess.h
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#ifndef PIMPROCESS_H_
#define PIMPROCESS_H_

#include <cstdint>
#include <memory>
#include <map>
//
#include "Platform/Sensors/SensorsStkciPubData.h"
#include "Platform/PlatformDefinition.h"
#include "PIMConfig.h"
#include "PIMPalletAuxCmd.h"
#include "Platform/IsolatedDIO/IsolatedDIO.h"
#include "Platform/Sensors/SensorsStkciPubData.h"

namespace PIM
{

struct healthstatus_pim_fb
{
	bool bConnectedDIO; // 1
	bool bSuccessReadDIO; //4
	int32_t error_count_ReadDIO;
	bool bFlexisoftStateOK;
	int32_t error_count_FS;

};

struct healthstatus_pim_ies
{
    bool is_fresh_data;
    bool is_ready;
    bool ignoreIES;
};

class PIMProcess
{
    ///\class PIMProcess
    ///\brief A class to process data
public:

    PIMProcess( std::shared_ptr<Platform::IsolatedDIO::IsolatedDIODriver> dev );

    ~PIMProcess();

    void SetConfigParams( const PIMCSCI_Config config_params );

    void operator()();

    void GetPIMProcessStatus(healthstatus_pim_fb &health_pim);

    void GetIesStatus(healthstatus_pim_ies &health_ies);

private:

    void DecodeMsg( uint8_t input );

    std::shared_ptr<Platform::IsolatedDIO::IsolatedDIODriver> sptr_DIO;
    bool bSuccess_initDIO;
    bool bSuccess_readDIO;
    uint8_t input_DIO;

    bool unmanned_switch;
    bool estop_status;
    bool solenoid_status;
    bool piston_status;
    bool left_engagement_sensor;
    bool right_engagement_sensor;
    bool left_straight_collision_sensor;
    bool left_cross_collision_sensor;
    bool right_straight_collision_sensor;
    bool right_cross_collision_sensor;
	bool flexisoft_state;
	uint64_t last_receive_time;

    //stkci pub data
   	std::shared_ptr<Platform::Sensors::SensorsStkciPubData> ptr_stkci_pub;

   	//
   	healthstatus_pim_fb healthstatus_pim;
};

}
#endif /* ifndef PIMPROCESS_H_ */
