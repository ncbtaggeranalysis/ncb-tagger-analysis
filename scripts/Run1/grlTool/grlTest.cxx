#include <iostream>
using namespace std;

#include <TString.h>

#include "grlTool.h"

int main() {

  try {

    GrlTool grlTool;
    vector<TString> grlVec = {
      "../data/GRL/data11_7TeV.periodAllYear_DetStatus-v36-pro10-02_CoolRunQuery-00-04-08_All_Good.xml"
    };
    grlTool.Load(grlVec);

    cout << grlTool.passesRunLB(191933, 110)<<endl;

  }
  catch (const char* err) { cout<<err<<endl;}
  catch (TString err) {cout<<err<<endl;} 

  return 0;

}
