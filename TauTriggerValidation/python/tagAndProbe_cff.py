import FWCore.ParameterSet.Config as cms

# filter HLT paths for T&P
import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
hltFilter = hlt.hltHighLevel.clone(
    TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
    HLTPaths = ['HLT_IsoMu22_v*'],
    andOr = cms.bool(True), # how to deal with multiple triggers: True (OR) accept if ANY is true, False (AND) accept if ALL are true
    throw = cms.bool(True) #if True: throws exception if a trigger path is invalid  
)

## good muons for T&P
goodMuons = cms.EDFilter("PATMuonRefSelector",
        src = cms.InputTag("slimmedMuons"),
        cut = cms.string(
                'pt > 24 && abs(eta) < 2.1 ' # kinematics
                '&& ( (pfIsolationR03().sumChargedHadronPt + max(pfIsolationR03().sumNeutralHadronEt + pfIsolationR03().sumPhotonEt - 0.5 * pfIsolationR03().sumPUPt, 0.0)) / pt() ) < 0.1 ' # isolation
                '&& isMediumMuon()' # quality -- medium muon
        ),
        filter = cms.bool(True)
)

## good taus - apply analysis selection
goodTaus = cms.EDFilter("PATTauRefSelector",
        src = cms.InputTag("slimmedTaus"),
        cut = cms.string(
                'pt > 20 && abs(eta) < 2.5 ' #kinematics
                '&& abs(charge) > 0 && abs(charge) < 2 ' #sometimes 2 prongs have charge != 1 
                '&& tauID("decayModeFinding") > 0.5 ' # tau ID
                '&& tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits") < 2.0 ' # tau iso - NOTE: can as well use boolean discriminators with WP
                '&& tauID("againstMuonTight3") > 0.5 ' # anti Muon tight
                '&& tauID("againstElectronVLooseMVA6") > 0.5 ' # anti-Ele loose
        ),
        filter = cms.bool(True)
)

## b jet veto : no additional b jets in the event (reject tt) -- use in sequence with 
bjets = cms.EDFilter("PATJetRefSelector",
        src = cms.InputTag("slimmedJets"),
        cut = cms.string(
                'pt > 30 && abs(eta) < 2.5 ' #kinematics
                '&& bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > 0.800' # b tag with medium WP
        ),
        filter = cms.bool(True)
)

TagAndProbe = cms.EDFilter("TauTagAndProbeFilter",
        taus  = cms.InputTag("goodTaus"),
        muons = cms.InputTag("goodMuons"),
        met   = cms.InputTag("slimmedMETs")
)

Ntuplizer = cms.EDAnalyzer("Ntuplizer",
    treeName = cms.string("TagAndProbe"),
    muons = cms.InputTag("TagAndProbe"),
    taus  = cms.InputTag("TagAndProbe"),
    triggerSet = cms.InputTag("selectedPatTrigger"),
    triggerResultsLabel = cms.InputTag("TriggerResults", "", "HLT"),
    VtxLabel          = cms.InputTag("offlineSlimmedPrimaryVertices"),
)

TAndPseq = cms.Sequence(
    hltFilter   +
    goodMuons   +
    goodTaus    +
    ~bjets      +
    TagAndProbe +
    Ntuplizer
)
