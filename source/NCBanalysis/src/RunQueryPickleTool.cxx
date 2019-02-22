#include "NCBanalysis/RunQueryPickleTool.h"

RunQueryPickleTool::RunQueryPickleTool(const string& type, const string& name, const IInterface* parent) :
  AthAlgTool(type,name,parent) {
  m_runNumber = -1;
}

RunQueryPickleTool::~RunQueryPickleTool() {
}

StatusCode RunQueryPickleTool::initialize() {

  return StatusCode::SUCCESS;
}

StatusCode RunQueryPickleTool::configure() {

  string configFile = PathResolverFindCalibFile(Form("NCBanalysis/RunQueryPickleTool/%i.root", m_runNumber));
  ATH_MSG_INFO ( "RunQueryPickleTool config file: " << configFile );

  TFile* m_configfile = TFile::Open(configFile.data());
  if (!m_configfile) throw "RunQueryPickleTool: config file pointer is null.";
  if (!m_configfile->IsOpen()) throw "RunQueryPickleTool: failed to open config file.";     
  TTree* m_configtree = (TTree*)m_configfile->Get("CollectionTree");
  if (!m_configtree) throw "CollectionTree pointer is null";
 
  m_stablebeams_firstlb=0;
  m_stablebeams_lastlb=0;
  m_stablebeams_value=0;

  m_configtree->SetBranchStatus("*",0);
  m_configtree->SetBranchStatus("stablebeams_firstlb",1);
  m_configtree->SetBranchStatus("stablebeams_lastlb",1); 
  m_configtree->SetBranchStatus("stablebeams_value",1);
  m_configtree->SetBranchAddress("stablebeams_firstlb",&m_stablebeams_firstlb);
  m_configtree->SetBranchAddress("stablebeams_lastlb",&m_stablebeams_lastlb);
  m_configtree->SetBranchAddress("stablebeams_value",&m_stablebeams_value);  

  if (m_configtree->GetEntries()!=1) throw "CollectionTree should have exactly 1 entry.";
  m_configtree->GetEntry(0);
  for (unsigned int i=0;i<m_stablebeams_value->size();i++)
    ATH_MSG_INFO("firstlb=" << m_stablebeams_firstlb->at(i)
              << " lastlb=" << m_stablebeams_lastlb->at(i)
              << " value=" << m_stablebeams_value->at(i) << endl); 
  //fconfig->Close();
  return StatusCode::SUCCESS;
}

bool RunQueryPickleTool::isStableBeams(unsigned int lbn) {
  for (unsigned int i=0;i<m_stablebeams_value->size();i++)
    if (m_stablebeams_firstlb->at(i) <= lbn && lbn <= m_stablebeams_lastlb->at(i))
      if (m_stablebeams_value->at(i))
        return true;
  return false;
}
