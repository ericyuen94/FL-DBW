/*
 * PIMTxCommsProcess.h
 *
 *  Created on: Dec 1, 2017
 *      Author: Emily Low
 */

#ifndef PIMTXCOMMSPROCESS_H_
#define PIMTXCOMMSPROCESS_H_

#include <memory>

#include "Platform/IsolatedDIO/IsolatedDIO.h"
#include "PIMPalletAuxCmd.h"
#include "RetreivePIMCmd.h"

namespace PIM
{

struct healthstatus_pim_tx
{
	bool bError_RxPIMCmdMsg; //3
	int32_t error_count_RxPIMCmdMsg;
	bool bError_SetDIOCmds; //2
	int32_t error_count_SetDIOCmds;
};

class PIMTxCommsProcess
{

public:

    PIMTxCommsProcess(std::shared_ptr<Platform::IsolatedDIO::IsolatedDIODriver> dev,
    		std::shared_ptr<RetreivePIMCmd> sptr_RetreivePIMCmd);

    void operator()();

    void GetPIMTxCommsProcessHealth(healthstatus_pim_tx &health);

    ~PIMTxCommsProcess();

private:

    void UpdateDIOMsg( const struct PalletAuxCmdMsg setCmd );
    bool SetDIOMsg();

    void NormalSound(PalletAuxCmdMsg &cmd);
    void ErrorSound(PalletAuxCmdMsg &cmd);
    //
    std::shared_ptr<Platform::IsolatedDIO::IsolatedDIODriver> sptr_DIO;
    std::shared_ptr<RetreivePIMCmd> sptr_RetreivePIMCmd_;

    //
    uint8_t temp_output[8];
    uint8_t output_DIO;
    bool heartbeat_high;

    //
    uint64_t startTime;
    healthstatus_pim_tx health_status;

    uint64_t pip_pip;
};

}

#endif /* PIMTXCOMMSPROCESS_H_ */
