
#include <iostream>
using namespace std;
#include <vector>

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TVector2.h>
#include <TSystem.h>
#include <TH1F.h>
#include <TH2F.h>

#include "taggerAnalysisLib.h"

const float cut_topoCluster_energy = 
10000;
//5000;

const float pi = TMath::Pi();

const vector<unsigned int> debugListEvents = {
  1605,5993,6446,6981,6869,8914,11574,10394,13610,13575
};

class TopoCluster {
  public:
          TopoCluster(float emscale_e, float rPerp, float phi, float z, float time, int fracSamplingMaxIndex);
         ~TopoCluster();
    void  Print();
    bool  passesEnergyAndRadius();
    bool  passesExpectedTime();
    float getExpectedTime(TString direction);
    float phi() { return m_phi; }
    float rPerp() { return m_rPerp; }
    bool  isLAr();
    bool  isTile();
    float time() { return m_time; }
    float z() { return m_z; }
    float emscale_e() { return m_emscale_e; }

    bool  isMatched() { return m_isMatched; };
    bool  setMatched() { m_isMatched = true; }

  private:
    float m_emscale_e;
    float m_rPerp;
    float m_phi;
    float m_z;
    float m_time;
    int   m_fracSamplingMaxIndex;
    bool  m_isMatched;
};

TopoCluster::TopoCluster(float emscale_e,float rPerp, float phi, float z, float time, int fracSamplingMaxIndex) {
  m_emscale_e = emscale_e;
  m_rPerp = rPerp;
  m_phi = phi;
  m_z = z;
  m_time = time;
  m_fracSamplingMaxIndex = fracSamplingMaxIndex;
  m_isMatched= false;
}

TopoCluster::~TopoCluster() {
  //cout << "TopoCluster::~TopoCluster()" << endl;
}

float TopoCluster::getExpectedTime(TString direction) {
  if (direction == "AtoC")
    return (- TMath::Sqrt(m_rPerp*m_rPerp + m_z*m_z) - m_z)*1E-3 / 3E8 * 1E9; // ns
  else if (direction == "CtoA")
    return (- TMath::Sqrt(m_rPerp*m_rPerp + m_z*m_z) + m_z)*1E-3 / 3E8 * 1E9; 
  throw "getExpectedTime: unknow direction";
  return 0;
}

bool TopoCluster::isLAr() {
  if (m_fracSamplingMaxIndex<=11) return true;
  return false;
}

bool TopoCluster::isTile() {
  if (m_fracSamplingMaxIndex>=12&&m_fracSamplingMaxIndex<=20) return true;
  return false;
}

void TopoCluster::Print() {
  cout << "TopoCluster: " 
       << Form("emscale_e=%5.0f ",m_emscale_e)
       << Form(" rPerp=%5.0f",m_rPerp)
       << Form(" phi=%5.1f",m_phi*180/pi)
       << Form(" z=%5.0f",m_z)
       << Form(" time=%.2f",m_time)
       << " passesExpectedTime: " << passesExpectedTime() 
       << " fracSamplingMaxIndex: " << m_fracSamplingMaxIndex
       << " isLAr: " << isLAr()
       << " isTile: " << isTile()
       << endl;
}

bool TopoCluster::passesEnergyAndRadius() {
  if (m_emscale_e < cut_topoCluster_energy) return false;
  if (m_rPerp < 881 || m_rPerp > 4250) return false;
  return true;
}

bool TopoCluster::passesExpectedTime() {
  float expectedTimeAtoC = getExpectedTime("AtoC");
  float expectedTimeCtoA = getExpectedTime("CtoA");
  
  if (TMath::Abs(m_time - expectedTimeAtoC) < 2.5) return true;
  if (TMath::Abs(m_time - expectedTimeCtoA) < 2.5) return true;

  return false;
}

class MuonSegment {
  public:
          MuonSegment(float x, float y, float z, float thetaPos, float thetaDir, int chamberIndex,float t0);
    bool  passesDeltaTheta();
    bool  isCSC();
    bool  isMDTInnerEndCap();
    void  Print();
    float phi() { return m_phi; }
    float rPerp() { return m_rPerp; }  
    float deltaTheta() { return m_deltaTheta; }
    float x() {return m_x; }
    float y() {return m_y; }
    float z() {return m_z; }
    float t0() {return m_t0; }

  private:

    float m_x;
    float m_y;
    float m_z;
    float m_thetaPos;
    float m_thetaDir;
    int m_chamberIndex;
    float m_t0;

    float m_deltaTheta;
    float m_rPerp;
    float m_phi;
};

MuonSegment::MuonSegment(float x, float y, float z, float thetaPos, float thetaDir, int chamberIndex,float t0) {
  m_x = x;
  m_y = y;
  m_z = z;
  m_thetaPos = thetaPos;
  m_thetaDir = thetaDir;
  m_chamberIndex = chamberIndex;
  m_deltaTheta = thetaPos-thetaDir;
  m_rPerp = TMath::Sqrt(x*x+y*y);
  m_phi = TMath::ATan2(y,x);
  m_t0=t0;
}

void MuonSegment::Print() {
  cout << "MuonSegment:     x     y     z rPerp   phi thetaPos thetaDir   deltaTheta  chamberIndex" << endl
       << Form("%18.0f", m_x) 
       << Form("%6.0f", m_y)
       << Form("%6.0f", m_z)
       << Form("%6.0f", m_rPerp)
       << Form("%6.1f",m_phi*180/pi)
       << Form("%9.1f",m_thetaPos*180/pi)
       << Form("%9.1f",m_thetaDir*180/pi)
       << Form("%11.1f(%s)",m_deltaTheta*180/pi,passesDeltaTheta() ? "T" : "F")
       << Form("%13i", m_chamberIndex);
  if (isCSC()) cout << " CSC";
  if (isMDTInnerEndCap()) cout << " MdtI";
  cout << endl;
}

bool MuonSegment::passesDeltaTheta() {
  if (isCSC() && TMath::Abs(m_deltaTheta) > 5*pi/180) return true;
  if (isMDTInnerEndCap() && TMath::Abs(m_deltaTheta) > 10*pi/180) return true;
  return false;
}

bool MuonSegment::isCSC() {
  if (m_chamberIndex == 15 || m_chamberIndex == 16) 
    return true;
  return false;
}

bool MuonSegment::isMDTInnerEndCap() {
  if (m_chamberIndex == 7 || m_chamberIndex == 8)
    return true;
  return false;
}

class EventInfo {
  public:
           EventInfo(int NPV, float mu,int lbn,int bcid,bool stableBeams);
     int   NPV() {return m_NPV;}
     float mu() {return m_mu;}
     int   lbn() {return m_lbn;}
     int   bcid() {return m_bcid;}
     bool  stableBeams() {return m_stableBeams;}

  private:
    int m_NPV;
    int m_bcid;
    float m_mu;
    float m_lbn;
    bool m_stableBeams;
};

EventInfo::EventInfo(int NPV, float mu,int lbn,int bcid,bool stableBeams) {
  m_NPV=NPV;
  m_mu=mu;
  m_lbn=lbn;
  m_bcid=bcid;
  m_stableBeams=stableBeams;
}

class Jet {
  public:
         Jet(float pt,float eta,float emf,float hecf,float hecq,float larq,float AverageLArQF,float negE,float phi,float time);
    float pt() {return m_pt;}
    float eta() {return m_eta;}
    float emf() {return m_emf;}
    float hecf() {return m_hecf;}
    float hecq() {return m_hecq;}
    float larq() {return m_larq;}
    float AverageLArQF() {return m_AverageLArQF;}
    float negE() {return m_negE;}
    float phi() {return m_phi;}
    float time() {return m_time;}
  private:
    float m_pt;
    float m_eta;
    float m_emf;
    float m_hecf;
    float m_hecq;
    float m_larq;
    float m_AverageLArQF;
    float m_negE;
    float m_phi;
    float m_time;
};

Jet::Jet(float pt,float eta,float emf,float hecf,float hecq,float larq,float AverageLArQF,float negE,float phi,float time) {
  m_pt=pt;
  m_eta=eta;
  m_emf=emf;
  m_hecf=hecf;
  m_hecq=hecq;
  m_larq=larq;
  m_AverageLArQF=AverageLArQF;
  m_negE=negE;
  m_phi=phi;
  m_time=time;
}

class HistTool {
  public:
            HistTool(TString name);
    void    Fill(EventInfo& eventInfo, vector<TopoCluster*> topoClusters,vector<MuonSegment*> muonSegments, vector<Jet*> jets);
    void    Write();
    TString name() {return m_name;};

  private:
    TString m_name;

    TH1F* m_hist_NPV;
    TH1F* m_hist_mu;
    TH1F* m_hist_bcid;
    TH1F* m_hist_lbn;
    TH1F* m_hist_stableBeams; 

    TH1F* m_hist_seg_deltaTheta[2];

    TH2F* m_hist_selectedSeg_xy[2][3];
    TH1F* m_hist_selectedSeg_deltaTheta[2];
    TH1F* m_hist_selectedSeg_t0[2];
    TH1F* m_hist_selectedSeg_phiDiffAC[2];
    TH1F* m_hist_selectedSeg_phiDiff_selectedClus[2];
    TH1F* m_hist_selectedSeg_rDiff_selectedClus[2];
    TH1F* m_hist_selectedSeg_passPhiDiffAC_timeDiff[2];

    TH2F* m_hist_selectedClus_t_z[3];
    TH1F* m_hist_selectedClus_emscale_e;
    TH2F* m_hist_matchedClus_t_z[3];
    TH1F* m_hist_matchedClus_emscale_e;
  
    TH1F* m_hist_jet0_pt;
    TH1F* m_hist_jet0_eta;
    TH1F* m_hist_jet0_phi;
    TH2F* m_hist_jet0_etaphi;
    TH1F* m_hist_jet0_emf;
    TH1F* m_hist_jet0_hecf;
    TH1F* m_hist_jet0_hecq;
    TH1F* m_hist_jet0_larq;
    TH1F* m_hist_jet0_AverageLArQF;
    TH1F* m_hist_jet0_negE;
    TH1F* m_hist_jet0_time;
    TH2F* m_hist_jet0_etatime;

    TH1F* m_hist_dijet_dphi;
    TH1F* m_hist_dijet_pTasymm;

    vector<TString> m_muonDetectors;
    vector<TString> m_sideStr;

    void Write(TObject* hist);
};

HistTool::HistTool(TString name) {
  m_name = name;
  m_muonDetectors={"CSC","MdtI"};
  m_sideStr={"","_SideA","_SideC"};

  m_hist_NPV = new TH1F(name+"_NPV",";NPV",100,0,100);
  m_hist_mu = new TH1F(name+"_mu",";mu",100,0,100);
  m_hist_bcid = new TH1F(name+"_bcid",";bcid",3564,1,3565);
  m_hist_lbn = new TH1F(name+"_lbn",";lbn",10000,0,10000);
  m_hist_stableBeams = new TH1F(name+"_stableBeams_lbn",";lbn",10000,0,10000);

  m_hist_jet0_pt = new TH1F(name+"_jet0_pt","; jet0 p_{T} [GeV]",10000,0,10000);
  m_hist_jet0_eta = new TH1F(name+"_jet0_eta","",100,-5,5);
  m_hist_jet0_phi = new TH1F(name+"_jet0_phi","",360,-pi,pi);
  m_hist_jet0_etaphi = new TH2F(name+"_jet0_etaphi","",100,-5,5,360,-pi,pi);
  m_hist_jet0_emf = new TH1F(name+"_jet0_emf","",100,0,1);
  m_hist_jet0_hecf = new TH1F(name+"_jet0_hecf","",100,0,1); 
  m_hist_jet0_hecq = new TH1F(name+"_jet0_hecq","",100,-0.5,1.5);
  m_hist_jet0_larq = new TH1F(name+"_jet0_larq","",100,-0.5,1.5);
  m_hist_jet0_AverageLArQF = new TH1F(name+"_jet0_AverageLArQF","",100,-0.5,1.5);
  m_hist_jet0_negE = new TH1F(name+"_jet0_negE","",100,-200,10);
  m_hist_jet0_time = new TH1F(name+"_jet0_time","",500,-100,100);
  m_hist_jet0_etatime = new TH2F(name+"_jet0_etatime","",100,-5,5,200,-50,50);

  m_hist_dijet_dphi = new TH1F(name+"_dijet_dphi","",100,-pi,pi);
  m_hist_dijet_pTasymm = new TH1F(name+"_dijet_pTasymm","",100,0,1); 

  for (int i=0;i<2;i++) {
    m_hist_seg_deltaTheta[i] = new TH1F(name+"_MuonSegments_deltaTheta_" + m_muonDetectors[i],"",360,0,180);

    for (int iside=0;iside<3;iside++)
      m_hist_selectedSeg_xy[i][iside] = new TH2F(name+"_SelectedMuonSegments_xy_"+m_muonDetectors[i]+m_sideStr[iside],"",200,-4000,4000,200,-4000,4000);
    m_hist_selectedSeg_deltaTheta[i] = new TH1F(name+"_SelectedMuonSegments_deltaTheta_" + m_muonDetectors[i],"",360,0,180);
    m_hist_selectedSeg_t0[i] = new TH1F(name+"_SelectedMuonSegments_t0_" + m_muonDetectors[i],"",4000,-2000,2000); 

     m_hist_selectedSeg_phiDiff_selectedClus[i] = new TH1F(name+"_SelectedMuonSegments_phiDiff_selectedClus_"+m_muonDetectors[i],";#phi_{CSC}-#phi_{clus}",360,-180,180);
     m_hist_selectedSeg_rDiff_selectedClus[i] = new TH1F(name+"_SelectedMuonSegments_rDiff_selectedClus_"+m_muonDetectors[i],";r_{CSC}-r_{clus} [mm]",100,0,1000);

     m_hist_selectedSeg_phiDiffAC[i] = new TH1F(name+"_SelectedMuonSegments_phiDiffAC_"+m_muonDetectors[i],"", 360,-180,180);
     m_hist_selectedSeg_passPhiDiffAC_timeDiff[i] = new TH1F(name+"_SelectedMuonSegments_passPhiDiffAC_timeDiff"+m_muonDetectors[i],"",400,-200,200);
  }

  TString clusHistNames[3] = {"All","LAr","Tile"};
  for (int i=0;i<3;i++) {
    m_hist_selectedClus_t_z[i] = new TH2F(name+"_selectedClus_t_z_" + clusHistNames[i], "TopoClusters with radius and energy cut: " + clusHistNames[i] + ";z [mm];t [ns]",100,-8000,8000,100,-50,50);
    m_hist_matchedClus_t_z[i] = new TH2F(name+"_matchedClus_t_z_" + clusHistNames[i],
                                         "Matched TopoClusters: " + clusHistNames[i] + ";z [mm];t [ns]",100,-8000,8000,100,-50,50);
  }
  m_hist_selectedClus_emscale_e = new TH1F(name+"_selectedClus_emscale_e","TopoClusters with energy and radius cut; emscale energy [GeV]",100,0,500);
  m_hist_matchedClus_emscale_e = new TH1F(name+"_matchedClus_emscale_e","TopoClusters matched to MuonSegments; emscale energy [GeV]",100,0,500);
}

void HistTool::Fill(EventInfo& eventInfo,vector<TopoCluster*> topoClusters,vector<MuonSegment*> muonSegments,vector<Jet*> jets) {

  m_hist_NPV->Fill(eventInfo.NPV());
  m_hist_mu->Fill(eventInfo.mu());
  m_hist_bcid->Fill(eventInfo.bcid());
  m_hist_lbn->Fill(eventInfo.lbn());
  m_hist_stableBeams->Fill(eventInfo.lbn(), eventInfo.stableBeams());

  if (jets.size()) {
    m_hist_jet0_pt->Fill(jets[0]->pt()/1000);
    m_hist_jet0_eta->Fill(jets[0]->eta());
    m_hist_jet0_phi->Fill(jets[0]->phi());
    m_hist_jet0_etaphi->Fill(jets[0]->eta(),jets[0]->phi());
    m_hist_jet0_emf->Fill(jets[0]->emf());
    m_hist_jet0_hecf->Fill(jets[0]->hecf());
    m_hist_jet0_hecq->Fill(jets[0]->hecq());
    m_hist_jet0_larq->Fill(jets[0]->larq());
    m_hist_jet0_AverageLArQF->Fill(jets[0]->AverageLArQF()/65535);
    m_hist_jet0_negE->Fill(jets[0]->negE()/1000);
    m_hist_jet0_time->Fill(jets[0]->time());
    m_hist_jet0_etatime->Fill(jets[0]->eta(),jets[0]->time());
  }

  if (jets.size()>=2) {
    m_hist_dijet_dphi->Fill(TVector2::Phi_mpi_pi(jets[0]->phi()-jets[1]->phi()));
    //m_hist_dijet_dphi
    float pTasymm = TMath::Abs(jets[0]->pt()-jets[1]->pt()) /
                             ((jets[0]->pt()+jets[1]->pt()) / 2);
    m_hist_dijet_pTasymm->Fill(pTasymm);
  } 

  for (int i=0;i<muonSegments.size();i++) {
    int idet = -1;
    if (muonSegments[i]->isCSC()) idet=0;
    else if (muonSegments[i]->isMDTInnerEndCap()) idet=1;
    else continue;

    m_hist_seg_deltaTheta[idet]->Fill(TMath::Abs(muonSegments[i]->deltaTheta()*180/pi)); 

    if (muonSegments[i]->passesDeltaTheta()) {
      for (int iside=0;iside<3;iside++)
        if (iside==0||(iside==1&&muonSegments[i]->z()>0)||(iside==2&&muonSegments[i]->z()<0))
          m_hist_selectedSeg_xy[idet][iside]->Fill(muonSegments[i]->x(),muonSegments[i]->y());
      m_hist_selectedSeg_deltaTheta[idet]->Fill(TMath::Abs(muonSegments[i]->deltaTheta()*180/pi));
      m_hist_selectedSeg_t0[idet]->Fill(muonSegments[i]->t0());
    }
  }

  for (int i=0;i<muonSegments.size();i++)
    if (muonSegments[i]->passesDeltaTheta())
      if (muonSegments[i]->z() < 0)
        for (int j=0;j<muonSegments.size();j++)
          if (muonSegments[j]->passesDeltaTheta())
            if (muonSegments[j]->z() > 0) {
               float phiDiffAC = TVector2::Phi_mpi_pi(muonSegments[i]->phi()-muonSegments[j]->phi())*180/pi;
               int idet=-1;
               if (muonSegments[i]->isCSC() && muonSegments[j]->isCSC()) idet=0;
               else if (muonSegments[i]->isMDTInnerEndCap() && muonSegments[j]->isMDTInnerEndCap()) idet=1;
               else continue;
               
               m_hist_selectedSeg_phiDiffAC[idet]->Fill(phiDiffAC);
               if (TMath::Abs(phiDiffAC)<4) {
                 float timeDiff = muonSegments[i]->t0()-muonSegments[j]->t0();
                 m_hist_selectedSeg_passPhiDiffAC_timeDiff[idet]->Fill(timeDiff);
               }
            }
 
  for (int i=0;i<topoClusters.size();i++)
    if (topoClusters[i]->passesEnergyAndRadius())
      for (int j=0;j<muonSegments.size();j++)
        if (muonSegments[j]->passesDeltaTheta()) {

          TopoCluster* clus = topoClusters[i];
          MuonSegment* seg = muonSegments[j];

          float dphi = TVector2::Phi_mpi_pi(clus->phi() - seg->phi()) * 180/pi;
          float dr = TMath::Abs(clus->rPerp() - seg->rPerp());

          int idet=-1;
          if (seg->isCSC()) idet=0;
          else if (seg->isMDTInnerEndCap()) idet=1;
          else continue;

          m_hist_selectedSeg_phiDiff_selectedClus[idet]->Fill(dphi);
          m_hist_selectedSeg_rDiff_selectedClus[idet]->Fill(dr);
        }

   // TopoCluster time versus z histograms for clusters matched to muon segments, and also for non-matched clusters. Also made separately for LAr and Tile clusters.
  for (int i=0;i<topoClusters.size();i++)
    if (topoClusters[i]->passesEnergyAndRadius()) {

      TopoCluster* clus = topoClusters[i];

      // non-matched clusters
      m_hist_selectedClus_t_z[0]->Fill(clus->z(), clus->time() );
      if (clus->isLAr())  m_hist_selectedClus_t_z[1]->Fill(clus->z(), clus->time() );
      if (clus->isTile()) m_hist_selectedClus_t_z[2]->Fill(clus->z(), clus->time() );
      m_hist_selectedClus_emscale_e->Fill(clus->emscale_e()/1000);

      // matched clusters
      if (clus->isMatched()) {
        m_hist_matchedClus_t_z[0]->Fill(clus->z(), clus->time() );
        if (clus->isLAr()) m_hist_matchedClus_t_z[1]->Fill(clus->z(), clus->time() );
        if (clus->isTile()) m_hist_matchedClus_t_z[2]->Fill(clus->z(), clus->time() );

        m_hist_matchedClus_emscale_e->Fill(clus->emscale_e()/1000);
      }
    }

}

void HistTool::Write() {
  Write(m_hist_NPV);
  Write(m_hist_bcid);
  Write(m_hist_mu);
  Write(m_hist_lbn);
  Write(m_hist_stableBeams);

  Write(m_hist_jet0_pt);
  Write(m_hist_jet0_eta);
  Write(m_hist_jet0_phi);
  Write(m_hist_jet0_etaphi); 
  Write(m_hist_jet0_emf);
  Write(m_hist_jet0_hecf);
  Write(m_hist_jet0_hecq);
  Write(m_hist_jet0_larq);
  Write(m_hist_jet0_AverageLArQF);
  Write(m_hist_jet0_negE);
  Write(m_hist_jet0_time); 
  Write(m_hist_jet0_etatime);

  for (int i=0;i<2;i++) {
    Write(m_hist_seg_deltaTheta[i]);
    for (int iside=0;iside<3;iside++) Write(m_hist_selectedSeg_xy[i][iside]);
    Write(m_hist_selectedSeg_deltaTheta[i]);
    Write(m_hist_selectedSeg_t0[i]);
    m_hist_selectedSeg_phiDiff_selectedClus[i]->SetMinimum(0);
    Write(m_hist_selectedSeg_phiDiff_selectedClus[i]);
    Write(m_hist_selectedSeg_rDiff_selectedClus[i]);
    m_hist_selectedSeg_phiDiffAC[i]->SetMinimum(0);
    Write(m_hist_selectedSeg_phiDiffAC[i]);
    Write(m_hist_selectedSeg_passPhiDiffAC_timeDiff[i]);
  }

  for (int i=0;i<3;i++) Write(m_hist_selectedClus_t_z[i]);
  for (int i=0;i<3;i++) Write(m_hist_matchedClus_t_z[i]);
  Write(m_hist_selectedClus_emscale_e);
  Write(m_hist_matchedClus_emscale_e);

}

void HistTool::Write(TObject* hist) {
  hist->Write(TString(hist->GetName()).ReplaceAll(name()+"_",""));
}

class SimpleTagger {
  public:
         SimpleTagger(TString sample, int evtMax, TString outDir, bool saveOutputTree);
        ~SimpleTagger();
   
    void Run();
    void Print();
    
    void OneSidedNoTimeLoose(); 
    
    bool is_dR_dPhi_Matched(TopoCluster* cluster, MuonSegment* segment);
    bool is_dR_dPhi_Matched(TopoCluster* cluster);    

    void setTopoClusterMatchedFlags();
    void fillHistograms();
    void fillOutputTree();

    bool passesEventSelection(TString selection);
    bool passesJetCleaningNoiseCuts();

    void Save();

    bool isDebugListEvent();

  private:
    TString m_sample;
    TString m_projectTag;
    TString m_runNumberStr;
    TString m_period;
    TString m_stream;
    TFile* m_file;
    TTree* m_tree;
    int m_entry;
    bool m_debug;
    int m_evtMax;
    TString m_outDir;
    bool m_saveOutputTree;

    int m_eventNumber;
    unsigned int m_backgroundFlag;
    int m_bcid;
    int m_NPV;
    int m_lbn;
    float m_mu;
    bool m_stableBeams;
    int m_bs_isUnpaired;
    int m_bs_isUnpairedB1;
    int m_bs_isUnpairedB2; 
    int m_bs_isUnpairedIsolated;
    int m_bs_isPaired;

    vector<float>* m_jets_pt;
    vector<float>* m_jets_eta;
    vector<float>* m_jets_emf;
    vector<float>* m_jets_hecf;
    vector<float>* m_jets_hecq;
    vector<float>* m_jets_larq;
    vector<float>* m_jets_AverageLArQF;
    vector<float>* m_jets_negE;
    vector<float>* m_jets_phi;
    vector<float>* m_jets_time;

    vector<float>* m_topoClusters_emscale_e;
    vector<float>* m_topoClusters_rPerp;
    vector<float>* m_topoClusters_phi;
    vector<float>* m_topoClusters_z;
    vector<float>* m_topoClusters_time;
    vector<int>* m_topoClusters_fracSamplingMaxIndex;

    vector<TopoCluster*> m_topoClusters;
    vector<MuonSegment*> m_muonSegments;
    vector<Jet*> m_jets;

    int m_nSelectedTopoClusters;
    int m_nSelectedMuonSegments;

    vector<float>* m_MuonSegments_x;
    vector<float>* m_MuonSegments_y;
    vector<float>* m_MuonSegments_z;
    vector<float>* m_MuonSegments_thetaPos;
    vector<float>* m_MuonSegments_thetaDir;
    vector<float>* m_MuonSegments_chamberIndex;
    vector<float>* m_MuonSegments_phi;
    vector<float>* m_MuonSegments_t0;

    vector<float>* m_NCB_MuonSegments_x;
    vector<float>* m_NCB_MuonSegments_y;
    vector<float>* m_NCB_MuonSegments_z;   
    vector<float>* m_NCB_MuonSegments_thetaPos;
    vector<float>* m_NCB_MuonSegments_thetaDir; 
    vector<float>* m_NCB_MuonSegments_chamberIndex;
    //vector<float>* m_NCB_MuonSegments_phi;
    vector<float>* m_NCB_MuonSegments_t0;

    TString m_outputFileName;
    TFile* m_outputFile;
    TTree* m_outputTree;
    vector<int> m_output_topoClus_passesEnergyAndRadius;
    vector<int> m_output_topoClus_passesExpectedTime;
    vector<float> m_output_topoClus_expectedTimeAtoC;
    vector<float> m_output_topoClus_expectedTimeCtoA;
    vector<int> m_output_topoClus_passes_dr_dphi_selectedMuonSeg;

    int m_simpleTaggerBackgroundFlag;

    vector<HistTool*> m_histTool;

};

SimpleTagger::SimpleTagger(TString sample, int evtMax, TString outDir, bool saveOutputTree) {
  m_sample = sample;
  m_evtMax = evtMax;
  m_outDir = outDir;
  m_saveOutputTree = saveOutputTree;

  TString version;
  getDatasetInfo(sample, m_projectTag, m_runNumberStr, m_period, m_stream, version);

  m_file = TFile::Open(m_sample);
  if (!m_file) throw "file pointer is null";
  if (!m_file->IsOpen()) throw "failed to  open root file";

  m_tree = (TTree*)m_file->Get("CollectionTree");
  cout << "tree entries: " << m_tree->GetEntries() << endl;

  m_jets_pt=0;
  m_jets_eta=0;
  m_jets_emf=0;
  m_jets_hecf=0;
  m_jets_hecq=0;
  m_jets_larq=0;
  m_jets_AverageLArQF=0;
  m_jets_negE=0;
  m_jets_phi=0;
  m_jets_time=0;
  m_topoClusters_emscale_e = 0;
  m_topoClusters_rPerp = 0;
  m_topoClusters_phi = 0;
  m_topoClusters_z = 0;
  m_topoClusters_time = 0;
  m_topoClusters_fracSamplingMaxIndex = 0;
  m_MuonSegments_x = 0;
  m_MuonSegments_y = 0;
  m_MuonSegments_z = 0;
  m_MuonSegments_thetaPos = 0;
  m_MuonSegments_thetaDir = 0;
  m_MuonSegments_chamberIndex = 0;
  m_MuonSegments_phi = 0;
  m_MuonSegments_t0 = 0;
  m_NCB_MuonSegments_x = 0;
  m_NCB_MuonSegments_y = 0;
  m_NCB_MuonSegments_z = 0;
  m_NCB_MuonSegments_thetaPos = 0;
  m_NCB_MuonSegments_thetaDir = 0;
  m_NCB_MuonSegments_chamberIndex = 0;
  m_NCB_MuonSegments_t0 = 0;
  
  m_tree->SetBranchStatus("*",0);
  m_tree->SetBranchStatus("EventNumber",1);
  m_tree->SetBranchStatus("backgroundFlag",1);
  m_tree->SetBranchStatus("NPV",1);
  m_tree->SetBranchStatus("StableBeams",1);
  m_tree->SetBranchStatus("bcid",1);
  m_tree->SetBranchStatus("averageInteractionsPerCrossing",1);
  m_tree->SetBranchStatus("lbn",1);
  m_tree->SetBranchStatus("BunchStructure.isUnpaired",1);
  m_tree->SetBranchStatus("BunchStructure.isUnpairedB1",1);
  m_tree->SetBranchStatus("BunchStructure.isUnpairedB2",1);
  m_tree->SetBranchStatus("BunchStructure.isUnpairedIsolated",1);
  m_tree->SetBranchStatus("BunchStructure.isPaired",1);

  m_tree->SetBranchStatus("AntiKt4EMTopoJets.pt",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.eta",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.emf",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.hecf",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.larq",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.hecq",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.AverageLArQF",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.negE",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.phi",1);
  m_tree->SetBranchStatus("AntiKt4EMTopoJets.time",1);

  m_tree->SetBranchStatus("TopoClusters.emscale_e",1);
  m_tree->SetBranchStatus("TopoClusters.rPerp",1);
  m_tree->SetBranchStatus("TopoClusters.phi",1); 
  m_tree->SetBranchStatus("TopoClusters.z",1);
  m_tree->SetBranchStatus("TopoClusters.time",1);
  m_tree->SetBranchStatus("TopoClusters.fracSamplingMaxIndex",1);
  m_tree->SetBranchStatus("*MuonSegments.x",1);
  m_tree->SetBranchStatus("*MuonSegments.y",1);
  m_tree->SetBranchStatus("*MuonSegments.z",1);
  m_tree->SetBranchStatus("*MuonSegments.thetaPos",1);
  m_tree->SetBranchStatus("*MuonSegments.thetaDir",1); 
  m_tree->SetBranchStatus("*MuonSegments.chamberIndex",1);
  m_tree->SetBranchStatus("*MuonSegments.t0",1);  
 
  m_tree->SetBranchAddress("EventNumber",&m_eventNumber);
  m_tree->SetBranchAddress("backgroundFlag",&m_backgroundFlag);
  m_tree->SetBranchAddress("NPV",&m_NPV);
  m_tree->SetBranchAddress("bcid",&m_bcid);
  m_tree->SetBranchAddress("lbn",&m_lbn);
  m_tree->SetBranchAddress("averageInteractionsPerCrossing",&m_mu);
  m_tree->SetBranchAddress("StableBeams",&m_stableBeams);
  m_tree->SetBranchAddress("BunchStructure.isUnpaired",&m_bs_isUnpaired);
  m_tree->SetBranchAddress("BunchStructure.isUnpairedB1",&m_bs_isUnpairedB1);
  m_tree->SetBranchAddress("BunchStructure.isUnpairedB2",&m_bs_isUnpairedB2); 
  m_tree->SetBranchAddress("BunchStructure.isUnpairedIsolated",&m_bs_isUnpairedIsolated); 
  m_tree->SetBranchAddress("BunchStructure.isPaired",&m_bs_isPaired);

  m_tree->SetBranchAddress("AntiKt4EMTopoJets.pt",&m_jets_pt);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.eta",&m_jets_eta); 
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.emf",&m_jets_emf);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.hecf",&m_jets_hecf);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.hecq",&m_jets_hecq);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.larq",&m_jets_larq);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.AverageLArQF",&m_jets_AverageLArQF);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.negE",&m_jets_negE);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.phi",&m_jets_phi);
  m_tree->SetBranchAddress("AntiKt4EMTopoJets.time",&m_jets_time);

  m_tree->SetBranchAddress("TopoClusters.emscale_e",&m_topoClusters_emscale_e);
  m_tree->SetBranchAddress("TopoClusters.rPerp",&m_topoClusters_rPerp);
  m_tree->SetBranchAddress("TopoClusters.phi",&m_topoClusters_phi);
  m_tree->SetBranchAddress("TopoClusters.z",&m_topoClusters_z);
  m_tree->SetBranchAddress("TopoClusters.time",&m_topoClusters_time);
  m_tree->SetBranchAddress("TopoClusters.fracSamplingMaxIndex",&m_topoClusters_fracSamplingMaxIndex);
  
  m_tree->SetBranchAddress("MuonSegments.x",&m_MuonSegments_x);
  m_tree->SetBranchAddress("MuonSegments.y",&m_MuonSegments_y);
  m_tree->SetBranchAddress("MuonSegments.z",&m_MuonSegments_z);
  m_tree->SetBranchAddress("MuonSegments.thetaPos",&m_MuonSegments_thetaPos);
  m_tree->SetBranchAddress("MuonSegments.thetaDir",&m_MuonSegments_thetaDir);
  m_tree->SetBranchAddress("MuonSegments.chamberIndex",&m_MuonSegments_chamberIndex);
  m_tree->SetBranchAddress("MuonSegments.t0",&m_MuonSegments_t0);  

  m_tree->SetBranchAddress("NCB_MuonSegments.x",&m_NCB_MuonSegments_x);
  m_tree->SetBranchAddress("NCB_MuonSegments.y",&m_NCB_MuonSegments_y);
  m_tree->SetBranchAddress("NCB_MuonSegments.z",&m_NCB_MuonSegments_z);
  m_tree->SetBranchAddress("NCB_MuonSegments.thetaPos",&m_NCB_MuonSegments_thetaPos);
  m_tree->SetBranchAddress("NCB_MuonSegments.thetaDir",&m_NCB_MuonSegments_thetaDir);
  m_tree->SetBranchAddress("NCB_MuonSegments.chamberIndex",&m_NCB_MuonSegments_chamberIndex);
  m_tree->SetBranchAddress("NCB_MuonSegments.t0",&m_NCB_MuonSegments_t0);  

  m_outputFileName = m_outDir + "/" + m_projectTag + "." + m_runNumberStr + "." + m_period+ "." + m_stream + ".HIST.root";
  m_outputFile = TFile::Open(m_outputFileName,"recreate");
  if (!m_outputFile) throw "failed to create output file";
  if (m_saveOutputTree) {
    m_outputTree = new TTree("SimpleTagger","");
    m_outputTree->Branch("SimpleTagger.backgroundFlag",&m_simpleTaggerBackgroundFlag);
    m_outputTree->Branch("TopoClusters.passesEnergyAndRadius","vector<int>",&m_output_topoClus_passesEnergyAndRadius);
    m_outputTree->Branch("TopoClusters.passesExpectedTime","vector<int>",&m_output_topoClus_passesExpectedTime);
    m_outputTree->Branch("TopoClusters.nSelected",&m_nSelectedTopoClusters);
    m_outputTree->Branch("TopoClusters.expectedTimeAtoC",&m_output_topoClus_expectedTimeAtoC);
    m_outputTree->Branch("TopoClusters.expectedTimeCtoA",&m_output_topoClus_expectedTimeCtoA);
    m_outputTree->Branch("TopoClusters.passes_dr_dphi_selectedMuonSeg","vector<int>",&m_output_topoClus_passes_dr_dphi_selectedMuonSeg);
    m_outputTree->Branch("MuonSegments.nSelected",&m_nSelectedMuonSegments);
  }

  m_debug=false;
  m_histTool.push_back(new HistTool("NoEventSelection"));
  m_histTool.push_back(new HistTool("passGRL"));
  m_histTool.push_back(new HistTool("passGRL&&StableBeams")); 
  //event selection for unpaired bunches
  //m_histTool.push_back(new HistTool("NPV==0&&passGRL&&StableBeams")); 
  //m_histTool.push_back(new HistTool("NPV==0&&passGRL&&StableBeams&&BunchStructure.isUnpaired"));
  //m_histTool.push_back(new HistTool("NPV==0&&passGRL&&StableBeams&&BunchStructure.isUnpaired&&!jetCleaning_noise"));
  //m_histTool.push_back(new HistTool("NPV==0&&passGRL&&StableBeams&&BunchStructure.isUnpaired&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000"));
  //m_histTool.push_back(new HistTool("NPV==0&&passGRL&&StableBeams&&BunchStructure.isUnpaired&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0")); 
  m_histTool.push_back(new HistTool("NPV==0&&passGRL&&StableBeams&&BunchStructure.isUnpairedB1&&BunchStructure.isUnpairedIsolated&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0")); 
  m_histTool.push_back(new HistTool("NPV==0&&passGRL&&StableBeams&&BunchStructure.isUnpairedB2&&BunchStructure.isUnpairedIsolated&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0"));
  m_histTool.push_back(new HistTool("NPV==0&&passGRL&&StableBeams&&BunchStructure.isUnpairedIsolated&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0"));
  //event selection for colliding bunches
  m_histTool.push_back(new HistTool("StableBeams&&BunchStructure.isPaired&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0"));

  m_histTool.push_back(new HistTool("StableBeams&&L1_MU10&&BunchStructure.isPaired&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0"));

  m_histTool.push_back(new HistTool("StableBeams&&BunchStructure.isPaired&&!jetCleaning_noise&&AntiKt4EMTopoJets.pt[0]>20000&&Abs(AntiKt4EMTopoJets.eta[0])<3.0&&AntiKt4EMTopoJets.pt[1]>20000&&dphi(jet0,jet1)>2.8&&pTasymm<0.4"));  
}

SimpleTagger::~SimpleTagger() {
  m_file->Close();
}

bool SimpleTagger::is_dR_dPhi_Matched(TopoCluster* clus, MuonSegment* seg) {
  if ( !(seg->isCSC() || seg->isMDTInnerEndCap()) ) return false;

  float dphi = TMath::Abs(TVector2::Phi_mpi_pi(clus->phi() - seg->phi()));
  float dr = TMath::Abs(clus->rPerp() - seg->rPerp());

  if ( dphi > 4*pi/180) return false;
  if ( seg->isCSC() && dr > 300) return false;
  if ( seg->isMDTInnerEndCap() && dr > 800) return false;  

  return true;   
}

bool SimpleTagger::is_dR_dPhi_Matched(TopoCluster* clus) {
  
  if (clus->passesEnergyAndRadius())
    for (int j=0;j<m_muonSegments.size();j++)
      if (m_muonSegments[j]->passesDeltaTheta())
        if (is_dR_dPhi_Matched(clus,m_muonSegments[j]))
          return true;

  return false;

}

bool SimpleTagger::isDebugListEvent() {
  for (unsigned int event : debugListEvents)
    if (m_eventNumber == event)
      return true;
  return false;
}

void SimpleTagger::fillHistograms() {
  EventInfo eventInfo(m_NPV,m_mu,m_lbn,m_bcid,m_stableBeams);
  
  for (int i=0;i<m_histTool.size();i++) 
    if (passesEventSelection(m_histTool[i]->name()))
      m_histTool[i]->Fill(eventInfo, m_topoClusters, m_muonSegments, m_jets);

}

void SimpleTagger::fillOutputTree() {
  
  m_output_topoClus_passesEnergyAndRadius.clear();
  m_output_topoClus_passesExpectedTime.clear();
  m_output_topoClus_expectedTimeAtoC.clear();
  m_output_topoClus_expectedTimeCtoA.clear();
  m_output_topoClus_passes_dr_dphi_selectedMuonSeg.clear();
  
  for (TopoCluster* clus : m_topoClusters) {
    m_output_topoClus_passesEnergyAndRadius.push_back(clus->passesEnergyAndRadius());
    m_output_topoClus_passesExpectedTime.push_back(clus->passesExpectedTime());

    bool isMatched = false;    

    m_output_topoClus_expectedTimeAtoC.push_back(clus->getExpectedTime("AtoC"));
    m_output_topoClus_expectedTimeCtoA.push_back(clus->getExpectedTime("CtoA"));

    for (MuonSegment* seg : m_muonSegments) 
      if (seg->passesDeltaTheta())  
        if (is_dR_dPhi_Matched(clus, seg))
          isMatched = true;
    m_output_topoClus_passes_dr_dphi_selectedMuonSeg.push_back(isMatched);
    
  }

  if (m_saveOutputTree)
    m_outputTree->Fill();

}

bool SimpleTagger::passesEventSelection(TString selection) {
  if (selection == "NoEventSelection") return true;
  if (selection.Contains("NPV==0") && m_NPV!=0) return false;
  if (selection.Contains("StableBeams") && !m_stableBeams) return false;
  if (selection.Contains("BunchStructure.isUnpaired") && !m_bs_isUnpaired) return false;
  if (selection.Contains("BunchStructure.isUnpairedB1") && !m_bs_isUnpairedB1) return false;
  if (selection.Contains("BunchStructure.isUnpairedB2") && !m_bs_isUnpairedB2) return false; 
  if (selection.Contains("BunchStructure.isUnpairedIsolated") && !m_bs_isUnpairedIsolated) return false; 
  if (selection.Contains("BunchStructure.isPaired") && !m_bs_isPaired) return false; 
  if (selection.Contains("!jetCleaning_noise") && passesJetCleaningNoiseCuts()) return false;
  if (selection.Contains("AntiKt4EMTopoJets.pt[0]>20000")) {
    if (m_jets_pt->size()==0) return false;
    if (m_jets_pt->at(0)<20000) return false;
  }
  if (selection.Contains("&&Abs(AntiKt4EMTopoJets.eta[0])<3.0")) {
    if (m_jets_eta->size()==0) return false;
    if (TMath::Abs(m_jets_eta->at(0))>=3.0) return false;
  }
  if (selection.Contains("AntiKt4EMTopoJets.pt[1]>20000")) {
    if (m_jets_pt->size()<2) return false;
    if (m_jets_pt->at(1)<20000) return false;
    if (selection.Contains("dphi(jet0,jet1)>2.8"))
      if (TVector2::Phi_mpi_pi(m_jets_phi->at(0)-m_jets_phi->at(1))<2.8) return false;
    if (selection.Contains("pTasymm<0.4")) {
      float pTasymm = TMath::Abs(m_jets_pt->at(0)-m_jets_pt->at(1)) /
                               ((m_jets_pt->at(0)+m_jets_pt->at(1)) / 2);
      if (pTasymm>0.4) return false; 
      
    }
  } 

  return true;
}

bool SimpleTagger::passesJetCleaningNoiseCuts() {

  for (unsigned int i=0;i<m_jets_pt->size();i++) 
    if (m_jets_pt->at(i)>20000) {
      if (m_jets_hecf->at(i)>0.5 && m_jets_hecq->at(i)>0.5 && m_jets_AverageLArQF->at(i)/65535>0.8) return true;
      if (TMath::Abs(m_jets_negE->at(i))>60000) return true;
      if (m_jets_emf->at(i)>0.95 && m_jets_larq->at(i)>0.8 && m_jets_AverageLArQF->at(i)/65535>0.8 && TMath::Abs(m_jets_eta->at(i))<2.8) return true;
    }

  return false;
}

void SimpleTagger::setTopoClusterMatchedFlags() {
  for (int i=0;i<m_topoClusters.size();i++)
    if (m_topoClusters[i]->passesEnergyAndRadius())
      if (is_dR_dPhi_Matched(m_topoClusters[i]))
        m_topoClusters[i]->setMatched();
}

void SimpleTagger::OneSidedNoTimeLoose() {
  bool passesOneSidedNoTimeLoose = false; 
  for (int i=0;i<m_topoClusters.size();i++) 
    if (m_topoClusters[i]->passesEnergyAndRadius())
      if (m_topoClusters[i]->passesExpectedTime())  
        for (int j=0;j<m_muonSegments.size();j++)
          if (m_muonSegments[j]->passesDeltaTheta())
            if (is_dR_dPhi_Matched(m_topoClusters[i],m_muonSegments[j]))
                passesOneSidedNoTimeLoose = true;

  if (passesOneSidedNoTimeLoose) 
    m_simpleTaggerBackgroundFlag |= (1<<19);

}

void SimpleTagger::Print() {
  cout << "-------------------------------------------------------------------------------------------" << endl;
  cout << "eventNumber=" << m_eventNumber << endl
       << "backgroundFlag bit 17-20: " 
       << ((m_backgroundFlag & (1<<17))!=0) << " " 
       << ((m_backgroundFlag & (1<<18))!=0)  << " " 
       << ((m_backgroundFlag & (1<<19))!=0) << " " 
       << ((m_backgroundFlag & (1<<20))!=0)
       << endl;
  
  for (TopoCluster* topoClus : m_topoClusters) 
    if (topoClus->passesEnergyAndRadius()) 
      topoClus->Print();
  cout << endl;

  for (MuonSegment* muonSeg : m_muonSegments)
    muonSeg->Print();  
  cout << endl;

  if (m_nSelectedTopoClusters && m_nSelectedMuonSegments) {
    cout << "clusters-segment delta-r delta-phi matching matrix:" << endl;
    for (int i=0;i<m_topoClusters.size();i++)
      if (m_topoClusters[i]->passesEnergyAndRadius() && m_topoClusters[i]->passesExpectedTime()) {
        cout << Form("  TopoCluster #%2i: ", i); 
        for (int j=0;j<m_muonSegments.size();j++)
          if (m_muonSegments[j]->passesDeltaTheta())
            cout << (is_dR_dPhi_Matched(m_topoClusters[i], m_muonSegments[j])) << " ";
        cout << endl;
      }
  }

  cout << "passes SimpleTagger bit 19: " << ((m_simpleTaggerBackgroundFlag & (1<<19)) !=0) << endl;

}

void SimpleTagger::Run() {
 
  int entry_max = 0;
  if (m_evtMax==-1) entry_max=m_tree->GetEntries();
  else entry_max=m_evtMax;
  for (m_entry=0; m_entry<entry_max; m_entry++) {
    if (m_entry % 5000 == 0) 
      cout << "entry #" << m_entry << endl;
    m_tree->GetEntry(m_entry);
         
    m_simpleTaggerBackgroundFlag = 0;
    m_topoClusters.clear();
    m_muonSegments.clear();
    m_jets.clear();
    m_nSelectedTopoClusters = 0;
    for (unsigned j=0;j<m_topoClusters_emscale_e->size();j++)  {
      TopoCluster* topoCluster = new TopoCluster(m_topoClusters_emscale_e->at(j),m_topoClusters_rPerp->at(j),m_topoClusters_phi->at(j),
                                                 m_topoClusters_z->at(j),m_topoClusters_time->at(j),m_topoClusters_fracSamplingMaxIndex->at(j));
      m_topoClusters.push_back( topoCluster );
      if (topoCluster->passesEnergyAndRadius()) m_nSelectedTopoClusters++;
    }

    m_nSelectedMuonSegments = 0;
    for (unsigned int j=0;j<m_MuonSegments_x->size();j++) 
      if (m_MuonSegments_chamberIndex->at(j) == 7 || m_MuonSegments_chamberIndex->at(j) == 8) {
        MuonSegment* muonSeg = new MuonSegment(m_MuonSegments_x->at(j), m_MuonSegments_y->at(j), m_MuonSegments_z->at(j), m_MuonSegments_thetaPos->at(j), m_MuonSegments_thetaDir->at(j), m_MuonSegments_chamberIndex->at(j), m_MuonSegments_t0->at(j));
        m_muonSegments.push_back(muonSeg);
        if (muonSeg->passesDeltaTheta()) m_nSelectedMuonSegments++;
      }  

    for (unsigned int j=0;j<m_NCB_MuonSegments_x->size();j++) 
      if (m_NCB_MuonSegments_chamberIndex->at(j) == 15 || m_NCB_MuonSegments_chamberIndex->at(j) == 16) {
        MuonSegment* muonSeg = new MuonSegment(m_NCB_MuonSegments_x->at(j), m_NCB_MuonSegments_y->at(j), m_NCB_MuonSegments_z->at(j), m_NCB_MuonSegments_thetaPos->at(j), m_NCB_MuonSegments_thetaDir->at(j), m_NCB_MuonSegments_chamberIndex->at(j), m_NCB_MuonSegments_t0->at(j));
        m_muonSegments.push_back(muonSeg);
        if (muonSeg->passesDeltaTheta()) m_nSelectedMuonSegments++;
    }  

    for (unsigned int i=0;i<m_jets_pt->size();i++) {
      Jet* jet = new Jet(m_jets_pt->at(i), m_jets_eta->at(i),m_jets_emf->at(i), m_jets_hecf->at(i),m_jets_hecq->at(i),m_jets_larq->at(i),m_jets_AverageLArQF->at(i),m_jets_negE->at(i),m_jets_phi->at(i),m_jets_time->at(i));
      m_jets.push_back(jet);
    }

    setTopoClusterMatchedFlags();

    OneSidedNoTimeLoose();

    fillHistograms();
    fillOutputTree();

    if (m_nSelectedTopoClusters)
      //if (isDebugListEvent()) 
      if (m_debug)
        Print();

    for (TopoCluster* topoCluster : m_topoClusters) delete topoCluster;
    for (MuonSegment* muonSegment : m_muonSegments) delete muonSegment;
    for (Jet* jet : m_jets) delete jet;

  }
  
}

void SimpleTagger::Save() {
  cout << "Saving histograms to: " << m_outputFileName << endl;
  m_outputFile->cd();
  for (int i=0;i<m_histTool.size();i++) {
    m_outputFile->mkdir(m_histTool[i]->name());
    m_outputFile->cd(m_histTool[i]->name());
    m_histTool[i]->Write();
    m_outputFile->cd("..");
  }

  if (m_saveOutputTree)
    m_outputTree->Write();

  m_outputFile->Close();
}

int main(int argc, char** argv) {

  TString sample="";
  int evtMax = -1;
  TString outDir="";
  bool saveOutputTree=false;
  for (int i=1;i<argc;i++) {
    if (TString(argv[i]) == "--inDS")
      sample = argv[i+1];
    if (TString(argv[i]) == "--EvtMax")
      evtMax = TString(argv[i+1]).Atoi();
    if (TString(argv[i]) == "--outDir")
      outDir = argv[i+1];
    if (TString(argv[i]) == "--saveOutputTree")
      saveOutputTree=true;
  }
  if (sample=="" || outDir=="") {
    cout<<"Bad syntax." << endl;
    return 1;
  }

  cout << "sample=" << sample << endl;
  cout << "outDir=" << outDir << endl;
  cout << "evtMax=" << evtMax << endl;
  try {
    SimpleTagger simpleTagger(sample, evtMax, outDir, saveOutputTree);
    simpleTagger.Run();
    simpleTagger.Save();
  } catch (const char* err) {
    cout << err << endl;
  }
  cout <<"Done."<<endl;
  return 0;
}
