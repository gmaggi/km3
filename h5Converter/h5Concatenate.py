#!/usr/bin/python 

import subprocess as sp
import os,argparse, glob 
import tables

parser = argparse.ArgumentParser(description= 'Concatenate H5 files')

parser.add_argument('--indir',type=str,dest='INDIR',
                    help='DIRECTORY WHERE H5 FILES LIVE')

parser.add_argument('--outfile',type=str,dest='OUTFILE',
                    help='DIRECTORY TO LOCATE OUPUT Concatenated H5 FILE')

parser.add_argument('--file_flag',type=str,dest='FILEFLAG',
                    help='START name of the files')


args=parser.parse_args()


if __name__=='__main__':
    print ''
    inputFiles = sorted ( glob.glob( os.path.join(args.INDIR, '*'+args.FILEFLAG+'*.h5') ) )
   
    print 'Initializing concatenated file with : ',inputFiles[0]
    tables.copy_file(inputFiles[0], args.OUTFILE)
    
    motherFile   = tables.open_file(args.OUTFILE, mode='r')
    motherStrGroups = motherFile.root.__members__
    motherFile.close()
   
    concatenatedFile = tables.open_file(args.OUTFILE, mode='a')
    concatenatedFile.title = 'ConcatenatedFile with Nfile: {0} ; InDir: {1} ; FileFlag: {2} ; OutFile: {3} ; WorkDir: {4}'\
                    .format(len(inputFiles),args.INDIR,args.FILEFLAG,args.OUTFILE,os.getcwd())
    
    for inputStrFile in inputFiles[1:]:
        print 'Concatenating File : ,',inputStrFile
        inputFile = tables.open_file(inputStrFile,'r')

        for motherStrGroup in motherStrGroups:
            if motherStrGroup in ['FileInfo','file_info','meta']: continue
    
            concGroup     = concatenatedFile.root.__getattr__(motherStrGroup)
            concStrTables = concGroup.__members__
            
            inGroup       = inputFile.root.__getattr__(motherStrGroup)    

            for concStrTable in concStrTables:
                concTable = concGroup.__getattr__(concStrTable)
                inTable = inGroup.__getattr__(concStrTable)

                concTable.append( inTable.read() )
                
    
        inputFile.close()

    print ''
    print 'Output {0} file contains: '.format(args.OUTFILE)
    print ''
    print concatenatedFile       
                
    concatenatedFile.close()
    
    
