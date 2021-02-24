#include <PCMPalletController.h>
//
#include <cstdint>
#include <cmath>
#include <memory>
#include <sstream>
#include <iostream>
#include <string>

namespace DBWPalletCommon
{

	PCMPalletController::PCMPalletController()
	{
		count_tiller_heartbeat = 0;
		logging_flag_status = false;
		heartbeat_forward_motion = true;
		m_steer_enc_pos = 0;
		m_line_enc_pos = 0;
		m_tilt_enc_pos = 0;

		//KW_Edit
		alternative_byte = false;
		previous_timestamp = 0;

		// reset pcm health status
		health_status_fb.open_emuc_can_port = false;
		health_status_fb.rx_line_encoder_fb = false;
		health_status_fb.rx_steer_encoder_fb = false;
		health_status_fb.error_rx_line_encoder_fb = 0;
		health_status_fb.error_rx_steer_encoder_fb = 0;
		
		health_status_fb.speed_mode = SPEED_MODE::INVALID;
		health_status_fb.speed_mode_readtime = Common::Utility::Time::getusCountSinceEpoch();
		health_status_fb.speed_mode_decodetime = Common::Utility::Time::getusCountSinceEpoch();

		health_status_fb.battery_readtime	 = Common::Utility::Time::getusCountSinceEpoch();
		health_status_fb.battery_decodetime  = Common::Utility::Time::getusCountSinceEpoch();

		health_status_fb.speed_readtime		 = Common::Utility::Time::getusCountSinceEpoch();
		health_status_fb.speed_decodetime	 = Common::Utility::Time::getusCountSinceEpoch();

		LastBYDBatteryData.exist 		= false;
		LastBYDBatteryData.timeReceive	= Common::Utility::Time::getusCountSinceEpoch();
		LastBYDSpeedData.exist			= false;
		LastBYDSpeedData.timeReceive	= Common::Utility::Time::getusCountSinceEpoch();
		LastBYDSpeedModeData.exist 		= false;
		LastBYDSpeedModeData.timeReceive= Common::Utility::Time::getusCountSinceEpoch();

		//
		bTargetReached = false;
		bTargetReachedFinal = false;

		//init pub for tilt motor
		//Set middleware publisher
		ptr_stkci_pub = std::make_shared<Platform::Sensors::SensorsStkciPubData>();
		std::vector<std::string> interface_list;
		interface_list.push_back("TiltMotorFbkMsg");
		ptr_stkci_pub->SetUpMiddleWareInterfaces(interface_list);
	}

	//Pallet Truck Two Add on
	bool PCMPalletController::OpenVecowCanPortModule()
	{
		//init variables
		bSuccess_OpenPCANPort = false;

		//
		sptr_pcanemuc = std::make_shared<Platform::PCanEmuc::PCanEmucDriver>();
		bool bSuccess = false;

		for(uint i = 0; i<100; i++)
		{
			bSuccess = sptr_pcanemuc->initEmucPCAN();
			if (bSuccess == true)
			{
				SetupSteerSikoEncoder();
				SetupLineBaumerEncoder();
				bSuccess_OpenPCANPort = true;
				std::cout << "Success open Vecow PCAN Port " << std::endl;
				health_status_fb.open_emuc_can_port = true;
				break;
			}
			else
			{
				usleep(1000000);
			}

		}
//		bSuccess = sptr_pcanemuc->initEmucPCAN();
//		if (bSuccess == true)
//		{
//			SetupSteerSikoEncoder();
//			SetupLineBaumerEncoder();
//			bSuccess_OpenPCANPort = true;
//			std::cout << "Success open Vecow PCAN Port " << std::endl;
//			health_status_fb.open_emuc_can_port = true;
//		}
//		else
		if(bSuccess == false)
		{
			std::cout << "Fail to open Vecow PCAN Port " << std::endl;
			health_status_fb.open_emuc_can_port = false;
			usleep(1000000);
		}

		//
		return bSuccess_OpenPCANPort;
	}

	bool PCMPalletController::OpenCanPort(const std::string port_name)
	{
		//init variables
		bSuccess_OpenPCANPort = false;

		//KW_Edit - Open PcanEmuc Port
		sptr_pcanemuc = std::make_shared<Platform::PCanEmuc::PCanEmucDriver>();
		bool bSuccess = false;
		bSuccess = sptr_pcanemuc->initEmucPCAN();
		if (bSuccess == true)
		{
			SetupSteerSikoEncoder();
			bSuccess_OpenPCANPort = true;
			std::cout << "Success open pcan port " << port_name << std::endl;
		}
		else
		{
			std::cout << "Cannot open pcan port " << port_name << std::endl;
		}

		return bSuccess_OpenPCANPort;
	}

	void PCMPalletController::SetLogger(std::string logfolder_path)
	{
		sptr_PCanLogger = std::make_shared<Platform::PCanUsb::PCanLogger>();
		sptr_PCanLogger->SetupPCanLogger(logfolder_path);
		logging_flag_status = true;
	}

	void PCMPalletController::GetBYDCANFeedback(pcm_palletbase_feedback &pcm_feedback)
	{
		pcm_feedback = out_pcm_feedback;
	}

	bool PCMPalletController::GetPCMPalletFeedback(PCMHealthStatusFeedback &out_health_status_fb)
	{
		bool success = false;
		bool bRead_steerenc = false;
		bool bRead_lineenc = false;
		bool bRead_tiltmotorenc = false;
		bool bRead_tiltenc = false;

		//receive via PCAN
		TPCANRdMsg CAN_RxMsgPort1;
		bool bReadSuccessEncoderFb;

		if (bSuccess_OpenPCANPort)
		{
			//KW_Edit - retreive pcane emuc messages
			bReadSuccessEncoderFb = sptr_pcanemuc->RxEmucCANMsg(&CAN_RxMsgPort1, 1);
			//bReadSuccess = sptr_pcanemuc->RxEmucCANMsg(&CAN_RxMsg, 1);

			if (bReadSuccessEncoderFb)
			{
				if (logging_flag_status)
				{
					sptr_PCanLogger->WriteLog(Common::Utility::Time::getusCountSinceEpoch(), CAN_RxMsgPort1);
				}

				//decode for tilt motor encoder
				bRead_tiltmotorenc = DecodeTiltMotorEncMsg(&CAN_RxMsgPort1, static_cast<int64_t>(Common::Utility::Time::getmsCountSinceEpoch()));

				// decode for siko steer Encoder
				bRead_steerenc = DecodeSikoEncMsg(&CAN_RxMsgPort1, static_cast<int64_t>(Common::Utility::Time::getmsCountSinceEpoch()), m_steer_enc_pos);
				if (bRead_steerenc)
				{
					health_status_fb.rx_steer_encoder_fb = true;
					health_status_fb.error_rx_steer_encoder_fb = 0;
				}
				else
				{
					health_status_fb.error_rx_steer_encoder_fb++;
				}

				// decode for baumer line Encoder
				bRead_lineenc = DecodeBaumerLineEncMsg(&CAN_RxMsgPort1, static_cast<int64_t>(Common::Utility::Time::getmsCountSinceEpoch()), m_line_enc_pos);
				if (bRead_lineenc)
				{
					health_status_fb.rx_line_encoder_fb = true;
					health_status_fb.error_rx_line_encoder_fb = 0;
				}
				else
				{
					health_status_fb.error_rx_line_encoder_fb++;
				}
				std::cout<<"health_status_fb.error_rx_line_encoder_fb = "<<health_status_fb.error_rx_line_encoder_fb<<std::endl;

				//decode for baumer tilt Encoder
				bRead_tiltenc = DecodeBaumerTiltEncMsg(&CAN_RxMsgPort1, static_cast<int64_t>(Common::Utility::Time::getmsCountSinceEpoch()), m_tilt_enc_pos);
				if (bRead_tiltenc)
				{
					health_status_fb.rx_tilt_encoder_fb = true;
					health_status_fb.error_rx_tilt_encoder_fb = 0;
				}
				else
				{
					health_status_fb.error_rx_tilt_encoder_fb++;
				}
				std::cout<<"health_status_fb.error_rx_tilt_encoder_fb = "<<health_status_fb.error_rx_tilt_encoder_fb<<std::endl;

			}
		}
		//
		int32_t error_count_threshold = 30;
		if (health_status_fb.error_rx_steer_encoder_fb > error_count_threshold)
		{
			health_status_fb.rx_steer_encoder_fb = false;
		}
		if (health_status_fb.error_rx_line_encoder_fb > error_count_threshold)
		{
			health_status_fb.rx_line_encoder_fb = false;
		}
		if (health_status_fb.error_rx_tilt_encoder_fb > error_count_threshold)
		{
			health_status_fb.rx_tilt_encoder_fb = false;
		}

		out_health_status_fb = health_status_fb;

		//
		return success;
	}

	void PCMPalletController::ReadPCM()
	{
		TPCANRdMsg CAN_RxMsgPort0;
		bool bRead = sptr_pcanemuc->RxEmucCANMsg(&CAN_RxMsgPort0, 0);

		if (bRead)
		{
			if( CAN_RxMsgPort0.Msg.ID == 0x384 ) 
			{
				//PUSH DATA TO SPEED MODE CAN CONTAINER
				std::unique_lock<std::mutex> safelock(mutex_BYDSpeedMode);
				LastBYDSpeedModeData.data 			= CAN_RxMsgPort0;
				LastBYDSpeedModeData.exist 			= true;
				LastBYDSpeedModeData.timeReceive	= Common::Utility::Time::getusCountSinceEpoch();
				safelock.unlock();
			}
			else if( CAN_RxMsgPort0.Msg.ID == 0x388 )
			{
				//PUSH DATA TO SPEED CAN CONTAINER
				std::unique_lock<std::mutex> safelock(mutex_BYDSpeed);
				LastBYDSpeedData.data 			= CAN_RxMsgPort0;
				LastBYDSpeedData.exist 			= true;
				LastBYDSpeedData.timeReceive	= Common::Utility::Time::getusCountSinceEpoch();
				safelock.unlock();	
			}
			else if( CAN_RxMsgPort0.Msg.ID == 0x444 )
			{
				//PUSH DATA TO BATTERY CAN CONTAINER
				std::unique_lock<std::mutex> safelock(mutex_BYDSpeedMode);
				LastBYDBatteryData.data 		= CAN_RxMsgPort0;
				LastBYDBatteryData.exist 		= true;
				LastBYDBatteryData.timeReceive	= Common::Utility::Time::getusCountSinceEpoch();
				safelock.unlock();	
			}
		}
	}

	bool PCMPalletController::SetPCMPalletCommands(const pcm_palletbase_commands &in_pcm_command)
	{
		bool success = false;

		//transmit via PCAN
		TPCANMsg CAN_TxMsg;
		bool b_SendTimeout;

		//
		std::cout << "SetPCMPalletCommands = " << in_pcm_command.analog_speed << std::endl;
		EncodePCMMsg(&CAN_TxMsg, in_pcm_command);
		sptr_pcan->TXCANMsg(&CAN_TxMsg, &b_SendTimeout);

		if (!b_SendTimeout)
		{
			success = true;
		}
		else
		{
			std::cout << "[ERROR] PCAN Send error !!!! " << std::endl;
		}

		//
		return success;
	}

	void PCMPalletController::DisplayPCM_CANData()
	{
		printf("[Display->RXCANMsg] ID:%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", display_pcm_canid,
			   display_rx_pcm_canmsg[0], display_rx_pcm_canmsg[1],
			   display_rx_pcm_canmsg[2], display_rx_pcm_canmsg[3],
			   display_rx_pcm_canmsg[4], display_rx_pcm_canmsg[5],
			   display_rx_pcm_canmsg[6], display_rx_pcm_canmsg[7]);

		//
		printf("Display PCM Command Out Byte = %x \n", display_send_cmd_byte);
		printf("Display PCM Speed Out Bytes = %x %x \n", display_send_speed_byte[1], display_send_speed_byte[0]);
	}

	bool PCMPalletController::GetSikoSteerEncoderFeedback(uint32_t &steer_enc_pos)
	{
		TPCANRdMsg CAN_RxMsg;
		bool b_ReadTimeout;
		bool result = false;

//		if (m_steer_enc_pos != 0)
//		{
//			steer_enc_pos = m_steer_enc_pos;
//			result = true;
//			//std::cout << "[Rx->Steer Enc Position] " << steer_enc_pos << std::endl;
//		}
//		else
//		{
//			//std::cout << "polling for siko steer encoder position .." << std::endl;
//		}

		if(health_status_fb.rx_steer_encoder_fb == true)
		{
			steer_enc_pos = m_steer_enc_pos;
			result = true;
		}
		else
		{

		}

		//
		return result;
	}

	bool PCMPalletController::GetBaumerLineEncoderFeedback(uint32_t &line_enc_pos)
	{
		TPCANRdMsg CAN_RxMsg;
		bool b_ReadTimeout;
		bool result = false;

//		if (m_line_enc_pos != 0)
//		{
//			line_enc_pos = m_line_enc_pos;
//			result = true;
//			//std::cout << "[Rx->Line Enc Position] " << line_enc_pos << std::endl;
//		}
//		else
//		{
//			//std::cout << "polling for siko line encoder position .." << std::endl;
//		}

		if(health_status_fb.error_rx_line_encoder_fb == true)
		{
			line_enc_pos = m_line_enc_pos;
			result = true;
		}
		else
		{

		}

		//
		return result;
	}

	bool PCMPalletController::GetBaumerTiltEncoderFeedback(uint32_t &tilt_enc_pos)
	{
		TPCANRdMsg CAN_RxMsg;
		bool b_ReadTimeout;
		bool result = false;

//		if (m_tilt_enc_pos != 0)
//		{/
//			tilt_enc_pos = m_tilt_enc_pos;
//			result = true;
//		}

		if(health_status_fb.error_rx_tilt_encoder_fb == true)
		{
			tilt_enc_pos = m_tilt_enc_pos;
			result = true;
		}
		else
		{

		}
		return result;
	}

	void PCMPalletController::ConvertSikoSteerCounttoAngle(uint32_t steer_enc_home_pos, uint32_t steer_enc_pos, float64_t &steer_angle_radian)
	{
		float64_t feedback_seiko_steer_angle;
		int32_t delta_enc_pos;
		//
		delta_enc_pos = steer_enc_home_pos - steer_enc_pos;
		feedback_seiko_steer_angle = delta_enc_pos / 41.9181;
		//
		steer_angle_radian = feedback_seiko_steer_angle * 3.141 / 180.0;
	}

	bool PCMPalletController::DecodePalletSpeedFbkMsg()
	{
		bool bSFlag = false;
		float32_t decoded_speed = 0.0;

		std::unique_lock<std::mutex> safelock(mutex_BYDSpeed);
		CANData localData = LastBYDSpeedData;
		safelock.unlock();

		uint64_t deltaTime = Common::Utility::Time::getusecCountSinceEpoch() - localData.timeReceive;
		if(localData.exist && localData.data.Msg.ID == 0x388 && deltaTime < 200000)
		{
			display_speed_canid = localData.data.Msg.ID;
			for (int i = 0; i < 8; i++)
			{
				display_rx_speed_canmsg[i] = localData.data.Msg.DATA[i];
			}

			//hex to integer conversion
			uint8_t buffer[2] = {0, 0};
			buffer[0] = localData.data.Msg.DATA[3];
			buffer[1] = localData.data.Msg.DATA[2];
			int16_t _nValue = 0;
			ConvertHexToInteger(buffer, _nValue);
			_nValue = (_nValue < 0) ? (_nValue * -1) : _nValue;
			decoded_speed = (_nValue * 0.001113);
			//std::cout << "[Current Speed is " << decoded_speed << " ]" << std::endl;

			//set output.
			health_status_fb.speed_readtime			= localData.timeReceive;
			health_status_fb.speed_decodetime		= Common::Utility::Time::getusecCountSinceEpoch();
			out_pcm_feedback.current_platform_speed = decoded_speed;
			bSFlag = true;
		}

		return bSFlag;
	}

	//bool PCMPalletController::DecodePalletSpeedFbkMsg(TPCANRdMsg * const pMsgBuff, const int64_t time,
	//		float32_t &current_speed)
	//{
	//	//
	//	bool bSFlag=false;
	//	float32_t freq;
	//	float32_t decoded_speed=0.0;
	//
	//	//
	//	if (pMsgBuff->Msg.ID == 0x388) // 0x388
	//	{
	//		freq = 1/((time-last_timestamp_rx_speed_msg)/1000000.0);
	//		last_timestamp_rx_speed_msg = time;
	//
	//		//print out
	////		printf("[!!! Rx->Speed Feedback] ID:%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x  Hz = %0.3f ||  ",
	////				pMsgBuff->Msg.ID,
	////				pMsgBuff->Msg.DATA[0],pMsgBuff->Msg.DATA[1],
	////				pMsgBuff->Msg.DATA[2],pMsgBuff->Msg.DATA[3],
	////				pMsgBuff->Msg.DATA[4],pMsgBuff->Msg.DATA[5],
	////				pMsgBuff->Msg.DATA[6],pMsgBuff->Msg.DATA[7],
	////				freq);
	//
	//		//for display purpose
	//		display_speed_canid = pMsgBuff->Msg.ID;
	//		for(int i=0; i<8; i++)
	//		{
	//			display_rx_speed_canmsg[i] = pMsgBuff->Msg.DATA[i];
	//		}
	//
	//		//tiller or move indicator
	//		if(pMsgBuff->Msg.DATA[0] == 0x24 || pMsgBuff->Msg.DATA[0] == 0x04)
	//		{
	//			//hex to integer conversion
	//			uint8_t buffer[4];
	//			buffer[0] = pMsgBuff->Msg.DATA[3]&0xFF;
	//			buffer[1] = pMsgBuff->Msg.DATA[2]&0xFF;
	//			buffer[2] = 0x00;
	//			buffer[3] = 0x00;
	//			uint32_t _nValue;
	//			ConvertHexToUInteger(buffer,_nValue);
	//
	//			//
	//			if(pMsgBuff->Msg.DATA[2] == 0xff)
	//			{
	//				decoded_speed = ComputeBackwardSpeed(_nValue);
	//				//std::cout << "Tiller on and move rear val = " << _nValue << " decoded_speed = " << decoded_speed << std::endl;
	//			}
	//			else
	//			{
	//				decoded_speed = ComputeForwardSpeed(_nValue);
	//				//std::cout << "Tiller on and move forward val = " << _nValue << " decoded_speed = " << decoded_speed << std::endl;
	//			}
	//		}//if.
	//		else if(pMsgBuff->Msg.DATA[0] == 0x20)
	//		{
	//			decoded_speed = 0.0;
	//			//std::cout << "Tiller on and move stop " << " decoded_speed = " << decoded_speed << std::endl;
	//		}
	////		else
	////		{
	////			std::cout << " " << std::endl;
	////		}
	//
	//		//
	//		bSFlag = true;
	//
	//		//set output.
	//		current_speed = decoded_speed;
	//	}
	//
	//	//
	//	return bSFlag;
	//}

	float32_t PCMPalletController::ComputeBackwardSpeed(uint32_t data_value)
	{
		float32_t current_bkward_speed;
		float32_t delta_change;
		float32_t er_per_sec;
		float32_t wr_per_sec;
		float32_t mm_per_sec;
		float32_t m_per_min;

		//
		delta_change = 65535 - data_value;
		er_per_sec = delta_change / 175.0;
		wr_per_sec = er_per_sec / 30.0;
		mm_per_sec = wr_per_sec * 723.0;
		m_per_min = mm_per_sec * 60 / 1000.0;
		if (fabs(m_per_min) > 0.00001)
		{
			current_bkward_speed = 2.0 / m_per_min;
		}
		else
		{
			current_bkward_speed = 0.0;
		}

		return current_bkward_speed;
	}

	float32_t PCMPalletController::ComputeForwardSpeed(uint32_t data_value)
	{
		float32_t current_fwd_speed;
		float32_t er_per_sec;
		float32_t wr_per_sec;
		float32_t mm_sec;
		float32_t m_min;

		//
		er_per_sec = data_value / 185.0;
		wr_per_sec = er_per_sec / 30.0;
		mm_sec = wr_per_sec * 723.0;
		m_min = (mm_sec * 60) / 1000.0;
		if (fabs(m_min) > 0.00001)
		{
			current_fwd_speed = 2.0 / m_min;
		}
		else
		{
			current_fwd_speed = 0.0;
		}

		return current_fwd_speed;
	}

	bool PCMPalletController::DecodePalletBatteryFbkMsg()
	{
		bool bSFlag = false;

		std::unique_lock<std::mutex> safelock(mutex_BYDBattery);
		CANData localData = LastBYDBatteryData;
		safelock.unlock();

		uint64_t deltaTime = Common::Utility::Time::getusecCountSinceEpoch() - localData.timeReceive;
		if(localData.exist && localData.data.Msg.ID == 0x444 && deltaTime < 200000)
		{
			display_battery_canid = localData.data.Msg.ID;
			for (int i = 0; i < 8; i++)
			{
				display_rx_battery_canmsg[i] = localData.data.Msg.DATA[i];
			}
			
			//hex to integer conversion
			uint8_t buffer[4] = {0,0,0,0};
			buffer[0] = localData.data.Msg.DATA[5];
			buffer[1] = 0x00;
			buffer[2] = 0x00;
			buffer[3] = 0x00;
			uint32_t _nValue = 0;
			ConvertHexToUInteger(buffer, _nValue);

			//set output.
			health_status_fb.battery_readtime 		   		= localData.timeReceive;
			health_status_fb.battery_decodetime				= Common::Utility::Time::getusecCountSinceEpoch();
			out_pcm_feedback.current_platform_batterylevel 	= _nValue;
			//std::cout << "[Current Battery is " << out_pcm_feedback.current_platform_batterylevel << " ]" << std::endl;

			bSFlag = true;
		}
		return bSFlag;
	}

	bool PCMPalletController::DecodeModeMsg()
	{
		bool bSFlag = false;
		
		std::unique_lock<std::mutex> safelock(mutex_BYDSpeedMode);
		CANData localData = LastBYDSpeedModeData;
		safelock.unlock();

		uint64_t deltaTime = Common::Utility::Time::getusecCountSinceEpoch() - localData.timeReceive;
		if(localData.exist && localData.data.Msg.ID == 0x384 && deltaTime < 200000)
		{
			if ( localData.data.Msg.DATA[1] == 0x0C )
			{
				health_status_fb.speed_mode 			= SPEED_MODE::TURTLE;
			}
			else if ( localData.data.Msg.DATA[1] == 0x04 )
			{
				health_status_fb.speed_mode 			= SPEED_MODE::RABBIT;
			}
			else
			{
				health_status_fb.speed_mode 			= SPEED_MODE::INVALID;
			}
			//std::cout << "[Current Mode is " << health_status_fb.speed_mode << " ]" << std::endl;
			health_status_fb.speed_mode_readtime 		= localData.timeReceive;
			health_status_fb.speed_mode_decodetime		= Common::Utility::Time::getusecCountSinceEpoch();
			bSFlag = true;
		}
		return bSFlag;
	}

	bool PCMPalletController::DecodePCMMsg(TPCANRdMsg *const pMsgBuff, const int64_t time,
										   pcm_palletbase_feedback &data_feedback)
	{
		TPCANRdMsg CANmsg;
		(void)memcpy(&CANmsg, pMsgBuff, mu32_TPCANRdMsgSize);

		bool bSFlag = false;
		if (CANmsg.Msg.ID == 0x2e0) // 0x2e0
		{
			//		printf("[Rx->PCM RXCANMsg] ID:%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", CANmsg.Msg.ID,
			//				CANmsg.Msg.DATA[0],CANmsg.Msg.DATA[1],
			//				CANmsg.Msg.DATA[2],CANmsg.Msg.DATA[3],
			//				CANmsg.Msg.DATA[4],CANmsg.Msg.DATA[5],
			//				CANmsg.Msg.DATA[6],CANmsg.Msg.DATA[7]);

			//for display purpose
			display_pcm_canid = CANmsg.Msg.ID;
			for (int i = 0; i < 8; i++)
			{
				display_rx_pcm_canmsg[i] = CANmsg.Msg.DATA[i];
			}

			//set the feedback accordingly
			//byte1
			if (CANmsg.Msg.DATA[0] & 0x02)
			{
				//std::cout << "pallet_detection_sensor_left = OK " << std::endl;
				data_feedback.pallet_detection_sensor_left = true;
			}
			else
			{
				//std::cout << "pallet_detection_sensor_left = NOK " << std::endl;
				data_feedback.pallet_detection_sensor_left = false;
			}
			//
			if (CANmsg.Msg.DATA[0] & 0x01)
			{
				//std::cout << "pallet_detection_sensor_right = OK " << std::endl;
				data_feedback.pallet_detection_sensor_right = true;
			}
			else
			{
				//std::cout << "pallet_detection_sensor_right = NOK " << std::endl;
				data_feedback.pallet_detection_sensor_right = false;
			}
			//
			if (CANmsg.Msg.DATA[0] & 0x04)
				data_feedback.pallet_height_sensor_top = true;
			else
				data_feedback.pallet_height_sensor_top = false;
			//
			if (CANmsg.Msg.DATA[0] & 0x08)
				data_feedback.pallet_height_sensor_bottom = true;
			else
				data_feedback.pallet_height_sensor_bottom = false;
			//
			if (CANmsg.Msg.DATA[0] & 0x20)
				data_feedback.collision_detection_sensor_left = true;
			else
				data_feedback.collision_detection_sensor_left = false;
			//
			if (CANmsg.Msg.DATA[0] & 0x10)
				data_feedback.collision_detection_sensor_right = true;
			else
				data_feedback.collision_detection_sensor_right = false;
			//
			//
			if (CANmsg.Msg.DATA[2] & 0x01)
				data_feedback.pcm_controller_error_status = true;
			else
				data_feedback.pcm_controller_error_status = false;
			//
			if (CANmsg.Msg.DATA[2] & 0x02)
				data_feedback.cmd_fork_control_error_status = true;
			else
				data_feedback.cmd_fork_control_error_status = false;
			//
			if (CANmsg.Msg.DATA[2] & 0x04)
				data_feedback.cmd_speed_value_error_status = true;
			else
				data_feedback.cmd_speed_value_error_status = false;
			//
			if (CANmsg.Msg.DATA[2] & 0x10)
				data_feedback.CAN_communication_error_status = true;
			else
				data_feedback.CAN_communication_error_status = false;

			//
			bSFlag = true;
		}

		return bSFlag;
	}

	void PCMPalletController::EncodePCMMsg(TPCANMsg *const pMsgBuff,
										   const pcm_palletbase_commands &data_commands)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x01D0;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}

		//encode out can data
		//byte 1
		if (data_commands.move_forward)
			CANData.DATA[0] |= 0x01;
		if (data_commands.move_backward)
			CANData.DATA[0] |= 0x02;
		if (data_commands.fork_lowering)
			CANData.DATA[0] |= 0x04;
		if (data_commands.fork_lifting)
			CANData.DATA[0] |= 0x08;
		if (data_commands.belly)
			CANData.DATA[0] |= 0x10;
		if (data_commands.slow_speed)
			CANData.DATA[0] |= 0x20;
		if (data_commands.tiller)
			CANData.DATA[0] |= 0x40;
		if (data_commands.trigger_buzzer)
			CANData.DATA[0] |= 0x80;

		//
		display_send_cmd_byte = CANData.DATA[0];
		//
		uint8_t out_hex[4];
		ConvertUIntegerToHex(data_commands.analog_speed, out_hex);
		CANData.DATA[1] = out_hex[0] & 0xFF;
		CANData.DATA[2] = out_hex[1] & 0x0F;
		display_send_speed_byte[0] = CANData.DATA[1];
		display_send_speed_byte[1] = CANData.DATA[2];

		printf("PCM Command Out Byte = %x ", CANData.DATA[0]);
		printf("PCM Speed Out Bytes = %x %x speed = %d \n\n", CANData.DATA[2], CANData.DATA[1], data_commands.analog_speed);
		//	for(size_t i=0; i<4; i++)
		//	{
		//		printf("%x | ",out_hex[i]);
		//	}
		//	printf("\n");

		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	//return set direction of change command to PCM cmd
	bool PCMPalletController::PCMSpeedCtrlLogic(float64_t current_speed)
	{
		//to determine is opposite speed direction from last command.
		float resultant_sign = 0.0;
		int change_of_cmd_to_direction = 0;

		//	std::cout << "last_cmd_speed = " << last_cmd_speed
		//			<< " cmd_speed = " << current_speed
		//			<< std::endl;

		//detect change in speed forward or reverse.
		if (fabs(last_cmd_speed) > 0.0)
		{
			resultant_sign = current_speed * last_cmd_speed;
			//std::cout << "fabs(last_cmd_speed) resultant_sign = " << resultant_sign << std::endl;
			if (resultant_sign < 0) //change of direction
			{
				//std::cout << "!!!! direction resultant_sign = " << resultant_sign << std::endl;
				change_of_cmd_to_direction = 1;
			} //
		}	  //set direction from reset speed zero
		else if (last_cmd_speed == 0.0 && fabs(current_speed) > 0.0)
		{
			//std::cout << "!!!! last_cmd_speed is zero resultant_sign = " << resultant_sign << std::endl;
			change_of_cmd_to_direction = 1;
		}

		//save last command speed
		last_cmd_speed = current_speed;

		//set output
		bool result = false;
		if (change_of_cmd_to_direction == 1)
		{
			result = true;
		}

		return result;
	}

	void PCMPalletController::SetStartUpProcedurePallet()
	{
		//reset the commands
		pcm_palletbase_commands cmd_tiller_startup;
		cmd_tiller_startup.move_forward = false;
		cmd_tiller_startup.move_backward = false;
		cmd_tiller_startup.fork_lowering = false;
		cmd_tiller_startup.fork_lifting = false;
		cmd_tiller_startup.belly = false;
		cmd_tiller_startup.slow_speed = false;
		cmd_tiller_startup.tiller = true;
		cmd_tiller_startup.trigger_buzzer = false;
		cmd_tiller_startup.analog_speed = 0;

		for (int32_t i = 0; i < 40; i++) //2s
		{
			SetPCMPalletCommands(cmd_tiller_startup);
			usleep(50000); //50ms requirements
		}

		//set tiller low
		cmd_tiller_startup.tiller = false;
		SetPCMPalletCommands(cmd_tiller_startup);
	}

	void PCMPalletController::SetHeartBeatTiller()
	{
		//to trigger tiller on conditions.
		if (last_cmd_speed == 0)
		{
			count_tiller_heartbeat++;
		}
		else
		{
			count_tiller_heartbeat = 0;
		}

		//reset the commands
		pcm_palletbase_commands cmd_tiller_heartbeat;
		cmd_tiller_heartbeat.move_forward = false;
		cmd_tiller_heartbeat.move_backward = false;
		cmd_tiller_heartbeat.fork_lowering = false;
		cmd_tiller_heartbeat.fork_lifting = false;
		cmd_tiller_heartbeat.belly = false;
		cmd_tiller_heartbeat.slow_speed = false;
		cmd_tiller_heartbeat.tiller = false;
		cmd_tiller_heartbeat.trigger_buzzer = false;
		cmd_tiller_heartbeat.analog_speed = 10;
		//
		std::cout << "!!!! count_tiller_heartbeat " << count_tiller_heartbeat << std::endl;
		//
		if (count_tiller_heartbeat > 6000) //5 minutes = 6000
		{
			count_tiller_heartbeat = 0;
			cmd_tiller_heartbeat.tiller = true;
			if (heartbeat_forward_motion)
			{
				cmd_tiller_heartbeat.move_forward = true;
				heartbeat_forward_motion = false;
			}
			else
			{
				cmd_tiller_heartbeat.move_backward = true;
				heartbeat_forward_motion = true;
			}
			for (int32_t i = 0; i < 20; i++) //45,25
			{
				SetPCMPalletCommands(cmd_tiller_heartbeat);
				usleep(50000); //50ms requirements
			}
			cmd_tiller_heartbeat.tiller = true;
			cmd_tiller_heartbeat.move_forward = false;
			cmd_tiller_heartbeat.move_backward = false;
			SetPCMPalletCommands(cmd_tiller_heartbeat);
			usleep(50000); //50ms requirements
		}
	}

	void PCMPalletController::SetShutdownPCMProcedure()
	{
		//reset the commands
		//	pcm_palletbase_commands cmd_tiller_shutdown;
		//	cmd_tiller_shutdown.move_forward=false;
		//	cmd_tiller_shutdown.move_backward=false;
		//	cmd_tiller_shutdown.fork_lowering=false;
		//	cmd_tiller_shutdown.fork_lifting=false;
		//	cmd_tiller_shutdown.belly=false;
		//	cmd_tiller_shutdown.slow_speed=false;
		//	cmd_tiller_shutdown.tiller=false;
		//	cmd_tiller_shutdown.trigger_buzzer=false;
		//	cmd_tiller_shutdown.analog_speed=0;
		//	for(int32_t i=0; i<30; i++)//2s //60 //50
		//	{
		//		SetPCMPalletCommands(cmd_tiller_shutdown);
		//		usleep(50000); //50ms requirements
		//	}

		//Pallet truck 2.
		//transmit via PCAN
		//	TPCANMsg CAN_TxMsg;
		//	//
		//	pcm_trucktwo_palletbase_commands in_pcm_command;
		//	in_pcm_command.analog_steer = 0;
		//	in_pcm_command.pcm_drive_cmd.analog_speed = 0;
		//	EncodePalletTruckTwoPCMMsg(&CAN_TxMsg,in_pcm_command);
		//	for(int32_t i=0; i<30; i++)//2s //60 //50
		//	{
		//		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 0);
		//		usleep(50000); //50ms requirements
		//	}

		//
		sptr_pcanemuc->deinitEmucPCAN();
	}

	void PCMPalletController::ResetLastCommandSpeed()
	{
		last_cmd_speed = 0;
	}

	bool PCMPalletController::DecodeTiltMotorEncMsg(TPCANRdMsg *const pMsgBuff, const int64_t time)
	{
		static int32_t ServoPosition = 0;
		static int32_t PrevPos = 0xFFFFFFFF;
		bool bSFlag;

		if (bListenFlag)
		{
			TPCANRdMsg CAN_RxMsg;
			TPCANMsg CAN_TxMsg;
			//if(!bTargetReached)
			//	{
			if (pMsgBuff->Msg.ID == 0x186 || pMsgBuff->Msg.ID == 0x187)
			{
				if ((pMsgBuff->Msg.DATA[0] == 0x37) && (pMsgBuff->Msg.DATA[1] == 0x17))
				{
					bTargetReached = true;
					//printf("position target reached\n");
					//receivePositionValue(&CAN_TxMsg);
					//sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
					//usleep(2000);
					//sptr_pcanemuc->RxEmucCANMsg(&CAN_RxMsg, 1);
				}
			}
			//	}
			//	else
			//	{

			if (pMsgBuff->Msg.ID == 0x586 || pMsgBuff->Msg.ID == 0x587)
			{
				std::cout << "OkayOkayOkayOkayOkayOkay" << std::endl;
				std::cout << "OkayOkayOkayOkayOkayOkay" << std::endl;
				std::cout << "OkayOkayOkayOkayOkayOkay" << std::endl;
				std::cout << "OkayOkayOkayOkayOkayOkay" << std::endl;
				//				std::cout << "enter" << std::endl;
				if (pMsgBuff->Msg.DATA[1] == 0x64 && pMsgBuff->Msg.DATA[2] == 0x60 && pMsgBuff->Msg.DATA[3] == 0x00)
				{
					int32_t Dummy = 0;
					ServoPosition = pMsgBuff->Msg.DATA[4];
					Dummy = pMsgBuff->Msg.DATA[5];
					Dummy <<= 8;
					ServoPosition |= Dummy;
					Dummy = pMsgBuff->Msg.DATA[6];
					Dummy <<= 16;
					ServoPosition |= Dummy;
					Dummy = pMsgBuff->Msg.DATA[7];
					Dummy <<= 24;
					ServoPosition |= Dummy;

					/*if (ServoPosition != PrevPos || Old0 != CAN_RxMsg.Msg.DATA[0] || Old1 = CAN_RxMsg.Msg.DATA[1])*/
					if (ServoPosition != PrevPos)
					{
						//						printf("ID: ");
						//						printf("%d ", pMsgBuff->Msg.ID);

						//						printf("DATA: ");
						//						for(uint i=0; i<pMsgBuff->Msg.LEN; i++)
						//						{
						//							printf("%02X ", pMsgBuff->Msg.DATA[i]);
						//						}
						bTargetReachedFinal = true;
						stat.EncoderPosStep = ServoPosition;
						if (ServoPosition > 1600)
						{
							stat.EncoderPosDegree = 90 + ((ServoPosition - 1600) * 36.0 / 160.0);
						}
						else
						{
							stat.EncoderPosDegree = 90 - ((1600 - ServoPosition) * 36.0 / 160.0);
						}
						Platform::Sensors::TiltMotorFbkMsg tilt_motor_Fbk;
						tilt_motor_Fbk.timestamp = Common::Utility::Time::getusecCountSinceEpoch();
						tilt_motor_Fbk.seq_key = 0;
						tilt_motor_Fbk.position = stat.EncoderPosDegree;
						ptr_stkci_pub->PubTiltMotorFbkMsg("TiltMotorFbkMsg", tilt_motor_Fbk);
						//pubRotorPositionAngleDegree.publish(stat);
						//printf("\n");
						PrevPos = ServoPosition;
						//std::cout << "angle = " << stat.EncoderPosDegree << std::endl;
					}
					bSFlag = true;
				}
			}
			else
			{
				bSFlag = false;
			}
			//}
		}

		return bSFlag;
	}

	bool PCMPalletController::DecodeSikoEncMsg(TPCANRdMsg *const pMsgBuff, const int64_t time, uint32_t &pos)
	{
		//TPCANRdMsg CANmsg;
		//(void) memcpy(&CANmsg, pMsgBuff, mu32_TPCANRdMsgSize);

		bool bSFlag;
		if (pMsgBuff->Msg.ID == 0x183) // 0x583
		{
			//		printf("[OUT->RXCANMsg Seiko Encoder] ID:%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", pMsgBuff->Msg.ID,
			//				pMsgBuff->Msg.DATA[0],pMsgBuff->Msg.DATA[1],
			//				pMsgBuff->Msg.DATA[2],pMsgBuff->Msg.DATA[3],
			//				pMsgBuff->Msg.DATA[4],pMsgBuff->Msg.DATA[5],
			//				pMsgBuff->Msg.DATA[6],pMsgBuff->Msg.DATA[7]);

			uint32_t PositionBuffer[4];
			uint32_t nPosition = 0U;

			PositionBuffer[0] = pMsgBuff->Msg.DATA[0];
			PositionBuffer[1] = pMsgBuff->Msg.DATA[1];
			PositionBuffer[2] = pMsgBuff->Msg.DATA[2];
			PositionBuffer[3] = pMsgBuff->Msg.DATA[3];

			PositionBuffer[1] = PositionBuffer[1] << 8;
			PositionBuffer[2] = PositionBuffer[2] << 16;
			PositionBuffer[3] = PositionBuffer[3] << 24;

			nPosition = PositionBuffer[0] | PositionBuffer[1] | PositionBuffer[2] | PositionBuffer[3];
			pos = nPosition;

			//
			bSFlag = true;
		}
		else
		{
			bSFlag = false;
		}

		return bSFlag;
	}

	bool PCMPalletController::DecodeBaumerLineEncMsg(TPCANRdMsg *const pMsgBuff, const int64_t time, uint32_t &line_enc_pos)
	{
		bool bSFlag;
		if (pMsgBuff->Msg.ID == 0x184)
		{
			uint32_t PositionBuffer[4];
			uint32_t nPosition = 0U;

			PositionBuffer[0] = pMsgBuff->Msg.DATA[0];
			PositionBuffer[1] = pMsgBuff->Msg.DATA[1];
			PositionBuffer[2] = pMsgBuff->Msg.DATA[2];
			PositionBuffer[3] = pMsgBuff->Msg.DATA[3];

			PositionBuffer[1] = PositionBuffer[1] << 8;
			PositionBuffer[2] = PositionBuffer[2] << 16;
			PositionBuffer[3] = PositionBuffer[3] << 24;

			nPosition = PositionBuffer[0] | PositionBuffer[1] | PositionBuffer[2] | PositionBuffer[3];
			line_enc_pos = nPosition;

			//
			bSFlag = true;
		}
		else
		{
			bSFlag = false;
		}

		return bSFlag;
	}

	bool PCMPalletController::DecodeBaumerTiltEncMsg(TPCANRdMsg *const pMsgBuff, const int64_t time, uint32_t &tilt_enc_pos)
	{
		bool bSFlag;
		if (pMsgBuff->Msg.ID == 0x284)
		{
			uint32_t PositionBuffer[2];
			uint32_t nPosition = 0U;

			PositionBuffer[0] = pMsgBuff->Msg.DATA[0];
			PositionBuffer[1] = pMsgBuff->Msg.DATA[1];

			PositionBuffer[1] = PositionBuffer[1] << 8;

			nPosition = PositionBuffer[0] | PositionBuffer[1];
			tilt_enc_pos = nPosition;

			//
			bSFlag = true;
		}
		else
		{
			bSFlag = false;
		}

		return bSFlag;
	}

	void PCMPalletController::EncodeSikoEncMsg(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x0603;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		CANData.DATA[0] = 0x40;
		CANData.DATA[1] = 0x04;
		CANData.DATA[2] = 0x60;

		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::ConvertUIntegerToHex(const uint32_t nValue, uint8_t (&pBuffer)[4])
	{
		const uint32_t nMask = 0x000000FFU;
		const uint32_t nBitShiftIndex[4] = {0U, 8U, 16U, 24U};
		//
		for (uint32_t i = 0U; i < 4; i++)
		{
			uint32_t nBuffer = 0U;
			if (nBitShiftIndex[i] <= 24U)
			{
				nBuffer = nValue >> nBitShiftIndex[i];
			}
			pBuffer[i] = static_cast<uint8_t>(nBuffer & nMask);
		} //
	}

	void PCMPalletController::ConvertHexToUInteger(const uint8_t (&pBuffer)[4], uint32_t &nValue)
	{
		const uint32_t nBitShiftIndex[4] = {0U, 8U, 16U, 24U};
		//
		for (uint32_t i = 0U; i < 4; i++)
		{
			uint32_t nBuffer = 0U;
			if (nBitShiftIndex[i] <= 24U)
			{
				const uint32_t nPreShiftBuffer = static_cast<uint32_t>(pBuffer[i]);
				nBuffer = nPreShiftBuffer << nBitShiftIndex[i];
			}
			nValue |= nBuffer;
		}
		//
	}

	void PCMPalletController::ConvertHexToInteger(const uint8_t (&pBuffer)[2], int16_t &nValue)
	{
		const uint32_t nBitShiftIndex[2] = {0U, 8U};
		//
		for (uint32_t i = 0U; i < 2; i++)
		{
			int16_t nBuffer = 0U;
			if (nBitShiftIndex[i] <= 8U)
			{
				const int16_t nPreShiftBuffer = static_cast<int16_t>(pBuffer[i]);
				nBuffer = nPreShiftBuffer << nBitShiftIndex[i];
			}
			nValue |= nBuffer;
		}
		//
	}

	void PCMPalletController::receivePositionValue(TPCANMsg *const pMsgBuff, const uint32_t CAN_RX_ID)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = CAN_RX_ID; //0x601 node id 1

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		CANData.DATA[0] = 0x40;
		CANData.DATA[1] = 0x64;
		CANData.DATA[2] = 0x60;
		CANData.DATA[3] = 0x00;
		CANData.DATA[4] = 0x00;
		CANData.DATA[5] = 0x00;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;
		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetSpeed(uchar_t DesiredSpeed, TPCANMsg *const pMsgBuff, const uint32_t CAN_RX_ID)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = CAN_RX_ID; //0x601 node id 1

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		CANData.DATA[0] = 0x23;
		CANData.DATA[1] = 0x10;
		CANData.DATA[2] = 0x24;
		CANData.DATA[3] = 0x05;
		CANData.DATA[4] = DesiredSpeed;
		CANData.DATA[5] = 0x00;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;
		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetAcceleration(uchar_t DesiredAcceleration, TPCANMsg *const pMsgBuff, const uint32_t CAN_RX_ID)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = CAN_RX_ID; //0x601 node id 1

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		CANData.DATA[0] = 0x23;
		CANData.DATA[1] = 0x10;
		CANData.DATA[2] = 0x24;
		CANData.DATA[3] = 0x04;
		CANData.DATA[4] = DesiredAcceleration;
		CANData.DATA[5] = 0x00;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;
		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SendControlCommand(uchar_t CAN_Command, uchar_t DeviceCAN_ID, TPCANMsg *const pMsgBuff, const uint32_t CAN_RX_ID)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = CAN_RX_ID; //0x201 node id 1

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		CANData.DATA[0] = CAN_Command;
		CANData.DATA[1] = DeviceCAN_ID;
		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SeActDeactCommand(uchar_t CAN_Command, uchar_t DeviceCAN_ID, TPCANMsg *const pMsgBuff, const uint32_t CAN_RX_ID)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = CAN_RX_ID; //0x00

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		CANData.DATA[0] = CAN_Command;
		CANData.DATA[1] = DeviceCAN_ID;
		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SendPositionSetpointCommand(int32_t PositionStep, TPCANMsg *const pMsgBuff, const uint32_t CAN_RX_ID)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = CAN_RX_ID; //0x401 node id 1

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		CANData.DATA[0] = 0x1F;
		CANData.DATA[1] = 0x00;
		CANData.DATA[2] = (PositionStep & 0xFF);
		CANData.DATA[3] = ((PositionStep >> 8) & 0xFF);
		CANData.DATA[4] = ((PositionStep >> 16) & 0xFF);
		CANData.DATA[5] = ((PositionStep >> 24) & 0xFF);
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;
		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::ServoGoToPos(float PositionDegree, uchar_t Speed, uint16_t TiltMotor)
	{
		int32_t PositionStep = 0;
		uint32_t CAN_NODE_ID = 0x00;
		if (PositionDegree > 90)
		{
			PositionDegree = PositionDegree - 90;
			PositionStep = 1600 + (PositionDegree * 160 / 36); // 1600 is the original 90 degree tilt motor
		}
		else
		{
			PositionDegree = 90 - PositionDegree;
			PositionStep = 1600 - (PositionDegree * 160 / 36); // 1600 is the original 90 degree tilt motor
		}

		//	    PositionStep = 1600;
		//
		if (TiltMotor == 1) //Lower Tilt Motor
		{
			CAN_NODE_ID = 0x06;
		}
		else if (TiltMotor == 2)
		{
			CAN_NODE_ID = 0x07;
		}
		bListenFlag = true;
		TPCANMsg CAN_TxMsg;

		SetSpeed(Speed, &CAN_TxMsg, 0x600 + CAN_NODE_ID);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(20000);

		SetAcceleration(1, &CAN_TxMsg, 0x600 + CAN_NODE_ID);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(20000);

		//Activate the motor
		SeActDeactCommand(0x01, CAN_NODE_ID, &CAN_TxMsg, 0x00);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(20000);

		SendControlCommand(0x06, 0x01, &CAN_TxMsg, 0x200 + CAN_NODE_ID);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(20000);

		SendControlCommand(0x07, 0x01, &CAN_TxMsg, 0x200 + CAN_NODE_ID);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(20000);

		SendControlCommand(0x0F, 0x01, &CAN_TxMsg, 0x200 + CAN_NODE_ID);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(20000);

		SendPositionSetpointCommand(PositionStep, &CAN_TxMsg, 0x400 + CAN_NODE_ID);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		std::cout << "position send" << std::endl;
		usleep(20000);

		int64_t time = Common::Utility::Time::getmsCountSinceEpoch();
		while (!bTargetReached)
		{
			int64_t current_time = Common::Utility::Time::getmsCountSinceEpoch();
			int64_t time_differences = (current_time - time) / 1000; //convert the differences to sec
			if(time_differences >= 60) //1min timeout
			{
				break;
			}
			receivePositionValue(&CAN_TxMsg, 0x600 + CAN_NODE_ID);
			sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
			usleep(20000);
		}

		SendControlCommand(0x0F, 0x01, &CAN_TxMsg, 0x200 + CAN_NODE_ID);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(10000);

		SendControlCommand(0x07, 0x01, &CAN_TxMsg, 0x200 + CAN_NODE_ID);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(10000);

		SendControlCommand(0x06, 0x01, &CAN_TxMsg, 0x200 + CAN_NODE_ID);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(10000);

		SendControlCommand(0x00, 0x01, &CAN_TxMsg, 0x200 + CAN_NODE_ID);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(10000);

		SeActDeactCommand(0x80, CAN_NODE_ID, &CAN_TxMsg, 0x00);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(10000);

		bListenFlag = false;
		bTargetReachedFinal = false;
		bTargetReached = false;
	}

	void PCMPalletController::SetupSteerSikoEncoder()
	{
		bool b_ReadTimeout;
		TPCANMsg CAN_TxMsg;
		//
		SetupCANMsgOneSteerSikoEncoder(&CAN_TxMsg);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1); //sptr_pcanemuc_encoder(&CAN_TxMsg, {0: CAN_1, 1: CAN_2});
		usleep(50000);								//50ms requirements
		//
		SetupCANMsgTwoSteerSikoEncoder(&CAN_TxMsg);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(50000); //50ms requirements
		//
		SetupCANMsgThreeSteerSikoEncoder(&CAN_TxMsg);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(50000); //50ms requirements
		//
		SetupCANMsgFourSteerSikoEncoder(&CAN_TxMsg);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(50000); //50ms requirements
		//
		SetupCANMsgFiveSteerSikoEncoder(&CAN_TxMsg);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(50000); //50ms requirements

		std::cout << " SetupSteerSikoEncoder Completed" << std::endl;
	}

	void PCMPalletController::SetupCANMsgOneSteerSikoEncoder(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		CANData.DATA[0] = 0x80;
		CANData.DATA[1] = 0x03;

		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetupCANMsgTwoSteerSikoEncoder(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x0603;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		CANData.DATA[0] = 0x22;
		CANData.DATA[1] = 0x00;
		CANData.DATA[2] = 0x18;
		CANData.DATA[3] = 0x02;
		CANData.DATA[4] = 0xFE;
		CANData.DATA[5] = 0x00;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;

		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetupCANMsgThreeSteerSikoEncoder(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x0603;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		CANData.DATA[0] = 0x22;
		CANData.DATA[1] = 0x00;
		CANData.DATA[2] = 0x18;
		CANData.DATA[3] = 0x05;
		CANData.DATA[4] = 0x64;
		CANData.DATA[5] = 0x00;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;

		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetupCANMsgFourSteerSikoEncoder(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x0603;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		CANData.DATA[0] = 0x22;
		CANData.DATA[1] = 0x10;
		CANData.DATA[2] = 0x10;
		CANData.DATA[3] = 0x01;
		CANData.DATA[4] = 0x73;
		CANData.DATA[5] = 0x61;
		CANData.DATA[6] = 0x76;
		CANData.DATA[7] = 0x65;

		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetupCANMsgFiveSteerSikoEncoder(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x00;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		//
		CANData.DATA[0] = 0x01;
		CANData.DATA[1] = 0x03;
		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetupLineBaumerEncoder()
	{
		bool b_ReadTimeout;
		TPCANMsg CAN_TxMsg;
		//
		SetupCANMsgOneLineBaumerEncoder(&CAN_TxMsg);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1); //sptr_pcanemuc_encoder(&CAN_TxMsg, {0: CAN_1, 1: CAN_2});
		usleep(50000);								//50ms requirements
		//
		SetupCANMsgTwoLineBaumerEncoder(&CAN_TxMsg);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(50000); //50ms requirements
		//
		SetupCANMsgThreeLineBaumerEncoder(&CAN_TxMsg);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(50000); //50ms requirements
		//
		SetupCANMsgFourLineBaumerEncoder(&CAN_TxMsg);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(50000); //50ms requirements
		//
		SetupCANMsgFiveLineBaumerEncoder(&CAN_TxMsg);
		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1);
		usleep(50000); //50ms requirements

		std::cout << " SetupLineSikoEncoder Completed" << std::endl;
	}

	void PCMPalletController::SetupCANMsgOneLineBaumerEncoder(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		CANData.DATA[0] = 0x80;
		CANData.DATA[1] = 0x04;

		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetupCANMsgTwoLineBaumerEncoder(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x0604;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		CANData.DATA[0] = 0x22;
		CANData.DATA[1] = 0x00;
		CANData.DATA[2] = 0x18;
		CANData.DATA[3] = 0x02;
		CANData.DATA[4] = 0xFE;
		CANData.DATA[5] = 0x00;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;

		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetupCANMsgThreeLineBaumerEncoder(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x0604;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		CANData.DATA[0] = 0x22;
		CANData.DATA[1] = 0x00;
		CANData.DATA[2] = 0x18;
		CANData.DATA[3] = 0x05;
		CANData.DATA[4] = 0x64;
		CANData.DATA[5] = 0x00;
		CANData.DATA[6] = 0x00;
		CANData.DATA[7] = 0x00;

		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetupCANMsgFourLineBaumerEncoder(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x0604;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		CANData.DATA[0] = 0x22;
		CANData.DATA[1] = 0x10;
		CANData.DATA[2] = 0x10;
		CANData.DATA[3] = 0x01;
		CANData.DATA[4] = 0x73;
		CANData.DATA[5] = 0x61;
		CANData.DATA[6] = 0x76;
		CANData.DATA[7] = 0x65;

		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::SetupCANMsgFiveLineBaumerEncoder(TPCANMsg *const pMsgBuff)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x00;

		TPCANMsg CANData;
		const uint32_t copysize = sizeof(TPCANMsg);
		//
		uint8_t i;
		for (i = 0U; i < 8U; i++)
		{
			CANData.DATA[i] = 0U;
		}
		//
		CANData.DATA[0] = 0x01;
		CANData.DATA[1] = 0x04;
		//
		CANData.ID = CAN_RX_SETTINGS1_ID;
		CANData.LEN = 8U;
		CANData.MSGTYPE = 0U;

		// Copy CANData to MsgBuff
		(void)memcpy(pMsgBuff, &CANData, copysize);
	}

	void PCMPalletController::OpenForkMainValve()
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x750;

		TPCANMsg CAN_TxMsg;

		CAN_TxMsg.DATA[0] = 0xC1;
		CAN_TxMsg.DATA[1] = 0x01;
		CAN_TxMsg.DATA[2] = 0x00;
		CAN_TxMsg.DATA[3] = 0x00;
		CAN_TxMsg.DATA[4] = 0x00;
		CAN_TxMsg.DATA[5] = 0x00;
		CAN_TxMsg.DATA[6] = 0x00;
		CAN_TxMsg.DATA[7] = 0x00;

		CAN_TxMsg.ID = CAN_RX_SETTINGS1_ID;
		CAN_TxMsg.LEN = 8U;
		CAN_TxMsg.MSGTYPE = 0U;

		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1); // Send Command to Fork valve
	}

	void PCMPalletController::SetForkliftForkCmds(Platform::Sensors::PalletBaseCmdMsg &in_data_basecmd)
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x750;

		TPCANMsg CAN_TxMsg;

		CAN_TxMsg.DATA[0] = 0xC1;

		if (in_data_basecmd.trigger_tiller == 0x03) // fork active, fork down
		{
			printf("Fork Down\n");
			CAN_TxMsg.DATA[1] = 0x03;
			CAN_TxMsg.DATA[2] = 0x00;
		}
		else if (in_data_basecmd.trigger_tiller == 0x05) // fork active, fork up
		{
			printf("Fork Up\n");
			CAN_TxMsg.DATA[1] = 0x05;
			CAN_TxMsg.DATA[2] = 0x00;
		}
		else if (in_data_basecmd.trigger_tiller == 0x11) // fork active, tilt forward
		{
			printf("Fork Tilt Forward\n");
			CAN_TxMsg.DATA[1] = 0x09;
			CAN_TxMsg.DATA[2] = 0x00;
		}
		else if (in_data_basecmd.trigger_tiller == 0x21) // fork active, tilt backward
		{
			printf("Fork Tilt Backward\n");
			CAN_TxMsg.DATA[1] = 0x11;
			CAN_TxMsg.DATA[2] = 0x00;
		}
		else if (in_data_basecmd.trigger_tiller == 0x41) // fork active, move right
		{
			printf("Fork Move Right\n");
			CAN_TxMsg.DATA[1] = 0x21;
			CAN_TxMsg.DATA[2] = 0x00;
		}
		else if (in_data_basecmd.trigger_tiller == 0x81) // fork active, move left
		{
			printf("Fork Move Left\n");
			CAN_TxMsg.DATA[1] = 0x01;
			CAN_TxMsg.DATA[2] = 0x01;
		}

		CAN_TxMsg.DATA[3] = 0x00;
		CAN_TxMsg.DATA[4] = 0x00;
		CAN_TxMsg.DATA[5] = 0x00;
		CAN_TxMsg.DATA[6] = 0x00;
		CAN_TxMsg.DATA[7] = 0x00;

		CAN_TxMsg.ID = CAN_RX_SETTINGS1_ID;
		CAN_TxMsg.LEN = 8U;
		CAN_TxMsg.MSGTYPE = 0U;

		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1); // Send Command to Fork valve

		//
		int64_t current_timestamp;
		current_timestamp = Common::Utility::Time::getmsCountSinceEpoch();
		std::cout << current_timestamp - previous_timestamp << " ms main loop thread time " << std::endl;
		previous_timestamp = current_timestamp;
	}

	void PCMPalletController::OpenSteerMainValve()
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x750;

		TPCANMsg CAN_TxMsg;

		CAN_TxMsg.DATA[0] = 0xC1;
		CAN_TxMsg.DATA[1] = 0x00;
		CAN_TxMsg.DATA[2] = 0x02;
		CAN_TxMsg.DATA[3] = 0x00;
		CAN_TxMsg.DATA[4] = 0x00;
		CAN_TxMsg.DATA[5] = 0x00;
		CAN_TxMsg.DATA[6] = 0x00;
		CAN_TxMsg.DATA[7] = 0x00;

		CAN_TxMsg.ID = CAN_RX_SETTINGS1_ID;
		CAN_TxMsg.LEN = 8U;
		CAN_TxMsg.MSGTYPE = 0U;

		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1); // Send Command to Steer valve
	}

	void PCMPalletController::OpenSteerPropValve()
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x750;

		TPCANMsg CAN_TxMsg;

		CAN_TxMsg.DATA[0] = 0xC1;
		CAN_TxMsg.DATA[1] = 0x00;
		CAN_TxMsg.DATA[2] = 0x06;
		CAN_TxMsg.DATA[3] = 0x00;
		CAN_TxMsg.DATA[4] = 0x00;
		CAN_TxMsg.DATA[5] = 0x00;
		CAN_TxMsg.DATA[6] = 0x00;
		CAN_TxMsg.DATA[7] = 0x00;

		CAN_TxMsg.ID = CAN_RX_SETTINGS1_ID;
		CAN_TxMsg.LEN = 8U;
		CAN_TxMsg.MSGTYPE = 0U;

		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1); // Send Command to Steer valve
	}

	void PCMPalletController::CloseAllValves()
	{
		const uint32_t CAN_RX_SETTINGS1_ID = 0x750;

		TPCANMsg CAN_TxMsg;

		CAN_TxMsg.DATA[0] = 0xC1;
		CAN_TxMsg.DATA[1] = 0x00;
		CAN_TxMsg.DATA[2] = 0x00;
		CAN_TxMsg.DATA[3] = 0x00;
		CAN_TxMsg.DATA[4] = 0x00;
		CAN_TxMsg.DATA[5] = 0x00;
		CAN_TxMsg.DATA[6] = 0x00;
		CAN_TxMsg.DATA[7] = 0x00;

		CAN_TxMsg.ID = CAN_RX_SETTINGS1_ID;
		CAN_TxMsg.LEN = 8U;
		CAN_TxMsg.MSGTYPE = 0U;

		sptr_pcanemuc->TxEmucCANMsg(&CAN_TxMsg, 1); // Send Command to valve
	}

	PCMPalletController::~PCMPalletController()
	{
		if (logging_flag_status)
		{
			sptr_PCanLogger->CloseDataLogger();
		}
		//sptr_pcan->stopCan();
		std::cout << " DEINIT EMUC BY PCMPALLETCONTROLLER" << std::endl;
		sptr_pcanemuc->deinitEmucPCAN();
	}

} // namespace DBWPalletCommon
