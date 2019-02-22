#include <iostream>
#include <fstream>

#include <TObjArray.h>

#include "grlTool.h"

//GrlTool::GrlTool() {}

void GrlTool::Load(vector<TString> grlVec) {
  cout<<"GrlTool::Load"<<endl;
  for (TString grl : grlVec) {
    cout<<grl<<endl;
    fstream f(grl.Data(), fstream::in);
    if (!f.is_open()) throw "GrlTool::Load failed to open file :" + grl;

    //<Run PrescaleRD0="8" PrescaleRD1="8">191933</Run>


    char linebuf[10000];
    TString linestr;
    while (f.getline(linebuf,10000)) {
      linestr = linebuf;
      if (linestr.Contains("<Run")) {
        cout<<linestr<<endl; 
        linestr.Remove(0,linestr.Index(">")+1); 
        cout<<linestr<<endl;
        if (!linestr.EndsWith("</Run>")) throw "GrlTool::Load: wrong grl file format";
        linestr.Remove(linestr.Index("</Run>"));
        cout<<linestr<<endl;
        int runNumber = linestr.Atoi();
        
        //  <LBRange Start="317" End="320"/>
        //</LumiBlockCollection>

        while (f.getline(linebuf,10000)) {
          linestr = linebuf;
          if (linestr.Contains("<LBRange ")) {
            cout << linestr << endl;
            linestr.ReplaceAll("<LBRange Start=","");
            linestr.ReplaceAll("End=","");
            linestr.ReplaceAll("/>","");
            linestr.ReplaceAll("\"","");
            cout << linestr << endl;
            int lb1 = TString(linestr.Tokenize(" ")->At(0)->GetName()).Atoi();
            int lb2 = TString(linestr.Tokenize(" ")->At(1)->GetName()).Atoi();
            std::pair<int,int> lbRange(lb1,lb2);
            m_run_LBrange[runNumber].push_back(lbRange); 
          } 
          else {
            if (!linestr.Contains("</LumiBlockCollection>"))
              throw "GrlTool::Load: wrong file format";
            break;
          }
        }
      }
    }
  }

  std::map<int,vector<pair<int,int>>>::iterator it = m_run_LBrange.begin();;
  for (; it!=m_run_LBrange.end();it++) {
    cout << it->first << endl;
    for (int i=0;i<it->second.size();i++)
      cout << "  " << it->second[i].first << " " << it->second[i].second << endl;
  }
}

bool GrlTool::passesRunLB(int runNumber, int lb) {
  std::map<int,vector<pair<int,int>>>::iterator it = m_run_LBrange.begin();;
  for (; it!=m_run_LBrange.end();it++) 
    if (it->first == runNumber) 
      for (int i=0;i<it->second.size();i++)
        if (it->second[i].first <= lb && lb <= it->second[i].second)
          return true;
 
  return false;
}
