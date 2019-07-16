import argparse, os
import numpy as np
try:
    import jnet, km3online
except ImportError:
    raise ImportError("check that jnet and km3online are in $ONLINE_DIR/lib, " 
                      "They should be created during the cmake process in the pybindings directory")
   
from km3online_physics.clfc_functions import get_score
from sklearn.externals import joblib
import pandas as pd


parser = argparse.ArgumentParser(description= 'KM3OnlineBuildKM3OnlineScoredEvent.py')

parser.add_argument('--host_name',dest='HOSTNAME',type=str,
                    help='ip:port for host name')

parser.add_argument('--listener',dest='LISTENER',type=str,
                    help='ip:port for listener')


clfc_up  = joblib.load(os.path.expandvars(("$ONLINE_DATA/no_simplexdz_ORCA7_HE_50GeV_5TeV_classifier_up.joblib")))
clfc_nu = joblib.load(os.path.expandvars(("$ONLINE_DATA/no_simplexdz_ORCA7_HE_50GeV_5TeV_classifier_nu_after_cut_clf_up_score_0.9.joblib")))
            
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

        track    = km3OnlineEvent.getTrack()
	if track.getStatus()!=1 or track.getNDF()==0:
	    #print "Status != 1 or bestmuon_ndf == 0, so skip event."
	    continue
        multiVar = km3OnlineEvent.getMultiVariables()

        bestmuon_dz      = track.getDZ()
        bestmuon_z       = track.getZ()
        bestmuon_quality = track.getQuality()
        bestmuon_ndf     = track.getNDF()
        Q_div_ndf        = bestmuon_quality/bestmuon_ndf
        coc              = multiVar.getCoC()
        tot              = multiVar.getToT()
        charge_above     = multiVar.getChargeAbove()
        charge_below     = multiVar.getChargeBelow()
        charge_ratio     = multiVar.getChargeRatio()
        deltapos_z       = multiVar.getDeltaPosZ()
        n_triggHits      = multiVar.getNTriggeredHits()


        feature_dict = {
            'charge_ratio': np.array([charge_ratio]),
            'charge_above': np.array([charge_above]),
            'charge_below': np.array([charge_below]),
            'bestmuon_dz': np.array([bestmuon_dz]),
            'deltapos_z': np.array([deltapos_z]),
            'bestmuon_quality': np.array([bestmuon_quality]),
            'bestmuon_z': np.array([bestmuon_z]),
            'coc': np.array([coc]),
            'n_triggHits': np.array([n_triggHits]),
            'tot': np.array([tot]),
            'Q_div_ndf': np.array([Q_div_ndf])
            }

        data_for_clf_up = pd.DataFrame(data = feature_dict, columns = feature_dict.keys())
	clf_up_score = get_score(data_for_clf_up, clfc_up)
        
        feature_dict['clf_up_score'] = clf_up_score
        data_for_clf_nu = pd.DataFrame(data = feature_dict, columns = feature_dict.keys())
        clf_nu_score = get_score(data_for_clf_nu, clfc_nu)

        score = np.asscalar(clf_nu_score)
        scoredEvent = km3online.KM3OnlineScoredEvent(km3OnlineEvent,score)
        print(scoredEvent)

        out_ControlHost.put(scoredEvent)
