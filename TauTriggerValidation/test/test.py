import FWCore.ParameterSet.Config as cms
process = cms.Process("TagAndProbe")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff") 

from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v8'
process.load('TauTriggerValidation.TauTriggerValidation.tagAndProbe_cff')

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    #     '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v1/000/272/760/00000/68B88794-7015-E611-8A92-02163E01366C.root'
         '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v1/000/273/017/00000/88722B6F-6419-E611-A131-02163E01236F.root'
    #    '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/082EC2A0-4C28-E611-BC61-02163E014412.root'
    #    '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/1014078C-4C28-E611-85FB-02163E0141C1.root',
    #    '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/203E5176-4C28-E611-B4F8-02163E014743.root',
    #    '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/32508866-4C28-E611-A38D-02163E011BAF.root',
    #    '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/44AF1068-4C28-E611-80D0-02163E01367B.root',
    #    '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/5AF4B08A-4C28-E611-AEC9-02163E01342C.root',
    #    '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/6E3FD070-4C28-E611-9A1E-02163E011DC7.root',
    #    '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/7005DB70-4C28-E611-8628-02163E0144DD.root',
    #    '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/7C2CB76B-4C28-E611-8D90-02163E01467F.root',
    #    '/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/274/199/00000/86B68469-4C28-E611-92A6-02163E01419C.root',
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.p = cms.Path(
    process.TAndPseq
)

# Silence output
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 5000

# Adding ntuplizer
process.TFileService=cms.Service('TFileService',fileName=cms.string('NTuple_SingleMuon2016B_v1.root'))
