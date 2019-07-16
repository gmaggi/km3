cmake_minimum_required(VERSION 2.8)

project(KM3OnlineExamples)

set(CMAKE_CXX_FLAGS "-std=c++11 -O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{ONLINE_DIR}/examples/)
	
if (NOT EXISTS "$ENV{JPP_DIR}")
   message(FATAL_ERROR "Could not find JPP_DIR environment variable")
endif()

if (NOT EXISTS "$ENV{JPP_LIB}")
   message(FATAL_ERROR "Could not find JPP_LIB environment variable")
endif()

list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS)

include_directories(${ROOT_INCLUDE_DIRS})

link_libraries(${ROOT_LIBRARIES})

include_directories($ENV{JPP_DIR}/software/ $ENV{ONLINE_DIR}/)

link_directories($ENV{JPP_LIB} $ENV{ONLINE_DIR}/lib/)

link_libraries( -lz -lKM3NeTDAQROOT -levtROOT -llang -ltriggerROOT -lkm3onlineevent)	

file(GLOB files KM3Online*.cc)
foreach(file ${files})
	     string(REPLACE "$ENV{ONLINE_DIR}examples/" "" cpp_file ${file})
	     string(REPLACE ".cc" "" exec_file ${cpp_file})
	     add_executable(${exec_file} ${cpp_file})
endforeach()













