#include <iostream>
using namespace std;
#include <fstream>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TCut.h>
#include <TTreePlayer.h>
#include <TObjArray.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TDirectory.h>
#include <TStyle.h>

const TString plotsFile = "eventDisplay.pdf"; 

int getDecisionBit(TString method) {
  if (method=="SegmentACNoTime") return 8192;
  if (method=="OneSidedTight")   return 64;
  if (method=="TwoSided")        return 128;
  throw "getDecisionBit for " + method + " not implemented";
  return 0;
}

vector<TH2F*> eventDisplayPage1getMuonHistograms(
                                       TTree* tree,
                                       int entry,
                                       TString method,
                                       vector<TString> weight,
                                       TString muonSegmentCollection) {
  //cout << "eventDisplayPage1getMuonHistograms entry=" << entry 
  //     << "method=" << method
  //     << " " << muonSegmentCollection<<endl; 

  TCut cut_entry=Form("Entry$==%i",entry);
  int decisionBit = (method!="") ? getDecisionBit(method) : -1;

  TH2F* hseg[2];
  for (int side=0;side<2;side++) {
    TCut cut_seg = "";
    if (muonSegmentCollection == "NCB_MuonSegments") 
      cut_seg="(NCB_MuonSegments.chamberIndex==15||NCB_MuonSegments.chamberIndex==16)";
    else if (muonSegmentCollection == "MuonSegments")
      cut_seg="(MuonSegments.chamberIndex==7||MuonSegments.chamberIndex==8)";
    
    cut_seg += (side==0) ? muonSegmentCollection + ".z<0" :
                           muonSegmentCollection + ".z>0";
    TString tmp;
    if (method!="")
      cut_seg += "BeamBackgroundData.result" +
                 (tmp=muonSegmentCollection).ReplaceAll("MuonSegments","MuonSeg") +
                 Form("&%i", decisionBit);
    cut_seg += cut_entry;
    if (weight.size())
      cut_seg *= muonSegmentCollection + "." + weight[1];
    tree->Draw(Form("TMath::Sqrt(%s.x*%s.x+%s.y*%s.y):%s.phi"
                    ">>h%i%i(100,-3.1415,3.1415,200,0,4250)",
                   muonSegmentCollection.Data(),
                   muonSegmentCollection.Data(),
                   muonSegmentCollection.Data(),
                   muonSegmentCollection.Data(),
                   muonSegmentCollection.Data(),
                   side, 
                   muonSegmentCollection == "NCB_MuonSegments"), 
                cut_seg, "goff");

    hseg[side] = (TH2F*)gDirectory->Get(Form("h%i%i",side,muonSegmentCollection == "NCB_MuonSegments"));
    if (!hseg[side]) throw "hseg is null";
    for (int i=1;i<=hseg[side]->GetNbinsX();i++)
      for (int j=1;j<=hseg[side]->GetNbinsY();j++)
        if (hseg[side]->GetBinContent(i,j)==0)
          hseg[side]->SetBinContent(i,j,-1000);
    float zmin, zmax;
    if (weight.size()==0) {
      zmin=0;
      zmax=1;
    }
    else if (weight[1]=="t0") {
      zmin = -25;
      zmax=25;
    }
    else throw "unknown weight for the segments.";
    hseg[side]->SetMinimum(zmin);
    hseg[side]->SetMaximum(zmax);
  }
  vector<TH2F*> hres;
  hres.push_back(hseg[0]);
  hres.push_back(hseg[1]);
  return hres;
}

void makeEventDisplayPage1(TTree* tree, 
                           int entry, 
                           TString method, 
                           vector<TString> weight, 
                           TString plotsFile) {
  cout<<"makeEventDisplay entry="<<entry << " " << method << endl;

  int decisionBit = (method!="") ? getDecisionBit(method) : -1;
  bool showOnlyTagged = (method!="") ? true : false;

  TCut cut_entry=Form("Entry$==%i",entry);

  TCanvas canv("c","",1000,600);
  canv.Divide(3,2);

  vector<TH2F*> hCSC = eventDisplayPage1getMuonHistograms(tree, entry, method, weight,"NCB_MuonSegments");

  vector<TH2F*> hMDT = eventDisplayPage1getMuonHistograms(tree, entry, method, weight,"MuonSegments"); 

  canv.cd(1); hCSC[0]->Draw("colz");
  canv.cd(3); hCSC[1]->Draw("colz");
  canv.cd(4); hMDT[0]->Draw("colz");
  canv.cd(6); hMDT[1]->Draw("colz");

  canv.cd(2);
  TCut cut_cl = "TopoClusters.emscale_e>10000&&TopoClusters.rPerp>881";
  if (method=="OneSidedTight") 
    cut_cl+="TopoClusters.time<-5";
  if (showOnlyTagged) 
    cut_cl+=Form("BeamBackgroundData.resultClus&%i",decisionBit);
  cut_cl+=cut_entry;
  if (weight.size())
    cut_cl*="TopoClusters."+weight[0];
  tree->Draw("TopoClusters.rPerp:TopoClusters.phi>>h(100,-3.1415,3.1415,200,0,4250)", cut_cl, "goff");
  TH2F* h = (TH2F*)gDirectory->Get("h");
  if (!h) throw "makeEventDisplay: h is null";

  float zmin, zmax;
  if (weight.size()==0) {
    zmin=0; zmax=1;
  }
  else if (weight[0]=="time") {
    zmin=-25;
    zmax=25;
  }
  else if (weight[0]=="emscale_e") {
    zmin=0;
    zmax=100000;
  }
  else throw "unknown weight for the clusters";
  h->SetMinimum(zmin);
  h->SetMaximum(zmax);
  for (int i=1;i<=h->GetNbinsX();i++)
    for (int j=1;j<=h->GetNbinsY();j++)
      if (h->GetBinContent(i,j)==0)
        h->SetBinContent(i,j,-1000);
  h->Draw("colz");

  canv.Print(plotsFile);

  //delete h,hNCB_MuonSeg[0], hNCB_MuonSeg[1];

}

void makeEventDisplayPage2(TTree* tree, 
                           int entry, 
                           TString method,
                           TString plotsFile) {
  cout << "makeEventDisplayPage2 entry=" << entry << " " << method << endl;
  TCanvas canv("c","",1000,600);
  TCut cut_entry = Form("Entry$==%i",entry);
  TCut cut_cl = "TopoClusters.emscale_e>10000&&TopoClusters.rPerp>881";
  cut_cl+=cut_entry;
  if (method!="")
    cut_cl+=Form("BeamBackgroundData.resultClus&%i", getDecisionBit(method));

  tree->Draw("TopoClusters.time:TopoClusters.eta>>h(100,-5,5,50,-25,25)",cut_cl,"goff");
  TH2F* h = (TH2F*)gDirectory->Get("h");
  if (!h) throw "h is null";
  h->Draw("colz");
  canv.Print(plotsFile);
}

int main(int argc, char** argv) {

  TString ntupleFile = "";
  TString taggingMethod = "";
  int maxEvents = 1;
  bool onlyTaggedEvents = 0;
 
  for (int i=1;i<argc;i++) {
    if (TString(argv[i])=="--ntupleFile") ntupleFile=argv[i+1];
    if (TString(argv[i])=="--taggingMethod") taggingMethod=argv[i+1];
    if (TString(argv[i])=="--maxEvents") maxEvents=TString(argv[i+1]).Atoi();
    if (TString(argv[i])=="--onlyTaggedEvents") onlyTaggedEvents=true;
  }
  cout<<"ntupleFile="<<ntupleFile<<endl;
  cout<<"taggingMethod="<<taggingMethod<<endl;
  cout<<"maxEvents="<<maxEvents<<endl;

  TFile* f = TFile::Open(ntupleFile);
  if (!f) { cout << "failed to open ntuple file" << endl; return 1;}
  TTree* tree = (TTree*)f->Get("CollectionTree");
  if (!tree) { cout << "failed to read CollectionTree" << endl; return 1; }
  cout << "CollectionTree entries=" << tree->GetEntries() << endl;

  //const TCut cut_extra = "(AntiKt4EMTopoJets.pt[0]-AntiKt4EMTopoJets.pt[1])/((AntiKt4EMTopoJets.pt[0]+AntiKt4EMTopoJets.pt[1])/2)<0.1&&AntiKt4EMTopoJets.pt[0]>400000";
  TCut cut_tagged("BeamBackgroundData.num"+taggingMethod);
  cout << taggingMethod << taggingMethod 
       << " events=" << tree->GetEntries(cut_tagged) << endl;

  tree->SetScanField(0);
  ((TTreePlayer*)tree->GetPlayer())->SetScanRedirect(1);
  ((TTreePlayer*)tree->GetPlayer())->SetScanFileName("eventDisplay.CollectionTreeScan.txt");
  tree->Scan("Entry$", onlyTaggedEvents ? cut_tagged : "");

  cout << "reading CollectionTreeScan.txt" << endl;
  fstream fscan("CollectionTreeScan.txt");
  if (!fscan.is_open()) {cout<<"failed to open scan file"<< endl;return 1;}
  char linebuf[1000];
  vector<int> taggedEntries;
  while (fscan.getline(linebuf,1000)) {
    TString line(linebuf);
    if (line.Contains("**")) continue;
    if (line.Contains("Row")) continue; 
    taggedEntries.push_back(TString(line.Tokenize(" ")->At(3)->GetName()).Atoi());
  }
  fscan.close();
  cout << "  CollectionTree::Scan tagged entries: " << taggedEntries.size() << endl;
  if (taggedEntries.size()==0) {
    cout<<"no tagged events"<<endl; 
    return 1;
  }

  TCanvas canv;
  canv.Print(plotsFile+"(");
  gStyle->SetOptStat("");
  
  gStyle->SetPalette(1);

  try { 
  for (int i=0; i<taggedEntries.size() && i<maxEvents; i++) {
    int entry=taggedEntries[i];

    makeEventDisplayPage1(tree, entry, "", vector<TString>{},plotsFile);    
    makeEventDisplayPage1(tree, entry, taggingMethod, vector<TString>{}, plotsFile);

    //makeEventDisplayPage1(tree, entry, "", vector<TString>{"time","t0"},plotsFile);
    //makeEventDisplayPage1(tree, entry, taggingMethod, vector<TString>{"time","t0"}, plotsFile);

    //makeEventDisplayPage2(tree, entry, "", plotsFile);
    //makeEventDisplayPage2(tree, entry, taggingMethod, plotsFile);
  }
  } 
  catch (const char* err) {cout << err << endl; return 1;}
  catch (TString err) {cout<<err<<endl;return 1;}
  canv.Print(plotsFile+")"); 
 
  f->Close();

  return 0;
}
