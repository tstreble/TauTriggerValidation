## script to configure the Tau Validation tool using a run range
## L. Cadamuro (LLR) , June 2016

import argparse
import os, sys
import json
from subprocess import Popen, PIPE

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Command line parser')
    parser.add_argument('--json',          dest='jsonFile',     help='json file name',                              default=None)
    parser.add_argument('--tag',           dest='tag',          help='tag defining this process',                   default=None)
    parser.add_argument('--dataset',       dest='dataset',      help='dataset to run validation on',                default="/SingleMuon/Run2016B-PromptReco-v2/MINIAOD")
    parser.add_argument('--userfilelist',  dest='userfilelist', help='user list of input files (no DAS query)',     default=None)
    parser.add_argument('--run-range',     dest='runrange',     help='first and last runs to be analyzed', nargs=2, default=None)
    args = parser.parse_args()

    ################### check of the input paramters
    if not args.runrange:
        print "** Error: please provide run range"
        sys.exit()

    ################### prepare the workspace
    if not args.tag:
        print "** Error: please provide a task tag name"
        sys.exit()
    if os.path.isdir(args.tag):
        print "** Error: folder " , args.tag , "already exists"
        sys.exit()
    os.system ('mkdir %s' % args.tag)

    ################### list all files to be processed
    filelist = []
    if args.userfilelist:
        print "... using user file list" , args.userfilelist
        try: ff = open(args.userfilelist)
        except :
            print "** Error: cannot open user file list" , args.userfilelist
            sys.exit()
        filelist = [line.strip() for line in ff]
    
    else:
        
        ### initialize user certificate
        print "... initializing proxy"
        proc = Popen ('voms-proxy-info', stdout=PIPE)
        tmp = [word for word in proc.stdout.read ().split ('\n') if 'timeleft' in word]
        if len (tmp) == 0 or int (tmp[0].split (':')[1]) < 10 : # hours
            os.system ('voms-proxy-init -voms cms')
        else: print "    > already initialized"

        ### fetch file list from DAS in this run range
        print "... retrieving file list from DAS, run range is: " , int (args.runrange[0]) , int (args.runrange[1]) , " . This could take some time"
        # NB : this version of das_client is not working in some systems, so I'm using the version from CMSSW
        # command = 'das_client --query="file dataset=%s run between [%i,%i]" --limit=0' % (args.dataset , int (args.runrange[0]) , int (args.runrange[1]) )
        command = 'python das_client.py --query="file dataset=%s run between [%i,%i]" --limit=0' % (args.dataset , int (args.runrange[0]) , int (args.runrange[1]) )
        print "    > command is: " , command
        pipe = Popen(command, shell=True, stdout=PIPE)
        for line in pipe.stdout:
            filelist.append (line.strip())
        print "    > found " , len(filelist) , " files in the run range"
    
    ################### mask the JSON (if any provided) by the run range
    skimmedJSON = {}
    if args.jsonFile:
        print "... using JSON file: " , args.jsonFile
        try: json = eval( open(args.jsonFile).read() )
        except:
            print "** Error: json" , args.jsonFile , "not a valid JSON"
            sys.exit()
        print "    > doing intersection with run range " , args.runrange[0] , args.runrange[1]
        skimmedJSON = dict ([ (key, value)  for key, value in json.iteritems() if key >= args.runrange[0] and key <= args.runrange[1] ])
        # print skimmedJSON
        if len(skimmedJSON) == 0:
            print "** Error: json " , args.jsonFile , " has no intersection with run range" , args.runrange[0] , args.runrange[1]
            sys.exit()

    ################### prepare input to each job passing as command line parameters
    # save JSON as txt
    if args.jsonFile:
        jsonName = args.tag + "/json_" + args.runrange[0] + "_" + args.runrange[1] + ".txt"
        fJSON = open (jsonName, 'w')
        fJSON.write('{')
        idx = 0
        for elem in skimmedJSON:
            fJSON.write ('\"' + elem + '\": ' + str(skimmedJSON[elem]))
            if idx == len(skimmedJSON) -1:
                fJSON.write("}")
            else:
                fJSON.write(",\n")
            idx += 1
        fJSON.close()

    # save file list to file - TBD: split into multiple file list for batch submission
    fileListFile = args.tag + "/filelist.txt"
    fFileList = open (fileListFile, 'w')
    for l in filelist:
        fFileList.write(l+'\n')
    fFileList.close()
    