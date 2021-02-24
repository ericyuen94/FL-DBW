#edit the variables here
find_package(Threads)

set(TARGET DBW_BenchTestEncoder)

file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_files_seikoenc "${TRUNK_DIR}/component/DBW/DBWCommon/PcanSeikoEncoder/*.cpp")
set(SOURCES ${src_files} ${src_files_seikoenc})

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
 	    ProjectPlatformSensors
 	    UGVDataMessagesCommon	    
            ProjectPlatformPCanUsb
	    ProjectPlatformPCanEmuc
	    pcan		  
            ${CMAKE_THREAD_LIBS_INIT}
            ${OPENCV_LDFLAGS}
)

#            
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
${TRUNK_DIR}/component/DBW/DBWCommon/PcanSeikoEncoder
)

#
add_executable(${TARGET} ${SOURCES})
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
