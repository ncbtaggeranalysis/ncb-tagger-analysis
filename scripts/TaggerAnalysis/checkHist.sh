XRANGE=""

#HIST="NPV==0&&passGRL&&StableBeams&&BunchStructure.isUnpairedIsolated&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0/SelectedMuonSegments_xy_CSC_SideC"
#OPTS=colz

#HIST="NPV==0&&StableBeams&&BunchStructure.isUnpairedIsolated&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0/MuonSegments_deltaTheta_MdtI"
#OPTS=hist

HIST="NPV==0&&StableBeams&&BunchStructure.isUnpairedIsolated&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0/SelectedMuonSegments_t0_MdtI"
OPTS=hist
XRANGE="--xmin -100 --xmax 100"

./checkHist --inDir HIST/v181121 --hist $HIST --pdfFile plots/checkHist.$(basename $HIST).pdf --opts $OPTS $XRANGE 
