import argparse
import sys

parser = argparse.ArgumentParser(description='Command line parser')
parser.add_argument('--json', dest='jsonFile', help='json file name', default=None)
parser.add_argument('--outfile', dest='outFile', help='output file name', default=None)
args = parser.parse_args()

if not args.jsonFile:
    print "** Error: need a JSON in input"
    sys.exit()

try:
    json = eval( open(args.jsonFile).read() )
except:
    print "** Error: " , args.jsonFile , " not a valid JSON"
    sys.exit()

print 'process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange( *('
for run,lumisections in sorted(json.iteritems()):
    for ls in lumisections:
        print "    '%s:%s-%s:%s'," % (run, ls[0], run, ls[1])
print '))'

if args.outFile:
    fOut = open (args.outFile, 'w')
    fOut.write('process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange( *(\n')
    for run,lumisections in sorted(json.iteritems()):
        for ls in lumisections:
            fOut.write("    '%s:%s-%s:%s',\n" % (run, ls[0], run, ls[1]))
    fOut.write('))\n')