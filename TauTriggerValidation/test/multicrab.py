#multicrab

dataset = {
  'dataset1': '/SingleMuon/Run2016E-PromptReco-v2/MINIAOD',
  'dataset2': '/SingleMuon/Run2016F-PromptReco-v1/MINIAOD',
  'dataset3': '/SingleMuon/Run2016G-PromptReco-v1/MINIAOD'
}

if __name__ == '__main__':
 from CRABAPI.RawCommand import crabCommand

def submit(config):
 res = crabCommand('submit', config = config)

from CRABClient.UserUtilities import config
config = config()
name = 'TagAndProbe_2016'
config.General.workArea = 'SingleMuon_'+name
config.General.transferOutputs = True
config.General.transferLogs = True
config.JobType.pluginName = 'Analysis'
config.Data.publication = False
config.Site.storageSite = 'T2_US_Wisconsin'
config.JobType.psetName = 'test.py'
config.section_('Data') 
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-279931_13TeV_PromptReco_Collisions16_JSON_NoL1T.txt'
config.Data.runRange = '276831-279931'
#Already submitted:
listOfSamples = ['dataset1','dataset2','dataset3']
for sample in listOfSamples:  
  config.General.requestName = sample
  config.Data.inputDataset = dataset[sample]
  config.Data.unitsPerJob = 20
  config.Data.totalUnits = -1
  config.Data.outLFNDirBase = '/store/user/uhussain/'+name
  submit(config)
