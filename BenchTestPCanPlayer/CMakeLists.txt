#edit the variables here
find_package(Threads)

set(TARGET DBW_BenchTestPCanPlayer)

#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_dbw_common_pcm "${TRUNK_DIR}/component/DBW/DBWCommon/PCM/*.cpp")
set(SOURCES ${src_files} ${src_dbw_common_pcm})

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
 	    ProjectPlatformSensors
            UGVDataMessagesCommon	    
            ProjectPlatformPCanUsb
	    pcan		  
            ProjectPlatformPCanEmuc
            ${CMAKE_THREAD_LIBS_INIT}
            ${OPENCV_LDFLAGS})

#            
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
${TRUNK_DIR}/component/DBW/DBWCommon/PcanSeikoEncoder
${TRUNK_DIR}/component/DBW/DBWCommon/PCM
)

#
add_executable(${TARGET} ${SOURCES})
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
