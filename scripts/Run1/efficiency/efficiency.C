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
#include <TSystem.h>
#include <TDirectory.h>

#include "grlTool.h"

const vector<TString> BeamBackgroundBranches = {  
  "bb",
  "bbMoore"
};

const vector<TString> branchList={
  "RunNumber", "lbn",
  "L1_J10_UNPAIRED_ISO", "L1_J10_UNPAIRED_NONISO",
  "larError", "tileError",
  "vx_type",
  "AntiKt4TopoEM_pt",
  "AntiKt4TopoEM_eta",
  "AntiKt4TopoEM_hecf",
  "AntiKt4TopoEM_HECQuality",
  "AntiKt4TopoEM_LArQuality",
  "AntiKt4TopoEM_NegativeE",
  "bb_numSegment",
  "bb_numSegmentEarly",
  "bb_numSegmentACNoTime",
  "bb_numSegmentAC",
  "bb_numMatched",
  "bb_numNoTimeLoose",
  "bb_numNoTimeMedium",
  "bb_numNoTimeTight",
  "bb_numOneSidedLoose",
  "bb_numOneSidedMedium",
  "bb_numOneSidedTight",
  "bb_numTwoSidedNoTime",
  "bb_numTwoSided",
  "bb_numClusterShape"
};

GrlTool grlTool;
const TCut cut_grl = "passesRunLB(RunNumber,lbn)";
bool passesRunLB(int RunNumber, int lbn) {
  return grlTool.passesRunLB(RunNumber, lbn);
}

const TCut cut_unpaired= "L1_J10_UNPAIRED_ISO||L1_J10_UNPAIRED_NONISO";
const TCut cut_npv = "Sum$(vx_type)==0";
const TCut cut_detflags = "larError==0&&tileError==0";

const TCut cut_noise0="Sum$(AntiKt4TopoEM_pt>20000&&AntiKt4TopoEM_hecf>0.5&&TMath::Abs(AntiKt4TopoEM_HECQuality)>0.5)";
const TCut cut_noise1="Sum$(AntiKt4TopoEM_pt>20000&&TMath::Abs(AntiKt4TopoEM_NegativeE)>60000)";
const TCut cut_noise2="Sum$(AntiKt4TopoEM_pt>20000&&AntiKt4TopoEM_hecf>1-TMath::Abs(AntiKt4TopoEM_HECQuality))";
const TCut cut_noise3="Sum$(AntiKt4TopoEM_pt>20000&&AntiKt4TopoEM_emfrac>0.9&&TMath::Abs(AntiKt4TopoEM_LArQuality)>0.8&&TMath::Abs(AntiKt4TopoEM_eta)<2.8)";
const TCut cut_noise4="Sum$(AntiKt4TopoEM_pt>20000&&AntiKt4TopoEM_emfrac<0.1)";

const TCut cut_noise = cut_noise0 || cut_noise1 || cut_noise2 || cut_noise3 || cut_noise4;

const TCut cut_jet0= "AntiKt4TopoEM_pt[0]>120000&&TMath::Abs(AntiKt4TopoEM_eta[0])<3.0";

const TCut cut_beamhalo = cut_grl+cut_unpaired+cut_npv+cut_detflags+!cut_noise+cut_jet0; 

void addHist(TChain& chain, TString var, TString histdef, TCut cut) {
  cout<<"addHist: " << var<<endl;
  chain.Draw(var+">>h" + histdef,cut,"goff");
  TH1F* h = (TH1F*)gDirectory->Get("h");
  if (!h) throw "addHist: histogram pointer is null for : " + var;
  h->Write("h_"+var.ReplaceAll("TMath::","").ReplaceAll("$","D"));
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

  for (TString branchName : branchList)
    if (!tree->GetBranch(branchName))
      throw "branch " + branchName + " does not exist";

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

int efficiency() {

  TString ntupledir = gSystem->Getenv("sampleDir");
  TString inputTxt = gSystem->Getenv("inputTxt");
  TString outputHistFile = "HIST.root";
  if (gSystem->Getenv("outputHistFile")) 
    outputHistFile = gSystem->Getenv("outputHistFile");
  int makeCutflow = 1;
  if (TString(gSystem->Getenv("makeCutflowHist"))!="")
    makeCutflow = TString(gSystem->Getenv("makeCutflowHist")).Atoi();

  try {

  cout << "ntupledir="<<ntupledir << endl;
  cout << "inputTxt="<<inputTxt << endl;

  gROOT->LoadMacro("grlTool.cxx++");

  vector<TString> grlVec={
    "data11_7TeV.periodAllYear_DetStatus-v36-pro10-02_CoolRunQuery-00-04-08_All_Good.xml",
    "data12_8TeV.periodAllYear_HEAD_DQDefects-00-01-00_PHYS_StandardGRL_All_Good.xml"
  };
  grlTool.Load(grlVec);

  TChain chain("background");
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
        if (runNumber!=runNumberOld) throw "ntuple samples with different run numbers found.";
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
    h_cutflow=new TH1F("h_cutflow","",7,0,7);
    vector<TString> labels={"No selection","GRL","Unpaired","NPV","detector flags","noise","jet0 pT and eta"};
    for (unsigned int i=1;i<=labels.size();i++)
      h_cutflow->GetXaxis()->SetBinLabel(i,labels[i-1]);
    vector<float> e;
    e.assign(labels.size(), 0);
    e[0]=chain.GetEntries();
    e[1]=chain.GetEntries(cut_grl);
    e[2]=chain.GetEntries(cut_grl+cut_unpaired);
    e[3]=chain.GetEntries(cut_grl+cut_unpaired+cut_npv);
    e[4]=chain.GetEntries(cut_grl+cut_unpaired+cut_npv+cut_detflags);
    e[5]=chain.GetEntries(cut_grl+cut_unpaired+cut_npv+cut_detflags+!cut_noise);
    e[6]=chain.GetEntries(cut_grl+cut_unpaired+cut_npv+cut_detflags+!cut_noise+cut_jet0);
    for (unsigned int i=1;i<=labels.size();i++) {
      cout << Form("%-20s %10.0f",labels[i-1].Data(),e[i-1]) << endl;
      h_cutflow->SetBinContent(i,e[i-1]);
    }
  }

  //*Br  548 :bb_numSegment : Int_t                                              *
  //*Br  549 :bb_numSegmentEarly : Int_t                                         *
  //*Br  550 :bb_numSegmentACNoTime : Int_t                                      *
  //*Br  551 :bb_numSegmentAC : Int_t                                            *
  //*Br  552 :bb_numMatched : Int_t                                              *
  //*Br  553 :bb_numNoTimeLoose : Int_t                                          *
  //*Br  554 :bb_numNoTimeMedium : Int_t                                         *
  //*Br  555 :bb_numNoTimeTight : Int_t                                          *
  //*Br  556 :bb_numOneSidedLoose : Int_t                                        *
  //*Br  557 :bb_numOneSidedMedium : Int_t                                       *
  //*Br  558 :bb_numOneSidedTight : Int_t                                        *
  //*Br  559 :bb_numTwoSidedNoTime : Int_t                                       *
  //*Br  560 :bb_numTwoSided : Int_t                                             *
  //*Br  561 :bb_numClusterShape : Int_t                                         *

  //*Br  586 :bbMoore_numSegment : Int_t                                         *
  //*Br  587 :bbMoore_numSegmentEarly : Int_t                                    *
  //*Br  588 :bbMoore_numSegmentACNoTime : Int_t                                 *
  //*Br  589 :bbMoore_numSegmentAC : Int_t                                       *
  //*Br  590 :bbMoore_numMatched : Int_t                                         *
  //*Br  591 :bbMoore_numNoTimeLoose : Int_t                                     *
  //*Br  592 :bbMoore_numNoTimeMedium : Int_t                                    *
  //*Br  593 :bbMoore_numNoTimeTight : Int_t                                     *
  //*Br  594 :bbMoore_numOneSidedLoose : Int_t                                   *
  //*Br  595 :bbMoore_numOneSidedMedium : Int_t                                  *
  //*Br  596 :bbMoore_numOneSidedTight : Int_t                                   *
  //*Br  597 :bbMoore_numTwoSidedNoTime : Int_t                                  *
  //*Br  598 :bbMoore_numTwoSided : Int_t                                        *
  //*Br  599 :bbMoore_numClusterShape : Int_t                                    *

  vector<TString> tagger_methods = {
    "Segment","SegmentEarly","SegmentACNoTime","SegmentAC",
    "Matched",
    "NoTimeLoose","NoTimeMedium","NoTimeTight",
    "OneSidedLoose","OneSidedMedium","OneSidedTight",
    "TwoSidedNoTime","TwoSided"
  };

  int nbeamhalo = chain.GetEntries(cut_beamhalo);
  cout<<"beamhalo sample:" << nbeamhalo <<endl;

  vector<TH1F*> h_ntaggedVec;
  for (TString BeamBackgroundBranch : BeamBackgroundBranches) {
    cout << "BeamBackgroundBranch: " << BeamBackgroundBranch << endl;
    TH1F* h_ntagged = new TH1F("h_ntagged_"+BeamBackgroundBranch,"",
                                 tagger_methods.size(),0,tagger_methods.size());
    h_ntaggedVec.push_back(h_ntagged); 
    int imethod=0;
    for (TString method : tagger_methods) {
      int ntagged = chain.GetEntries(TCut(BeamBackgroundBranch+"_num"+method) + cut_beamhalo);
      cout<<method<<" : " << ntagged <<endl;
      h_ntagged->SetBinContent(imethod+1,ntagged);
      h_ntagged->GetXaxis()->SetBinLabel(imethod+1,method);
      imethod++;
    }
  }

  TFile* fhist = TFile ::Open(Form("%s.%i.HIST.root",project_tag.Data(),runNumber), "recreate");
  if (!fhist) throw "failed to create HIST file";
  if (!fhist->IsOpen()) throw "failed to open HIST file";

  if (makeCutflow) {
    h_cutflow->SetMinimum(0);
    h_cutflow->Write();
  }

  addHist(chain,"RunNumber","(150000,150000,300000)","");
  addHist(chain,"lbn","(10000,0,10000)",cut_beamhalo);
  addHist(chain,"Sum$(vx_type)","(10,0,10)",cut_beamhalo);
  addHist(chain,"larError","(3,0,3)",cut_beamhalo);
  addHist(chain,"tileError","(3,0,3)",cut_beamhalo);
  addHist(chain,"(L1_J10_UNPAIRED_ISO||L1_J10_UNPAIRED_NONISO)","(2,0,2)",cut_beamhalo);
  addHist(chain,"AntiKt4TopoEM_pt[0]/1000","(500,0,500)",cut_beamhalo);
  addHist(chain,"AntiKt4TopoEM_eta[0]","(100,-5,5)",cut_beamhalo);
  addHist(chain,"AntiKt4TopoEM_Timing[0]","(100,-25,25)",cut_beamhalo);
  addHist(chain,"AntiKt4TopoEM_Timing[0]:AntiKt4TopoEM_eta[0]","(100,-5,5,100,-25,25)",cut_beamhalo);

  /*
  for (TString BeamBackgroundBranch : BeamBackgroundBranches) {
    addHist(chain,BeamBackgroundBranch+"_cl_n","(100,0,100)",cut_beamhalo);
    addHist(chain,BeamBackgroundBranch+"_cl_drdz[0]","(100,0,5)",cut_beamhalo);
    addHist(chain,BeamBackgroundBranch+"_cl_pt[0]/1000","(200,0,200)",cut_beamhalo);
    addHist(chain,BeamBackgroundBranch+"_cl_eta[0]","(100,-5,5)",cut_beamhalo);
    addHist(chain,BeamBackgroundBranch+"_cl_phi[0]","(100,-3.1415,3.1415)",cut_beamhalo); 
    addHist(chain,BeamBackgroundBranch+"_cl_time[0]:"+
                  BeamBackgroundBranch+"_cl_eta[0]",
                  "(100,-5,5,100,-25,25)",cut_beamhalo);
    addHist(chain,"("+BeamBackgroundBranch+"_cl_E_em[0]+"+
                      BeamBackgroundBranch+"_cl_E_had[0])/1000",
                      "(200,0,200)",cut_beamhalo);
  }
  */

  /*
  addHist(chain, cut_noise0.GetTitle(),"(2,0,2)",cut_beamhalo);
  addHist(chain, cut_noise1.GetTitle(),"(2,0,2)",cut_beamhalo);
  addHist(chain, cut_noise2.GetTitle(),"(2,0,2)",cut_beamhalo);
  addHist(chain, cut_noise3.GetTitle(),"(2,0,2)",cut_beamhalo);
  addHist(chain, cut_noise4.GetTitle(),"(2,0,2)",cut_beamhalo);
  */

  const vector<TString> muonsegCollections = {
    "mooreseg"
    //"mboyseg",
    //"mgseg"
  };

  for (TString muonsegColl : muonsegCollections) {
    TCut cut_MdtInnerEndcap = Form("%s_stationName==15||%s_stationName==24",muonsegColl.Data(),muonsegColl.Data());
    TCut cut_CSC = Form("%s_stationName==33||%s_stationName==34",muonsegColl.Data(),muonsegColl.Data());

    vector<TCut> cut_muondetectors = {cut_MdtInnerEndcap, cut_CSC};

    for (TCut cut_muondet : cut_muondetectors) {

      addHist(chain,muonsegColl+"_n","(100,0,100)",cut_beamhalo);
      addHist(chain,Form("Sum$(%s)",cut_muondet.GetTitle()),"(50,0,50)",cut_beamhalo);
      addHist(chain,muonsegColl+"_x","(1000,-10000,10000)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_y","(1000,-10000,10000)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_y:"+muonsegColl+"_x",
                  "(1000,-10000,10000,1000,-10000,10000)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_z","(1000,-16000,16000)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_phi","(100,-3.1415,3.1415)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_theta","(100,0,6.283)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_sector","(16,0,16)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_stationEta","(100,-5,5)",cut_beamhalo+cut_muondet); 
      addHist(chain,muonsegColl+"_isEndcap","(2,0,2)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_stationName","(36,0,36)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_author","(32,0,32)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_chi2","(100,0,100)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_ndof","(100,0,100)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_t0","(200,-100,100)",cut_beamhalo+cut_muondet);
      addHist(chain,muonsegColl+"_t0err","(100,0,100)",cut_beamhalo+cut_muondet);
      
    }

  }

  for (TH1F* h_ntagged : h_ntaggedVec) {
    h_ntagged->SetEntries(nbeamhalo);
    h_ntagged->SetMinimum(0);
    h_ntagged->Write();
  }
  fhist->Close();
 
  }
  catch (const char* err) {cout<<err<<endl; return 1;}
  catch (TString err) { cout << err << endl; return 1;}

  return 0;
}
