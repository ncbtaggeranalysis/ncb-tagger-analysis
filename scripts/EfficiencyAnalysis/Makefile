default: effAnalysis effTable effPlots 

effAnalysis: effAnalysis.cxx
	g++ -o effAnalysis effAnalysis.cxx $(shell root-config --cflags --libs) ../TaggerAnalysis/taggerAnalysisLib.cxx ../LuminosityTool/LuminosityTool.cxx -I ../

effTable: effTable.cxx
	g++ -o effTable effTable.cxx $(shell root-config --cflags --libs) ../TaggerAnalysis/taggerAnalysisLib.cxx -I ../TaggerAnalysis

effPlots: effPlots.cxx
	g++ -o effPlots effPlots.cxx $(shell root-config --cflags --libs) ../TaggerAnalysis/taggerAnalysisLib.cxx -I ../TaggerAnalysis

