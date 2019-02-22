// $Id: BeamBackgroundData.h 589193 2014-03-25 00:13:52Z tsulaia $
#ifndef RECBACKGROUNDEVENT_BEAMBACKGROUNDDATA
#define RECBACKGROUNDEVENT_BEAMBACKGROUNDDATA

#include "AthLinks/ElementLinkVector.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODJet/JetContainer.h"
#include <vector>

#include <iostream>
using namespace std;

/**
 * @Short Class holding the results of the Beam Background Identification Method
 *
 *        The results are obtained by BeamBackgroundFiller.
 *
 * @author David Salek <David.Salek@cern.ch>
 *
 * $Revision: 589193 $
 * $Date: 2014-03-25 01:13:52 +0100 (Tue, 25 Mar 2014) $
 */
class BeamBackgroundData
{
public:
  BeamBackgroundData() :
  m_decision(0),
  m_numSegment(0),
  m_numSegmentEarly(0),
  m_numSegmentACNoTime(0),
  m_numSegmentAC(0),
  m_numMatched(0),
  m_numNoTimeLoose(0),
  m_numNoTimeMedium(0),
  m_numNoTimeTight(0),  
  m_numOneSidedLoose(0),
  m_numOneSidedMedium(0),
  m_numOneSidedTight(0),
  m_numTwoSidedNoTime(0),
  m_numTwoSided(0),
  m_numClusterShape(0),
  m_numJet(0),
  m_direction(0)
  {};

  ~BeamBackgroundData() {};

  // decision bits
  enum Decision {
    Segment         = 1024,
    SegmentEarly    = 2048,
    SegmentACNoTime = 8192,
    SegmentAC       = 4096,
    Matched         =    1,
    NoTimeLoose     =    2,
    NoTimeMedium    =    4,
    NoTimeTight     =    8,
    OneSidedLoose   =   16,
    OneSidedMedium  =   32,
    OneSidedTight   =   64,
    TwoSidedNoTime  =  512,
    TwoSided        =  128,
    ClusterShape    =  256,

    TwoSidedNoTimeMedium = 16384,
    TwoSidedNoTimeTight = 32768
  };


  int GetDecision() const { return m_decision; };

  int GetNumSegment() const { return m_numSegment; };
  int GetNumSegmentEarly() const { return m_numSegmentEarly; };
  int GetNumSegmentACNoTime() const { return m_numSegmentACNoTime; };
  int GetNumSegmentAC() const { return m_numSegmentAC; };
  int GetNumMatched() const { return m_numMatched; };
  int GetNumNoTimeLoose() const { return m_numNoTimeLoose; };
  int GetNumNoTimeMedium() const { return m_numNoTimeMedium; };
  int GetNumNoTimeTight() const { return m_numNoTimeTight; };
  int GetNumOneSidedLoose() const { return m_numOneSidedLoose; };
  int GetNumOneSidedMedium() const { return m_numOneSidedMedium; };
  int GetNumOneSidedTight() const { return m_numOneSidedTight; };
  int GetNumTwoSidedNoTime() const { return m_numTwoSidedNoTime; };
  int GetNumTwoSidedNoTimeMedium() const { return m_numTwoSidedNoTimeMedium; };
  int GetNumTwoSidedNoTimeTight() const { return m_numTwoSidedNoTimeTight; }; 
  int GetNumTwoSided() const { return m_numTwoSided; };
  int GetNumClusterShape() const { return m_numClusterShape; };
  int GetNumJet() const { return m_numJet; };

  int GetResultSeg(int i) const {
    //cout << "ABCD GetResultSeg i=" << i << endl;
    //cout << "m_resultSeg.size=" << m_resultSeg.size() << endl;
    //cout << "elements" << endl;
    //for (int r : m_resultSeg) cout << " " << m_resultSeg[i];
    //cout << endl;
    return m_resultSeg[i];
}
  int GetResultClus(int i) const { return m_resultClus[i]; };
  int GetResultJet(int i) const { return m_resultJet[i]; };
  int GetIndexSeg(const xAOD::MuonSegment* seg) const;
  int GetIndexClus(const xAOD::CaloCluster* clus) const;
  int GetIndexJet(const xAOD::Jet* jet) const;
  const xAOD::MuonSegment* GetIndexSeg(int i) const { return *m_indexSeg[i]; };
  const xAOD::CaloCluster* GetIndexClus(int i) const { return *m_indexClus[i]; };
  const xAOD::Jet* GetIndexJet(int i) const { return *m_indexJet[i]; };
  float GetDrdzClus(int i) const { return m_drdzClus[i]; };

  int GetDirection() const { return m_direction; };

  vector<float> GetTimeDiffACSegmentACNoTime() { return m_timeDiffAC_SegmentACNoTime; }
  vector<float> GetTimeDiffACSegmentAC() { return m_timeDiffAC_SegmentAC; }
  vector<float> GetTimeDiffACTwoSidedNoTime() { return m_timeDiffAC_TwoSidedNoTime; }
  vector<float> GetTimeDiffACTwoSided() { return m_timeDiffAC_TwoSided; }

  void SetDecision(int decision) { m_decision = decision; };

  void SetNumSegment(int numSegment) { m_numSegment = numSegment; };
  void SetNumSegmentEarly(int numSegmentEarly) { m_numSegmentEarly = numSegmentEarly; };
  void SetNumSegmentACNoTime(int numSegmentACNoTime) { m_numSegmentACNoTime = numSegmentACNoTime; };
  void SetNumSegmentAC(int numSegmentAC) { m_numSegmentAC = numSegmentAC; };
  void SetNumMatched(int numMatched) { m_numMatched = numMatched; };
  void SetNumNoTimeLoose(int numNoTimeLoose) { m_numNoTimeLoose = numNoTimeLoose; };
  void SetNumNoTimeMedium(int numNoTimeMedium) { m_numNoTimeMedium = numNoTimeMedium; };
  void SetNumNoTimeTight(int numNoTimeTight) { m_numNoTimeTight = numNoTimeTight; };
  void SetNumOneSidedLoose(int numOneSidedLoose) { m_numOneSidedLoose = numOneSidedLoose; };
  void SetNumOneSidedMedium(int numOneSidedMedium) { m_numOneSidedMedium = numOneSidedMedium; };
  void SetNumOneSidedTight(int numOneSidedTight) { m_numOneSidedTight = numOneSidedTight; };
  void SetNumTwoSidedNoTimeMedium(int numTwoSidedNoTimeMedium) { m_numTwoSidedNoTimeMedium = numTwoSidedNoTimeMedium; };
  void SetNumTwoSidedNoTimeTight(int numTwoSidedNoTimeTight) { m_numTwoSidedNoTimeTight = numTwoSidedNoTimeTight; };      
  void SetNumTwoSidedNoTime(int numTwoSidedNoTime) { m_numTwoSidedNoTime = numTwoSidedNoTime; };
  void SetNumTwoSided(int numTwoSided) { m_numTwoSided = numTwoSided; };
  void SetNumClusterShape(int numClusterShape) { m_numClusterShape = numClusterShape; };
  void SetNumJet(int numJet) { m_numJet = numJet; };

  void SetDirection(int direction) { m_direction = direction; };

  //void FillIndexSeg(ElementLinkVector<Trk::SegmentCollection> indexSeg) { m_indexSeg = indexSeg; };
  void FillIndexSeg(ElementLinkVector<xAOD::MuonSegmentContainer> indexSeg) { m_indexSeg = indexSeg; };
  void FillIndexClus(ElementLinkVector<xAOD::CaloClusterContainer> indexClus) { m_indexClus = indexClus; };
  void FillIndexJet(ElementLinkVector<xAOD::JetContainer>indexJet) { m_indexJet = indexJet; };
  void FillResultSeg(std::vector<int>* resultSeg) {
    //cout << "ABCD FillResultSeg: " << endl;
    //cout << "resultSeg->size()=" << resultSeg->size() << endl;
    //cout << "elements: " << endl;
    //for (int i=0;i<resultSeg->size();i++) cout << " " << resultSeg->at(i);
    //cout << endl;

    //m_resultClus = *resultSeg;
    for (unsigned int i=0;i<resultSeg->size();i++)
      m_resultSeg.push_back(resultSeg->at(i)); 

    //cout << "m_resultSeg.size()=" << m_resultSeg.size() << endl;
    //cout << "elements: " << endl;
    //for (int i=0;i<m_resultSeg.size();i++) cout << " " << m_resultSeg.at(i);
    //cout << endl;
  }

  void FillResultClus(std::vector<int>* resultClus) { 
    //cout << "ABCD FillResultClus: resultClus->size()=" << resultClus->size() << endl;
    //m_resultClus = *resultClus; 
    for (int result : *resultClus) 
      m_resultClus.push_back(result);
    //cout << "m_resultClus.size()=" << m_resultClus.size() << endl;
  };

  void FillResultJet(std::vector<int>* resultJet) { 
    //m_resultJet = *resultJet; 
    //cout << "ABCD FillResultJet resultJet->size()=" << resultJet->size() << endl;
    for (int result : *resultJet)
      m_resultJet.push_back(result);
    //cout << "m_resultJet.size()=" << m_resultJet.size() << endl;
  };

  void FillMatchMatrix(std::vector<std::vector<int> >* matchMatrix) { 
    //cout << "ABCD FillMatchMatrix matchMatrix->size()=" << matchMatrix->size() << endl;
    for (vector<int> resultVec : *matchMatrix)
      m_matchMatrix.push_back(resultVec);
    //m_matchMatrix = *matchMatrix; 
    //cout << "m_matchMatrix.size()= " << m_matchMatrix.size() << endl;
  };
  void FillDrdzClus(std::vector<float>* drdzClus) { 
    //cout << "ABCD FillDrDzClus drdzClus->size() =" << drdzClus->size() << endl;
    for (float d : *drdzClus)
      m_drdzClus.push_back(d);
    //cout << "m_drdzClus.size()=" << m_drdzClus.size() << endl;
    //m_drdzClus = *drdzClus; 
  };
  void FillTimeDiffACSegmentACNoTime(std::vector<float>* timeDiffAC) {
    m_timeDiffAC_SegmentACNoTime = *timeDiffAC;
  }
  void FillTimeDiffACSegmentAC(std::vector<float>* timeDiffAC) {
    m_timeDiffAC_SegmentAC = *timeDiffAC;
  }
  void FillTimeDiffACTwoSidedNoTime(std::vector<float>* timeDiffAC) {
    m_timeDiffAC_TwoSidedNoTime = *timeDiffAC;
  }
  void FillTimeDiffACTwoSided(std::vector<float>* timeDiffAC) {
    m_timeDiffAC_TwoSided = *timeDiffAC;
  }

private:
  // bit-wise results of Beam Background Identification Methods
  int m_decision;

  int m_numSegment;         // number of segments with the direction parallel to the beam pipe
  int m_numSegmentEarly;    // number of early segments
  int m_numSegmentACNoTime; // number of mathced pairs of segments on side A and side C
  int m_numSegmentAC;       // number of mathced pairs of segments with corresponding time difference
  int m_numMatched;         // number of clusters matched with the segments
  int m_numNoTimeLoose;     // number of clusters identified by the "No-Time Method"
  int m_numNoTimeMedium;
  int m_numNoTimeTight;
  int m_numOneSidedLoose;   // number of clusters identified by the "One-Sided Method"
  int m_numOneSidedMedium;
  int m_numOneSidedTight;
  int m_numTwoSidedNoTime;  // number of clusters identified by the "Two-Sided No-Time Method"
  int m_numTwoSidedNoTimeMedium;
  int m_numTwoSidedNoTimeTight;   
  int m_numTwoSided;        // number of clusters identified by the "Two-Sided Method"
  int m_numClusterShape;    // number of clusters identified by the "Cluster-Shape Method"
  int m_numJet;             // number of fake jets

  int m_direction;  // direction of beam halo from the "Two-Sided Method" (positive for A->C, negative for C->A)

  vector<float> m_timeDiffAC_SegmentACNoTime;
  vector<float> m_timeDiffAC_SegmentAC;
  vector<float> m_timeDiffAC_TwoSidedNoTime;
  vector<float> m_timeDiffAC_TwoSided;

  ElementLinkVector<xAOD::CaloClusterContainer> m_indexClus;  // link to the calorimeter cluster
  ElementLinkVector<xAOD::MuonSegmentContainer> m_indexSeg;  // line to the muon segment
  std::vector<std::vector<int> > m_matchMatrix;  // matching matrix (stores results for each muon segment and calorimeter cluster)
  std::vector<int> m_resultSeg;  // summary of the results for each muon segment
  std::vector<int> m_resultClus;  // summary of the results for each cluster segment
  std::vector<float> m_drdzClus;  // shape of the cluster

  ElementLinkVector<xAOD::JetContainer> m_indexJet;  // link to the jet
  std::vector<int> m_resultJet;  // summary of the results for each jet
};

CLASS_DEF(BeamBackgroundData, 208768555, 1)

#endif
