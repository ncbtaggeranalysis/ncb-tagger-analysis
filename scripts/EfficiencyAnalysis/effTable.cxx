#include <iostream>
using namespace std;
#include <vector>

#include <TString.h>
#include <TFile.h>
#include <TEfficiency.h>
#include <TMath.h>
#include <TH1.h>

const vector<TString> tagging_methods= {
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
  "TwoSided",
  "NoTimeTight&&TwoSided"
};

vector<float> getEfficiency(TFile* f, TString beamhalo_config, TString tagging_method) {
  TString name = beamhalo_config+"/eff_"+tagging_method;
  TEfficiency* eff = (TEfficiency*)f->Get(name);
  if (!eff) throw "failed to read TEfficiency object:" + name;
  vector<float> result;
  float nomEff_mean=eff->GetEfficiency(1);
  float nomEff_statErr=0.5*(eff->GetEfficiencyErrorLow(1)+eff->GetEfficiencyErrorUp(1));
  float ntagged = eff->GetPassedHistogram()->GetEntries();
  result.push_back(nomEff_mean);
  result.push_back(nomEff_statErr);
  result.push_back(ntagged);
  return result;
}

int getNumEventsInSample(TFile* f, TString config) {
  TEfficiency* eff = (TEfficiency*)f->Get(config+"/eff_TwoSided");
  if (!eff) throw "printConfig: eff pointer is null";
  return eff->GetTotalHistogram()->GetEntries();
}

int main(int argc, char** argv) {

  TString effFile="";
  TString beamhaloNominalConfig="";
  for (int i=1;i<argc;i++) {
    if (TString(argv[i])=="--effFile")
      effFile=argv[i+1];
    if (TString(argv[i])=="--nominalConfig")
      beamhaloNominalConfig=argv[i+1];
  }

  TString tmp;
  vector<TString> beamhalo_configs = {
    beamhaloNominalConfig,
    (tmp=beamhaloNominalConfig).ReplaceAll("AntiKt4EMTopoJets.pt[0]>120000","AntiKt4EMTopoJets.pt[0]>20000"),
    (tmp=beamhaloNominalConfig).ReplaceAll("AntiKt4EMTopoJets.pt[0]>120000","AntiKt4EMTopoJets.pt[0]>200000"),
    (tmp=beamhaloNominalConfig).ReplaceAll("BunchStructure.isUnpairedIsolated","L1_J12_UNPAIRED_ISO")
    //(tmp=beamhaloNominalConfig).ReplaceAll("BunchStructure.isUnpairedIsolated","(L1_J12_UNPAIRED_ISO||L1_J12_UNPAIRED_NONISO)")
  };

  //if (beamhaloNominalConfig.Contains("!cut_jetCln_noise_EMF"))
  //  beamhalo_configs.push_back((tmp=beamhaloNominalConfig).ReplaceAll("!cut_jetCln_noise_EMF","!cut_jetCln_noise"));
  //else if (beamhaloNominalConfig.Contains("!cut_jetCln_noise"))
  //  beamhalo_configs.push_back((tmp=beamhaloNominalConfig).ReplaceAll("!cut_jetCln_noise","!cut_jetCln_noise_EMF"));


  TFile* f = TFile::Open(effFile);
  if (!f) { cout << "failed to open file: " << effFile << endl; return 1;}

  vector<float> nomEff_mean;
  nomEff_mean.assign(tagging_methods.size(),0);

  vector<float> nomEff_ntagged;
  nomEff_ntagged.assign(tagging_methods.size(),0);

  vector<float> nomEff_statErr;
  nomEff_statErr.assign(tagging_methods.size(),0);

  vector<vector<float>> syst_err;
  for (TString config : beamhalo_configs) {
    vector<float> syst_err1;
    syst_err1.assign(tagging_methods.size(),0);
    syst_err.push_back(syst_err1);
  }

  try {

    for (int i=0;i<tagging_methods.size();i++) {
      vector<float> eff = getEfficiency(f, beamhalo_configs[0], tagging_methods[i]);
      nomEff_mean[i]=eff[0];
      nomEff_statErr[i]=eff[1];
      nomEff_ntagged[i]=eff[2];
    }

    for (int i=1;i<beamhalo_configs.size();i++) 
      for (int j=0;j<tagging_methods.size();j++) {
        vector<float> eff = getEfficiency(f,beamhalo_configs[i],tagging_methods[j]);
        syst_err[i][j]= eff[0] - nomEff_mean[j];
        syst_err[0][j]+= TMath::Power(syst_err[i][j],2); 
    }

    for (int j=0;j<tagging_methods.size();j++)
      syst_err[0][j]=TMath::Sqrt(syst_err[0][j]); 

    cout << "Nominal configuration: " << endl << beamhalo_configs[0] << endl
         << "  events: " << getNumEventsInSample(f,beamhalo_configs[0]) << endl << endl;
    cout << "Variated configurations: " << endl;
    for (int i=1; i<beamhalo_configs.size();i++)
      cout << beamhalo_configs[i] << endl 
         << "  events: " << getNumEventsInSample(f,beamhalo_configs[i]) << endl << endl;
    
    cout<<Form("%-21s%8s%11s%10s%50s","","Tagged","Efficiency", "Stat.",     "Systematic error") << endl;
    cout<<Form("%-21s%8s%11s%10s%10s","","events","[%]",        "error [%]", "Total [%]");
    for (int i=1;i<beamhalo_configs.size();i++)
      cout << "   Comp. "<<i;
    cout<<endl; 
    for (int j=0;j<tagging_methods.size();j++) {
      cout << Form("%-21s%8.0f%11.1f%10.2f", tagging_methods[j].Data(),
                                      nomEff_ntagged[j],
                                      100*nomEff_mean[j],
                                      100*nomEff_statErr[j]);
     
      for (int i=0;i<beamhalo_configs.size();i++)
        cout<<Form("%10.1f",100*syst_err[i][j]);

      cout<<endl;
    }
    cout<<endl;
  }
  catch (const char* err) {cout<<err<<endl;return 1;}
  catch (TString err) {cout<<err<<endl;return 1;}            

  f->Close();

  return 0;
}
