#include <iostream>
using namespace std;
#include <fstream>

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include "taggerAnalysisLib.h"

int main(int argc, char** argv) {
  
  if (argc!=2) {
    cout<<"Syntax is: " << argv[0] << " <ntuple list file>" << endl;
    return 1;
  }

  fstream ftxt(argv[1], fstream::in);
  if (!ftxt.is_open()) { cout <<"failed to open txt file" << endl; return 1;}

  TString ntuplefile;
  while (ftxt >> ntuplefile) {

    TString projectTag;
    TString runNumberStr;
    TString period;
    TString stream;
    TString version;
    getDatasetInfo(ntuplefile, projectTag, runNumberStr, period, stream, version);

    //cout << ntuplefile << endl;
    TFile* f  = TFile::Open(ntuplefile);
    if (!f) { cout<<"failed to open root file" << endl; return 1;}
    if (!f->IsOpen()) { cout<<"failed to open root file" << endl; return 1;}

    TTree* tree=(TTree*)f->Get("CollectionTree");
    if (!tree) { cout <<"failed to read CollctionTree"<<endl;return 1;}
    cout<<projectTag << " " << period << " " << runNumberStr << " " 
        <<Form("%10i%10i%10i",
               tree->GetEntries(),
               tree->GetEntries("larError"),
               tree->GetEntries("tileError"))
        <<endl;
    //break;
  }

  ftxt.close();
  return 0;
}
