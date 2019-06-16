import argparse, os

try:
    import jnet, km3online
except ImportError:
    raise ImportError("check that jnet and km3online are in $ONLINE_DIR/lib, " 
                      "They should be created by doing make in the pybindings directory")
   
parser = argparse.ArgumentParser(description= 'KM3OnlineRead.py')

parser.add_argument('--host_name',dest='HOSTNAME',type=str,
                    help='ip:port for host name')
                    
args = parser.parse_args()


if __name__=="__main__":

    try:
        controlHost = jnet.JControlHostObjectIterator(args.HOSTNAME)
    except RuntimeError:
        print "Socket connection failed"
        raise 

    
    while controlHost.hasNext():
        event = controlHost.next()

        track  = event.getRecoTrack()

        print(track)

        
