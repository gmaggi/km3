cmake_minimum_required(VERSION 2.8)

project(pybindings)

set(CMAKE_CXX_FLAGS "-std=c++11 -O3")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{ONLINE_DIR}/lib/)

##ROOT things
list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS)

set(PYTHON_INCLUDE /usr/include/python2.7)

include_directories(
		$ENV{BOOST_INCLUDE_PATH} 
		${PYTHON_INCLUDE} 
		$ENV{JPP_DIR}/software/ 
		$ENV{ONLINE_DIR}/ 
		${ROOT_INCLUDE_DIRS})

link_directories(
		$ENV{BOOST_LIB_PATH} 
		$ENV{JPP_LIB} 
		$ENV{ONLINE_DIR}/lib/)

add_library( jnet SHARED 
	     JControlHostObjectIteratorPy.cc 
	     JLigierObjectIteratorPy.cc 
	     JNetBoostPyModule.cc)

target_link_libraries( jnet 
		       boost_python 
		       python2.7 
		       z 
		       ${ROOT_LIBRARIES} 
		       KM3NeTDAQROOT 
		       evtROOT 
		       lang 
		       triggerROOT 
		       km3onlineevent)

add_library( km3online SHARED 
	     KM3OnlineBoostPyModule.cc 
	     KM3OnlineEvent.cc 
	     KM3OnlineTrack.cc
	     KM3OnlineMultiVariables.cc)


target_link_libraries( km3online 
		       boost_python 
		       python2.7 
		       z 
		       ${ROOT_LIBRARIES} 
		       KM3NeTDAQROOT 
		       evtROOT 
		       lang 
		       triggerROOT 
		       km3onlineevent)


set_target_properties(jnet PROPERTIES PREFIX "")	

set_target_properties(km3online PROPERTIES PREFIX "")	

