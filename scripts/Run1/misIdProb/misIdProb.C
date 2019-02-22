#include <vector>
#include <iostream>
using namespace std;
#include <fstream>

#include <TString.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TCut.h>
#include <TH1F.h>
#include <TFile.h>
#include <TObjArray.h>
#include <TROOT.h>
#include <TVector2.h>
#include <TSystem.h>

#include "grlTool.h"

GrlTool grlTool;
const TCut cut_grl = "passesRunLB(RunNumber,lbn)";
bool passesRunLB(int RunNumber, int lbn) {
  return grlTool.passesRunLB(RunNumber, lbn);
}

void addHist(TChain& chain, TString var, TString histdef, TCut cut) {
  cout<<"addHist: " << var<<endl;
  chain.Draw(var+">>h" + histdef,cut,"goff");
  TH1F* h = (TH1F*)gDirectory->Get("h");
  if (!h) throw "addHist: histogram pointer is null for : " + var;
  h->Write("h_"+var.ReplaceAll("TMath::",""));
  delete h;
}

void getDatasetInfo(TString sample, TString& projectTag, int& runNumber) {
  cout << "getDatasetInfo: " << sample << endl;
  TString project_tag="";
  if (sample.Contains("data11_7TeV")) projectTag="data11_7TeV";
  else if (sample.Contains("data12_8TeV")) projectTag="data12_8TeV";
  else throw "getDatasetInfo: unknown project tag for sample: " + sample;

  TFile* f=TFile::Open(sample);
  TTree* tree = (TTree*)f->Get("background");
  if (!tree) tree = (TTree*)f->Get("physics");
  if (!tree) throw "getDatasetInfo: failed to read tree";

  tree->Draw("RunNumber>>h","","goff");

  TH1F* h=(TH1F*)gDirectory->Get("h");
  if (!h) throw "getDatasetInfo: histogram pointer is null";
  runNumber = (int)h->GetMean();
  float sigma = h->GetRMS();
  if (sigma > 1e-9)
    throw "getDatasetInfo: NTUP file contains data from more than one run";
  cout << "runNumber=" << runNumber << endl;
  delete h;
  f->Close();
}

int misIdProb(TString ntupleType) {

  TCut cut_npv="Sum$(vxp_type==1||vxp_type==3)";

  // HEC spikes
  TCut cut_cln0="Sum$(jet_AntiKt4TopoEM_pt>20000&&jet_AntiKt4TopoEM_hecf>0.5&&TMath::Abs(jet_AntiKt4TopoEM_HECQuality)>0.5)";
 
  TCut cut_cln1="Sum$(jet_AntiKt4TopoEM_pt>20000&&TMath::Abs(jet_AntiKt4TopoEM_NegativeE)>60000)";

  TCut cut_cln2="Sum$(jet_AntiKt4TopoEM_pt>20000&&jet_AntiKt4TopoEM_hecf>1-TMath::Abs(jet_AntiKt4TopoEM_HECQuality))";

  // Coherent EM noise
  TCut cut_cln3="Sum$(jet_AntiKt4TopoEM_pt>20000&&jet_AntiKt4TopoEM_emfrac>0.9&&TMath::Abs(jet_AntiKt4TopoEM_LArQuality)>0.8&&TMath::Abs(jet_AntiKt4TopoEM_eta)<2.8)";

  // Non-collision background
  TCut cut_cln4="Sum$(jet_AntiKt4TopoEM_pt>20000&&jet_AntiKt4TopoEM_fracSamplingMax>0.99&&TMath::Abs(jet_AntiKt4TopoEM_eta)<2)";

  TCut cut_cln5= ntupleType == "NTUP_COMMON" ? 
    "Sum$(jet_AntiKt4TopoEM_pt>20000&&jet_AntiKt4TopoEM_emfrac<0.05&&jet_AntiKt4TopoEM_sumPtTrk_pv0_500MeV/jet_AntiKt4TopoEM_pt<0.1&&TMath::Abs(jet_AntiKt4TopoEM_eta)<2)" : "";

  TCut cut_cln6="Sum$(jet_AntiKt4TopoEM_pt>20000&&jet_AntiKt4TopoEM_emfrac<0.05&&TMath::Abs(jet_AntiKt4TopoEM_eta)>=2)";

  TCut cut_cln7= ntupleType == "NTUP_COMMON" ?  
    "Sum$(jet_AntiKt4TopoEM_pt>20000&&jet_AntiKt4TopoEM_emfrac>0.95&&jet_AntiKt4TopoEM_sumPtTrk_pv0_500MeV/jet_AntiKt4TopoEM_pt<0.05&&TMath::Abs(jet_AntiKt4TopoEM_eta)<2)" : "";

  TCut cut_cln8="Sum$(jet_AntiKt4TopoEM_pt>20000&&TMath::Abs(jet_AntiKt4TopoEM_Timing)>10)";

  TCut cut_cln=cut_cln0||cut_cln1||cut_cln2||cut_cln3||cut_cln4||cut_cln5||cut_cln6||cut_cln7||cut_cln8;

  TCut cut_el = "Sum$(el_pt>20000&&el_medium&&TMath::Abs(el_eta)<2.47)";
  TCut cut_mu = "Sum$(mu_muid_pt>10000&&mu_muid_isCombinedMuon&&TMath::Abs(mu_muid_eta)<2.4)";

  TCut cut_njets = "jet_AntiKt4TopoEM_n>=2";
  TCut cut_jet0_pt_eta = "jet_AntiKt4TopoEM_pt[0]>120000&&TMath::Abs(jet_AntiKt4TopoEM_eta[0])<2";
  TCut cut_jet0_chf_emf = ntupleType=="NTUP_COMMON" ? 
    "jet_AntiKt4TopoEM_sumPtTrk_pv0_500MeV[0]/jet_AntiKt4TopoEM_pt[0]>0.02&&jet_AntiKt4TopoEM_emfrac[0]>0.1" :
    "jet_AntiKt4TopoEM_emfrac[0]>0.1";
  TCut cut_dijet_balance = "TMath::Abs(jet_AntiKt4TopoEM_pt[0]-jet_AntiKt4TopoEM_pt[1])/(jet_AntiKt4TopoEM_pt[0]+jet_AntiKt4TopoEM_pt[1])*2<0.4";
  TCut cut_dijet_deltaphi = "TVector2::Phi_mpi_pi(jet_AntiKt4TopoEM_phi[0]-jet_AntiKt4TopoEM_phi[1])>2.8";
 
  TString jets = "jet_AntiKt4TopoEM";

  if (ntupleType=="NTUP_BKGD") {
    jets = "AntiKt4TopoEM";

    cut_npv = TString(cut_npv.GetTitle()).ReplaceAll("vxp_type","vx_type");
    cut_cln = TString(cut_cln.GetTitle()).ReplaceAll("jet_","");
    cut_el = "";
    cut_mu = "";
    cut_njets = TString(cut_njets.GetTitle()).ReplaceAll("jet_","");
    cut_jet0_pt_eta = TString(cut_jet0_pt_eta.GetTitle()).ReplaceAll("jet_","");
    cut_jet0_chf_emf = TString(cut_jet0_chf_emf.GetTitle()).ReplaceAll("jet_","");
    cut_dijet_balance = TString(cut_dijet_balance.GetTitle()).ReplaceAll("jet_","");
    cut_dijet_deltaphi = TString(cut_dijet_deltaphi.GetTitle()).ReplaceAll("jet_","");
  }  

  try {

    TString ntupledir = gSystem->Getenv("sampleDir");
    TString inputTxt = gSystem->Getenv("inputTxt");
    TString outputHistFile = "HIST.root";
    if (gSystem->Getenv("outputHistFile"))
      outputHistFile = gSystem->Getenv("outputHistFile");
    int makeCutflow = 1;
    if (TString(gSystem->Getenv("makeCutflowHist"))!="")
      makeCutflow = TString(gSystem->Getenv("makeCutflowHist")).Atoi();

    gROOT->LoadMacro("grlTool.cxx++");

    vector<TString> grlVec={"data11_7TeV.periodAllYear_DetStatus-v36-pro10-02_CoolRunQuery-00-04-08_All_Good.xml"};
    grlTool.Load(grlVec);

    TString treeName="";
    if (ntupleType=="NTUP_COMMON") treeName="physics";
    else if (ntupleType=="NTUP_BKGD") treeName="background";
    else { cout<<"unknown ntuple type"<<endl;return 1;}

    TChain chain(treeName);
    TString project_tag="";
    int runNumber = 0;
    if (ntupledir != "")
      chain.Add(ntupledir+"/*.root*");
    else if (TString(inputTxt) != "") {
      cout << "reading sample list from txt file: " << endl;
      fstream ftxt(inputTxt, std::fstream::in);
      if (!ftxt.is_open()) throw "failed to open file:" + inputTxt;
      TString line;
      ftxt >> line;
      cout << "line=" << line <<endl;
      TObjArray* objArray = line.Tokenize(',');
      int runNumberOld = 0;
      for (int i=0;i<objArray->GetEntries();i++) {
        TString sample = objArray->At(i)->GetName();
        cout << "sample="<<sample << endl;
        runNumberOld = runNumber;
        getDatasetInfo(sample, project_tag, runNumber);
        cout << "runNumber=" << runNumber << endl;
        if (i!=0)
          if (runNumber!=runNumberOld) 
            throw "ntuple samples with different run numbers found.";
        chain.Add(sample);
      }
      TObjArray* fileArray = chain.GetListOfFiles();
      for (int i=0;i<fileArray->GetEntries();i++) {
        cout << "file #"<< i << " " << fileArray->At(i)->GetTitle() << endl;
        TFile* f = TFile::Open(fileArray->At(i)->GetTitle());
        if (!f) throw "Failed to open ntuple file.";
        f->Close();
      }
      ftxt.close();
    }
    else { cout << "must initialize either $sampleDir or $inputTxt"<<endl;return 1;}

    TH1F* h_cutflow = 0;
    if (makeCutflow) {
    vector<TString> labels={
      "Total",
      "GRL",
      "NPV",
      "Cleaning",
      "Electron veto",
      "Muon veto",
      ">=2 jets",
      "jet0 pt,eta",
      "jet0 chf,emf",
      "dijet balance",
      "dijet delta phi"
    };
    h_cutflow=new TH1F("h_cutflow","",labels.size(),0,labels.size());
    for (unsigned int i=1;i<=labels.size();i++)
      h_cutflow->GetXaxis()->SetBinLabel(i,labels[i-1]);
    vector<float> e;
    e.assign(labels.size(), 0);   
 
    e[0]=chain.GetEntries();
    e[1]=chain.GetEntries(cut_grl);
    e[2]=chain.GetEntries(cut_grl+cut_npv);
    e[3]=chain.GetEntries(cut_grl+cut_npv+!cut_cln);
    e[4]=chain.GetEntries(cut_grl+cut_npv+!cut_cln+!cut_el);
    e[5]=chain.GetEntries(cut_grl+cut_npv+!cut_cln+!cut_el+!cut_mu);
    e[6]=chain.GetEntries(cut_grl+cut_npv+!cut_cln+!cut_el+!cut_mu+cut_njets);
    e[7]=chain.GetEntries(cut_grl+cut_npv+!cut_cln+!cut_el+!cut_mu+cut_njets+cut_jet0_pt_eta);
    e[8]=chain.GetEntries(cut_grl+cut_npv+!cut_cln+!cut_el+!cut_mu+cut_njets+cut_jet0_pt_eta+cut_jet0_chf_emf);
    e[9]=chain.GetEntries(cut_grl+cut_npv+!cut_cln+!cut_el+!cut_mu+cut_njets+cut_jet0_pt_eta+cut_jet0_chf_emf+cut_dijet_balance);
    e[10]=chain.GetEntries(cut_grl+cut_npv+!cut_cln+!cut_el+!cut_mu+cut_njets+cut_jet0_pt_eta+cut_jet0_chf_emf+cut_dijet_balance+cut_dijet_deltaphi);

    for (unsigned int i=1;i<=labels.size();i++) {
      cout << Form("%-20s %10.0f",labels[i-1].Data(),e[i-1]) << endl;
      h_cutflow->SetBinContent(i,e[i-1]);
    }

    }

    TCut cut_dijet = cut_grl+cut_npv+!cut_cln+!cut_el+!cut_mu+cut_njets+cut_jet0_pt_eta+cut_jet0_chf_emf+cut_dijet_balance+cut_dijet_deltaphi;

    //*Br 2301 :bb_decision : Int_t                                                *
    //*Br 2302 :bb_numSegment : Int_t                                              *
    //*Br 2303 :bb_numSegmentEarly : Int_t                                         *
    //*Br 2304 :bb_numSegmentACNoTime : Int_t                                      *
    //*Br 2305 :bb_numSegmentAC : Int_t                                            *
    //*Br 2306 :bb_numMatched : Int_t                                              *
    //*Br 2307 :bb_numNoTimeLoose : Int_t                                          *
    //*Br 2308 :bb_numNoTimeMedium : Int_t                                         *
    //*Br 2309 :bb_numNoTimeTight : Int_t                                          *
    //*Br 2310 :bb_numOneSidedLoose : Int_t                                        *
    //*Br 2311 :bb_numOneSidedMedium : Int_t                                       *
    //*Br 2312 :bb_numOneSidedTight : Int_t                                        *
    //*Br 2313 :bb_numTwoSidedNoTime : Int_t                                       *
    //*Br 2314 :bb_numTwoSided : Int_t                                             *
    //*Br 2315 :bb_numClusterShape : Int_t                                         *

    const vector<TString> tagger_methods = {
      "Segment","SegmentEarly","SegmentACNoTime","SegmentAC",
      "NoTimeLoose","NoTimeMedium","NoTimeTight",
      "OneSidedLoose","OneSidedMedium","OneSidedTight",
      "TwoSidedNoTime","TwoSided"
    };

    int ndijet = chain.GetEntries(cut_dijet);
    cout<<"dijet sample:" << ndijet <<endl;
    TH1F* h_ntagged = new TH1F("h_ntagged","",tagger_methods.size(),0,tagger_methods.size());

    int imethod=0;
    for (TString method : tagger_methods) {
      int ntagged = chain.GetEntries(TCut("bb_num"+method) + cut_dijet);
      cout<<method<<" : " << ntagged <<endl;
      h_ntagged->SetBinContent(imethod+1,ntagged);
      h_ntagged->GetXaxis()->SetBinLabel(imethod+1,method);
      imethod++;
    }

    TFile* fhist = TFile ::Open(Form("%s.%i.HIST.root",project_tag.Data(),runNumber), "recreate");
    if (!fhist) throw "failed to create HIST file";
    if (!fhist->IsOpen()) throw "failed to open HIST file";

    if (makeCutflow) {
      h_cutflow->SetMinimum(0);
      h_cutflow->Write();
    }

    //addHist(chain,"RunNumber","(150000,150000,300000)","");
    //addHist(chain,"lbn","(10000,0,10000)",cut_dijet);
    addHist(chain,"averageIntPerXing","(100,0,100)",cut_dijet);
    //addHist(chain,"Sum$(vxp_type==1||vxp_type==3)","(20,0,20)",cut_dijet);
    addHist(chain,jets+"_pt[0]/1000","(100,0,500)",cut_dijet);
    addHist(chain,jets+"_eta[0]","(100,-5,5)",cut_dijet);
    //addHist(chain,"jet_AntiKt4TopoEM_sumPtTrk_pv0_500MeV[0]/jet_AntiKt4TopoEM_pt[0]","(150,0,1.5)",cut_dijet); 
    //addHist(chain,"jet_AntiKt4TopoEM_emfrac[0]","(150,0,1.5)",cut_dijet);
    //addHist(chain,"TMath::Abs(jet_AntiKt4TopoEM_pt[0]-jet_AntiKt4TopoEM_pt[1])/(jet_AntiKt4TopoEM_pt[0]+jet_AntiKt4TopoEM_pt[1])*2","(100,0,1)",cut_dijet);
    //addHist(chain,"TVector2::Phi_mpi_pi(jet_AntiKt4TopoEM_phi[0]-jet_AntiKt4TopoEM_phi[1])","(100,-3.1415,3.1415)",cut_dijet);

    //addHist(chain,"Sum$(musp_innerSegments)","(20,0,20)",cut_dijet);

    if (ntupleType=="NTUP_BKGD") {
      addHist(chain,"Sum$(mooreseg_stationName==15)","(10,0,10)",cut_dijet);
      addHist(chain,"Sum$(mooreseg_stationName==24)","(10,0,10)",cut_dijet);
      addHist(chain,"Sum$(mooreseg_stationName==33)","(10,0,10)",cut_dijet);
      addHist(chain,"Sum$(mooreseg_stationName==34)","(10,0,10)",cut_dijet);
    }

    h_ntagged->SetEntries(ndijet);
    h_ntagged->SetMinimum(0);
    h_ntagged->Write();
    fhist->Close();

  }

  catch (const char* err) {cout<<err<<endl; }
  catch (TString err) { cout << err << endl; } 

  return 0;

}
