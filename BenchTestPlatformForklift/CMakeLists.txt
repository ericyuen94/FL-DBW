#edit the variables here
find_package(Threads)

set(TARGET DBW_BenchTestPlatformForklift)

#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
#MESSAGE(STATUS "TRUNK_DIR = " ${TRUNK_DIR})  
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_stkci_comms "${CMAKE_CURRENT_SOURCE_DIR}/PlatformForkliftComms/*.cpp")
file(GLOB src_dbw_common_pcm "${TRUNK_DIR}/component/DBW/DBWCommon/PCM/*.cpp")
file(GLOB src_dbw_common_adam "${TRUNK_DIR}/component/DBW/DBWCommon/AdamController/*.cpp")
set(SOURCES ${src_files} ${src_stkci_comms} ${src_dbw_common_pcm} ${src_dbw_common_adam})

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

set(EXTERNALLIBS ${MODBUS_LIB1}
		 ${MODBUS_LIB1}
		 ${MODBUS_LIB2})

#add libraries
set(LIBS    ProjectPlatformIsolatedDIO
	    ProjectPlatformController
            ProjectPlatformUtility
 	    ProjectPlatformSensors
            ProjectPlatformPCanUsb
	    UGVDataMessagesCommon
	    pcan		  
            ProjectPlatformPCanEmuc
            ${CMAKE_THREAD_LIBS_INIT}
            ${OPENCV_LDFLAGS}
	    ${EXTERNALLIBS})

#          
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/PlatformForkliftComms
${TRUNK_DIR}/component/DBW/DBWCommon/PCM
${TRUNK_DIR}/component/DBW/DBWCommon/AdamController
)

#
add_executable(${TARGET} ${SOURCES})
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
