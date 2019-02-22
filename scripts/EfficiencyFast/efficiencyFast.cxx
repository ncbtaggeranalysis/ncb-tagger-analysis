#include <iostream>
using namespace std;

#include <TFile.h>
#include <TTree.h>
#include <TCut.h>
#include <TH1F.h>
#include <TObjArray.h>
#include <TDirectory.h>
#include <TSystem.h>

#include "TaggerAnalysis/taggerAnalysisLib.h"

const TCut cut_jetCln_noise0 = "Sum$(AntiKt4EMTopoJets.hecf>0.5&&AntiKt4EMTopoJets.hecq>0.5&&AntiKt4EMTopoJets.AverageLArQF/65535>0.8&&AntiKt4EMTopoJets.pt>20000)";
const TCut cut_jetCln_noise1  ="Sum$(TMath::Abs(AntiKt4EMTopoJets.negE)>60000&&AntiKt4EMTopoJets.pt>20000)";
const TCut cut_jetCln_noise2 = "Sum$(AntiKt4EMTopoJets.emf>0.95&&AntiKt4EMTopoJets.larq>0.8&&AntiKt4EMTopoJets.AverageLArQF/65535>0.8&&TMath::Abs(AntiKt4EMTopoJets.eta)<2.8&&AntiKt4EMTopoJets.pt>20000)";
const TCut cut_jetCln_noise = cut_jetCln_noise0 || cut_jetCln_noise1 || cut_jetCln_noise2;

const vector<TString> tagging_methods = {
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

void addHist(TTree* tree, 
             TString var, 
             TString histdef, 
             TCut mainCut,
             TCut extraCut = "") {
  cout<<"addHist: " << var << " " << histdef << " " 
      << mainCut.GetTitle() << " " << mainCut.GetTitle() << endl;
  tree->Draw(var+">>h("+histdef+")",mainCut+extraCut,"goff");
  TH1F* h=(TH1F*)gDirectory->Get("h");
  if (!h) throw "addHist: histogram pointer is null";
  TString histname=var;
  if (TString(extraCut.GetTitle())!="") 
    histname+="_{"+TString(extraCut.GetTitle())+"}"; 
  histname.ReplaceAll("/","over");
  h->Write(histname);
  delete h;
}
 
int main(int argc, char** argv) {
  TString sample="";
  TString outDir="";
  TString sampleType="";
  bool doJets=false;
  for (int i=1;i<argc;i++) {
    if (TString(argv[i]) == "--inDS")
      sample=argv[i+1];
    if (TString(argv[i]) == "--outDir")
      outDir=argv[i+1];
    if (TString(argv[i]) == "--sampleType")
      sampleType=argv[i+1];
    if (TString(argv[i]) == "--doJets")
      doJets=TString(argv[i+1]).Atoi();
  }
  cout<<"sample="<<sample<<endl;
  cout<<"sampleType="<<sampleType<<endl;
  if (sample=="") { cout << "Bad syntax" << endl; return 1; }
  if (sampleType!="dijet"&&sampleType!="beamhalo") {cout<<"unknown sample type"<<endl; return 1; }

  bool isMC = sample.Contains("mc");
  bool isData = !isMC;

  try {

  TFile* f = TFile::Open(sample);
  if (!f) { cout<<"f is null"<<endl;return 1;}
  if (!f->IsOpen()) { cout<<"failed to open root file"<<endl;return 1;}
  TTree* tree = (TTree*)f->Get("CollectionTree");
  cout << "CollectionTree entries=" << tree->GetEntries() << endl;
  TH1F* h_cutflow = 0; 
  if (sampleType=="dijet") h_cutflow = (TH1F*)f->Get("cutflow_MisIdProb");

  TString outputHISTFile="";

  TString projectTag;
  int runNumber;
  TString period;
  TString stream;
  TaggerAnalysisLib::getDatasetInfo(sample, projectTag, runNumber, period, stream);
  outputHISTFile = outDir + "/" + projectTag + "." 
                                + TString::Format("%i",runNumber) 
                                + (isData ? "." + period + "." + stream : TString("")) 
                                + ".HIST.root";
  
  cout << outputHISTFile << endl;
 
  TFile* fhist = TFile::Open(outputHISTFile,"recreate");

  vector<TCut> cut_beamhaloVec;
  vector<TCut> cut_dijetVec = {""};
  vector<TCut> cut_sampleVec;

  vector<float> beamhaloPtCutVec={
    20,
   120,
   200
  }; // GeV

  for (float ptCut : beamhaloPtCutVec) 
    if (isData) 
      cut_beamhaloVec.push_back(Form("passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>%.0f&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0",ptCut*1000)+!cut_jetCln_noise);
    else 
      cut_beamhaloVec.push_back(Form("AntiKt4EMTopoJets.pt[0]>%.0f&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0",ptCut*1000));

  if (isData)
    cut_beamhaloVec.push_back("passGRL&&StableBeams&&(L1_J12_UNPAIRED_ISO||L1_J12_UNPAIRED_NONISO)&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0"+!cut_jetCln_noise);

  if (sampleType=="beamhalo")
    cut_sampleVec = cut_beamhaloVec;
  else if (sampleType=="dijet")
    cut_sampleVec = cut_dijetVec;
  else throw "unknown sample type: "+sampleType;

  int ntotal = 0;
  if (sampleType=="beamhalo")
    ntotal = tree->GetEntries();
  else 
    ntotal = h_cutflow->GetBinContent(1);

  for (TCut cut_sample : cut_sampleVec) {
    cout << "selection cut=" 
         << TString(cut_sample.GetTitle()).ReplaceAll(cut_jetCln_noise.GetTitle(),
                    "cut_jetCln_noise") 
         << endl;
    int nselected = tree->GetEntries(cut_sample);

    TH1F* h_ntagged = new TH1F("h_ntagged","",tagging_methods.size(),0,tagging_methods.size());
    int bin=1;
    for (TString method : tagging_methods) {
      TCut cut_tagged=""; 
      if (method == "NoTimeTight&&TwoSided")
        cut_tagged = "Sum$((BeamBackgroundData.resultClus&8)&&"
                          "(BeamBackgroundData.resultClus&128))";
      else
        cut_tagged = "BeamBackgroundData.num" +method;
      int ntagged = tree->GetEntries(cut_sample + cut_tagged);
      cout << Form("%-40s%10i%10i%10i",
                   method.Data(),
                   ntotal,
                   nselected,
                   ntagged) << endl;  
      h_ntagged->SetBinContent(bin, ntagged);
      h_ntagged->GetXaxis()->SetBinLabel(bin,method);
      bin++;
    }
    h_ntagged->SetEntries(nselected);

    TString dir=cut_sample.GetTitle();
    if (dir!="") {
      dir.ReplaceAll(cut_jetCln_noise.GetTitle(),"cut_jetCln_noise");
      dir.ReplaceAll("TMath::","");
    }
    else
      dir="noSelection";
    fhist->mkdir(dir);
    
    if (!fhist->cd(dir)) { cout << "failed to cd to << " << dir << endl; return 1;}
    h_ntagged->SetMinimum(0);
    h_ntagged->Write();

    delete h_ntagged;

    if (doJets) {
      addHist(tree,"AntiKt4EMTopoJets.n","50,0,50",cut_sample);
      addHist(tree,"AntiKt4EMTopoJets.pt[0]/1000","1000,0,10000",cut_sample);
      addHist(tree,"AntiKt4EMTopoJets.phi[0]:AntiKt4EMTopoJets.eta[0]",
                   "100,-5,5,100,-3.1415,3.1415",cut_sample);   
      addHist(tree,"AntiKt4EMTopoJets.time[0]:AntiKt4EMTopoJets.eta[0]",
                   "100,-5,5,200,-100,100",cut_sample);
      addHist(tree,"2*(AntiKt4EMTopoJets.pt[0]-AntiKt4EMTopoJets.pt[1])/"
                   "(AntiKt4EMTopoJets.pt[0]+AntiKt4EMTopoJets.pt[1])", 
                   "100,0,1",
                   cut_sample,
                   "AntiKt4EMTopoJets.n>=2");
      addHist(tree,"TMath::Abs(TVector2::Phi_mpi_pi(AntiKt4EMTopoJets.phi[0]-AntiKt4EMTopoJets.phi[1]))", 
                    "100, 0, 3.1415",
                    cut_sample,
                    "AntiKt4EMTopoJets.n>=2");

      addHist(tree,"AntiKt4EMTopoJets.emf[0]","100,-0.5,1.5",cut_sample);
      addHist(tree,"AntiKt4EMTopoJets.SumPtTrkPt500[0]/AntiKt4EMTopoJets.pt[0]",
                   "100,-0.5,1.5",cut_sample);

      addHist(tree,"AntiKt4EMTopoJets.hecf[0]","100,-0.5,1.5",cut_sample); 
      addHist(tree,"AntiKt4EMTopoJets.hecq[0]","100,-0.5,1.5",cut_sample);
      addHist(tree,"AntiKt4EMTopoJets.larq[0]","100,-0.5,1.5",cut_sample);    
      addHist(tree,"AntiKt4EMTopoJets.AverageLArQF[0]/65535","100,-0.5,1.5",cut_sample);
      addHist(tree,"AntiKt4EMTopoJets.negE[0]/1000","100,-200,100",cut_sample);
      addHist(tree,"AntiKt4EMTopoJets.fracSamplingMax[0]","100,-0.5,1.5",cut_sample);
      addHist(tree,"AntiKt4EMTopoJets.fracSamplingMaxIndex[0]","24,0,24",cut_sample);
      addHist(tree,"AntiKt4EMTopoJets.NumTrkPt1000[0]","100,0,100",cut_sample);
      addHist(tree,"AntiKt4EMTopoJets.SumPtTrkPt500[0]/1000","100,0,10000",cut_sample);
      addHist(tree,"AntiKt4EMTopoJets.isBadLoose[0]","2,0,2",cut_sample);
    }

    if (isData)
      addHist(tree,"StableBeams","2,0,2",cut_sample);
    addHist(tree, "NPV","100,0,100", cut_sample);
    addHist(tree, "averageInteractionsPerCrossing","100,0,100", cut_sample);

    if (sampleType=="dijet") {

      if (isData)
      addHist(tree, "BunchStructure.numBunchesColliding","2808,0,2808", cut_sample);

      addHist(tree,"Sum$(TMath::Abs(AntiKt4EMTopoJets.eta)<3.0)","50,0,50",cut_sample);


      addHist(tree, "Sum$(TopoClusters.emscale_e>10000)","200,0,200", cut_sample); 
        //addHist(tree,
        //"Sum$((BeamBackgroundData.resultClus&1)!=0)", "20,0,20", cut_sample);
        

      addHist(tree,
        "Sum$(MuonSegments.chamberIndex==7||MuonSegments.chamberIndex==8)",
        "100,0,100", cut_sample); 

      addHist(tree,
        "Sum$(MuonSegments.chamberIndex==15||MuonSegments.chamberIndex==16)",
        "100,0,100", cut_sample);

      addHist(tree,
        "Sum$(NCB_MuonSegments.chamberIndex==15||NCB_MuonSegments.chamberIndex==16)",
        "100,0,100", cut_sample);

      addHist(tree, 
        "MuonSegments.t0", "200,-100,100",
        cut_sample,
        "MuonSegments.chamberIndex==7||MuonSegments.chamberIndex==8"); 

      addHist(tree, "NCB_MuonSegments.t0", "200,-100,100", cut_sample);

      //addHist(tree,
      //  "Sum$(MuonSegments.chamberIndex==7||MuonSegments.chamberIndex==8)+"
      //  "Sum$(NCB_MuonSegments.chamberIndex==15||NCB_MuonSegments.chamberIndex==16)",
      //  "(100,0,100)", cut_sample);

      //addHist(tree,
      //  "Sum$((BeamBackgroundData.resultNCB_MuonSeg&1024)!=0)+"
      //  "Sum$((BeamBackgroundData.resultMuonSeg&1024)!=0)",
      //  "(50,0,50)", cut_sample);
    }

    addHist(tree, "beamIntensity_B1","5000,0,5000", cut_sample);
    addHist(tree, "beamIntensity_B2","5000,0,5000", cut_sample);
    addHist(tree, "0.5*(beamIntensity_B1+beamIntensity_B2)","5000,0,5000", cut_sample);

  }

  fhist->Close();

  }
  catch (const char* err) { cout <<err<<endl; return 1;}
  catch (TString err) { cout<<err<<endl; return 1;}

  return 0;
}
