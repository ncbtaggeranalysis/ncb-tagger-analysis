
#ifndef _H_BEAM_INTENSITY_TOOL
#define _H_BEAM_INTENSITY_TOOL

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <TString.h>
#include <TDatime.h>

#include <PathResolver/PathResolver.h>
#include <AthenaBaseComps/AthAlgTool.h>
//#include <NCBanalysis/IBeamIntensityTool.h>

//#include <time.h>

using namespace std;

class BeamIntensityTool : /*virtual public IBeamIntensityTool,*/ public AthAlgTool {

  public:

    BeamIntensityTool(const string& type, const std::string& name, const IInterface* parent);

    void config(int runNumber);
    bool configIsLoaded(int runNumber);

    float getIntensityBeam1(int lb);
    float getIntensityBeam2(int lb);

    float getLBduration(int lb1, int lb2);
    int   getRunDuration();

    void  print();

  private:

    int m_runNumber;

    TString m_beginOfRunDate;
    TString m_endOfRunDate;

    vector<int> m_LBvec;
    vector<TString> m_startTimeVec;
    vector<float> m_durationVec;
    vector<float> m_energyVec;
    vector<float> m_intensityBeam1Vec;
    vector<float> m_intensityBeam2Vec;

    int getMonthNum(string month);

};

#endif
