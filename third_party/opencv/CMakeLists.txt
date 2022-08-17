cmake_minimum_required(VERSION 3.15 FATAL_ERROR) #minimum version
find_package(OpenCV QUIET)

#01. check if exists
if (OpenCV_FOUND)
    message(STATUS "Found preinstalled opencv libraries")
else()
    message(STATUS "Preinstalled opencv not found")
    message(STATUS "Opencv will be installed...")
    
    #02. install from repository
    execute_process(COMMAND sudo apt-get install -y libopencv-dev python3-opencv COMMAND_ERROR_IS_FATAL ANY)
    
    message(STATUS "Opencv correctly installed")
endif()
