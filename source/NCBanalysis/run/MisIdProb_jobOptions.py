import AthenaPoolCnvSvc.ReadAthenaPool

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr+= THistSvc()
svcMgr.THistSvc.Output  = ["MisIdProb DATAFILE='MisIdProb.root' OPT='RECREATE'"];

import glob, os
svcMgr.EventSelector.InputCollections = glob.glob(os.environ['InputCollections'])

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from NCBanalysis.NCBanalysisConf import BeamBackgroundFiller
beamBackgroundFiller1=BeamBackgroundFiller(name="BeamBackgroundFiller1")
beamBackgroundFiller1.BeamBackgroundKey="BeamBackgroundData"
topSequence+=beamBackgroundFiller1

#beamBackgroundFiller2=BeamBackgroundFiller(name="BeamBackgroundFiller2")
#beamBackgroundFiller2.BeamBackgroundKey="BeamBackgroundDataNoMDTSegmentsWitht0EqualTo0"
#beamBackgroundFiller2.useMDTSegmentsWitht0EqualTo0=False
#topSequence+=beamBackgroundFiller2

#beamBackgroundFiller3=BeamBackgroundFiller(name="BeamBackgroundFiller3")
#beamBackgroundFiller3.BeamBackgroundKey="BeamBackgroundDataNoMDT"
#beamBackgroundFiller3.useMDT=False
#topSequence+=beamBackgroundFiller3

ntupleWriterTool=CfgMgr.NtupleWriterTool("NtupleWriterTool")
#ntupleWriterTool.BeamBackgroundKeys=["BeamBackgroundData","BeamBackgroundDataNoMDTSegmentsWitht0EqualTo0","BeamBackgroundDataNoMDT"]
ntupleWriterTool.BeamBackgroundKeys=["BeamBackgroundData"]
ntupleWriterTool.OutputStream="MisIdProb"
ntupleWriterTool.doBunchStructure=True
ntupleWriterTool.doBeamIntensity=True
ntupleWriterTool.doGRLTool=True
ntupleWriterTool.doStableBeams=True
ntupleWriterTool.doEventInfo=True
ntupleWriterTool.doBeamBackgroundData=True
ntupleWriterTool.doTruthEvents=False
ntupleWriterTool.doBCM_RDOs=False
ntupleWriterTool.doPixelClusters=False
ntupleWriterTool.doSCTClusters=False
ntupleWriterTool.doTopoClusters=True
#ntupleWriter.TopoClusterEcut=-99999  # disable this cut
ntupleWriterTool.TopoClusterEcut=5000     # MeV
ntupleWriterTool.doJets=True
ntupleWriterTool.doMuonSegments=True
ToolSvc+=ntupleWriterTool

grlTool=CfgMgr.GoodRunsListSelectionTool(name="GRLTool",GoodRunsListVec=[
   "NCBanalysis/GRL/data15_13TeV.periodAllYear_HEAD_DQDefects-00-02-02_PHYS_StandardGRL_All_Good.xml",
   "NCBanalysis/GRL/data16_13TeV.periodAllYear_HEAD_DQDefects-00-02-04_PHYS_StandardGRL_All_Good.xml",
   "NCBanalysis/GRL/data17_13TeV.periodAllYear_HEAD_Unknown_PHYS_StandardGRL_All_Good.xml",
   "NCBanalysis/GRL/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml"])
ToolSvc+=grlTool

jetClnToolLoose=CfgMgr.JetCleaningTool(name="JetCleaningToolLoose",CutLevel="LooseBad")
ToolSvc+=jetClnToolLoose

muonSelectionToolLoose=CfgMgr.CP__MuonSelectionTool(name="MuonSelectionTool", MuQuality=2);
ToolSvc+=muonSelectionToolLoose

muonCalibrationAndSmearingTool=CfgMgr.CP__MuonCalibrationAndSmearingTool(name="MuonCalibrationAndSmearingTool", Year="Data16");
ToolSvc+=muonCalibrationAndSmearingTool

#tool used by the BeamBackgroundFiller
ToolSvc+=CfgMgr.Muon__MuonIdHelperTool(name="MuonIdHelperTool",OutputLevel=INFO)

histTool=CfgMgr.HistTool("HistTool")
histTool.OutputStream="MisIdProb"
ToolSvc+=histTool

misIdProbAlg=CfgMgr.MisIdProb("MisIdProbAlg")
#misIdProbAlg.GRLTool=grlTool
misIdProbAlg.use_GRL=False
#misIdProbAlg.JetCleaningToolLoose=jetClnToolLoose
#misIdProbAlg.MuonSelectionToolLoose=muonSelectionToolLoose
#misIdProbAlg.MuonCalibrationAndSmearingTool=muonCalibrationAndSmearingTool
#misIdProbAlg.HistTool=histTool
#misIdProbAlg.NtupleWriterTool=ntupleWriterTool
topSequence+=misIdProbAlg

svcMgr.MessageSvc.OutputLevel = DEBUG
svcMgr.MessageSvc.infoLimit = 0
