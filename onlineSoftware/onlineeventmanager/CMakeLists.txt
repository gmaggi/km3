cmake_minimum_required(VERSION 2.8)
	
project(onlineeventmanager)

set(CMAKE_CXX_FLAGS "-std=c++11 -O3")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{ONLINE_DIR}/lib/)

	
if (EXISTS "$ENV{JPP_DIR}")
   include_directories($ENV{JPP_DIR}/software/)
else()
   message(FATAL_ERROR "Could not find JPP_DIR environment variable")
endif()

if (EXISTS "$ENV{JPP_LIB}")
   link_directories($ENV{JPP_LIB})
   link_libraries(-lKM3NeTDAQROOT)
else()
   message(FATAL_ERROR "Could not find JPP_LIB environment variable")
endif()


### ROOT stuff

if (NOT EXISTS "$ENV{ROOTSYS}")
   message(FATAL_ERROR "Could not find ROOTSYS environment variable")
endif()

## Provides FindROOT.cmake
list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)

## Set the path of the header files of this project to produce the ROOT dictionaries
list(APPEND CMAKE_PREFIX_PATH $ENV{ONLINE_DIR}/onlineeventmanager)

## Defines ROOT_GENERATE_DICTIONARY and ROOT_INCLUDE_DIRS
find_package(ROOT REQUIRED COMPONENTS)

## Include ROOT headers
include_directories(${ROOT_INCLUDE_DIRS})

ROOT_GENERATE_DICTIONARY(KM3OnlineGraphicROOTDict KM3OnlineGraphic.hh KM3OnlineEvent.hh LINKDEF LinkDef.hh)


add_library(km3onlineevent SHARED
			   KM3OnlineGraphic.cc
			   KM3OnlineEvent.cc
			   KM3OnlineGraphicROOTDict)
			  
	    











