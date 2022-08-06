cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(ACTUAL_DIR ${CMAKE_SOURCE_DIR}/source)

file(GLOB_RECURSE local_source *.cpp *.hpp)
include_directories(${ACTUAL_DIR}/include) #private interface
include_directories(${ACTUAL_DIR}/../include) #public interface

add_executable(${PROJECT_NAME} ${local_source})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads ${JSONCPP_LIBRARIES} ${OpenCV_LIBS} ${wxWidgets_LIBRARIES} gcov)
