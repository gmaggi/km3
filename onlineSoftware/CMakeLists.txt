cmake_minimum_required(VERSION 2.8)

project(onlineSoftware)

add_subdirectory(onlineeventmanager)

add_subdirectory(onlineMainScripts)

if (NOT EXISTS "$ENV{BOOST_INCLUDE_PATH}" OR NOT EXISTS "$ENV{BOOST_LIB_PATH}")
   	message(STATUS "BOOST_INCLUDE_PATH or BOOST_LIB_PATH is not set, utilities and pybindings will not be built")	
else()
	add_subdirectory(utilities)
	add_subdirectory(pybindings)
endif()
