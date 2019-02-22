
#include "NCBanalysis/BunchStructureTool.h"

BunchStructureTool::BunchStructureTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type,name,parent) {
  declareProperty("debug", m_debug = false);
  ATH_MSG_INFO("BunchStructureTool constructor");
}

bool BunchStructureTool::isUnpairedBeam1(int bcid, int lb) {

  for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
    if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
      for (int bcid1 : unpaired_beam1Vec[lb_index])
        if (bcid == bcid1)
          return true;
  return false;
}

bool BunchStructureTool::isUnpairedBeam2(int bcid, int lb) {

  for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
    if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
      for (int bcid2 : unpaired_beam2Vec[lb_index])
        if (bcid == bcid2)
          return true;
  return false;
}

bool BunchStructureTool::isUnpaired(int bcid, int lb) {
  if (isUnpairedBeam1(bcid, lb)) return true;
  if (isUnpairedBeam2(bcid, lb)) return true;
  return false;
}

bool BunchStructureTool::isColliding(int bcid, int lb) {
  for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
    if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
      for (int bcid_paired : paired_bunchesVec[lb_index])
        if (bcid == bcid_paired)
          return true;
  return false;
}

bool BunchStructureTool::isCollidingFirst(int bcid, int lb) {
  if (!isColliding(bcid, lb)) 
    return false;
  for (int i=1;i<=2;i++)
    if (bcid-i>=0&&isColliding(bcid-i,lb))
      return false; 
  return true;
}

bool BunchStructureTool::isEmpty(int bcid, int lb) {
  if (isColliding(bcid, lb) || isUnpaired(bcid,lb))
    return false;
  return true;
}

bool BunchStructureTool::isFilled(int bcid, int lb) {
  return !isEmpty(bcid, lb);
}

bool BunchStructureTool::isUnpairedFirst(int bcid, int lb) {
  
  if (isUnpairedBeam1(bcid, lb)) {
    for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
      if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
        if (unpaired_beam1Vec[lb_index].size())
          if (unpaired_beam1Vec[lb_index][0] == bcid)
            return true;
  } 

  else if (isUnpairedBeam2(bcid, lb)) {
    for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
      if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
        if (unpaired_beam2Vec[lb_index].size())
          if (unpaired_beam2Vec[lb_index][0] == bcid)
            return true;
  }

  return false;
}

bool BunchStructureTool::isUnpairedLast(int bcid, int lb) {

  if (isUnpairedBeam1(bcid, lb)) {
    for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
      if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
        if (unpaired_beam1Vec[lb_index].size())
          if (unpaired_beam1Vec[lb_index][unpaired_beam1Vec[lb_index].size()-1] == bcid)
            return true;
  }

  else if (isUnpairedBeam2(bcid, lb)) {
    for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
      if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
        if (unpaired_beam2Vec[lb_index].size())
          if (unpaired_beam2Vec[lb_index][unpaired_beam2Vec[lb_index].size()-1] == bcid)
            return true;
  }

  return false;
}

bool BunchStructureTool::isUnpairedIsolated(int bcid, int lb){

  if (!isUnpaired(bcid,lb)) return false;

  int closest_bcid_left = -1;
  int closest_bcid_right = -1;

  for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
    if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index]) {

      for (int paired_bcid : paired_bunchesVec[lb_index]) {
        if (paired_bcid < bcid) 
          if (closest_bcid_left == -1 || bcid - paired_bcid < bcid - closest_bcid_left)
            closest_bcid_left = paired_bcid;
        if (paired_bcid > bcid)
          if (closest_bcid_right == -1 || paired_bcid - bcid < closest_bcid_right - bcid)
            closest_bcid_right = paired_bcid;
      }

      // check the bunches in the other beam
      if (isUnpairedBeam2(bcid, lb)) 
        for (int unpaired_bcid1 : unpaired_beam1Vec[lb_index]) {
          if (unpaired_bcid1 < bcid) 
            if (closest_bcid_left == -1 || bcid - unpaired_bcid1 < bcid - closest_bcid_left)
              closest_bcid_left = unpaired_bcid1;
          if (unpaired_bcid1 > bcid)
            if (closest_bcid_right == -1 || unpaired_bcid1 - bcid < closest_bcid_right - bcid)
              closest_bcid_right = unpaired_bcid1;
        }
      
      if (isUnpairedBeam1(bcid, lb)) 
        for (int unpaired_bcid2 : unpaired_beam2Vec[lb_index]) {
          if (unpaired_bcid2 < bcid)
            if (closest_bcid_left == -1 || bcid - unpaired_bcid2 < bcid - closest_bcid_left)
              closest_bcid_left = unpaired_bcid2;
          if (unpaired_bcid2 > bcid)
            if (closest_bcid_right == -1 || unpaired_bcid2 - bcid < closest_bcid_right - bcid)
              closest_bcid_right = unpaired_bcid2;
        }
  }

  bool isIsolatedLeft = false;
  if (closest_bcid_left == -1 || bcid - closest_bcid_left >= 8)
    isIsolatedLeft = true;

  bool isIsolatedRight = false;
  if (closest_bcid_right == -1 || closest_bcid_right - bcid >= 8)
    isIsolatedRight = true;

  if (m_debug)
    cout << "BunchStructureTool::isIsolated: bcid = " << bcid << " lb=" << lb << endl
         << "  closest bcid left=" << closest_bcid_left << endl
         << "  closest bcid right=" << closest_bcid_right << endl
         << "  isIsolatedLeft=" << isIsolatedLeft << endl
         << "  isIsolatedRight=" << isIsolatedRight << endl;

  return (isIsolatedLeft && isIsolatedRight);
}

bool BunchStructureTool::isStableBeam(int lb) {
  for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
    if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
      return m_stableBeams[lb_index];
  throw Form("isStableBeam: wrong lb=%i", lb);
}

void BunchStructureTool::config(int runNumber) {

  if (runNumber == BunchStructureTool::runNumber)
    return;
  
  BunchStructureTool::runNumber = runNumber;
  
  string bunchConfigFile = PathResolverFindCalibFile(Form("NCBanalysis/BunchStructureTool/%i.txt", runNumber));
  cout << "BunchStructureTool: config file=" << bunchConfigFile << endl;  

  ifstream fdata(bunchConfigFile.data(), std::ifstream::in);
  if (!fdata.is_open()) {
    cout<<"failed to open file"<<endl;
    throw 0;
  }

  int linecounter=-1;
  //char ch;
  string line;
  string word[7];

  while (getline(fdata, line)) {
    linecounter++;

    if (linecounter==0) {
      TString firstLine = line.data();
      firstLine.ReplaceAll("Identifiers (BCID) for paired and unpaired bunches for run ","");
      firstLine.ReplaceAll(":","");
      cout << "header line: runNum=" << firstLine << endl;
      if (firstLine.Atoi() != runNumber) {
        cout << "wrong runNumber in config file" << endl;
        throw 0;
      }
    }

    //skip header
    if (linecounter<4) 
      continue;

    //for (int i=0;i<line.size();i++)
    //  cout << i << " " << line[i] << " " << Form("%hhu", line[i])<<endl;

    // word
    // 0 lbmin
    // 1 -
    // 2 lbmax
    // 3 stable beam
    // 4 paired bcids
    // 5 unpaired beam 1
    // 6 unpaired beam 2
    size_t pos1=0;
    size_t pos2=0;
    for (int i=0;i<7;i++) {
      pos2=line.find(9,pos1);
      word[i]=line.substr(pos1,pos2-pos1);
      cout << "word #"<<i<<" "<<word[i]<<endl;
      pos1=pos2+1;
    }

    // read the lb range 
    int lb_min, lb_max;
    lb_min = stoi(word[0]);

    word[2].erase(word[2].find(":"));
    lb_max = stoi(word[2]);

    cout << "lb_min=" << lb_min 
         << " lb_max=" << lb_max 
         << endl; 
    lb_minVec.push_back(lb_min);
    lb_maxVec.push_back(lb_max);

    cout << "word#3=" << word[3] << endl;
    bool stableBeams = false;
    if (word[3].find("T") != string::npos) 
      stableBeams = true;
    cout<<"stable beam: " << stableBeams << endl;
    m_stableBeams.push_back(stableBeams);	
    
    vector<int> paired_bunches;
    while (word[4].find(",") != string::npos)
      word[4].erase(word[4].find(","), 1);
    if (!isEmptyColumn(word[4])) {
      stringstream ss;
      ss << word[4];
      int bcid;
      while (ss >> bcid)
        paired_bunches.push_back(bcid);
      for (int id : paired_bunches)
        cout << " paired bunch " << id << endl;
    }
    paired_bunchesVec.push_back(paired_bunches);

    // read the unpaired beam 1 bunches
    vector<int> unpaired_beam1;
    while (word[5].find(",") != string::npos)
      word[5].erase(word[5].find(","), 1);
    if (!isEmptyColumn(word[5])) {
      stringstream ss;
      ss << word[5];
      int bcid;
      while (ss >> bcid)
        unpaired_beam1.push_back(bcid);
      for (int id : unpaired_beam1)
        cout << " unpaired_beam1 " << id << endl;
    }
    unpaired_beam1Vec.push_back(unpaired_beam1);

    // read the unpaired beam 2 bunches
    vector<int> unpaired_beam2;
    while (word[6].find(",") != string::npos)
      word[6].erase(word[6].find(","), 1);
    if (!isEmptyColumn(word[6])) {
      stringstream ss;
      ss << word[6];
      int bcid;
      while (ss >> bcid)
        unpaired_beam2.push_back(bcid);
      for (int id : unpaired_beam2)
        cout << " unpaired_beam2 " << id << endl;
    }
    unpaired_beam2Vec.push_back(unpaired_beam2);

  }

  fdata.close();

  print();

}

void BunchStructureTool::print() {
  cout << "BunchStructureTool::print runNumber: " << runNumber << endl; 
  for (unsigned int lb_index = 0; lb_index < lb_minVec.size(); lb_index++) {
    cout << "LB range: [" << lb_minVec[lb_index] << ", " << lb_maxVec[lb_index] << "]" << endl;
	
    cout << "Stable beams: " << m_stableBeams[lb_index] << endl;

    cout << "paired bunches: ";
    for (int bcid : paired_bunchesVec[lb_index])
      cout << bcid << " ";
    cout << endl;

    cout << "Unpaired bunches beam 1: ";
    for (int bcid: unpaired_beam1Vec[lb_index])
      cout << bcid << " ";
    cout << endl;

    cout << "Unpaired bunches beam 2: ";
    for (int bcid: unpaired_beam2Vec[lb_index])
      cout << bcid << " ";
    cout << endl;
  }

}

bool BunchStructureTool::isEmptyColumn(string word) {
  const char *buf = word.data();
  if (TString(Form("%hhu",buf[0]))=="226" &&
      TString(Form("%hhu",buf[1]))=="136" &&
      TString(Form("%hhu",buf[2]))=="146")
    return true;
  return false;
}

int BunchStructureTool::numBunchesColliding(int lb) {
  for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
    if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
      return paired_bunchesVec[lb_index].size();
  return -1; 
}

int BunchStructureTool::numBunchesUnpairedBeam1(int lb) {
  for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
    if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
      return unpaired_beam1Vec[lb_index].size();
  return -1;
}

int BunchStructureTool::numBunchesUnpairedBeam2(int lb) {
  for (unsigned int lb_index=0;lb_index<lb_minVec.size();lb_index++)
    if (lb >= lb_minVec[lb_index] && lb <= lb_maxVec[lb_index])
      return unpaired_beam2Vec[lb_index].size();
  return -1;
}
