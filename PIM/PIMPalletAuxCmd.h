/**
 * @file PIMPalletAuxCmd.h
 * @date Dec 15, 2017 
 * @author CheeKiat, Tan
 * $Revision: $
 * $Id: $
 * @brief Struture Command to IOXP.
 * @section DESCRIPTION
 * 
 */
 
/*
 *===========
 * Changelog
 *===========
 * No.  Date        Modified By   Remarks
 * ---  ----------  ------------  -------------------
 * 1.   15/12/2017	CheeKiat      Initial Creation
 * 2. 
 *
 */
#ifndef PIMPALLETAUXCMD_H_
#define PIMPALLETAUXCMD_H_

#include <stdint.h>

struct PalletAuxCmdMsg
{
    //SystemMsgsHeader header;//@key
    //unsigned long long timestamp;
    //unsigned short seqKey;
    uint8_t trigger_warning_light;
    uint8_t trigger_left_direction_light;
    uint8_t trigger_right_direction_light;
    uint8_t trigger_buzzer;
};

struct PalletIOXPStatus
{
    int16_t supplyToIOXpUe;  //0.1 V/bit Range: 0.0 - 32.0V
    int16_t supplyBfRelayUb; //0.1 V/bit Range: 0.0 - 32.0V
    int16_t supplyAfRelayUb; //0.1 V/bit Range: 0.0 - 32.0V
    uint8_t ioXpBoardTemp;    //1 degC/bit Range: -40 - +85 degC

    uint8_t warningLightActive; //0 - Disable, 1 - Enable
    int16_t warningLightOutCurrent; //1mA/bit Range: 0 - 4000mA

    uint8_t leftSignalLightActive; //0 - Disable, 1 - Enable
    int16_t leftSignalLightOutCurrent; //1mA/bit Range: 0 - 4000mA

    uint8_t rightSignalLightActive; //0 - Disable, 1 - Enable
    int16_t rightSignalLightOutCurrent; //1mA/bit Range: 0 - 4000mA

    uint8_t eStopStatus; //0 - Disable, 1 - Enable
    uint8_t mannedUnmannedSwStatus; //0 - Manned, 1 - Unmanned
};


#endif /* PIMPALLETAUXCMD_H_ */
