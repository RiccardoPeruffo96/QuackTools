cmake_minimum_required(VERSION 3.15 FATAL_ERROR) #minimum version
find_package(OpenCV QUIET)

#01. check if exists
if (OpenCV_FOUND)
    message(STATUS "Found preinstalled opencv libraries")
else()
    message(STATUS "Preinstalled opencv not found")
    message(STATUS "Opencv will be installed...")
    
    #02. install from repository
    execute_process(COMMAND sudo apt update --fix-missing)
    message(STATUS "Searching opencv in public repository...")
    execute_process(COMMAND sudo apt install -y libopencv-dev)

    #03. clone the git then install it if repository doesn't work
    if (NOT OpenCV_FOUND)
      message(STATUS "Installing opencv from repository has some problem...")
      message(STATUS "Install opencv from git...")
      execute_process(COMMAND rm -r -f ${CMAKE_CURRENT_SOURCE_DIR}/build)
      execute_process(COMMAND mkdir ${CMAKE_CURRENT_SOURCE_DIR}/build)
      execute_process(COMMAND chmod +x ../script/install_opencv.sh)
      execute_process(COMMAND ./../script/install_opencv.sh ${CMAKE_CURRENT_SOURCE_DIR}/build)
    endif()
    
    message(STATUS "Opencv correctly installed")
endif()
