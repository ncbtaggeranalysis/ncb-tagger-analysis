import AthenaPoolCnvSvc.ReadAthenaPool

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr+= THistSvc()
svcMgr.THistSvc.Output  = ["MisIdProb DATAFILE='MisIdProb.root' OPT='RECREATE'"];

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from NCBanalysis.NCBanalysisConf import BeamBackgroundFiller
beamBackgroundFiller1=BeamBackgroundFiller("BeamBackgroundFiller1")
beamBackgroundFiller1.BeamBackgroundKey="BeamBackgroundData"
topSequence+=beamBackgroundFiller1

#beamBackgroundFiller2=BeamBackgroundFiller("BeamBackgroundFiller2")
#beamBackgroundFiller2.BeamBackgroundKey="BeamBackgroundDataNoMDTSegmentsWitht0EqualTo0"
#beamBackgroundFiller2.useMDTSegmentsWitht0EqualTo0=False
#topSequence+=beamBackgroundFiller2

#beamBackgroundFiller3=BeamBackgroundFiller("BeamBackgroundFiller3")
#beamBackgroundFiller3.BeamBackgroundKey="BeamBackgroundDataNoMDT"
#beamBackgroundFiller3.useMDT=False
#topSequence+=beamBackgroundFiller3

ToolSvc+=CfgMgr.GoodRunsListSelectionTool("GRLTool",GoodRunsListVec=[
   "NCBanalysis/GRL/data15_13TeV.periodAllYear_HEAD_DQDefects-00-02-02_PHYS_StandardGRL_All_Good.xml",
   "NCBanalysis/GRL/data16_13TeV.periodAllYear_HEAD_DQDefects-00-02-04_PHYS_StandardGRL_All_Good.xml",
   "NCBanalysis/GRL/data17_13TeV.periodAllYear_HEAD_Unknown_PHYS_StandardGRL_All_Good.xml",
   "NCBanalysis/GRL/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml"])

ToolSvc+=CfgMgr.JetCleaningTool("JetCleaningToolLoose",CutLevel="LooseBad")
ToolSvc+=CfgMgr.CP__MuonSelectionTool("MuonSelectionTool", MuQuality=2);
ToolSvc+=CfgMgr.CP__MuonCalibrationAndSmearingTool("MuonCalibrationAndSmearingTool", Year="Data16");

histTool=CfgMgr.HistTool("MisIdProbHist")
histTool.OutputStream="MisIdProb"
ToolSvc+=histTool

if len(svcMgr.EventSelector.InputCollections) == 0:
  svcMgr.EventSelector.InputCollections = ['dummy.root']
print 'svcMgr.EventSelector.InputCollections=',svcMgr.EventSelector.InputCollections
import PyUtils.AthFile as athFile
fi=athFile.fopen(svcMgr.EventSelector.InputCollections[0])
stream_names=fi.fileinfos['stream_names']
print "fileinfos['stream_names']=",stream_names

ntupleWriterTool=CfgMgr.NtupleWriterTool("NtupleWriterTool")
ntupleWriterTool.OutputStream="MisIdProb"
ntupleWriterTool.doEventInfo=True
if 'IS_DATA' in fi.fileinfos['evt_type']:
  ntupleWriterTool.doBunchStructure=True
  ntupleWriterTool.doBeamIntensity=True
  ntupleWriterTool.doGRLTool=True
  ntupleWriterTool.doTrigger=True
  if stream_names == ['StreamAOD']:
    ntupleWriterTool.doBeamBackgroundData=True
    ntupleWriterTool.BeamBackgroundKeys=["BeamBackgroundData"]
    ntupleWriterTool.doTopoClusters=True
    #ntupleWriterTool.TopoClusterEcut=-99999   # disable this cut
    ntupleWriterTool.TopoClusterEcut=10000     # MeV
    ntupleWriterTool.doJets=True
    ntupleWriterTool.doMuonSegments=True
    ntupleWriterTool.doStableBeams=True
  elif stream_names == ['StreamDAOD_EXOT2']:
    ntupleWriterTool.doJets=True
    ntupleWriterTool.doStableBeams=True

if 'IS_SIMULATION' in fi.fileinfos['evt_type']:
  if stream_names == ['StreamAOD']:
    ntupleWriterTool.BeamBackgroundKeys=["BeamBackgroundData"]
    ntupleWriterTool.doBeamBackgroundData=True
    ntupleWriterTool.doBunchStructure=False
    ntupleWriterTool.doBeamIntensity=False
    ntupleWriterTool.doGRLTool=False
    ntupleWriterTool.doStableBeams=False
    ntupleWriterTool.doTopoClusters=True
    #ntupleWriterTool.TopoClusterEcut=-99999  # disable this cut
    ntupleWriterTool.TopoClusterEcut=10000     # MeV
    ntupleWriterTool.doJets=True
    ntupleWriterTool.doMuonSegments=True

ToolSvc+=ntupleWriterTool

from NCBanalysis.NCBanalysisConf import MisIdProb
misIdProb=MisIdProb("MisIdProb")
misIdProb.use_GRL=False

if stream_names == ['StreamAOD']:
  misIdProb.use_electron_requirement=True
  misIdProb.use_muon_requirement=True
elif stream_names == ['StreamDAOD_EXOT2']:
  misIdProb.use_electron_requirement=False
  misIdProb.use_muon_requirement=False
  misIdProb.use_HistTool=False
topSequence+=misIdProb

svcMgr.MessageSvc.OutputLevel = DEBUG
svcMgr.MessageSvc.infoLimit = 0
