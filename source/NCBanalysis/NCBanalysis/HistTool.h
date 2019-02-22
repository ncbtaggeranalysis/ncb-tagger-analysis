#ifndef HIST_TOOL_H
#define HIST_TOOL_H

#include <string>
using namespace std;
#include <vector>

#include <TH1F.h>
#include <TVector3.h>

#include <AthenaBaseComps/AthAlgTool.h>
#include "GaudiKernel/ITHistSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "NCBanalysis/IHistTool.h"

class HistTool : virtual public IHistTool, public AthAlgTool {

  public:

         HistTool(const string& type, const std::string& name, const IInterface* parent);
        ~HistTool();
    
    StatusCode initialize();
    StatusCode Fill();

  private:
    TString m_name;
    string m_outputStream; 
    unsigned int m_nMonitoredJets;
    string m_jetCollection;
 
    vector<TH1F*> m_hist;
    TH1F* getHist(TString name);

    ITHistSvc* m_thistSvc;

};

#endif
