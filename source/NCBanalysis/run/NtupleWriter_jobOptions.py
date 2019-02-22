import AthenaPoolCnvSvc.ReadAthenaPool

import glob
import os

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr+= THistSvc()
svcMgr.THistSvc.Output  = ["NtupleWriter DATAFILE='NtupleWriter.root' OPT='RECREATE'"];

InputCollections = glob.glob(os.environ['InputCollections'])
svcMgr.EventSelector.InputCollections = InputCollections

tdt=CfgMgr.Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc+=tdt
tdt.OutputLevel=WARNING

bunchStructureTool=CfgMgr.BunchStructureTool("BunchStructureTool")
bunchStructureTool.debug=False
ToolSvc+=bunchStructureTool

ToolSvc+=CfgMgr.BeamIntensityTool("BeamIntensityTool")
#ToolSvc+=CfgMgr.RunQueryPickleTool("RunQueryPickleTool")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from NCBanalysis.NCBanalysisConf import BeamBackgroundFiller
beamBackgroundFiller=BeamBackgroundFiller("BeamBackgroundFiller")
topSequence+=beamBackgroundFiller

ntupleWriterTool=CfgMgr.NtupleWriterTool("NtupleWriterTool");

import PyUtils.AthFile as athFile
fi=athFile.fopen(InputCollections[0])
stream_names=fi.fileinfos['stream_names']
print "fileinfos['stream_names']=",stream_names

jetClnToolLoose=CfgMgr.JetCleaningTool("JetCleaningToolLoose",CutLevel="LooseBad")
ToolSvc+=jetClnToolLoose

if 'IS_DATA' in fi.fileinfos['evt_type']:
  grlTool=CfgMgr.GoodRunsListSelectionTool("GRLTool",GoodRunsListVec=[
    "NCBanalysis/GRL/data15_13TeV.periodAllYear_HEAD_DQDefects-00-02-02_PHYS_StandardGRL_All_Good.xml",
    "NCBanalysis/GRL/data16_13TeV.periodAllYear_HEAD_DQDefects-00-02-04_PHYS_StandardGRL_All_Good.xml",
    "NCBanalysis/GRL/data17_13TeV.periodAllYear_HEAD_Unknown_PHYS_StandardGRL_All_Good.xml",
    "NCBanalysis/GRL/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml"])
  ToolSvc+=grlTool
  ntupleWriterTool.doBunchStructure=True
  ntupleWriterTool.doBeamIntensity=True
  ntupleWriterTool.doGRLTool=True
  ntupleWriterTool.doStableBeams=True

if 'IS_SIMULATION' in fi.fileinfos['evt_type'] and stream_names == ['StreamAOD']: 

  beamBackgroundFiller2=BeamBackgroundFiller("BeamBackgroundFiller2")
  beamBackgroundFiller2.cutEnergy=5000
  beamBackgroundFiller2.BeamBackgroundKey="BeamBackgroundDataCutEnergy5GeV"
  topSequence+=beamBackgroundFiller2
  ntupleWriterTool.BeamBackgroundKeys=["BeamBackgroundData","BeamBackgroundDataCutEnergy5GeV"]
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

if stream_names == ['StreamEVGEN']:
  ntupleWriterTool.mcEventCollection="GEN_EVENT"
  ntupleWriterTool.doMCEventCollection=True;

if stream_names == ['StreamHITS']:
  ntupleWriterTool.mcEventCollection="TruthEvent"
  ntupleWriterTool.doMCEventCollection=True
  # example of how to set the geometry : skeleton.HITtoRDO.py
  from AthenaCommon.GlobalFlags import globalflags
  globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-R2-2016-01-00-01')
  # example of how to initialize the geometry : RecExCommon_topOptions.py
  include("RecExCond/AllDet_detDescr.py")
  ntupleWriterTool.doHITS=True

if stream_names == ['StreamESD']:
 ntupleWriterTool.doEventInfo=True 
 ntupleWriterTool.doJets=True
 ntupleWriterTool.doPixelClusters=False 

if stream_names == ['StreamDAOD_IDNCB']:
  ntupleWriterTool.doEventInfo=True
  ntupleWriterTool.doTrigger=True
  ntupleWriterTool.doBeamBackgroundData=True
  #ntupleWriterTool.doPixelClusters=True
  ntupleWriterTool.doTopoClusters=True
  #ntupleWriterTool.TopoClusterEcut=-99999  # disable this cut
  ntupleWriterTool.TopoClusterEcut=10000     # MeV
  ntupleWriterTool.doJets=True
  ntupleWriterTool.doMuonSegments=True

ToolSvc+=ntupleWriterTool
ntupleWriterAlg=CfgMgr.NtupleWriter("NtupleWriter");
topSequence+=ntupleWriterAlg

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.infoLimit = 0
