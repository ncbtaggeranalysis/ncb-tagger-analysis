#include "NCBanalysis/NtupleWriterTool.h"

NtupleWriterTool::NtupleWriterTool(const string& type, const string& name, const IInterface* parent) :
  AthAlgTool(type,name,parent),
  m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_grlTool("GoodRunsListSelectionTool/GRLTool"),
  m_jetCleaningToolLoose("JetCleaningTool/JetCleaningToolLoose"),
  m_bunchStructureTool("BunchStructureTool/BunchStructureTool"),
  m_beamIntensityTool("BeamIntensityTool/BeamIntensityTool"),
  m_runQueryPickleTool("RunQueryPickleTool/RunQueryPickleTool") 
{
  ATH_MSG_INFO("NtupleWriterTool constructor, name=" << name);
  declareProperty("OutputStream", m_outputStream="NtupleWriter");
  declareProperty("CollectionTree", m_treeName="CollectionTree");
  declareProperty("JetCollection", m_jetCollection = "AntiKt4EMTopoJets"); 
 
  declareProperty("doEventInfo", m_doEventInfo = false);
  declareProperty("doTruthEvents", m_doTruthEvents = false);
  declareProperty("doHITS", m_doHITS = false);
  declareProperty("doTrigger", m_doTrigger = false);
  declareProperty("doBCM_RDOs", m_doBCM_RDOs = false);
  declareProperty("doPixelClusters", m_doPixelClusters = false);
  declareProperty("doSCTClusters", m_doSCTClusters = false);
  declareProperty("doTopoClusters", m_doTopoClusters = false);
  declareProperty("TopoClusterEcut", m_topoCluster_e_cut = -99999);
  declareProperty("doJets", m_doJets = false);
  declareProperty("doMuonSegments", m_doMuonSeg = false);
  declareProperty("doMDT_DriftCircles", m_doMDT_DriftCircles = false);
  declareProperty("doMuonPatternCombinations", m_doMuonPatternCombinations = false);
  declareProperty("doMBTScells", m_doMBTScells = false);

  declareProperty("doBunchStructure", m_doBunchStructure = false);
  declareProperty("doBeamIntensity", m_doBeamIntensity = false);
  declareProperty("doBeamBackgroundData", m_doBeamBackgroundData = false);
  declareProperty("BeamBackgroundKeys", m_beamBackgroundKeys = {"BeamBackgroundData"});
  declareProperty("doMCEventCollection", m_doMCEventCollection = false); 
  declareProperty("mcEventCollection", m_mcEventCollection = "GEN_EVENT"); 
  declareProperty("doStableBeams", m_doStableBeams = false);

  declareProperty("doGRLTool", m_doGRLTool = false);
  //declareProperty("GRLTool", m_grlTool);
  //declareProperty("JetCleaningToolLoose", m_jetCleaningToolLoose);

  m_beamBackgroundData = 0;

}

NtupleWriterTool::~NtupleWriterTool() {

}

StatusCode NtupleWriterTool::initialize() {
  ATH_MSG_INFO(/*name() << */"NtupleWriterTool initialize");

  ATH_CHECK(service("THistSvc",m_thistSvc));

  m_tree = new TTree(m_treeName.data(), "");

  if (m_doEventInfo) {
    m_tree->Branch("RunNumber", &m_runNumber);
    m_tree->Branch("EventNumber", &m_eventNumber);
    m_tree->Branch("bcid", &m_bcid);
    m_tree->Branch("lbn", &m_lbn);
    m_tree->Branch("backgroundFlag", &m_backgroundFlag);
    m_tree->Branch("NPV", &m_npv);
    m_tree->Branch("averageInteractionsPerCrossing", &m_averageInteractionsPerCrossing);
    m_tree->Branch("pixelError", &m_pixelError);
    m_tree->Branch("sctError", &m_sctError);
    m_tree->Branch("trtError", &m_trtError);
    m_tree->Branch("larError", &m_larError);
    m_tree->Branch("tileError", &m_tileError);
    m_tree->Branch("muonError", &m_muonError); 
  } 

  if (m_doTruthEvents)
     m_tree->Branch("TruthEvent.weights",&m_truthEvent_weights);
  
  if (m_doMCEventCollection) {
     TString mcEventCollection = m_mcEventCollection;
     m_tree->Branch(mcEventCollection + ".n", &m_mc_n);
     m_tree->Branch(mcEventCollection + ".pdg_id","vector<int>",&m_mc_pdg_id);
     m_tree->Branch(mcEventCollection + ".status","vector<int>",&m_mc_status);
     m_tree->Branch(mcEventCollection + ".eventWeights","vector<double>",&m_mc_weights);

     m_tree->Branch(mcEventCollection + ".momentum_px","vector<float>", &m_mc_momentum_px);
     m_tree->Branch(mcEventCollection + ".momentum_py","vector<float>", &m_mc_momentum_py);
     m_tree->Branch(mcEventCollection + ".momentum_pz","vector<float>", &m_mc_momentum_pz);
     m_tree->Branch(mcEventCollection + ".momentum_e","vector<float>",&m_mc_momentum_e);
     m_tree->Branch(mcEventCollection + ".momentum_rho","vector<float>", &m_mc_momentum_rho);

     m_tree->Branch(mcEventCollection + ".vertex_point3d_x","vector<float>",&m_mc_vertex_point3d_x);
     m_tree->Branch(mcEventCollection + ".vertex_point3d_y","vector<float>",&m_mc_vertex_point3d_y);
     m_tree->Branch(mcEventCollection + ".vertex_point3d_z","vector<float>",&m_mc_vertex_point3d_z);

     m_tree->Branch(mcEventCollection + ".vertex_position_x","vector<float>",&m_mc_vertex_position_x);
     m_tree->Branch(mcEventCollection + ".vertex_position_y","vector<float>",&m_mc_vertex_position_y);
     m_tree->Branch(mcEventCollection + ".vertex_position_z","vector<float>",&m_mc_vertex_position_z);
     m_tree->Branch(mcEventCollection + ".vertex_position_t","vector<float>",&m_mc_vertex_position_t);
     m_tree->Branch(mcEventCollection + ".vertex_position_theta","vector<float>",&m_mc_vertex_position_theta);
     m_tree->Branch(mcEventCollection + ".vertex_position_phi","vector<float>",&m_mc_vertex_position_phi);
     m_tree->Branch(mcEventCollection + ".vertex_position_eta","vector<float>",&m_mc_vertex_position_eta);
  }

  if (m_doHITS) {
    m_tree->Branch("SiHits.n","vector<int>",&m_siHits_n);
    m_tree->Branch("SiHits.id","vector<vector<unsigned long int>>",&m_siHits_id);
    m_tree->Branch("SiHits.meanTime","vector<vector<float>>", &m_siHits_meanTime);
    m_tree->Branch("SiHits.energyLoss","vector<vector<float>>", &m_siHits_energyLoss);
    m_tree->Branch("SiHits.localStartPositionX","vector<vector<float>>", &m_siHits_localStartPositionX);
    m_tree->Branch("SiHits.localStartPositionY","vector<vector<float>>", &m_siHits_localStartPositionY);
    m_tree->Branch("SiHits.localStartPositionZ","vector<vector<float>>", &m_siHits_localStartPositionZ);
    m_tree->Branch("SiHits.globalPositionX","vector<vector<float>>", &m_siHits_globalPositionX);
    m_tree->Branch("SiHits.globalPositionY","vector<vector<float>>", &m_siHits_globalPositionY);
    m_tree->Branch("SiHits.globalPositionZ","vector<vector<float>>", &m_siHits_globalPositionZ);
    m_tree->Branch("SiHits.barrelEndcap","vector<vector<short int>>", &m_siHits_barrelEndcap);
    m_tree->Branch("SiHits.layerDisk","vector<vector<short int>>", &m_siHits_layerDisk);
    m_tree->Branch("SiHits.etaModule","vector<vector<short int>>", &m_siHits_etaModule);
    m_tree->Branch("SiHits.phiModule","vector<vector<short int>>", &m_siHits_phiModule);
    m_tree->Branch("SiHits.side","vector<vector<short int>>", &m_siHits_side);
    m_tree->Branch("SiHits.layer_disk","vector<vector<short int>>",&m_siHits_layer_disk);
    m_tree->Branch("SiHits.isPixel","vector<vector<short int>>",&m_siHits_isPixel); 
    m_tree->Branch("SiHits.isSCT","vector<vector<short int>>",&m_siHits_isSCT); 

    m_tree->Branch("SiHits.PixelID.eta_index","vector<vector<short int>>",&m_siHits_eta_index); // description of eta_index and phi_index in PixelID.h
    m_tree->Branch("SiHits.PixelID.phi_index","vector<vector<short int>>",&m_siHits_phi_index);
    m_tree->Branch("SiHits.PixelID.is_blayer","vector<vector<short int>>",&m_siHits_is_blayer);
    m_tree->Branch("SiHits.PixelID.is_barrel","vector<vector<short int>>",&m_siHits_is_barrel);

    m_tree->Branch("TRTHits.n", &m_trtHits_n, "m_trtHits_n/I");

    m_tree->Branch("LArHits.n","vector<int>",&m_larHits_n);
    m_tree->Branch("LArHits.energy","vector<vector<float>>",&m_larHits_energy);
    m_tree->Branch("LArHits.time","vector<vector<float>>",&m_larHits_time);
    m_tree->Branch("LArHits.CaloDetDescrElement.x","vector<vector<float>>",&m_larHits_CaloDetDescrElement_x);
    m_tree->Branch("LArHits.CaloDetDescrElement.y","vector<vector<float>>",&m_larHits_CaloDetDescrElement_y);
    m_tree->Branch("LArHits.CaloDetDescrElement.z","vector<vector<float>>",&m_larHits_CaloDetDescrElement_z);
    m_tree->Branch("LArHits.SamplingLayer","vector<vector<int>>",&m_larHits_samplingLayer);

    m_tree->Branch("LArCalibrationHits.n","vector<int>", &m_larCalibrationHits_n);

    m_tree->Branch("TileHits.n",&m_tileHits_n,"m_tileHits_n/I");
    m_tree->Branch("MBTSHits.n",&m_mbtsHits_n,"m_mbtsHits_n/I");

    m_tree->Branch("MDTHits.n",&m_tileHits_n,"m_mdtHits_n/I");
    m_tree->Branch("MDTHits.globalPosition.X","vector<float>",&m_mdtHits_globalPositionX);
    m_tree->Branch("MDTHits.globalPosition.Y","vector<float>",&m_mdtHits_globalPositionY);
    m_tree->Branch("MDTHits.globalPosition.Z","vector<float>",&m_mdtHits_globalPositionZ);

    m_tree->Branch("CSCHits.n",&m_cscHits_n,"m_cscHits_n/I");
    m_tree->Branch("CSCHits.globalPosition.X","vector<float>",&m_cscHits_globalPositionX);
    m_tree->Branch("CSCHits.globalPosition.Y","vector<float>",&m_cscHits_globalPositionY);
    m_tree->Branch("CSCHits.globalPosition.Z","vector<float>",&m_cscHits_globalPositionZ);

    m_tree->Branch("TGCHits.n",&m_tgcHits_n,"m_tgcHits_n/I");

    m_tree->Branch("RPCHits.n",&m_rpcHits_n,"m_rpcHits_n/I");

    if (detStore()->retrieve(m_PixelID, "PixelID").isFailure()) {
      ATH_MSG_INFO( "failed to retrieve the PixelID");
      return StatusCode::FAILURE;
    } 
  }

  if (m_doGRLTool)
    m_tree->Branch("passGRL", &m_passGRL); 

  if (m_doTrigger) {

     m_trigItem_name = {
       
     "L1_BCM_AC_CA_BGRP0",
     "L1_BCM_AC_CA_UNPAIRED_ISO",    

     "L1_BCM_AC_UNPAIRED_ISO",
     "L1_BCM_AC_UNPAIRED_NONISO",
     "L1_BCM_AC_CALIB",
     "L1_BCM_AC_ABORTGAPNOTCALIB",    

     "L1_BCM_CA_UNPAIRED_ISO",
     "L1_BCM_CA_UNPAIRED_NONISO",
     "L1_BCM_CA_CALIB",
     "L1_BCM_CA_ABORTGAPNOTCALIB",

     "L1_BCM_Wide_BGRP0",
     "L1_BCM_Wide_EMPTY",
     "L1_BCM_Wide_UNPAIRED_ISO",
     "L1_BCM_Wide_UNPAIRED_NONISO",
     "L1_BCM_Wide_CALIB",
     "L1_BCM_Wide_ABORTGAPNOTCALIB",

     "L1_J12",
     "L1_J12_UNPAIRED_ISO",
     "L1_J12_UNPAIRED_NONISO",
     "L1_J50_UNPAIRED_ISO",
     "L1_J50_UNPAIRED_NONISO",

     "L1_MBTS_1_UNPAIRED_ISO", // MinBias stream
 
     "L1_MBTS_2_UNPAIRED_ISO",
     "L1_MBTS_4_A_UNPAIRED_ISO",
     "L1_MBTS_4_C_UNPAIRED_ISO",

     "L1_RD0_UNPAIRED_ISO",
     "HLT_noalg_bkg_L1RD0_UNPAIRED_ISO",
     "HLT_mb_sp_L1RD0_UNPAIRED_ISO",
     "HLT_mb_sp_ncb_L1RD0_UNPAIRED_ISO",

     "L1_MU10",
     "L1_MU20",
     "HLT_mu10",
     "HLT_mu20",
     "HLT_mu60",
     "HLT_mu80"
     };

     for (TString trigItem : m_trigItem_name)
       m_trigItem_isPassed.push_back(0);

     for (unsigned int i=0;i<m_trigItem_name.size();i++)
       m_tree->Branch(m_trigItem_name[i],&m_trigItem_isPassed[i]);
    
   }

   if (m_doStableBeams)
     m_tree->Branch("StableBeams",&m_stableBeams);

   if (m_doBCM_RDOs) {
     m_tree->Branch("BCM_RDO_Container_nCollections",&m_BCM_RDO_Container_nCollections);

     m_tree->Branch("BCM_RDO_Collection_nBCM_RDOs", "vector<unsigned int>",&m_BCM_RDO_Collection_nBCM_RDOs);
     m_tree->Branch("BCM_RDO_Collection_channel", "vector<unsigned int>",&m_BCM_RDO_Collection_channel);

     m_tree->Branch("BCM_RawData_word1", "vector<int>", &m_BCM_RawData_word1);
     m_tree->Branch("BCM_RawData_word2", "vector<int>", &m_BCM_RawData_word2);
     m_tree->Branch("BCM_RawData_channel", "vector<int>", &m_BCM_RawData_channel);
     m_tree->Branch("BCM_RawData_Pulse1Position","vector<int>",&m_BCM_RawData_pulse1Position);
     m_tree->Branch("BCM_RawData_Pulse1Width","vector<int>",&m_BCM_RawData_pulse1Width);
     m_tree->Branch("BCM_RawData_Pulse2Position","vector<int>",&m_BCM_RawData_pulse2Position);
     m_tree->Branch("BCM_RawData_Pulse2Width","vector<int>",&m_BCM_RawData_pulse2Width);
     m_tree->Branch("BCM_RawData_LVL1A","vector<int>",&m_BCM_RawData_LVL1A);
     m_tree->Branch("BCM_RawData_BCID","vector<int>",&m_BCM_RawData_BCID);
     m_tree->Branch("BCM_RawData_LVL1ID","vector<int>",&m_BCM_RawData_LVL1ID);
     m_tree->Branch("BCM_RawData_Error","vector<int>",&m_BCM_RawData_Error);
   }

   if (m_doPixelClusters) {
     m_tree->Branch("PixelClusters.n", &m_pixCl_n);
     m_tree->Branch("PixelClusters.globalPositionX","vector<float>",&m_pixCl_globalPosX);
     m_tree->Branch("PixelClusters.globalPositionY","vector<float>",&m_pixCl_globalPosY);
     m_tree->Branch("PixelClusters.globalPositionZ","vector<float>",&m_pixCl_globalPosZ); 
     m_tree->Branch("PixelClusters.eta_pixel_index","vector<int>",&m_pixCl_eta_pixel_index);
     m_tree->Branch("PixelClusters.phi_pixel_index","vector<int>",&m_pixCl_phi_pixel_index);
     m_tree->Branch("PixelClusters.eta_module","vector<int>",&m_pixCl_eta_module);
     m_tree->Branch("PixelClusters.phi_module","vector<int>",&m_pixCl_phi_module);
     m_tree->Branch("PixelClusters.sizePhi","vector<int>",&m_pixCl_sizePhi);
     m_tree->Branch("PixelClusters.sizeZ","vector<int>",&m_pixCl_sizeZ);
     m_tree->Branch("PixelClusters.charge","vector<float>",&m_pixCl_charge);
     m_tree->Branch("PixelClusters.isFake","vector<char>",&m_pixCl_isFake);
     m_tree->Branch("PixelClusters.isSplit","vector<int>",&m_pixCl_isSplit);
     m_tree->Branch("PixelClusters.ToT","vector<int>",&m_pixCl_ToT);
     m_tree->Branch("PixelClusters.nRDO","vector<int>",&m_pixCl_nRDO);
     m_tree->Branch("PixelClusters.bec","vector<int>",&m_pixCl_bec);
   }  
 
   if (m_doSCTClusters) {
     m_tree->Branch("SCT_Clusters.n", &m_sctCl_n);
     m_tree->Branch("SCT_Clusters.globalPositionX","vector<float>",&m_sctCl_globalPosX);
     m_tree->Branch("SCT_Clusters.globalPositionY","vector<float>",&m_sctCl_globalPosY);
     m_tree->Branch("SCT_Clusters.globalPositionZ","vector<float>",&m_sctCl_globalPosZ); 
     m_tree->Branch("SCT_Clusters.bec","vector<int>",&m_sctCl_bec);
     m_tree->Branch("SCT_Clusters.eta_module","vector<int>",&m_sctCl_eta_module);
     m_tree->Branch("SCT_Clusters.phi_module","vector<int>",&m_sctCl_phi_module);
     m_tree->Branch("SCT_Clusters.SiWidth","vector<int>",&m_sctCl_SiWidth);
     m_tree->Branch("SCT_Clusters.rdo_strip","vector<vector<int>>",&m_sctCl_rdo_strip);
     m_tree->Branch("SCT_Clusters.rdo_timebin","vector<vector<int>>",&m_sctCl_rdo_timebin);
     m_tree->Branch("SCT_Clusters.side","vector<int>",&m_sctCl_side);
     m_tree->Branch("SCT_Clusters.layer","vector<int>",&m_sctCl_layer);
   }

   if (m_doTopoClusters) {
     m_tree->Branch("TopoClusters.n",&m_cl_n,"cl_n/i");
     m_tree->Branch("TopoClusters.pt","vector<float>",&m_cl_pt);
     m_tree->Branch("TopoClusters.e","vector<float>",&m_cl_e);
     m_tree->Branch("TopoClusters.emscale_e","vector<float>",&m_cl_emscale_e);
     m_tree->Branch("TopoClusters.eta","vector<float>",&m_cl_eta); 
     m_tree->Branch("TopoClusters.phi","vector<float>",&m_cl_phi);  
     m_tree->Branch("TopoClusters.rPerp","vector<float>",&m_cl_rPerp);
     m_tree->Branch("TopoClusters.z","vector<float>",&m_cl_z);
     m_tree->Branch("TopoClusters.time","vector<float>",&m_cl_time);
     m_tree->Branch("TopoClusters.fracSamplingMax","vector<float>",&m_cl_fracSamplingMax);
     m_tree->Branch("TopoClusters.fracSamplingMaxIndex","vector<int>",&m_cl_fracSamplingMaxIndex);  
   }

   if (m_doJets) {
     TString jetCollection = m_jetCollection.data();
     m_tree->Branch(jetCollection+".n", &m_jets_n,"n/i");
     m_tree->Branch(jetCollection+".pt","vector<float>",&m_jets_pt);
     m_tree->Branch(jetCollection+".eta","vector<float>",&m_jets_eta);
     m_tree->Branch(jetCollection+".phi","vector<float>",&m_jets_phi);
     m_tree->Branch(jetCollection+".time","vector<float>",&m_jets_time);
     m_tree->Branch(jetCollection+".emf","vector<float>",&m_jets_emf);
     m_tree->Branch(jetCollection+".hecf","vector<float>",&m_jets_hecf);
     m_tree->Branch(jetCollection+".hecq","vector<float>",&m_jets_hecq);
     m_tree->Branch(jetCollection+".larq","vector<float>",&m_jets_larq);
     m_tree->Branch(jetCollection+".AverageLArQF","vector<float>",&m_jets_AverageLArQF);
     m_tree->Branch(jetCollection+".negE","vector<float>",&m_jets_negE);
     m_tree->Branch(jetCollection+".fracSamplingMax","vector<float>",&m_jets_fracSamplingMax);
     m_tree->Branch(jetCollection+".fracSamplingMaxIndex","vector<int>",&m_jets_fracSamplingMaxIndex);
     //m_tree->Branch(jetCollection+".width","vector<float>",&m_jets_width);
     //m_tree->Branch(jetCollection+".n90","vetor<int>",&m_jets_n90);
     m_tree->Branch(jetCollection+".NumTrkPt1000","vector<int>",&m_jets_numTrkPt1000);
     m_tree->Branch(jetCollection+".SumPtTrkPt500","vector<float>", &m_jets_SumPtTrkPt500);
     m_tree->Branch(jetCollection+".isBadLoose","vector<bool>",&m_jets_isBadLoose);
   }  
  
   if (m_doMuonSeg) {
     m_tree->Branch("NCB_MuonSegments.n",&m_NCB_MuonSeg_n);
     m_tree->Branch("NCB_MuonSegments.x","vector<float>",&m_NCB_MuonSeg_x);
     m_tree->Branch("NCB_MuonSegments.y","vector<float>",&m_NCB_MuonSeg_y);
     m_tree->Branch("NCB_MuonSegments.z","vector<float>",&m_NCB_MuonSeg_z);
     m_tree->Branch("NCB_MuonSegments.px","vector<float>",&m_NCB_MuonSeg_px);
     m_tree->Branch("NCB_MuonSegments.py","vector<float>",&m_NCB_MuonSeg_py);
     m_tree->Branch("NCB_MuonSegments.pz","vector<float>",&m_NCB_MuonSeg_pz); 
     m_tree->Branch("NCB_MuonSegments.t0","vector<float>",&m_NCB_MuonSeg_t0);
     m_tree->Branch("NCB_MuonSegments.t0error","vector<float>",&m_NCB_MuonSeg_t0error);
     m_tree->Branch("NCB_MuonSegments.chi2","vector<float>",&m_NCB_MuonSeg_chi2);
     m_tree->Branch("NCB_MuonSegments.ndof","vector<float>",&m_NCB_MuonSeg_ndof);
     m_tree->Branch("NCB_MuonSegments.sector","vector<int>",&m_NCB_MuonSeg_sector);
     m_tree->Branch("NCB_MuonSegments.chamberIndex","vector<int>",&m_NCB_MuonSeg_chamberIndex);
     m_tree->Branch("NCB_MuonSegments.technology","vector<int>",&m_NCB_MuonSeg_technology);
     m_tree->Branch("NCB_MuonSegments.nPrecisionHits","vector<int>",&m_NCB_MuonSeg_nPrecisionHits);
     m_tree->Branch("NCB_MuonSegments.thetaPos","vector<float>",&m_NCB_MuonSeg_thetaPos);
     m_tree->Branch("NCB_MuonSegments.thetaDir","vector<float>",&m_NCB_MuonSeg_thetaDir);
     m_tree->Branch("NCB_MuonSegments.phi","vector<float>",&m_NCB_MuonSeg_phi);

     m_tree->Branch("MuonSegments.n",&m_MuonSeg_n);
     m_tree->Branch("MuonSegments.x","vector<float>",&m_MuonSeg_x);
     m_tree->Branch("MuonSegments.y","vector<float>",&m_MuonSeg_y);
     m_tree->Branch("MuonSegments.z","vector<float>",&m_MuonSeg_z);
     m_tree->Branch("MuonSegments.px","vector<float>",&m_MuonSeg_px);
     m_tree->Branch("MuonSegments.py","vector<float>",&m_MuonSeg_py);
     m_tree->Branch("MuonSegments.pz","vector<float>",&m_MuonSeg_pz); 
     m_tree->Branch("MuonSegments.t0","vector<float>",&m_MuonSeg_t0);
     m_tree->Branch("MuonSegments.t0error","vector<float>",&m_MuonSeg_t0error);
     m_tree->Branch("MuonSegments.chi2","vector<float>",&m_MuonSeg_chi2);
     m_tree->Branch("MuonSegments.ndof","vector<float>",&m_MuonSeg_ndof);
     m_tree->Branch("MuonSegments.sector","vector<int>",&m_MuonSeg_sector);
     m_tree->Branch("MuonSegments.chamberIndex","vector<int>",&m_MuonSeg_chamberIndex);
     m_tree->Branch("MuonSegments.technology","vector<int>",&m_MuonSeg_technology);
     m_tree->Branch("MuonSegments.nPrecisionHits","vector<int>",&m_MuonSeg_nPrecisionHits);
     m_tree->Branch("MuonSegments.thetaPos","vector<float>",&m_MuonSeg_thetaPos);
     m_tree->Branch("MuonSegments.thetaDir","vector<float>",&m_MuonSeg_thetaDir);
     m_tree->Branch("MuonSegments.phi","vector<float>",&m_MuonSeg_phi);
   }

   if (m_doMDT_DriftCircles) {
     m_tree->Branch("MdtPrepData.MDT_DriftCircles.n",&m_MDT_DriftCircles_n);
     m_tree->Branch("MdtPrepData.MDT_DriftCircles.tdc","vector<int>",&m_MDT_DriftCircles_tdc);
     m_tree->Branch("MdtPrepData.MDT_DriftCircles.adc","vector<int>",&m_MDT_DriftCircles_adc);
     m_tree->Branch("MdtPrepData.MDT_DriftCircles.status","vector<int>",&m_MDT_DriftCircles_status);
     m_tree->Branch("MdtPrepData.MDT_DriftCircles.globalPositionX","vector<float>",&m_MDT_DriftCircles_globalPositionX);
     m_tree->Branch("MdtPrepData.MDT_DriftCircles.globalPositionY","vector<float>",&m_MDT_DriftCircles_globalPositionY);
     m_tree->Branch("MdtPrepData.MDT_DriftCircles.globalPositionZ","vector<float>",&m_MDT_DriftCircles_globalPositionZ);
   }

   if (m_doMuonPatternCombinations) {
     m_tree->Branch("MuonPatternCombinations_n",&m_MuonPatternCombination_n);
     m_tree->Branch("MuonPatternCombinations.trackParameter.X","vector<float>",&m_MuonPatternCombination_trackParameter_X);
     m_tree->Branch("MuonPatternCombinations.trackParameter.Y","vector<float>",&m_MuonPatternCombination_trackParameter_Y);
     m_tree->Branch("MuonPatternCombinations.trackParameter.Z","vector<float>",&m_MuonPatternCombination_trackParameter_Z);
     m_tree->Branch("MuonPatternCombinations.trackParameter.pX","vector<float>",&m_MuonPatternCombination_trackParameter_pX);
     m_tree->Branch("MuonPatternCombinations.trackParameter.pY","vector<float>",&m_MuonPatternCombination_trackParameter_pY);
     m_tree->Branch("MuonPatternCombinations.trackParameter.pZ","vector<float>",&m_MuonPatternCombination_trackParameter_pZ);
     m_tree->Branch("MuonPatternCombinations.trackParameter.pT","vector<float>",&m_MuonPatternCombination_trackParameter_pT);
     m_tree->Branch("MuonPatternCombinations.trackParameter.charge","vector<float>",&m_MuonPatternCombination_trackParameter_charge);
     m_tree->Branch("MuonPatternCombinations.trackRoadType","vector<int>",&m_MuonPatternCombination_trackRoadType);
   }

   if (m_doMuonSegmentCombinations) {
     
   }

   if (m_doBeamBackgroundData)
     for (unsigned int i=0;i<m_beamBackgroundKeys.size();i++) {
       //m_tree->Branch(m_beamBackgroundKeys[i]+".backgroundFlag",&m_beamBackgroundData_backgroundFlag[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numSegment").data(),&m_beamBackgroundData_numSegment[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numSegmentEarly").data(),&m_beamBackgroundData_numSegmentEarly[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numSegmentACNoTime").data(),&m_beamBackgroundData_numSegmentACNoTime[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numSegmentAC").data(),&m_beamBackgroundData_numSegmentAC[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numMatched").data(),&m_beamBackgroundData_numMatched[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numNoTimeLoose").data(),&m_beamBackgroundData_numNoTimeLoose[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numNoTimeMedium").data(),&m_beamBackgroundData_numNoTimeMedium[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numNoTimeTight").data(),&m_beamBackgroundData_numNoTimeTight[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numOneSidedLoose").data(),&m_beamBackgroundData_numOneSidedLoose[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numOneSidedMedium").data(),&m_beamBackgroundData_numOneSidedMedium[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numOneSidedTight").data(),&m_beamBackgroundData_numOneSidedTight[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numTwoSidedNoTime").data(),&m_beamBackgroundData_numTwoSidedNoTime[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".numTwoSided").data(),&m_beamBackgroundData_numTwoSided[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".resultClus").data(),"vector<int>",&m_beamBackgroundData_resultClus[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".resultMuonSeg").data(),"vector<int>",&m_beamBackgroundData_resultMuonSeg[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".resultNCB_MuonSeg").data(),"vector<int>",&m_beamBackgroundData_resultNCB_MuonSeg[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".timeDiffAC_SegmentACNoTime").data(),"vector<float>",&m_beamBackgroundData_timeDiffAC_SegmentACNoTime[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".timeDiffAC_SegmentAC").data(),"vector<float>",&m_beamBackgroundData_timeDiffAC_SegmentAC[i]);
       m_tree->Branch((m_beamBackgroundKeys[i]+".timeDiffAC_TwoSidedNoTime").data(),"vector<float>",&m_beamBackgroundData_timeDiffAC_TwoSidedNoTime[i]); 
       m_tree->Branch((m_beamBackgroundKeys[i]+".timeDiffAC_TwoSided").data(),"vector<float>",&m_beamBackgroundData_timeDiffAC_TwoSided[i]);
     }

   if (m_doBeamIntensity) {
     m_tree->Branch("beamIntensity_B1",&m_beamIntensity_B1);
     m_tree->Branch("beamIntensity_B2",&m_beamIntensity_B2);
     m_tree->Branch("lb_duration", &m_lb_duration);
   }

   if (m_doBunchStructure) {
     m_tree->Branch("BunchStructure.isUnpaired",&m_bs_isUnpaired);
     m_tree->Branch("BunchStructure.isUnpairedB1",&m_bs_isUnpairedB1); 
     m_tree->Branch("BunchStructure.isUnpairedB2",&m_bs_isUnpairedB2);
     m_tree->Branch("BunchStructure.isUnpairedIsolated",&m_bs_isUnpairedIsolated); 
     m_tree->Branch("BunchStructure.isColliding",&m_bs_isColliding);
     m_tree->Branch("BunchStructure.isCollidingFirst",&m_bs_isCollidingFirst);
     m_tree->Branch("BunchStructure.isEmpty",&m_bs_isEmpty); 
     m_tree->Branch("BunchStructure.stableBeams",&m_bs_stableBeams); 
     m_tree->Branch("BunchStructure.numBunchesColliding",&m_bs_numBunchesColliding);
     m_tree->Branch("BunchStructure.numBunchesUnpairedB1",&m_bs_numBunchesUnpairedB1);
     m_tree->Branch("BunchStructure.numBunchesUnpairedB2",&m_bs_numBunchesUnpairedB2);
   }

   if (m_thistSvc->regTree("/"+m_outputStream+"/CollectionTree",m_tree).isFailure()) {
     ATH_MSG_INFO("failed to register tree");
     return StatusCode::FAILURE;
   }
   else ATH_MSG_INFO("NtupleWriterTool tree registered successfully");


   return StatusCode::SUCCESS;
}

StatusCode NtupleWriterTool::Fill() {

  if (m_doEventInfo) {

    const xAOD::EventInfo* eventInfo = 0;
    if (evtStore()->retrieve( eventInfo, "EventInfo").isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve EventInfo" );
      return StatusCode::FAILURE;
    }

    m_runNumber = eventInfo->runNumber();
    m_eventNumber = eventInfo->eventNumber();
    m_bcid = eventInfo->bcid();
    m_lbn = eventInfo->lumiBlock();
    m_backgroundFlag = eventInfo->eventFlags(xAOD::EventInfo::Background);
    m_averageInteractionsPerCrossing = eventInfo->averageInteractionsPerCrossing();
    m_pixelError = eventInfo->errorState(xAOD::EventInfo::Pixel);
    m_sctError = eventInfo->errorState(xAOD::EventInfo::SCT);
    m_trtError = eventInfo->errorState(xAOD::EventInfo::TRT);
    m_larError = eventInfo->errorState(xAOD::EventInfo::LAr);
    m_tileError = eventInfo->errorState(xAOD::EventInfo::Tile);
    m_muonError = eventInfo->errorState(xAOD::EventInfo::Muon);   
 
    const xAOD::VertexContainer* primaryVertices = 0;
    if (evtStore()->retrieve( primaryVertices, "PrimaryVertices").isFailure()) 
      ATH_MSG_ERROR( "Failed to retrieve PrimaryVertices" );
    else {
      m_npv=0;
      for (size_t iVertex=0; iVertex< primaryVertices->size(); iVertex++)
        if ( primaryVertices->at(iVertex)->vertexType() == xAOD::VxType::VertexType::PriVtx || 
             primaryVertices->at(iVertex)->vertexType() == xAOD::VxType::VertexType::PileUp)
          m_npv++;
    }
  }

  if (m_doGRLTool) 
    m_passGRL = m_grlTool->passRunLB(m_runNumber, m_lbn);

  if (m_doTruthEvents) {
    //TruthEvents (DataVector<xAOD::TruthEvent_v1>)
    const xAOD::TruthEventContainer* truthEventContainer;
    if (evtStore()->retrieve(truthEventContainer, "TruthEvents").isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve the TruthEvents" );
      return StatusCode::FAILURE;
    }
    
    if (truthEventContainer->size() != 1) {
      ATH_MSG_ERROR( "expected TruthEventContainer size different from 1");
      return StatusCode::FAILURE;
    }
    const xAOD::TruthEvent* truthEvent = truthEventContainer->at(0);
    m_truthEvent_weights.clear();
    for (float weight : truthEvent->weights()) 
      m_truthEvent_weights.push_back(weight);
  }

  if (m_doMCEventCollection) {
    const DataHandle<McEventCollection> mcCollptr;

    if ( evtStore()->retrieve(mcCollptr, m_mcEventCollection).isFailure() ) {
      ATH_MSG_INFO ( "cannot retrieve McEventCollection with key " << m_mcEventCollection);
       return StatusCode::SUCCESS;
    }
 
    m_mc_pdg_id.clear();
    m_mc_status.clear();
    m_mc_weights.clear();

    m_mc_momentum_px.clear();
    m_mc_momentum_py.clear();
    m_mc_momentum_pz.clear();
    m_mc_momentum_e.clear();  
    m_mc_momentum_rho.clear();  
 
    m_mc_vertex_point3d_x.clear();
    m_mc_vertex_point3d_y.clear();
    m_mc_vertex_point3d_z.clear();

    m_mc_vertex_position_x.clear();
    m_mc_vertex_position_y.clear();
    m_mc_vertex_position_z.clear();
    m_mc_vertex_position_t.clear();
    m_mc_vertex_position_theta.clear();
    m_mc_vertex_position_phi.clear();
    m_mc_vertex_position_eta.clear();

    McEventCollection::const_iterator itr;
    if (mcCollptr->size() != 1) {
      ATH_MSG_INFO ("mcCollPtr expected to have dimension of 1");
      return StatusCode::FAILURE;
    }
  
    itr = mcCollptr->begin(); 
    //ATH_MSG_DEBUG( "  event_number="<<(*itr)->event_number());;

    HepMC::GenEvent::particle_const_iterator itrPart;
    m_mc_n = (*itr)->particles_size(); 
    //ATH_MSG_DEBUG ( "m_n=" << m_n );
    for (itrPart = (*itr)->particles_begin(); itrPart!=(*itr)->particles_end(); ++itrPart ) {
      // GenParticle does not have a time value.
      HepMC::GenParticle *part=*itrPart;
      //afs/cern.ch/sw/lcg/external/HepMC/2.03.06/slc3_ia32_gcc344/include/HepMC/GenParticle.h
      ///afs/cern.ch/sw/lcg/external/HepMC/2.03.06/src/HepMC-2.03.06/src/
      //ATH_MSG_DEBUG("pdg_id="<<part->pdg_id() 
      //               << " status=" << part->status()
      //               << " px=" << part->momentum().px()<< " "
      //               << " py=" << part->momentum().py()<< " "
      //               << " pz=" << part->momentum().pz()<< " "  
      //               << " e(total energy)="  << part->momentum().e() << " MeV" );
      //ATH_MSG_DEBUG("parent_event:");
      //part->parent_event()->print(); 
      //ATH_MSG_INFO("production_vertex:");
      //ATH_MSG_INFO("  #in particles="<<part->production_vertex()->particles_in_size());
      //ATH_MSG_INFO("  #out particles="<<part->production_vertex()->particles_out_size());
      //ATH_MSG_INFO("  (x,y,z)="<<part->production_vertex()->point3d().x()<<", "<<part->production_vertex()->point3d().y()<<", "<<part->production_vertex()->point3d().z()); 
      //part->production_vertex()->print();
      //part->print();

      m_mc_pdg_id.push_back(part->pdg_id());
      m_mc_status.push_back(part->status());

      m_mc_momentum_px.push_back(part->momentum().px());
      m_mc_momentum_py.push_back(part->momentum().py());
      m_mc_momentum_pz.push_back(part->momentum().pz());
      m_mc_momentum_e.push_back(part->momentum().e());
      m_mc_momentum_rho.push_back(part->momentum().rho());

      m_mc_vertex_point3d_x.push_back(part->production_vertex()->point3d().x());
      m_mc_vertex_point3d_y.push_back(part->production_vertex()->point3d().y()); 
      m_mc_vertex_point3d_z.push_back(part->production_vertex()->point3d().z());
 
      m_mc_vertex_position_x.push_back(part->production_vertex()->position().x());
      m_mc_vertex_position_y.push_back(part->production_vertex()->position().y());
      m_mc_vertex_position_z.push_back(part->production_vertex()->position().z());
      m_mc_vertex_position_t.push_back(part->production_vertex()->position().t());
      m_mc_vertex_position_theta.push_back(part->production_vertex()->position().theta());
      m_mc_vertex_position_phi.push_back(part->production_vertex()->position().phi());
      m_mc_vertex_position_eta.push_back(part->production_vertex()->position().eta());
    }

    for (double w : (*itr)->weights() )
      m_mc_weights.push_back(w);

  }

  if (m_doHITS) {
   
    m_siHits_n.clear();
    m_siHits_id.clear();
    m_siHits_energyLoss.clear();
    m_siHits_meanTime.clear();
    m_siHits_localStartPositionX.clear();
    m_siHits_localStartPositionY.clear();
    m_siHits_localStartPositionZ.clear();
    m_siHits_globalPositionX.clear();
    m_siHits_globalPositionY.clear();
    m_siHits_globalPositionZ.clear();
    m_siHits_barrelEndcap.clear();
    m_siHits_layerDisk.clear();
    m_siHits_etaModule.clear();
    m_siHits_phiModule.clear();
    m_siHits_side.clear();
    m_siHits_eta_index.clear();
    m_siHits_phi_index.clear();
    m_siHits_is_barrel.clear();
    m_siHits_layer_disk.clear();
    m_siHits_is_blayer.clear();
    m_siHits_isPixel.clear(); 
    m_siHits_isSCT.clear();   

    TString SiHitContainers[4] = {"BCMHits","BLMHits", "PixelHits","SCT_Hits"};
 
    const SiHitCollection* siHits[4];
    for (unsigned int i=0;i<4;i++) {

      if (evtStore()->retrieve(siHits[i], SiHitContainers[i].Data()).isFailure()) {
        ATH_MSG_INFO("Failed to read " << SiHitContainers[i] );
        return StatusCode::FAILURE;
      }

      //ATH_MSG_INFO("#" << SiHitContainers[i] << ": " <<siHits[i]->Size());
  
      vector<unsigned long int> siHits_id;
      vector<float> siHits_energyLoss;
      vector<float> siHits_meanTime;
      vector<float> siHits_localStartPositionX;
      vector<float> siHits_localStartPositionY;
      vector<float> siHits_localStartPositionZ;
      vector<float> siHits_globalPositionX;
      vector<float> siHits_globalPositionY;
      vector<float> siHits_globalPositionZ;
      vector<short int> siHits_barrelEndcap;
      vector<short int> siHits_layerDisk;
      vector<short int> siHits_etaModule;
      vector<short int> siHits_phiModule;
      vector<short int> siHits_side;
      vector<short int> siHits_eta_index;
      vector<short int> siHits_phi_index;
      vector<short int> siHits_is_barrel;
      vector<short int> siHits_layer_disk;
      vector<short int> siHits_is_blayer;
      vector<short int> siHits_isPixel;
      vector<short int> siHits_isSCT;
    
      for (const SiHit siHit : *(siHits[i])) { //AtlasHitsVector<SiHit>
      
        Identifier id = Identifier(siHit.identify());

        unsigned long int id_long = strtol(id.getString().data(),0,16);
        
        siHits_energyLoss.push_back(siHit.energyLoss());
        siHits_meanTime.push_back(siHit.meanTime());
        siHits_localStartPositionX.push_back(siHit.localStartPosition().x() );
        siHits_localStartPositionY.push_back(siHit.localStartPosition().y());
        siHits_localStartPositionZ.push_back(siHit.localStartPosition().z());
        siHits_barrelEndcap.push_back(siHit.getBarrelEndcap());
        siHits_layerDisk.push_back(siHit.getLayerDisk());
        siHits_etaModule.push_back(siHit.getEtaModule());
        siHits_phiModule.push_back(siHit.getPhiModule());
        siHits_side.push_back(siHit.getSide());
        siHits_isPixel.push_back(siHit.isPixel());
        siHits_isSCT.push_back(siHit.isSCT());

        siHits_id.push_back(id_long);
        siHits_layer_disk.push_back(siHit.getLayerDisk());

        //if (m_isGeometryLoaded) {
        siHits_eta_index.push_back(m_PixelID->eta_index(id));
        siHits_phi_index.push_back(m_PixelID->phi_index(id));
        siHits_is_blayer.push_back(m_PixelID->is_blayer(id));
        siHits_is_barrel.push_back(m_PixelID->is_barrel(id));
      
        // using the GeoSiHit does not work DURING the SIMULATION because the
        // pointer to the PixelDetectorManager object, InDetDD::PixelDetectorManager *_pix, is null, in the GeoSiHit::init function from GeoSiHit.icc
        // but WORKS during the reconstruction RDOtoESD
      
        GeoSiHit geoSiHit(siHit);       
        //HepGeom::Point3D<double> p = geoSiHit.getGlobalPosition();
        //cout << "GeoSiHit getGlobalPostion: " 
        //     << geoSiHit.getGlobalPosition().x() << " "
        //     << geoSiHit.getGlobalPosition().y() << " " 
        //     << geoSiHit.getGlobalPosition().z() << endl;
        siHits_globalPositionX.push_back(geoSiHit.getGlobalPosition().x());
        siHits_globalPositionY.push_back(geoSiHit.getGlobalPosition().y());
        siHits_globalPositionZ.push_back(geoSiHit.getGlobalPosition().z());
      
	//}

      }    

      m_siHits_n.push_back( (siHits[i])->size() );
      m_siHits_id.push_back(siHits_id);
      m_siHits_energyLoss.push_back(siHits_energyLoss);
      m_siHits_meanTime.push_back(siHits_meanTime);
      m_siHits_localStartPositionX.push_back(siHits_localStartPositionX);
      m_siHits_localStartPositionY.push_back(siHits_localStartPositionY);
      m_siHits_localStartPositionZ.push_back(siHits_localStartPositionZ);
      m_siHits_globalPositionX.push_back(siHits_globalPositionX);
      m_siHits_globalPositionY.push_back(siHits_globalPositionY);
      m_siHits_globalPositionZ.push_back(siHits_globalPositionZ);
      m_siHits_barrelEndcap.push_back(siHits_barrelEndcap);
      m_siHits_layerDisk.push_back(siHits_layerDisk);
      m_siHits_etaModule.push_back(siHits_etaModule);
      m_siHits_phiModule.push_back(siHits_phiModule);
      m_siHits_side.push_back(siHits_side);
      m_siHits_eta_index.push_back(siHits_eta_index);
      m_siHits_phi_index.push_back(siHits_phi_index);
      m_siHits_is_barrel.push_back(siHits_is_barrel);
      m_siHits_layer_disk.push_back(siHits_layer_disk);
      m_siHits_is_blayer.push_back(siHits_is_blayer);
      m_siHits_isPixel.push_back(siHits_isPixel);
      m_siHits_isSCT.push_back(siHits_isSCT); 
  
    }

    //TRTUncompressedHits (TRT_HitCollection_p3) [InDet]

    //example of how to read the TRT hits: TrtHitsTestTool.cxx
    //typedef AtlasHitsVector<TRTUncompressedHit> TRTUncompressedHitCollection;
    //  AtlasHitsVector<T> => vector<T>
    /*const DataHandle<TRTUncompressedHitCollection> trtHits;
    if (evtStore()->retrieve(trtHits,"TRTUncompressedHits").isFailure()) {
      ATH_MSG_INFO ( "failed to retrieve the TRTUncompressedHitCollection" );
    }   else {
      ATH_MSG_INFO ( "#TRTUncompresedHits=" << trtHits->size() );
      m_trtHits_n = trtHits->size();  
    }*/
    
    m_larHits_n.clear();
    m_larHits_energy.clear();
    m_larHits_time.clear();
    m_larHits_CaloDetDescrElement_x.clear();
    m_larHits_CaloDetDescrElement_y.clear();
    m_larHits_CaloDetDescrElement_z.clear();
    m_larHits_samplingLayer.clear();

    //ATH_MSG_INFO ("Reading the LAr containers");
    // LArHitMiniFCAl not present in the HITS files
    const TString LArHitContainers[4]={"LArHitEMB","LArHitEMEC","LArHitFCAL","LArHitHEC"}; 
    const LArHitContainer* larHits[4];
    for (int i=0;i<4;i++) {

      ATH_MSG_INFO(LArHitContainers[i]);
      if (evtStore()->retrieve(larHits[i], LArHitContainers[i].Data()).isFailure()) {
        ATH_MSG_INFO("Failed to read " << LArHitContainers[i] );
        return StatusCode::FAILURE; 
      }
      
      //ATH_MSG_INFO ( "#" << LArHitContainers[i] << "=" << (larHits[i])->size() );

      vector<float> larHits_energy;
      vector<float> larHits_time;
      vector<float> larHits_x;
      vector<float> larHits_y;
      vector<float> larHits_z;
      vector<int> larHits_samplingLayer;
      for (const LArHit* larHit : *(larHits[i])) { // AthenaHitsVector<LArHit>
        larHits_energy.push_back(larHit->energy());
        larHits_time.push_back(larHit->time());

        GeoLArHit geoLArHit(*larHit);
        larHits_x.push_back(geoLArHit.getDetDescrElement()->x());
        larHits_y.push_back(geoLArHit.getDetDescrElement()->y());
        larHits_z.push_back(geoLArHit.getDetDescrElement()->z());
        larHits_samplingLayer.push_back(geoLArHit.SamplingLayer());
      }

      m_larHits_n.push_back(larHits[i]->size());
      m_larHits_energy.push_back(larHits_energy);
      m_larHits_time.push_back(larHits_time);
      m_larHits_CaloDetDescrElement_x.push_back(larHits_x);
      m_larHits_CaloDetDescrElement_y.push_back(larHits_y);
      m_larHits_CaloDetDescrElement_z.push_back(larHits_z);
      m_larHits_samplingLayer.push_back(larHits_samplingLayer);
    }


    /*ATH_MSG_INFO("Reading the calibration hit containers");
    m_larCalibrationHits_n.clear();
    const TString CaloCalibrationHitContainers[6] = {
      "LArCalibrationHitActive", "LArCalibrationHitInactive", "LArCalibrationHitDeadMaterial",
      "TileCalibHitActiveCell", "TileCalibHitInactiveCell", "TileCalibHitDeadMaterial"
    };
    const CaloCalibrationHitContainer* caloCalibrationHits[6];
    for (int i=0;i<6;i++) 
      if (evtStore()->retrieve(caloCalibrationHits[i], CaloCalibrationHitContainers[i].Data()).isFailure()) 
        ATH_MSG_INFO ( "failed to retrieve " << CaloCalibrationHitContainers[i]);
      else {
        ATH_MSG_INFO ( "#" << CaloCalibrationHitContainers[i] << "=" << caloCalibrationHits[i]->size()); 
        m_larCalibrationHits_n.push_back( caloCalibrationHits[i]->size() );
      }*/

    ATH_MSG_INFO("Reading the Tile hit container");
    const TileHitVector* mbtsHits = 0;
    if (evtStore()->retrieve(mbtsHits,"MBTSHits").isFailure())
      ATH_MSG_INFO("Failed to read the MBTS HITS");
    else {
      ATH_MSG_DEBUG ( "#MBTS hits: " << mbtsHits->size() );
      m_mbtsHits_n = mbtsHits->Size();
    }
  
    const TileHitVector* tileHitVector = 0;
    if (evtStore()->retrieve(tileHitVector,"TileHitVec").isFailure())
      ATH_MSG_INFO("Failed to read the Tile HITS");
    else  {
      ATH_MSG_DEBUG ( "#tile hits: " << tileHitVector->Size() );
    m_tileHits_n = tileHitVector->Size();
    }
 
    m_cscHits_globalPositionX.clear();
    m_cscHits_globalPositionY.clear();
    m_cscHits_globalPositionZ.clear();  
    const CSCSimHitCollection* cscHits = 0;
    if ( evtStore()->retrieve(cscHits, "CSC_Hits").isFailure() ) 
      ATH_MSG_INFO ( "failed to retrieve the CSC_Hits" );
    else {
      ATH_MSG_DEBUG ( "#CSC hits = " << cscHits->size() );

      CSCSimHitCollection::const_iterator cscIter = cscHits->begin();
      for (; cscIter!=cscHits->end(); cscIter++) {
        //if (m_isGeometryLoaded) {
        GeoCSCHit geoCSCHit(*cscIter);
        m_cscHits_globalPositionX.push_back(geoCSCHit.getGlobalPosition().x());
        m_cscHits_globalPositionY.push_back(geoCSCHit.getGlobalPosition().y());
        m_cscHits_globalPositionZ.push_back(geoCSCHit.getGlobalPosition().z());
        //}
      }
      m_cscHits_n = cscHits->size();
    }

    m_mdtHits_globalPositionX.clear();
    m_mdtHits_globalPositionY.clear();
    m_mdtHits_globalPositionZ.clear();  
    const MDTSimHitCollection* mdtHits = 0;
    if ( evtStore()->retrieve(mdtHits, "MDT_Hits").isFailure() ) 
      ATH_MSG_INFO ( "failed to retrieve the MDT_Hits" );
    else {
      //ATH_MSG_INFO ( "#MDT_Hits=" << mdtHits->size() );
      m_mdtHits_n = mdtHits->size();

      MDTSimHitCollection::const_iterator mdtIter = mdtHits->begin();
      for (; mdtIter!=mdtHits->end(); mdtIter++) {
        //if (m_isGeometryLoaded) {
        GeoMDTHit geoMDTHit(*mdtIter);
        m_mdtHits_globalPositionX.push_back(geoMDTHit.getGlobalPosition().x());
        m_mdtHits_globalPositionY.push_back(geoMDTHit.getGlobalPosition().y());
        m_mdtHits_globalPositionZ.push_back(geoMDTHit.getGlobalPosition().z());
        //}
      }
    }

    //ATH_MSG_INFO("Reading the TGC hits");
    const TGCSimHitCollection* tgcHits = 0;
    if ( evtStore()->retrieve(tgcHits, "TGC_Hits").isFailure() ) 
      ATH_MSG_INFO ( "failed to retrieve the TGC_Hits" );
    else {
      //ATH_MSG_DEBUG ( "#TGC_Hits=" << tgcHits->size() );
      m_tgcHits_n = tgcHits->size();
    }

    //ATH_MSG_INFO("Reading the RPC hits");
    const RPCSimHitCollection* rpcHits = 0;
    if ( evtStore()->retrieve(rpcHits, "RPC_Hits").isFailure() ) 
      ATH_MSG_INFO ( "failed to retrieve the RPC_Hits" );
    else {
      //ATH_MSG_DEBUG ( "#RPC_Hits=" << rpcHits->size() );
      m_rpcHits_n = rpcHits->size();
    }
  
    /*ATH_MSG_INFO("Reading the RecoTimingObj");
    const RecoTimingObj* recoTimingObj = 0;
    if (evtStore()->retrieve(recoTimingObj, "EVNTtoHITS_timings").isFailure()) 
      ATH_MSG_INFO( "failed to retrieve the RecoTimingObj ");
    else {
      ATH_MSG_DEBUG ( "RecoTimingObj size=" << recoTimingObj->size() ); 
     for (unsigned int i=0;i<recoTimingObj->size();i++)
       ATH_MSG_DEBUG ( "word #" << i << " " << (*recoTimingObj)[i] );
    }*/
  } // doHITS

  if (m_doTrigger)
    for (unsigned int i=0; i<m_trigItem_name.size(); i++) {
        m_trigItem_isPassed[i] = m_trigDecTool->isPassed(m_trigItem_name[i].Data());
        //cout << "NtupleWriter:fill " << m_trigItem_name[i] << " isPassed=" << m_trigDecTool->isPassed(m_trigItem_name[i].Data()) << endl; 
      }

  if (m_doStableBeams) {
    if ((unsigned int)m_runNumber != m_runQueryPickleTool->runNumber()) {
      m_runQueryPickleTool->setRunNumber(m_runNumber);    
      ATH_CHECK(m_runQueryPickleTool->configure());
    }
    m_stableBeams = m_runQueryPickleTool->isStableBeams(m_lbn);
  }

  if (m_doBCM_RDOs) {
    const BCM_RDO_Container* bcm_RDO_Container;
    if (evtStore()->retrieve(bcm_RDO_Container,"BCM_RDOs").isFailure()) {
      ATH_MSG_INFO ("failed to retrieve the BCM_RDO Container");
      return StatusCode::FAILURE;
    }
    m_BCM_RDO_Container_nCollections = bcm_RDO_Container->size();
  
    m_BCM_RDO_Collection_nBCM_RDOs.clear();
    m_BCM_RDO_Collection_channel.clear();

    m_BCM_RawData_word1.clear();
    m_BCM_RawData_word2.clear();
    m_BCM_RawData_channel.clear();
    m_BCM_RawData_pulse1Position.clear();
    m_BCM_RawData_pulse1Width.clear();
    m_BCM_RawData_pulse2Position.clear();
    m_BCM_RawData_pulse2Width.clear();
    m_BCM_RawData_LVL1A.clear();
    m_BCM_RawData_BCID.clear();
    m_BCM_RawData_LVL1ID.clear();
    m_BCM_RawData_Error.clear();

    for (unsigned int coll=0;coll<m_BCM_RDO_Container_nCollections;coll++) {
      //ATH_MSG_INFO("  BCM_RDO_Collection #" << coll);
      const BCM_RDO_Collection* bcm_RDO_Collection = bcm_RDO_Container->at(coll);

      //ATH_MSG_INFO("    #BCM_RawData=" << bcm_RDO_Collection->size() );
      m_BCM_RDO_Collection_nBCM_RDOs.push_back( bcm_RDO_Collection->size() );

      //ATH_MSG_INFO("    getChannel=" << bcm_RDO_Collection->getChannel() );
      m_BCM_RDO_Collection_channel.push_back( bcm_RDO_Collection->getChannel() );

      for (unsigned int rdo = 0; rdo < bcm_RDO_Collection->size(); rdo++) {
        const BCM_RawData* bcm_RawData = bcm_RDO_Collection->at(rdo);
        //ATH_MSG_INFO(" channel pulse1Position pulse2Position=" 
        //             << bcm_RawData->getChannel() << " " << bcm_RawData->getPulse1Position() << " " << bcm_RawData->getPulse2Position() );
        m_BCM_RawData_word1.push_back(bcm_RawData->getWord1());
        m_BCM_RawData_word2.push_back(bcm_RawData->getWord2());
        m_BCM_RawData_channel.push_back(bcm_RawData->getChannel());
        m_BCM_RawData_pulse1Position.push_back(bcm_RawData->getPulse1Position());
        m_BCM_RawData_pulse1Width.push_back(bcm_RawData->getPulse1Width());
        m_BCM_RawData_pulse2Position.push_back(bcm_RawData->getPulse2Position());
        m_BCM_RawData_pulse2Width.push_back(bcm_RawData->getPulse2Width());
        m_BCM_RawData_LVL1A.push_back(bcm_RawData->getLVL1A());
        m_BCM_RawData_BCID.push_back(bcm_RawData->getBCID());
        m_BCM_RawData_LVL1ID.push_back(bcm_RawData->getLVL1ID());
        m_BCM_RawData_Error.push_back(bcm_RawData->getError());
      }

    }
  }

  if (m_doPixelClusters) {

    m_pixCl_globalPosX.clear();
    m_pixCl_globalPosY.clear();
    m_pixCl_globalPosZ.clear(); 
    m_pixCl_eta_pixel_index.clear();
    m_pixCl_phi_pixel_index.clear();
    m_pixCl_eta_module.clear();
    m_pixCl_phi_module.clear();
    m_pixCl_sizePhi.clear();
    m_pixCl_sizeZ.clear();
    m_pixCl_charge.clear();
    m_pixCl_isFake.clear();
    m_pixCl_isSplit.clear();
    m_pixCl_ToT.clear();
    m_pixCl_nRDO.clear();
    m_pixCl_bec.clear();

    // In the DAOD_IDNCBs, PixelClusters are of type DataVector<xAOD::TrackMeasurementValidation_v1> [InDet], being the output of the thinning tool.
    // while in the ESDs :                           InDet::PixelClusterContainer_p3 [InDet]
 
    const xAOD::TrackMeasurementValidationContainer* pixelClusters = 0;
    if (evtStore()->retrieve(pixelClusters, "PixelClusters").isFailure()) {
      ATH_MSG_INFO( "Failed to retrieve the PixelClusters");
      return StatusCode::FAILURE;
    }
      
    m_pixCl_n = pixelClusters->size();
    xAOD::TrackMeasurementValidationContainer::const_iterator pixIt = pixelClusters->begin();
    for (; pixIt != pixelClusters->end(); pixIt++) {
      m_pixCl_globalPosX.push_back((*pixIt)->globalX());
      m_pixCl_globalPosY.push_back((*pixIt)->globalY());
      m_pixCl_globalPosZ.push_back((*pixIt)->globalZ());

      //Using auxdata<char>("isFake") you get: SG::ExcConstAuxData: Non-const operation `fetch item' performed on const aux data `::isFake'
      //Using auxdataConst works.

      // variables in the DAOD_IDNCBs:
      m_pixCl_eta_pixel_index.push_back( (*pixIt)->auxdataConst<int>("eta_pixel_index") );
      m_pixCl_phi_pixel_index.push_back( (*pixIt)->auxdataConst<int>("phi_pixel_index") );
      m_pixCl_eta_module.push_back( (*pixIt)->auxdataConst<int>("eta_module") );
      m_pixCl_phi_module.push_back( (*pixIt)->auxdataConst<int>("phi_module") );
      m_pixCl_sizePhi.push_back( (*pixIt)->auxdataConst<int>("sizePhi") );
      m_pixCl_sizeZ.push_back( (*pixIt)->auxdataConst<int>("sizeZ") );
      m_pixCl_charge.push_back( (*pixIt)->auxdataConst<float>("charge") );
      m_pixCl_isFake.push_back( (*pixIt)->auxdataConst<char>("isFake") );
      m_pixCl_isSplit.push_back( (*pixIt)->auxdataConst<int>("isSplit") );
      m_pixCl_ToT.push_back( (*pixIt)->auxdataConst<int>("ToT") );
      m_pixCl_nRDO.push_back( (*pixIt)->auxdataConst<int>("nRDO") );
      m_pixCl_bec.push_back( (*pixIt)->auxdataConst<int>("bec") );
    }

  }

  if (m_doSCTClusters) {

    m_sctCl_globalPosX.clear();
    m_sctCl_globalPosY.clear();
    m_sctCl_globalPosZ.clear(); 
    m_sctCl_bec.clear();
    m_sctCl_eta_module.clear();
    m_sctCl_phi_module.clear();
    m_sctCl_SiWidth.clear();
    m_sctCl_rdo_strip.clear();
    m_sctCl_rdo_timebin.clear();
    m_sctCl_side.clear();
    m_sctCl_layer.clear();
  
    //SCT_Clusters (DataVector<xAOD::TrackMeasurementValidation_v1>) [InDet]
    const xAOD::TrackMeasurementValidationContainer* sctClusters = 0;
    if (evtStore()->retrieve(sctClusters, "SCT_Clusters").isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve the SCT_Clusters");
      return StatusCode::FAILURE;
    }  

    m_sctCl_n = sctClusters->size();    
    xAOD::TrackMeasurementValidationContainer::const_iterator sctIt = sctClusters->begin();
    for (; sctIt != sctClusters->end(); sctIt++) {
      m_sctCl_globalPosX.push_back((*sctIt)->globalX());
      m_sctCl_globalPosY.push_back((*sctIt)->globalY());
      m_sctCl_globalPosZ.push_back((*sctIt)->globalZ());

      //variables in the DAOD_IDNCBs:
      m_sctCl_bec.push_back( (*sctIt)->auxdataConst<int>("bec") );
      m_sctCl_eta_module.push_back( (*sctIt)->auxdataConst<int>("eta_module") );
      m_sctCl_phi_module.push_back( (*sctIt)->auxdataConst<int>("phi_module") );
      m_sctCl_SiWidth.push_back( (*sctIt)->auxdataConst<int>("SiWidth") );
      m_sctCl_rdo_strip.push_back( (*sctIt)->auxdataConst<vector<int>>("rdo_strip") );
      m_sctCl_rdo_timebin.push_back( (*sctIt)->auxdataConst<vector<int>>("rdo_timebin") );
      m_sctCl_side.push_back( (*sctIt)->auxdataConst<int>("side") );
      m_sctCl_layer.push_back( (*sctIt)->auxdataConst<int>("layer") );
    }
  } 

  const xAOD::CaloClusterContainer* topoclusters = 0; 
  if (m_doTopoClusters) {
    if (evtStore()->retrieve(topoclusters, "CaloCalTopoClusters").isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve the TopoClusters" );
      return StatusCode::FAILURE;
    }    
    //ATH_MSG_INFO("#CaloCalTopoClusters: " << topoclusters->size() );
    m_cl_n = topoclusters->size();  
    m_cl_pt.clear();
    m_cl_e.clear();
    m_cl_emscale_e.clear();  
    m_cl_eta.clear();  
    m_cl_phi.clear();  
    m_cl_rPerp.clear();  
    m_cl_z.clear();  
    m_cl_time.clear();  
    m_cl_fracSamplingMax.clear();  
    m_cl_fracSamplingMaxIndex.clear();
    for (const xAOD::CaloCluster *cl : *topoclusters) 
    if (m_topoCluster_e_cut == -99999 || cl->e() > m_topoCluster_e_cut) {  
      m_cl_pt.push_back(cl->pt());
      m_cl_e.push_back(cl->e());

      // from RecBackgroundAlgs/src/BeamBackgroundFiller.cxx:
      float eEmClus =
      cl->eSample(CaloSampling::CaloSample::PreSamplerB) +
      cl->eSample(CaloSampling::CaloSample::EMB1) +
      cl->eSample(CaloSampling::CaloSample::EMB2) +
      cl->eSample(CaloSampling::CaloSample::EMB3) +
      cl->eSample(CaloSampling::CaloSample::PreSamplerE) +
      cl->eSample(CaloSampling::CaloSample::EME1) +
      cl->eSample(CaloSampling::CaloSample::EME2) +
      cl->eSample(CaloSampling::CaloSample::EME3) +
      cl->eSample(CaloSampling::CaloSample::FCAL0);

      float eHadClus =
      cl->eSample(CaloSampling::CaloSample::HEC0) +  
      cl->eSample(CaloSampling::CaloSample::HEC1) +
      cl->eSample(CaloSampling::CaloSample::HEC2) +
      cl->eSample(CaloSampling::CaloSample::HEC3) +  
      cl->eSample(CaloSampling::CaloSample::TileBar0) +
      cl->eSample(CaloSampling::CaloSample::TileBar1) +
      cl->eSample(CaloSampling::CaloSample::TileBar2) +  
      cl->eSample(CaloSampling::CaloSample::TileGap1) + 
      cl->eSample(CaloSampling::CaloSample::TileGap2) +
      cl->eSample(CaloSampling::CaloSample::TileGap3) +
      cl->eSample(CaloSampling::CaloSample::TileExt0) +
      cl->eSample(CaloSampling::CaloSample::TileExt1) +
      cl->eSample(CaloSampling::CaloSample::TileExt2) +
      cl->eSample(CaloSampling::CaloSample::FCAL1) +
      cl->eSample(CaloSampling::CaloSample::FCAL2);
 
      double eClus = eEmClus + eHadClus;
      m_cl_emscale_e.push_back(eClus);

      m_cl_eta.push_back(cl->eta());
      m_cl_phi.push_back(cl->phi());

      TVector3 clVec;
      clVec.SetPtEtaPhi(cl->pt(),cl->eta(),cl->phi());
      m_cl_rPerp.push_back( cl->getMomentValue(xAOD::CaloCluster::MomentType::CENTER_MAG) * sin(clVec.Theta()) ); // mm
      m_cl_z.push_back( cl->getMomentValue(xAOD::CaloCluster::MomentType::CENTER_MAG) * cos(clVec.Theta()) ); // mm

      m_cl_time.push_back(cl->time());

      int SamplingMaxIndex = -1;
      float maxE = -1;
      float totE = 0;
      for (int smpl=0;smpl<24;smpl++) {
        if (cl->eSample((xAOD::CaloCluster::CaloSample)smpl) > maxE) {
          maxE = cl->eSample((xAOD::CaloCluster::CaloSample)smpl);
          SamplingMaxIndex = smpl;
        }
        totE += cl->eSample((xAOD::CaloCluster::CaloSample)smpl);
      }
      m_cl_fracSamplingMax.push_back( maxE / totE );
      m_cl_fracSamplingMaxIndex.push_back( SamplingMaxIndex );
    }
  }
  
  if (m_doJets) {
    const xAOD::JetContainer* jets = 0;
    if (evtStore()->retrieve(jets, m_jetCollection).isFailure()) {
      ATH_MSG_INFO( "Failed to retrieve the jets" );
      return StatusCode::FAILURE;
    }
    
    m_jets_n = jets->size();
    m_jets_pt.clear();
    m_jets_eta.clear();
    m_jets_phi.clear();
    m_jets_time.clear();  
    m_jets_emf.clear();
    m_jets_hecf.clear();
    m_jets_hecq.clear();
    m_jets_larq.clear();
    m_jets_AverageLArQF.clear();
    m_jets_negE.clear();
    m_jets_fracSamplingMax.clear();
    m_jets_fracSamplingMaxIndex.clear();
    //m_jets_width.clear();
    //m_jets_n90.clear();
    m_jets_numTrkPt1000.clear();
    m_jets_SumPtTrkPt500.clear();
    m_jets_isBadLoose.clear();  
 
    for (const xAOD::Jet* jet : * jets) {
      m_jets_pt.push_back(jet->pt());
      m_jets_eta.push_back(jet->eta());
      m_jets_phi.push_back(jet->phi());
      m_jets_time.push_back(jet->getAttribute<float>("Timing"));

      m_jets_emf.push_back(jet->getAttribute<float>("EMFrac"));
      m_jets_hecf.push_back(jet->getAttribute<float>("HECFrac"));
      m_jets_hecq.push_back(jet->getAttribute<float>("HECQuality"));
      m_jets_larq.push_back(jet->getAttribute<float>("LArQuality"));
      m_jets_AverageLArQF.push_back(jet->getAttribute<float>("AverageLArQF"));
      m_jets_negE.push_back(jet->getAttribute<float>("NegativeE"));
      m_jets_fracSamplingMax.push_back(jet->getAttribute<float>("FracSamplingMax"));
      m_jets_fracSamplingMaxIndex.push_back(jet->getAttribute<int>("FracSamplingMaxIndex")); 
     
      //float width;
      //if (jet->getAttribute<float>("Width"), &width) // no "Width" variable in DAOD_EXOT2;
      //  m_jets_width.push_back(width);
      //m_jets_n90.push_back(jet->getAttribute<int>("N90Cells")); ERROR SG::ExcBadAuxVar: Attempt to retrieve nonexistent aux data item `::N90Cells' 
      
      vector<int> numTrkPt1000;
      jet->getAttribute<vector<int>>("NumTrkPt1000", numTrkPt1000);
      if (numTrkPt1000.size())
        m_jets_numTrkPt1000.push_back(numTrkPt1000.at(0));
      m_jets_isBadLoose.push_back(!m_jetCleaningToolLoose->keep(*jet));

      vector<float> sumpttrkVec;
      jet->getAttribute<vector<float>>("SumPtTrkPt500", sumpttrkVec);
      m_jets_SumPtTrkPt500.push_back( sumpttrkVec.empty() ? 0 : sumpttrkVec[0] );
   }
  }

  const xAOD::MuonSegmentContainer* NCB_MuonSegments = 0;
  const xAOD::MuonSegmentContainer* MuonSegments = 0;

  if (m_doMuonSeg) {
    if (evtStore()->retrieve(NCB_MuonSegments,"NCB_MuonSegments").isFailure()) {
      ATH_MSG_INFO( "failed to retrieve the NCB_MuonSegments." );
      return StatusCode::FAILURE;
    }

    m_NCB_MuonSeg_n = NCB_MuonSegments->size();

    m_NCB_MuonSeg_x.clear();
    m_NCB_MuonSeg_y.clear();
    m_NCB_MuonSeg_z.clear(); 
    m_NCB_MuonSeg_px.clear();
    m_NCB_MuonSeg_py.clear();
    m_NCB_MuonSeg_pz.clear(); 
    m_NCB_MuonSeg_t0.clear();
    m_NCB_MuonSeg_t0error.clear();
    m_NCB_MuonSeg_chi2.clear();
    m_NCB_MuonSeg_ndof.clear();
    m_NCB_MuonSeg_sector.clear();
    m_NCB_MuonSeg_chamberIndex.clear();
    m_NCB_MuonSeg_technology.clear();
    m_NCB_MuonSeg_nPrecisionHits.clear();
    m_NCB_MuonSeg_thetaPos.clear();
    m_NCB_MuonSeg_thetaDir.clear();
    m_NCB_MuonSeg_phi.clear();

    for (const xAOD::MuonSegment* muonSeg : *NCB_MuonSegments) {
      m_NCB_MuonSeg_x.push_back(muonSeg->x());
      m_NCB_MuonSeg_y.push_back(muonSeg->y());
      m_NCB_MuonSeg_z.push_back(muonSeg->z());
      m_NCB_MuonSeg_px.push_back(muonSeg->px());
      m_NCB_MuonSeg_py.push_back(muonSeg->py());
      m_NCB_MuonSeg_pz.push_back(muonSeg->pz());
      m_NCB_MuonSeg_t0.push_back(muonSeg->t0());
      m_NCB_MuonSeg_t0error.push_back(muonSeg->t0error());
      m_NCB_MuonSeg_chi2.push_back(muonSeg->chiSquared());
      m_NCB_MuonSeg_ndof.push_back(muonSeg->numberDoF());
      m_NCB_MuonSeg_sector.push_back(muonSeg->sector());
      m_NCB_MuonSeg_chamberIndex.push_back(muonSeg->chamberIndex());
      m_NCB_MuonSeg_technology.push_back(muonSeg->technology());
      m_NCB_MuonSeg_nPrecisionHits.push_back(muonSeg->nPrecisionHits());

      m_NCB_MuonSeg_thetaPos.push_back( TVector3(muonSeg->x(), muonSeg->y(), muonSeg->z()).Theta() );
      m_NCB_MuonSeg_thetaDir.push_back( TVector3(muonSeg->px(), muonSeg->py(), muonSeg->pz()).Theta() );
      m_NCB_MuonSeg_phi.push_back( TVector3(muonSeg->x(), muonSeg->y(), muonSeg->z()).Phi() );
    }

    if (evtStore()->retrieve(MuonSegments,"MuonSegments").isFailure()) {
      ATH_MSG_ERROR ( "failed to retrieve the MuonSegments." );
      return StatusCode::FAILURE;
    }

    m_MuonSeg_n = MuonSegments->size();

    m_MuonSeg_x.clear();
    m_MuonSeg_y.clear();
    m_MuonSeg_z.clear(); 
    m_MuonSeg_px.clear();
    m_MuonSeg_py.clear();
    m_MuonSeg_pz.clear(); 
    m_MuonSeg_t0.clear();
    m_MuonSeg_t0error.clear();
    m_MuonSeg_chi2.clear();
    m_MuonSeg_ndof.clear();
    m_MuonSeg_sector.clear();
    m_MuonSeg_chamberIndex.clear();
    m_MuonSeg_technology.clear();
    m_MuonSeg_nPrecisionHits.clear();
    m_MuonSeg_thetaPos.clear();
    m_MuonSeg_thetaDir.clear();
    m_MuonSeg_phi.clear();

    for (const xAOD::MuonSegment* muonSeg : *MuonSegments) {
   
      m_MuonSeg_x.push_back(muonSeg->x());
      m_MuonSeg_y.push_back(muonSeg->y());
      m_MuonSeg_z.push_back(muonSeg->z());
      m_MuonSeg_px.push_back(muonSeg->px());
      m_MuonSeg_py.push_back(muonSeg->py());
      m_MuonSeg_pz.push_back(muonSeg->pz());
      m_MuonSeg_t0.push_back(muonSeg->t0());
      m_MuonSeg_t0error.push_back(muonSeg->t0error());
      m_MuonSeg_chi2.push_back(muonSeg->chiSquared());
      m_MuonSeg_ndof.push_back(muonSeg->numberDoF());
      m_MuonSeg_sector.push_back(muonSeg->sector());
      m_MuonSeg_chamberIndex.push_back(muonSeg->chamberIndex());
      m_MuonSeg_technology.push_back(muonSeg->technology());
      m_MuonSeg_nPrecisionHits.push_back(muonSeg->nPrecisionHits());

      m_MuonSeg_thetaPos.push_back( TVector3(muonSeg->x(), muonSeg->y(), muonSeg->z()).Theta() );
      m_MuonSeg_thetaDir.push_back( TVector3(muonSeg->px(), muonSeg->py(), muonSeg->pz()).Theta() );
      m_MuonSeg_phi.push_back( TVector3(muonSeg->x(), muonSeg->y(), muonSeg->z()).Phi() );
    }
  }

  if (m_doMDT_DriftCircles) {
    const Muon::MdtPrepDataContainer* mdtPrepDataContainer = 0;
    if (!evtStore()->retrieve(mdtPrepDataContainer,"MDT_DriftCircles").isSuccess()) {
      ATH_MSG_ERROR("failed to retrieve the MDT_DriftCircles");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("MdtPrepDataContainer #collections=" << mdtPrepDataContainer->size());
    m_MDT_DriftCircles_tdc.clear();
    m_MDT_DriftCircles_adc.clear();
    m_MDT_DriftCircles_status.clear();
    m_MDT_DriftCircles_globalPositionX.clear();
    m_MDT_DriftCircles_globalPositionY.clear();
    m_MDT_DriftCircles_globalPositionZ.clear();
    Muon::MdtPrepDataContainer::const_iterator colIt = mdtPrepDataContainer->begin();
    for (;colIt!=mdtPrepDataContainer->end();colIt++) {
      const Muon::MdtPrepDataCollection* mdtCol = *colIt;
      Muon::MdtPrepDataCollection::const_iterator mdtPrepDataIt = mdtCol->begin();
      for (;mdtPrepDataIt!=mdtCol->end();mdtPrepDataIt++) {
	Muon::MdtPrepData* mdtPrepData = *mdtPrepDataIt;
        m_MDT_DriftCircles_tdc.push_back(mdtPrepData->tdc());
        m_MDT_DriftCircles_adc.push_back(mdtPrepData->adc());
        m_MDT_DriftCircles_status.push_back(mdtPrepData->status());
        m_MDT_DriftCircles_globalPositionX.push_back(mdtPrepData->globalPosition().x());
        m_MDT_DriftCircles_globalPositionY.push_back(mdtPrepData->globalPosition().y());
        m_MDT_DriftCircles_globalPositionZ.push_back(mdtPrepData->globalPosition().z());
      }
    }
    m_MDT_DriftCircles_n = m_MDT_DriftCircles_tdc.size();
  }

  if (m_doMuonPatternCombinations) {
    const MuonPatternCombinationCollection* muonPatternCombinationCollection = 0;
    if (!evtStore()->retrieve(muonPatternCombinationCollection,"MuonHoughPatternCombinations").isSuccess()) {
      ATH_MSG_ERROR("failed to retrieve the MuonHoughPatternCombinations");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("MuonPatternCombinationCollection size=" << muonPatternCombinationCollection->size());
    m_MuonPatternCombination_trackParameter_X.clear();
    m_MuonPatternCombination_trackParameter_Y.clear();
    m_MuonPatternCombination_trackParameter_Z.clear();
    m_MuonPatternCombination_trackParameter_pX.clear();
    m_MuonPatternCombination_trackParameter_pY.clear();
    m_MuonPatternCombination_trackParameter_pZ.clear();
    m_MuonPatternCombination_trackParameter_pT.clear();
    m_MuonPatternCombination_trackParameter_charge.clear();
    m_MuonPatternCombination_trackRoadType.clear();
    MuonPatternCombinationCollection::const_iterator muonPatternCombinationCollectionIt = muonPatternCombinationCollection->begin();
    for (; muonPatternCombinationCollectionIt != muonPatternCombinationCollection->end();muonPatternCombinationCollection++) {
      const Muon::MuonPatternCombination* muonPatternCombination = *muonPatternCombinationCollectionIt;
      /*
      const Trk::TrackParameters* trackParameters = muonPatternCombination->trackParameter();
      if (trackParameters) {
        m_MuonPatternCombination_trackParameter_X.push_back(trackParameters->position().x());
        m_MuonPatternCombination_trackParameter_Y.push_back(trackParameters->position().y());
        m_MuonPatternCombination_trackParameter_Z.push_back(trackParameters->position().z());
        m_MuonPatternCombination_trackParameter_pX.push_back(trackParameters->momentum().x());
        m_MuonPatternCombination_trackParameter_pY.push_back(trackParameters->momentum().y());
        m_MuonPatternCombination_trackParameter_pZ.push_back(trackParameters->momentum().z());
        m_MuonPatternCombination_trackParameter_pT.push_back(trackParameters->pT());
        m_MuonPatternCombination_trackParameter_charge.push_back(trackParameters->charge());
	}*/
      m_MuonPatternCombination_trackRoadType.push_back(muonPatternCombination->trackRoadType());
    }
    m_MuonPatternCombination_n = m_MuonPatternCombination_trackRoadType.size();
  }

  if (m_doMuonSegmentCombinations) {
    
  }

  if (m_doMBTScells) {
    
  }

  if (m_doBeamBackgroundData) 
  for (unsigned int j=0;j<m_beamBackgroundKeys.size();j++) {
    if (evtStore()->retrieve(m_beamBackgroundData,m_beamBackgroundKeys[j]).isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve the BeamBackgroundData container with key:" << m_beamBackgroundKeys[j]);
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG("Retrieved the BeamBackgroundData container with key:" << m_beamBackgroundKeys[j]);
    
    //m_beamBackgroundData_backgroundFlag = 0;
    m_beamBackgroundData_resultClus[j].clear();
    m_beamBackgroundData_resultMuonSeg[j].clear();
    m_beamBackgroundData_resultNCB_MuonSeg[j].clear();

    ATH_MSG_DEBUG("numSegment=" << m_beamBackgroundData->GetNumSegment() );
    ATH_MSG_DEBUG("numSegmentACNoTime=" << m_beamBackgroundData->GetNumSegmentACNoTime()); 

    m_beamBackgroundData_numSegment[j] = m_beamBackgroundData->GetNumSegment();
    m_beamBackgroundData_numSegmentEarly[j] = m_beamBackgroundData->GetNumSegmentEarly();
    m_beamBackgroundData_numSegmentACNoTime[j] = m_beamBackgroundData->GetNumSegmentACNoTime();
    m_beamBackgroundData_numSegmentAC[j] = m_beamBackgroundData->GetNumSegmentAC();
    m_beamBackgroundData_numMatched[j] = m_beamBackgroundData->GetNumMatched();
    m_beamBackgroundData_numNoTimeLoose[j] = m_beamBackgroundData->GetNumNoTimeLoose();
    m_beamBackgroundData_numNoTimeMedium[j] = m_beamBackgroundData->GetNumNoTimeMedium();
    m_beamBackgroundData_numNoTimeTight[j] = m_beamBackgroundData->GetNumNoTimeTight();
    m_beamBackgroundData_numOneSidedLoose[j] = m_beamBackgroundData->GetNumOneSidedLoose();
    m_beamBackgroundData_numOneSidedMedium[j] = m_beamBackgroundData->GetNumOneSidedMedium();
    m_beamBackgroundData_numOneSidedTight[j] = m_beamBackgroundData->GetNumOneSidedTight();
    m_beamBackgroundData_numTwoSidedNoTime[j] = m_beamBackgroundData->GetNumTwoSidedNoTime();
    m_beamBackgroundData_numTwoSided[j] = m_beamBackgroundData->GetNumTwoSided();
    //if (m_beamBackgroundData->GetNumSegment()>0)        m_beamBackgroundData_backgroundFlag |= (1<<17);
    //if (m_beamBackgroundData->GetNumClusterShape()>0)   m_beamBackgroundData_backgroundFlag |= (1<<18);
    //if (m_beamBackgroundData->GetNumNoTimeLoose()>0)    m_beamBackgroundData_backgroundFlag |= (1<<19);
    //if (m_beamBackgroundData->GetNumTwoSidedNoTime()>0) m_beamBackgroundData_backgroundFlag |= (1<<20);
    m_beamBackgroundData_timeDiffAC_SegmentACNoTime[j] = m_beamBackgroundData->GetTimeDiffACSegmentACNoTime();
    m_beamBackgroundData_timeDiffAC_SegmentAC[j] = m_beamBackgroundData->GetTimeDiffACSegmentAC();
    m_beamBackgroundData_timeDiffAC_TwoSidedNoTime[j] = m_beamBackgroundData->GetTimeDiffACTwoSidedNoTime();
    m_beamBackgroundData_timeDiffAC_TwoSided[j] = m_beamBackgroundData->GetTimeDiffACTwoSided();
    if (topoclusters)
      for (const xAOD::CaloCluster* clus : *topoclusters) 
        if (m_topoCluster_e_cut == -99999 || clus->e() > m_topoCluster_e_cut) {  
          int i=0;
          bool isMatched = false;
          for (;i<m_beamBackgroundData->GetNumMatched();i++) 
            if (clus == m_beamBackgroundData->GetIndexClus(i)) {
              isMatched=true;
              break;
            }

        m_beamBackgroundData_resultClus[j].push_back(isMatched ? m_beamBackgroundData->GetResultClus(i) : 0);
      }
   
    if (MuonSegments) 
      for (const xAOD::MuonSegment* seg : *MuonSegments) {
        int i=0;
        bool isTagged = false;
        for (;i<m_beamBackgroundData->GetNumSegment();i++) 
           if (seg == m_beamBackgroundData->GetIndexSeg(i)) {
             isTagged=true;
             break;
           }
        m_beamBackgroundData_resultMuonSeg[j].push_back(isTagged ? m_beamBackgroundData->GetResultSeg(i) : 0);
      }   

    if (NCB_MuonSegments) 
      for (const xAOD::MuonSegment* seg : *NCB_MuonSegments) {
        int i=0;
        bool isTagged = false;
        for (;i<m_beamBackgroundData->GetNumSegment();i++) 
          if (seg == m_beamBackgroundData->GetIndexSeg(i)) {
            isTagged=true;
            break;
          }
        m_beamBackgroundData_resultNCB_MuonSeg[j].push_back(isTagged ? m_beamBackgroundData->GetResultSeg(i) : 0);
      }    
  }

  if (m_doBeamIntensity) {
    m_beamIntensityTool->config(m_runNumber);
    m_beamIntensity_B1 = m_beamIntensityTool->getIntensityBeam1(m_lbn);
    m_beamIntensity_B2 = m_beamIntensityTool->getIntensityBeam2(m_lbn);
    m_lb_duration = m_beamIntensityTool->getLBduration(m_lbn, m_lbn);
  }

  if (m_doBunchStructure) {
    m_bunchStructureTool->config(m_runNumber);
    m_bs_isUnpaired = m_bunchStructureTool->isUnpaired(m_bcid, m_lbn);
    m_bs_isUnpairedB1 = m_bunchStructureTool->isUnpairedBeam1(m_bcid, m_lbn);
    m_bs_isUnpairedB2 = m_bunchStructureTool->isUnpairedBeam2(m_bcid, m_lbn);
    m_bs_isUnpairedIsolated = m_bunchStructureTool->isUnpairedIsolated(m_bcid, m_lbn);
    m_bs_isColliding = m_bunchStructureTool->isColliding(m_bcid, m_lbn);
    m_bs_isCollidingFirst = m_bunchStructureTool->isCollidingFirst(m_bcid, m_lbn);
    m_bs_isEmpty = m_bunchStructureTool->isEmpty(m_bcid, m_lbn);
    m_bs_stableBeams = m_bunchStructureTool->isStableBeam(m_lbn);
    m_bs_numBunchesColliding = m_bunchStructureTool->numBunchesColliding(m_lbn);
    m_bs_numBunchesUnpairedB1 = m_bunchStructureTool->numBunchesUnpairedBeam1(m_lbn);
    m_bs_numBunchesUnpairedB2 = m_bunchStructureTool->numBunchesUnpairedBeam2(m_lbn);
  }   

  m_tree->Fill();

  return StatusCode::SUCCESS;
}

StatusCode NtupleWriterTool::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode NtupleWriterTool::Write() {
  //ATH_MSG_INFO("Saving tree with " << m_tree->GetEntries() << " entries.");
  //m_tree->Write();
  return StatusCode::SUCCESS;
}
