#ifndef MISIDPROB_H 
#define MISIDPROB_H

#include <string>
using namespace std;
#include <iostream>
#include <string>

#include <TH1F.h>
#include <TString.h>
#include <TMath.h>
#include <TVector2.h>

#include <AthenaBaseComps/AthAlgorithm.h>
#include <GaudiKernel/ToolHandle.h>
#include <GoodRunsLists/IGoodRunsListSelectionTool.h>
#include "JetInterface/IJetSelector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"
#include "NCBanalysis/IHistTool.h"
#include "NCBanalysis/NtupleWriterTool.h"

class MisIdProb : public AthAlgorithm {
  public:

               MisIdProb(const std::string& name, ISvcLocator* pSvcLocator);
              ~MisIdProb();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:
    //ToolHandle<Trig::TrigDecisionTool> m_trigDecTool;
    ToolHandle<IGoodRunsListSelectionTool> m_grlTool;
    ToolHandle<IJetSelector> m_jetCleaningToolLoose;
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionToolLoose;
    ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muonCalibTool;
    ToolHandle<IHistTool> m_histTool;
    ToolHandle<INtupleWriterTool> m_ntupleWriterTool;
    ITHistSvc* m_thistSvc;

    TString m_name;
    string m_jetCollection;
    TH1F* m_hcutflow;
    unsigned int m_npv;
    float m_cut_jet1Pt;
    float m_cut_jet1chf;
    float m_cut_pTasymm;
    float m_cut_deltaPhi;
    bool m_use_GRL;
    bool m_use_electron_requirement;
    bool m_use_muon_requirement;
    bool m_use_HistTool;
};

#endif
