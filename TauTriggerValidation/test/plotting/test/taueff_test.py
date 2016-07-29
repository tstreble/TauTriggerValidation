'''
Usage:python taueff_test .py RootFile.root label[optional]

Script to make some quick efficiency plots for tau trigger validation tool

Author: Usama Hussain, UW Madison

'''

from subprocess import Popen
from sys import argv, exit, stdout, stderr

import ROOT

# So things don't look like crap.
ROOT.gROOT.SetStyle("Plain")
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

######## File #########
if len(argv) < 2:
   print 'Usage:python taueff_test .py RootFile.root label[optional]'
   exit()

infile = argv[1]
ntuple_file = ROOT.TFile(infile)

######## LABEL & SAVE WHERE #########

if len(argv)>2:
   saveWhere='~/www/'+argv[3]+'_'
else:
   saveWhere='EfficiencyPlots/Eff_'


#####################################
#Get the denom, num from the  NTUPLE                 #
#####################################


denom = ntuple_file.Get("barrel_HLT/tau_pt_den")
num = ntuple_file.Get("barrel_HLT/tau_pt_num")
canvas = ROOT.TCanvas("asdf", "adsf", 800, 800
)

def make_l1g_efficiency(denom, num,markercolor,linecolor):
    ''' Make an efficiency graph with the "UCT" style '''
    eff = ROOT.TGraphAsymmErrors(num, denom)
    eff.SetMarkerStyle(22)
    eff.SetMarkerColor(markercolor)
    eff.SetMarkerSize(1.2)
    eff.SetLineColor(linecolor)
    return eff

def efficiencies(binning, filename,title='', xaxis=''):
    frame = ROOT.TH1F("frame", "frame", *binning)
    l1g = make_l1g_efficiency(denom, num,ROOT.kGreen,ROOT.kBlack)
    frame.SetMaximum(1.20)
    frame.SetMinimum(0.40)
    frame.GetYaxis().SetLabelFont(42);
    frame.GetYaxis().SetLabelSize(0.025);
    frame.GetYaxis().SetTitleSize(0.03);
    frame.GetYaxis().SetTitleFont(42);
    frame.GetXaxis().SetLabelFont(42);
    frame.GetXaxis().SetLabelSize(0.025);
    frame.GetXaxis().SetTitleSize(0.03);
    frame.GetXaxis().SetTitleFont(42);
    frame.GetXaxis().SetTitle(xaxis)
    frame.GetYaxis().SetTitle("L1 + HLT efficiency")
    frame.Draw()
    l1g.Draw('pe')
    legend = ROOT.TLegend(0.11,0.75,0.56,0.90,'','brNDC')
    legend.SetFillColor(ROOT.kWhite)
    legend.SetBorderSize(0)
    legend.SetLineColor(1)
    legend.SetLineStyle(2)
    legend.SetFillStyle(0)
    legend.SetTextFont(42)
    legend.AddEntry(l1g, "Basic Selections", "pe")
    legend.Draw()
    saveas = saveWhere+filename+'.png'
    print saveas
    canvas.SaveAs(saveas)

efficiencies([100, 0, 100], 'taupt_eff',"TauTriggerValidation", "offline #tau #p_{T} (GeV)")
