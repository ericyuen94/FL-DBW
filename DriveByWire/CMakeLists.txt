#edit the variables here
find_package(Threads)

set(TARGET DBW_DriveByWire)

#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
#MESSAGE(STATUS "TRUNK_DIR = " ${TRUNK_DIR})  
#
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_comms "${CMAKE_CURRENT_SOURCE_DIR}/DBWComms/*.cpp")
file(GLOB src_dbwthreads "${CMAKE_CURRENT_SOURCE_DIR}/DBWThreads/*.cpp")
file(GLOB src_readconfig "${CMAKE_CURRENT_SOURCE_DIR}/ReadConfig/*.cpp")
file(GLOB src_smartmotor "${CMAKE_CURRENT_SOURCE_DIR}/SmartMotor/*.cpp")
file(GLOB src_files_dbw "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/*.cpp")
file(GLOB src_files_readconfig "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/ReadConfig/*.cpp")
file(GLOB src_files_common "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Common/*.c")
file(GLOB src_files_motrona "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Motrona/*.cpp")
file(GLOB src_files_smart_motor "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/SmartMotor/*.c")
file(GLOB src_files_interface "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Interface/*.c")
file(GLOB src_files_logger "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Logger/*.cpp")
file(GLOB src_files_seikoenc "${TRUNK_DIR}/component/DBW/DBWCommon/PcanSeikoEncoder/*.cpp")
file(GLOB src_dbw_common_pcm "${TRUNK_DIR}/component/DBW/DBWCommon/PCM/*.cpp")
file(GLOB src_dbw_common_adam "${TRUNK_DIR}/component/DBW/DBWCommon/AdamController/*.cpp")
#
set(SOURCES 
${src_files} 
${src_comms} 
${src_dbwthreads} 
${src_readconfig}
${src_smartmotor} 
${src_files_dbw}
${src_files_readconfig}
${src_files_common}
${src_files_motrona}
${src_files_smart_motor}
${src_files_interface}
${src_files_logger}
${src_files_seikoenc}
${src_dbw_common_pcm} 
${src_dbw_common_adam}
)

#
FIND_LIBRARY(MODBUS_LIB1
      	     NAME libmodbus.so.5.1.0
	     PATHS /usr/local/lib)

FIND_LIBRARY(MODBUS_LIB2
      	     NAME libmodbus.so.5
	     PATHS /usr/local/lib)

FIND_LIBRARY(MODBUS_LIB3
      	     NAME libmodbus.so
	     PATHS /usr/local/lib)

INCLUDE_DIRECTORIES(/usr/local/include)
#
set(EXTERNALLIBS ${MODBUS_LIB1}
		 ${MODBUS_LIB1}
		 ${MODBUS_LIB2})
            
#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
 	    ProjectPlatformSensors
 	    ProjectPlatformPCanUsb
            UGVDataMessagesCommon	
            pcan    
	    ProjectPlatformPCanEmuc
	    ProjectPlatformIsolatedDIO
            ${CMAKE_THREAD_LIBS_INIT}
	    ${EXTERNALLIBS})

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/DBWComms
${CMAKE_CURRENT_SOURCE_DIR}/DBWThreads
${CMAKE_CURRENT_SOURCE_DIR}/ReadConfig
${CMAKE_CURRENT_SOURCE_DIR}/SmartMotor
${TRUNK_DIR}/component/DBW/DBWCommon/PCM
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/ReadConfig
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Common
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Motrona
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/SmartMotor
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Interface
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Logger
${TRUNK_DIR}/component/DBW/DBWCommon/PcanSeikoEncoder
${TRUNK_DIR}/component/DBW/DBWCommon/AdamController
)

#
add_executable(${TARGET} ${SOURCES})
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
