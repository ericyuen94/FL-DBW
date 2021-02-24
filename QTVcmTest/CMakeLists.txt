#edit the variables here
find_package(Threads)
find_package(Qt5Widgets REQUIRED)

#
set(TARGET DBW_QTVcmTest)

#MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB src_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB src_gui "${CMAKE_CURRENT_SOURCE_DIR}/gui/*.cpp")
set(SOURCES ${src_files} ${src_gui} ${src_comms})

#add libraries
set(LIBS    ProjectPlatformController
            ProjectPlatformUtility
 	    ProjectPlatformSensors
            UGVDataMessagesCommon
            ${CMAKE_THREAD_LIBS_INIT}
            ${OPENCV_LDFLAGS}
	    ${QT_LIBRARIES})
            
#include directories
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/gui
${Qt5Widgets_INCLUDES}
${Boost_INCLUDE_DIRS} 
)

#
SET(QTMOC_HEADERS 
${CMAKE_CURRENT_SOURCE_DIR}/gui/testVCMdialog.h 
${CMAKE_CURRENT_SOURCE_DIR}/gui/testVCMviewerTab.h)

#
QT5_WRAP_CPP(QTMOC_HEADERS_MOC ${QTMOC_HEADERS})
ADD_DEFINITIONS(${QT_DEFINITIONS})


#
add_executable(${TARGET} ${SOURCES} ${QTMOC_HEADERS_MOC})
qt5_use_modules(${TARGET} Widgets)
set(LIBS ${LIBS} ${COMMONLIBS})
add_dependencies(${TARGET} ${LIBS})
target_link_libraries(${TARGET} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
