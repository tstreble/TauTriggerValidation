#ifndef NTUPLIZER_H
#define NTUPLIZER_H

/*
    ** class   : Ntuplizer
    ** author  : S. Bologna (Milano-Bicocca)
    ** date    : June 2016
    ** brief   : match tau probe to trigger filter and save results into a root TTree
*/

#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <TNtuple.h>


#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/ESHandle.h>
#include <FWCore/Utilities/interface/InputTag.h>
#include <DataFormats/PatCandidates/interface/Muon.h>
#include <DataFormats/PatCandidates/interface/Tau.h>
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"


#include "CommonTools/UtilAlgos/interface/TFileService.h"


class Ntuplizer : public edm::EDAnalyzer {
    public:
        /// Constructor
        explicit Ntuplizer(const edm::ParameterSet&);
        /// Destructor
        virtual ~Ntuplizer();  
        
    private:
        //----edm control---
        virtual void beginJob() ;
        virtual void beginRun(edm::Run const&, edm::EventSetup const&);
        virtual void analyze(const edm::Event&, const edm::EventSetup&);
        virtual void endJob();
        virtual void endRun(edm::Run const&, edm::EventSetup const&);
        int  checkPathList (const std::vector<std::string>& filtersToCheck, const std::vector<std::string>& filtersList) ;
        void Initialize(); 
        
        TTree *_tree;
        std::string _treeName;
        // -------------------------------------
        // variables to be filled in output tree
        ULong64_t       _indexevents;
        Int_t           _runNumber;
        Int_t           _lumi;

        edm::EDGetTokenT<pat::MuonRefVector>  _muonsTag;
        edm::EDGetTokenT<pat::TauRefVector>   _tauTag;
        edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> _triggerObjects;
        edm::EDGetTokenT<edm::TriggerResults> _triggerBits;

        edm::InputTag _processName;
        HLTConfigProvider _hltConfig;

        // set of path names to be saved
        // will be saved bitwise, according to position of the searched string into the vector
        // unsigned int _doubleMediumIsoPFTau32;
        // unsigned int _doubleMediumIsoPFTau35;
        // unsigned int _doubleMediumIsoPFTau40;

        std::vector<std::string> _filters_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1;
        std::vector<std::string> _filters_HLT_IsoMu19_eta2p1_LooseIsoPFTau20;
        std::vector<std::string> _filters_HLT_IsoMu19_eta2p1_MediumIsoPFTau35_Trk1_eta2p1_Reg;
        std::vector<std::string> _filters_HLT_LooseIsoPFTau50_Trk30_eta2p1;

        // output variables
        float _tauPt;
        float _tauEta;
        float _tauPhi;
        float _tauEnergy;
        int _pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1;
        int _pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20;
        int _pass_HLT_IsoMu19_eta2p1_MediumIsoPFTau35_Trk1_eta2p1_Reg;
        int _pass_HLT_LooseIsoPFTau50_Trk30_eta2p1;

};


// ----Constructor and Destructor -----
Ntuplizer::Ntuplizer(const edm::ParameterSet& iConfig) :
_muonsTag       (consumes<pat::MuonRefVector>                     (iConfig.getParameter<edm::InputTag>("muons"))),
_tauTag         (consumes<pat::TauRefVector>                      (iConfig.getParameter<edm::InputTag>("taus"))),
_triggerObjects (consumes<pat::TriggerObjectStandAloneCollection> (iConfig.getParameter<edm::InputTag>("triggerSet"))),
_triggerBits    (consumes<edm::TriggerResults>                    (iConfig.getParameter<edm::InputTag>("triggerResultsLabel")))
{
    _treeName = iConfig.getParameter<std::string>("treeName");
    _processName = iConfig.getParameter<edm::InputTag>("triggerResultsLabel");

    _filters_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1 = {
                "hltOverlapFilterIsoMu17MediumIsoPFTau32Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau32Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau32Reg"
            } ;
    _filters_HLT_IsoMu19_eta2p1_LooseIsoPFTau20 = {
                "hltOverlapFilterIsoMu17MediumIsoPFTau35Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau35Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau35Reg"
            } ;
    _filters_HLT_IsoMu19_eta2p1_MediumIsoPFTau35_Trk1_eta2p1_Reg = {
                "hltOverlapFilterIsoMu17MediumIsoPFTau40Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau40Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau40Reg"
            } ;
    _filters_HLT_LooseIsoPFTau50_Trk30_eta2p1 = {
                "hltOverlapFilterIsoMu17MediumIsoPFTau40Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau40Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau40Reg"
            } ;
    // FIXME !! previous names are dummy . Need to initialized the with L2, L2.5 and L3 (final) filter names.
    // in principle can accept as many filter names as desired.
    // They will be stored bitwise (i.e., _diTau32_filters.at(0) will be in bit 0 of pass_diTau32)

    Initialize();
    return;
}

Ntuplizer::~Ntuplizer()
{}

void Ntuplizer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{}

void Ntuplizer::Initialize() {
    
    _indexevents = 0;
    _runNumber = 0;
    _lumi = 0;

    _tauPt     = -1.;    
    _tauEta    = -1.;    
    _tauPhi    = -1.;    
    _tauEnergy = -1.;    

    _pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1 = 0;
    _pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20 = 0;
    _pass_HLT_IsoMu19_eta2p1_MediumIsoPFTau35_Trk1_eta2p1_Reg = 0;
    _pass_HLT_LooseIsoPFTau50_Trk30_eta2p1 = 0;
}

int  Ntuplizer::checkPathList (const std::vector<std::string>& filtersToCheck, const std::vector<std::string>& filtersList)
{
    int flags = 0;
    for (std::string label : filtersList)
    {
        for (uint ifilter = 0; ifilter < filtersToCheck.size(); ++ifilter)
        {
            if (label == filtersToCheck.at(ifilter)) flags |= (1 << ifilter) ;
        }
    }
    return flags;
}

void Ntuplizer::beginJob()
{
    edm::Service<TFileService> fs;
    _tree = fs -> make<TTree>(_treeName.c_str(), _treeName.c_str());

    //Branches
    _tree -> Branch("EventNumber",&_indexevents,"EventNumber/l");
    _tree -> Branch("RunNumber",&_runNumber,"RunNumber/I");
    _tree -> Branch("lumi",&_lumi,"lumi/I");

    _tree -> Branch("tauPt",     &_tauPt);
    _tree -> Branch("tauEta",    &_tauEta);
    _tree -> Branch("tauPhi",    &_tauPhi);
    _tree -> Branch("tauEnergy", &_tauEnergy);

    _tree -> Branch("pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1",         &_pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1);
    _tree -> Branch("pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20",                  &_pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20);
    _tree -> Branch("pass_HLT_IsoMu19_eta2p1_MediumIsoPFTau35_Trk1_eta2p1_Reg", &_pass_HLT_IsoMu19_eta2p1_MediumIsoPFTau35_Trk1_eta2p1_Reg);
    _tree -> Branch("pass_HLT_LooseIsoPFTau50_Trk30_eta2p1",                    &_pass_HLT_LooseIsoPFTau50_Trk30_eta2p1);
    
    return;
}


void Ntuplizer::endJob()
{
    return;
}


void Ntuplizer::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
    return;
}

void Ntuplizer::analyze(const edm::Event& iEvent, const edm::EventSetup& eSetup)
{

    Initialize();

    _indexevents = iEvent.id().event();
    _runNumber   = iEvent.id().run();
    _lumi        = iEvent.luminosityBlock();

    // search for the tag in the event
    edm::Handle<pat::MuonRefVector> muonHandle;
    edm::Handle<pat::TauRefVector>  tauHandle;
    edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
    edm::Handle<edm::TriggerResults> triggerBits;

    iEvent.getByToken(_muonsTag, muonHandle);
    iEvent.getByToken(_tauTag,   tauHandle);
    iEvent.getByToken(_triggerObjects, triggerObjects);
    iEvent.getByToken(_triggerBits, triggerBits);

    const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);
    const pat::TauRef tau = (*tauHandle)[0] ;

    for (pat::TriggerObjectStandAlone obj : *triggerObjects)
    {
        if (deltaR (*tau, obj) < 0.5)
        {
            obj.unpackPathNames(names);            
            const std::vector<std::string>& vLabels = obj.filterLabels();
            _pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1         = checkPathList (_filters_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1         , vLabels) ;
            _pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20                  = checkPathList (_filters_HLT_IsoMu19_eta2p1_LooseIsoPFTau20                  , vLabels) ;
            _pass_HLT_IsoMu19_eta2p1_MediumIsoPFTau35_Trk1_eta2p1_Reg = checkPathList (_filters_HLT_IsoMu19_eta2p1_MediumIsoPFTau35_Trk1_eta2p1_Reg , vLabels) ;
            _pass_HLT_LooseIsoPFTau50_Trk30_eta2p1                    = checkPathList (_filters_HLT_LooseIsoPFTau50_Trk30_eta2p1                    , vLabels) ;
            break;
        }
    }

    _tauPt     = tau->pt();
    _tauEta    = tau->eta();
    _tauPhi    = tau->phi();
    _tauEnergy = tau->energy();

    _tree -> Fill();    
}

#include <FWCore/Framework/interface/MakerMacros.h>
DEFINE_FWK_MODULE(Ntuplizer);

#endif //NTUPLIZER_H