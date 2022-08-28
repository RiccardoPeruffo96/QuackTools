cmake_minimum_required(VERSION 3.15 FATAL_ERROR) #minimum version
find_package(jsoncpp QUIET)

#01. clone git and build it
if (jsoncpp_FOUND)
    message(STATUS "Found preinstalled jsoncpp libraries")
else()
    message(STATUS "Preinstalled jsoncpp not found")
    message(STATUS "jsoncpp will be installed...")

    #03. clone the git then install it
    execute_process(COMMAND rm -r -f ${CMAKE_CURRENT_SOURCE_DIR}/build)
    execute_process(COMMAND mkdir ${CMAKE_CURRENT_SOURCE_DIR}/build)
    execute_process(COMMAND chmod +x ../script/install_jsoncpp.sh)
    execute_process(COMMAND ./../script/install_jsoncpp.sh ${CMAKE_CURRENT_SOURCE_DIR}/build)
    
    message(STATUS "jsoncpp correctly installed")

endif()
