# Install script for directory: /proj/pallet/Tatooine/branch-forklift/component/DBW

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/BenchTestEncoder/cmake_install.cmake")
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/ForkDebugger/cmake_install.cmake")
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/QTVcmTest/cmake_install.cmake")
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/DriveByWire/cmake_install.cmake")
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/TeleOpsJoystick/cmake_install.cmake")
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/PIM/cmake_install.cmake")
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/VCM/cmake_install.cmake")
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/DBWCommon/cmake_install.cmake")
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/QTSMCtrl/cmake_install.cmake")
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/SoftwareEStop/cmake_install.cmake")
  include("/proj/pallet/Tatooine/branch-forklift/component/DBW/ProbotDBWInterfaceV3/cmake_install.cmake")

endif()

