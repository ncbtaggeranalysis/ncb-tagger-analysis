#include <iostream>
using namespace std;
#include <stdio.h>

#include <TString.h>
#include <TFile.h>
#include <TH1F.h>
#include <TMath.h>

int main(int argc, char** argv) {

  TString histFile = "";
  TString effHist = "";
  TString systFile = "";

  for (int i=1;i<argc;i++) {
   if (TString(argv[i])=="--effHistFile") histFile=argv[i+1];
   if (TString(argv[i])=="--effHistName") effHist=argv[i+1];
   if (TString(argv[i])=="--systFile")    systFile=argv[i+1];
  }

  cout<<histFile<<endl;
  cout<<effHist<<endl;
  cout<<systFile<<endl;

  bool isMisIdProb = histFile.Contains("misIdProb") ? true : false;

  try {
    TFile* f = TFile::Open(histFile);
    if (!f) throw "failed to open " + histFile;

    TH1F* h_ntagged = (TH1F*)f->Get(effHist);
    if (!h_ntagged) {
      f->ls();
      throw "failed to read histogram: " + effHist;
    }
    //h_ntagged->Print("all");

    TH1F* h_cutflow = (TH1F*)f->Get("h_cutflow");
    if (!h_cutflow) 
      throw "failed to read h_cutflow";

    TFile* fSyst = TFile::Open(systFile);
    if (!fSyst) throw "failed to open systFile: " + systFile;
    TString segmentCollection="";
    if (effHist=="h_ntagged_bbMoore") segmentCollection="MooreSegments";
    else if (effHist=="h_ntagged_bb") segmentCollection="ConvertedMBoySegments";
    else throw "unknown effHist: " + effHist;

    int nsample=h_ntagged->GetEntries(); 
    cout << Form("Total events in the NTUP sample:%9.0f",h_cutflow->GetBinContent(1)) << endl;
    cout << Form("Events in the selected sample:  %9i",nsample) << endl;; 
    for (int i=1;i<=h_ntagged->GetNbinsX();i++) {
      TString method=h_ntagged->GetXaxis()->GetBinLabel(i);
      int ntagged=h_ntagged->GetBinContent(i);
      float eff = (float)ntagged/nsample; 
      float statErr= eff*TMath::Sqrt(1./ntagged + 1./nsample);

      TString effSystName = "h_eff_"+segmentCollection+"__"+method;
      TH1F* heffSyst = (TH1F*)fSyst->Get(effSystName);
      if (!heffSyst) 
        throw "failed to read efficiency histogram for the systemnatic error: "+effSystName;
      float systErr = heffSyst->GetRMS() / heffSyst->GetMean() ;

      cout << Form(isMisIdProb ? "%-15s %9i %15.6f%% %15.6f%% %15.2f%%" : 
                                 "%-15s %9i %15.2f%% %15.2f%% %15.2f%%",
                   method.Data(), 
                   ntagged,
                   100*eff, 100*statErr, 100*systErr) << endl;
    }      
 
    f->Close();
    fSyst->Close();
  }
  catch (const char* err) {cout<<err<<endl; }
  catch (TString err) { cout << err << endl; }

  return 0;
}
