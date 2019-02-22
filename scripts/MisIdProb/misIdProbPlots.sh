# data
#inputEfficiencyFastDir=../EfficiencyFast/HIST/190211.dataRun2_13TeV.physics_Main
#inputEfficiencyAnalysisDir=../EfficiencyAnalysis/HIST/190211.dataRun2_13TeV.physics_Main
#inputTxtNtuple=../inputTxtNtuple/190211/dataRun2_13TeV.physics_Main.txt
#config=noSelection

# MC 
inputEfficiencyFastDir=../EfficiencyFast/HIST/190211.mc16_13TeV.jetjet
inputEfficiencyAnalysisDir=../EfficiencyAnalysis/HIST/190211.mc16_13TeV.jetjet
inputTxtNtuple=../inputTxtNtuple/190211/mc16_13TeV.jetjet.txt
config=noSelection

minDijetEvents=-1
#minDijetEvents=1000000

#-------------------------------------------------------------------------------------

outputPDFFile="OUTPUT/misIdProbPlots.$(basename $inputEfficiencyFastDir).$config.minDijetEvents$minDijetEvents.pdf"

./misIdProbPlots \
--inputEfficiencyFastDir $inputEfficiencyFastDir \
--inputEfficiencyAnalysisDir $inputEfficiencyAnalysisDir \
--inputTxtNtuple $inputTxtNtuple \
--config $config \
--outputPDFFile $outputPDFFile \
--minDijetEvents $minDijetEvents
