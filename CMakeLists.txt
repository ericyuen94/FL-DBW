#go down and compile subdirectories

#specify Sub Directories here
set(SUBDIR_TO_BUILD 
BenchTestSteerEncoder
#BenchTestStkciPCMCard
#BenchTestPCMCard
BenchTestEncoder
ForkDebugger
#BenchTestPCanPlayer
#BenchTestSteerCalibPlayer
#BenchTestPlatformStacker
QTVcmTest
DriveByWire
TeleOpsJoystick
PIM
VCM
DBWCommon
#BenchTestPlatformBydFl
#QTCalibSteerMotorCurvature
QTSMCtrl
#QTVcmTrajectory
#BenchTestPlatformForklift
#BenchTestStkciVecowPCMCard
SoftwareEStop
ProbotDBWInterfaceV3
)

foreach(subdir_to_build ${SUBDIR_TO_BUILD})
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir_to_build})
        add_subdirectory(${subdir_to_build})
    endif()
endforeach(subdir_to_build)
