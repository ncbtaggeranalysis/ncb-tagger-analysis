import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import theApp

if len(svcMgr.EventSelector.InputCollections) == 0:
  svcMgr.EventSelector.InputCollections = ['dummy.root']

import PyUtils.AthFile as athFile
fi=athFile.fopen(svcMgr.EventSelector.InputCollections[0])
stream_names=fi.fileinfos['stream_names']
print "fileinfos['stream_names']=",stream_names

ntupleWriterTool=CfgMgr.NtupleWriterTool("NtupleWriterTool")
ToolSvc+=CfgMgr.Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc+=CfgMgr.JetCleaningTool("JetCleaningToolLoose",CutLevel="LooseBad")

if 'IS_DATA' in fi.fileinfos['evt_type']:
  ToolSvc+=CfgMgr.BunchStructureTool(name = "BunchStructureTool")
  ToolSvc+=CfgMgr.BeamIntensityTool(name = "BeamIntensityTool")
  ToolSvc+=CfgMgr.RunQueryPickleTool(name = "RunQueryPickleTool")
  ToolSvc+=CfgMgr.GoodRunsListSelectionTool("GRLTool",GoodRunsListVec=[
    "NCBanalysis/GRL/data15_13TeV.periodAllYear_HEAD_DQDefects-00-02-02_PHYS_StandardGRL_All_Good.xml",
    "NCBanalysis/GRL/data16_13TeV.periodAllYear_HEAD_DQDefects-00-02-04_PHYS_StandardGRL_All_Good.xml",
    "NCBanalysis/GRL/data17_13TeV.periodAllYear_HEAD_Unknown_PHYS_StandardGRL_All_Good.xml",
    "NCBanalysis/GRL/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml"])

  ntupleWriterTool.doBunchStructure=True
  ntupleWriterTool.doBeamIntensity=True
  ntupleWriterTool.doGRLTool=True
  ntupleWriterTool.doStableBeams=True
  ntupleWriterTool.doEventInfo=True
  ntupleWriterTool.doTrigger=True
  ntupleWriterTool.doBeamBackgroundData=True
  ntupleWriterTool.doTopoClusters=True
  #ntupleWriterTool.TopoClusterEcut=-99999  # disable this cut
  ntupleWriterTool.TopoClusterEcut=5000     # MeV
  ntupleWriterTool.doJets=True
  ntupleWriterTool.doMuonSegments=True

if 'IS_SIMULATION' in fi.fileinfos['evt_type'] and stream_names == ['StreamAOD']:
  ntupleWriterTool.BeamBackgroundKeys=["BeamBackgroundData"]
  ntupleWriterTool.doBeamBackgroundData=True
  ntupleWriterTool.doBunchStructure=False
  ntupleWriterTool.doBeamIntensity=False
  ntupleWriterTool.doGRLTool=False
  ntupleWriterTool.doStableBeams=False
  ntupleWriterTool.doTopoClusters=True
  #ntupleWriterTool.TopoClusterEcut=-99999  # disable this cut
  ntupleWriterTool.TopoClusterEcut=5000     # MeV
  ntupleWriterTool.doJets=True
  ntupleWriterTool.doMuonSegments=True

ToolSvc+=ntupleWriterTool

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from NCBanalysis.NCBanalysisConf import BeamBackgroundFiller
beamBackgroundFiller=BeamBackgroundFiller("BeamBackgroundFiller")
topSequence+=beamBackgroundFiller

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr+= THistSvc()
svcMgr.THistSvc.Output  = ["NtupleWriter DATAFILE='NtupleWriter.root' OPT='RECREATE'"];

ntupleWriterAlg=CfgMgr.NtupleWriter("NtupleWriter");
topSequence+=ntupleWriterAlg

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.infoLimit = 0
