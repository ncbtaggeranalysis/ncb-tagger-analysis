#include <iostream>
using namespace std;
#include <utility>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphAsymmErrors.h>
#include <TLegend.h>
#include <TStyle.h>

#include "TaggerAnalysis/taggerAnalysisLib.h"
#include "LuminosityTool/LuminosityTool.h"

const TString taggerMethods[] = {
  "Segment",
  "SegmentEarly",
  "SegmentACNoTime",
  "SegmentAC",
  //"Matched",
  "NoTimeLoose",
  "NoTimeMedium",
  "NoTimeTight",
  "OneSidedLoose",
  "OneSidedMedium",
  "OneSidedTight",
  "TwoSidedNoTime",
  "TwoSided",

  "NoTimeTight&&TwoSided"
};

const vector<pair<int,int>> calo_samplings = {
  {0,0}, {1,3}, {4,7}, 
  {8,11}, 
  {12,14}, {15,17}, {18,20},
  {21,23}
};

const int MAX_CONFIGS=100;

const int nTaggerMethods = sizeof(taggerMethods)/sizeof(TString);

const int debug = 0;

const double jetPtBins[] = { 
  0,20,40,60,80,100,120,140,160,180,200,
  250,300,350,400,500,600,700,800,1000
};
const int nJetPtBins = sizeof(jetPtBins)/sizeof(double)-1;

const float pi = TMath::Pi();

TString getSubdetectorName(int fracSamplingMaxIndex) {
  if (fracSamplingMaxIndex<=3) return "EMB";
  else if (fracSamplingMaxIndex<=7) return "EMEC";
  else if (fracSamplingMaxIndex<=11) return "HEC";
  else if (fracSamplingMaxIndex<=14) return "TileBar";
  else if (fracSamplingMaxIndex<=17) return "TileGap";
  else if (fracSamplingMaxIndex<=20) return "TileExt";
  else if (fracSamplingMaxIndex<=23) return "FCAL";
  throw Form("getSubdetectorName unknown sample: %i",fracSamplingMaxIndex);
  return "";
}

class Sample {
  public:
         Sample(TString filename, vector<TString> configs, bool isBeamhalo);
        ~Sample();
    void FillHistograms(int evtMax); 
    void Write(TString outDir);
    void Write(TObject* eff);

  private:

    TEfficiency* m_eff[nTaggerMethods][MAX_CONFIGS];
    //TEfficiency* m_eff_wLumi[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_collidingFirst[nTaggerMethods][MAX_CONFIGS];

    TEfficiency* m_eff_cl0_pt[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_cl0_eta[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_cl0_phi[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_cl0_time[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_cl0_etaphi[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_cl0_etatime[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_cl0_fracSamplingMaxIndex[nTaggerMethods][MAX_CONFIGS];

    TEfficiency* m_eff_jet0_pt[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_jet0_eta[nTaggerMethods][MAX_CONFIGS];
    //TEfficiency* m_eff_jet0_etaWeighted[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_jet0_phi[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_jet0_time[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_jet0_etaPhi[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_jet0_etaTime[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_jet0_fracSamplingMaxIndex[nTaggerMethods][MAX_CONFIGS];

    TEfficiency* m_eff_mu[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_NPV[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_bcid[nTaggerMethods][MAX_CONFIGS];
    TEfficiency* m_eff_lbn[nTaggerMethods][MAX_CONFIGS];

    vector<TH1F*> m_h_cl0_pt;
    vector<TH1F*> m_h_cl0_e;
    vector<TH1F*> m_h_cl0_emscale_e;
    vector<TH1F*> m_h_cl0_eta;
    vector<TH1F*> m_h_cl0_phi;
    vector<TH1F*> m_h_cl0_rPerp;
    vector<TH1F*> m_h_cl0_z;
    vector<TH1F*> m_h_cl0_time;
    vector<TH1F*> m_h_cl0_fracSamplingMax;
    vector<TH1F*> m_h_cl0_fracSamplingMaxIndex;

    TH1F* m_h_taggedcl_pt[nTaggerMethods][MAX_CONFIGS];
    TH1F* m_h_taggedcl_e[nTaggerMethods][MAX_CONFIGS];
    TH1F* m_h_taggedcl_emscale_e[nTaggerMethods][MAX_CONFIGS];
    TH1F* m_h_taggedcl_eta[nTaggerMethods][MAX_CONFIGS];
    TH1F* m_h_taggedcl_phi[nTaggerMethods][MAX_CONFIGS];
    TH1F* m_h_taggedcl_rPerp[nTaggerMethods][MAX_CONFIGS];
    TH1F* m_h_taggedcl_z[nTaggerMethods][MAX_CONFIGS];
    TH2F* m_h_taggedcl_etatime[nTaggerMethods][MAX_CONFIGS];
    TH1F* m_h_taggedcl_fracSamplingMax[nTaggerMethods][MAX_CONFIGS];
    TH1F* m_h_taggedcl_fracSamplingMaxIndex[nTaggerMethods][MAX_CONFIGS];

    vector<TH1F*> m_h_jet0_pt;
    vector<TH1F*> m_h_jet0_eta;
    vector<TH1F*> m_h_jet0_phi;
    vector<TH2F*> m_h_jet0_etaphi;
    vector<TH1F*> m_h_jet0_time;
    vector<TH2F*> m_h_jet0_etatime;
    vector<TH1F*> m_h_jet0_emf;
    vector<TH1F*> m_h_jet0_hecf;
    vector<TH1F*> m_h_jet0_hecq;
    vector<TH1F*> m_h_jet0_larq;
    vector<TH1F*> m_h_jet0_AverageLArQF;
    vector<TH1F*> m_h_jet0_negE;
    vector<TH1F*> m_h_jet0_fracSamplingMax;
    vector<TH1F*> m_h_jet0_fracSamplingMaxIndex;
    vector<TH1F*> m_h_jet0_SumPtTrkPt500;

    TH1F* m_h_beamIntensityB1;
    TH1F* m_h_beamIntensityB2;

    TFile* m_file;
    TTree* m_tree;

    float m_mu;
    int m_NPV;
    int m_bcid;
    int m_lbn;
    bool m_passGRL;
    bool m_stableBeams;

    int m_bs_isColliding;
    int m_bs_isCollidingFirst;    
    int m_bs_isUnpairedIsolated;
    int m_bs_isUnpairedB1;
    int m_bs_isUnpairedB2;

    int m_L1_J12_UNPAIRED_ISO;
    int m_L1_J12_UNPAIRED_NONISO;

    vector<float>* m_cl_pt;
    vector<float>* m_cl_e;
    vector<float>* m_cl_emscale_e;
    vector<float>* m_cl_eta;
    vector<float>* m_cl_phi;
    vector<float>* m_cl_rPerp;
    vector<float>* m_cl_z;
    vector<float>* m_cl_time;
    vector<float>* m_cl_fracSamplingMax;
    vector<int>* m_cl_fracSamplingMaxIndex;

    vector<float>* m_jet_pt;
    vector<float>* m_jet_eta;
    vector<float>* m_jet_phi;
    vector<float>* m_jet_time;
    vector<float>* m_jet_fracSamplingMax;
    vector<int>* m_jet_fracSamplingMaxIndex;
    vector<float>* m_jet_emf;
    vector<float>* m_jet_hecf;
    vector<float>* m_jet_hecq;
    vector<float>* m_jet_larq;  
    vector<float>* m_jet_AverageLArQF;
    vector<float>* m_jet_negE;
    vector<float>* m_jet_SumPtTrkPt500;

    float m_beamIntensityB1;
    float m_beamIntensityB2;

    int m_BeamBackgroundData_numEvents[nTaggerMethods];
    vector<int>* m_BeamBackgroundData_resultClus;

    int m_runNumber;
    int getRunNumber(TString name);
    TString m_name;
    TString m_filename;

    bool eventFailsJetCleaningNoiseCuts(bool useEMFcut);
    bool eventPassesEventSelection(vector<TString> cutVec);
 
    vector<int> decisionBit(TString method); 
    bool  isTaggedEvent(TString method); 
    bool  isTaggedCluster(int clusterIndex, TString method);
 
    vector<TString> m_configs;

    bool m_isBeamhaloSample;

    //LuminosityTool* m_lumiTool;
    bool m_doTaggedClusters;
};

Sample::Sample(TString filename, vector<TString> configs, bool isBeamhalo) {
  cout << "Sample constructor: " << filename << endl;
  m_isBeamhaloSample = isBeamhalo;
  if (configs.size() > MAX_CONFIGS) 
    throw "Exeeded the maximum number of configs.";
  m_configs = configs;
  m_file = TFile::Open(filename);
  if (!m_file) throw "failed to open root file";
  if (!m_file->IsOpen()) throw "Failed to open root file";

  m_tree = (TTree*)m_file->Get("CollectionTree");
  if (!m_tree) throw "Tree pointer is null";
 
  cout << "CollectionTree entries=" << m_tree->GetEntries() << endl;

  m_runNumber = getRunNumber(filename); 
  m_name = Form("%i", m_runNumber);
  m_filename = filename;

  //m_lumiTool = new LuminosityTool();
  //m_lumiTool->config(m_runNumber); 

  if (debug) cout << "Initializing TEfficiency objects ..." << endl;

  for (int j=0;j<m_configs.size();j++) {
    TString config = TString::Format("config%i",j);
    for (int i=0;i<nTaggerMethods;i++) {

      m_eff[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_"+config, "", 1, 0, 1);
      m_eff[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      //m_eff_wLumi[i][j] = new TEfficiency("eff_wLumi"+taggerMethods[i]+"_"+config, "", 1, 0, 1);
      //m_eff_wLumi[i][j]->SetStatisticOption(TEfficiency::kFNormal);
      //m_eff_wLumi[i][j]->SetUseWeightedEvents();

      m_eff_collidingFirst[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_collidingFirst_"+config, "", 1, 0, 1);
      m_eff_collidingFirst[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_cl0_pt[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_cl0_pt_"+config, "", 100,0,1000);
      m_eff_cl0_pt[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_cl0_eta[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_cl0_eta_"+config, "", 100, -5, 5);
      m_eff_cl0_eta[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_cl0_phi[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_cl0_phi_"+config, "", 100, -pi, pi);
      m_eff_cl0_phi[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_cl0_etaphi[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_cl0_etaphi_"+config, "", 100, -5, 5, 100, -pi, pi);
      m_eff_cl0_etaphi[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_cl0_etatime[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_cl0_etatime_"+config, "", 100, -5, 5, 60, -30, 30);
      m_eff_cl0_etatime[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_cl0_time[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_cl0_time_"+config, "", 60, -30, 30);
      m_eff_cl0_time[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_cl0_fracSamplingMaxIndex[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_cl0_fracSamplingMaxIndex_"+config, "", 24, 0, 24);
      m_eff_cl0_fracSamplingMaxIndex[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      // ---------------------------------------------------------------------------
      m_eff_jet0_pt[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_jet0_pT_"+config, "", nJetPtBins, jetPtBins);
      m_eff_jet0_pt[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_jet0_eta[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_jet0_eta_"+config, "", 30, -3,3);
      m_eff_jet0_eta[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_jet0_phi[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_jet0_phi_"+config, "", 18,-TMath::Pi(),TMath::Pi());
      m_eff_jet0_phi[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_jet0_time[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_jet0_time_"+config, "", 30, -30, 30);
      m_eff_jet0_time[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_jet0_etaPhi[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_jet0_etaphi_"+config, "", 30, -3,3, 18, -TMath::Pi(),TMath::Pi());
      m_eff_jet0_etaPhi[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_jet0_etaTime[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_jet0_etatime_"+config, "", 30, -3,3, 30, -30, 30);
      m_eff_jet0_etaTime[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_jet0_fracSamplingMaxIndex[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_jet0_fracSamplingMaxIndex_"+config, "", 24,0,24);
      m_eff_jet0_fracSamplingMaxIndex[i][j]->SetStatisticOption(TEfficiency::kFNormal);
   
      m_h_taggedcl_pt[i][j] = new TH1F("h_taggedcl_"+taggerMethods[i]+"_pt_"+config,"",1000,0,1000);
      m_h_taggedcl_e[i][j] = new TH1F("h_taggedcl_"+taggerMethods[i]+"_e_"+config,"",1000,0,1000);
      m_h_taggedcl_emscale_e[i][j] = new TH1F("h_taggedcl_"+taggerMethods[i]+"_emscale_e_"+config,"",1000,0,1000);
      m_h_taggedcl_eta[i][j] = new TH1F("h_taggedcl_"+taggerMethods[i]+"_eta_"+config,"",100,-5,5);
      m_h_taggedcl_phi[i][j] = new TH1F("h_taggedcl_"+taggerMethods[i]+"_phi_"+config,"",100,-pi,pi);
      m_h_taggedcl_rPerp[i][j] = new TH1F("h_taggedcl_"+taggerMethods[i]+"_rPerp_"+config,"",500,500,4500);
      m_h_taggedcl_z[i][j] = new TH1F("h_taggedcl_"+taggerMethods[i]+"_z_"+config,"",500,-7000,7000);
      m_h_taggedcl_etatime[i][j] = new TH2F("h_taggedcl_"+taggerMethods[i]+"_etatime_"+config,"",100,-5,5,100,-50,50);
      m_h_taggedcl_fracSamplingMax[i][j] = new TH1F("h_taggedcl_"+taggerMethods[i]+"_fracSamplingMax_"+config,"",100,-0.5,1.5);
      m_h_taggedcl_fracSamplingMaxIndex[i][j] = new TH1F("h_taggedcl_"+taggerMethods[i]+"_fracSamplingMaxIndex_"+config,"",24,0,24);

      m_eff_mu[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_mu_"+config, "", 100, 0, 100);
      m_eff_mu[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_NPV[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_NPV_"+config, "", 100, 0, 100);
      m_eff_NPV[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_bcid[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_bcid_"+config, "", 3564, 0, 3564);
      m_eff_bcid[i][j]->SetStatisticOption(TEfficiency::kFNormal);

      m_eff_lbn[i][j] = new TEfficiency("eff_"+taggerMethods[i]+"_lbn_"+config, "", 10000, 0, 10000);
      m_eff_lbn[i][j]->SetStatisticOption(TEfficiency::kFNormal);
    }

    m_h_cl0_pt.push_back(new TH1F("h_cl0_pt_"+config,"",1000,0,1000));
    m_h_cl0_e.push_back(new TH1F("h_cl0_e_"+config,"",1000,0,1000));
    m_h_cl0_emscale_e.push_back(new TH1F("h_cl0_emscale_e_"+config,"",1000,0,1000));
    m_h_cl0_eta.push_back(new TH1F("h_cl0_eta_"+config,"",100,-5,5));
    m_h_cl0_phi.push_back(new TH1F("h_cl0_phi_"+config,"",100,-pi,pi));
    m_h_cl0_rPerp.push_back(new TH1F("h_cl0_rPerp_"+config,"",500,500,4500)); 
    m_h_cl0_z.push_back(new TH1F("h_cl0_z_"+config,"",500,-7000,7000));
    m_h_cl0_time.push_back(new TH1F("h_cl0_time_"+config,"",100,-50,50));
    m_h_cl0_fracSamplingMax.push_back(new TH1F("h_cl0_fracSamplingMax_"+config,"",100,-0.5,1.5));
    m_h_cl0_fracSamplingMaxIndex.push_back(new TH1F("h_cl0_fracSamplingMaxIndex_"+config,"",24,0,24));

    m_h_jet0_pt.push_back(new TH1F("h_jet0_pt_"+config,"",1000,0,10000));
    m_h_jet0_eta.push_back(new TH1F("h_jet0_eta_"+config,"",100,-5,5));
    m_h_jet0_phi.push_back(new TH1F("h_jet0_phi_"+config,"",100,-pi,pi));
    m_h_jet0_etaphi.push_back(new TH2F("h_jet0_etaphi_"+config,"",100,-5,5,100,-pi,pi));
    m_h_jet0_time.push_back(new TH1F("h_jet0_time_"+config,"",100,-50,50));
    m_h_jet0_etatime.push_back(new TH2F("h_jet0_etatime_"+config,"",100,-5,5,50,-25,25));
    m_h_jet0_emf.push_back(new TH1F("h_jet0_emf_"+config,"",100,-0.5,1.5));
    m_h_jet0_hecf.push_back(new TH1F("h_jet0_hecf_"+config,"",100,-0.5,1.5)); 
    m_h_jet0_hecq.push_back(new TH1F("h_jet0_hecq_"+config,"",100,-0.5,1.5));
    m_h_jet0_larq.push_back(new TH1F("h_jet0_larq_"+config,"",100,-0.5,1.5));
    m_h_jet0_AverageLArQF.push_back(new TH1F("h_jet0_AverageLArQF_"+config,"",100,-0.5,1.5));
    m_h_jet0_negE.push_back(new TH1F("h_jet0_negE_"+config,"",100,-100,100));
    m_h_jet0_fracSamplingMax.push_back(new TH1F("h_jet0_fracSamplingMax_"+config,"",100,-0.5,1.5)); 
    m_h_jet0_fracSamplingMaxIndex.push_back(new TH1F("h_jet0_fracSamplingMaxIndex_"+config,"",24,0,24));
    m_h_jet0_SumPtTrkPt500.push_back(new TH1F("h_jet0_SumPtTrkPt500_"+config,"",100,0,100));

  }

  m_h_beamIntensityB1 = new TH1F("h_beamIntensityB1","",100,0,4000);
  m_h_beamIntensityB2 = new TH1F("h_beamIntensityB2","",100,0,4000);

  m_tree->SetBranchStatus("*",0);
  //m_tree->SetBranchStatus(backgroundFlagBranch,1);
  m_tree->SetBranchStatus("NPV",1);
  m_tree->SetBranchStatus("averageInteractionsPerCrossing",1);
  m_tree->SetBranchStatus("lbn",1);
  m_tree->SetBranchStatus("bcid",1);
  m_tree->SetBranchStatus("passGRL",1);
  m_tree->SetBranchStatus("StableBeams",1);
  m_tree->SetBranchStatus("L1_J12_UNPAIRED_ISO",1);
  m_tree->SetBranchStatus("L1_J12_UNPAIRED_NONISO",1);
  m_tree->SetBranchStatus("BunchStructure.isColliding",1);
  m_tree->SetBranchStatus("BunchStructure.isCollidingFirst",1);
  m_tree->SetBranchStatus("BunchStructure.isUnpairedIsolated",1);
  m_tree->SetBranchStatus("BunchStructure.isUnpairedB1",1);
  m_tree->SetBranchStatus("BunchStructure.isUnpairedB2",1);
  m_tree->SetBranchStatus("TopoClusters.pt",1);
  m_tree->SetBranchStatus("TopoClusters.e",1);
  m_tree->SetBranchStatus("TopoClusters.emscale_e",1);
  m_tree->SetBranchStatus("TopoClusters.eta",1);
  m_tree->SetBranchStatus("TopoClusters.phi",1);
  m_tree->SetBranchStatus("TopoClusters.rPerp",1);
  m_tree->SetBranchStatus("TopoClusters.z",1);
  m_tree->SetBranchStatus("TopoClusters.time",1);
  m_tree->SetBranchStatus("TopoClusters.fracSamplingMax",1);
  m_tree->SetBranchStatus("TopoClusters.fracSamplingMaxIndex",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.pt",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.eta",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.phi",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.time",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.fracSamplingMax",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.fracSamplingMaxIndex",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.emf",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.hecf",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.hecq",1);  
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.larq",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.AverageLArQF",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.negE",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.SumPtTrkPt500",1);
  m_tree->SetBranchStatus("beamIntensity_B1",1);
  m_tree->SetBranchStatus("beamIntensity_B2",1);

  m_cl_pt = 0;
  m_cl_e = 0;
  m_cl_emscale_e = 0;
  m_cl_eta = 0;
  m_cl_phi = 0;
  m_cl_rPerp = 0;
  m_cl_z = 0;
  m_cl_time = 0;
  m_cl_fracSamplingMax = 0;
  m_cl_fracSamplingMaxIndex = 0;

  m_jet_pt = 0;
  m_jet_eta = 0;
  m_jet_phi = 0;
  m_jet_time = 0;
  m_jet_fracSamplingMax = 0;
  m_jet_fracSamplingMaxIndex = 0;
  m_jet_emf = 0;
  m_jet_hecf = 0;
  m_jet_hecq = 0;  
  m_jet_larq = 0; 
  m_jet_AverageLArQF = 0;
  m_jet_negE = 0;
  m_jet_SumPtTrkPt500 = 0;

  m_BeamBackgroundData_resultClus = 0;

  m_tree->SetBranchAddress("NPV",&m_NPV);
  m_tree->SetBranchAddress("averageInteractionsPerCrossing",&m_mu);
  m_tree->SetBranchAddress("bcid",&m_bcid);
  m_tree->SetBranchAddress("lbn",&m_lbn);
  m_tree->SetBranchAddress("passGRL",&m_passGRL);
  m_tree->SetBranchAddress("StableBeams",&m_stableBeams);
  m_tree->SetBranchAddress("L1_J12_UNPAIRED_ISO",&m_L1_J12_UNPAIRED_ISO);
  m_tree->SetBranchAddress("L1_J12_UNPAIRED_NONISO",&m_L1_J12_UNPAIRED_NONISO);
  m_tree->SetBranchAddress("BunchStructure.isColliding",&m_bs_isColliding);
  m_tree->SetBranchAddress("BunchStructure.isCollidingFirst",&m_bs_isCollidingFirst);
  m_tree->SetBranchAddress("BunchStructure.isUnpairedIsolated",&m_bs_isUnpairedIsolated);
  m_tree->SetBranchAddress("BunchStructure.isUnpairedB1",&m_bs_isUnpairedB1);
  m_tree->SetBranchAddress("BunchStructure.isUnpairedB2",&m_bs_isUnpairedB2);
  m_tree->SetBranchAddress("TopoClusters.pt",&m_cl_pt);
  m_tree->SetBranchAddress("TopoClusters.e",&m_cl_e);
  m_tree->SetBranchAddress("TopoClusters.emscale_e",&m_cl_emscale_e);
  m_tree->SetBranchAddress("TopoClusters.eta",&m_cl_eta);
  m_tree->SetBranchAddress("TopoClusters.phi",&m_cl_phi);
  m_tree->SetBranchAddress("TopoClusters.rPerp",&m_cl_rPerp);
  m_tree->SetBranchAddress("TopoClusters.z",&m_cl_z);
  m_tree->SetBranchAddress("TopoClusters.time",&m_cl_time);
  m_tree->SetBranchAddress("TopoClusters.fracSamplingMax",&m_cl_fracSamplingMax);
  m_tree->SetBranchAddress("TopoClusters.fracSamplingMaxIndex",&m_cl_fracSamplingMaxIndex);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.pt",&m_jet_pt);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.eta",&m_jet_eta);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.phi",&m_jet_phi);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.time",&m_jet_time);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.fracSamplingMax",&m_jet_fracSamplingMax); 
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.fracSamplingMaxIndex",&m_jet_fracSamplingMaxIndex);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.emf",&m_jet_emf); 
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.hecf",&m_jet_hecf);   
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.hecq",&m_jet_hecq);  
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.larq",&m_jet_larq);  
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.AverageLArQF",&m_jet_AverageLArQF);  
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.negE",&m_jet_negE);  
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.SumPtTrkPt500",&m_jet_SumPtTrkPt500);

  m_tree->SetBranchStatus("BeamBackgroundData.resultClus",1);
  m_tree->SetBranchAddress("BeamBackgroundData.resultClus",&m_BeamBackgroundData_resultClus);
  for (int i=0; i<nTaggerMethods; i++) {
    m_tree->SetBranchStatus("BeamBackgroundData.num" + taggerMethods[i], 1);
    m_tree->SetBranchAddress("BeamBackgroundData.num" + taggerMethods[i], m_BeamBackgroundData_numEvents + i);
  }
  m_tree->SetBranchAddress("beamIntensity_B1",&m_beamIntensityB1);
  m_tree->SetBranchAddress("beamIntensity_B2",&m_beamIntensityB2);

  m_doTaggedClusters=true;
  if (m_filename.Contains("181121"))
    m_doTaggedClusters=false;
}

Sample::~Sample() {
  cout<<"Sample::~Sample"<<endl;
  for (int j=0;j<m_configs.size();j++) {
    for (int i=0;i<nTaggerMethods;i++) {
      delete m_eff[i][j];
      //delete m_eff_wLumi[i][j];
      delete m_eff_collidingFirst[i][j];

      delete m_eff_cl0_pt[i][j];
      delete m_eff_cl0_eta[i][j];
      delete m_eff_cl0_phi[i][j];
      delete m_eff_cl0_etaphi[i][j];
      delete m_eff_cl0_etatime[i][j];
      delete m_eff_cl0_time[i][j];
      delete m_eff_cl0_fracSamplingMaxIndex[i][j];

      delete m_eff_jet0_pt[i][j];
      delete m_eff_jet0_eta[i][j];
      delete m_eff_jet0_phi[i][j];
      delete m_eff_jet0_time[i][j];
      delete m_eff_jet0_fracSamplingMaxIndex[i][j];

      delete m_eff_NPV[i][j];
      delete m_eff_mu[i][j];
      delete m_eff_bcid[i][j];
      delete m_eff_lbn[i][j];

      delete m_h_taggedcl_pt[i][j];
      delete m_h_taggedcl_e[i][j];
      delete m_h_taggedcl_emscale_e[i][j];
      delete m_h_taggedcl_eta[i][j];
      delete m_h_taggedcl_phi[i][j];
      delete m_h_taggedcl_rPerp[i][j];
      delete m_h_taggedcl_z[i][j];
      delete m_h_taggedcl_etatime[i][j];
      delete m_h_taggedcl_fracSamplingMax[i][j];
      delete m_h_taggedcl_fracSamplingMaxIndex[i][j];

    }
    delete m_h_cl0_pt[j];
    delete m_h_cl0_e[j];
    delete m_h_cl0_emscale_e[j];   
    delete m_h_cl0_eta[j];
    delete m_h_cl0_phi[j];
    delete m_h_cl0_rPerp[j];
    delete m_h_cl0_z[j];
    delete m_h_cl0_time[j];
    delete m_h_cl0_fracSamplingMax[j];
    delete m_h_cl0_fracSamplingMaxIndex[j];

    delete m_h_jet0_pt[j];
    delete m_h_jet0_eta[j];
    delete m_h_jet0_phi[j];
    delete m_h_jet0_etaphi[j];
    delete m_h_jet0_time[j];
    delete m_h_jet0_etatime[j];
    delete m_h_jet0_emf[j];
    delete m_h_jet0_hecf[j];
    delete m_h_jet0_hecq[j];
    delete m_h_jet0_larq[j]; 
    delete m_h_jet0_AverageLArQF[j];
    delete m_h_jet0_negE[j];
    delete m_h_jet0_fracSamplingMax[j];
    delete m_h_jet0_fracSamplingMaxIndex[j];
    delete m_h_jet0_SumPtTrkPt500[j];
  }
  delete m_h_beamIntensityB1;
  delete m_h_beamIntensityB2;
  m_file->Close();
}

void Sample::FillHistograms(int evtMax) {
  cout << "Sample::FillHistograms : " << m_name << endl;
 
  unsigned int max_entry = 0;
  if (evtMax == -1) max_entry = m_tree->GetEntries(); else max_entry = evtMax; 

  vector<vector<TString>> configsStrVec;
  for (int j=0;j<m_configs.size();j++) {
    TObjArray* objArray = m_configs[j].Tokenize("&&");
    vector<TString> strVec;
    for (int i=0;i<objArray->GetEntries();i++)
      strVec.push_back(objArray->At(i)->GetName());
    configsStrVec.push_back(strVec);
  }

  for (unsigned int ientry=0;ientry<max_entry;ientry++) {
   
    m_tree->GetEntry(ientry);
    
    cout << "entry " << ientry << " #jets=" << m_jet_pt->size() << endl;

    //float instLuminosity = m_lumiTool->instLuminosity(m_lbn);

    for (int j=0;j<m_configs.size();j++)
      if (eventPassesEventSelection(configsStrVec[j])) {

        for (int i=0;i<nTaggerMethods;i++) {

          //bool isTagged = (m_BeamBackgroundData_numEvents[i] != 0) ; 
          bool isTagged = isTaggedEvent(taggerMethods[i]);

          m_eff[i][j]->Fill(isTagged, 0);
          //if (instLuminosity)
          //  m_eff_wLumi[i][j]->FillWeighted(isTagged, 
          //                                  1/instLuminosity, 0);

          if (m_bs_isCollidingFirst)
            m_eff_collidingFirst[i][j]->Fill(isTagged, 0);

          if (m_cl_pt->size()) {
            m_eff_cl0_pt[i][j]->Fill(isTagged, m_cl_pt->at(0)/1000);
            m_eff_cl0_eta[i][j]->Fill(isTagged, m_cl_eta->at(0));
            m_eff_cl0_phi[i][j]->Fill(isTagged, m_cl_phi->at(0));
            m_eff_cl0_etaphi[i][j]->Fill(isTagged, m_cl_eta->at(0), m_cl_phi->at(0)); 
            m_eff_cl0_etatime[i][j]->Fill(isTagged, m_cl_eta->at(0), m_cl_time->at(0));
            m_eff_cl0_time[i][j]->Fill(isTagged, m_cl_time->at(0));
            m_eff_cl0_fracSamplingMaxIndex[i][j]->Fill(isTagged, m_cl_fracSamplingMaxIndex->at(0));
          }

          m_eff_jet0_pt[i][j]->Fill(isTagged, m_jet_pt->at(0)/1000);
          m_eff_jet0_eta[i][j]->Fill(isTagged, m_jet_eta->at(0));
          m_eff_jet0_phi[i][j]->Fill(isTagged, m_jet_phi->at(0));
          m_eff_jet0_time[i][j]->Fill(isTagged, m_jet_time->at(0));
          m_eff_jet0_etaPhi[i][j]->Fill(isTagged, m_jet_eta->at(0),m_jet_phi->at(0));
          m_eff_jet0_etaTime[i][j]->Fill(isTagged, m_jet_eta->at(0),m_jet_time->at(0));
          m_eff_jet0_fracSamplingMaxIndex[i][j]->Fill(isTagged, m_jet_fracSamplingMaxIndex->at(0));

          m_eff_bcid[i][j]->Fill(isTagged, m_bcid);
          m_eff_lbn[i][j]->Fill(isTagged, m_lbn);
          m_eff_NPV[i][j]->Fill(isTagged, m_NPV);
          m_eff_mu[i][j]->Fill(isTagged, m_mu);

          if (m_doTaggedClusters) {

            if (m_cl_pt->size() != m_BeamBackgroundData_resultClus->size())
              throw Form("m_cl_pt->size()=%i and m_BeamBackgroundData_resultClus->size()=%i do not  match", m_cl_pt->size(), m_BeamBackgroundData_resultClus->size());

            for (int k=0;k<m_cl_pt->size();k++) 
              if (isTaggedCluster(k, taggerMethods[i])) { 
                m_h_taggedcl_pt[i][j]->Fill(m_cl_pt->at(k)/1000);
                m_h_taggedcl_e[i][j]->Fill(m_cl_e->at(k)/1000);
                m_h_taggedcl_emscale_e[i][j]->Fill(m_cl_emscale_e->at(k)/1000);
                m_h_taggedcl_eta[i][j]->Fill(m_cl_eta->at(k));
                m_h_taggedcl_phi[i][j]->Fill(m_cl_phi->at(k));
                m_h_taggedcl_rPerp[i][j]->Fill(m_cl_rPerp->at(k));
                m_h_taggedcl_z[i][j]->Fill(m_cl_z->at(k));
                m_h_taggedcl_etatime[i][j]->Fill(m_cl_eta->at(k), m_cl_time->at(k));
                m_h_taggedcl_fracSamplingMax[i][j]->Fill(m_cl_fracSamplingMax->at(k));
                m_h_taggedcl_fracSamplingMaxIndex[i][j]->Fill(m_cl_fracSamplingMaxIndex->at(k));            
            }
          }
        }

        if (m_cl_pt->size()) {
          m_h_cl0_pt[j]->Fill(m_cl_pt->at(0)/1000);
          m_h_cl0_e[j]->Fill(m_cl_e->at(0)/1000);
          m_h_cl0_emscale_e[j]->Fill(m_cl_emscale_e->at(0)/1000); 
          m_h_cl0_eta[j]->Fill(m_cl_eta->at(0));
          m_h_cl0_phi[j]->Fill(m_cl_phi->at(0));
          m_h_cl0_rPerp[j]->Fill(m_cl_rPerp->at(0));
          m_h_cl0_z[j]->Fill(m_cl_z->at(0));
          m_h_cl0_time[j]->Fill(m_cl_time->at(0));
          m_h_cl0_fracSamplingMax[j]->Fill(m_cl_fracSamplingMax->at(0));
          m_h_cl0_fracSamplingMaxIndex[j]->Fill(m_cl_fracSamplingMaxIndex->at(0));

        }

        m_h_jet0_pt[j]->Fill(m_jet_pt->at(0) / 1000);
        m_h_jet0_eta[j]->Fill(m_jet_eta->at(0)); 
        m_h_jet0_phi[j]->Fill(m_jet_phi->at(0));
        m_h_jet0_etaphi[j]->Fill(m_jet_eta->at(0),m_jet_phi->at(0));
        m_h_jet0_time[j]->Fill(m_jet_time->at(0));
        m_h_jet0_etatime[j]->Fill(m_jet_eta->at(0),m_jet_time->at(0));
        m_h_jet0_emf[j]->Fill(m_jet_emf->at(0));
        m_h_jet0_hecf[j]->Fill(m_jet_hecf->at(0));
        m_h_jet0_hecq[j]->Fill(m_jet_hecq->at(0));
        m_h_jet0_larq[j]->Fill(m_jet_larq->at(0));
        m_h_jet0_AverageLArQF[j]->Fill(m_jet_AverageLArQF->at(0)/65535);  
        m_h_jet0_negE[j]->Fill(m_jet_negE->at(0)/1000);
        m_h_jet0_fracSamplingMax[j]->Fill(m_jet_fracSamplingMax->at(0)); 
        m_h_jet0_fracSamplingMaxIndex[j]->Fill(m_jet_fracSamplingMaxIndex->at(0)); 
        m_h_jet0_SumPtTrkPt500[j]->Fill(m_jet_SumPtTrkPt500->at(0)/1000);
        
      }
    m_h_beamIntensityB1->Fill(m_beamIntensityB1);
    m_h_beamIntensityB2->Fill(m_beamIntensityB2); 
  }
}

int Sample::getRunNumber(TString name) {
  if (name.Contains("309680")) return 309680;
  else if (name.Contains("309681")) return 309681;
  else if (name.Contains("309682")) return 309682;
  else if (name.Contains("309683")) return 309683;
  else if (name.Contains("309684")) return 309684;
  else if (name.Contains("309685")) return 309685;

  TString projectTag;
  int runNumber;
  TString period;
  TString stream;
  TaggerAnalysisLib::getDatasetInfo(name, projectTag, runNumber, period, stream); 

  return runNumber;
}

bool Sample::eventFailsJetCleaningNoiseCuts(bool useEMFcut) {
  for (unsigned int i=0;i<m_jet_pt->size();i++)
    if (m_jet_pt->at(i)>20000) {

      if (m_jet_hecf->at(i)>0.5 && 
          m_jet_hecq->at(i)>0.5 && 
          m_jet_AverageLArQF->at(i)/65535>0.8) 
        return true;

      if (TMath::Abs(m_jet_negE->at(i))>60000) 
        return true;

      if (m_jet_emf->at(i)>0.95 && 
          m_jet_larq->at(i)>0.8 && 
          m_jet_AverageLArQF->at(i)/65535>0.8 && 
          TMath::Abs(m_jet_eta->at(i))<2.8) 
        return true;

      if (useEMFcut)
        if (m_jet_emf->at(i)<0.1)
          return true;
    }

  return false;
}

bool Sample::eventPassesEventSelection(vector<TString> cutVec) {
  for (int i=0;i<cutVec.size();i++) {

    TString cut = cutVec[i];

    if (cut=="noSelection") return true; 
    if (cut=="passGRL" && !m_passGRL) return false;
    if (cut=="NPV==0" && m_NPV!=0) return false;
    if (cut=="StableBeams" && !m_stableBeams) return false;
    if (cut=="BunchStructure.isUnpairedIsolated" && !m_bs_isUnpairedIsolated)
      return false;
    if (cut=="BunchStructure.isUnpairedB1" && !m_bs_isUnpairedB1) return false;
    if (cut=="BunchStructure.isUnpairedB2" && !m_bs_isUnpairedB2) return false;
     if (cut=="L1_J12_UNPAIRED_ISO" && !m_L1_J12_UNPAIRED_ISO) return false;
    if (cut=="(L1_J12_UNPAIRED_ISO||L1_J12_UNPAIRES_NONISO)" &&
      !m_L1_J12_UNPAIRED_ISO && !m_L1_J12_UNPAIRED_NONISO)
      return false;

    if ( (cut.Contains("AntiKt4EMTopoJets") || cut.Contains("cut_jetCln"))
        && m_jet_pt->size() == 0) return false;

    if (cut=="!cut_jetCln_noise" && eventFailsJetCleaningNoiseCuts(false))
      return false;
    if (cut=="!cut_jetCln_noise_EMF" && eventFailsJetCleaningNoiseCuts(true))
      return false;

    if (cut.Contains("AntiKt4EMTopoJets.pt[0]>")) {
      TString ptCutStr=cut;
      float ptCut = TString(ptCutStr.Tokenize(">")->At(1)->GetName()).Atof();
      if (debug) cout << "ptCut=" << ptCut << endl;
      if (m_jet_pt->at(0) <= ptCut)
        return false;
    }

    if (cut=="TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0" && TMath::Abs(m_jet_eta->at(0)) >= 3.0) 
      return false;

    if (cut=="AntiKt4EMTopoJets.larq[0]<0.05" && m_jet_larq->at(0)>=0.05)
      return false;

    if (cut=="AntiKt4EMTopoJets.time[0]<0" && m_jet_time->at(0)>=0)  return false;
    if (cut=="AntiKt4EMTopoJets.time[0]<-5" && m_jet_time->at(0)>=-5) return false;

    if (cut.Contains("AntiKt4EMTopoJets.fracSamplingMaxIndex[0]>=")) {
       TString fracSamplingMaxCut = cut;
       int sampling = TString(fracSamplingMaxCut.Tokenize(">=")->At(1)->GetName()).Atoi();
       if (m_jet_fracSamplingMaxIndex->at(0)<sampling)
         return false;
    }

    if (cut.Contains("AntiKt4EMTopoJets.fracSamplingMaxIndex[0]<=")) {
       TString fracSamplingMaxCut = cut;
       int sampling = TString(fracSamplingMaxCut.Tokenize("<=")->At(1)->GetName()).Atoi();
       if (m_jet_fracSamplingMaxIndex->at(0)>sampling)
         return false;
    }

    if (cut.Contains("AntiKt4EMTopoJets.fracSamplingMaxIndex[0]!=")) { 
       TString fracSamplingMaxCut = cut;
       int sampling = TString(fracSamplingMaxCut.Tokenize("!=")->At(1)->GetName()).Atoi();
       if (m_jet_fracSamplingMaxIndex->at(0)==sampling) 
         return false;
    }
    
  }

  return true;
}

void Sample::Write(TString outDir) {
  cout<<"Sample::Write outDir="<<outDir << endl;

  TString projectTag, period, stream;
  int runNumber;
  TaggerAnalysisLib::getDatasetInfo(m_filename, projectTag, runNumber, period, stream);

  TString outputFile = outDir+"/"
                      + projectTag+"."
                      + TString::Format("%i",runNumber)+"."+period+"."
                      + ((m_isBeamhaloSample) ? TString("beamhalo") : TString("dijet"))
                      + ".HIST.root";
  TFile* fout = TFile::Open(outputFile,"recreate");

  for (int j=0;j<m_configs.size();j++) {
    TString dir = m_configs[j];
    dir.ReplaceAll("TMath::Abs","Abs");
    fout->mkdir(dir);
    if (!fout->cd(dir)) throw "failed to cd to " + dir;
    for (int i=0;i<nTaggerMethods;i++) {
      Write(m_eff[i][j]);
      //Write(m_eff_wLumi[i][j]);

      Write(m_eff_collidingFirst[i][j]);

      Write(m_eff_cl0_pt[i][j]);
      Write(m_eff_cl0_eta[i][j]);
      Write(m_eff_cl0_phi[i][j]);
      Write(m_eff_cl0_etaphi[i][j]);
      Write(m_eff_cl0_etatime[i][j]);
      Write(m_eff_cl0_time[i][j]);
      Write(m_eff_cl0_fracSamplingMaxIndex[i][j]);

      Write(m_eff_jet0_pt[i][j]);
      Write(m_eff_jet0_eta[i][j]);
      //Write(m_eff_jet0_etaWeighted[i][j]);
      Write(m_eff_jet0_etaPhi[i][j]);
      Write(m_eff_jet0_etaTime[i][j]);
      Write(m_eff_jet0_phi[i][j]);
      Write(m_eff_jet0_time[i][j]);
      Write(m_eff_jet0_fracSamplingMaxIndex[i][j]); 

      Write(m_eff_NPV[i][j]);
      Write(m_eff_mu[i][j]);
      Write(m_eff_bcid[i][j]);
      Write(m_eff_lbn[i][j]);

      Write(m_h_taggedcl_pt[i][j]);
      Write(m_h_taggedcl_e[i][j]);
      Write(m_h_taggedcl_emscale_e[i][j]);
      Write(m_h_taggedcl_eta[i][j]);
      Write(m_h_taggedcl_phi[i][j]);
      Write(m_h_taggedcl_rPerp[i][j]);
      Write(m_h_taggedcl_z[i][j]);
      Write(m_h_taggedcl_etatime[i][j]);
      Write(m_h_taggedcl_fracSamplingMax[i][j]);
      Write(m_h_taggedcl_fracSamplingMaxIndex[i][j]);
    }
    Write(m_h_cl0_pt[j]);
    Write(m_h_cl0_e[j]);
    Write(m_h_cl0_emscale_e[j]);
    Write(m_h_cl0_eta[j]);
    Write(m_h_cl0_phi[j]);
    Write(m_h_cl0_rPerp[j]);
    Write(m_h_cl0_z[j]);
    Write(m_h_cl0_time[j]);
    Write(m_h_cl0_fracSamplingMax[j]);
    Write(m_h_cl0_fracSamplingMaxIndex[j]);

    Write(m_h_jet0_pt[j]);
    Write(m_h_jet0_eta[j]); 
    Write(m_h_jet0_phi[j]);
    Write(m_h_jet0_etaphi[j]);
    Write(m_h_jet0_time[j]);
    Write(m_h_jet0_etatime[j]);
    Write(m_h_jet0_emf[j]);
    Write(m_h_jet0_hecf[j]);
    Write(m_h_jet0_hecq[j]);
    Write(m_h_jet0_larq[j]);
    Write(m_h_jet0_AverageLArQF[j]);
    Write(m_h_jet0_negE[j]);
    Write(m_h_jet0_fracSamplingMax[j]);
    Write(m_h_jet0_fracSamplingMaxIndex[j]);
    Write(m_h_jet0_SumPtTrkPt500[j]);

    Write(m_h_beamIntensityB1);
    Write(m_h_beamIntensityB2);
  }

  fout->Close();
}

void Sample::Write(TObject* eff) {
  TString name=eff->GetName();
  if (name.Contains("_config"))
    name.Remove(name.Index("_config"));
  eff->Write(name);
}

vector<int> Sample::decisionBit(TString methodStr) {
  int bit;
  vector<int> bitVec;
  TObjArray* objArray = methodStr.Tokenize("&&");
  for (int i=0;i<objArray->GetEntries();i++) {
    TString method = objArray->At(i)->GetName();
    if (method=="Segment")              bit=1024;
    else if (method=="SegmentEarly")    bit=2048;
    else if (method=="SegmentACNoTime") bit=8192;
    else if (method=="SegmentAC")       bit=4096;
    else if (method=="Matched")         bit=   1;
    else if (method=="NoTimeLoose")     bit=   2;
    else if (method=="NoTimeMedium")    bit=   4;
    else if (method=="NoTimeTight")     bit=   8;
    else if (method=="OneSidedLoose")   bit=  16;
    else if (method=="OneSidedMedium")  bit=  32;
    else if (method=="OneSidedTight")   bit=  64;
    else if (method=="TwoSidedNoTime")  bit= 512;
    else if (method=="TwoSided")        bit= 128;
    else if (method=="ClusterShape")    bit= 256;
    else throw "Sample::decisionBit unknown method: "+methodStr;
    bitVec.push_back(bit);
  }
  delete objArray;
  return bitVec;
}

bool Sample::isTaggedEvent(TString methodStr) {

  TObjArray* objArray = methodStr.Tokenize("&&");
  if (objArray->GetEntries()==1) {
    int i=0;
    bool found=false;
    for (;i<nTaggerMethods;i++)
      if (taggerMethods[i]==methodStr) {
        found=true;
        break;
      }
    if (!found) throw "isTaggedEvent: unknown method: "+methodStr;
    delete objArray;
    return (m_BeamBackgroundData_numEvents[i] != 0); 
  }

  else {
    if (methodStr.Contains("Segment"))
      throw "isTaggedEvent: Segment AND methods not implemented";
    for (int clusIndex=0;clusIndex<m_cl_pt->size();clusIndex++) 
      if (isTaggedCluster(clusIndex, methodStr))
        return true;
  } 

  return false;
}

bool Sample::isTaggedCluster(int clusterIndex, TString method) {

  int resultClus = m_BeamBackgroundData_resultClus->at(clusterIndex);

  vector<int> decisionBitVec = decisionBit(method);
 
  for (int decisionBit : decisionBitVec)
    if ((resultClus & decisionBit)==0) 
      return false;

  return true; 
}

int main(int argc, char** argv) {

  TString samplefile="";
  TString outDir="";
  int evtMax = -1;
  bool isDijetSample=false;
  bool isBeamhaloSample=false;

  for (int i=1;i<argc;i++) {
    if (TString(argv[i]).BeginsWith("--sample")) {
      TString sample=argv[i+1];
      samplefile=sample.Tokenize(":")->At(0)->GetName();
      TString sampleType=sample.Tokenize(":")->At(1)->GetName();
      if (sampleType=="beamhalo") isBeamhaloSample=true;
      else if (sampleType=="dijet") isDijetSample=true;
      if (isBeamhaloSample) cout << "beamhalo sample" << endl;
      if (isDijetSample) cout << "dijet sample"<< endl;
     }
    if (TString(argv[i])=="--evtMax")
      evtMax=TString(argv[i+1]).Atoi();
    if (TString(argv[i])=="--outDir")
      outDir=argv[i+1];
  }
  if (samplefile=="") {cout<<"Bad syntax."<<endl;return 1;}

  vector<TString> jetCleaningNoiseCuts = {
    "!cut_jetCln_noise",
    "!cut_jetCln_noise_EMF"
  };

  vector<float> ptCutVec={20,120,200,600};
  vector<TString> nominalConfigs = {
    "passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>%.0f&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0&&",

    // jet larq cut
    "passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>%.0f&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0&&AntiKt4EMTopoJets.larq[0]<0.05&&",

    // jet time < 0 
    "passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>%.0f&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0&&AntiKt4EMTopoJets.larq[0]<0.05&&AntiKt4EMTopoJets.time[0]<0&&",

    // jet time < -5
    "passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>%.0f&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0&&AntiKt4EMTopoJets.larq[0]<0.05&&AntiKt4EMTopoJets.time[0]<-5&&",

    // unpaired bunches beam 1
    "passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&BunchStructure.isUnpairedB1&&AntiKt4EMTopoJets.pt[0]>%.0f&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0&&",

    // unpaired bunches beam 2
    "passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&BunchStructure.isUnpairedB2&&AntiKt4EMTopoJets.pt[0]>%.0f&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0&&",

   // L1_J12_UNAPAIRED_ISO
   "passGRL&&StableBeams&&NPV==0&&L1_J12_UNPAIRED_ISO&&AntiKt4EMTopoJets.pt[0]>%.0f&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0&&",

   // L1_J12_UNAPIRED_ISO/NONISO
   //"passGRL&&StableBeams&&NPV==0&&(L1_J12_UNPAIRED_ISO||L1_J12_UNPAIRED_NONISO)&&AntiKt4EMTopoJets.pt[0]>%.0f&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0&&" 
  };
  vector<TString> beamhaloConfigs;
 
  for (TString jetCleaningNoiseCut : jetCleaningNoiseCuts)
    for (float ptCut : ptCutVec)
      for (TString nominalConfig : nominalConfigs) 
        beamhaloConfigs.push_back(Form(nominalConfig.Data(),ptCut*1000)+jetCleaningNoiseCut);

  // first nominal configuration unfolded excluding:
  //   EMEC
  //   EMEC+HEC
  //   EMEC+HEC+TileGap3
  //   EMEC+HEC+TileGap3+TileEB
  vector<int> excludedCalos = {2,3,5,6};
  for (int i : excludedCalos) {
    TString tmp;
    TString config=(tmp=nominalConfigs[0]).ReplaceAll("%.0f","120000");
    for (int j : excludedCalos)
      if (j<=i) {
        TString config0="";
        for (int k=calo_samplings[j].first;k<=calo_samplings[j].second;k++)
          config0+=Form("AntiKt4EMTopoJets.fracSamplingMaxIndex[0]!=%i&&",k);
        config+=config0;
      }
    beamhaloConfigs.push_back(config+jetCleaningNoiseCuts[0]); 
  }

  TString tmp;
  for (int i=0;i<24;i++)
    beamhaloConfigs.push_back((tmp=nominalConfigs[0]).ReplaceAll("%.0f","120000")+Form("AntiKt4EMTopoJets.fracSamplingMaxIndex[0]<=%i&&",i)+jetCleaningNoiseCuts[0]);

  vector<TString> misIdProbConfigs = {
    //"StableBeams"
    "noSelection"
  };

  try {

    Sample* sample;
    if (isBeamhaloSample) 
      sample = new Sample(samplefile, beamhaloConfigs, true);
    else  
      sample = new Sample(samplefile, misIdProbConfigs, false);
 
    sample->FillHistograms(evtMax);

    sample->Write(outDir);

  }

  catch (const char* err) { cout << err << endl;cout << err << endl;return 1;}
  catch (const TString* err) { cout << err << endl;cout << err << endl;return 1;}

  return 0;
}


