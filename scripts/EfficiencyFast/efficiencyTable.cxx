#include <iostream>
using namespace std;
#include <vector>

#include <TFile.h>
#include <TString.h>
#include <TH1F.h>
#include <TEfficiency.h>

#include "taggerAnalysisLib.h"

vector<TString> configs={

// nominal
"(passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0)&&(!(cut_jetCln_noise))",

// variations
//"(passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>20000&&(AntiKt4EMTopoJets.eta[0])<3.0)&&(!(cut_jetCln_noise))",

//"(passGRL&&StableBeams&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>200000&&(AntiKt4EMTopoJets.eta[0])<3.0)&&(!(cut_jetCln_noise))",

"(passGRL&&StableBeams&&(L1_J12_UNPAIRED_ISO||L1_J12_UNPAIRED_NONISO)&&NPV==0&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0)&&(!(cut_jetCln_noise))"

};

namespace efficiencyInDataTakingPeriods {

  vector<float> mean;
  vector<float> rms;

  void getMeanRMS(TFile *f, TString method, float& mean, float& rms) {

    const TString beamhalo_config = "(passGRL&&NPV==0&&StableBeams&&BunchStructure.isUnpairedIsolated&&AntiKt4EMTopoJets.pt[0]>120000&&(AntiKt4EMTopoJets.eta[0])<3.0)&&(!(cut_jetCln_noise))";
    TString histname = beamhalo_config + "/h_" + method;
    TH1F* hVsDataTakingPeriod = (TH1F*)f->Get(histname);
    if (!hVsDataTakingPeriod) 
      throw "getEfficiencyInDataTakingPeriodsRMS: failed to read histogram:" + histname;
    TH1F* hEff = new TH1F("hEff","",100,0,1);
    //without the last bin which is for all data
    for (int bin=1;bin<=hVsDataTakingPeriod->GetNbinsX()-1;bin++) {
      float eff = hVsDataTakingPeriod->GetBinContent(bin); 
      if (eff)
        hEff->Fill(eff);
      //cout << hVsDataTakingPeriod->GetXaxis()->GetBinLabel(bin) << " " << eff << endl;
    }
    mean = hEff->GetMean();
    rms = hEff->GetRMS();
    delete hEff; 
  }
}

int main(int argc, char** argv) {

  TString effFastFile="";
  //TString effVsRunNumberFile="";
  for (int i=1;i<argc;i++) {
    if (TString(argv[i])=="--effFastFile")
      effFastFile=argv[i+1];
    //if (TString(argv[i])=="--effVsRunNumberFile")
    //  effVsRunNumberFile=argv[i+1]; 
  }
  if (effFastFile=="") {cout<<"Bad syntax"<<endl;return 1;}

  try {

    TFile* f=TFile::Open(effFastFile);
    if (!f) throw "failed to open file: " + effFastFile;

    vector<TString> tagging_methods;
    TH1F* h_ntagged_tmp = (TH1F*)f->Get(configs[0]+"/h_ntagged");
    if (!h_ntagged_tmp) throw "h_ntagged_tmp is null";
    for (int i=1;i<=h_ntagged_tmp->GetNbinsX();i++)
      tagging_methods.push_back(h_ntagged_tmp->GetXaxis()->GetBinLabel(i));

    //TFile* fEffVsRunNum = TFile::Open(effVsRunNumberFile);
    //if (!fEffVsRunNum) throw "failed to open file: " + effVsRunNumberFile;

    vector<float> nomEff;
    nomEff.assign(tagging_methods.size(), 0);

    vector<float> statError;
    statError.assign(tagging_methods.size(), 0);
   
    vector<float> sysError;
    sysError.assign(tagging_methods.size(), 0);
 
    for (int iconfig=0;iconfig<configs.size();iconfig++) {

      TString config=configs[iconfig];
      cout<<endl<<config<<endl<<endl;
      TString histname=config+"/h_ntagged";
      TH1F* h_ntagged = (TH1F*)f->Get(histname);
      if (!h_ntagged) 
        throw "Failed to read histogram " + histname; 
      if (h_ntagged->GetNbinsX() != tagging_methods.size())
        throw "Wrong number of histogram bins.";

      int nbeamhalo = h_ntagged->GetEntries();
      cout << Form("Selected sample:    %10i",nbeamhalo) << endl;
      for (int imethod=0;imethod<tagging_methods.size();imethod++) {
        int ntagged = h_ntagged->GetBinContent(imethod+1);

        float mean_eff = (nbeamhalo !=0) ? (float)ntagged/nbeamhalo : 0;

        float statErrorUp=TEfficiency::Normal(nbeamhalo,ntagged,0.683,true)-mean_eff; 
        float statErrorDown=mean_eff-TEfficiency::Normal(nbeamhalo,ntagged,0.683,false); 
        float statErr = 0.5*(statErrorUp+statErrorDown);

        cout <<Form("%-20s%10i%10.1f%% %15.2f%%", 
                    tagging_methods[imethod].Data(), ntagged, 100*mean_eff, 100*statErr)<<endl;
      
        if (iconfig==0) {
          nomEff[imethod]=mean_eff;
          statError[imethod]=statErr;
        }

        sysError[imethod]+= TMath::Power(mean_eff - nomEff[imethod], 2);
      }

    }
    /*
    cout<<"Efficiency in data taking periods"<<endl; 
    efficiencyInDataTakingPeriods::rms.assign(tagging_methods.size(),0);
    for (int i=0;i<tagging_methods.size();i++) {
      float mean;
      float rms;
      efficiencyInDataTakingPeriods::getMeanRMS(fEffVsRunNum,tagging_methods[i],mean,rms);
      cout << Form("%15s mean=%8.3f rms=%8.3f",tagging_methods[i].Data(), mean, rms) << endl;
      efficiencyInDataTakingPeriods::rms[i] = rms;  
    }*/

    cout<<"---------------------------------------------------------------------------" << endl; 
    cout<<Form("%15s%15s%15s%15s","","Efficiency","Statistical","Systematic") << endl 
        <<Form("%15s%15s%15s%15s","","[%]",       "error [%]",  "error [%]" ) <<endl;
    cout<<"---------------------------------------------------------------------------" << endl;
    for (int i=0;i<tagging_methods.size();i++) {

      cout<<Form("%-15s%15.1f%15.2f%15.1f",
                 tagging_methods[i].Data(), 
                 100*nomEff[i],
                 100*statError[i],
                 100*TMath::Sqrt(sysError[i])
                 //100*efficiencyInDataTakingPeriods::rms[i]
                 )
                 << endl;
    }
    cout<<"---------------------------------------------------------------------------" << endl;
    f->Close();

 
  } 
  catch (const char* err) { cout << err << endl; }
  catch (TString err) { cout<<err<<endl;}

  return 0;
}

