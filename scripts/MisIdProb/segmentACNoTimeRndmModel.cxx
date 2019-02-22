
#include <iostream>
using namespace std;

#include <TRandom.h>
#include <TVector2.h>
#include <TMath.h>

#include "segmentACNoTimeRndmModel.h"

const float pi=TMath::Pi();

int RndmModel::numberOfTaggedEventsSegmentACNoTime(int nevents, int average_nsegments) {
   int ntagged = 0;
   TRandom r;

   int nsegments = r.Poisson(average_nsegments);
   //int nsegments = average_nsegments;

   for (int i=0;i<nevents;i++) {
     bool tagged = false;
    
     int nsegA=r.Gaus(nsegments/2, nsegments/4);
     if (nsegA<0) nsegA=0;
     int nsegC=nsegments-nsegA; 
     for (int j=0; j<nsegA; j++)
       for (int k=0;k<nsegC; k++) {
         double phiC = r.Uniform(2*pi);
         double phiA = r.Uniform(2*pi);
         if (TMath::Abs(phiC-phiA) < 4*pi/180) {
           tagged=true;
           break;
         }
       }
     if (tagged) ntagged++;
  }
  return ntagged;
}

