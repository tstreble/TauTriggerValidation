import sys
sys.path.append('..')
from objects.Efficiencies import Efficiency1D
from objects.Plotter      import Plotter

import ROOT
import numpy as np
import array as ar
from itertools import product
from copy import deepcopy as dc

ROOT.gROOT.SetBatch(True)
ROOT.TH1.SetDefaultSumw2()

baseline = '&'.join([
    ' l1_pt > 17 '                                 ,
    ' abs(l1_eta) < 2.1 '                          ,
    ' l1_reliso05 < 0.1 '                          ,
    ' l1_muonid_medium > 0.5 '                     ,
    ' l2_pt > 20 '                                 ,
    ' abs(l2_eta) < 2.1 '                          ,
    ' abs(l2_charge) == 1 '                        ,
    ' l2_decayModeFinding '                        ,
    ' l2_byIsolationMVArun2v1DBnewDMwLT >= 3'      ,
    ' l2_againstMuon3 > 1.5 '                      ,
    ' l2_againstElectronMVA6 > 1.5 '               ,
#     ' n_bjets == 0 '                               ,
#     ' pass_leptons == 1 '                          ,
#     ' veto_dilepton == 0 '                         ,
#     ' veto_thirdlepton == 0 '                      ,
#     ' veto_otherlepton == 0 '                      ,
    ' (l2_decayMode == 0 || l2_decayMode == 1) '   ,
    ' mt < 40 '                                    ,
    ' n_jets > 2'                                  ,
])

mt = {
#     'lowmt'  : ' mt <  30 ',
#     'highmt' : ' mt >= 30 ',
    ''       : ' 1 '       ,
}

zmass = {
#     'zmass' : ' mvis > 40. && mvis < 80. ',
    ''      : ' 1 '                       ,
}

sign = { 
#     'sub' : ' (l1_charge != l2_charge) - (l1_charge == l2_charge) ',
#     'ss'  : ' (l1_charge == l2_charge) ',
    'os'  : ' (l1_charge != l2_charge) ',
}

eta_bins = {
#     'barrel' : ' abs(l2_eta) < 1.444 ',
#     'endcap' : ' abs(l2_eta) > 1.566 ',
    ''       : ' 1 '                  ,
}

decaymode = {
#     'dm0'   : ' l2_decayMode == 0  ',
#     'dm1'   : ' l2_decayMode == 1  ',
#     'dm10'  : ' l2_decayMode == 10 ',
    ''      : ' 1 '                 ,
}

TriggerSelection = {
    'HLT'       : '  probe & l2_trig_obj_pt > 0'                   ,
    'L3Iso'     : '  l2_hltPFTau50TrackPt30LooseAbsOrRelIso_pt > 0',
    'L3TrackPt' : '  l2_hltPFTau50TrackPt30_pt > 0'                ,
    'L3Track'   : '  l2_hltPFTau50Track_pt > 0'                    ,
    'L3Pt'      : '  l2_hltPFTau50_pt > 0'                         ,
    'L2'        : '  l2_hltSingleL2Tau35eta2p2_pt > 0'             ,
}

filenames = [
    '../../2016Bv4_HLT_IsoMu16_eta2p1_MET30_LooseIsoPFTau50_Trk30_eta2p1/SingleMuon_Run2016B_PromptReco_v1/H2TauTauTreeProducerTauMu/tree.root',
    '../../2016Bv4_HLT_IsoMu16_eta2p1_MET30_LooseIsoPFTau50_Trk30_eta2p1/SingleMuon_Run2016B_PromptReco_v2/H2TauTauTreeProducerTauMu/tree.root',    
]

t1 = ROOT.TChain('tree')

for fname in filenames:
    t1.Add(fname)

nbins   = 40
# bins    = [0., 10., 20., 25., 30., 32.5, 35., 37.5, 40., 42.5, 45., 50., 55., 60., 70., 90., 120., 200., 500., 1000.]
bins    = [0., 10., 25., 32.5, 40., 50., 60., 90., 200.]

variables = [
#     Efficiency1D(tree=t1, name='bx'     , variable='bx'        , histo_name='bx'     , cut_num='1', cut_den='1', xlabel='# bunch crossing'  , ylabel='L1 + HLT #tau efficiency', bins=125  , bini=0     , bine=3500   ),
#     Efficiency1D(tree=t1, name='run'    , variable='run'       , histo_name='bx'     , cut_num='1', cut_den='1', xlabel='run'               , ylabel='L1 + HLT #tau efficiency', bins=1400 , bini=273100, bine=274500 ),
    Efficiency1D(tree=t1, name='tau_eta', variable='l2_eta'    , histo_name='tau_eta', cut_num='1', cut_den='1', xlabel='offline #tau #eta' , ylabel='L1 + HLT #tau efficiency', bins=nbins, bini=-3.   , bine=   3.  ),
    Efficiency1D(tree=t1, name='tau_phi', variable='l2_phi'    , histo_name='tau_phi', cut_num='1', cut_den='1', xlabel='offline #tau #phi' , ylabel='L1 + HLT #tau efficiency', bins=nbins, bini=-3.15 , bine=   3.15),
    Efficiency1D(tree=t1, name='npv'    , variable='n_vertices', histo_name='npv'    , cut_num='1', cut_den='1', xlabel='# PV'              , ylabel='L1 + HLT #tau efficiency', bins=10   , bini= 0    , bine=  30   ),
    Efficiency1D(tree=t1, name='tau_pt' , variable='l2_pt'     , histo_name='tau_pt' , cut_num='1', cut_den='1', xlabel='offline #tau p_{T}', ylabel='L1 + HLT #tau efficiency', bins=bins ,                         ),
#     Efficiency1D(tree=t1, name='mvis'   , variable='mvis'      , histo_name='mvis'   , cut_num='1', cut_den='1', xlabel='offline #tau #eta' , ylabel='L1 + HLT #tau efficiency', bins=nbins, bini= 0.   , bine= 100.  ),
]

HLTPlotter = Plotter(variables     = variables                    , 
                     out_filename  = 'tau_met_plots.root'         ,
                     sel_baseline  = baseline                     , 
                     sel_extra_den = [ eta_bins, mt, zmass, sign ], 
                     sel_num       = [ TriggerSelection ]         )

HLTPlotter.run()
