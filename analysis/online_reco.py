#!/usr/bin/python

import subprocess as sp
import os,argparse, glob, sys 

parser = argparse.ArgumentParser(description= 'Launch several jobs (online) via qsub command')


parser.add_argument('--indir',type=str,dest='INDIR',
                    help='DIRECTORY WHERE FILES LIVE')

parser.add_argument('--outdir',type=str,dest='OUTDIR',
                    help='DIRECTORY TO LOCATE OUPUT FILES')

parser.add_argument('--file_flag',type=str,dest='FILEFLAG',
                    help='START name of the files')

parser.add_argument('--skip_file_flag',type=str,dest='SKIPFILEFLAG',
                    default=None,
                    help='Skip files with this flag')

parser.add_argument('--nfiles',type=int,dest='NFILES',
                    default= None,
                    help='Number of files, default full sample in input directory')


parser.add_argument('--detector_file',type=str,dest='DETECTORFILE',
                    default=None,
                    help='input DETECTORFILE')

parser.add_argument('--detector',type=str,dest='DETECTOR',
                    default=None, 
                    help='detector type; ARCA or ORCA')


parser.add_argument('--aanet_light_tables',type=str,dest='AANETLIGHTTABLES',
                    default= '/sps/km3net/users/gmaggi/data/arca/genhen_JUNE_2___.hist.root',
                    help='Input dir where AANet pdf table are stored')

if not 'JPP_DATA' in os.environ: raise OSError('Can not find JPP_DATA')

parser.add_argument('--jpp_light_tables',type=str,dest='JPPLIGHTTABLES',
                    default= os.path.expandvars('$JPP_DATA/')+'J%p.dat',
                    help='Input dir where JPP pdf tables are stored, defaut: JPP_DATA')

parser.add_argument('--muon_params',type=str,dest='MUONPARAMS',
                    help='reco Muon parameters in .txt format, defaut in: JPP_DATA')

parser.add_argument('--keep_label_number', dest='KEEPLABELNUMBER', action='store_true')


args=parser.parse_args()


#################
#### sentinel
#################

sentinel=args.OUTDIR+'/scriptCommand.txt'
if os.path.exists(sentinel):
    raise IOError('file {0} already exists'.format(sentinel))

scriptCommand = open(sentinel,'w')
scriptCommand.write('online_reco.py '+' '.join(sys.argv[1:])+'\n\n')
for k,v in vars(args).items():
    scriptCommand.write(k+' '+str(v)+'\n')
scriptCommand.close()

################

def _make_outFile_name(inFile):
    if args.KEEPLABELNUMBER:
        return inFile.replace('.root','.online_reco.h5')

    fileNumber = inFile.split('.')[-2]

    try: int(fileNumber)
    except ValueError: print("can not cast inFile.split('.')[-2]")

    n_digits=5
    if len(fileNumber) < n_digits:
        for d in range( n_digits-len(fileNumber) ): fileNumber='0'+fileNumber
    elif len(fileNumber) == n_digits:
        fileNumber=fileNumber
    else:
        raise ValueError("bad number label in file {0}".format(inFile))

    preOutput = inFile.split('.')[0:-2]

    output    = '.'.join(preOutput)+'.'+'online_reco'+'.'+fileNumber+'.h5'

    return output


def make_online_reco():
    if not 'ANALYSIS_DIR' in os.environ: 
        raise OSError('Can not find ANALYSIS_DIR')    

    if not 'H5_CPP_CONVERTER_DIR' in os.environ:
        raise OSError('Can not find H5_CPP_CONVERTER_DIR')        

    if args.DETECTORFILE==None: raise ValueError('set detector_file parser arg')        
    if args.DETECTOR==None    : raise ValueError('set detector parser arg')

    filesInDir = glob.glob( os.path.join(args.INDIR, '*'+args.FILEFLAG+'*.root') )

    if args.SKIPFILEFLAG!=None:
        inputFiles = filter( lambda l : not args.SKIPFILEFLAG in l, filesInDir ) 
    else:
        inputFiles = filesInDir

    if not args.KEEPLABELNUMBER:
        inputFiles.sort(key=lambda f: int(f.split('.')[-2]))

    print 'Number of files with flag {0} in directory {1} is {2} '.format(args.FILEFLAG,
                                                                          args.INDIR,
                                                                          len(inputFiles)) 
    
    
    if args.DETECTOR=='ARCA':
        onlineExecI = 'OffLineMain'+' -a {0} -P {1}  -A {2} -@ {3}'.format(args.DETECTORFILE,
                                                                           args.JPPLIGHTTABLES,
                                                                           args.AANETLIGHTTABLES,
                                                                           args.MUONPARAMS)
    elif args.DETECTOR=='ORCA':
        onlineExecI = 'OffLineMain'+' -a {0} -P {1} -A {2} -@ {3}'.format(args.DETECTORFILE,
                                                                          args.JPPLIGHTTABLES,
                                                                          args.AANETLIGHTTABLES,
                                                                          args.MUONPARAMS)
        
    jobDir   = args.OUTDIR+'/jobs/'
    if not os.path.isdir(jobDir): os.mkdir(jobDir)
    errorDir = args.OUTDIR+'/errors/'
    if not os.path.isdir(errorDir): os.mkdir(errorDir)
    outDir   = args.OUTDIR+'/outs/'
    if not os.path.isdir(outDir): os.mkdir(outDir)

    n_files=0
    for inPath in inputFiles:
        print 'Reading Input ',inPath
        inFile = inPath.split('/')[-1]
        
        outSh  = jobDir+inFile.replace('.root','.sh')
        sh = open(outSh,'w')
        sh.write('#!/bin/bash'+'\n')

        onlineOutFile = args.OUTDIR+'/'+_make_outFile_name(inFile)      
        onlineExecII=' -f '+inPath+' -o '+ onlineOutFile
        onlineExec = onlineExecI+onlineExecII
        sh.write('lscpu'+'\n')
        sh.write(onlineExec+'\n')
        
        sh.close()
        sp.check_call(['chmod','+x',outSh])
        
        outError = errorDir+inFile.replace('.root','.error.txt')
        error = open(outError,'w')      
        error.close()

        outOut = outDir+inFile.replace('.root','.out.txt')
        out = open(outOut,'w')      
        out.close()

        sp.check_call(['qsub','-e',outError,'-o',outOut,outSh])

        del outSh
        del outError
        del outOut

        n_files+=1
        if args.NFILES==n_files: break


if __name__=='__main__':
    make_online_reco()
    
    
