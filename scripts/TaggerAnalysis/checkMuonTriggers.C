{
  TString sample="/storage/baaf/backup/olariu/NCB/NCBTaggerAnalysis/181121/NtupleWriter/physics_Main/user.olariu.data18_13TeV.00348197.physics_Main.merge.AOD.f920_m1947.181205.1541_NtupleWriter/data18_13TeV.00348197.physics_Main.root";

  TFile* f = TFile::Open(sample);
  TTree* tree = (TTree*)f->Get("CollectionTree");
  if (!tree) { cout<<"tree is null"<<endl; return;}

  vector<TString> muTriggers={
  "L1_MU10",
  "L1_MU20",
  "HLT_mu10", 
  "HLT_mu20", 
  "HLT_mu60", 
  "HLT_mu80"
  };
  cout << sample << endl;
  for (TString muTrigger : muTriggers) 
    cout << muTrigger << " " << tree->GetEntries(muTrigger) << endl;
}
