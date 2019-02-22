#include "NCBanalysis/HistTool.h"

const float Pi = TMath::Pi();

HistTool::HistTool(const string& type, const string& name, const IInterface* parent) :
  AthAlgTool(type,name,parent),
  m_name(name) {
  declareProperty("OutputStream", m_outputStream = "HistTool");
  declareProperty("nMonitoredJets", m_nMonitoredJets = 2);
  declareProperty("JetCollection", m_jetCollection = "AntiKt4EMTopoJets");
  m_name.ReplaceAll("ToolSvc.","");
  ATH_MSG_INFO("HistTool constructor");
}

HistTool::~HistTool() {
}

StatusCode HistTool::initialize() {
  ATH_MSG_INFO("HistTool::initialize");
  
  m_hist.push_back(new TH1F("NPV_"+m_name,m_name+";NPV",50,0,50)); 
  m_hist.push_back(new TH1F("mu_"+m_name,m_name+";<#mu>",50,0,50));
  m_hist.push_back(new TH1F("bcid_"+m_name,m_name+";bcid",3188,0,3188));
  m_hist.push_back(new TH1F("lbn_"+m_name,m_name+";lbn",3000,0,3000));
  m_hist.push_back(new TH1F("larError_"+m_name,m_name+";larError",3,0,3));
  m_hist.push_back(new TH1F("tileError_"+m_name,m_name+";tileError",3,0,3));
  m_hist.push_back(new TH1F("backgroundFlagBit_"+m_name,m_name+";backgroundFlag bit",32,0,32));

  for (unsigned int i=0;i<m_nMonitoredJets;i++) {
    m_hist.push_back(new TH1F(Form("jet%i_eta_%s",i, m_name.Data()), 
                     Form("%s;jet%i #eta",m_name.Data(),i),100,-5,5));
    m_hist.push_back(new TH1F(Form("jet%i_phi_%s",i, m_name.Data()), 
                     Form("%s;jet%i #phi",m_name.Data(),i),100,-Pi,Pi));
    m_hist.push_back(new TH1F(Form("jet%i_pt_%s",i, m_name.Data()), 
                     Form("%s;jet%i p_{T} [GeV]",m_name.Data(), i),100,0,1000));
    m_hist.push_back(new TH1F(Form("jet%i_EMFrac_%s",i, m_name.Data()), 
                     Form("%s;jet%i EMFrac",m_name.Data(), i),100,-0.5,1.5));
    m_hist.push_back(new TH1F(Form("jet%i_HECFrac_%s",i, m_name.Data()), 
                     Form("%s;jet%i HECFrac",m_name.Data(), i),100,-0.5,1.5));
    m_hist.push_back(new TH1F(Form("jet%i_LArQuality_%s",i, m_name.Data()), 
                     Form("%s;jet%i LArQuality",m_name.Data(), i),100,-0.5,1.5));
    m_hist.push_back(new TH1F(Form("jet%i_NegativeE_%s",i, m_name.Data()), 
                     Form("%s;jet%i NegativeE",m_name.Data(), i),100,-100,0));
    m_hist.push_back(new TH1F(Form("jet%i_HECQuality_%s",i, m_name.Data()), 
                     Form("%s;jet%i HECQuality",m_name.Data(), i),100,-0.5,1.5));
    m_hist.push_back(new TH1F(Form("jet%i_AverageLArQF_%s",i, m_name.Data()),
                     Form("%s;jet%i AverageLArQF",m_name.Data(), i),100,-0.5,1.5));
    m_hist.push_back(new TH1F(Form("jet%i_chf_%s",i, m_name.Data()), 
                     Form("%s;jet%i chf",m_name.Data(), i),100,-0.5,1.5));
    m_hist.push_back(new TH1F(Form("jet%i_FracSamplingMax_%s",i, m_name.Data()), 
                     Form("%s;jet%i FracSamplingMax",m_name.Data(), i),100,-0.5,1.5));
    m_hist.push_back(new TH1F(Form("jet%i_FracSamplingMaxIndex_%s",i, m_name.Data()),
                     Form("%s;jet%i FracSamplingMaxIndex",m_name.Data(), i),24,0,24));
    m_hist.push_back(new TH1F(Form("jet%i_Timing_%s",i, m_name.Data()), 
                     Form("%s;jet%i Timing [ns]",m_name.Data(), i),200,-50,50));
    m_hist.push_back(new TH1F(Form("jet%i_Jvt_%s",i, m_name.Data()),
                     Form("%s;jet%i Jvt",m_name.Data(), i),100,-0.5,1.5));
  }
  m_hist.push_back(new TH1F(Form("jet0_jet1_pTasymm_%s", m_name.Data()),
                     Form("%s;p_{T}^{asymm}(jet0, jet1)",m_name.Data()),100,-0.5,5));
  m_hist.push_back(new TH1F(Form("jet0_jet1_deltaPhi_%s", m_name.Data()),
                     Form("%s;|#delta#phi(jet0, jet1)|",m_name.Data()),100,0,3.1415));

  m_hist.push_back(new TH1F(Form("MuonSegments_MDT_EIS_EIL_t0_%s",m_name.Data()),
                            Form("%s;MuonSegments MDT EIS EIL t0 [ns]",m_name.Data()),
                            200,-100,100));
  m_hist.push_back(new TH1F(Form("MuonSegments_MDT_EIS_EIL_t0error_%s",m_name.Data()),
                            Form("%s;MuonSegments MDT EIS EIL t0error [ns]",m_name.Data()),
                            200,-100,100));
  m_hist.push_back(new TH1F(Form("MuonSegments_MDT_EIS_EIL_sector_%s",m_name.Data()),
                            Form("%s;MuonSegments MDT EIS EIL sector",m_name.Data()),
                            16,0,16));
  m_hist.push_back(new TH1F(Form("MuonSegments_MDT_EIS_EIL_nPrecisionHits_%s",m_name.Data()),
                         Form("%s;MuonSegments MDT EIS EIL nPrecisionHits",m_name.Data()),
                            20,0,20));
  m_hist.push_back(new TH1F(Form("MuonSegments_MDT_EIS_EIL_thetaPos_%s",m_name.Data()),
                          Form("%s;MuonSegments MDT EIS EIL #theta_{Pos}",m_name.Data()),
                            180,0,180));
  m_hist.push_back(new TH1F(Form("MuonSegments_MDT_EIS_EIL_thetaDir_%s",m_name.Data()),                             Form("%s;MuonSegments MDT EIS EIL #theta_{Dir}",m_name.Data()),
                            180,0,180));
  m_hist.push_back(new TH1F(Form("MuonSegments_MDT_EIS_EIL_deltaTheta_%s",m_name.Data()),                             Form("%s;MuonSegments MDT EIS EIL |#theta_{Pos}-#theta_{Dir}|",m_name.Data()),
                            180,0,180)); 
  m_hist.push_back(new TH1F(Form("MuonSegments_MDT_EIS_EIL_phi_%s",m_name.Data()),                             Form("%s;MuonSegments MDT EIS EIL #phi",m_name.Data()),
                            100,-Pi,Pi));

  //
  m_hist.push_back(new TH1F(Form("NCB_MuonSegments_CSC_t0_%s",m_name.Data()),
                            Form("%s;NCB_MuonSegments CSC t0 [ns]",m_name.Data()),
                            200,-100,100));
  m_hist.push_back(new TH1F(Form("NCB_MuonSegments_CSC_t0error_%s",m_name.Data()),
                            Form("%s;NCB_MuonSegments CSC t0error [ns]",m_name.Data()),
                            200,-100,100));
  m_hist.push_back(new TH1F(Form("NCB_MuonSegments_CSC_sector_%s",m_name.Data()),
                            Form("%s;NCB_MuonSegments CSC sector",m_name.Data()),
                            16,0,16));
  m_hist.push_back(new TH1F(Form("NCB_MuonSegments_CSC_nPrecisionHits_%s",m_name.Data()),
                            Form("%s;NCB_MuonSegments CSC nPrecisionHits",m_name.Data()),
                            16,0,16));
  m_hist.push_back(new TH1F(Form("NCB_MuonSegments_CSC_thetaPos_%s",m_name.Data()),
                            Form("%s;NCB_MuonSegments CSC #theta_{Pos}",m_name.Data()),
                            180,0,180));
  m_hist.push_back(new TH1F(Form("NCB_MuonSegments_CSC_thetaDir_%s",m_name.Data()),
                            Form("%s;NCB_MuonSegments CSC #theta_{Dir}",m_name.Data()),
                            180,0,180));
  m_hist.push_back(new TH1F(Form("NCB_MuonSegments_CSC_deltaTheta_%s",m_name.Data()),
                            Form("%s;NCB_MuonSegments CSC |#theta_{Pos}-#theta_{Dir}|",m_name.Data()),
                            180,0,180)); 
  m_hist.push_back(new TH1F(Form("NCB_MuonSegments_CSC_phi_%s",m_name.Data()),                             Form("%s;NCB_MuonSegments CSC #phi",m_name.Data()),
                            100,-Pi,Pi));

  ATH_CHECK(service("THistSvc",m_thistSvc));
  for (TH1F* hist : m_hist)
    if (m_thistSvc->regHist("/"+m_outputStream+"/"+string(hist->GetName()), hist).isSuccess()) 
      ATH_MSG_INFO("registered histogram: " << hist->GetName());
    else {
      ATH_MSG_ERROR("failed to register histogram: " << hist->GetName());
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}

StatusCode HistTool::Fill() {
  
  const xAOD::EventInfo* eventInfo = 0;
  if (evtStore()->retrieve( eventInfo, "EventInfo").isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve EventInfo");
    return StatusCode::FAILURE;
  }

  const xAOD::VertexContainer* primaryVertices = 0;
  if (evtStore()->retrieve( primaryVertices, "PrimaryVertices").isFailure()) {
    ATH_MSG_ERROR( "Error: failed to retrieve PrimaryVertices");
    return StatusCode::FAILURE;
  }
  int NPV=0;
  for (size_t iVertex=0; iVertex< primaryVertices->size(); iVertex++) {
    if ( primaryVertices->at(iVertex)->vertexType() == xAOD::VxType::VertexType::PriVtx ||
         primaryVertices->at(iVertex)->vertexType() == xAOD::VxType::VertexType::PileUp)
      NPV++;
  }

  getHist("NPV")->Fill(NPV);
  getHist("mu")->Fill(eventInfo->averageInteractionsPerCrossing());
  getHist("bcid")->Fill(eventInfo->bcid());
  getHist("lbn")->Fill(eventInfo->lumiBlock());
  getHist("larError")->Fill(eventInfo->errorState(xAOD::EventInfo::LAr));
  getHist("tileError")->Fill(eventInfo->errorState(xAOD::EventInfo::Tile));

  uint32_t backgroundFlag = eventInfo->eventFlags(xAOD::EventInfo::Background);
  for (int bit=0;bit<32;bit++)
    if (backgroundFlag&(1<<bit))
      getHist("backgroundFlagBit")->Fill(bit);

  const xAOD::JetContainer* jets = 0;
  if (evtStore()->retrieve(jets, m_jetCollection).isFailure()) {
    ATH_MSG_ERROR( "Failed to retrieve the jets" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("HistTool::Fill jets->size=" << jets->size());
  for (unsigned int i=0;i<m_nMonitoredJets;i++)
    if (i<jets->size()) {
      getHist(Form("jet%i_eta",i))->Fill(jets->at(i)->eta());
      getHist(Form("jet%i_phi",i))->Fill(jets->at(i)->phi());
      getHist(Form("jet%i_pt",i))->Fill(jets->at(i)->pt()/1000);
      getHist(Form("jet%i_EMFrac",i))->Fill(jets->at(i)->getAttribute<float>("EMFrac"));
      getHist(Form("jet%i_HECFrac",i))->Fill(jets->at(i)->getAttribute<float>("HECFrac"));
      getHist(Form("jet%i_LArQuality",i))->Fill(jets->at(i)->getAttribute<float>("LArQuality"));
      getHist(Form("jet%i_HECQuality",i))->Fill(jets->at(i)->getAttribute<float>("HECQuality"));
      getHist(Form("jet%i_AverageLArQF",i))->Fill(jets->at(i)->getAttribute<float>("AverageLArQF")/65535);
      getHist(Form("jet%i_NegativeE",i))->Fill(jets->at(i)->getAttribute<float>("NegativeE")/1000);
      vector<float> SumPtTrkPt500Vec;
      jets->at(i)->getAttribute<std::vector<float>>(xAOD::JetAttribute::SumPtTrkPt500,SumPtTrkPt500Vec);
      float SumPtTrkPt500 = SumPtTrkPt500Vec.size() != 0 ? SumPtTrkPt500Vec.at(0) : 0;
      getHist(Form("jet%i_chf",i))->Fill(SumPtTrkPt500 / jets->at(i)->pt());
      getHist(Form("jet%i_FracSamplingMax",i))->Fill(jets->at(i)->getAttribute<float>("FracSamplingMax"));
      getHist(Form("jet%i_FracSamplingMaxIndex",i))->Fill(jets->at(i)->getAttribute<int>("FracSamplingMaxIndex"));  
      getHist(Form("jet%i_Timing",i))->Fill(jets->at(i)->getAttribute<float>("Timing"));
      getHist(Form("jet%i_Jvt",i))->Fill(jets->at(i)->getAttribute<float>("Jvt"));
    }
  if (jets->size()>=2) {
    getHist("jet0_jet1_pTasymm")->Fill(TMath::Abs(jets->at(0)->pt()-jets->at(1)->pt()) /
                  ((jets->at(0)->pt()+jets->at(1)->pt()) / 2));
    getHist("jet0_jet1_deltaPhi")->Fill(TMath::Abs(TVector2::Phi_mpi_pi(jets->at(0)->phi()-jets->at(1)->phi())));
  }

  const xAOD::MuonSegmentContainer* muonSegments = 0;
  if (evtStore()->retrieve(muonSegments,"MuonSegments").isFailure()) {
      ATH_MSG_INFO( "failed to retrieve the MuonSegments." );
      return StatusCode::FAILURE;
  }
  for (const xAOD::MuonSegment *muonSeg : *muonSegments) 
    if (muonSeg->chamberIndex()==7 || muonSeg->chamberIndex()==8) {
      getHist("MuonSegments_MDT_EIS_EIL_t0")->Fill(muonSeg->t0());
      getHist("MuonSegments_MDT_EIS_EIL_t0error")->Fill(muonSeg->t0error());
      getHist("MuonSegments_MDT_EIS_EIL_sector")->Fill(muonSeg->sector());
      getHist("MuonSegments_MDT_EIS_EIL_nPrecisionHits")->Fill(muonSeg->nPrecisionHits());
      float thetaPos = TVector3(muonSeg->x(), muonSeg->y(), muonSeg->z()).Theta();
      float thetaDir = TVector3(muonSeg->px(), muonSeg->py(), muonSeg->pz()).Theta();
      float deltaTheta = TMath::Abs(thetaPos - thetaDir);
      getHist("MuonSegments_MDT_EIS_EIL_thetaPos")->Fill(thetaPos*180/Pi);
      getHist("MuonSegments_MDT_EIS_EIL_thetaDir")->Fill(thetaDir*180/Pi);
      getHist("MuonSegments_MDT_EIS_EIL_deltaTheta")->Fill(deltaTheta*180/Pi);
      getHist("MuonSegments_MDT_EIS_EIL_phi")->Fill(TVector3(muonSeg->x(), muonSeg->y(), muonSeg->z()).Phi());
    }

  const xAOD::MuonSegmentContainer* ncb_muonSegments = 0;
  if (evtStore()->retrieve(ncb_muonSegments,"NCB_MuonSegments").isFailure()) {
      ATH_MSG_INFO( "failed to retrieve the NCB_MuonSegments." );
      return StatusCode::FAILURE;
  }
  for (const xAOD::MuonSegment *ncb_muonSeg : *ncb_muonSegments)
    if (ncb_muonSeg->chamberIndex()==15 || ncb_muonSeg->chamberIndex()==16) {
      getHist("NCB_MuonSegments_CSC_t0")->Fill(ncb_muonSeg->t0());
      getHist("NCB_MuonSegments_CSC_t0error")->Fill(ncb_muonSeg->t0error());
      getHist("NCB_MuonSegments_CSC_sector")->Fill(ncb_muonSeg->sector());
      getHist("NCB_MuonSegments_CSC_nPrecisionHits")->Fill(ncb_muonSeg->nPrecisionHits());
      float thetaPos = TVector3(ncb_muonSeg->x(), ncb_muonSeg->y(), ncb_muonSeg->z()).Theta();
      float thetaDir = TVector3(ncb_muonSeg->px(), ncb_muonSeg->py(), ncb_muonSeg->pz()).Theta();
      float deltaTheta = TMath::Abs(thetaPos - thetaDir);
      getHist("NCB_MuonSegments_CSC_thetaPos")->Fill(thetaPos*180/Pi);
      getHist("NCB_MuonSegments_CSC_thetaDir")->Fill(thetaDir*180/Pi);
      getHist("NCB_MuonSegments_CSC_deltaTheta")->Fill(deltaTheta*180/Pi);
      getHist("NCB_MuonSegments_CSC_phi")->Fill(TVector3(ncb_muonSeg->x(), ncb_muonSeg->y(), ncb_muonSeg->z()).Phi());
    }

  return StatusCode::SUCCESS;
}

TH1F* HistTool::getHist(TString varname) {
  for (TH1F* h : m_hist)
    if (h->GetName() == varname+"_" + m_name)
      return h;
  ATH_MSG_FATAL("Histogram for variable " << varname << " not found.");
  return 0;
}
