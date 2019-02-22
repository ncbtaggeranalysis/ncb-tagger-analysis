#include <iostream>
using namespace std;
#include <fstream>

#include <TString.h>
#include <TObjArray.h>

int main(int argc, char** argv) {

  TString GRL="";
  int lb=0;
  int runNumber=0;

  for (int i=1;i<argc;i++) {
    if (TString(argv[i])=="--GRL")
      GRL=argv[i+1];
    if (TString(argv[i])=="--runNumber")
      runNumber=TString(argv[i+1]).Atoi(); 
    if (TString(argv[i])=="--lb")
      lb=TString(argv[i+1]).Atoi();
  }
  //cout<<"GRL="<<GRL<<endl;
  //cout<<"lb="<<lb<<endl;
  //cout<<"runNumber="<<runNumber<<endl;
  if (GRL==""||runNumber==0||lb==0) {cout<<"Bad syntax"<<endl;return -1;}

  TObjArray* objArray = GRL.Tokenize(",");
  vector<TString> grlVec;
  for (int i=0;i<objArray->GetEntries();i++)
    grlVec.push_back(objArray->At(i)->GetName());

  bool isInGRL = false;
  bool foundData = false;

  for (TString grl : grlVec) {
    //cout<<grl<<endl;
    fstream fgrl(grl, fstream::in);
    if (!fgrl.is_open()) throw "failed to open grl file";

    char linebuf[10000];
    while (fgrl.getline(linebuf,10000) && !foundData) {
      TString line(linebuf);  
      //cout << line << endl;
      //if (line==Form("<Run>%i</Run>", runNumber)) {
      if (line.BeginsWith(Form("<Run>%i", runNumber))) {
        //cout<<"Found data for this run"<<endl; 
        foundData=true;
        while (fgrl.getline(linebuf,1000) && (line=TString(linebuf))!="</LumiBlockCollection>") {
          //<LBRange Start="217" End="252"/>
            if (!line.BeginsWith("<LBRange")) { 
              cout<<line<<endl;
              throw "grl file bad syntax";
            } 
            line.ReplaceAll("<LBRange Start=","");
            line.ReplaceAll("End=","");
            line.ReplaceAll("/>","");
            line.ReplaceAll("\"","");
            int lb1=TString(line.Tokenize(" ")->At(0)->GetName()).Atoi();
            int lb2=TString(line.Tokenize(" ")->At(1)->GetName()).Atoi(); 
            //cout << line<<" lb=" <<lb1 <<" " << lb2<<endl; 
            if (lb>=lb1&&lb<=lb2) isInGRL=true;
        }
       
      }
    }
    fgrl.close(); 
  }

  return isInGRL;
}
