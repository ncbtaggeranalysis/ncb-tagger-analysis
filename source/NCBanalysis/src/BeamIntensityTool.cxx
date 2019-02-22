#include "NCBanalysis/BeamIntensityTool.h"

BeamIntensityTool::BeamIntensityTool(const string& type, const string& name, const IInterface* parent) : 
  AthAlgTool(type,name,parent) {
  m_runNumber = 0;
}

void BeamIntensityTool::config(int runNumber) {

  if (runNumber == m_runNumber)
    return;

  cout << "BeamIntensityTool::config: " << runNumber << endl;

  TString fname = TString::Format("NCBanalysis/BeamIntensityTool/%i.txt", runNumber);
  string fnamePathResolver = PathResolverFindCalibFile(fname.Data());
  cout << "Opening: " << fnamePathResolver << endl;

  ifstream fdata(fnamePathResolver.data(), std::ifstream::in);
  if (!fdata.is_open()) {
    ATH_MSG_INFO("Failed to open file");
    throw 1;
  }

  int linecounter = -1;
  string line;

  // read input file
  while (getline(fdata, line)) {

    linecounter++;

    if (linecounter==0) {
      TString firstLine = line.data();
      if (!firstLine.BeginsWith("LHC beam energy and intensities during run ")) {
        cout << "BeamIntensityTool::config wrong file format" << endl;
        throw 1;
      }
      firstLine.ReplaceAll("LHC beam energy and intensities during run ","");
      firstLine.ReplaceAll(":","");
  
      if (firstLine.Atoi() != runNumber) {
        cout << "BeamIntensityTool::config wrong run number in txt file" << endl;
        throw 1;
      } 

      continue;

    }

    // read the begin/end of run date
    // can be: (Begin/end of run: Sun Sep 25 2016 06:37:25 − 13:20:21)
    //   or:   (Begin/end of run: Thu Sep 29 2016 19:43:29 − Fri Sep 30, 20:58:02) 
    else if (linecounter == 1) {

      TString dateStr = line.data();

      dateStr.ReplaceAll("(Begin/end of run: ","");
      dateStr.ReplaceAll(")","");
      dateStr.ReplaceAll(",","");

      if (!dateStr.Contains("−")) {
        cout << "Beam intensity tool::config wrong format for line 2" << endl;
        throw 1;
      } 

      stringstream linestream;
      linestream << dateStr.Data();
      vector<string> words; 

      while (!linestream.eof()) {
        string word;
        linestream >> word;
        words.push_back(word);
      }

      for (string word : words)
        cout << "  date word: " << word << endl;

      if (stoi(words[3])<2015 || stoi(words[3]) > 2020) {
        cout << "  Beam intensity tool::config wrong year? " << endl;
        throw 1;
      }

      // SQL format is for example: 2016-05-09 03:38:16
      m_beginOfRunDate = (words[3] + "-"    
                       + Form("%02i",getMonthNum(words[1])) + "-"   
                       + words[2] + " "
                       + words[4]).data(); // time

      cout << "#word=" << words.size() << endl;
      if (words.size() == 10)
        m_endOfRunDate = (words[3] + "-"      
                       + Form("%02i",getMonthNum(words[7])) + "-"
                       + words[8] + " "
                       + words[9]).data(); 
      else if (words.size() == 7)
        m_endOfRunDate = (words[3] + "-"      
                       + Form("%02i",getMonthNum(words[1])) + "-"
                       + words[2] + " "
                       + words[6]).data(); 
      else {
        cout << "reading run date/time: wrong number of words: " << words.size() << endl;
        throw 1;
      } 

      continue; 

    }

    else if (linecounter<=8)
      continue; // skip header

    if (line.find("Created by AtlRunQuery") != string::npos) // last line 
    {
      cout << "reached last line" << endl;
      break; 
    }

    stringstream linestream;
    linestream << line;

    int LB;
    string startTime;
    float duration;
    float energy;
    float intensityBeam1;
    float intensityBeam2;

    linestream >> LB >> startTime >> duration >> energy >> intensityBeam1 >> intensityBeam2;

    cout <<" LB=" << LB << " startTime=" << startTime << " energy=" << energy << " intensityBeam1=" << intensityBeam1 << " intensityBeam2=" << intensityBeam2 << endl;

    if (linecounter==9 && LB != 1) {
      cout << "first LB should be 1; it is: " << LB << endl;
      throw 1;
    }

    m_LBvec.push_back(LB);
    m_startTimeVec.push_back(startTime);
    m_durationVec.push_back(duration);
    m_energyVec.push_back(energy);
    m_intensityBeam1Vec.push_back(intensityBeam1);
    m_intensityBeam2Vec.push_back(intensityBeam2);

  }

  m_runNumber = runNumber;

  for (unsigned int lb=0;lb<m_LBvec.size();lb++) 
    cout << Form("%4i",m_LBvec[lb]) 
         << Form("%10s",m_startTimeVec[lb].Data())
         << Form("%7.2f",m_durationVec[lb])
         << Form("%7.1f",m_energyVec[lb])
         << Form("%7.1f",m_intensityBeam1Vec[lb])
         << Form("%7.1f",m_intensityBeam2Vec[lb])
         << endl; 

}

bool BeamIntensityTool::configIsLoaded(int runNumber) {
  return (runNumber == m_runNumber);
}

float BeamIntensityTool::getIntensityBeam1(int lb) {

  for (unsigned int i=0;i<m_LBvec.size();i++) 
    if (m_LBvec[i] == lb)
      return m_intensityBeam1Vec[i];

  cout << " BeamIntensityTool::getIntensityBeam1: wrong LB=" << lb << endl;
  throw 1;
}


float BeamIntensityTool::getIntensityBeam2(int lb) {

  for (unsigned int i=0;i<m_LBvec.size();i++) 
    if (m_LBvec[i] == lb)
      return m_intensityBeam2Vec[i];

  cout << " BeamIntensityTool::getIntensityBeam2: wrong LB=" << lb << endl;
  throw 1;
}

float BeamIntensityTool::getLBduration(int lb1, int lb2) {
  //cout << "BeamIntensityTool::getLBDuration: lb range: [" << lb1 << ", " << lb2 << "]" << endl; 
  float duration=0;
  for (unsigned int i=0;i<m_LBvec.size();i++)
   if (m_LBvec[i]>=lb1 && m_LBvec[i]<=lb2) 
     duration+=m_durationVec[i];
  return duration;
}

int BeamIntensityTool::getRunDuration() {
  time_t t_end = TDatime(m_endOfRunDate).Convert();
  time_t t_start = TDatime(m_beginOfRunDate).Convert();
  cout << "BeamIntensityTool::getRunDuration()" << endl
       << "  t_start=" << t_start << endl
       << "  t_end=" << t_end << endl;
  return t_end - t_start;
}

void BeamIntensityTool::print() {
  cout << "BeamIntensityTool::print() " << endl
       << "  runNumber=" << m_runNumber << endl
       << "  beginOfRunDate=" << m_beginOfRunDate << endl
       << "  endOfRunDate=" << m_endOfRunDate << endl
       << "  getLBduration(0,1000000)=" << getLBduration(0,1000000) << endl
       << "  getRunDuration=" << getRunDuration() << endl;

}

int BeamIntensityTool::getMonthNum(string month) {
  if (month=="Jan")
    return 1;
  else if (month=="Feb")
    return 2;
  else if (month=="Mar")
    return 3;
  else if (month=="Apr")
    return 4;
  else if (month=="May")
    return 5;
  else if (month=="Jun")
    return 6;
  else if (month=="Jul")
    return 7;
  else if (month=="Aug")
    return 8;
  else if (month=="Sep")
    return 9;
  else if (month=="Oct")
    return 10;  
  else if (month=="Nov")
    return 11;
  else if (month=="Dec")
    return 12;

  else {
    cout << "BeamIntensityTool::getMonth wrong month:" << month << endl;
    throw 1;
  }

  return -1;
}

