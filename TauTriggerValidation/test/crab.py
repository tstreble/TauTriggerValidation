from WMCore.Configuration import Configuration
config = Configuration()
#General                                                                                                                                                                           
config.section_('General')
config.General.requestName = 'TagaAndProbe_2016'
config.General.workArea = 'TagAndProbe_2016_SingleMuon_2016_Sep16'
config.General.transferLogs = True
#JobType                                                                                                                                                                           
config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'test.py'
config.JobType.outputFiles = ['NTuple.root']

#Data                                                                                                                                                                              
config.section_('Data')
config.Data.inputDataset = '/SingleMuon/Run2016B-PromptReco-v2/MINIAOD'
#config.Data.inputDBS = 'global'
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Cert_271036-279931_13TeV_PromptReco_Collisions16_JSON_NoL1T.txt'
config.Data.splitting = 'LumiBased'
config.Data.runRange = '276831-279931'
config.Data.unitsPerJob = 15
config.Data.publication = False
config.Data.totalUnits =  -1
config.Data.ignoreLocality = True
config.Data.outLFNDirBase  ='/store/user/uhussain/TagAndProbeValidation_Sep16'
#User                                                                                                                                                                              
config.section_('User')
#Site                                                                                                                                                                              
config.section_('Site')
config.Site.storageSite = 'T2_US_Wisconsin'

