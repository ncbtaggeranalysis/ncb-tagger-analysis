#ifndef I_BEAM_INTENSITY_TOOL_H
#define I_BEAM_INTENSITY_TOOL_H

#include "GaudiKernel/IAlgTool.h"

class IBeamIntensityTool : virtual public IAlgTool {
public:

  virtual void config(int runNumber) = 0;
  virtual bool configIsLoaded(int runNumber) = 0;

  virtual float getIntensityBeam1(int lb) = 0;
  virtual float getIntensityBeam2(int lb) = 0;

  virtual float getLBduration(int lb1, int lb2) = 0;
  virtual int   getRunDuration() = 0; 

  virtual void  print() = 0;

};

#endif
