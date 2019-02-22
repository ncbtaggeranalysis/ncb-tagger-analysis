default: misIdProbPlots misIdProbBCID eventDisplay segmentACNoTimeRndmModel_main 

misIdProbPlots: misIdProbPlots.cxx ../LuminosityTool/LuminosityTool.cxx
	g++ -o misIdProbPlots misIdProbPlots.cxx ../TaggerAnalysis/taggerAnalysisLib.cxx -I../ ../LuminosityTool/LuminosityTool.cxx $(shell root-config --cflags --libs)

misIdProbBCID: misIdProbBCID.cxx
	g++ -o misIdProbBCID misIdProbBCID.cxx ../TaggerAnalysis/taggerAnalysisLib.cxx -I../ $(shell root-config --cflags --libs) 

eventDisplay: eventDisplay.cxx
	g++ -o eventDisplay eventDisplay.cxx $(shell root-config --cflags --libs)

segmentACNoTimeRndmModel_main: segmentACNoTimeRndmModel_main.cxx
	g++ -o segmentACNoTimeRndmModel_main segmentACNoTimeRndmModel_main.cxx segmentACNoTimeRndmModel.cxx $(shell root-config --cflags --libs)

