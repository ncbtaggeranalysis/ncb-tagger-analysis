#include <iostream>
using namespace std;
#include <vector>

#include "segmentACNoTimeRndmModel.h"

int main() {

int nevents = 1000;
  vector<int> nsegmentsVec = {
    //2,4,6,8,10,12,14,16,18,20
      10,20
  };
            
  for (int nsegments : nsegmentsVec) {
    int ntagged = RndmModel::numberOfTaggedEventsSegmentACNoTime(nevents, nsegments);
    cout << nevents << " " << nsegments << " tagged=" << ntagged << endl;
  }
}
