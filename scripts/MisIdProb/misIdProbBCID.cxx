#include <iostream>
using namespace std;
#include <vector>

#include <TString.h>
#include <TFile.h>
#include <TEfficiency.h>
#include <TSystem.h>
#include <TObjArray.h>
#include <TH1F.h>
#include <TROOT.h>

#include "TaggerAnalysis/taggerAnalysisLib.h"

const TString taggerMethods[] = {
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
  "TwoSided"
};

class MisIdProbBCID {
  public:
         MisIdProbBCID(vector<TString> samples, TString method, TString outputROOTFile);
    void FillHistograms();
    void Write();

  private:
    vector<TString> m_samples;
    vector<TString> m_runNumberStr;
    TString m_method;
    TH1F* m_h_misIdProb_collidingAll;
    TH1F* m_h_misIdProb_collidingFirst;
    TString m_outputROOTFile;
};

MisIdProbBCID::MisIdProbBCID(vector<TString> samples, 
                             TString method, 
                             TString outputROOTFile) {
  for (TString sample: samples) {
    cout << sample << endl;
    TString basename = gSystem->BaseName(sample);
    TString runNumberStr = basename.Tokenize(".")->At(1)->GetName();
    runNumberStr = Form("%i",runNumberStr.Atoi());
    m_runNumberStr.push_back(runNumberStr);
  }
  m_method = method;
  m_outputROOTFile=outputROOTFile;
  m_samples=samples;
  m_h_misIdProb_collidingAll = new TH1F(method+"_misIdProb_CollidingAll","", samples.size(),0,samples.size());
  m_h_misIdProb_collidingFirst = new TH1F(method+"_misIdProb_CollidingFirst","", samples.size(),0,samples.size()); 
  for (int i=0;i<samples.size();i++) {
    m_h_misIdProb_collidingAll->GetXaxis()->SetBinLabel(i+1, m_runNumberStr[i]);
    m_h_misIdProb_collidingFirst->GetXaxis()->SetBinLabel(i+1, m_runNumberStr[i]);
  }
}

void MisIdProbBCID::FillHistograms() {
  cout<<"MisIdProbBCID::FillHistograms"<<endl;
  int bin=0;
  for (TString sample : m_samples) {
    cout<<sample<<endl;
    TFile* f = TFile::Open(sample);
    if (!f) throw "failed to read file: "+sample;

    TString objname="noSelection/eff_"+m_method;
    TEfficiency* eff_collidingAll = (TEfficiency*)f->Get(objname);
    if (!eff_collidingAll)
      throw "failed to read: "+objname;

    objname="noSelection/eff_"+m_method+"_collidingFirst";
    TEfficiency* eff_collidingFirst = (TEfficiency*)f->Get(objname);
    if (!eff_collidingFirst)
      throw "failed to read: "+objname;

    eff_collidingAll->GetEfficiency(1);
    eff_collidingFirst->GetEfficiency(1);
 
    m_h_misIdProb_collidingAll->SetBinContent(bin+1, eff_collidingAll->GetEfficiency(1));
    m_h_misIdProb_collidingFirst->SetBinContent(bin+1, eff_collidingFirst->GetEfficiency(1));

   m_h_misIdProb_collidingAll->SetBinError(bin+1,(eff_collidingAll->GetEfficiencyErrorUp(1)+eff_collidingAll->GetEfficiencyErrorLow(1))/2);
   m_h_misIdProb_collidingFirst->SetBinError(bin+1,(eff_collidingFirst->GetEfficiencyErrorUp(1)+eff_collidingFirst->GetEfficiencyErrorLow(1))/2);

   f->Close();
   bin++;
  }
}

void MisIdProbBCID::Write() {
  cout << "MisIdProbBCID::Write: " << m_outputROOTFile<<endl;
  TFile* f = TFile::Open(m_outputROOTFile,"update");
  m_h_misIdProb_collidingAll->SetMinimum(0);
  m_h_misIdProb_collidingFirst->SetMinimum(0);
  m_h_misIdProb_collidingAll->Write();
  m_h_misIdProb_collidingFirst->Write();
  f->Close();
}

int main(int argc, char** argv) {

  TString inputDir="";

  for (int i=1;i<argc;i++) {
    if (TString(argv[i])=="--inputDir")
      inputDir=argv[i+1];
  }
  if (inputDir=="") { cout<<"Syntax error."<<endl; return 1;}

  vector<TString> samples = getListOfFilesInDirectory(inputDir, ".HIST.root");
 
  try {
    gROOT->ProcessLine(".! rm misIdProbBCID.root");
    for (TString method : taggerMethods) {
      MisIdProbBCID analysis(samples, method, "misIdProbBCID.root");
      analysis.FillHistograms();
      analysis.Write();
    }
  }
  catch (const char* err) { cout<<err<<endl; return 1;}
  catch (TString err) { cout<<err<<endl; return 1;}

  return 0;
}
