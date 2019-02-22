taggingMethod=SegmentACNoTime
#taggingMethod=OneSidedTight
#taggingMethod=TwoSided

#ntupleFile=$STORAGE/NCB/NCBTaggerAnalysis/190131/user.olariu.data18_13TeV.00348354.physics_Main.merge.AOD.f920_m1947.190131.1752_MisIdProb/NtupleWriter.merge.root
ntupleFile=$STORAGE/NCB/NCBTaggerAnalysis/190131/user.olariu.data18_13TeV.00348251.physics_Main.merge.AOD.f920_m1947.190131.1752_MisIdProb/NtupleWriter.merge.root

./eventDisplay \
--ntupleFile $ntupleFile \
--taggingMethod $taggingMethod \
--maxEvents 10 
#--onlyTaggedEvents 
