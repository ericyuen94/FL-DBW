/*
 * DBWInitializeController.cpp
 *
 *  Created on: Aug 8, 2017
 *      Author: Emily Low
 */

#include "DBWInitializeController.h"
#include "DBWManager.h"
#include "Common/Utility/TreeNodeNavigate.h"
#include <memory>

//
#include "RetrieveVehMeasure.h"
#include "RetreiveDBWCmds.h"
#include "RetreiveBaseCmds.h"
#include "RetreiveVIMStatus.h"
#include "RetrievePLMStatus.h"
#include "RetrieveBaseFeedback.h"
//
#include "BydFLDriveSteerForkThread.h"
#include "DBWProcess.h"

namespace DBW
{

DBWInitializeController::DBWInitializeController(DBWManager* owner):
			Platform::Controller::BaseCSCIInitializeController(owner),
			ptr_owner(owner)
{
	
}

Common::InitializeStageStatus DBWInitializeController::LoadSpecificParameters()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;
   	

	return ret_status;
}

Common::InitializeStageStatus DBWInitializeController::SetupSpecificServices()
{
	Common::InitializeStageStatus ret_status = Common::InitializeStageStatus_Success;

	// ready function
	ptr_owner->setMainLoopPeriod(100);//ms

	//the comms threads
	int32_t thread_id=11;
	ptr_owner->startThread(ptr_owner->sptr_RetrieveVehMeasure,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveDBWCmds_TeleOp,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveDBWCmds_VCM,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveBaseCmds,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveVIMStatus,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetrievePLMStatus,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetrievePLMErrorStatus,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetreiveTiltMotorCmds,thread_id,0,10);
	//
	thread_id++;
	ptr_owner->startThread(ptr_owner->sptr_RetrieveBaseFeedback,thread_id,0,10);

//	//
//	thread_id++;
//	ptr_owner->startThread(ptr_owner->sptr_DBWProcess,ptr_owner->config_params.warning_temperature,0,10);
//
//	//
//	thread_id++;
//	ptr_owner->startThread(ptr_owner->sptr_DBWProcess,ptr_owner->config_params.error_temperature,0,10);

	//the commands and feedbacks threads
	thread_id++;
	ptr_owner->startThread(std::bind(&BydFLDriveSteerForkThread::Thread_MainControl,ptr_owner->sptr_BydFLDriveSteerForkThread.get()),
				thread_id,ptr_owner->config_params.ControlCmdThreads["MainControl"].mThreadPeriodmsec,10);

	thread_id++;
	ptr_owner->startThread(std::bind(&BydFLDriveSteerForkThread::Thread_ValveControl,ptr_owner->sptr_BydFLDriveSteerForkThread.get()),
			thread_id,ptr_owner->config_params.ControlCmdThreads["ValveControl"].mThreadPeriodmsec,10);

	thread_id++;
	ptr_owner->startThread(std::bind(&BydFLDriveSteerForkThread::Thread_TiltMotorControl, ptr_owner->sptr_BydFLDriveSteerForkThread.get()),
			thread_id, ptr_owner->config_params.ControlCmdThreads["TiltMotorControl"].mThreadPeriodmsec,10);

	thread_id++;
	ptr_owner->startThread(std::bind(&BydFLDriveSteerForkThread::Thread_FeedbackDBWandPalletBaseStatus,
			ptr_owner->sptr_BydFLDriveSteerForkThread.get()),
			thread_id,
			ptr_owner->config_params.ControlCmdThreads["PalletBaseCanFeedback"].mThreadPeriodmsec,10);

	thread_id++;
	ptr_owner->startThread(std::bind(&BydFLDriveSteerForkThread::Thread_ReadPCM,
				ptr_owner->sptr_BydFLDriveSteerForkThread.get()),
				thread_id,
				0,10);
	thread_id++;
	ptr_owner->startThread(std::bind(&BydFLDriveSteerForkThread::Thread_DecodeBattery,
				ptr_owner->sptr_BydFLDriveSteerForkThread.get()),
				thread_id,
				50,10);
	thread_id++;
	ptr_owner->startThread(std::bind(&BydFLDriveSteerForkThread::Thread_DecodeSpeed,
				ptr_owner->sptr_BydFLDriveSteerForkThread.get()),
				thread_id,
				50,10);
	thread_id++;
	ptr_owner->startThread(std::bind(&BydFLDriveSteerForkThread::Thread_DecodeSpeedMode,
				ptr_owner->sptr_BydFLDriveSteerForkThread.get()),
				thread_id,
				50,10);
	thread_id++;
	ptr_owner->startThread(std::bind(&BydFLDriveSteerForkThread::TorqueControl_Thread,
				ptr_owner->sptr_BydFLDriveSteerForkThread.get()),
				thread_id,
				50,10);

	//	thread_id++;
	//	ptr_owner->startThread(std::bind(&DriveMotorThread::Thread_CommandBasePallet,
	//			ptr_owner->sptr_DriveMotorThread.get()),
	//			thread_id,
	//			ptr_owner->config_params.ControlCmdThreads["DriveMotor"].mThreadPeriodmsec+10,10);


	return ret_status;
}

DBWInitializeController::~DBWInitializeController()
{

}

}

