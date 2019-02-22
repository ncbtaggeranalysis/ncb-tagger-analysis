
#include <iostream>
using namespace std;
#include <vector>

#include <TString.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TObjArray.h>

class FinalPlots {
  public:
         FinalPlots(TString unpairedSample, TString collisionSample, TString plotsFile);
        ~FinalPlots();

    void addMuonSegmentYvsX();
    void addMuonSegmentDeltaTheta();
    void addMuonSegmentTime();
    void addMuonSegmentPhiDiffAC(bool zoomed);
    void addMuonSegmentTimeDiffAC();
    void addMuonSegmentClusterRDiff();

    void addMuonSegmentClusterPhiDiff();  

    void addJetPlot(TString hist);
    void addJetPlot2D(TString hist, TString sample);

  private:
    TString m_unpairedSample;
    TString m_collisionSample;

    TFile* m_fUnpaired;
    TFile* m_fColl;
    TCanvas m_canvas;
    TString m_plotsFile;
    TLegend* m_legend;

    TString m_mainDirUnpaired;
    TString m_mainDirColl;

    void saveCanvas();

    vector<TString> m_tags;
}; 

FinalPlots::FinalPlots(TString unpairedSample, TString collisionSample, TString plotsFile) {
  m_unpairedSample = unpairedSample;
  m_collisionSample = collisionSample;   
  m_plotsFile = plotsFile;
  TObjArray* objArray = m_plotsFile.Tokenize("."); 
  m_tags.push_back(objArray->At(1)->GetName());
  m_tags.push_back(objArray->At(2)->GetName());

  cout << "Open file: " << m_unpairedSample << endl;
  m_fUnpaired = TFile::Open(m_unpairedSample);
  if (!m_fUnpaired) throw "File pointer to the unpaired sample is null";
  if (!m_fUnpaired->IsOpen()) throw "failed to open the unpaired sample";
  
  m_fColl = TFile::Open(m_collisionSample);
  if (!m_fColl) throw "File pointer to the collision sample is null";
  if (!m_fColl->IsOpen()) throw "failed to open collision sample"; 

  m_canvas.Print(m_plotsFile+"(");

  gStyle->SetOptStat("");
  m_legend = 0;

  m_mainDirUnpaired="NPV==0&&StableBeams&&BunchStructure.isUnpairedIsolated&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0";
  m_mainDirColl="StableBeams&&BunchStructure.isPaired&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0";

}

FinalPlots::~FinalPlots() {
  m_canvas.Clear();
  m_canvas.Print(m_plotsFile+")");
}

void FinalPlots::addMuonSegmentYvsX() {
  cout<<"addMuonSegmentYvsX"<<endl;

  TString histMDT="SelectedMuonSegments_xy_MdtI";
  TH2F* hMDT = (TH2F*)m_fUnpaired->Get(m_mainDirUnpaired + "/" + histMDT);
  if (!hMDT) throw "hMDT unpaired is null";

  TString histCSC="SelectedMuonSegments_xy_CSC";
  TH2F* hCSC = (TH2F*)m_fUnpaired->Get(m_mainDirUnpaired + "/" + histCSC);
  if (!hCSC) throw "hCSC unpaired is null";  

  float maxz = TMath::Max(hCSC->GetMaximum(), hMDT->GetMaximum());
  hCSC->SetMaximum(maxz);
  hMDT->SetMaximum(maxz);

  m_canvas.SetRightMargin(0.25);
  hMDT->Draw("colz");
  hMDT->SetTitle("");
  hMDT->GetXaxis()->SetTitle("x [mm]");
  hMDT->GetYaxis()->SetTitle("y [mm]");
  hCSC->Draw("colz same");
  m_legend = new TLegend(0.75,0.5,0.99,0.9);
  m_legend->AddEntry(hCSC, m_tags[0], "");
  m_legend->AddEntry(hCSC, m_tags[1], "");   
  m_legend->AddEntry(hCSC, Form("CSC entries=%.0f",hCSC->GetEntries()),"");
  m_legend->AddEntry(hMDT, Form("MdtI entries=%.0f",hMDT->GetEntries()),"");
  m_legend->Draw();
  saveCanvas();  

}

void FinalPlots::addMuonSegmentDeltaTheta() {
  cout<<"addMuonSegmentDeltaTheta"<<endl;
  vector<TString> histVec = {
   "MuonSegments_deltaTheta_CSC",
   "MuonSegments_deltaTheta_MdtI"
  };

  for (TString hist : histVec) {
    cout << hist << endl;
    TH1F* hUnpaired = (TH1F*)m_fUnpaired->Get(m_mainDirUnpaired+"/"+"/"+hist);
    if (!hUnpaired) throw "hUnpaired is null";

    TH1F* hColl = (TH1F*)m_fColl->Get(m_mainDirColl+"/"+hist);
    if (!hColl) throw "hColl is null";

    hUnpaired->Scale(1./hUnpaired->Integral());
    hColl->Scale(1./hColl->Integral());
    hUnpaired->SetLineColor(kRed);
    float maxy=TMath::Max(hUnpaired->GetMaximum(),hColl->GetMaximum());
    hColl->SetMaximum(maxy);
    hColl->Draw("hist"); 
    hColl->GetXaxis()->SetRangeUser(0,30);
    hColl->SetTitle(";#lbar #theta_{pos}-#theta_{dir} #lbar [degrees]");
    hUnpaired->Draw("same hist");
    TLegend legend(.7,.5,.99,.9);
    if (hist.EndsWith("CSC")) legend.AddEntry(hUnpaired,"CSC","");
    else if (hist.EndsWith("MdtI")) legend.AddEntry(hUnpaired,"MDT Inner end-cap","");
    legend.AddEntry(hUnpaired,"Unpaired","l");
    legend.AddEntry(hUnpaired, m_tags[0], "");
    legend.AddEntry(hUnpaired, m_tags[1], "");
    legend.AddEntry(hUnpaired,Form("Entries=%.0f",hUnpaired->GetEntries()),"");
    legend.AddEntry(hColl,"Colliding","l");
    legend.AddEntry(hColl,Form("Entries=%.0f",hColl->GetEntries()),"");
    legend.Draw();
    saveCanvas();
  }
}

void FinalPlots::addMuonSegmentTime() {
  cout<<"addMuonSegmentTime"<<endl;
  vector<TString> histVec = {
    "SelectedMuonSegments_t0_CSC",
    "SelectedMuonSegments_t0_MdtI"
  };

  for (TString hist : histVec) {
    cout << hist << endl;
    TH1F* hUnpaired = (TH1F*)m_fUnpaired->Get(m_mainDirUnpaired + "/" +hist);
    if (!hUnpaired) throw "hUnpaired is null";
    TH1F* hColl = (TH1F*)m_fColl->Get(m_mainDirColl+"/"+hist);
    if (!hColl) throw "hColl is null";
    hUnpaired->Scale(1./hUnpaired->Integral());
    hColl->Scale(1./hColl->Integral());
    hUnpaired->SetLineColor(kRed);
    hUnpaired->SetMaximum(0.09);
    hUnpaired->Draw("hist");
    hUnpaired->GetXaxis()->SetRangeUser(-100,100);
    hUnpaired->SetTitle(";t [ns]");
    hColl->Draw("same hist");
    TLegend legend(.7,.5,.99,.9); 
    if (hist.EndsWith("CSC")) legend.AddEntry(hUnpaired,"CSC","");
    else if (hist.EndsWith("MdtI")) legend.AddEntry(hUnpaired,"MDT Inner end-cap","");
    legend.AddEntry(hUnpaired,"Unpaired","l");
    legend.AddEntry(hUnpaired, m_tags[0], "");
    legend.AddEntry(hUnpaired, m_tags[1], "");
    legend.AddEntry(hUnpaired,Form("Entries=%.0f",hUnpaired->GetEntries()),"");
    legend.AddEntry(hColl,"Colliding","l");
    legend.AddEntry(hColl,Form("Entries=%.0f",hColl->GetEntries()),"");
    legend.Draw();
    saveCanvas();
  }
}

void FinalPlots::addMuonSegmentPhiDiffAC(bool zoomed) {
  cout<<"addMuonSegmentTimeDiffAC"<<endl;
  vector<TString> histVec = {
    "SelectedMuonSegments_phiDiffAC_CSC",
    "SelectedMuonSegments_phiDiffAC_MdtI"
  };

  for (TString hist : histVec) {
    cout << hist << endl;
    TH1F* hUnpaired = (TH1F*)m_fUnpaired->Get(m_mainDirUnpaired + "/" +hist);
    if (!hUnpaired) throw "hUnpaired is null";
    TH1F* hColl = (TH1F*)m_fColl->Get(m_mainDirColl+"/"+hist);
    if (!hColl) throw "hColl is null";
    hUnpaired->Scale(1./hUnpaired->Integral());
    hColl->Scale(1./hColl->Integral());
    hUnpaired->SetLineColor(kRed);
    hUnpaired->SetMinimum(0); 
    //hUnpaired->SetMaximum(0.006);
    hUnpaired->Draw("hist");
    if (zoomed)
      hUnpaired->GetXaxis()->SetRangeUser(-30,30);
    hUnpaired->SetTitle(";#Phi_{A}-#Phi_{C} [degrees]");
    hColl->Draw("same hist");
    TLegend legend(.7,.5,.99,.9);
    if (hist.EndsWith("CSC")) legend.AddEntry(hUnpaired,"CSC","");
    else if (hist.EndsWith("MdtI")) legend.AddEntry(hUnpaired,"MDT Inner end-cap","");
    legend.AddEntry(hUnpaired,"Unpaired","l");
    legend.AddEntry(hUnpaired, m_tags[0], "");
    legend.AddEntry(hUnpaired, m_tags[1], "");
    legend.AddEntry(hUnpaired,Form("Entries=%.0f",hUnpaired->GetEntries()),"");
    legend.AddEntry(hColl,"Colliding","l");
    legend.AddEntry(hColl,Form("Entries=%.0f",hColl->GetEntries()),"");
    legend.Draw();
    saveCanvas();
  }   
}

void FinalPlots::addMuonSegmentTimeDiffAC() {
  cout<<"addMuonSegmentTimePhiDiffAC"<<endl;
  vector<TString> histVec = {
    "SelectedMuonSegments_passPhiDiffAC_timeDiffCSC",
    "SelectedMuonSegments_passPhiDiffAC_timeDiffMdtI"
  };

  for (TString hist : histVec) {
    cout << hist << endl;
    TH1F* hUnpaired = (TH1F*)m_fUnpaired->Get(m_mainDirUnpaired + "/" +hist);
    if (!hUnpaired) throw "hUnpaired is null";
    TH1F* hColl = (TH1F*)m_fColl->Get(m_mainDirColl+"/"+hist);
    if (!hColl) throw "hColl is null";
    hUnpaired->Scale(1./hUnpaired->Integral());
    hColl->Scale(1./hColl->Integral());
    hUnpaired->SetLineColor(kRed);
    hUnpaired->SetMinimum(0);
    //hUnpaired->SetMaximum(0.006);
    //    hUnpaired->Draw("hist");
    hUnpaired->Draw("hist");
    //if (zoomed)
    //  hUnpaired->GetXaxis()->SetRangeUser(-30,30);
    hUnpaired->SetTitle(";t_{A}-t_{C} [ns]");
    hColl->Draw("same hist");
    TLegend legend(.7,.5,.99,.9);
    if (hist.EndsWith("CSC")) legend.AddEntry(hUnpaired,"CSC","");
    else if (hist.EndsWith("MdtI")) legend.AddEntry(hUnpaired,"MDT Inner end-cap","");
    legend.AddEntry(hUnpaired,"Unpaired","l");
    legend.AddEntry(hUnpaired, m_tags[0], "");
    legend.AddEntry(hUnpaired, m_tags[1], "");
    legend.AddEntry(hUnpaired,Form("Entries=%.0f",hUnpaired->GetEntries()),"");
    legend.AddEntry(hColl,"Colliding","l");
    legend.AddEntry(hColl,Form("Entries=%.0f",hColl->GetEntries()),"");
    legend.Draw();
    saveCanvas();
  }

}

void FinalPlots::addMuonSegmentClusterPhiDiff() {
  cout<<"addMuonSegmentClusterPhiDiff"<<endl;
  vector<TString> histVec = {
    "SelectedMuonSegments_phiDiff_selectedClus_CSC",
    "SelectedMuonSegments_phiDiff_selectedClus_MdtI"
  }; 
  for (TString hist : histVec) {
    cout << hist << endl;
    TH1F* hUnpaired = (TH1F*)m_fUnpaired->Get(m_mainDirUnpaired + "/" +hist);
    if (!hUnpaired) throw "hUnpaired is null";
    TH1F* hColl = (TH1F*)m_fColl->Get(m_mainDirColl+"/"+hist);
    if (!hColl) throw "hColl is null";
    hUnpaired->Scale(1./hUnpaired->Integral());
    hColl->Scale(1./hColl->Integral());
    hUnpaired->SetLineColor(kRed);
    //hUnpaired->SetMaximum(0.2);
    hUnpaired->Draw("hist");
    hUnpaired->GetXaxis()->SetRangeUser(-30,30);
    hUnpaired->SetTitle(";#Phi_{Clus}-#Phi_{Seg} [degrees]");
    hColl->Draw("same hist");
    TLegend legend(.7,.5,.99,.9);
    if (hist.EndsWith("CSC")) legend.AddEntry(hUnpaired,"CSC","");
    else if (hist.EndsWith("MdtI")) legend.AddEntry(hUnpaired,"MDT Inner end-cap","");
    legend.AddEntry(hUnpaired,"Unpaired","l");
    legend.AddEntry(hUnpaired, m_tags[0], "");
    legend.AddEntry(hUnpaired, m_tags[1], "");
    legend.AddEntry(hUnpaired,Form("Entries=%.0f",hUnpaired->GetEntries()),"");
    legend.AddEntry(hColl,"Colliding","l");
    legend.AddEntry(hColl,Form("Entries=%.0f",hColl->GetEntries()),"");
    legend.Draw();
    saveCanvas();
  }
}

void FinalPlots::addMuonSegmentClusterRDiff() {
  cout<<"addMuonSegmentClusterRDiff"<<endl;
  vector<TString> histVec = {
    "SelectedMuonSegments_rDiff_selectedClus_CSC",
    "SelectedMuonSegments_rDiff_selectedClus_MdtI"
  };
  for (TString hist : histVec) {
    cout << hist << endl;
    TH1F* hUnpaired = (TH1F*)m_fUnpaired->Get(m_mainDirUnpaired + "/" +hist);
    if (!hUnpaired) throw "hUnpaired is null";
    TH1F* hColl = (TH1F*)m_fColl->Get(m_mainDirColl+"/"+hist);
    if (!hColl) throw "hColl is null";
    hUnpaired->Scale(1./hUnpaired->Integral());
    hColl->Scale(1./hColl->Integral());
    hUnpaired->SetLineColor(kRed);
    float maxy=TMath::Max(hUnpaired->GetMaximum(),hColl->GetMaximum());
    hUnpaired->SetMaximum(1.1*maxy);
    hUnpaired->Draw("hist");
    //hUnpaired->GetXaxis()->SetRangeUser(-30,30);
    hUnpaired->SetTitle(";r_{Clus}-r_{Seg} [mm]");
    hColl->Draw("same hist");
    TLegend legend(.7,.5,.99,.9);
    if (hist.EndsWith("CSC")) legend.AddEntry(hUnpaired,"CSC","");
    else if (hist.EndsWith("MdtI")) legend.AddEntry(hUnpaired,"MDT Inner end-cap","");
    legend.AddEntry(hUnpaired,"Unpaired","l");
    legend.AddEntry(hUnpaired, m_tags[0], "");
    legend.AddEntry(hUnpaired, m_tags[1], "");
    legend.AddEntry(hUnpaired,Form("Entries=%.0f",hUnpaired->GetEntries()),"");
    legend.AddEntry(hColl,"Colliding","l");
    legend.AddEntry(hColl,Form("Entries=%.0f",hColl->GetEntries()),"");
    legend.Draw();
    saveCanvas();
  }
}

void FinalPlots::addJetPlot(TString hist) {
  
    cout << "addJetPlots " << hist << endl;
    TH1F* hUnpaired = (TH1F*)m_fUnpaired->Get(m_mainDirUnpaired + "/" +hist);
    if (!hUnpaired) throw "hUnpaired is null";
    TH1F* hColl = (TH1F*)m_fColl->Get(m_mainDirColl+"/"+hist);
    if (!hColl) throw "hColl is null";
    hUnpaired->Scale(1./hUnpaired->Integral());
    hColl->Scale(1./hColl->Integral());
    hUnpaired->SetLineColor(kRed);
    if (hist == "jet0_phi") {
      hUnpaired->SetTitle("; Leading jet #Phi"); 
      hUnpaired->Rebin(8);
      hUnpaired->SetMaximum(0.15);
      hColl->Rebin(8);
    }
    if (hist == "jet0_time") {
      hUnpaired->SetTitle("; Leading jet time [ns]");
      hUnpaired->GetXaxis()->SetRangeUser(-20,20); 
      hUnpaired->SetMaximum(0.06);
    }
    hUnpaired->Draw("hist");
    hColl->Draw("same hist");
    TLegend legend(.6,.6,.89,.89);
    legend.SetBorderSize(0);
    legend.AddEntry(hUnpaired,"Unpaired","l");
    legend.AddEntry(hUnpaired, m_tags[0], "");
    legend.AddEntry(hUnpaired, m_tags[1], "");
    legend.AddEntry(hUnpaired,Form("Entries: %.0f",hUnpaired->GetEntries()),"");
    legend.AddEntry(hColl,"Colliding","l");
    legend.AddEntry(hColl,Form("Entries: %.0f",hColl->GetEntries()),""); 
    legend.Draw();
    saveCanvas();
}

void FinalPlots::addJetPlot2D(TString hist, TString sample) {
  cout << "addJetPlot2D " << hist << " " << sample << endl;
  TH2F* h = 0;
  if (sample == "unpaired") 
    h = (TH2F*)m_fUnpaired->Get(m_mainDirUnpaired+"/"+hist);
  else if (sample == "colliding") 
    h = (TH2F*)m_fColl->Get(m_mainDirColl+"/"+hist);

  if (!h) throw "h is null";
  h->Draw("colz");
  if (hist=="jet0_etaphi") {
    h->SetTitle(";Leading jet #eta;Leading jet #phi");
    h->RebinY(4);
    h->GetXaxis()->SetRangeUser(-3,3);
  }
  if (hist=="jet0_etatime") {
    h->SetTitle(";Leading jet #eta;Leading jet time [ns]");
    h->GetYaxis()->SetRangeUser(-20,20);
    h->GetXaxis()->SetRangeUser(-3,3);
  }
  m_canvas.SetRightMargin(0.3);
  if (hist=="jet0_etaphi") m_canvas.SetLogz(1);
  saveCanvas();
  m_canvas.SetRightMargin(0.1);
  m_canvas.SetLogz(0);
}

void FinalPlots::saveCanvas() {
  m_canvas.Print(m_plotsFile);
}

int main(int argc, char** argv) {

  TString collisionSample = "";
  TString unpairedSample = "";
  TString plotsFile = "";

  for (int i=1;i<argc;i++) {

    if (TString(argv[i]) == "--collision-sample")
      collisionSample = argv[i+1];
    if (TString(argv[i]) == "--unpaired-sample")
      unpairedSample = argv[i+1];
    if (TString(argv[i]) == "--plotsFile")
      plotsFile = argv[i+1];
  }

  try {
    FinalPlots finalPlots(unpairedSample, collisionSample, plotsFile);

    finalPlots.addJetPlot("jet0_phi");
    finalPlots.addJetPlot2D("jet0_etaphi","unpaired");
    finalPlots.addJetPlot2D("jet0_etaphi","colliding");
    finalPlots.addJetPlot("jet0_time");
    finalPlots.addJetPlot2D("jet0_etatime","unpaired");
    finalPlots.addJetPlot2D("jet0_etatime","colliding");

    finalPlots.addMuonSegmentYvsX();
    finalPlots.addMuonSegmentDeltaTheta(); 
    finalPlots.addMuonSegmentTime();
    finalPlots.addMuonSegmentPhiDiffAC(false);
    finalPlots.addMuonSegmentPhiDiffAC(true);
    finalPlots.addMuonSegmentTimeDiffAC();
    finalPlots.addMuonSegmentClusterPhiDiff();
    finalPlots.addMuonSegmentClusterRDiff();

 }
  catch (const char* err) {
    cout << err << endl;
  }

  return 0;

}
