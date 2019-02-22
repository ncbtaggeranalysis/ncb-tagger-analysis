#ifndef NTUPLE_WRITER_TOOL_H
#define NTUPLE_WRITER_TOOL_H

#include <vector>
using namespace std;
#include <iostream>
#include <string>

#include <TString.h>
#include <TTree.h>
#include <TFile.h>
#include <TVector3.h>
#include <TSystem.h>
#include <TDirectory.h>

#include "xAODEventInfo/EventInfo.h"
#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "TileEvent/TileContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "NCBanalysis/BeamBackgroundData.h"
#include "GeneratorObjects/McEventCollection.h"
#include "InDetIdentifier/PixelID.h"

// headers for reading the HITS:
#include "TileSimEvent/TileHitVector.h"
#include "LArSimEvent/LArHitContainer.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetSimEvent/SiHitCollection.h"
//#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollection_p3.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "GeoAdaptors/GeoSiHit.h"
#include "GeoAdaptors/GeoLArHit.h"
#include "GeoAdaptors/GeoMuonHits.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "InDetSimEvent/SiHitIdHelper.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IAlgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GoodRunsLists/IGoodRunsListSelectionTool.h"
#include "JetInterface/IJetSelector.h"
#include "NCBanalysis/BunchStructureTool.h"
#include "NCBanalysis/BeamIntensityTool.h"
#include "NCBanalysis/INtupleWriterTool.h"
#include "NCBanalysis/RunQueryPickleTool.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/INTupleSvc.h"

class NtupleWriterTool : virtual public INtupleWriterTool, public AthAlgTool {

  public:

               NtupleWriterTool(const string& type, const std::string& name, const IInterface* parent);
              ~NtupleWriterTool();

    StatusCode initialize();
    StatusCode finalize();
    StatusCode Fill();
    StatusCode Write();

  private:
    string m_outputStream;
    const char* m_path;
    TTree* m_tree;
    //string m_fileName;
    string m_treeName;

    int m_runNumber;
    int m_eventNumber;
    int m_bcid;
    int m_lbn;
    unsigned int m_backgroundFlag;
    int m_npv;
    float m_averageInteractionsPerCrossing;

    bool m_pixelError;
    bool m_sctError;
    bool m_trtError;
    bool m_larError;
    bool m_tileError;
    bool m_muonError;

    bool m_doBeamIntensity;
    bool m_doBunchStructure;
    bool m_doBeamBackgroundData;
    vector<string> m_beamBackgroundKeys;
     
    bool m_doEventInfo;
    bool m_doTruthEvents;
    bool m_doMCEventCollection;
    string m_mcEventCollection;
    bool m_doHITS;
    bool m_doTrigger;
    bool m_doBCM_RDOs;
    bool m_doPixelClusters;
    bool m_doSCTClusters;
    bool m_doTopoClusters;
    float m_topoCluster_e_cut;
    bool m_doJets;
    bool m_doMuonSeg;
    bool m_doMDT_DriftCircles;  
    bool m_doMuonPatternCombinations;
    bool m_doMuonSegmentCombinations;  
    bool m_doMBTScells; 
    bool m_doGRLTool;

    bool m_passGRL;

    int m_bs_isUnpaired;
    int m_bs_isUnpairedB1;
    int m_bs_isUnpairedB2;
    int m_bs_isUnpairedIsolated;
    int m_bs_isColliding;
    int m_bs_isCollidingFirst;
    int m_bs_isEmpty;
    int m_bs_stableBeams;
    int m_bs_numBunchesColliding;
    int m_bs_numBunchesUnpairedB1;
    int m_bs_numBunchesUnpairedB2;

    bool m_doStableBeams;
    bool m_stableBeams;

    unsigned int m_BCM_RDO_Container_nCollections;

    vector<unsigned int> m_BCM_RDO_Collection_nBCM_RDOs;
    vector<unsigned int> m_BCM_RDO_Collection_channel;

    vector<int> m_BCM_RawData_word1;
    vector<int> m_BCM_RawData_word2;
    vector<int> m_BCM_RawData_channel;
    vector<int> m_BCM_RawData_pulse1Position;
    vector<int> m_BCM_RawData_pulse1Width;
    vector<int> m_BCM_RawData_pulse2Position;
    vector<int> m_BCM_RawData_pulse2Width;
    vector<int> m_BCM_RawData_LVL1A;
    vector<int> m_BCM_RawData_BCID;
    vector<int> m_BCM_RawData_LVL1ID;
    vector<int> m_BCM_RawData_Error;

    int m_NCB_MuonSeg_n;
    vector<float> m_NCB_MuonSeg_x;
    vector<float> m_NCB_MuonSeg_y;
    vector<float> m_NCB_MuonSeg_z;
    vector<float> m_NCB_MuonSeg_px;
    vector<float> m_NCB_MuonSeg_py;
    vector<float> m_NCB_MuonSeg_pz;
    vector<float> m_NCB_MuonSeg_t0;
    vector<float> m_NCB_MuonSeg_t0error;
    vector<float> m_NCB_MuonSeg_chi2;
    vector<float> m_NCB_MuonSeg_ndof;
    vector<int> m_NCB_MuonSeg_sector;
    vector<int> m_NCB_MuonSeg_chamberIndex;
    vector<int> m_NCB_MuonSeg_technology;
    vector<int> m_NCB_MuonSeg_nPrecisionHits;
    vector<float> m_NCB_MuonSeg_thetaPos;
    vector<float> m_NCB_MuonSeg_thetaDir;
    vector<float> m_NCB_MuonSeg_phi;

    int m_MuonSeg_n;
    vector<float> m_MuonSeg_x;
    vector<float> m_MuonSeg_y;
    vector<float> m_MuonSeg_z;
    vector<float> m_MuonSeg_px;
    vector<float> m_MuonSeg_py;
    vector<float> m_MuonSeg_pz;
    vector<float> m_MuonSeg_t0;
    vector<float> m_MuonSeg_t0error;
    vector<float> m_MuonSeg_chi2;
    vector<float> m_MuonSeg_ndof;
    vector<int> m_MuonSeg_sector;
    vector<int> m_MuonSeg_chamberIndex;
    vector<int> m_MuonSeg_technology;
    vector<int> m_MuonSeg_nPrecisionHits;
    vector<float> m_MuonSeg_thetaPos;
    vector<float> m_MuonSeg_thetaDir;
    vector<float> m_MuonSeg_phi;

    int m_MDT_DriftCircles_n;
    vector<int> m_MDT_DriftCircles_tdc;
    vector<int> m_MDT_DriftCircles_adc;
    vector<int> m_MDT_DriftCircles_status;
    vector<float> m_MDT_DriftCircles_globalPositionX;
    vector<float> m_MDT_DriftCircles_globalPositionY;
    vector<float> m_MDT_DriftCircles_globalPositionZ;

    int m_MuonPatternCombination_n;
    vector<float> m_MuonPatternCombination_trackParameter_X;
    vector<float> m_MuonPatternCombination_trackParameter_Y;
    vector<float> m_MuonPatternCombination_trackParameter_Z;
    vector<float> m_MuonPatternCombination_trackParameter_pX;
    vector<float> m_MuonPatternCombination_trackParameter_pY;
    vector<float> m_MuonPatternCombination_trackParameter_pZ;
    vector<float> m_MuonPatternCombination_trackParameter_pT;
    vector<float> m_MuonPatternCombination_trackParameter_charge;
    vector<int> m_MuonPatternCombination_trackRoadType;

    unsigned int m_cl_n;
    vector<float> m_cl_pt;
    vector<float> m_cl_e;
    vector<float> m_cl_emscale_e;
    vector<float> m_cl_eta;
    vector<float> m_cl_phi;
    vector<float> m_cl_rPerp;
    vector<float> m_cl_z;
    vector<float> m_cl_time;
    vector<float> m_cl_fracSamplingMax;
    vector<int>   m_cl_fracSamplingMaxIndex;

    string m_jetCollection;
    unsigned int m_jets_n;
    vector<float> m_jets_pt;
    vector<float> m_jets_eta;
    vector<float> m_jets_phi;
    vector<float> m_jets_time;

    vector<float> m_jets_emf;
    vector<float> m_jets_hecf;
    vector<float> m_jets_hecq;
    vector<float> m_jets_AverageLArQF;
    vector<float> m_jets_larq;
    vector<float> m_jets_negE;
    //vector<float> m_jets_chf;
    vector<float> m_jets_fracSamplingMax;
    vector<int> m_jets_fracSamplingMaxIndex;
    vector<float> m_jets_width;
    //vector<int> m_jets_n90;
    vector<int> m_jets_numTrkPt1000;
    vector<float> m_jets_SumPtTrkPt500;
    vector<bool> m_jets_isBadLoose;

    bool m_doSiClusters;

    int           m_pixCl_n;
    vector<float> m_pixCl_globalPosX; 
    vector<float> m_pixCl_globalPosY;
    vector<float> m_pixCl_globalPosZ;
    vector<int> m_pixCl_eta_pixel_index;
    vector<int> m_pixCl_phi_pixel_index;
    vector<int> m_pixCl_eta_module;
    vector<int> m_pixCl_phi_module;
    vector<int> m_pixCl_sizePhi;
    vector<int> m_pixCl_sizeZ;
    vector<float> m_pixCl_charge;
    vector<char> m_pixCl_isFake;
    vector<int> m_pixCl_isSplit;
    vector<int> m_pixCl_ToT;
    vector<int> m_pixCl_nRDO;
    vector<int> m_pixCl_bec; //barrel-endcap

    int           m_sctCl_n;
    vector<float> m_sctCl_globalPosX;
    vector<float> m_sctCl_globalPosY;
    vector<float> m_sctCl_globalPosZ; 
    vector<int> m_sctCl_bec;
    vector<int> m_sctCl_eta_module;
    vector<int> m_sctCl_phi_module;
    vector<int> m_sctCl_SiWidth;
    vector<vector<int>> m_sctCl_rdo_strip;
    vector<vector<int>> m_sctCl_rdo_timebin;
    vector<int> m_sctCl_side;
    vector<int> m_sctCl_layer;

    vector<float> m_truthEvent_weights;

    vector<TString> m_trigItem_name;
    vector<int>     m_trigItem_isPassed;

    float m_beamIntensity_B1;
    float m_beamIntensity_B2;
    float m_lb_duration;

    BeamBackgroundData* m_beamBackgroundData;
    #define MAX_BEAMBACKGROUND_KEYS 10
    //unsigned int m_beamBackgroundData_backgroundFlag;
    int m_beamBackgroundData_numSegment[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numSegmentEarly[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numSegmentACNoTime[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numSegmentAC[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numMatched[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numNoTimeLoose[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numNoTimeMedium[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numNoTimeTight[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numOneSidedLoose[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numOneSidedMedium[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numOneSidedTight[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numTwoSidedNoTime[MAX_BEAMBACKGROUND_KEYS];
    int m_beamBackgroundData_numTwoSided[MAX_BEAMBACKGROUND_KEYS];
    vector<int> m_beamBackgroundData_resultClus[MAX_BEAMBACKGROUND_KEYS];
    vector<int> m_beamBackgroundData_resultMuonSeg[MAX_BEAMBACKGROUND_KEYS];
    vector<int> m_beamBackgroundData_resultNCB_MuonSeg[MAX_BEAMBACKGROUND_KEYS];

    vector<float> m_beamBackgroundData_timeDiffAC_SegmentACNoTime[MAX_BEAMBACKGROUND_KEYS];
    vector<float> m_beamBackgroundData_timeDiffAC_SegmentAC[MAX_BEAMBACKGROUND_KEYS];
    vector<float> m_beamBackgroundData_timeDiffAC_TwoSidedNoTime[MAX_BEAMBACKGROUND_KEYS];
    vector<float> m_beamBackgroundData_timeDiffAC_TwoSided[MAX_BEAMBACKGROUND_KEYS];

    int m_mc_n;
    vector<int> m_mc_pdg_id;
    vector<int> m_mc_status;
    vector<float> m_mc_momentum_px;
    vector<float> m_mc_momentum_py;
    vector<float> m_mc_momentum_pz;
    vector<float> m_mc_momentum_e;
    vector<float> m_mc_momentum_rho; // spatial vector component magnitude
    vector<double> m_mc_weights;
    vector<float> m_mc_vertex_point3d_x;
    vector<float> m_mc_vertex_point3d_y;
    vector<float> m_mc_vertex_point3d_z;
    vector<float> m_mc_vertex_position_x;
    vector<float> m_mc_vertex_position_y;
    vector<float> m_mc_vertex_position_z;
    vector<float> m_mc_vertex_position_t;
    vector<float> m_mc_vertex_position_theta;
    vector<float> m_mc_vertex_position_phi;
    vector<float> m_mc_vertex_position_eta;

    ToolHandle<Trig::TrigDecisionTool> m_trigDecTool;
    ToolHandle<IGoodRunsListSelectionTool> m_grlTool;
    ToolHandle<IJetSelector> m_jetCleaningToolLoose;
    ToolHandle<BunchStructureTool> m_bunchStructureTool;
    ToolHandle<BeamIntensityTool> m_beamIntensityTool;
    ToolHandle<RunQueryPickleTool> m_runQueryPickleTool;

    // HITS
    vector<int> m_siHits_n;
    vector<vector<unsigned long int>> m_siHits_id;
    vector<vector<float>> m_siHits_meanTime;
    vector<vector<float>> m_siHits_energyLoss;
    vector<vector<float>> m_siHits_localStartPositionX;
    vector<vector<float>> m_siHits_localStartPositionY;
    vector<vector<float>> m_siHits_localStartPositionZ;
    vector<vector<short int>> m_siHits_barrelEndcap;
    vector<vector<short int>> m_siHits_layerDisk;
    vector<vector<short int>> m_siHits_etaModule;
    vector<vector<short int>> m_siHits_phiModule;
    vector<vector<short int>> m_siHits_side;
    vector<vector<short int>> m_siHits_eta_index;
    vector<vector<short int>> m_siHits_phi_index;
    vector<vector<short int>> m_siHits_is_barrel;
    vector<vector<short int>> m_siHits_layer_disk;
    vector<vector<short int>> m_siHits_is_blayer;
    vector<vector<short int>> m_siHits_isPixel;
    vector<vector<short int>> m_siHits_isSCT; 

    vector<vector<float>> m_siHits_globalPositionX;
    vector<vector<float>> m_siHits_globalPositionY; 
    vector<vector<float>> m_siHits_globalPositionZ;

    int m_trtHits_n; 

    vector<int> m_larHits_n;
    vector<vector<float>> m_larHits_energy;
    vector<vector<float>> m_larHits_time;
    vector<vector<float>> m_larHits_CaloDetDescrElement_x;
    vector<vector<float>> m_larHits_CaloDetDescrElement_y;
    vector<vector<float>> m_larHits_CaloDetDescrElement_z;
    vector<vector<int>> m_larHits_samplingLayer;

    vector<int> m_larCalibrationHits_n;

    int m_tileHits_n;
    int m_mbtsHits_n;

    int m_mdtHits_n;
    vector<float> m_mdtHits_globalPositionX;
    vector<float> m_mdtHits_globalPositionY;
    vector<float> m_mdtHits_globalPositionZ;

    int m_cscHits_n;
    vector<float> m_cscHits_globalPositionX;
    vector<float> m_cscHits_globalPositionY;
    vector<float> m_cscHits_globalPositionZ;

    int m_tgcHits_n;

    int m_rpcHits_n;

    const PixelID* m_PixelID;   

    INTupleSvc* m_ntupleSvc;
    ITHistSvc* m_thistSvc; 

};

#endif
