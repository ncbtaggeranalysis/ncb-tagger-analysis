TString outputPDFFile = "compRun1vsRun2.pdf"; 

template <class T>
void drawPlot(T* tree,
              TString def, 
              TCut cut,
              TString opt) {

  TCanvas canvas;
  TH1F* h;
  tree->Draw(def,cut);
  h = (TH1F*)gDirectory->Get("h");
  h->Draw(opt);
  canvas.Print(outputPDFFile);
}

void compRun1vsRun2() {

  TChain chainRun1("background");
  chainRun1.Add("$STORAGE/data11_7TeV.00191933.physics_Background.merge.NTUP_BKGD.f414_p1196/*.root*");

  TFile* fRun2 = TFile::Open("$STORAGE/NCB/NCBTaggerAnalysis/181121/NtupleWriter/physics_Background/data18_13TeV/periodC/user.olariu.data18_13TeV.00350220.physics_Background.merge.DAOD_IDNCB.f934_m1963.181129.2012_NtupleWriter/NtupleWriter.merge.root");
  TTree* treeRun2 = (TTree*)fRun2->Get("CollectionTree");

  TString histdefRz_Run1 = "TMath::Sqrt(mooreseg_x*mooreseg_x+mooreseg_y*mooreseg_y):"
                                       "mooreseg_z>>h(200,-16000,16000,200,0,12000)";
  TString histdefRz_Run2 = "TMath::Sqrt(MuonSegments.x*MuonSegments.x+"
                                       "MuonSegments.y*MuonSegments.y):"
                                       "MuonSegments.z>>h(200,-16000,16000,200,0,12000)"; 

  TCut cut_unpaired_Run1 = "L1_J10_UNPAIRED_ISO||L1_J10_UNPAIRED_NONISO";
  TCut cut_unpaired_Run2 = "L1_J12_UNPAIRED_ISO||L1_J12_UNPAIRED_NONISO";

  TCanvas canvas;
  canvas.Print(outputPDFFile+"(");

  // ------------------------------------------------------------------------------ //
  //    MDT Inner Endcap
  // ------------------------------------------------------------------------------ //

  // Run1 stationName=15  =>  Run2 chamberIndex=8
  drawPlot<TChain>(&chainRun1, histdefRz_Run1, "mooreseg_stationName==15","colz");
  drawPlot<TTree>(treeRun2, histdefRz_Run2, "MuonSegments.chamberIndex==8","colz");
 
  // Run1 stationName=24  =>  Run2 chamberIndex=7          
  drawPlot<TChain>(&chainRun1, histdefRz_Run1, "mooreseg_stationName==24","colz");
  drawPlot<TTree>(treeRun2, histdefRz_Run2, "MuonSegments.chamberIndex==7","colz");

  // Run1 vs Run2  Number of segments per event
  drawPlot<TChain>(&chainRun1, "Sum$(mooreseg_stationName==15)>>h(10,0,10)",
                               cut_unpaired_Run1,"hist");
  drawPlot<TTree>(treeRun2, "Sum$(MuonSegments.chamberIndex==8)>>h(10,0,10)",
                             cut_unpaired_Run2,"hist");

  drawPlot<TChain>(&chainRun1, "Sum$(mooreseg_stationName==24)>>h(10,0,10)",
                               cut_unpaired_Run1,"hist");
  drawPlot<TTree>(treeRun2, "Sum$(MuonSegments.chamberIndex==7)>>h(10,0,10)",
                             cut_unpaired_Run2,"hist");

  // ----------------------------------------------------------------------------- //
  //    CSC
  // ----------------------------------------------------------------------------- //
  histdefRz_Run2.ReplaceAll("MuonSegments","NCB_MuonSegments");

  drawPlot<TChain>(&chainRun1, histdefRz_Run1, "mooreseg_stationName==33","colz");
  drawPlot<TTree>(treeRun2, histdefRz_Run2, "NCB_MuonSegments.chamberIndex==15", "colz");

  drawPlot<TChain>(&chainRun1, histdefRz_Run1, "mooreseg_stationName==34","colz");
  drawPlot<TTree>(treeRun2, histdefRz_Run2, "NCB_MuonSegments.chamberIndex==16", "colz");

  // Run1 vs Run2  Number of segments per event
  // CSC Small
  drawPlot<TChain>(&chainRun1, "Sum$(mooreseg_stationName==33)>>h(10,0,10)",
                               cut_unpaired_Run1,"hist");
  drawPlot<TTree>(treeRun2, "Sum$(NCB_MuonSegments.chamberIndex==15)>>h(10,0,10)",
                             cut_unpaired_Run2,"hist");

  // CSC Large
  drawPlot<TChain>(&chainRun1, "Sum$(mooreseg_stationName==34)>>h(10,0,10)",
                               cut_unpaired_Run1,"hist");
  drawPlot<TTree>(treeRun2, "Sum$(NCB_MuonSegments.chamberIndex==16)>>h(10,0,10)",
                             cut_unpaired_Run2,"hist");

  canvas.Print(outputPDFFile+")");

}
