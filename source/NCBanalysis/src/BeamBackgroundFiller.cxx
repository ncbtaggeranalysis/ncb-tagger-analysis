// $Id: BeamBackgroundFiller.cxx 693118 2015-09-04 07:27:46Z salekd $
//#include "RecBackgroundAlgs/BeamBackgroundFiller.h"
#include "NCBanalysis/BeamBackgroundFiller.h"
//#include "AthenaKernel/errorcheck.h"
//#include "GeoPrimitives/GeoPrimitives.h"
//#include "MuonPrepRawData/CscPrepData.h"
//#include "xAODCaloEvent/CaloCluster.h"
//#include "xAODJet/JetConstituentVector.h"
//#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloGeoHelpers/CaloSampling.h"

BeamBackgroundFiller::BeamBackgroundFiller(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  p_clusContainer(nullptr),
  p_cscSegmentContainer(nullptr),
  p_mdtSegmentContainer(nullptr),
  p_jetContainer(nullptr),
  p_beamBackgroundData(nullptr),
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
  //m_idToFixedIdTool("MuonCalib::IdToFixedIdTool")
{
	
  m_doMuonBoyCSCTiming = true;

  declareProperty("cscSegmentContainerKey", m_cscSegmentContainerKey="NCB_MuonSegments");
  declareProperty("mdtSegmentContainerKey", m_mdtSegmentContainerKey="MuonSegments");
  declareProperty("caloClusterContainerKey", m_caloClusterContainerKey="CaloCalTopoClusters");
  declareProperty("jetContainerKey", m_jetContainerKey="AntiKt4EMTopoJets");
  declareProperty("BeamBackgroundKey", m_beamBackgroundDataKey="BeamBackgroundData", "SG key for output object.");
 
  declareProperty("doMuonBoyCSCTiming", m_doMuonBoyCSCTiming = true);
 
  declareProperty("cutThetaCsc" , m_cutThetaCsc  = 5. );
  declareProperty("cutThetaMdtI", m_cutThetaMdtI = 10.);
  declareProperty("cutPhi" , m_cutPhiSeg  = 4.);
  declareProperty("cutPhiCsc" , m_cutPhiCsc  = 4.);
  declareProperty("cutPhiMdtI", m_cutPhiMdtI = 4.);
  declareProperty("cutRadiusCsc" , m_cutRadiusCsc  = 300.);
  declareProperty("cutRadiusMdtI", m_cutRadiusMdtI = 800.);
 
  declareProperty("cutEnergy", m_cutEnergy = 10000.);
 
  // CSC :  881 < R < 2081
  // LAr barrel :  1500 < R < 1970
  // TileCal :  2280 < R < 4250
  declareProperty("cutRadiusLow" , m_cutRadiusLow  = 881. );
  declareProperty("cutRadiusHigh", m_cutRadiusHigh = 4250.);
 
  declareProperty("cutMuonTime", m_cutMuonTime = 25.);
  declareProperty("cutClusTime", m_cutClusTime = 2.5); 
  declareProperty("cutTimeDiffAC", m_cutTimeDiffAC = 25.);
 
  declareProperty("cutDrdz", m_cutDrdz = .15);

  declareProperty("useMDTSegmentsWitht0EqualTo0", m_useMDTSegmentsWitht0EqualTo0 = true);
  declareProperty("useMDT", m_useMDT = true);
}

//------------------------------------------------------------------------------
BeamBackgroundFiller::~BeamBackgroundFiller()
{ 
}

StatusCode BeamBackgroundFiller::initialize() {
  //CHECK( m_helperTool.retrieve() );
  //CHECK( m_idHelperTool.retrieve() );
  //CHECK( m_idToFixedIdTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode BeamBackgroundFiller::execute() {

  //p_cscSegmentContainer=evtStore()->tryConstRetrieve<Trk::SegmentCollection>(m_cscSegmentContainerKey);
  p_cscSegmentContainer=evtStore()->tryConstRetrieve<xAOD::MuonSegmentContainer>(m_cscSegmentContainerKey);
  if(!p_cscSegmentContainer ) {
    //msg(MSG::WARNING) << "Could not retrieve const Trk::SegmentCollection " << m_cscSegmentContainerKey << endreq;
    msg(MSG::WARNING) << "Could not retrieve const xAOD::MuonSegmentContainer " << m_cscSegmentContainerKey << endreq;
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG( m_cscSegmentContainerKey << " retrieved from StoreGate");
  }
 
  //p_mdtSegmentContainer=evtStore()->tryConstRetrieve<Trk::SegmentCollection>(m_mdtSegmentContainerKey);
  p_mdtSegmentContainer=evtStore()->tryConstRetrieve<xAOD::MuonSegmentContainer>(m_mdtSegmentContainerKey);
  if(!p_mdtSegmentContainer ) {
    //msg(MSG::WARNING) << "Could not retrieve const Trk::SegmentCollection " << m_mdtSegmentContainerKey << endreq;
    msg(MSG::WARNING) << "Could not retrieve const xAOD::MuonSegmentContainer " << m_mdtSegmentContainerKey << endreq;
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG( m_mdtSegmentContainerKey << " retrieved from StoreGate");
  }
 
  p_clusContainer=evtStore()->tryConstRetrieve<xAOD::CaloClusterContainer>(m_caloClusterContainerKey);
  if (!p_clusContainer) {
    msg(MSG::WARNING) << m_caloClusterContainerKey << " could not be retrieved from StoreGate !" << endreq;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG( m_caloClusterContainerKey << " retrieved from StoreGate");
  }
 
  p_jetContainer=evtStore()->tryConstRetrieve<xAOD::JetContainer>(m_jetContainerKey);
  if (!p_jetContainer) {
    msg(MSG::WARNING) << m_jetContainerKey << " could not be retrieved from StoreGate !" << endreq;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG( m_jetContainerKey << " retrieved from StoreGate");
  }

  // prepare to record the results in storegate
  p_beamBackgroundData = new BeamBackgroundData();

  StatusCode sc = evtStore()->record(p_beamBackgroundData, m_beamBackgroundDataKey);
  if (sc.isFailure())  {
    msg(MSG::ERROR) << "Unable to record BeamBackgroundData/" << m_beamBackgroundDataKey << " in StoreGate" << endreq;
    delete p_beamBackgroundData;
    return sc;
  } else {
    ATH_MSG_DEBUG( "BeamBackgroundData recorded in StoreGate");
  }

  FillMatchMatrix();
  //PrintMatchMatrix();
  
  SegmentMethod();
  
  OneSidedMethod();
  
  TwoSidedMethod();

  //ClusterShapeMethod();

  // identify fake jets	
  //FindFakeJets does not run on AODs, because it uses the TopoClusters inside jets which are probably not stored in the AODs.
  //FindFakeJets();

  // fill the results into BeamBackgroundData
  FillBeamBackgroundData();

  return StatusCode::SUCCESS;
}

StatusCode BeamBackgroundFiller::finalize() {
  
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
/**
  * This function selects the muon segments with the direction parallel to the beam pipe and
  * calorimeter clusters above certain energy threshold.
  * Matching matrix is created to store the results of beam background identification
  * for each cluster and segment
  */
void BeamBackgroundFiller::FillMatchMatrix() {
  m_numMatched = 0;
	
  m_indexSeg.clear();
  m_resultSeg.clear();
  m_indexClus.clear();
  m_matchMatrix.clear();
  m_resultClus.clear();

  // select only the CSC segments with the global direction parallel to the beam pipe
  for(unsigned int i=0; i<p_cscSegmentContainer->size(); i++) {
    const xAOD::MuonSegment* seg = p_cscSegmentContainer->at(i);
    if (!seg) {
      ATH_MSG_ERROR ("Muon segment pointer is null.");
      std::abort();
    } 

    //Identifier id = m_helperTool->chamberId(*seg);
    //if ( !id.is_valid() ) continue;
    //if ( !m_idHelperTool->isMuon(id) ) continue;
    	
    //if ( !m_idHelperTool->isCsc(id) ) continue;
    if (seg->chamberIndex() != 15 && seg->chamberIndex() != 16) continue;
    if (!m_useMDTSegmentsWitht0EqualTo0 && seg->t0() == 0) continue;

    //const Amg::Vector3D& globalPos = seg->globalPosition();
    //const Amg::Vector3D& globalDir = seg->globalDirection();
    const Amg::Vector3D globalPos(seg->x(),seg->y(),seg->z());
    const Amg::Vector3D globalDir(seg->px(),seg->py(),seg->pz()); 
    double thetaPos = globalPos.theta();
    double thetaDir = globalDir.theta();

    //ATH_MSG_DEBUG("NCB_MuonSegments phi=" << globalPos.phi());
	
    double d2r = TMath::Pi()/180.;
    if( TMath::Cos(2.*(thetaPos-thetaDir)) > TMath::Cos(2.*m_cutThetaCsc*d2r) ) continue;
	   
    ElementLink<xAOD::MuonSegmentContainer> segLink;
    segLink.toIndexedElement(*p_cscSegmentContainer, i);
    m_indexSeg.push_back(segLink);
  }
  //ATH_MSG_DEBUG( "m_indexSeg.size() after adding CSC seg=" << m_indexSeg.size() );

  // select only the MDT segments with the global direction parallel to the beam pipe
  if (m_useMDT)
  for(unsigned int i=0; i<p_mdtSegmentContainer->size(); i++) {
    const xAOD::MuonSegment* seg = p_mdtSegmentContainer->at(i);
    if (!seg) throw "Muon segment pointer is null.";

    //Identifier id = m_helperTool->chamberId(*seg);
    //if ( !id.is_valid() ) continue; 
    //if ( !m_idHelperTool->isMuon(id) ) continue;
	
    //Muon::MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(id);	
    //if(!( chIndex == Muon::MuonStationIndex::EIL || chIndex == Muon::MuonStationIndex::EIS )) continue;
 
    if (seg->chamberIndex() != 7 && seg->chamberIndex() != 8) continue;
	
    //const Amg::Vector3D& globalPos = seg->globalPosition();
    //const Amg::Vector3D& globalDir = seg->globalDirection();
    const Amg::Vector3D globalPos(seg->x(),seg->y(),seg->z());
    const Amg::Vector3D globalDir(seg->px(),seg->py(),seg->pz());
    double thetaPos = globalPos.theta();
    double thetaDir = globalDir.theta();   

    //ATH_MSG_DEBUG("MDT InnerEndCap seg phi=" << globalPos.phi());
 
    double d2r = M_PI/180.;
    if( TMath::Cos(2.*(thetaPos-thetaDir)) > TMath::Cos(2.*m_cutThetaMdtI*d2r) ) continue;
	   
    ElementLink<xAOD::MuonSegmentContainer> segLink;
    segLink.toIndexedElement(*p_mdtSegmentContainer, i);
    m_indexSeg.push_back(segLink);
  }
  //ATH_MSG_DEBUG( "m_indexSeg.size after adding also MDT seg=" << m_indexSeg.size() );

  m_resultSeg.assign(m_indexSeg.size(), int(0));

  // find matching clusters
  //cout << "#topoclustersin container=" << p_clusContainer->size() << endl;
  for(unsigned int i=0; i<p_clusContainer->size(); i++) {
    //cout << "clus #" << i << endl;
    const xAOD::CaloCluster* clus = p_clusContainer->at(i);

    double eEmClus =
      clus->eSample(CaloSampling::CaloSample::PreSamplerB) +
      clus->eSample(CaloSampling::CaloSample::EMB1) +
      clus->eSample(CaloSampling::CaloSample::EMB2) +
      clus->eSample(CaloSampling::CaloSample::EMB3) +
      clus->eSample(CaloSampling::CaloSample::PreSamplerE) +
      clus->eSample(CaloSampling::CaloSample::EME1) +
      clus->eSample(CaloSampling::CaloSample::EME2) +
      clus->eSample(CaloSampling::CaloSample::EME3) +
      clus->eSample(CaloSampling::CaloSample::FCAL0);
    double eHadClus =
      clus->eSample(CaloSampling::CaloSample::HEC0) +  
      clus->eSample(CaloSampling::CaloSample::HEC1) +
      clus->eSample(CaloSampling::CaloSample::HEC2) +
      clus->eSample(CaloSampling::CaloSample::HEC3) +  
      clus->eSample(CaloSampling::CaloSample::TileBar0) +
      clus->eSample(CaloSampling::CaloSample::TileBar1) +
      clus->eSample(CaloSampling::CaloSample::TileBar2) +  
      clus->eSample(CaloSampling::CaloSample::TileGap1) + 
      clus->eSample(CaloSampling::CaloSample::TileGap2) +
      clus->eSample(CaloSampling::CaloSample::TileGap3) +
      clus->eSample(CaloSampling::CaloSample::TileExt0) +
      clus->eSample(CaloSampling::CaloSample::TileExt1) +
      clus->eSample(CaloSampling::CaloSample::TileExt2) +
      clus->eSample(CaloSampling::CaloSample::FCAL1) +
      clus->eSample(CaloSampling::CaloSample::FCAL2);
    double eClus = eEmClus + eHadClus;
    //cout << "eClus=" << eClus << " clus->e()=" << clus->e() << endl;

    // ignore low energy clusters
    if ( eClus < m_cutEnergy ) continue;

    double rClus(0.);
    if (!clus->retrieveMoment(xAOD::CaloCluster_v1::CENTER_MAG,rClus))
        rClus = 0;
    rClus = rClus/cosh(clus->eta());
    //cout << "rClus=" << rClus << endl;

    //double xClus = rClus * cos( clus->phi());
    //double yClus = rClus * sin( clus->phi());
    //double zClus = rClus * sinh(clus->eta());
    double phiClus = clus->phi();
    //cout << "phiClus=" << phiClus << endl;	
 
    // remove clusters at low radius (outside the CSC acceptance)
    if( rClus < m_cutRadiusLow ) continue;
    if( rClus > m_cutRadiusHigh ) continue;

    std::vector<int> matchedSegmentsPerCluster;  
    matchedSegmentsPerCluster.assign(m_indexSeg.size(), int(0));
    bool matched = false;

    for(unsigned int j=0; j<m_indexSeg.size(); j++) {
      const xAOD::MuonSegment* seg =  *m_indexSeg[j];

      //if (!seg) std::abort();
      if (!seg) throw "Muon segment pointer is null.";	 
      //Identifier id = m_helperTool->chamberId(*seg);
      //bool isCsc = m_idHelperTool->isCsc(id);
      bool isCsc = (seg->chamberIndex()==15 || seg->chamberIndex()==16);
	
      //const Amg::Vector3D& globalPos = seg->globalPosition();
      const Amg::Vector3D globalPos(seg->x(),seg->y(),seg->z());
      double phiSeg = globalPos.phi();
      double rSeg = globalPos.perp();
	
      // match in phi
      double d2r = M_PI/180.;
      if( TMath::Cos(phiClus-phiSeg) < TMath::Cos(m_cutPhiCsc*d2r) && isCsc ) continue;
      if( TMath::Cos(phiClus-phiSeg) < TMath::Cos(m_cutPhiMdtI*d2r) && !isCsc ) continue;
	
      // match in radius 
      if( TMath::Abs(rClus-rSeg) > m_cutRadiusCsc && isCsc ) continue;
      if( TMath::Abs(rClus-rSeg) > m_cutRadiusMdtI && !isCsc ) continue;
	 
      matchedSegmentsPerCluster[j] = 1;
      matched = true;
      m_resultSeg[j] = m_resultSeg[j] | BeamBackgroundData::Matched;

    }

    if(!matched) continue;
	
    ElementLink<xAOD::CaloClusterContainer> clusLink;
    clusLink.toIndexedElement(*p_clusContainer, i);
    m_indexClus.push_back(clusLink);
    m_matchMatrix.push_back(matchedSegmentsPerCluster);
    m_numMatched++;

  } 

  m_resultClus.assign(m_indexClus.size(), int(1));

}

/*void BeamBackgroundFiller::PrintMatchMatrix() {
  if (m_matchMatrix.size()) { 
    cout << "BeamBackgroundDataFiller::PrintMatchMatrix" << endl;
      for (vector<int> segVec : m_matchMatrix) {
      for (int seg : segVec);
        cout << seg << " ";
      //cout << endl;
    }
  }
}*/

/**
 * This function looks at the segments found by the FillMatchMatrix function.
 * The following results are set:
 * - number of segments with the direciton parallel to the beam pipe
 *   (all the segments in the matching matrix)
 * - number of such segments with early time
 * - number of segment pairs on side A and C
 * - number of segment pairs on side A and C with the corresponding time difference
 */
void BeamBackgroundFiller::SegmentMethod()
{
  //ATH_MSG_DEBUG("SegmentMethod m_indexSeg.size()="<<m_indexSeg.size());

  m_numSegment = 0;
  m_numSegmentEarly = 0;
  m_numSegmentACNoTime = 0;
  m_numSegmentAC = 0;

  m_timeDiffAC_SegmentACNoTime.clear();
  m_timeDiffAC_SegmentAC.clear();

  for(unsigned int segIndex=0; segIndex<m_indexSeg.size(); segIndex++) {

    const xAOD::MuonSegment* seg = *m_indexSeg[segIndex];
    if (!seg) throw "BeamBackgroundFiller::SegmentMethod: segment pointer is null";

    const Amg::Vector3D globalPos(seg->x(),seg->y(),seg->z());
    double zSeg = globalPos.z();

    double tSeg = seg->t0();

    m_numSegment++;
    m_resultSeg[segIndex] = m_resultSeg[segIndex] | BeamBackgroundData::Segment;

    // muon segment: in-time (1), early (2), ambiguous (0)
    int timeStatus = 0;
    double inTime = -(-TMath::Abs(zSeg) + globalPos.mag())*(1e-3/3e8/1e-9);
    double early  = -( TMath::Abs(zSeg) + globalPos.mag())*(1e-3/3e8/1e-9);
    if( TMath::Abs(tSeg - inTime) < m_cutMuonTime ) timeStatus = 1;
    if( TMath::Abs(tSeg - early ) < m_cutMuonTime ) timeStatus = 2;
    
    if( timeStatus == 2 ) {
      m_numSegmentEarly++;
      m_resultSeg[segIndex] = m_resultSeg[segIndex] | BeamBackgroundData::SegmentEarly;
    }

    // take only the segments on side A (z > 0)
    if( zSeg < 0. ) continue;

    unsigned int segIndexA = segIndex;

    double tSegA = tSeg;
    double timeStatusA = timeStatus;

    double phiSegA = globalPos.phi();
    //ATH_MSG_DEBUG("segIndexA="<<segIndexA
    //             <<" phiSegA="<<phiSegA
    //             <<" zSeg=" << zSeg
    //             <<" tSegA="<<tSegA);

    for(unsigned int segIndexC=0; segIndexC<m_indexSeg.size(); segIndexC++) {

      const xAOD::MuonSegment* segC = *m_indexSeg[segIndexC];
      if (!segC) throw "BeamBackgroundFiller::SegmentMethod: segment pointer is null";
      const Amg::Vector3D globalPos(segC->x(),segC->y(),segC->z());
      double zSegC = globalPos.z();

      double tSegC = segC->t0();

      //ATH_MSG_DEBUG("segC=" << segC << "globalPos.z()="<<globalPos.z());

      // take only the segments on side C (z < 0)
      if( zSegC > 0. ) continue;

      // muon segment: in-time (1), early (2), ambiguous (0)
      int timeStatusC = 0;
      double inTime = -(-TMath::Abs(zSegC) + globalPos.mag())*(1e-3/3e8/1e-9);
      double early  = -( TMath::Abs(zSegC) + globalPos.mag())*(1e-3/3e8/1e-9);
      if( TMath::Abs(tSegC - inTime) < m_cutMuonTime ) timeStatusC = 1;
      if( TMath::Abs(tSegC - early ) < m_cutMuonTime ) timeStatusC = 2;

      double phiSegC = globalPos.phi();
      //ATH_MSG_DEBUG("segIndexC=" << segIndexC
      //            << " phiSegC=" << phiSegC
      //            << " zSegC=" << zSegC
      //            << " tSegC=" << tSegC);
      // match in phi
      double d2r = M_PI/180.;
      if( TMath::Cos(phiSegA-phiSegC) < TMath::Cos(m_cutPhiSeg*d2r) ) continue;
      m_numSegmentACNoTime++;
      
      m_resultSeg[segIndexA] = m_resultSeg[segIndexA] | BeamBackgroundData::SegmentACNoTime;
      m_resultSeg[segIndexC] = m_resultSeg[segIndexC] | BeamBackgroundData::SegmentACNoTime;

      m_timeDiffAC_SegmentACNoTime.push_back(tSegA-tSegC);

      if( timeStatusA == 0 || timeStatusC == 0 ) continue;

      // check the time difference
      if( TMath::Abs(tSegA - tSegC) > m_cutTimeDiffAC ) {
        m_numSegmentAC++;
        m_resultSeg[segIndexA] = m_resultSeg[segIndexA] | BeamBackgroundData::SegmentAC;
        m_resultSeg[segIndexC] = m_resultSeg[segIndexC] | BeamBackgroundData::SegmentAC;
      }

      m_timeDiffAC_SegmentAC.push_back(tSegA-tSegC);

    }

  }
  //ATH_MSG_DEBUG("SegmentMethod m_numSegmentACNoTime="<<m_numSegmentACNoTime);
}

//------------------------------------------------------------------------------
/**
 * This function is the implementation of the "No-Time Method" and the "One-Sided Method".
 * The "One-Sided Method" compares the time of the calorimeter cluster with the expected time.
 * The expected time is calculated based on the direction of the beam background muon
 * which is reconstructed from the position and time of the muon segment.
 * The "No-Time Method" does not use the time information of the muon segment
 * thus the direction of the beam background muon is not known.
 * Therefore, the cluster time is compared to two expected time values
 * (corresponding to both A->C and C->A directions).
 */

void BeamBackgroundFiller::OneSidedMethod()
{
  m_numNoTimeLoose = 0;
  m_numNoTimeMedium = 0;
  m_numNoTimeTight = 0;

  m_numOneSidedLoose = 0;
  m_numOneSidedMedium = 0;
  m_numOneSidedTight = 0;


  for(unsigned int clusIndex=0; clusIndex<m_indexClus.size(); clusIndex++) {

    const xAOD::CaloCluster* clus = *m_indexClus[clusIndex];

    double rClus(0.);
    if (!clus->retrieveMoment(xAOD::CaloCluster_v1::CENTER_MAG,rClus))
      rClus = 0;
    rClus = rClus/cosh(clus->eta());
    double zClus = rClus * sinh(clus->eta());
    double tClus = clus->time();

    // calculate expected cluster time
    double expectedClusterTimeAC = -( zClus + TMath::Sqrt(rClus*rClus + zClus*zClus))*(1e-3/3e8/1e-9);
    double expectedClusterTimeCA = -(-zClus + TMath::Sqrt(rClus*rClus + zClus*zClus))*(1e-3/3e8/1e-9);

    for(unsigned int segIndex=0; segIndex<m_indexSeg.size(); segIndex++) {

      if( !(m_matchMatrix[clusIndex][segIndex]&1) ) continue;

      const xAOD::MuonSegment* seg = *m_indexSeg[segIndex];
      if (!seg) throw "MuonSegment pointer is null.";

      const Amg::Vector3D  globalPos(seg->x(),seg->y(),seg->z());
      double zSeg = globalPos.z();

      double tSeg = seg->t0();
      // muon segment: in-time (1), early (2), ambiguous (0)
      int timeStatus = 0;
      double inTime = -(-TMath::Abs(zSeg) + globalPos.mag())*(1e-3/3e8/1e-9);
      double early  = -( TMath::Abs(zSeg) + globalPos.mag())*(1e-3/3e8/1e-9);
      if( TMath::Abs(tSeg - inTime) < m_cutMuonTime ) timeStatus = 1;
      if( TMath::Abs(tSeg - early ) < m_cutMuonTime ) timeStatus = 2;

      // reconstruct beam background direction: A->C (1), C->A (-1)
      int direction = 0;
      if( (zSeg>0 && timeStatus==2) || (zSeg<0 && timeStatus==1) ) direction = 1;
      if( (zSeg>0 && timeStatus==1) || (zSeg<0 && timeStatus==2) ) direction = -1;

      // check the cluster time without the beam background direction information
      if( TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime ||
          TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime ) {
        m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::NoTimeLoose;
      }
      if( (TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime && -tClus > m_cutClusTime) ||
          (TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime && -tClus > m_cutClusTime) ) {
        m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::NoTimeMedium;
      }
      if( (TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime && -tClus > 2.*m_cutClusTime) ||
          (TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime && -tClus > 2.*m_cutClusTime) ) {
        m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::NoTimeTight;
      }

      // check the cluster time with the beam background direction information
      if( direction == 1 ) {
        if( TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedLoose;
        }
        if( TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime && -tClus > m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedMedium;
        }
        if( TMath::Abs(tClus - expectedClusterTimeAC) < m_cutClusTime && -tClus > 2.*m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedTight;
        }
      } else
      if( direction == -1 ) {
        if( TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedLoose;
        }
        if( TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime && -tClus > m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedMedium;
        }
        if( TMath::Abs(tClus - expectedClusterTimeCA) < m_cutClusTime && -tClus > 2.*m_cutClusTime ) {
          m_matchMatrix[clusIndex][segIndex] = m_matchMatrix[clusIndex][segIndex] | BeamBackgroundData::OneSidedTight;
        }
      }

      m_resultClus[clusIndex] = m_resultClus[clusIndex] | m_matchMatrix[clusIndex][segIndex];
      m_resultSeg[segIndex] = m_resultSeg[segIndex] | m_matchMatrix[clusIndex][segIndex];

    }

    if(m_resultClus[clusIndex] & BeamBackgroundData::NoTimeLoose ) m_numNoTimeLoose++;
    if(m_resultClus[clusIndex] & BeamBackgroundData::NoTimeMedium) m_numNoTimeMedium++;
    if(m_resultClus[clusIndex] & BeamBackgroundData::NoTimeTight ) m_numNoTimeTight++;

    if(m_resultClus[clusIndex] & BeamBackgroundData::OneSidedLoose ) m_numOneSidedLoose++;
    if(m_resultClus[clusIndex] & BeamBackgroundData::OneSidedMedium) m_numOneSidedMedium++;
    if(m_resultClus[clusIndex] & BeamBackgroundData::OneSidedTight ) m_numOneSidedTight++;

  }

}


//------------------------------------------------------------------------------
/**
 * This function is the implementation of the "Two-Sided No-Time Method" and
 * the "Two-Sided Method" that looks at the clusters matched with
 * at least one muon segment on side A and one muon segment on side C.
 * In case of the "Two-Sided Method",
 * corresponding time difference of the muon segments is required
 * and the direction of the beam background muon is also stored.
 */

void BeamBackgroundFiller::TwoSidedMethod()
{
  //cout << "TwoSidedMethod" << endl;
  m_numTwoSidedNoTime = 0;
  m_numTwoSided = 0;
  m_direction = 0;

  m_timeDiffAC_TwoSidedNoTime.clear();
  m_timeDiffAC_TwoSided.clear();

  for(unsigned int clusIndex=0; clusIndex<m_indexClus.size(); clusIndex++) {

    for(unsigned int segIndexA=0; segIndexA<m_indexSeg.size(); segIndexA++) {

      if( !(m_matchMatrix[clusIndex][segIndexA]&1) ) continue;

      const xAOD::MuonSegment* seg = *m_indexSeg[segIndexA];
      if (!seg) throw "MuonSegment pointer is null";

      const Amg::Vector3D  globalPos(seg->x(),seg->y(),seg->z());
      double zSegA = globalPos.z();

      double tSegA = seg->t0();

      // muon segment: in-time (1), early (2), ambiguous (0)
      int timeStatusA = 0;
      double inTime = -(-TMath::Abs(zSegA) + globalPos.mag())*(1e-3/3e8/1e-9);
      double early  = -( TMath::Abs(zSegA) + globalPos.mag())*(1e-3/3e8/1e-9);
      if( TMath::Abs(tSegA - inTime) < m_cutMuonTime ) timeStatusA = 1;
      if( TMath::Abs(tSegA - early ) < m_cutMuonTime ) timeStatusA = 2;

      if( zSegA < 0. ) continue;

      for(unsigned int segIndexC=0; segIndexC<m_indexSeg.size(); segIndexC++) {

        if( !(m_matchMatrix[clusIndex][segIndexC]&1) ) continue;

        const xAOD::MuonSegment* seg = *m_indexSeg[segIndexC];
        if (!seg) throw "MuonSegment pointer is null.";

        const Amg::Vector3D  globalPos(seg->x(),seg->y(),seg->z());
        double zSegC = globalPos.z();

        double tSegC = seg->t0();

        // muon segment: in-time (1), early (2), ambiguous (0)
        int timeStatusC = 0;
        double inTime = -(-TMath::Abs(zSegC) + globalPos.mag())*(1e-3/3e8/1e-9);
        double early  = -( TMath::Abs(zSegC) + globalPos.mag())*(1e-3/3e8/1e-9);
        if( TMath::Abs(tSegC - inTime) < m_cutMuonTime ) timeStatusC = 1;
        if( TMath::Abs(tSegC - early ) < m_cutMuonTime ) timeStatusC = 2;

        if( zSegC > 0. ) continue;

        m_matchMatrix[clusIndex][segIndexA] = m_matchMatrix[clusIndex][segIndexA] | BeamBackgroundData::TwoSidedNoTime;
        m_matchMatrix[clusIndex][segIndexC] = m_matchMatrix[clusIndex][segIndexC] | BeamBackgroundData::TwoSidedNoTime;
        m_resultSeg[segIndexA] = m_resultSeg[segIndexA] | m_matchMatrix[clusIndex][segIndexA];
        m_resultSeg[segIndexC] = m_resultSeg[segIndexC] | m_matchMatrix[clusIndex][segIndexC];
        m_timeDiffAC_TwoSidedNoTime.push_back(tSegA-tSegC);

        if( timeStatusA == 0 || timeStatusC == 0 ) continue;

        // check the time difference
        if( TMath::Abs(tSegA - tSegC) > m_cutTimeDiffAC ) {
          m_matchMatrix[clusIndex][segIndexA] = m_matchMatrix[clusIndex][segIndexA] | BeamBackgroundData::TwoSided;
          m_matchMatrix[clusIndex][segIndexC] = m_matchMatrix[clusIndex][segIndexC] | BeamBackgroundData::TwoSided;
          m_resultSeg[segIndexA] = m_resultSeg[segIndexA] | m_matchMatrix[clusIndex][segIndexA];
          m_resultSeg[segIndexC] = m_resultSeg[segIndexC] | m_matchMatrix[clusIndex][segIndexC];
          m_timeDiffAC_TwoSided.push_back(tSegA-tSegC);

          // direction of beam background
          if( timeStatusA == 2 ) m_direction++;  // A->C
          if( timeStatusC == 2 ) m_direction--;  // C->A
        }
  
      }

      m_resultClus[clusIndex] = m_resultClus[clusIndex] | m_matchMatrix[clusIndex][segIndexA];
    }

    if(m_resultClus[clusIndex] & BeamBackgroundData::TwoSidedNoTime) m_numTwoSidedNoTime++;
    if(m_resultClus[clusIndex] & BeamBackgroundData::TwoSided) m_numTwoSided++;
  }
}

//------------------------------------------------------------------------------
/**
 * This function stores all the results in BeamBackgroundData
 */
void BeamBackgroundFiller::FillBeamBackgroundData()
{
  p_beamBackgroundData->SetNumSegment(m_numSegment);
  p_beamBackgroundData->SetNumSegmentEarly(m_numSegmentEarly);
  p_beamBackgroundData->SetNumSegmentACNoTime(m_numSegmentACNoTime);
  p_beamBackgroundData->SetNumSegmentAC(m_numSegmentAC);
  p_beamBackgroundData->SetNumMatched(m_numMatched);
  p_beamBackgroundData->SetNumNoTimeLoose(m_numNoTimeLoose);
  p_beamBackgroundData->SetNumNoTimeMedium(m_numNoTimeMedium);
  p_beamBackgroundData->SetNumNoTimeTight(m_numNoTimeTight);
  p_beamBackgroundData->SetNumOneSidedLoose(m_numOneSidedLoose);
  p_beamBackgroundData->SetNumOneSidedMedium(m_numOneSidedMedium);
  p_beamBackgroundData->SetNumOneSidedTight(m_numOneSidedTight);
  p_beamBackgroundData->SetNumTwoSidedNoTime(m_numTwoSidedNoTime);
  p_beamBackgroundData->SetNumTwoSided(m_numTwoSided);
  p_beamBackgroundData->SetNumClusterShape(m_numClusterShape);
  p_beamBackgroundData->SetNumJet(m_numJet);

  int decision = 0;
  for(unsigned int i=0; i<m_indexSeg.size(); i++) {
    decision = decision | m_resultSeg[i];
  }
  for(unsigned int i=0; i<m_indexClus.size(); i++) {
    decision = decision | m_resultClus[i];
  }

  p_beamBackgroundData->SetDecision(decision);

  p_beamBackgroundData->SetDirection(m_direction);

  p_beamBackgroundData->FillIndexSeg(m_indexSeg);
  p_beamBackgroundData->FillResultSeg(&m_resultSeg);
  p_beamBackgroundData->FillIndexClus(m_indexClus);
  p_beamBackgroundData->FillMatchMatrix(&m_matchMatrix);

  p_beamBackgroundData->FillResultClus(&m_resultClus);
  //p_beamBackgroundData->FillIndexJet(m_indexJet);
  p_beamBackgroundData->FillDrdzClus(&m_drdzClus);

  p_beamBackgroundData->FillIndexJet(m_indexJet);
  p_beamBackgroundData->FillResultJet(&m_resultJet);
  p_beamBackgroundData->FillTimeDiffACSegmentACNoTime(&m_timeDiffAC_SegmentACNoTime);
  p_beamBackgroundData->FillTimeDiffACSegmentAC(&m_timeDiffAC_SegmentAC);
  p_beamBackgroundData->FillTimeDiffACTwoSidedNoTime(&m_timeDiffAC_TwoSidedNoTime);
  p_beamBackgroundData->FillTimeDiffACTwoSided(&m_timeDiffAC_TwoSided);

  /*cout << "parallel segments " <<
                 m_numSegment << " " <<
                 m_numSegmentEarly << " " <<
                 m_numSegmentACNoTime << " " <<
                 m_numSegmentAC << endl;

  cout << "matched clusters " <<
                 m_numMatched << " " <<
                 m_numNoTimeLoose << " " <<
                 m_numNoTimeMedium << " " <<
                 m_numNoTimeTight << " " <<
                 m_numOneSidedLoose << " " <<
                 m_numOneSidedMedium << " " <<
                 m_numOneSidedTight << " " <<
                 m_numTwoSidedNoTime << " " <<
                 m_numTwoSided << " " <<
		 m_numClusterShape << endl; */
}

void BeamBackgroundFiller::FindFakeJets()
{
  //ATH_MSG_INFO( "FindFakeJets" );
  m_numJet = 0;

  m_indexJet.clear();
  m_resultJet.clear();

  // find the jet that contains this cluster

  for(unsigned int jetIndex=0; jetIndex<p_jetContainer->size(); jetIndex++) {
    cout << "jetIndex=" << jetIndex << endl;
    const xAOD::Jet* jet = p_jetContainer->at(jetIndex);
    cout << "jet=" << jet << endl;

    bool isFakeJet = false;
    int resultJet = 0;

    xAOD::JetConstituentVector vec = jet->getConstituents();
    cout << "vec.size=" << vec.size() << endl;
    xAOD::JetConstituentVector::iterator constIt = vec.begin();
    xAOD::JetConstituentVector::iterator constItE = vec.end();
	
    for(; constIt != constItE; ++constIt) {
      if(constIt->type()!=xAOD::Type::CaloCluster)
        continue;
      cout << "inside constIt loop." << endl;
      const xAOD::CaloCluster* jetConst = dynamic_cast<const xAOD::CaloCluster*>(constIt->rawConstituent());

      for(unsigned int clusIndex=0; clusIndex<m_indexClus.size(); clusIndex++) {
        const xAOD::CaloCluster* clus = *m_indexClus[clusIndex];

        if( jetConst == clus ) {
          isFakeJet = true;
          resultJet = resultJet | m_resultClus[clusIndex];
        }
      }
    }

    if( isFakeJet ) {
      cout << "inside if isFakeJet" << endl;
      ElementLink<xAOD::JetContainer> jetLink;
      jetLink.toIndexedElement(*p_jetContainer, jetIndex);
      m_indexJet.push_back(jetLink);
      m_resultJet.push_back(resultJet);
      m_numJet++;
    }
  }
}

