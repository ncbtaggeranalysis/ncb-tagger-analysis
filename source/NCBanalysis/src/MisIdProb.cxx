#include "NCBanalysis/MisIdProb.h"

MisIdProb::MisIdProb(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_grlTool("GoodRunsListSelectionTool/GRLTool"),
  m_jetCleaningToolLoose("JetCleaningTool/JetCleaningToolLoose"),
  m_muonSelectionToolLoose("CP::MuonSelectionTool/MuonSelectionToolLoose"),
  m_muonCalibTool("CP::MuonCalibrationAndSmearingTool/MuonCalibrationAndSmearingTool"),
  m_histTool("HistTool/HistTool"),
  m_ntupleWriterTool("NtupleWriterTool/NtupleWriterTool"),
  m_name(name)
{
  //declareProperty("GRLTool", m_grlTool);
  //declareProperty("JetCleaningToolLoose", m_jetCleaningToolLoose);
  //declareProperty("MuonSelectionToolLoose", m_muonSelectionToolLoose);
  //declareProperty("MuonCalibrationAndSmearingTool", m_muonCalibTool);
  //declareProperty("HistTool", m_histTool);
  //declareProperty("NtupleWriterTool", m_ntupleWriterTool);
  declareProperty("JetCollection", m_jetCollection = "AntiKt4EMTopoJets");
  declareProperty("cut_jet1Pt", m_cut_jet1Pt = 120000);
  declareProperty("cut_jet1chf", m_cut_jet1chf = 0.02);
  declareProperty("cut_pTasymm", m_cut_pTasymm = 0.4);
  declareProperty("cut_deltaPhi", m_cut_deltaPhi = 2.8);
  declareProperty("use_GRL", m_use_GRL = false);
  declareProperty("use_electron_requirement", m_use_electron_requirement = true);
  declareProperty("use_muon_requirement", m_use_muon_requirement = true);
  declareProperty("use_HistTool", m_use_HistTool = true);
}

MisIdProb::~MisIdProb() {
}

StatusCode MisIdProb::initialize() {
  ATH_CHECK(service("THistSvc",m_thistSvc));

  // For some reason the tools at this point are not already initialized. The log message is that (same for all tools):
  // MisIdProbAlg        DEBUG ToolHandle NtupleWriterTool/NtupleWriterTool not used
  // In this case initialize the tools by hand.
  if (m_grlTool->FSMState() < Gaudi::StateMachine::INITIALIZED)
    ATH_CHECK(m_grlTool->initialize());
  if (m_jetCleaningToolLoose->FSMState() < Gaudi::StateMachine::INITIALIZED)
    ATH_CHECK(m_jetCleaningToolLoose->initialize());
  if (m_muonSelectionToolLoose->FSMState() < Gaudi::StateMachine::INITIALIZED)
    m_muonSelectionToolLoose->initialize();
  if (m_muonCalibTool->FSMState() < Gaudi::StateMachine::INITIALIZED)
    m_muonCalibTool->initialize();
  if (m_histTool->FSMState() < Gaudi::StateMachine::INITIALIZED)
    ATH_CHECK(m_histTool->initialize());
  if (m_ntupleWriterTool->FSMState() < Gaudi::StateMachine::INITIALIZED)
    ATH_CHECK(m_ntupleWriterTool->initialize());

  const TString cutflow_labels[]={
    "No selection",
    "GRL",
    "NPV #geq 1",
    "jet cleaning",
    "electron veto",
    "muon veto",
    "N_{jet}>=2",
    "Jvt^{jet1}>0.8 && Jvt^{jet2}>0.8",
    Form("jet1 p_{T}>%.0f GeV,#lbar#eta#lbar<2", m_cut_jet1Pt/1000),
    Form("jet1 chf>%.02f and EMF>0.1",m_cut_jet1chf),
    "Dijet asymmetry<0.4",
    Form("#Delta #phi(jet1,jet2)>%.1f",m_cut_deltaPhi)
  };
  const unsigned int nBins = sizeof(cutflow_labels)/sizeof(TString);
  m_hcutflow = new TH1F("cutflow_"+m_name,"",nBins,0,nBins);
  for (unsigned int i=0;i<nBins;i++)
    m_hcutflow->GetXaxis()->SetBinLabel(i+1,cutflow_labels[i]);
  m_hcutflow->GetXaxis()->LabelsOption("v");

  if (m_thistSvc->regHist("/MisIdProb/cutflow",m_hcutflow).isFailure()) {
     ATH_MSG_INFO("failed to register cutflow histogram");
     return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode MisIdProb::execute() {
  //
  // back-to-back di-jet event selection from ATL-COM-DAPR-2012-001 Table 7 
  //

  m_hcutflow->Fill(0);
  const xAOD::EventInfo* eventInfo = 0;
  if (evtStore()->retrieve( eventInfo, "EventInfo").isFailure()) {
    ATH_MSG_ERROR( "Failed to retrieve EventInfo" );
    return StatusCode::FAILURE;
  }

  uint32_t runNumber = eventInfo->runNumber();
  uint32_t lbn = eventInfo->lumiBlock();
  if (m_use_GRL) {
    if (!m_grlTool->passRunLB(runNumber, lbn)) 
      return StatusCode::SUCCESS;
    m_hcutflow->Fill(1);
  }

  const xAOD::VertexContainer* primaryVertices = 0;
  if (evtStore()->retrieve( primaryVertices, "PrimaryVertices").isFailure()) {
    ATH_MSG_ERROR( "Failed to retrieve PrimaryVertices" );
    return StatusCode::FAILURE;
  }

  m_npv=0;
  for (size_t iVertex=0; iVertex< primaryVertices->size(); iVertex++)
    if ( primaryVertices->at(iVertex)->vertexType() == xAOD::VxType::VertexType::PriVtx ||
         primaryVertices->at(iVertex)->vertexType() == xAOD::VxType::VertexType::PileUp)
      m_npv++;
  if (m_npv==0)
    return StatusCode::SUCCESS;
  m_hcutflow->Fill(2); 

  const xAOD::JetContainer* jets = 0;
  if (evtStore()->retrieve(jets, m_jetCollection).isFailure()) {
    ATH_MSG_ERROR( "Failed to retrieve the jets" );
    return StatusCode::FAILURE;
  }
  for (const xAOD::Jet* jet : *jets)
    if (!m_jetCleaningToolLoose->keep(*jet))
      return StatusCode::SUCCESS;
  m_hcutflow->Fill(3);

  if (m_use_electron_requirement) {
    const xAOD::ElectronContainer* electrons = 0;
    if (evtStore()->retrieve(electrons, "Electrons").isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve the electrons");
      return StatusCode::FAILURE;
    }
    bool passElectronVeto = true;
    for (const xAOD::Electron* el :* electrons) {
      //cout << Form("electrons:  pt=%5.0f eta=%5.1f passSelection=%i", el->pt()/1000, el->eta(), el->passSelection(xAOD::EgammaParameters::Loose)) << endl;
      if (el->pt()>10000 && el->passSelection(xAOD::EgammaParameters::Loose))
        passElectronVeto = false;
    }
    if (!passElectronVeto)
      return StatusCode::SUCCESS; 
    m_hcutflow->Fill(4);
  }

  if (m_use_muon_requirement) {
    const xAOD::MuonContainer* muons = 0;
    if (evtStore()->retrieve(muons, "Muons").isFailure()) {
      ATH_MSG_ERROR("Failed to retrieve the muons");
      return StatusCode::FAILURE;
    }
    cout << "#muons: " << muons->size() << endl;
    bool passMuonVeto = true;
    for (const xAOD::Muon* muon : *muons) {
      xAOD::Muon* calibMuon = new xAOD::Muon();
      calibMuon->makePrivateStore(*muon);
      m_muonCalibTool->applyCorrection(*calibMuon);
      bool goodMuon = false; // (m_muonSelTool->accept(muon)) ? true : false;
      if (m_muonSelectionToolLoose->getQuality(*calibMuon) <= xAOD::Muon::Loose) {
        goodMuon = true;
        //cout << Form("muon pt=%5.0f eta=%5.1f calib pt=%5.0f eta=%5.1f author=%i goodMuon=%i", muon->pt()/1000, muon->eta(),calibMuon->pt()/1000,calibMuon->eta(), muon->author(),goodMuon) << endl;
      }
      if (goodMuon && calibMuon->pt() > 10000)
        passMuonVeto = false;
    
      delete calibMuon;
    }
    if (!passMuonVeto)
      return StatusCode::SUCCESS;
    m_hcutflow->Fill(5);
  }

  if (jets->size() < 2)
    return StatusCode::SUCCESS;
  m_hcutflow->Fill(6);

  if (jets->at(0)->getAttribute<float>("Jvt")<0.8 || 
      jets->at(1)->getAttribute<float>("Jvt")<0.8)
    return StatusCode::SUCCESS;
  m_hcutflow->Fill(7);

  if (jets->at(0)->pt()<m_cut_jet1Pt || TMath::Abs(jets->at(0)->eta())>2)
    return StatusCode::SUCCESS;
  m_hcutflow->Fill(8);  

  vector<float> chfVec;
  vector<float> emfVec;
  for (const xAOD::Jet* jet : *jets) {
    vector<float> SumPtTrkPt500Vec;
    jet->getAttribute<std::vector<float>>(xAOD::JetAttribute::SumPtTrkPt500,SumPtTrkPt500Vec);
    float SumPtTrkPt500 = SumPtTrkPt500Vec.size() != 0 ? SumPtTrkPt500Vec.at(0) : 0;
    chfVec.push_back(SumPtTrkPt500 / jet->pt());
    emfVec.push_back(jet->getAttribute<float>("EMFrac"));
  }  

  if (chfVec[0] < 0.02 || emfVec[0]<0.1)
    return StatusCode::SUCCESS;
  m_hcutflow->Fill(9);

  float pTasymm = TMath::Abs(jets->at(0)->pt()-jets->at(1)->pt()) /
                  ((jets->at(0)->pt()+jets->at(1)->pt()) / 2); 
  if (pTasymm>m_cut_pTasymm)
    return StatusCode::SUCCESS;
  m_hcutflow->Fill(10);
 
  float deltaPhi = TMath::Abs(TVector2::Phi_mpi_pi(jets->at(0)->phi()-jets->at(1)->phi()));
  if (deltaPhi < m_cut_deltaPhi)
    return StatusCode::SUCCESS;
  m_hcutflow->Fill(11);

  for (int i=0;i<2;i++)
    cout << "jet pt=" << jets->at(i)->pt()
                      << " eta=" << jets->at(i)->eta()
                      << " Jvt=" << jets->at(i)->getAttribute<float>("Jvt")
                      << endl;
  cout << "pTasymm=" << pTasymm << endl
       << "deltaPhi=" << deltaPhi << endl;

  if (m_use_HistTool)
    ATH_CHECK(m_histTool->Fill());
  ATH_CHECK(m_ntupleWriterTool->Fill());

  return StatusCode::SUCCESS;
}

StatusCode MisIdProb::finalize() {
  m_hcutflow->Print("all");
  return StatusCode::SUCCESS;
}
