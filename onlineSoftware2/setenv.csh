#!/bin/csh

if (! $?JPP_LIB) then
    echo "ERROR: You have to source JPP first"
else
    echo "JPP_LIB found at $JPP_LIB"
    echo "Setting variables for Online Software"        

    if ( $#argv == 1 ) then
	setenv ONLINE_DIR $1
    else 
	setenv ONLINE_DIR `pwd -L`
    endif
    
    setenv ONLINE_DATA ${ONLINE_DIR}/data/ 

    setenv LD_LIBRARY_PATH ${ONLINE_DIR}/lib/:$LD_LIBRARY_PATH

    if (! $?BOOST_LIB_PATH) then
	echo "BOOST_LIB_PATH not found; pybinding neither utilities can be built"
    else
	setenv LD_LIBRARY_PATH ${BOOST_LIB_PATH}:$LD_LIBRARY_PATH
    endif

    chmod +x ${ONLINE_DIR}/KM3OnlineMains/KM3OnlineMain.py
    chmod +x ${ONLINE_DIR}/KM3OnlineUtilities/KM3OnlinePlotterForMonitoring.py

    ln -sf ${ONLINE_DIR}/KM3OnlineMains/KM3OnlineMain.py ${ONLINE_DIR}/bin/KM3OnlineMain.py
    ln -sf ${ONLINE_DIR}/KM3OnlineUtilities/KM3OnlinePlotterForMonitoring.py ${ONLINE_DIR}/bin/KM3OnlinePlotterForMonitoring.py

    setenv PATH ${ONLINE_DIR}/bin:$PATH
    setenv PYTHONPATH ${ONLINE_DIR}/lib:$PYTHONPATH
    setenv PYTHONPATH ${ONLINE_DIR}:$PYTHONPATH
    
    setenv ONLINE_RECO_VERSION `cd $ONLINE_DIR; git rev-parse --short HEAD` 
    setenv JPP_VERSION `cd $JPP_DIR; git rev-parse --short HEAD` 
endif


