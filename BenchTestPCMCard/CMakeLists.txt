#edit the variables here
find_package(Threads)

set(TARGET DBW_BenchTestPCMCard)

#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set(SOURCES ${src_files})

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
 	   		ProjectPlatformSensors
 	   		UGVDataMessagesCommon	    
            ProjectPlatformPCanUsb
	    	pcan		  
            ${CMAKE_THREAD_LIBS_INIT}
            ${OPENCV_LDFLAGS})

#            
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
)

#
add_executable(${TARGET} ${SOURCES})
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
