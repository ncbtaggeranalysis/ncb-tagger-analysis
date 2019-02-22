#ifndef I_BUNCH_STRUCTURE_TOOL_H
#define I_BUNCH_STRUCTURE_TOOL_H

#include "GaudiKernel/IAlgTool.h"

class IBunchStructureTool : virtual public IAlgTool {
public:

  virtual bool isPaired(int bcid, int lb) = 0;
  virtual bool isUnpaired(int bcid, int lb) = 0;
  virtual bool isUnpairedBeam1(int bcid, int lb) = 0;
  virtual bool isUnpairedBeam2(int bcid, int lb) = 0;
  virtual bool isUnpairedFirst(int bcid, int lb) = 0;
  virtual bool isUnpairedLast(int bcid, int lb) = 0; 
  virtual bool isUnpairedIsolated(int bcid, int lb) = 0;
  virtual bool isFilled(int bcid, int lb) = 0;
  virtual bool isEmpty(int bcid, int lb) = 0;

  virtual int numBunchesColliding(int lb) = 0;
  virtual int numBunchesUnpairedBeam1(int lb) = 0;
  virtual int numBunchesUnpairedBeam2(int lb) = 0;
  virtual bool isStableBeam(int lb) = 0;

  virtual void config(int runNumber) = 0;
  virtual void print() = 0;

};

#endif
