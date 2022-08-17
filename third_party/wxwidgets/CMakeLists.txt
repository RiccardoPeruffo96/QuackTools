# check if wxWidgets is already installed in the system - using CMake's built in script FindwxWidget
find_package(wxWidgets QUIET)

#01. clone git and build it
if (wxWidgets_FOUND)
    message(STATUS "Found preinstalled wxWidgets libraries")
else()
    message(STATUS "Preinstalled wxWidgets not found")
    message(STATUS "wxWidgets will be installed...")
    
    #02. search about wxgtk3.0 prerequired (not optional)
    execute_process(COMMAND sudo apt-get install -y libwxgtk3.0-gtk3-dev COMMAND_ERROR_IS_FATAL ANY)
    
    message(STATUS "wxWidgets correctly installed")
endif()

add_library(wxWidgets_external INTERFACE)
