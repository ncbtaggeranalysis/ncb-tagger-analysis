#ifndef RUN_QUERY_PICKLE_TOOL_H
#define RUN_QUERY_PICKLE_TOOL_H

#include <string>
using namespace std;
#include <vector>
#include <fstream>

#include <TString.h>
#include <TFile.h>
#include <TTree.h>

#include <AthenaBaseComps/AthAlgTool.h>
#include <PathResolver/PathResolver.h>

class RunQueryPickleTool : public AthAlgTool {

  public:

         RunQueryPickleTool(const string& type, const std::string& name, const IInterface* parent);
        ~RunQueryPickleTool();
    
    StatusCode initialize();
    StatusCode configure();

    unsigned int runNumber() {return m_runNumber;};
    void setRunNumber(unsigned int runNumber) {m_runNumber=runNumber;};

    bool isStableBeams(unsigned int lbn);

  private:

    unsigned int m_runNumber;

    vector<unsigned int>* m_stablebeams_firstlb;
    vector<unsigned int>* m_stablebeams_lastlb;
    vector<bool>* m_stablebeams_value;

    TFile* m_configfile;
    TTree* m_configtree;
};

#endif
