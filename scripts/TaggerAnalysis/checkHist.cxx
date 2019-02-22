#include <iostream>
using namespace std;
#include <vector>
#include <string>
#include <algorithm>

#include <TSystemDirectory.h>
#include <TString.h>
#include <TList.h>
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TStyle.h>

int main(int argc, char** argv) {

  TString inDir="";
  TString hist="";
  TString pdfFile="";
  TString opts="";
  float xmin=0;
  float xmax=0;
  for (int i=1;i<argc;i++) {
    if (TString(argv[i])=="--inDir")
      inDir=argv[i+1];
    if (TString(argv[i])=="--hist")
      hist=argv[i+1];
    if (TString(argv[i])=="--pdfFile")
      pdfFile=argv[i+1];
    if (TString(argv[i])=="--xmin")
      xmin=TString(argv[i+1]).Atof();
    if (TString(argv[i])=="--xmax")
      xmax=TString(argv[i+1]).Atof();
    if (TString(argv[i])=="--opts")
      opts=TString(argv[i+1]); 
  }

  if (inDir==""||hist==""||pdfFile==""||opts=="") {
    cout<<"Bad syntax."<<endl;
    cout<<"inDir="<<inDir<<endl;
    cout<<"hist="<<hist<<endl;
    cout<<"pdfFile="<<pdfFile<<endl;
    cout<<"opts="<<opts<<endl;
    return 1;
  }

  TSystemDirectory dir("",inDir);
  TList* list = dir.GetListOfFiles();
  if (!list) { cout<<"list pointer is null"<<endl;return 1;}
  vector<string> filenameVec;
  for (int i=0;i<list->GetEntries();i++) {
    TString filename=list->At(i)->GetName();
    if (!filename.EndsWith(".HIST.root")) continue;
    filenameVec.push_back(filename.Data());
  }  
  std::sort(filenameVec.begin(),filenameVec.end());

  TCanvas canv;
  canv.Print(pdfFile+"(");
  gStyle->SetOptStat("");
  for (string filename : filenameVec) {
    
    cout<<filename<<endl;
    TFile* f = TFile::Open(inDir+"/"+TString(filename.data()));
    if (!f) {cout<<"file pointer is null"<<endl;return 1;}
    if (!f->IsOpen()) {cout<<"failed to open root file"<<endl;return 1;}
    TH1F* h=(TH1F*)f->Get(hist);
    if (!h) {cout<<"histogram pointer is null"<<endl;return 1;}
    h->SetTitle(filename.data());
    if (xmin!=0||xmax!=0)
      h->GetXaxis()->SetRangeUser(xmin,xmax);
    h->Draw(opts);
    canv.Print(pdfFile);
    f->Close();

  }
  canv.Clear();
  canv.Print(pdfFile+")");
  return 0;
}
