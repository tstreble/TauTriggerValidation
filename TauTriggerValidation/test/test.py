import FWCore.ParameterSet.VarParsing as VarParsing
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Config as cms
process = cms.Process("TagAndProbe")

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
options.maxEvents  = -999
options.parseArguments()


from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v8'
process.load('TauTriggerValidation.TauTriggerValidation.tagAndProbe_cff')
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
       # '/store/mc/RunIISpring16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1/40000/00200284-F15C-E611-AA9B-002590574776.root',
        # '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/082EC2A0-4C28-E611-BC61-02163E014412.root',
        # '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/1014078C-4C28-E611-85FB-02163E0141C1.root',
        # '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/203E5176-4C28-E611-B4F8-02163E014743.root',
        # '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/32508866-4C28-E611-A38D-02163E011BAF.root',
        # '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/44AF1068-4C28-E611-80D0-02163E01367B.root',
        # '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/5AF4B08A-4C28-E611-AEC9-02163E01342C.root',
        # '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/6E3FD070-4C28-E611-9A1E-02163E011DC7.root',
        '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/7005DB70-4C28-E611-8628-02163E0144DD.root',
        '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/7C2CB76B-4C28-E611-8D90-02163E01467F.root',
        '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/86B68469-4C28-E611-92A6-02163E01419C.root',
        '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/275/125/00000/24FC42B2-8036-E611-B42D-02163E012BD1.root'
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
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


process.p = cms.Path(
    process.TAndPseq
)

# Silence output
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

# Adding ntuplizer
process.TFileService=cms.Service('TFileService',fileName=cms.string(options.outputFile))
