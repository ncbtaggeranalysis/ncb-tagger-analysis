default: main fast checkHist

main: taggerAnalysis.cxx
	g++ -o taggerAnalysis taggerAnalysis.cxx taggerAnalysisLib.cxx $(shell root-config --cflags --libs) 

fast: taggerAnalysisFast.cxx
	g++ -o taggerAnalysisFast taggerAnalysisFast.cxx taggerAnalysisLib.cxx $(shell root-config --cflags --libs)

checkLArTileError: checkLArTileError.cxx
	g++ -o checkLArTileError checkLArTileError.cxx taggerAnalysisLib.cxx $(shell root-config --cflags --libs)

checkHist: checkHist.cxx
	g++ -o checkHist checkHist.cxx taggerAnalysisLib.cxx $(shell root-config --cflags --libs)  
