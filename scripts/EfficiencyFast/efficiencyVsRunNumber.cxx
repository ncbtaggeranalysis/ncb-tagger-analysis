#include <iostream>
using namespace std;

#include <TFile.h>
#include <TH1F.h>
#include <TObjArray.h>
#include <TMath.h>
#include <TEfficiency.h>

#include "taggerAnalysisLib.h"

const vector<TString> configVec={
//  "(passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>20000&&(AntiKt4EMTopoJets.eta[0])<3.0)&&(!(cut_jetCln_noise))",

  "(passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&(AntiKt4EMTopoJets.eta[0])<3.0)&&(!(cut_jetCln_noise))"
};

vector<TString> getTaggerMethods(TString rootfile, TString config) {
  vector<TString> methods;
  TFile* f = TFile::Open(rootfile);
  if (!f) throw "getTaggerMethods: file pointer is null";
  if (!f->IsOpen()) throw "getTaggerMethods: faild to open file";
  TH1F* h = (TH1F*)f->Get(config+"/h_ntagged");
  if (!h) throw "getTaggerMethods: failed to read histogram: " + config+"/h_ntagged";
  for (int bin=1;bin<=h->GetNbinsX();bin++)
    methods.push_back(h->GetXaxis()->GetBinLabel(bin));
  return methods;
}

TH1F* getHistogramVsRunNumber(TString method, int index, vector<TString> histFiles, TString config) {
  cout << endl << "getHistogramVsRunNumber: " << method << endl << config << endl;
  TH1F* hRunNum = new TH1F("h_"+method,"",histFiles.size(),0,histFiles.size());
  int irun = 0;
  for (TString histFile : histFiles) {
    TFile* f = TFile::Open(histFile);
    if (!f) throw "getTaggerMethods: file pointer is null";
    if (!f->IsOpen()) throw "getTaggerMethods: faild to open file";
    TH1F* h = (TH1F*)f->Get(config+"/h_ntagged");
    if (!h) throw "getTaggerMethods: histogram pointer is null";

    int ntagged=h->GetBinContent(index+1);
    int nbeamhalo=h->GetEntries(); 
    float eff = 0;
    float statErr = 0;
    if (nbeamhalo) {
      eff=(float)ntagged/nbeamhalo;

      float statErrorUp=TEfficiency::Normal(nbeamhalo,ntagged,0.683,true)-eff;
      float statErrorDown=eff-TEfficiency::Normal(nbeamhalo,ntagged,0.683,false);
      statErr = 0.5*(statErrorUp+statErrorDown);

    }

    cout << method << " "  
         << Form("%-70s%10i%10i%10.2f%10.2f", histFile.Data(), nbeamhalo, ntagged, 100*eff,100*statErr) << endl;

    hRunNum->SetBinContent(irun+1, eff);
    hRunNum->SetBinError(irun+1, statErr);

    TObjArray* objArray = histFile.Tokenize("."); 
    TString binlabel=objArray->At(1)->GetName();
    if (binlabel.BeginsWith("period")) binlabel.ReplaceAll("period","");
    hRunNum->GetXaxis()->SetBinLabel(irun+1,binlabel);
    //hRunNum->GetXaxis()->LabelsOption("v");
    f->Close();
    irun++;
  }
  return hRunNum;
}

int main(int argc, char** argv) {

  TString histDir="";
  TString allDataFile="";
  for (int i=1;i<argc;i++) {
    if (TString(argv[i]) == "--histDir")
      histDir=argv[i+1];
    if (TString(argv[i]) == "--allDataFile")
      allDataFile=argv[i+1];
  }

  if (histDir==""||allDataFile=="") {cout<<"Bad syntax"<<endl;return 1;}

  try {

    vector<TString> histFiles = getListOfFilesInDirectory(histDir, ".root");
    histFiles.push_back(allDataFile);

    vector<TString> taggerMethods = getTaggerMethods(histFiles[0], configVec[0]);

    int iconfig=0;
    for (TString config : configVec) {

      vector<TH1F*> histVec;
      for (int i=0;i<taggerMethods.size();i++) 
        histVec.push_back( getHistogramVsRunNumber(taggerMethods[i],i, histFiles, config) );

      TFile* fout= TFile::Open("efficiencyVsRunNumber.root","update");
      cout <<"Writing histograms:"<<endl;
      TString dir=configVec[iconfig];
      iconfig++;
      fout->mkdir(dir);
      if (!fout->cd(dir)) throw "failed to cd";
      for (TH1F* h : histVec) {
        cout<<h->GetName()<<endl;
        h->SetMaximum(1);
        h->Write();
        delete h;
      }
      fout->Close();
    }
  } 
  catch (const char* err) { cout<<err<<endl;}
  catch (const TString err) { cout<<err<<endl;}

  return 0;
}
