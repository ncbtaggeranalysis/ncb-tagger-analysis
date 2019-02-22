#ifndef TAGGER_ANALYSIS_H
#define TAGGER_ANALYSIS_H

#include <iostream>
using namespace std;
#include <string>
#include <algorithm>
#include <utility>

#include <TString.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TList.h>
#include <TMath.h>
#include <TEfficiency.h>

namespace TaggerAnalysisLib {

  pair<TString,TString> getDatasetProjectTagAndPeriod(int runNumber);

  void getDatasetInfo(TString filename, 
                      TString& projectTag, 
                      int& runNumber, 
                      TString& period, 
                      TString& stream);

  int getDatasetRunNumber(TString ds); 

  vector<TString> getListOfFilesInDirectory(TString dir, TString pattern);

pair<float,float> getEfficiency(int nsample, int ntagged);

}

#endif
