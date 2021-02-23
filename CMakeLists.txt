cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
SET(CMAKE_C_COMPILER /usr/bin/clang)
SET(CMAKE_CXX_COMPILER /usr/bin/clang++)
SET(CMAKE_CXX_FLAGS "-g -O0")
project(doomaimer)
find_package(ECM REQUIRED NO_MODULE) # Add ECM
LIST(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}") # Append ecm module path to  cmake module path
find_package(XCB REQUIRED COMPONENTS XCB XFIXES COMPOSITE) # Add XCB

find_package(ImageMagick COMPONENTS Magick++)
find_program(MAGICK_CONFIG "Magick++-config")

# Set MAGICKCORE_QUANTUM_DEPTH and MAGICKCORE_HDRI_ENABLE according to what is suggested by
# Magick++-config --cxxflags (avoids the warning)
execute_process(COMMAND "${MAGICK_CONFIG}" "--cxxflags" OUTPUT_VARIABLE MAGICK_CXX_FLAGS)
# Might as well get the libs while we're at it
execute_process(COMMAND "${MAGICK_CONFIG}" "--libs" OUTPUT_VARIABLE MAGICK_LD_FLAGS)
# Remove trailing whitespace
string(STRIP "${MAGICK_CXX_FLAGS}" MAGICK_CXX_FLAGS)
string(STRIP "${MAGICK_LD_FLAGS}" MAGICK_LD_FLAGS)

# Append to compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MAGICK_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MAGICK_LD_FLAGS}")

add_executable(doomaimer main.cpp xcb_functions.cpp)

target_link_libraries(doomaimer ${XCB_LIBRARIES}) #${ImageMagick_LIBRARIES})
target_include_directories(doomaimer PRIVATE ${XCB_INCULDE_DIRS}) #${ImageMagick_INCLUDE_DIRS})

set_property(TARGET doomaimer PROPERTY CXX_STANDARD 20)
set_property(TARGET doomaimer PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/..")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
