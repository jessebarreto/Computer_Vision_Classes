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

cmake_minimum_required(VERSION 2.8)
project(Computer_Vision_Classes)


# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

## Find OpenCV
find_package(OpenCV REQUIRED)
set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
include_directories(SYSTEM "${OpenCV_INCLUDE_DIRS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Wunused")

## Subdirectories
add_subdirectory(Week1)
add_subdirectory(Project1)
add_subdirectory(Week2)
