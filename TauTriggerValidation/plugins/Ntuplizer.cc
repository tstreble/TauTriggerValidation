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
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/L1Trigger/interface/BXVector.h"
#include "DataFormats/L1Trigger/interface/Tau.h"
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
        Int_t     bx_;
        Int_t npv_;
        Int_t     nVtx_;
        Int_t     nTrksPV_;
        Bool_t    isPVGood_;
        Bool_t    hasGoodVtx_;
        float     vtx_;
        float     vty_;
        float     vtz_;
  
        edm::EDGetTokenT<pat::MuonRefVector>  _muonsTag;
        edm::EDGetTokenT<pat::TauRefVector>   _tauTag;
        edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> _triggerObjects;
        edm::EDGetTokenT<edm::TriggerResults> _triggerBits;
        edm::EDGetTokenT<reco::VertexCollection> vtxToken_;
        edm::EDGetTokenT<BXVector <l1t::Tau> > l1Stage2TauSource_;

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

        //variables for l1Taus
        std::vector<float> _l1tauPt;
        std::vector<float> _l1tauEta;
        std::vector<float> _l1tauPhi;

        float _tauPt;
        float _tauEta;
        float _tauPhi;
        float _tauEnergy;
        int _tauDecayMode;

        //storing pT of the L1 Tau/Iso Tau Candidate
        float l1isoTauPt;
        int l1IsoMatched;
 
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
//l1Stage2TauSource_(consumes<BXVector<l1t::Tau> > (iConfig.getParameter<edm::InputTag>("stage2TauSource")))
{

     l1Stage2TauSource_ = consumes<BXVector<l1t::Tau> > (iConfig.getParameter<edm::InputTag>("stage2TauSource"));
     vtxToken_= consumes<reco::VertexCollection>     (iConfig.getParameter<edm::InputTag>("VtxLabel"));
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
    bx_ = 0;
    npv_ = 0;
    _tauPt     = -1.;    
    _tauEta    = -1.;    
    _tauPhi    = -1.;    
    _tauEnergy = -1.;    
    _tauDecayMode = 0;

     l1IsoMatched = 0;
     l1isoTauPt = -1.;
    
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
    _tree-> Branch("BunchCrossing",&bx_,"BunchCrossing/I");
    _tree->Branch("npv",&npv_,"npv/I");
    _tree->Branch("nVtx", &nVtx_);
    _tree->Branch("nTrksPV",&nTrksPV_);
    _tree->Branch("isPVGood",&isPVGood_);
    _tree->Branch("hasGoodVtx",&hasGoodVtx_);
    _tree->Branch("vtx",&vtx_);
    _tree->Branch("vty",&vty_);
    _tree->Branch("vtz",&vtz_);

    _tree -> Branch("tauPt",     &_tauPt);
    _tree -> Branch("tauEta",    &_tauEta);
    _tree -> Branch("tauPhi",    &_tauPhi);
    _tree -> Branch("tauEnergy", &_tauEnergy);
    _tree -> Branch("tauDecayMode", &_tauDecayMode);
    
    _tree->Branch("l1tauPt",&_l1tauPt);
    _tree->Branch("l1tauEta",&_l1tauEta);
    _tree->Branch("l1tauPhi",&_l1tauPhi);

    _tree-> Branch("l1IsoTauPt",&l1isoTauPt);
    _tree-> Branch("l1IsoMatched", &l1IsoMatched);

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
    using namespace edm;
    Initialize();

    //cleanup from previous execution
    _l1tauPt.clear();
    _l1tauEta.clear();
    _l1tauPhi.clear();

    _indexevents = iEvent.id().event();
    _runNumber   = iEvent.id().run();
    _lumi        = iEvent.luminosityBlock();
    bx_          = iEvent.bunchCrossing();

    // search for the tag in the event
    Handle<pat::MuonRefVector> muonHandle;
    Handle<pat::TauRefVector>  tauHandle;
    Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
    Handle<edm::TriggerResults> triggerBits;

    iEvent.getByToken(_muonsTag, muonHandle);
    iEvent.getByToken(_tauTag,   tauHandle);

    iEvent.getByToken(_triggerObjects, triggerObjects);
    iEvent.getByToken(_triggerBits, triggerBits);
    
    Handle<reco::VertexCollection> vtxHandle;
    iEvent.getByToken(vtxToken_, vtxHandle);

    Handle < BXVector<l1t::Tau> > stage2Taus;
    iEvent.getByToken(l1Stage2TauSource_,stage2Taus);
   
    //number of primary vertices 
    npv_ = vtxHandle->size();

    nVtx_ = -1;
    if (vtxHandle.isValid()) {
    nVtx_ = 0;

    hasGoodVtx_ = false;
    for (uint32_t v = 0; v < vtxHandle->size(); v++) {
    const reco::Vertex &vertex = (*vtxHandle)[v];
    if (nVtx_ == 0) {
        nTrksPV_ = vertex.nTracks();
        vtx_     = vertex.x();
        vty_     = vertex.y();
        vtz_     = vertex.z();

        isPVGood_ = false;
        if (vertex.ndof() > 4. && fabs(vertex.z()) <= 24. && fabs(vertex.position().rho()) <= 2.) isPVGood_ = true;
      }

      if (vertex.ndof() > 4. && fabs(vertex.z()) <= 24. && fabs(vertex.position().rho()) <= 2.) hasGoodVtx_ = true;
      nVtx_++;

    }
 } else
    LogWarning("Ntuplizer") << "Primary vertices info not unavailable";
    
    const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);
    const pat::TauRef tau = (*tauHandle)[0] ;

    for (pat::TriggerObjectStandAlone obj : *triggerObjects)
    {
        if (deltaR (*tau, obj) < 0.5)
        {
            obj.unpackPathNames(names);   
            // Here we want a loop over HLT filters?
            //     for (size_t iF = 0; iF < obj.filterLabels().size(); ++iF) {
            //           string label = obj.filterLabels()[iF];         
            const std::vector<std::string>& vLabels = obj.filterLabels();
            _pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1         = checkPathList (_filters_HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1         , vLabels) ;
            _pass_HLT_IsoMu19_eta2p1_LooseIsoPFTau20                  = checkPathList (_filters_HLT_IsoMu19_eta2p1_LooseIsoPFTau20                  , vLabels) ;
            _pass_HLT_IsoMu19_eta2p1_MediumIsoPFTau35_Trk1_eta2p1_Reg = checkPathList (_filters_HLT_IsoMu19_eta2p1_MediumIsoPFTau35_Trk1_eta2p1_Reg , vLabels) ;
            _pass_HLT_LooseIsoPFTau50_Trk30_eta2p1                    = checkPathList (_filters_HLT_LooseIsoPFTau50_Trk30_eta2p1                    , vLabels) ;
            break;
        }
    }
    if (tauHandle->size()>0){
 	double deltaR_ = 0.5;

    _tauPt     = tau->pt();
    _tauEta    = tau->eta();
    _tauPhi    = tau->phi();
    _tauEnergy = tau->energy();
    _tauDecayMode = tau->decayMode();

    //Looping over L1 Taus and matching them with tau candidate chosen above
     for(uint32_t k = 0; k<stage2Taus->size(); k++){
        const l1t::Tau &L1Tau = (*stage2Taus)[k];

	_l1tauPt.push_back(L1Tau.pt());
        _l1tauEta.push_back(L1Tau.eta());
        _l1tauPhi.push_back( L1Tau.phi());

	double dR = deltaR( tau->p4(), L1Tau.p4()); //matching condition

	if( dR < deltaR_){
	  l1isoTauPt  = L1Tau.pt();
	  l1IsoMatched = 1;
	  break;
	}
      }
}
    _tree -> Fill();    
}

#include <FWCore/Framework/interface/MakerMacros.h>
DEFINE_FWK_MODULE(Ntuplizer);

#endif //NTUPLIZER_H
