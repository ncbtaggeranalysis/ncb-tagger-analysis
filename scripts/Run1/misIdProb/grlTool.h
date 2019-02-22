#ifndef GRL_TOOL_H
#define GRL_TOOL_H

using namespace std;
#include <vector>
#include <map>
#include <utility>

#include <TString.h>

class GrlTool {

  public:
   
         GrlTool() {};
    void Load(vector<TString> grlVec);
    bool passesRunLB(int runNumber, int lb);

  private:
    map<int, vector<pair<int,int>>> m_run_LBrange;
};

#endif
