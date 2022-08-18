# check if wxWidgets is already installed in the system - using CMake's built in script FindwxWidget
cmake_minimum_required(VERSION 3.15 FATAL_ERROR) #minimum version

find_package(wxWidgets QUIET)

#01. clone git and build it
if (wxWidgets_FOUND)
    message(STATUS "Found preinstalled wxWidgets libraries")
else()
    message(STATUS "Preinstalled wxWidgets not found")
    message(STATUS "wxWidgets will be installed...")
    
    #02. search about gtk+-3.0 prerequired (not optional)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 QUIET gtk+-3.0)

    if(GTK3_FOUND)
      message(STATUS "Found preinstalled GTK3, now we'll start with wxWidgets...")
    else()
      message(STATUS "Preinstalled GTK3 not found")
      message(STATUS "GTK3 will be installed...")
      execute_process(COMMAND sudo apt update --fix-missing)
      execute_process(COMMAND sudo apt install -y libgtk-3-dev)
      message(STATUS "GTK3 installed, now we'll start with wxWidgets...")
    endif()
    
    #03. clone the git then install it
    message(STATUS "Installing wxWidgets from git...")
    execute_process(COMMAND rm -r -f ${CMAKE_CURRENT_SOURCE_DIR}/build)
    execute_process(COMMAND mkdir ${CMAKE_CURRENT_SOURCE_DIR}/build)
    execute_process(COMMAND chmod +x ../script/install_wxgtk3.sh)
    execute_process(COMMAND ./../script/install_wxgtk3.sh ${CMAKE_CURRENT_SOURCE_DIR}/build)
    
    message(STATUS "wxWidgets correctly installed")
endif()

add_library(wxWidgets_external INTERFACE)
