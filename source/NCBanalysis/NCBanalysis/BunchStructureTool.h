
#ifndef H_BUNCH_TOOL
#define H_BUNCH_TOOL

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <TString.h>

#include "PathResolver/PathResolver.h"

#include <AthenaBaseComps/AthAlgTool.h>
//#include <NCBanalysis/IBunchStructureTool.h>

using namespace std;

class BunchStructureTool : /*virtual public IBunchStructureTool, */public AthAlgTool {
  public:
            BunchStructureTool(const std::string& type, const std::string& name, const IInterface* parent);

    bool isColliding(int bcid, int lb);
    bool isCollidingFirst(int bcid, int lb);
    bool isUnpaired(int bcid, int lb);
    bool isUnpairedBeam1(int bcid, int lb);
    bool isUnpairedBeam2(int bcid, int lb);
    bool isEmpty(int bcid, int lb);
    bool isUnpairedFirst(int bcid, int lb);
    bool isUnpairedLast(int bcid, int lb); 
    bool isUnpairedIsolated(int bcid, int lb);
    bool isFilled(int bcid, int lb);

    int numBunchesColliding(int lb);
    int numBunchesUnpairedBeam1(int lb);
    int numBunchesUnpairedBeam2(int lb);

    bool isStableBeam(int lb);

    void config(int runNumber);
    void print();

  private:
    
    int runNumber;
    int lb;

    vector<int> lb_minVec;
    vector<int> lb_maxVec;
    vector<vector<int>> paired_bunchesVec;
    vector<vector<int>> unpaired_beam1Vec;
    vector<vector<int>> unpaired_beam2Vec;

    vector<bool> m_stableBeams;

    bool isEmptyColumn(string word);

    bool m_debug;
};

#endif
