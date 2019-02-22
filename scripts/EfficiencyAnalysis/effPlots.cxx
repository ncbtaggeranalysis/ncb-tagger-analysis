#include <iostream>
using namespace std;

#include <TEfficiency.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2F.h>
#include <TString.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TGraphAsymmErrors.h>
#include <TMath.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TObjArray.h>

#include "taggerAnalysisLib.h"

const vector<TString> tagging_methods = {
"OneSidedLoose",
"OneSidedMedium",
"OneSidedTight",
"TwoSided"
};

const vector<TString> configs={
  "passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0&&!cut_jetCln_noise",

"passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0&&AntiKt4EMTopoJets.larq[0]<0.05&&!cut_jetCln_noise",

"passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0&&AntiKt4EMTopoJets.larq[0]<0.05&&AntiKt4EMTopoJets.time[0]<0&&!cut_jetCln_noise",

"passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0&&AntiKt4EMTopoJets.larq[0]<0.05&&AntiKt4EMTopoJets.time[0]<-5&&!cut_jetCln_noise",

"passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&BunchStructure.isUnpairedB1&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0&&!cut_jetCln_noise",

"passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&BunchStructure.isUnpairedB2&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0&&!cut_jetCln_noise",

// noise cuts including emf<0.1 cut 
"passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0&&!cut_jetCln_noise_EMF"

};

const vector<pair<int,int>> calo_samplings = {
  {0,0}, {1,3}, {4,7},
  {8,11},
  {12,14}, {15,17}, {18,20},
  {21,23}
};

const TString plotsFile = "effPlots.pdf";
const float pi=TMath::Pi();

vector<int> getStyle(TString method) {
  if (method=="OneSidedLoose")       return vector<int>{46, 1, kFullCircle};
  else if (method=="OneSidedMedium") return vector<int>{ 6, 1, kFullCircle}; 
  else if (method=="OneSidedTight")  return vector<int>{ 2, 1, kFullCircle};
  else if (method=="TwoSided")       return vector<int>{ 4, 1, kFullSquare};
  else throw "unknown method: "+method;
  return vector<int>{0};
}

void addHist(TFile* f, TString histname, bool logy) {
  TH1* h=(TH1*)f->Get(histname);
  if (!h) throw "failed to read histogram:" + histname;
  if (histname.Contains("jet0_pt")) h->GetXaxis()->SetRangeUser(0,2000);
  h->Draw(histname.Contains("etatime") || histname.Contains("etaphi")  ? "colz" :"hist");
  h->GetXaxis()->SetTitle(histname.Remove(0,histname.Index("/")+1).ReplaceAll("h_","").ReplaceAll("_"," ").ReplaceAll("jet0","jet1"));
  h->SetMinimum(logy ? 0.5 : 0);
  gPad->SetLogy(logy);
}

void addEventSelectionLegendEntries(TLegend& leg, TString config) {
  TObjArray* cutsObjArray = config.Tokenize("&&");
  for (int i=0;i<cutsObjArray->GetEntries();i++) {
    TString label = cutsObjArray->At(i)->GetName();
    label.ReplaceAll("passGRL","GRL");
    label.ReplaceAll("StableBeams","Stable beams");
    label.ReplaceAll("NPV==0","No primary vertex");
    label.ReplaceAll("BunchStructure.isUnpairedIsolated","Unpaired isolated bunches");
    label.ReplaceAll("BunchStructure.isUnpairedB1","Unpaired bunches B1");
    label.ReplaceAll("BunchStructure.isUnpairedB2","Unpaired bunches B2");
    label.ReplaceAll("!cut_jetCln_noise_EMF","Official noise cleaning and emf < 0.1");
    label.ReplaceAll("!cut_jetCln_noise","Official noise cleaning");
    leg.AddEntry((TObject*)0, label, "");
  }
}

void addMonPlots(TFile* f, TString config, TString plotsFile) {
  TCanvas canv("canv","");
  canv.Divide(4,3);
  vector<pair<TString,bool>> hists = {
    {"h_jet0_pt",1},
    {"h_jet0_etaphi",0},
    {"h_jet0_etatime",0},
    {"h_jet0_emf",1},
    {"h_jet0_hecf",1},
    {"h_jet0_hecq",1},
    {"h_jet0_larq",1},
    {"h_jet0_AverageLArQF",1},
    {"h_jet0_negE",1},
    {"h_jet0_fracSamplingMax",1},
    {"h_jet0_fracSamplingMaxIndex",1}
    //{"h_jet0_SumPtTrkPt500",0}
  };

  for (int i=0;i<hists.size();i++) {
    canv.cd(i+1);
    addHist(f,config+"/"+hists[i].first,hists[i].second);
  }
  canv.cd(12);
  TLegend leg(.0,.0,.9,.95);
  addEventSelectionLegendEntries(leg, config);
  leg.Draw();
  canv.Print(plotsFile);
}

void addEffPlot(TFile* f, TString datalabel, TString config, TString var, TString xlabel, float eff_xmin, float eff_xmax,bool logy, TString plotsFile) {

  TCanvas canv("canv","",1000,500);

  canv.Clear();
  canv.SetWindowSize(1000,500);
  canv.Divide(3,1);

  TLegend leg1(.0,.6,.9,.95);
  leg1.AddEntry((TObject*)0,datalabel,"");

  TLegend leg2(.0,.0,.9,.55);
  addEventSelectionLegendEntries(leg2, config);
  canv.cd(3);
  leg2.SetY2(0.55);
  leg2.Draw();

  for (int i=0;i<tagging_methods.size();i++) {

    TString method=tagging_methods[i];
    
    TString name = config + "/eff_"+method+"_"+var;
    TEfficiency* eff = (TEfficiency*)f->Get(name);
    if (!eff) throw "failed to read: "+name;

    canv.cd(1);

    TH1F* htotal = (TH1F*)eff->GetTotalHistogram();
    htotal->SetTitle("");
    htotal->GetXaxis()->SetTitle(xlabel);
    htotal->SetLineColor(kBlack);
    htotal->Draw(i==0? "" : "same"); 
    htotal->SetMinimum(logy ? 0.5 : 0);
    htotal->SetLineWidth(2);
    if (i==0) leg1.AddEntry(htotal,Form("Total : %.0f",htotal->GetEntries()),"l");

    TH1F* hpassed = (TH1F*)eff->GetPassedHistogram();
    hpassed->SetLineColor(getStyle(method)[0]);
    hpassed->SetLineStyle(getStyle(method)[1]);
    hpassed->SetLineWidth(2);
    //hpassed->SetMarkerStyle(getStyle(method)[2]);
    //hpassed->SetMarkerColor(getStyle(method)[1]);
    //hpassed->SetMarkerSize(0.2);
    hpassed->Draw("same");
    gPad->SetLogy(logy);

    leg1.AddEntry(hpassed,tagging_methods[i] + Form(" : %.0f",hpassed->GetEntries()),"l");

    canv.cd(2);
    TGraphAsymmErrors* gr_eff = (TGraphAsymmErrors*)eff->CreateGraph();
    gr_eff->GetXaxis()->SetTitle(xlabel);
    gr_eff->Draw(i==0 ? "APZ" : "PZ");
    gr_eff->GetXaxis()->SetRangeUser(eff_xmin,eff_xmax);
    gr_eff->SetLineColor(getStyle(method)[0]);
    gr_eff->SetLineStyle(getStyle(method)[1]);
    gr_eff->SetLineWidth(2);
    //gr_eff->SetMarkerStyle(getStyle(method)[2]);
    gr_eff->SetMarkerColor(getStyle(method)[0]);
    gr_eff->SetMarkerSize(0.5);
    gr_eff->SetMinimum(0);
    gr_eff->SetMaximum(1);

    //thw following code doesn't work:
    //Error in <TEfficiency::CreatePaintingistogram>: Call this function only for dimension == 2
    //TH1F* h_eff = (TH1F*)eff->CreateHistogram();
    //h_eff->Draw("same");
    //h_eff->SetLineColor(getStyle(method).first);
    //h_eff->SetLineStyle(getStyle(method).second);
    //h_eff->SetMinimum(0);
    //h_eff->SetMaximum(1); 
  }

  canv.cd(3);
  leg1.Draw();

  canv.Print(plotsFile);
 
}

void addEffEtaPlotInCaloSamplings(TFile* f, TString datalabel, TString config, TString plotsFile) {
  TCanvas canv;
  canv.Divide(3,3);
  TString configtmp;
  int i=0;
  for (pair<int,int> calo_sample : calo_samplings) {
    canv.cd(++i); 
    (configtmp=config).ReplaceAll("&&!cut_jetCln_noise",
        Form("&&AntiKt4EMTopoJets.fracSamplingMaxIndex[0]<=%i&&!cut_jetCln_noise", calo_sample.second));
    TString name=configtmp+"/eff_OneSidedLoose_jet0_eta";
    TEfficiency *eff = (TEfficiency*)f->Get(name);
    if (!eff) throw "addEffEtaPlotInCaloSamplings: failed to read TEfficiency object: " + name;
    eff->SetTitle(Form("fracSamplingMaxIndex[0]<=%i",calo_sample.second));eff->SetTitle(Form("fracSamplingMaxIndex[0]<=%i",calo_sample.second));
    TGraphAsymmErrors* gr_eff = eff->CreateGraph();
    gr_eff->Draw("AP");
    gr_eff->SetMinimum(0);
    gr_eff->SetMaximum(1); 
  }
  canv.cd(9);
  TLegend leg(.05,.05,.95,.95);
  leg.AddEntry((TObject*)0, datalabel, "");
  leg.Draw();
  canv.Print(plotsFile);
}

void addPlotInRuns(TString dataDir, TString filePattern, TString effname, TString plotsFile) {
  TCanvas canv;
  canv.Divide(3,3);

  vector<TString> files = getListOfFilesInDirectory(dataDir,filePattern);
  int i=0;
  for (TString filename : files) {
    cout << filename << endl;
    TFile* f = TFile::Open(filename);
    TEfficiency* eff = (TEfficiency*)f->Get(effname);
    if (!eff) throw "addPlotInRuns: failed to read: "+effname;
    if (eff->GetTotalHistogram()->GetEntries()==0) {
      cout<<"skipping, no entries in the total histogram"<<endl;
      continue;
    }
    canv.cd(++i);
    eff->SetTitle(filename);
    eff->Draw();
    if (i==9) { canv.Print(plotsFile); i=0; }
  }
}

int main(int argc, char** argv) {

  TString mergedAllFile="";
  TString mergedAllLabel="";
  TString data16File="";
  TString data17File="";
  TString data18File="";
  TString data16_data18File=""; // exclude data17

  for (int i=1;i<argc;i++) {
    if (TString(argv[i])=="--mergedAllFile")     mergedAllFile=argv[i+1];
    if (TString(argv[i])=="--mergedAllLabel")    mergedAllLabel=argv[i+1];
    if (TString(argv[i])=="--data16File")        data16File=argv[i+1];
    if (TString(argv[i])=="--data17File")        data17File=argv[i+1];
    if (TString(argv[i])=="--data18File")        data18File=argv[i+1];
    if (TString(argv[i])=="--data16_data18File") data16_data18File=argv[i+1];
  }
  if (mergedAllFile=="") { cout<<"Bad syntax"<<endl;return 1;}

  TString dataDir = mergedAllFile.Tokenize("/")->At(0)->GetName();

  try {
    TFile* f=TFile::Open(mergedAllFile);
    if (!f) throw "failed to open root file";

    TFile* fData16 = TFile::Open(data16File);
    TFile* fData17 = TFile::Open(data17File);
    TFile* fData18 = TFile::Open(data18File);
    TFile* fData16_Data18 = TFile::Open(data16_data18File);

    //gStyle->SetErrorX(0); // doesn't work (?)
    gStyle->SetOptStat("");
    TCanvas canv;
    canv.Print(plotsFile+"(");

    for (int i=0;i<configs.size();i++) {     
      addMonPlots(f, configs[i], plotsFile);

      addEffPlot(f, mergedAllLabel, configs[i], "jet0_pT","jet1 pT [GeV]",   0, 1000, true,  plotsFile);

      addEffPlot(f, mergedAllLabel, configs[i], "jet0_eta","jet1 eta", -5,    5, false, plotsFile);
      addEffPlot(fData16, "2016", configs[i], "jet0_eta","jet1 eta", -5,    5, false, plotsFile);
      addEffPlot(fData17, "2017", configs[i], "jet0_eta","jet1 eta", -5,    5, false, plotsFile);
      addEffPlot(fData18, "2018", configs[i], "jet0_eta","jet1 eta", -5,    5, false, plotsFile);

      if (i==0) { 
        addEffEtaPlotInCaloSamplings(f, mergedAllLabel, configs[i], plotsFile);
        if (fData16) addEffEtaPlotInCaloSamplings(fData16, "2016",configs[i], plotsFile);
        if (fData17) addEffEtaPlotInCaloSamplings(fData17, "2017",configs[i], plotsFile);
        if (fData18) addEffEtaPlotInCaloSamplings(fData18, "2018",configs[i], plotsFile);

       TString tmp;
       // exclude the EMEC
       //addEffPlot(f, mergedAllLabel,
       //           (tmp=configs[i]).ReplaceAll("&&!cut_jetCln_noise",
       //                          "&&AntiKt4EMTopoJets.fracSamplingMaxIndex[0]!=4"
       //                          "&&AntiKt4EMTopoJets.fracSamplingMaxIndex[0]!=5"
       //                          "&&AntiKt4EMTopoJets.fracSamplingMaxIndex[0]!=6"
       //                          "&&AntiKt4EMTopoJets.fracSamplingMaxIndex[0]!=7"
       //                          "&&!cut_jetCln_noise"),
       //                          "jet0_eta","jet1 eta", -5,    5, false, plotsFile);

      // exclude 2017 data and include only EMB, EMEC and HEC
      //addEffPlot(fData16_Data18, "2016,2018",
      //           (tmp=configs[i]).ReplaceAll("&&!cut_jetCln_noise",
      //                           "&&AntiKt4EMTopoJets.fracSamplingMaxIndex[0]<=11"
      //                           "&&!cut_jetCln_noise"),
      //                           "jet0_eta","jet1 eta", -5,    5, false, plotsFile);

        //addPlotInRuns(dataDir,"data17_13TeV",configs[0]+"/eff_OneSidedLoose_jet0_eta", plotsFile);
      }

      addEffPlot(f, mergedAllLabel, configs[i], "jet0_phi","jet1 phi",-pi,   pi, false, plotsFile);
      addEffPlot(f, mergedAllLabel, configs[i], "jet0_time","jet1 time [ns]",-30,  30, true, plotsFile);    
      addEffPlot(f, mergedAllLabel, configs[i], "jet0_fracSamplingMaxIndex","jet1 fracSamplingMax index",0,  24, true, plotsFile);
    }
    canv.Print(plotsFile+")");

    f->Close();
  }
  catch (const char* err) {cout<<err<<endl;}
  catch (TString err) { cout<<err<<endl;}

  return 0;
}
