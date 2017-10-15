import FWCore.ParameterSet.VarParsing as VarParsing
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Config as cms
process = cms.Process("TagAndProbe")

isMC = True
useGenMatch = True



process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

#### handling of cms line options for tier3 submission
#### the following are dummy defaults, so that one can normally use the config changing file list by hand etc.

options = VarParsing.VarParsing ('analysis')
options.register ('skipEvents',
                  -1, # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.int,          # string, int, or float
                  "Number of events to skip")
options.register ('JSONfile',
                  "", # default value
                  VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                  VarParsing.VarParsing.varType.string,          # string, int, or float
                  "JSON file (empty for no JSON)")
options.outputFile = 'NTuple.root'
options.inputFiles = []
options.maxEvents  = 10
options.parseArguments()


from Configuration.AlCa.autoCond import autoCond

if not isMC:
    process.GlobalTag.globaltag = '92X_dataRun2_HLT_v7'
    process.load('TauTriggerValidation.TauTriggerValidation.tagAndProbe_cff')
    process.source = cms.Source("PoolSource",
                                fileNames = cms.untracked.vstring(
                                   '/store/data/Run2017B/SingleMuon/RAW-RECO/MuTau-PromptReco-v1/000/297/488/00000/5CA074E9-C45B-E711-9BDD-02163E0133FE.root'
                                   )
                                )

else:
    process.GlobalTag.globaltag = '92X_upgrade2017_realistic_v12'
    process.load('TauTriggerValidation.TauTriggerValidation.MCanalysis_cff')
    process.source = cms.Source("PoolSource",
                                fileNames = cms.untracked.vstring(
                                    '/store/mc/RunIISummer17MiniAOD/VBFHToTauTau_M125_13TeV_powheg_pythia8/MINIAODSIM/NZSFlatPU28to62_HIG07_92X_upgrade2017_realistic_v10-v1/70000/0080A67C-FBA4-E711-A8FE-00259029E84C.root'
                                    )
                                )


if options.JSONfile:
    print "Using JSON: " , options.JSONfile
    process.source.lumisToProcess = LumiList.LumiList(filename = options.JSONfile).getVLuminosityBlockRange()

if options.inputFiles:
    process.source.fileNames = cms.untracked.vstring(options.inputFiles)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

if options.maxEvents >= -1:
    process.maxEvents.input = cms.untracked.int32(options.maxEvents)
if options.skipEvents >= 0:
    process.source.skipEvents = cms.untracked.uint32(options.skipEvents)


if isMC and not useGenMatch:
    process.Ntuplizer.taus = cms.InputTag("goodTaus")

process.p = cms.Path(
    process.TAndPseq
)

# Silence output
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

# Adding ntuplizer
process.TFileService=cms.Service('TFileService',fileName=cms.string(options.outputFile))
