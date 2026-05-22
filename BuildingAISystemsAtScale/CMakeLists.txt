cmake_minimum_required(VERSION 4.3.2)

project(AutonomousVehicleSensorFusionPipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AutonomousVehicleSensorFusionPipeline main.cpp)

include(GNUInstallDirs)
install(TARGETS AutonomousVehicleSensorFusionPipeline
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
