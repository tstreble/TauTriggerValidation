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
        std::vector<std::string> _doubleMediumIsoPFTau32_filters;
        std::vector<std::string> _doubleMediumIsoPFTau35_filters;
        std::vector<std::string> _doubleMediumIsoPFTau40_filters;


        // output variables
        float _tauPt;
        float _tauEta;
        float _tauPhi;
        float _tauEnergy;
        int   _pass_doubleMediumIsoPFTau32;
        int   _pass_doubleMediumIsoPFTau35;
        int   _pass_doubleMediumIsoPFTau40;

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

    _doubleMediumIsoPFTau32_filters = {
                "hltOverlapFilterIsoMu17MediumIsoPFTau32Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau32Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau32Reg"
            } ;
    _doubleMediumIsoPFTau35_filters = {
                "hltOverlapFilterIsoMu17MediumIsoPFTau35Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau35Reg",
                "hltOverlapFilterIsoMu17MediumIsoPFTau35Reg"
            } ;
    _doubleMediumIsoPFTau40_filters = {
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

    _pass_doubleMediumIsoPFTau32 = 0;
    _pass_doubleMediumIsoPFTau35 = 0;
    _pass_doubleMediumIsoPFTau40 = 0;
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

    _tree -> Branch("pass_doubleMediumIsoPFTau32", &_pass_doubleMediumIsoPFTau32);
    _tree -> Branch("pass_doubleMediumIsoPFTau35", &_pass_doubleMediumIsoPFTau35);
    _tree -> Branch("pass_doubleMediumIsoPFTau40", &_pass_doubleMediumIsoPFTau40);
    
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
            _pass_doubleMediumIsoPFTau32 = checkPathList (_doubleMediumIsoPFTau32_filters , vLabels) ;
            _pass_doubleMediumIsoPFTau35 = checkPathList (_doubleMediumIsoPFTau35_filters , vLabels) ;;
            _pass_doubleMediumIsoPFTau40 = checkPathList (_doubleMediumIsoPFTau40_filters , vLabels) ;;
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
