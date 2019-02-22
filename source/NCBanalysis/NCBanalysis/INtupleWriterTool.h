#ifndef I_NTUPLE_WRITER_TOOL_H
#define I_NTUPLE_WRITER_TOOL_H

#include "GaudiKernel/IAlgTool.h"

class INtupleWriterTool : virtual public IAlgTool {
public:

  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;
  virtual StatusCode Fill() = 0;
  virtual StatusCode Write() = 0;

};

#endif
