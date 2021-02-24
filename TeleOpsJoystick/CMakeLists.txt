#edit the variables here
find_package(Threads)

set(TARGET DBW_TeleOpsJoystick)

#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_joystick "${CMAKE_CURRENT_SOURCE_DIR}/Joystick/*.cpp")
file(GLOB src_readconfig "${CMAKE_CURRENT_SOURCE_DIR}/ReadConfig/*.cpp")
file(GLOB src_comms "${CMAKE_CURRENT_SOURCE_DIR}/TeleOpsComms/*.cpp")

set(SOURCES 
${src_files} 
${src_joystick} 
${src_readconfig} 
${src_comms} 
)

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
 	    ProjectPlatformSensors
            UGVDataMessagesCommon	    
            ${CMAKE_THREAD_LIBS_INIT})
            
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/Joystick
${CMAKE_CURRENT_SOURCE_DIR}/ReadConfig
${CMAKE_CURRENT_SOURCE_DIR}/TeleOpsComms
)

#
add_executable(${TARGET} ${SOURCES})
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
