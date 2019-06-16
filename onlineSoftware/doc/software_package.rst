Software Package
================

Some description of the code for an user/mainteiner, it is not meant for a deep undertanding of the ORS package. For further details go to the `onlineSoftware KM3NeT Git repository`_ 

.. _onlineSoftware KM3NeT Git repository: https://git.km3net.de/common/onlineSoftware

Usage
-----

These are the options for the main sript to run the ORS: ``KM3OnlineMain.py``

.. argparse::
   :filename: ../onlineMainScripts/KM3OnlineMain.py
   :func: parser
   :prog: KM3OnlineMain


KM3OnlineFileWritter
********************

The application ``KM3OnlineFileWriter`` creates AANet files in a certain ouput_directory, and is organized by subdirectories with the detector ID number and each file is related to a single run::

    KM3OnlineFileWriter -H hostname:port -f output_directory -D detector_name_ORCA_or_ARCA -d debub_level     

    
This application should be ran in background mode (e.g. using the ``nohup`` command, or ``screen``).

This AANet file format is the following:


  +-----------------+--------------------+--------------------+
  | name            | Aanet object file  | type               |
  +=================+====================+====================+
  | Detector ID     | det_id             | int                |
  +-----------------+--------------------+--------------------+
  | Run Number      | run_id             | int                |
  +-----------------+--------------------+--------------------+
  | Frame Index     | frame_index        | int                |
  +-----------------+--------------------+--------------------+
  | Trigger Counter | trigger_counter    | ULong64_t          |
  +-----------------+--------------------+--------------------+
  | UTC seconds     | t                  | TTimeStamp         |
  +-----------------+--------------------+--------------------+
  | Track fit       | trks ([0])         | Trk                |
  +-----------------+--------------------+--------------------+
  | Shower fit      | trks ([1])         | Trk                |
  +-----------------+--------------------+--------------------+
  | MultiVariables  | getusr             | std::vector<double>|
  +-----------------+--------------------+--------------------+


  ``MultiVariables`` contains the variables listed in the next table. These observables are calculated for the ``KM3OnlineMultiVariablesCalculator`` class, and they subsequently build the ``KM3OnlineMultiVariables`` class. These two objects are available in this onlineSoftware. 


  +--------------+-----------------------------------------------------------------------------------+
  | name         | Description                                                                       |
  +==============+===================================================================================+
  | CoC          | center of charge of an event                                                      |
  +--------------+-----------------------------------------------------------------------------------+
  | ToT          | accumulated ToT of an event                                                       |
  +--------------+-----------------------------------------------------------------------------------+
  | ChargeAbove  |                                                                                   |
  +--------------+-----------------------------------------------------------------------------------+
  | ChargeBelow  |                                                                                   |
  +--------------+-----------------------------------------------------------------------------------+
  | ChargeRatio  |                                                                                   |
  +--------------+-----------------------------------------------------------------------------------+
  | DeltaPosZ    | center_last-center_first, last and first consider 20% of the hits sorted in time  |
  +--------------+-----------------------------------------------------------------------------------+
  | NSnapHits    | number of snapshot hits in an event                                               |
  +--------------+-----------------------------------------------------------------------------------+
  | NTrigHits    | number of triggered hits in an event                                              | 
  +--------------+-----------------------------------------------------------------------------------+

.. note::

  The main author of `AANet`_ is Aart Heijboer.

..  _AANet: http://antares.in2p3.fr/users/heijboer/aanetdox/index.html


.. note::
   File directories are organized as YYYY/MM/DD/. Those are stored in:

   * (For ORCA) @antorcaoff1.in2p3.fr machine, in the directory: /home/gmaggi/data/online

   * (For ARCA) @supernova machine, in the directory: /home/gmaggi/data/online  


Code
----

TODO
