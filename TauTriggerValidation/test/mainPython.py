import FWCore.ParameterSet.VarParsing as VarParsing
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Config as cms

process = cms.Process("TagAndProbe")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff") 

from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v8'
process.load('TauTriggerValidation.TauTriggerValidation.tagAndProbe_cff')

### cmd line options

options = VarParsing.VarParsing ('analysis')
# options.register ('skipEvents',
#                   -1, # default value
#                   VarParsing.VarParsing.multiplicity.singleton, # singleton or list
#                   VarParsing.VarParsing.varType.int,          # string, int, or float
#                   "Number of events to skip")
options.register ('JSONfile',
                  "", # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,          # string, int, or float
                  "JSON file (empty for no JSON)")
options.outputFile = 'NtupleTrigValid.root'
# options.maxEvents = -999
options.inputFiles = []
options.parseArguments()


process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.inputFiles)
)

if options.JSONfile:
    print "Using JSON: " , options.JSONfile
    process.source.lumisToProcess = LumiList.LumiList(filename = options.JSONfile).getVLuminosityBlockRange()

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# if options.maxEvents >= -1:
#     process.maxEvents.input = cms.untracked.int32(options.maxEvents)
# if options.skipEvents >= 0:
#     process.source.skipEvents = cms.untracked.uint32(options.skipEvents)


process.p = cms.Path(
    process.TAndPseq
)

# Silence output
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 5000

# Adding ntuplizer
process.TFileService=cms.Service('TFileService',fileName=cms.string(options.outputFile))
