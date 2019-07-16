#!/usr/bin/python

import socket
import subprocess as sp, time
import argparse, os, time
import logging

parser = argparse.ArgumentParser(description= 'Handle online reconstructions')

parser.add_argument('--in_hostname',dest='IN_HOSTNAME',type=str,
                    default='127.0.0.1:5554',
                    help='IP of the machine that is sending data, default CPPM machine: 127.0.0.1:5554')

parser.add_argument('--destination_hostname',dest='DESTINATION_HOSTNAME',type=str,
                    default='localhost',
                    help='IP of the machine where jobs will be distributed, default localhost')

parser.add_argument('--destination_ports',dest='DESTINATION_PORTS',nargs='+',
                    default=['5551'],
                    help='Ports where jobs(recos) will be distributed')

parser.add_argument('--listener',dest='LISTENER',type=str,
                    default='localhost:5554',
                    help='hostname:port which performs as a server')

parser.add_argument('--run_local_listener', dest='RUNLOCALLISTENER', 
                    action='store_true',
                    help="Run local ligier as a listener, under the port given in --listener")

parser.add_argument('--detector_file',dest='DETECTORFILE',type=str,
                    default=os.path.expandvars('$JPP_DATA/')+'KM3NeT_-00000001_20171212.detx',
                    help='Detector file, default in JPP_DATA for MC tests ')

parser.add_argument('--shower_pdf',type=str,dest='SHOWERPDF',
                    default= os.path.expandvars('$JPP_DATA/')+'genhen_JUNE_2___.hist.root',
                    help='Input dir where AANet pdf table are stored, default in JPP_DATA')

parser.add_argument('--track_pdf',type=str,dest='TRACKPDF',
                    default= os.path.expandvars('$JPP_DATA/'),
                    help='Input dir where JPP pdf tables are stored, defaut: JPP_DATA')

parser.add_argument('--muon_parameters',type=str,dest='MUONPARAMETERS',
                    default= os.path.expandvars('$JPP_DATA/')+'muonReconstruction_parameters_orca.txt',
                    help='Muon reco parameters as .txt file')

parser.add_argument('--n_ports', type=int, dest='N_PORTS',
                    default=2,
                    help="Max number of ports")

parser.add_argument('--debug_level', type=int, dest='DEBUGLEVEL',
                    default=1,
                    help="Debug level for all the applications")
                    

def is_port_busy(hostname_port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    if ':' in hostname_port:
        port     = hostname_port.split(':')[1]
        hostname = hostname_port.split(':')[0]
    else:
        port     = hostname_port
        hostname = 'localhost'

    try:
        port=int(port)
    except ValueError:
        print "can not cast port: {0} to int".format(port)
        raise
        
    
    s.bind((hostname, port))
    
    return (hostname,port)


if __name__=="__main__":

    if not "JPP_DATA" in os.environ:
        raise OSError('Can not find JPP_DATA')
        
    if not "ONLINE_DIR" in os.environ:
        raise OSError('Can not find ONLINE_DIR')

    args = parser.parse_args()

    TRACKPDF=args.TRACKPDF+'J%p.dat'



    if args.RUNLOCALLISTENER:
        try:
            listener_port = is_port_busy(args.LISTENER)[1]
        except socket.error:
            raise IOError('the port at {0} is busy and/or the host address is wrong. I stop'.format(args.LISTENER))
    
        local_ligier='nohup JLigier -P {0} -T {1} -d {2} &'.format(listener_port,3000,args.DEBUGLEVEL)
        sp.check_call(local_ligier,shell=True)
        print "Local JLigier is set under port ", listener_port
        time.sleep(5)

    
    destinations=''
    mainOnlineCommand  = ['KM3OnlineRecos',
                          '-P','PORT','-L',args.LISTENER,
                          '-d',str(args.DEBUGLEVEL),
                          '-a',args.DETECTORFILE,
                          '-A',args.SHOWERPDF,
                          '-p',TRACKPDF,
                          '-@',args.MUONPARAMETERS]
                          

    distributer        = os.path.expandvars('$JPP_DIR/examples/JNet/JDistributer')
    if not os.path.exists(distributer):
        raise IOError("File {0} does not exist, you might need to do make in JPP_DIR/examples/JNet".format(distributer))

    distributerCommand = [distributer,
                          '-H',args.IN_HOSTNAME,
                          '-d',str(args.DEBUGLEVEL),
                          '-D']


    ports=args.DESTINATION_PORTS
    n_freePorts=0
    for port in ports:
        print '------> PORT: ',port

        try:
            is_port_busy(port)
            n_freePorts+=1
        except socket.error:
            newPort = int(port)+1
            logging.warning("Port {0} is busy, I skip this and add a new one: {1}".format(port,newPort))
            ports.append(str(newPort))
            continue

        if n_freePorts<args.N_PORTS: 
            newPort = int(port)+1
            ports.append(str(newPort))

        command = [port if e=='PORT' else e for e in mainOnlineCommand] 
        sp.check_call(' '.join(command)+' &', shell=True)
        time.sleep(10)
        destinations +=  args.DESTINATION_HOSTNAME+':'+port+';' 


    destinations = '"'+destinations[0:-1]+'"'
    distributerCommand.append(destinations)

    p = sp.Popen(' '.join(distributerCommand),shell=True, stderr=sp.PIPE)
    p.communicate()
        
         
        
    

