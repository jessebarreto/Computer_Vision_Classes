#######################################################################
#
# THIS FILE MAKES THE COMPILATION OF CODE DEVELOPED IN THIS SEMESTER AUTOMATIC.
# BY jesseh.barreto@gmail.com
#
#
# TO USE JUST ADD A NEW SUB_DIRECTORY WITH A CMAKELIST.TXT INSIDE.
# WITH:
#       add_executable(bin_name source_code)
#       target_link_libraries(bin_name libraries)
#
#######################################################################

project(Computer_Vision_Classes)
cmake_minimum_required(VERSION 2.8)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Find OpenCV
find_package(OpenCV REQUIRED core imgproc highgui)
set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
include_directories(SYSTEM "${OpenCV_INCLUDE_DIRS}")

# Subdirectories
add_subdirectory(Week1)
add_subdirectory(Week2)
