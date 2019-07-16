#!/usr/bin/env python

'''
Script inspired in https://git.km3net.de/km3py/km3mon/blob/master/scripts/online_reco.py.
This uses km3pipe framework to plot some variables calculated in the online reconstruction system.
The C++ objects are calculated and exposed to python (pybindings) in the onlineSoftware. 
The pybindings are created via boost::python.
'''

from collections import deque
from datetime import datetime, timedelta
import time
import os
import multiprocessing as mp
import numpy as np

import argparse

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

import km3pipe as kp
import km3pipe.style

import km3online
import jnet

km3pipe.style.use('km3pipe')


class KM3OnlinePlotterForMonitoring(kp.Module):
    def configure(self):
        
        self.fontsize = 16

        self.print_each_n_minutes = self.require('print_each_n_minutes')
        self.max_events           = self.require('max_events')
        self.plots_path           = self.require('plots_path')
        self.interval             = self.require('interval')
       
        server_ip   = self.require('server_ip')
        server_port = self.require('server_port')
        server      = str(server_ip)+':'+str(server_port)

        self.controlHostIterator=None
        try:
            self.controlHostIterator = jnet.JControlHostObjIt_KM3OnlineScoredEvent(server)
        except RuntimeError:
            raise RuntimeError("Socket connection failed for JControlHostObjIt_KM3OnlineEvent with ip {0} and port {1}".
                               format(server_ip,server_port))
       
        shared = mp.Manager()

        self.zeniths       = shared.list()
        self.qualities     = shared.list()
        self.nTriggerHits  = shared.list()
        self.cocs          = shared.list()
        self.tots          = shared.list()
        self.chargeRatios  = shared.list()
        self.deltaPosZs    = shared.list()
	self.scores        = shared.list()

    
        pairs = [(self.create_zenith_plot       , self.zeniths      ),
                 (self.create_quality_plot      , self.qualities    ),
                 (self.create_nTriggerHits_plot , self.nTriggerHits ),
                 (self.create_coc_plot          , self.cocs         ),
                 (self.create_tot_plot          , self.tots         ),
                 (self.create_chargeRatio_plot  , self.chargeRatios ),
                 (self.create_deltaPosZ_plot    , self.deltaPosZs   ),
                 (self.create_score_plot        , self.scores       )]


        for method, container in pairs:
            process = mp.Process(target = self.plotter,args=(method,container))
            process.start()
        

        self.counter = 0
        self.time_future = datetime.now() + timedelta( minutes = self.print_each_n_minutes )
        
    def process(self, blob):
        
        if self.controlHostIterator.hasNext():

            self.counter+=1
            onlineEvent = self.controlHostIterator.next()
            
            track          = onlineEvent.getTrack()
            multiVariables = onlineEvent.getMultiVariables()            

            if track.getStatus()==1:
                zenith = np.cos( kp.math.angle_between([0, 0, -1], [track.getDX(), track.getDY(), track.getDZ()]) )
                   
                self.zeniths.append(zenith)
                self.qualities.append( track.getQuality() )
                
            nTriggerHits = multiVariables.getNTriggeredHits() 
            if nTriggerHits != 0:
                self.nTriggerHits.append(nTriggerHits)                
                self.cocs.append( multiVariables.getCoC() )
                self.tots.append( multiVariables.getToT() )
                self.chargeRatios.append( multiVariables.getChargeRatio() )
                self.deltaPosZs.append( multiVariables.getDeltaPosZ() )

	    score = onlineEvent.getScore()
            self.scores.append(score)

        if self.time_future <= datetime.now():
            print("Processed last {0} events at (local-time) {1}".
                  format(self.counter,time.strftime("%H:%M:%S %d-%m-%Y") ) )
            self.counter=0
            self.time_future = datetime.now() + timedelta( minutes = self.print_each_n_minutes )

        return blob
    
    
    def plotter(self,method,container):
        while True:
            time.sleep(self.interval)
          
            if len(container) > self.max_events:
                boundary = len(container)-self.max_events
                for it in range(boundary): container.pop(0)

            try:  
                method(container)
            except ValueError:
                print("Exception on plotting because of the size of container in plotter function, I just pass")
                pass

    def create_quality_plot(self,container):
        n = len(container)
        
        plt.clf()
        fig, ax = plt.subplots(figsize=(16, 8))
        ax.hist(container,bins=100,histtype="step",lw=3,
                label="(last %d events)" % n)

        ax.set_title("Fit-Quality distribution. Online reconstruction system\n%s UTC" %
                     datetime.utcnow().strftime("%c"))
        ax.set_xlabel(r"Quality (JMuonGandalf)", fontsize=self.fontsize)
        ax.set_ylabel("normed count", fontsize=self.fontsize)
        ax.tick_params(labelsize=self.fontsize)
        ax.set_yscale("log") 
        plt.legend(fontsize=self.fontsize, loc=2)

        filename = os.path.join(self.plots_path, 'mm_gandalf_quality.png')
        
        plt.savefig(filename, dpi=120, bbox_inches="tight")
        plt.close()
        

    def create_zenith_plot(self,container):
        n = len(container)

        plt.clf()
        fig, ax = plt.subplots(figsize=(16, 8))
        ax.hist(container,bins=180, density=True, histtype="step",lw=3,
                label="(last %d events)" % n)

        ax.set_title("cos(zenith) distribution. Online reconstruction system\n%s UTC" %
                     datetime.utcnow().strftime("%c"))
        ax.set_xlabel(r"cos(zenith) (JMuonGandalf)", fontsize=self.fontsize)
        ax.set_ylabel("normed count", fontsize=self.fontsize)
        ax.tick_params(labelsize=self.fontsize)
        ax.set_yscale("log") 
        plt.legend(fontsize=self.fontsize, loc=2)
       
        filename = os.path.join(self.plots_path, 'mm_gandalf_zenith.png')
        plt.savefig(filename, dpi=120, bbox_inches="tight")
        plt.close()
    

    def create_nTriggerHits_plot(self,container):
        n = len(container)

        plt.clf()
        fig, ax = plt.subplots(figsize=(16, 8))
        ax.hist(container,bins=100,density=True,histtype="step",lw=3,
                label="(last %d events)" % n)

        ax.set_title("N Triggered Hits distribution. Online reconstruction system\n%s UTC" %
                     datetime.utcnow().strftime("%c"))
        ax.set_xlabel(r"N Triggered Hits (KM3OnlineMultiVariables)", fontsize=self.fontsize)
        ax.set_ylabel("normed count", fontsize=self.fontsize)
        ax.tick_params(labelsize=self.fontsize)
        ax.set_yscale("log") 
        plt.legend(fontsize=self.fontsize, loc=2)

        filename = os.path.join(self.plots_path, 'mm_multivariables_nTriggeredHits.png')
        plt.savefig(filename, dpi=120, bbox_inches="tight")
        plt.close()


    def create_coc_plot(self,container):
        n = len(container)

        plt.clf()
        fig, ax = plt.subplots(figsize=(16, 8))
        ax.hist(container,bins=100,density=True,histtype="step",lw=3,
                label="(last %d events)" % n)

        ax.set_title("CoC distribution. Online reconstruction system\n%s UTC" %
                     datetime.utcnow().strftime("%c"))
        ax.set_xlabel(r"CoC (KM3OnlineMultiVariables)", fontsize=self.fontsize)
        ax.set_ylabel("normed count", fontsize=self.fontsize)
        ax.tick_params(labelsize=self.fontsize)
        ax.set_yscale("log") 
        plt.legend(fontsize=self.fontsize, loc=2)

        filename = os.path.join(self.plots_path, 'mm_multivariables_coc.png')
        plt.savefig(filename, dpi=120, bbox_inches="tight")
        plt.close()


    def create_tot_plot(self,container):
        n = len(container)

        plt.clf()
        fig, ax = plt.subplots(figsize=(16, 8))
        ax.hist(container,bins=100,density=True,histtype="step",lw=3,
                label="(last %d events)" % n)

        ax.set_title("Tot distribution. Online reconstruction system\n%s UTC" %
                     datetime.utcnow().strftime("%c"))
        ax.set_xlabel(r"Tot (KM3OnlineMultiVariables)", fontsize=self.fontsize)
        ax.set_ylabel("normed count", fontsize=self.fontsize)
        ax.tick_params(labelsize=self.fontsize)
        ax.set_yscale("log") 
        plt.legend(fontsize=self.fontsize, loc=2)

        filename = os.path.join(self.plots_path, 'mm_multivariables_tot.png')
        plt.savefig(filename, dpi=120, bbox_inches="tight")
        plt.close()


    def create_chargeRatio_plot(self,container):
        n = len(container)

        plt.clf()
        fig, ax = plt.subplots(figsize=(16, 8))
        ax.hist(container,bins=100,density=True,histtype="step",lw=3,
                label="(last %d events)" % n)

        ax.set_title("Charge Ratio distribution. Online reconstruction system\n%s UTC" %
                     datetime.utcnow().strftime("%c"))
        ax.set_xlabel(r"Charge Ratio (KM3OnlineMultiVariables)", fontsize=self.fontsize)
        ax.set_ylabel("normed count", fontsize=self.fontsize)
        ax.tick_params(labelsize=self.fontsize)
        ax.set_yscale("log") 
        plt.legend(fontsize=self.fontsize, loc=2)

        filename = os.path.join(self.plots_path, 'mm_multivariables_chargeRatio.png')
        plt.savefig(filename, dpi=120, bbox_inches="tight")
        plt.close()


    def create_deltaPosZ_plot(self,container):
        n = len(container)

        plt.clf()
        fig, ax = plt.subplots(figsize=(16, 8))
        ax.hist(container,bins=100,density=True,histtype="step",lw=3,
                label="(last %d events)" % n)

        ax.set_title("Delta Pos Z distribution. Online reconstruction system\n%s UTC" %
                     datetime.utcnow().strftime("%c"))
        ax.set_xlabel(r"Delta Pos Z (KM3OnlineMultiVariables)", fontsize=self.fontsize)
        ax.set_ylabel("normed count", fontsize=self.fontsize)
        ax.tick_params(labelsize=self.fontsize)
        ax.set_yscale("log") 
        plt.legend(fontsize=self.fontsize, loc=2)

        filename = os.path.join(self.plots_path, 'mm_multivariables_deltaPosZ.png')
        plt.savefig(filename, dpi=120, bbox_inches="tight")
        plt.close()

    def create_score_plot(self,container):
        n = len(container)

        plt.clf()
        fig, ax = plt.subplots(figsize=(16, 8))
        ax.hist(container,bins=100, density=True, histtype="step",lw=3,
                label="(last %d events)" % n)

        ax.set_title("Classifier score distribution. Online reconstruction system\n%s UTC" %
                     datetime.utcnow().strftime("%c"))
        ax.set_xlabel(r"Score", fontsize=self.fontsize)
        ax.set_ylabel("normed count", fontsize=self.fontsize)
        ax.tick_params(labelsize=self.fontsize)
        ax.set_yscale("log")
        plt.legend(fontsize=self.fontsize, loc=2)

        filename = os.path.join(self.plots_path, 'mm_score.png')
        plt.savefig(filename, dpi=120, bbox_inches="tight")
        plt.close()


def main():

    parser = argparse.ArgumentParser(description= 'KM3OnlinePlotterForMonitoring')
    
    parser.add_argument('--server_ip',dest='SERVER_IP',type=str, 
                        default = os.path.expandvars('$ENV_DETECTOR_MANAGER_IP'),
                        help='IP of the machine that is sending data')

    parser.add_argument('--server_port',dest='SERVER_PORT',type=str,
                        default = os.path.expandvars('$ENV_MONITORING_LIGIER_PORT'),
                        help='PORT of the machine that is sending data')
    
    parser.add_argument('--plots_path',dest='PLOTS_PATH',type=str,
                        help='Directory where plots are stored')

    parser.add_argument('--max_events',dest='MAX_EVENTS',type=int,
                        default=5000,
                        help='Max number of event to plot (deque limit)')

    parser.add_argument('--print_each_n_minutes',dest='PRINT_EACH_N_MINUTES',type=int,
                        default=5,
                        help='Every n_events_debug, it will print a message: Processed last n events at time')

    parser.add_argument('--interval',dest='INTERVAL',type=int,
                        default=60,
                        help='Time interval to refresh the plots')
    
        
    args = parser.parse_args()

    pipe = kp.Pipeline()
    pipe.attach(KM3OnlinePlotterForMonitoring, 
                plots_path           = args.PLOTS_PATH,
                server_ip            = args.SERVER_IP,
                server_port          = args.SERVER_PORT,
                max_events           = args.MAX_EVENTS,
                print_each_n_minutes = args.PRINT_EACH_N_MINUTES,
                interval             = args.INTERVAL)
    pipe.drain()


if __name__ == '__main__':
    main()
