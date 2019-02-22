#include <iostream>
using namespace std;
#include <fstream>

#include <TFile.h>
#include <TH1F.h>
#include <TString.h>

const int MIN_NUM_EVENTS_BEAMHALO_SAMLPLE = 100;

int main(int argc, char** argv) {

  try {

    fstream ftxt(argv[1],std::fstream::in);
    if (!ftxt.is_open()) 
      throw "failed to open txt file";

    TString effHist = argv[2];
    cout << effHist << endl;

    TString sample;
    vector<TString> tagger_methods;
    vector<TH1F*> heff;
    TString projectTag = "";
    while (ftxt >> sample) {
      cout<<sample<<endl;
      if (projectTag=="") {
        if (sample.Contains("data11_7TeV")) projectTag="data11_7TeV";
        else if (sample.Contains("data12_8TeV")) projectTag="data12_8TeV";
        else throw "unknown project tag";
      }
      TFile* f = TFile::Open(sample);
      if (!f) throw "failed to open root file: " + sample;
      TH1F* h_ntagged = (TH1F*)f->Get(effHist);
      if (!h_ntagged) throw effHist + " is null";
      TH1F* h_cutflow = (TH1F*)f->Get("h_cutflow");
      if (!h_cutflow) throw "failed to read cutflow histogram";

      if (tagger_methods.size()==0) 
        for (int i=1;i<=h_ntagged->GetNbinsX();i++) {
          TString method = h_ntagged->GetXaxis()->GetBinLabel(i);
          tagger_methods.push_back(method);
          heff.push_back(new TH1F("h_eff_" + method,"",100,0,1));
        }   

      cout << Form("Total events without event selection: %10.0f",
                    h_cutflow->GetBinContent(1)) << endl;
      int nsample = h_ntagged->GetEntries();
      cout << Form("Events in the selected sample:        %10i", nsample) << endl; 
      for (int i=1;i<=h_ntagged->GetNbinsX();i++) {
        TString method = h_ntagged->GetXaxis()->GetBinLabel(i);
        int ntagged = h_ntagged->GetBinContent(i);
        float eff = nsample != 0 ? (float)ntagged/nsample : 0;
        cout << Form("%-22s%10i%15.1f%%",method.Data(),ntagged,100*eff) << endl;
        if (eff)
          if (nsample >= MIN_NUM_EVENTS_BEAMHALO_SAMLPLE)
            heff[i-1]->Fill(eff);
      }

      //f->Close(); 
    }

    TFile* fout = TFile::Open("HIST/"+projectTag+".efficiencyVsRunNumber.root","update");
    TString segmentCollection = "";
    if (effHist=="h_ntagged_bb") segmentCollection="ConvertedMBoySegments";
    else if (effHist="h_ntagged_bbMoore") segmentCollection="MooreSegments";
    else throw "Unknown segment collection";
 
    for (int i=0;i<tagger_methods.size();i++)
      heff[i]->Write(TString(heff[i]->GetName()).ReplaceAll("h_eff","h_eff_"+segmentCollection+"_"), TObject::kOverwrite);
    fout->Close();
    ftxt.close();



  }
  catch (const char* err) { cout<<err<<endl;return 1;}
  catch (TString err) {cout<<err<<endl;return 1;}
  
  return 0;
}
