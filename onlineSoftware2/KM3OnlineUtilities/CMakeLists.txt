cmake_minimum_required(VERSION 2.8)

project(onlineMainScripts)

set(CMAKE_CXX_FLAGS "-std=c++11 -O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{ONLINE_DIR}/bin/)
	
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

include_directories($ENV{JPP_DIR}/software/ $ENV{AANET_DIR} $ENV{ONLINE_DIR} $ENV{BOOST_INCLUDE_PATH})

link_directories($ENV{JPP_LIB} $ENV{ONLINE_DIR}/lib/ $ENV{BOOST_LIB_PATH} $ENV{AANET_LIB})

link_libraries( -lz 
		-lKM3NeTDAQROOT 
		-levtROOT 
		-llang 
		-ltriggerROOT 
		-lkm3onlineevent 
		-laa
		-lboost_system
		-lboost_filesystem)	

add_executable(KM3OnlineFileWriter KM3OnlineFileWriter.cc)
add_executable(KM3OnlineEventViewer KM3OnlineEventViewer.cc)












