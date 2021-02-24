#edit the variables here
find_package(Threads)
find_package(Qt5Widgets REQUIRED)

#
set(TARGET DBW_QTSMCtrl)


#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_gui "${CMAKE_CURRENT_SOURCE_DIR}/gui/*.cpp")
file(GLOB src_pcm "${CMAKE_CURRENT_SOURCE_DIR}/PCM/*.cpp")
file(GLOB src_dbw_common_pcm "${TRUNK_DIR}/component/DBW/DBWCommon/PCM/*.cpp")
file(GLOB src_files_dbw "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/*.cpp")
file(GLOB src_files_readconfig "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/ReadConfig/*.cpp")
file(GLOB src_files_common "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Common/*.c")
file(GLOB src_files_motrona "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Motrona/*.cpp")
file(GLOB src_files_smart_motor "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/SmartMotor/*.c")
file(GLOB src_files_interface "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Interface/*.c")
file(GLOB src_files_logger "${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Logger/*.cpp")
file(GLOB src_files_seikoenc "${TRUNK_DIR}/component/DBW/DBWCommon/PcanSeikoEncoder/*.cpp")
set(SOURCES ${src_files} ${src_gui} ${src_files_dbw} ${src_files_readconfig} ${src_files_common} 
${src_files_motrona} ${src_files_smart_motor} ${src_files_interface} ${src_files_logger} 
${src_files_seikoenc} ${src_pcm} ${src_dbw_common_pcm})

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
    	    ProjectPlatformSensors
    	    ProjectPlatformPCanUsb
            pcan
            UGVDataMessagesCommon
	    ProjectPlatformPCanEmuc
            ${CMAKE_THREAD_LIBS_INIT}
            ${OPENCV_LDFLAGS}
	    ${QT_LIBRARIES})
            

#include directories
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/gui
${Qt5Widgets_INCLUDES}
${Boost_INCLUDE_DIRS} 
${TRUNK_DIR}/component/DBW/DBWCommon/PCM
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/ReadConfig
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Common
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Motrona
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/SmartMotor
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Interface
${TRUNK_DIR}/component/DBW/DBWCommon/SerialSmartMotorSteer/Logger
${TRUNK_DIR}/component/DBW/DBWCommon/PcanSeikoEncoder
)

#
SET(QTMOC_HEADERS 
${CMAKE_CURRENT_SOURCE_DIR}/gui/smctrlmapviewerTab.h 
${CMAKE_CURRENT_SOURCE_DIR}/gui/smctrltabdialog.h)

#
QT5_WRAP_CPP(QTMOC_HEADERS_MOC ${QTMOC_HEADERS})
ADD_DEFINITIONS(${QT_DEFINITIONS})


#
add_executable(${TARGET} ${SOURCES} ${QTMOC_HEADERS_MOC})
qt5_use_modules(${TARGET} Widgets)
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
