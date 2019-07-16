import argparse, os

try:
    import jnet, km3online
except ImportError:
    raise ImportError("check that jnet and km3online are in $ONLINE_DIR/lib, " 
                      "They should be created during the cmake process in the pybindings directory")
   
parser = argparse.ArgumentParser(description= 'KM3OnlineBuildKM3OnlineScoredEvent.py')

parser.add_argument('--host_name',dest='HOSTNAME',type=str,
                    help='ip:port for host name')

parser.add_argument('--listener',dest='LISTENER',type=str,
                    help='ip:port for listener')

                    
args = parser.parse_args()


if __name__=="__main__":

    try:
        in_ControlHost = jnet.JControlHostObjIt_KM3OnlineEvent(args.HOSTNAME)
    except RuntimeError:
        raise RuntimeError("Socket connection at {0} failed".format(args.HOSTNAME))

    try:
        out_ControlHost = jnet.JControlHostObjOut_KM3OnlineScoredEvent(args.LISTENER)
    except RuntimeError:
        raise RuntimeError("Socket connection at {0} failed".format(args.LISTENER))
    
    
    while in_ControlHost.hasNext():
        km3OnlineEvent = in_ControlHost.next()

        any_score=0.456
        scoredEvent = km3online.KM3OnlineScoredEvent(km3OnlineEvent,any_score)
        print(scoredEvent)

        out_ControlHost.put(scoredEvent)
        
