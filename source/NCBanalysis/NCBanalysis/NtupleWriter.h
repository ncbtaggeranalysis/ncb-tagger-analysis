#ifndef NTUPLE_WRITER_H
#define NTUPLE_WRITER_H

using namespace std;
#include <string>

#include <AthenaBaseComps/AthAlgorithm.h>

#include "NCBanalysis/NtupleWriterTool.h"

class NtupleWriter : public AthAlgorithm {

  public:

    NtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);
    ~NtupleWriter();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    ToolHandle<NtupleWriterTool> m_ntupleWriterTool;

};

#endif
