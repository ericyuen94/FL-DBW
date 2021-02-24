#edit the variables here
find_package(Threads)

set(TARGET DBW_BenchTestSteerCalibPlayer)

#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_steer_calib_reader "${TRUNK_DIR}/component/DBW/QTCalibSteerMotorCurvature/Logger/*.cpp")
set(SOURCES ${src_files} ${src_steer_calib_reader})

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
	    ProjectPlatformSensors
	    ProjectPlatformLog
            UGVDataMessagesCommon
 	    ${Boost_SYSTEM_LIBRARY} 
	    ${Boost_FILESYSTEM_LIBRARY} 
	    ${Boost_LIBRARIES}
	    ${CMAKE_THREAD_LIBS_INIT}
            ${OPENCV_LDFLAGS})

#            
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
${TRUNK_DIR}/component/DBW/QTCalibSteerMotorCurvature/Logger
)

#
add_executable(${TARGET} ${SOURCES})
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
