#edit the variables here
find_package(Threads)

set(TARGET DBW_PIM)

#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_comms "${CMAKE_CURRENT_SOURCE_DIR}/PIMcomms/*.cpp")

#
set(SOURCES 
${src_files}
${src_comms} 
${src_readconfig} 
)

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
 	    ProjectPlatformSensors
	    ProjectPlatformIsolatedDIO
            UGVDataMessagesCommon
            ProjectPlatformPCanUsb
            pcan	    
            ${CMAKE_THREAD_LIBS_INIT})

#            
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/PIMcomms
)

#
add_executable(${TARGET} ${SOURCES})
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
