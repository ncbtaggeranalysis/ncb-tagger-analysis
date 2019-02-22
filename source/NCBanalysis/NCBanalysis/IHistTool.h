#ifndef I_HIST_TOOL_H
#define I_HIST_TOOL_H

#include "GaudiKernel/IAlgTool.h"

class IHistTool : virtual public IAlgTool {

  public:

    virtual StatusCode initialize() = 0;
    virtual StatusCode Fill() = 0;

};

#endif
