#edit the variables here
find_package(Threads)

set(TARGET DBW_VCM)

#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_files_comms "${CMAKE_CURRENT_SOURCE_DIR}/VcmComms/*.cpp")
file(GLOB src_dbw_common_flc "${TRUNK_DIR}/component/DBW/DBWCommon/FuzzyLogicController/*.cpp")

#
set(SOURCES 
${src_files} 
${src_files_comms} 
${src_dbw_common_flc}
)

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
		ProjectPlatformSensors
            UGVDataMessagesCommon
            ${CMAKE_THREAD_LIBS_INIT})

#            
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/VcmComms
${TRUNK_DIR}/component/DBW/DBWCommon/FuzzyLogicController
)

#
add_executable(${TARGET} ${SOURCES})
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
