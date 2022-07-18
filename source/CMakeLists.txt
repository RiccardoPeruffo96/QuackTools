cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(ACTUAL_DIR ${CMAKE_SOURCE_DIR}/source)

file(GLOB_RECURSE local_source *.cpp *.hpp)
include_directories(${ACTUAL_DIR}/include) #private interface
include_directories(${ACTUAL_DIR}/../include) #public interface

add_executable(${PROJECT_NAME} ${local_source})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

find_package(jsoncpp REQUIRED)
include_directories(${JSON_INC_PATH})

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads jsoncpp_lib ${OpenCV_LIBS} ${wxWidgets_LIBRARIES})
