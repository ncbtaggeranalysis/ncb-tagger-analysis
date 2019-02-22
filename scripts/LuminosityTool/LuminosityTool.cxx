#include "LuminosityTool.h"

LuminosityTool::LuminosityTool() {

}

void LuminosityTool::config(int runNumber) {
  TString configfile = Form("../../source/NCBanalysis/data/LuminosityTool/%i.txt", 
                            runNumber);
  cout<<"LuminosityTool::config: " << configfile << endl;
  
  ifstream fconfig(configfile);
  if (!fconfig.is_open()) 
    throw "LuminosityTool::config: failed to load config file "+configfile;

  char linebuf[10000];
  TString linestr;

  fconfig.getline(linebuf, 10000);
  linestr = linebuf;
  if (!linestr.Contains("Online luminosity [algorithm: ATLAS_PREFERRED] per LB for run "))
    throw "config file format error first line";
  if (!linestr.Contains(Form("%i",runNumber)))
    throw Form("config file does not contain the run number ", runNumber);

  m_runNumber = runNumber;

  fconfig.getline(linebuf, 10000); linestr=linebuf;
  if (!linestr.Contains("Begin/end of run")) throw "config file format error line 2";

  fconfig.getline(linebuf, 10000); linestr=linebuf;
  fconfig.getline(linebuf, 10000); linestr=linebuf;
  if (!linestr.Contains("LB	Start time")) 
    throw "config file format error line 4";

  fconfig.getline(linebuf, 10000); linestr=linebuf;
  if (!linestr.Contains("(CEST)	Duration"))
    throw "config file format error line 5";

  fconfig.getline(linebuf, 10000); linestr=linebuf;
  if (!linestr.Contains("(sec)	Inst. luminosity"))
    throw "config file format error line 6";

  fconfig.getline(linebuf, 10000); linestr=linebuf;
  if (!linestr.Contains("Int. luminosity"))
    throw "config file format error line 7";

  fconfig.getline(linebuf, 10000); linestr=linebuf;
  if (!linestr.Contains("Cumul. luminosity"))
    throw "config file format error line 8";

  fconfig.getline(linebuf, 10000); linestr=linebuf;
  if (!linestr.Contains("Stable beams"))
    throw "config file format error line 9";

  bool endLineReached=false;
  int linecounter=9;
  while (fconfig.getline(linebuf, 10000)) {
    linestr = linebuf;
    linecounter++;
    if (linestr.Contains("Created by AtlRunQuery on:")) {
      endLineReached=true;
      break;
    }

    TObjArray *objArray = linestr.Tokenize("	");
    int ncolumns = objArray->GetEntries();
    if (ncolumns!=8) 
      throw Form("config file line %i ncolumns=%i",linecounter,ncolumns);
    
    m_lb.push_back(TString(objArray->At(0)->GetName()).Atoi());
    m_duration.push_back(TString(objArray->At(2)->GetName()).Atof());
    m_instLuminosity.push_back(TString(objArray->At(3)->GetName()).Atof()); 
    m_intLuminosity.push_back(TString(objArray->At(4)->GetName()).Atof());
    m_mu.push_back(TString(objArray->At(6)->GetName()).Atof());
    m_stableBeams.push_back(TString(objArray->At(7)->GetName())=="T" ? true : false);
  }

  if (!endLineReached)
    throw "config file format error last line";
  fconfig.close();
}

void LuminosityTool::print() {
  cout<<"LuminosityTool::print runNumber="<<m_runNumber<<endl;
  for (int i=0;i<m_lb.size();i++)
    cout<<m_lb[i]<<" "<<m_duration[i]<< " "
        <<m_instLuminosity[i]<<" "<<m_intLuminosity[i]<<" "
        <<m_mu[i]<<" " <<m_stableBeams[i]<<endl;
}

float LuminosityTool::averageInstLuminosityStableBeams() {
  float sumInstLumi = 0;
  float sumIntLumi = 0;
  float average = 0;

  for (int i=0;i<m_lb.size();i++) 
    if (m_stableBeams[i]) {
      sumInstLumi += m_intLuminosity[i]*m_instLuminosity[i];
      sumIntLumi += m_intLuminosity[i];    
    }

  if (sumIntLumi)
    average = sumInstLumi/sumIntLumi;
  return average;
}

int LuminosityTool::lastLB() {
  return m_lb[m_lb.size()-1];
}

float LuminosityTool::instLuminosity(int lb) {
  for (int i=0;i<m_lb.size();i++)
    if (m_lb[i]==lb)
      return m_instLuminosity[i];
  throw Form("LuminosityTool::InstLuminosity lb=%i not found");
  return 0;
}
