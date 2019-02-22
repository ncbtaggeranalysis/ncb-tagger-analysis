#ifndef LUMINOSITY_TOOL
#define $LUMINOSITY_TOOL

#include <fstream>
using namespace std;
#include <iostream>

#include <TString.h>
#include <TObjArray.h>

class LuminosityTool {

  public:

         LuminosityTool();

    void config(int runNumber);
    void print();

    float instLuminosity(int lb);
    float averageInstLuminosityStableBeams();
    int   lastLB();

  private:

  int m_runNumber; 
  vector<int> m_lb;
  vector<float> m_duration;
  vector<float> m_instLuminosity;
  vector<float> m_intLuminosity;
  vector<float> m_mu;
  vector<int> m_stableBeams;

};

#endif
