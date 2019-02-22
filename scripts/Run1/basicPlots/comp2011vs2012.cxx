#include <iostream>
using namespace std;

#include <TChain.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TString.h>
#include <TCut.h>
#include <TDirectory.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TMath.h>

const TString plots="comp2011vs2012.pdf";

void addPlot(TChain& c1, TChain& c2, TString var, TString histdef, TCut cut) {
  cout<<"addPlot " << var << " "<<cut.GetTitle() << endl;
  c1.Draw(var+">>h1"+histdef,cut,"goff");
  c2.Draw(var+">>h2"+histdef,cut,"goff");
  TH1F* h1=(TH1F*)gDirectory->Get("h1");
  if (!h1) throw "h1 is null";
  TH1F* h2=(TH1F*)gDirectory->Get("h2");
  if (!h2) throw "h2 is null";
  TCanvas canv;
  h1->Scale(1./h1->Integral());
  h2->Scale(1./h2->Integral());
  h1->SetMaximum(1.1*TMath::Max(h1->GetMaximum(),h2->GetMaximum()));
  h1->Draw("hist");
  h2->SetLineColor(kRed);
  h2->Draw("hist same");
  h1->SetLineWidth(2);
  h2->SetLineWidth(2);
  TLegend leg(.7,.7,.98,.98);
  leg.AddEntry(h1,"2011","l");
  leg.AddEntry(h1,Form("Entries=%.0f",h1->GetEntries()),"");
  leg.AddEntry(h2,"2012","l");
  leg.AddEntry(h2,Form("Entries=%.0f",h2->GetEntries()),"");
  leg.Draw();
  canv.Print(plots);
}

int main() {

  try {

  TChain c1("background");
  c1.Add("$HOME/workspace/data/data11_7TeV.00179804.physics_Background.merge.NTUP_BKGD.r2603_p1196/*.root*");

  TChain c2("background");
  c2.Add("$HOME/workspace/data/data12_8TeV.00212172.physics_Background.merge.NTUP_BKGD.r4065_p1278_p1378/*.root*");

  //cout<<"c1 entries="<<c1.GetEntries()<<endl;
  //cout<<"c2 entries="<<c2.GetEntries()<<endl;
  TCanvas canv;
  canv.Print(plots+"(");
  gStyle->SetOptStat("");

  vector<TString> segCollections={"mooreseg","mboyseg"};

  for (TString segCollection : segCollections) {

    TCut cut_CSC = (segCollection + "_stationName==33||"+segCollection+"_stationName==34").Data();

    TCut cut = cut_CSC;
    cut="L1_J10_UNPAIRED_ISO||L1_J10_UNPAIRED_NONISO"; 
    addPlot(c1,c2,segCollection+"_theta", "(100,0,3.1415)",cut);
    TString rxy = "TMath::Sqrt("+segCollection+"_x*"+segCollection+"_x+"
                                +segCollection+"_y*"+segCollection+"_y)";
    addPlot(c1,c2,rxy,"(100,0,5000)",cut); 
    TString thetaPos="180/TMath::Pi()*TMath::ATan2("+rxy+","+segCollection+"_z)";
    addPlot(c1,c2,thetaPos,"(180,0,180)",cut);
    addPlot(c1,c2,"TMath::Abs("+thetaPos+"-"+segCollection+"_theta)","(180,0,180)",cut);
    addPlot(c1,c2,segCollection+"_t0","(100,-100,100)",cut);

  }

  canv.Clear();
  canv.Print(plots+")");

  }
  catch (const char* err) { cout<<err<<endl;return 1;}
  catch (TString err) { cout<<err<<endl;return 1;}

  return 0;
}
