default: efficiencyFast efficiencyTable efficiencyVsRunNumber

efficiencyFast: efficiencyFast.cxx
	g++ -o efficiencyFast efficiencyFast.cxx $(shell root-config --cflags --libs) -I../ ../TaggerAnalysis/taggerAnalysisLib.cxx

efficiencyTable: efficiencyTable.cxx
	g++ -o efficiencyTable efficiencyTable.cxx $(shell root-config --cflags --libs) ../TaggerAnalysis/taggerAnalysisLib.cxx -I ../TaggerAnalysis/ 

efficiencyVsRunNumber: efficiencyVsRunNumber.cxx
	g++ -o efficiencyVsRunNumber efficiencyVsRunNumber.cxx $(shell root-config --cflags --libs) -I../TaggerAnalysis ../TaggerAnalysis/taggerAnalysisLib.cxx
