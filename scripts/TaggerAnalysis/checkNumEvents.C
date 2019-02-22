{

vector<vector<TString>> samples_Run2 = {
{
"data16_13TeV.periodA.physics_Background.merge.HIST.root",
"data16_13TeV.periodB.physics_Background.merge.HIST.root",
"data16_13TeV.periodC.physics_Background.merge.HIST.root",
"data16_13TeV.periodD.physics_Background.merge.HIST.root",
"data16_13TeV.periodE.physics_Background.merge.HIST.root",
"data16_13TeV.periodF.physics_Background.merge.HIST.root",
"data16_13TeV.periodG.physics_Background.merge.HIST.root",
"data16_13TeV.periodI.physics_Background.merge.HIST.root",
"data16_13TeV.periodJ.physics_Background.merge.HIST.root",
"data16_13TeV.periodK.physics_Background.merge.HIST.root",
"data16_13TeV.periodL.physics_Background.merge.HIST.root"
},
{
"data17_13TeV.periodA.physics_Background.merge.HIST.root",
"data17_13TeV.periodB.physics_Background.merge.HIST.root",
"data17_13TeV.periodC.physics_Background.merge.HIST.root",
"data17_13TeV.periodD.physics_Background.merge.HIST.root",
"data17_13TeV.periodE.physics_Background.merge.HIST.root",
"data17_13TeV.periodF.physics_Background.merge.HIST.root",
"data17_13TeV.periodG.physics_Background.merge.HIST.root",
"data17_13TeV.periodH.physics_Background.merge.HIST.root",
"data17_13TeV.periodI.physics_Background.merge.HIST.root",
"data17_13TeV.periodK.physics_Background.merge.HIST.root",
"data17_13TeV.periodN.physics_Background.merge.HIST.root"
},
{
"data18_13TeV.periodA.physics_Background.merge.HIST.root",
"data18_13TeV.periodB.physics_Background.merge.HIST.root",
"data18_13TeV.periodC.physics_Background.merge.HIST.root",
"data18_13TeV.periodD.physics_Background.merge.HIST.root",
"data18_13TeV.periodE.physics_Background.merge.HIST.root",
"data18_13TeV.periodF.physics_Background.merge.HIST.root",
"data18_13TeV.periodG.physics_Background.merge.HIST.root",
"data18_13TeV.periodH.physics_Background.merge.HIST.root",
"data18_13TeV.periodI.physics_Background.merge.HIST.root",
"data18_13TeV.periodJ.physics_Background.merge.HIST.root",
"data18_13TeV.periodK.physics_Background.merge.HIST.root",
"data18_13TeV.periodL.physics_Background.merge.HIST.root",
"data18_13TeV.periodM.physics_Background.merge.HIST.root",
"data18_13TeV.periodN.physics_Background.merge.HIST.root",
"data18_13TeV.periodO.physics_Background.merge.HIST.root",
"data18_13TeV.periodQ.physics_Background.merge.HIST.root"
}
};

for (vector<TString> samples_OneYear : samples_Run2) { 

  vector<float> n[2];
  float nTotal[2] = {0,0};

  for (TString sample : samples_OneYear) {
    cout<<sample<<"  ";
    TFile* f = TFile::Open("HIST/WorkDir/merge/"+sample);

    TH1F* h1 = (TH1F*)f->Get("NPV==0&&StableBeams&&BunchStructure.isUnpairedIsolated&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0/jet0_phi");
    if (!h1) { cout << "h1 is null" << endl; return;}

    TH1F* h2 = (TH1F*)f->Get("NPV==0&&StableBeams&&BunchStructure.isUnpaired&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0/jet0_phi");
    if (!h2) { cout << "h2 is null" << endl; return;}

    cout<<Form("%10.0f",h1->GetEntries())
        <<Form("%10.0f",h2->GetEntries())<<endl;
    n[0].push_back(h1->GetEntries());
    n[1].push_back(h2->GetEntries());

    nTotal[0]+=h1->GetEntries();
    nTotal[1]+=h2->GetEntries();
  }

  cout << "total events=" 
       << Form("%10.0f", nTotal[0]) 
       << Form("%10.0f", nTotal[1])
       << endl;

  //cout << "periods BtoE=" << Form("%10.0f", n[1]+n[2]+n[3]+n[4]) << endl;

}

}
