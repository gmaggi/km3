#!/bin/zsh

if [ -z "$JPP_LIB" ]; then
    echo "ERROR: You have to source JPP first"
else
    echo "JPP_LIB found at $JPP_LIB"
    echo "Setting variables for Online Software"        

    if [ $# -eq 1 ]; then
	export ONLINE_DIR=$1
    else 
	export ONLINE_DIR=`pwd -L`
    fi
    
    export ONLINE_DATA=${ONLINE_DIR}/data/ 

    export LD_LIBRARY_PATH=${ONLINE_DIR}/lib/:$LD_LIBRARY_PATH

    if [ -z "$BOOST_LIB_PATH" ]; then
	echo "BOOST_LIB_PATH not found; pybinding neither utilities can be built"
    else
	export LD_LIBRARY_PATH=$BOOST_LIB_PATH:$LD_LIBRARY_PATH
    fi

    chmod +x ${ONLINE_DIR}/KM3OnlineMains/KM3OnlineMain.py
    chmod +x ${ONLINE_DIR}/KM3OnlineUtilities/KM3OnlinePlotterForMonitoring.py

    ln -sf ${ONLINE_DIR}/KM3OnlineMains/KM3OnlineMain.py ${ONLINE_DIR}/bin/KM3OnlineMain.py 
    ln -sf ${ONLINE_DIR}/KM3OnlineUtilities/KM3OnlinePlotterForMonitoring.py ${ONLINE_DIR}/bin/KM3OnlinePlotterForMonitoring.py 

    export PATH=${ONLINE_DIR}/bin:$PATH
    export PYTHONPATH=${ONLINE_DIR}/lib:$PYTHONPATH
    export PYTHONPATH=${ONLINE_DIR}:$PYTHONPATH
    
    export ONLINE_RECO_VERSION=`cd $ONLINE_DIR; git rev-parse --short HEAD` 
    export JPP_VERSION=`cd $JPP_DIR; git rev-parse --short HEAD` 
fi


