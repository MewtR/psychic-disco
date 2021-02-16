cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
SET(CMAKE_C_COMPILER /usr/bin/clang)
SET(CMAKE_CXX_COMPILER /usr/bin/clang++)

project(doomaimer)
find_package(ECM REQUIRED NO_MODULE) # Add ECM
LIST(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}") # Append ecm module path to  cmake module path
find_package(XCB REQUIRED)

add_executable(doomaimer main.cpp)

target_link_libraries(doomaimer ${XCB_LIBRARIES})
target_include_directories(doomaimer PRIVATE ${XCB_INCULDE_DIRS})

set_property(TARGET doomaimer PROPERTY CXX_STANDARD 20)
set_property(TARGET doomaimer PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/..")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
