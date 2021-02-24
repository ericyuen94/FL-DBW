/*
 * DBWManager.h
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */
#ifndef DBWMANAGER_H_
#define DBWMANAGER_H_

#include "Platform/Controller/BaseCSCIManager.h"
//
#include "GetDBWCSCIConfiguration.h"
//
#include "GetDBWEncodersConfiguration.h"
//
#include "BydFLDriveSteerForkThread.h"
//
#include "Platform/PlatformConstants.h"
#include "Platform/Log/LogUtils.h"
//
#include "GetSmartMotorSteerConfiguration.h"
//
#define max_file_size 1000000000 //in bytes - 1gb = 1000000000

namespace DBW
{

class DBWInitializeController;
class DBWProcess;
//
class RetrieveVehMeasure;
class RetreiveBaseCmds;
class RetreiveDBWCmds;
class RetreiveVIMStatus;
class RetrievePLMStatus;
class RetrieveBaseFeedback;
class RetreiveTiltMotorCmds;
//
class BydFLDriveSteerForkThread;

//
class DBWManager: public Platform::Controller::BaseCSCIManager
{
	///\class DBWManager
	///\brief  A class to coordinate the actions for pallet DBW

	// allows DBWController to access to everything in DBWManager
    friend class DBWInitializeController;

public:

	DBWManager(int32_t argc = 0, char **argv = NULL);

	~DBWManager();

protected:

	///\brief  Clean up Specific Resources
	///\note Defines a pure virtual function from BaseCSCIManager
    void setupSpecificCSCIResources();

	///\brief  Clean up Specific Resources
    ///\note   Defines a pure virtual function from BaseCSCIManager
    void cleanupSpecificCSCIResources();

    ///\brief  Default Liveliness Lost Handler
    ///\note Overrides Definition from BaseManager
    void DefaultThreadLivelinessLostFunc(const int32_t & aThreadID);

    void readyFunction();

    //Logging
    void SetupHealthDataLog();
    void UpdateDir();
    void WriteDBWDataLogText(std::string msg);
    void WriteFSMDataLogText(const dbw_liveness_status &health);

private:

    // Used to Control Main Loop Iteration in Intialize State
    std::shared_ptr<DBWInitializeController> sptr_init_controller;
    std::shared_ptr<DBWProcess> sptr_DBWProcess;

    //STKCI
    std::shared_ptr<RetrieveVehMeasure> sptr_RetrieveVehMeasure;
    std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds_TeleOp;
    std::shared_ptr<RetreiveDBWCmds> sptr_RetreiveDBWCmds_VCM;
    std::shared_ptr<RetreiveBaseCmds> sptr_RetreiveBaseCmds;
    std::shared_ptr<RetreiveVIMStatus> sptr_RetreiveVIMStatus;
    std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMStatus;
    std::shared_ptr<RetrievePLMStatus> sptr_RetrievePLMErrorStatus;
    std::shared_ptr<RetreiveTiltMotorCmds> sptr_RetreiveTiltMotorCmds;
    std::shared_ptr<RetrieveBaseFeedback> sptr_RetrieveBaseFeedback;


    //
    std::shared_ptr<BydFLDriveSteerForkThread> sptr_BydFLDriveSteerForkThread;

    //
    DBWCSCI_Config config_params;
    //
    bool read_config_success;

    //
    DBWEncodersConfig config_encoder_params;
    //
    bool read_encoder_config_success;

    //
    DBWPalletCommon::SmartMotorSteerConfig config_brake_params;
    //
    bool read_steermotor_config_success;

    // Logging
    std::string global_log_folder_path_;
    std::string global_filename1_txt;
    std::string global_filename2_txt;

    std::ofstream ptrFileText_logger_health;
    std::ofstream ptrFileText_logger_fsm_data;

    //
    uint32_t plm_warning_status[Platform::PLM_STATUS_TOTAL_BITS];
    uint32_t plm_error_status[Platform::PLM_STATUS_TOTAL_BITS];
};

}
#endif /* ifndef DBWMANAGER_H_ */
