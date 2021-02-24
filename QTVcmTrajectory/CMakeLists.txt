#edit the variables here
find_package(Threads)
find_package(Qt5Widgets REQUIRED)

#
set(TARGET DBW_QTVcmTrajectory)

#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_gui 	"${CMAKE_CURRENT_SOURCE_DIR}/gui/*.cpp")
file(GLOB src_config "${CMAKE_CURRENT_SOURCE_DIR}/config/*.cpp")
file(GLOB src_comms "${CMAKE_CURRENT_SOURCE_DIR}/comms/*.cpp")
file(GLOB src_logger "${CMAKE_CURRENT_SOURCE_DIR}/logger/*.cpp")
set(SOURCES ${src_files} ${src_gui} ${src_config} ${src_comms} ${src_logger})

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
    	    ProjectPlatformSensors
    	    ProjectPlatformLog
            UGVDataMessagesCommon
            ${CMAKE_THREAD_LIBS_INIT}
		    ${Boost_FILESYSTEM_LIBRARY} 
            ${Boost_LIBRARIES}
            ${OPENCV_LDFLAGS}
		    ${QT_LIBRARIES})
            
#include directories
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/gui
${CMAKE_CURRENT_SOURCE_DIR}/config
${CMAKE_CURRENT_SOURCE_DIR}/comms
${CMAKE_CURRENT_SOURCE_DIR}/logger
${Qt5Widgets_INCLUDES}
${Boost_INCLUDE_DIRS} 
)

#
SET(QTMOC_HEADERS 
${CMAKE_CURRENT_SOURCE_DIR}/gui/QTVcmTrajectorydialog.h 
${CMAKE_CURRENT_SOURCE_DIR}/gui/QTVcmTrajectoryTab.h)

#
QT5_WRAP_CPP(QTMOC_HEADERS_MOC ${QTMOC_HEADERS})
ADD_DEFINITIONS(${QT_DEFINITIONS})


#
add_executable(${TARGET} ${SOURCES} ${QTMOC_HEADERS_MOC})
qt5_use_modules(${TARGET} Widgets)
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
