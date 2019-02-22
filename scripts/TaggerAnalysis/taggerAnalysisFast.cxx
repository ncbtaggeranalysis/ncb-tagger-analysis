#include <vector>
using namespace std;
#include <iostream>
#include <fstream>
#include <map>
#include <utility>

#include <TFile.h>
#include <TString.h>
#include <TTree.h>
#include <TH1F.h>
#include <TDirectory.h>
#include <TCut.h>
#include <TDirectory.h>
#include <TSystem.h>

#include "taggerAnalysisLib.h"

const bool doEvent = true;
const bool doJets = false;
const bool doTopoClusters = false;
const bool doMuonSegments = true;

const TCut cut_jetCln_noise0 = "Sum$(AntiKt4EMTopoJets.hecf>0.5&&AntiKt4EMTopoJets.hecq>0.5&&AntiKt4EMTopoJets.AverageLArQF/65535>0.8&&AntiKt4EMTopoJets.pt>20000)";
const TCut cut_jetCln_noise1  ="Sum$(TMath::Abs(AntiKt4EMTopoJets.negE)>60000&&AntiKt4EMTopoJets.pt>20000)";
const TCut cut_jetCln_noise2 = "Sum$(AntiKt4EMTopoJets.emf>0.95&&AntiKt4EMTopoJets.larq>0.8&&AntiKt4EMTopoJets.AverageLArQF/65535>0.8&&TMath::Abs(AntiKt4EMTopoJets.eta)<2.8&&AntiKt4EMTopoJets.pt>20000)";
const TCut cut_jetCln_noise = cut_jetCln_noise0 || cut_jetCln_noise1 || cut_jetCln_noise2;

const TCut cut_MuonSegments_MdtI = "MuonSegments.chamberIndex==7||MuonSegments.chamberIndex==8";
const TCut cut_NCB_MuonSegments_CSC = "NCB_MuonSegments.chamberIndex==15||NCB_MuonSegments.chamberIndex==16";
const TCut cut_deltaThetaMdtI = "TMath::Abs(MuonSegments.thetaPos-MuonSegments.thetaDir) > 10*3.1415/180";
const TCut cut_deltaThetaCSC = "TMath::Abs(NCB_MuonSegments.thetaPos-NCB_MuonSegments.thetaDir) > 5*3.1415/180";

const TCut cut_GRL = "passGRL";
const TCut cut_stableBeams = "StableBeams";
const TCut cut_NPV = "NPV==0";
const TCut cut_trigger = "L1_J12_UNPAIRED_ISO || L1_J12_UNPAIRED_NONISO";
const TCut cut_eventFlag = "larError==0 && tileError==0";

const vector<TCut> cutlist_sample_beamhalo = { 
  /*cut_GRL+cut_eventFlag+*/cut_stableBeams+cut_NPV/*+cut_trigger*/+TCut("BunchStructure.isUnpairedIsolated&&BunchStructure.isUnpairedB1")+TCut("AntiKt4EMTopoJets.pt[0]>20000") + TCut("TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0") + !cut_jetCln_noise,

  /*cut_GRL+cut_eventFlag+*/cut_stableBeams+cut_NPV/*+cut_trigger*/+TCut("BunchStructure.isUnpairedIsolated&&BunchStructure.isUnpairedB2")+TCut("AntiKt4EMTopoJets.pt[0]>20000") + TCut("TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0") + !cut_jetCln_noise,

  /*cut_GRL+cut_eventFlag+*/cut_stableBeams+cut_NPV/*+cut_trigger*/+TCut("BunchStructure.isUnpairedIsolated")+TCut("AntiKt4EMTopoJets.pt[0]>20000") + TCut("TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0") + !cut_jetCln_noise
};

const vector<TCut> cutlist_sample_collisions = { 
  /*cut_GRL+cut_eventFlag+*/cut_stableBeams+"L1_J12"+TCut("AntiKt4EMTopoJets.pt[0]>20000") + TCut("TMath::Abs(AntiKt4EMTopoJets.eta[0])<3.0") + !cut_jetCln_noise
};

const vector<TString> jet_triggers = {"L1_J12_UNPAIRED_NONISO", "L1_J12_UNPAIRED_ISO"};

TString cutTitleToDirName(TString cut) {
  cut.ReplaceAll(cut_jetCln_noise.GetTitle(), "jetCleaning_Noise");
  cut.ReplaceAll("/","over").ReplaceAll("TMath::","");
  return cut;
}

void addHisto(TTree* tree, TString var, TString histdef, TCut cut, TString opts = "") {
  cout << "addHisto: " << var << " " << histdef << " " << cut.GetTitle() << endl;
  tree->Draw(var + ">>h(" + histdef + ")",cut,"goff " + opts);
  TH1F* h = (TH1F*)gDirectory->Get("h");
  if (!h) throw "histogram pointer is null";
  h->Write((var + "_" + histdef).ReplaceAll("/","over"));
  delete h;
}

int main(int argc, char** argv) {

  TString ds="";
  TString outDir="";
  for (int i=1;i<argc-1;i++) {
    if (TString(argv[i]) == "--inDS")
      ds = argv[i+1];
    if (TString(argv[i]) == "--outDir")
      outDir=argv[i+1];   
  }
  if (ds=="" || outDir=="") {
    cout << "Syntax is: " << argv[0] << " --inDS <ntuple file> --outDir <output directory>" << endl;
    return 1;
  }

  cout << "ds=" << ds << endl;
  cout << "outDir=" << outDir << endl;

  try {
    TString projectTag;
    TString runNumberStr;
    TString period;
    TString stream;
    TString version;
    getDatasetInfo(ds, projectTag, runNumberStr, period, stream, version);
    cout << "projectTag=" << projectTag << " RunNumber=" << runNumberStr << " period=" << period << " stream=" << stream << " version=" << version << endl;
    
    TFile* f = TFile::Open(ds);
    if (!f) throw "input root file pointer is null.";
    if (!f->IsOpen()) throw "failed to open root file";
    TTree* tree = (TTree*)f->Get("CollectionTree");
    if (!tree) throw "CollectionTree pointer is null";
    cout << "CollectionTree entries=" << tree->GetEntries() << endl;
      
    TFile* fout = TFile::Open(outDir + "/" + projectTag + "." +runNumberStr + "." + period+ "." + stream + ".HIST-FAST.root","recreate");
    if (!fout) throw "failed to create output root file";
    if (!fout->IsOpen()) "failed to create output root file";   

    vector<TCut> cutlist_sample;
    if (stream=="physics_Background") cutlist_sample = cutlist_sample_beamhalo;
    else if (stream="physics_Main") cutlist_sample = cutlist_sample_collisions;
 
    for (TCut cut_sample : cutlist_sample ) {
      TString dir0=cut_sample.GetTitle();
      dir0=cutTitleToDirName(dir0);
      fout->mkdir(dir0);
      if (!fout->cd(dir0)) throw "failed to cd";

      if (doEvent) {
        addHisto(tree, "lbn","10000,0,10000", cut_sample);
        addHisto(tree, "averageInteractionsPerCrossing","100,0,100", cut_sample);
        addHisto(tree, "NPV","100,0,100", cut_sample);
        addHisto(tree, "bcid", "3564,1,3565", cut_sample);
      }

      if (doJets) {
        addHisto(tree, "AntiKt4EMTopoJets.n", "50,0,50", cut_sample);
        addHisto(tree, "AntiKt4EMTopoJets.pt[0]/1000", "10000,0,10000", cut_sample);
        addHisto(tree, "AntiKt4EMTopoJets.eta[0]", "100,-5,5", cut_sample);
        addHisto(tree, "AntiKt4EMTopoJets.phi[0]", "100,-3.1415,3.1415", cut_sample); 
        addHisto(tree, "AntiKt4EMTopoJets.phi[0]:AntiKt4EMTopoJets.eta[0]","200,-5,5,100,-3.1415,3.1415", cut_sample, "colz");

        addHisto(tree, "AntiKt4EMTopoJets.time[0]","500,-100,100", cut_sample);
        addHisto(tree, "AntiKt4EMTopoJets.time[0]:AntiKt4EMTopoJets.eta[0]","200,-5,5,500,-50,50", cut_sample, "colz");
        //addHisto(tree, "AntiKt4EMTopoJets.emf[0]", "100,0,1", cut_sample);
        //addHisto(tree, "AntiKt4EMTopoJets.hecf[0]", "100,0,1", cut_sample);
        //addHisto(tree, "AntiKt4EMTopoJets.negE[0]/1000", "100,-200,10", cut_sample);
        //addHisto(tree, "AntiKt4EMTopoJets.fracSamplingMax[0]", "100,0,1", cut_sample);
        //addHisto(tree, "AntiKt4EMTopoJets.fracSamplingMaxIndex[0]", "24,0,24", cut_sample);
        //addHisto(tree, "AntiKt4EMTopoJets.hecq[0]","100,-0.5,1.5", cut_sample);
        //addHisto(tree, "AntiKt4EMTopoJets.larq[0]","100,-0.5,1.5", cut_sample);
        //addHisto(tree, "AntiKt4EMTopoJets.AverageLArQF[0]/65535","100,-0.5,1.5", cut_sample);
        //addHisto(tree, "AntiKt4EMTopoJets.NumTrkPt1000[0]", "50,0,50", cut_sample);
        //addHisto(tree, "AntiKt4EMTopoJets.SumPtTrkPt500[0]/AntiKt4EMTopoJets.pt[0]", "100,-0.5,1.5",cut_sample+TCut("AntiKt4EMTopoJets.n"));

      }

      if (doTopoClusters) {
        addHisto(tree, "TopoClusters.n","100,0,100",cut_sample);
        addHisto(tree, "TopoClusters.pt[0]/1000","10000,0,10000",cut_sample);
        addHisto(tree, "TopoClusters.e[0]/1000","10000,0,10000",cut_sample);
        addHisto(tree, "TopoClusters.emscale_e[0]/1000","2000,0,2000",cut_sample);
        addHisto(tree, "TopoClusters.eta[0]","100,-5,5",cut_sample);
        addHisto(tree, "TopoClusters.phi[0]","100,-3.1415,3.1415",cut_sample);
        addHisto(tree, "TopoClusters.rPerp[0]", "1000,0,5000",cut_sample);
        addHisto(tree, "TopoClusters.z[0]","1000,-7000,7000",cut_sample);
        addHisto(tree, "TopoClusters.time[0]","200,-100,100",cut_sample);
        addHisto(tree, "TopoClusters.time[0]:TopoClusters.eta[0]","200,-5,5,200,-50,50", cut_sample, "colz");
        addHisto(tree, "TopoClusters.fracSamplingMax[0]","100,0,100",cut_sample);
        addHisto(tree, "TopoClusters.fracSamplingMaxIndex[0]","24,0,24",cut_sample);
      }
    
      if (doMuonSegments) { 

        vector<TCut> cutlist_MuonSegments = { cut_MuonSegments_MdtI, cut_MuonSegments_MdtI + cut_deltaThetaMdtI};

        for (TCut cut_MuonSegments : cutlist_MuonSegments) {
          TString dir1 = TString("MuonSegments.") + TString(cut_MuonSegments.GetTitle());
          dir1=cutTitleToDirName(dir1);
          cout << dir1 << endl;
          fout->mkdir(dir0 + "/" + dir1);
          if (!fout->cd(dir0 + "/" + dir1)) throw "failed to cd";  
          gDirectory->pwd();
          addHisto(tree, "MuonSegments.x","200,-5000,5000", cut_sample + cut_MuonSegments);  
          addHisto(tree, "MuonSegments.y:MuonSegments.x","200,-5000,5000,200,-5000,5000", cut_sample + cut_MuonSegments,"colz");
          addHisto(tree, "MuonSegments.z","200,-15000,15000", cut_sample + cut_MuonSegments);
          addHisto(tree, "MuonSegments.thetaPos*180/3.1415","360,0,180",cut_sample + cut_MuonSegments);
          addHisto(tree, "MuonSegments.thetaDir*180/3.1415","360,0,180",cut_sample + cut_MuonSegments);
          addHisto(tree, "TMath::Abs(MuonSegments.thetaPos-MuonSegments.thetaDir)*180/3.1415","360,0,180",cut_sample + cut_MuonSegments);
          addHisto(tree, "MuonSegments.phi","100,-3.1415,3.1415",cut_sample + cut_MuonSegments);
          addHisto(tree, "MuonSegments.t0","4000,-2000,2000", cut_sample + cut_MuonSegments);
          addHisto(tree, "MuonSegments.t0","1000,-1000,110000", cut_sample + cut_MuonSegments);
          addHisto(tree, "MuonSegments.t0error","1000,0,1000", cut_sample + cut_MuonSegments);
          addHisto(tree, "MuonSegments.t0error","1000,0,110000", cut_sample + cut_MuonSegments);
          addHisto(tree, "MuonSegments.chamberIndex","17,0,17", cut_sample + cut_MuonSegments);
          addHisto(tree, "MuonSegments.sector","16,0,16", cut_sample + cut_MuonSegments);
          addHisto(tree, "MuonSegments.nPrecisionHits","20,0,20", cut_sample + cut_MuonSegments);
          fout->cd("..");
        }

	vector<TCut> cutlist_NCB_MuonSegments = { cut_NCB_MuonSegments_CSC, cut_NCB_MuonSegments_CSC + cut_deltaThetaCSC};

        for (TCut cut_NCB_MuonSegments : cutlist_NCB_MuonSegments) {
          TString dir1 = TString("NCB_MuonSegments.") + TString(cut_NCB_MuonSegments.GetTitle());
          dir1=cutTitleToDirName(dir1);
          fout->mkdir(dir0 + "/" + dir1);
          if (!fout->cd(dir0 + "/" + dir1)) throw "failed to cd";
          gDirectory->pwd(); 
          addHisto(tree, "NCB_MuonSegments.y:NCB_MuonSegments.x","200,-5000,5000,200,-5000,5000", cut_sample + cut_NCB_MuonSegments,"colz");
          addHisto(tree, "NCB_MuonSegments.z","200,-15000,15000", cut_sample + cut_NCB_MuonSegments);
          addHisto(tree, "NCB_MuonSegments.thetaPos*180/3.1415","360,0,180",cut_sample + cut_NCB_MuonSegments);
          addHisto(tree, "NCB_MuonSegments.thetaDir*180/3.1415","360,0,180",cut_sample + cut_NCB_MuonSegments);
          addHisto(tree, "TMath::Abs(NCB_MuonSegments.thetaPos-NCB_MuonSegments.thetaDir)*180/3.1415","360,0,180",cut_sample + cut_NCB_MuonSegments);
          addHisto(tree, "NCB_MuonSegments.phi","100,-3.1415,3.1415",cut_sample + cut_NCB_MuonSegments);
          addHisto(tree, "NCB_MuonSegments.t0","4000,-2000,2000",cut_sample + cut_NCB_MuonSegments);
          addHisto(tree, "NCB_MuonSegments.t0","1000,-100,110000", cut_sample + cut_NCB_MuonSegments);   // tails at t0=99999
          addHisto(tree, "NCB_MuonSegments.t0error","1000,0,110000", cut_sample + cut_NCB_MuonSegments); // tails at t0error=99999
          addHisto(tree, "NCB_MuonSegments.chamberIndex","17,0,17", cut_sample + cut_NCB_MuonSegments);
          addHisto(tree, "NCB_MuonSegments.sector","16,0,16",cut_sample + cut_NCB_MuonSegments);
          addHisto(tree, "NCB_MuonSegments.nPrecisionHits","20,0,20",cut_sample + cut_NCB_MuonSegments);
          fout->cd("..");
        }
      }

      fout->cd("..");
    }

    fout->Close();

  } catch (const char* err) {
    cout << err << endl;
  }
  cout<<"Done."<<endl;
  return 0;
}
