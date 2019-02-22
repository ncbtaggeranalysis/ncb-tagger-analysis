#include <vector>
#include <iostream>
using namespace std;

#include <TString.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TCut.h>
#include <TFile.h>
#include <TTree.h>

int main() {

  const TString plotsFile="plotsFast.pdf";

  TChain chain("background");
  chain.Add("$STORAGE/data11_7TeV.00191933.physics_Background.merge.NTUP_BKGD.f414_p1196/*.root*");
  cout<<"entries="<<chain.GetEntries()<<endl;

  const vector<TString> vars={
    "mooreseg_n",
    "mooreseg_x",
    "mooreseg_y",
    "mooreseg_z",
    "mooreseg_phi",
    "mooreseg_theta",
    "mooreseg_locX", 
    "mooreseg_locY", 
    "mooreseg_locAngleXZ", 
    "mooreseg_locAngleYZ", 
    "mooreseg_sector", 
    "mooreseg_stationEta", 
    "mooreseg_isEndcap", 
    "mooreseg_stationName", 
    "mooreseg_author", 
    "mooreseg_chi2", 
    "mooreseg_ndof", 
    "mooreseg_t0",
    "mooreseg_t0err"

/*
    "mboyseg_n", 
    "mboyseg_x",
    "mboyseg_y", 
    "mboyseg_z", 
    "mboyseg_phi", 
    "mboyseg_theta", 
    "mboyseg_locX", 
    "mboyseg_locY", 
    "mboyseg_locAngleXZ", 
    "mboyseg_locAngleYZ", 
    "mboyseg_sector", 
    "mboyseg_stationEta", 
    "mboyseg_isEndcap",
    "mboyseg_stationName", 
    "mboyseg_author", 
    "mboyseg_chi2", 
    "mboyseg_ndof", 
    "mboyseg_t0", 
    "mboyseg_t0err",

    "mgseg_n",
    "mgseg_x",   
    "mgseg_y",   
    "mgseg_z",   
    "mgseg_phi", 
    "mgseg_theta", 
    "mgseg_locX", 
    "mgseg_locY", 
    "mgseg_locAngleXZ", 
    "mgseg_locAngleYZ", 
    "mgseg_sector", 
    "mgseg_stationEta", 
    "mgseg_isEndcap", 
    "mgseg_stationName", 
    "mgseg_author", 
    "mgseg_chi2", 
    "mgseg_ndof", 
    "mgseg_t0",  
    "mgseg_t0err",

    "AntiKt4TopoEM_pt",
    "AntiKt4TopoEM_phi"
*/

  };

  const TCut cut_CSC = "mooreseg_stationName==33||mooreseg_stationName==34";
  const TCut cut_unpaired="L1_J10_UNPAIRED_ISO||L1_J10_UNPAIRED_NONISO"; 
  const TCut cut_jets="AntiKt4TopoEM_pt[0]>20000";
  const TCut cut = cut_unpaired;

  TCanvas canv;
  canv.Print(plotsFile+"(");

/*
  for (TString var : vars) {
    cout<<var<<endl;
    chain.Draw(var + ">>h",cut,"goff");
    TH1F* h = (TH1F*)gDirectory->Get("h");

    h->Draw(); 
    canv.Print(plotsFile); 
    delete h;
  } 
*/

/*
  chain.Draw("mooreseg_y:mooreseg_x>>h",cut,"goff");
  TH1F* h = (TH1F*)gDirectory->Get("h");
  h->Draw("colz");
  ccanv.Print(plotsFile);
  delete h; 
*/ 
/*
  for (int stationName=0;stationName<=36;stationName++) {
    cout<<"stationName="<<stationName<<endl;
    chain.Draw("TMath::Sqrt(mooreseg_x*mooreseg_x+mooreseg_y*mooreseg_y):"
               "mooreseg_z>>h(200,-16000,16000,200,0,12000)", 
                Form("mooreseg_stationName==%i",stationName),
               "goff");
    TH2F* h = (TH2F*)gDirectory->Get("h");
    h->Draw("colz");
    canv.Print(plotsFile);
    delete h; 
  }
*/

/*
  vector<int> stationName8m={15,24,33,34};
  // 33,34 : CSC
  // 15,24 : MDT Inner end-cap
  for (int i : stationName8m) {
    chain.Draw("TMath::Sqrt(mooreseg_x*mooreseg_x+mooreseg_y*mooreseg_y)>>h", Form("mooreseg_stationName==%i",i),"goff");
    TH1F* h = (TH1F*)gDirectory->Get("h");
    h->Draw();
    canv.Print(plotsFile);
    delete h;
  }
*/

  canv.Clear();
  canv.Print(plotsFile+")");

  return 0;
}
