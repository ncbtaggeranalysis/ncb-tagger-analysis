#ifndef BIB_TAGGER_MIS_ID_PROB
#define BIB_TAGGER_MIS_ID_PROB

#include <vector>
#include <iostream>
using namespace std;
#include <string>
#include <utility> //pair
#include <vector>

#include <TH1F.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TVector2.h>

#include <AthLinks/ElementLinkVector.h>

//#include "GoodRunsLists/GoodRunsListSelectionTool.h"
#include "PathResolver/PathResolver.h"
#include "JetSelectorTools/JetCleaningTool.h"

#include "xAODRootAccess/TEvent.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "JetCalibTools/JetCalibUtils.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegmentAuxContainer.h"

#include "NCBanalysis/EventHist.h"
#include "NCBanalysis/MuonSegmentHist.h"
#include "NCBanalysis/JetHist.h"
#include "NCBanalysis/BeamBackgroundDataHist.h"

#include "NCBanalysis/BeamBackgroundData.h"
#include "NCBanalysis/NtupleWriter.h"

using namespace std;
using namespace CP;

// had trouble declaring the bins as vector<pair<float,float>> = { {0,1000}, {0,10}, {10,20}, etc }
// got: "double free or corruption" 

const extern int muNbins = 
//11;
1;
const extern float muBins[muNbins][2] = { 
  {0,1000} 
  /*{0,5}, 
  {5,10}, 
  {10,15}, 
  {15,20}, 
  {20,25},
  {25,30}, 
  {30,35},
  {35,40}, 
  {40,45},
  {45,50}*/ 
};

class BIBTaggerMisIdProb {

  public:
  
  BIBTaggerMisIdProb(vector<string> GRL, TString jetClnCut, TString jetCollection, float jetPtcut, bool useJVT, float dphiCut, float chf1Cut);
        ~BIBTaggerMisIdProb();

    void Run(xAOD::TEvent& event);
    void Write(TString filename);

    void SetTrigDecTool(Trig::TrigDecisionTool* trigTool) {m_trigTool = trigTool; };   
    void SetBeamBackgroundData(BeamBackgroundData* beamBackgroundData) { m_beamBackgroundData = beamBackgroundData; }; 

    void initNtupleWriter();

  private:

    //GoodRunsListSelectionTool *m_grlTool;
    JetCleaningTool *m_clnTool;
    MuonSelectionTool* m_muonSelTool;
    MuonCalibrationAndSmearingTool* m_muonCalibTool;
    Trig::TrigDecisionTool* m_trigTool;

    BeamBackgroundData* m_beamBackgroundData;

    TString muBinStr(int index);
    bool eventIsInMuBin(float mu, int index);

    int m_nTagged;
    int m_nDijet;
    TString m_jetClnCut;
    TString m_jetCollection;
    float m_jetPtcut;
    bool m_useJVT;
    float m_dphiCut;
    float m_chf1Cut;

    float m_muMin;
    float m_muMax;

    TString m_name;

    TH1F* m_hcutflow;
    
    vector<EventHist*> m_eventHistNoSel;
    vector<EventHist*> m_eventHist;

    vector<vector<BeamBackgroundDataHist*>> m_beamBackgroundDataHistNoSel;
    vector<vector<BeamBackgroundDataHist*>> m_beamBackgroundDataHist;

    vector<JetHist*> m_jetHist_allJets;
    vector<JetHist*> m_jetHist[4];
    vector<JetHist*> m_jetHist_tagged[4];

    MuonSegmentHist* m_muonSegHist;

    vector<TH1F*> m_hpTasym;
    vector<TH1F*> m_hdphi;

    NtupleWriter* m_ntupleWriter;
    
};

#endif
