#edit the variables here
find_package(Threads)

set(TARGET DBW_BenchTestPlatformStacker)

file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_stkci_comms "${CMAKE_CURRENT_SOURCE_DIR}/StkciPCMCardComms/*.cpp")
set(SOURCES ${src_files} ${src_stkci_comms})

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
 	    ProjectPlatformSensors
 	    UGVDataMessagesCommon	    
            ProjectPlatformPCanUsb
	    pcan		  
            ProjectPlatformPCanEmuc
            ${CMAKE_THREAD_LIBS_INIT}
            ${OPENCV_LDFLAGS}
)

#            
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/StkciPCMCardComms
)

#
add_executable(${TARGET} ${SOURCES})
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
