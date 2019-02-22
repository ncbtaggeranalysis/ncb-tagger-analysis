#include <TObjArray.h>
#include <TObjString.h>
#include <TSystem.h>

#include "taggerAnalysisLib.h"

namespace TaggerAnalysisLib {

pair<TString,TString> getDatasetProjectTagAndPeriod(int runNumber) {
  pair<TString,TString> res;

  if ((runNumber>=266904&&runNumber<=267167) ||
         runNumber==267638 || runNumber==267639) res={"data15_13TeV","periodA"};
  if (runNumber>=267358&&runNumber<=267599)      res={"data15_13TeV","periodB"};
  if (runNumber>=270441&&runNumber<=272531)      res={"data15_13TeV","periodC"};
  if (runNumber>=276073&&runNumber<=276954)      res={"data15_13TeV","periodD"};  
  if (runNumber>=278727&&runNumber<=279928)      res={"data15_13TeV","periodE"};


  if (runNumber>=296939&&runNumber<=300287)      res={"data16_13TeV","periodA"};
  if (runNumber>=300345&&runNumber<=300908)      res={"data16_13TeV","periodB"};

  if (runNumber>=324320&&runNumber<=325558)      res={"data17_13TeV","periodA"};
  if (runNumber>=325713&&runNumber<=328393)      res={"data17_13TeV","periodB"};

  if (runNumber>=348197&&runNumber<=348836) res={"data18_13TeV","periodA"};
  if (runNumber>=348885&&runNumber<=349533) res={"data18_13TeV","periodB"};
  if (runNumber>=349534&&runNumber<=350220) res={"data18_13TeV","periodC"};
  if (runNumber>=350310&&runNumber<=352107) res={"data18_13TeV","periodD"};
  if (runNumber>=352123&&runNumber<=352137) res={"data18_13TeV","periodE"};
  if (runNumber>=352274&&runNumber<=352514) res={"data18_13TeV","periodF"};
  if (runNumber>=354107&&runNumber<=354396) res={"data18_13TeV","periodG"};
  if (runNumber>=354826&&runNumber<=355224) res={"data18_13TeV","periodH"};
  if (runNumber>=355261&&runNumber<=355273) res={"data18_13TeV","periodI"};
  if (runNumber>=355331&&runNumber<=355468) res={"data18_13TeV","periodJ"};
  if (runNumber>=355529&&runNumber<=356259) res={"data18_13TeV","periodK"};
  if (runNumber>=357050&&runNumber<=359171) res={"data18_13TeV","periodL"};
  if (runNumber>=359191&&runNumber<=360414) res={"data18_13TeV","periodM"};
  if (runNumber>=361635&&runNumber<=361696) res={"data18_13TeV","periodN"};
  if (runNumber>=361738&&runNumber<=363400) res={"data18_13TeV","periodO"};
  if (runNumber>=363664&&runNumber<=364292) res={"data18_13TeV","periodQ"};

  if (res.first=="")   
    cout << Form("getDatasetProjectTagAndPeriod: unknown runNumber: %i", runNumber) << endl;
  return res;
}

void getDatasetInfo(TString filename, 
                    TString& projectTag, 
                    int& runNumber, 
                    TString& period, 
                    TString& stream) {

  projectTag="";
  runNumber=0;
  period="";
  stream="";
  TString runNumberStr="";

  TString dirname = gSystem->DirName(filename);
  TString dirnameBase = gSystem->BaseName(dirname);
  TString filenameBase = gSystem->BaseName(filename);

  TObjArray* objArray = filenameBase.Tokenize(".");
  for (int i=0;i<objArray->GetEntries();i++) {
    TString word = objArray->At(i)->GetName();
    if (word == "data15_13TeV" || 
        word == "data16_13TeV" || 
        word == "data17_13TeV" || 
        word == "data18_13TeV" ||
        word == "mc16_13TeV") {
      projectTag = word;
      if (i+1<objArray->GetEntries()) {
        runNumberStr= objArray->At(i+1)->GetName();
        if (runNumberStr.IsDigit())
          runNumber = runNumberStr.Atoi();
      }  
      break;
    }
  }

  objArray = dirnameBase.Tokenize(".");
  for (int i=0;i<objArray->GetEntries();i++) {
    TString word = objArray->At(i)->GetName();
    if (word == "data15_13TeV" ||
        word == "data16_13TeV" ||
        word == "data17_13TeV" ||
        word == "data18_13TeV" ||
        word == "mc16_13TeV") {
      projectTag = word;
      if (i+1<objArray->GetEntries()) {
        runNumberStr= objArray->At(i+1)->GetName();
        if (runNumberStr.IsDigit())
          runNumber = runNumberStr.Atoi();
      }
      break;
    }
  }

  if (!projectTag.Contains("data") && !projectTag.Contains("mc"))
    throw "wrong projectTag=" + projectTag+" for filename="+filename;

  if (runNumber==0||projectTag=="")  
    throw Form("getDatasetInfo: failed to set runNumber(%i) or projectTag(%s),"
               "filename=%s", 
               runNumber, projectTag.Data(),filename.Data());

  //cout<<"projectTag="<<projectTag<<endl;
  
  if (projectTag.Contains("data")) {
    period = getDatasetProjectTagAndPeriod(runNumber).second;
    if (filename.Contains("physics_Main")) stream="physics_Main";
    else if (filename.Contains("physics_Background")) stream="physics_Background";
  }
}

int getDatasetRunNumber(TString ds) {
    TObjArray* objArray = TString(gSystem->BaseName(ds)).Tokenize(".");
  TString runNumberStr="";
  for (int i=0;i<objArray->GetEntries();i++) {
    TString word = objArray->At(i)->GetName();
    if (word == "data15_13TeV" || 
        word == "data16_13TeV" || 
        word == "data17_13TeV" || 
        word == "data18_13TeV" || 
        word == "mc16_13TeV") {
      runNumberStr= objArray->At(i+1)->GetName();
      break;
    }
  }
  return runNumberStr.Atoi();
}

vector<TString> getListOfFilesInDirectory(TString inDir, TString pattern) {
  TSystemDirectory dir("",inDir);
  TList* list = dir.GetListOfFiles();
  if (!list) throw "getListOfFilesInDirectory: pointer to the list of files is null";
  vector<string> filenameVec;
  for (int i=0;i<list->GetEntries();i++) {
    TString filename=list->At(i)->GetName();
    if (!filename.Contains(pattern)) continue;
    filenameVec.push_back(filename.Data());
  }
  std::sort(filenameVec.begin(),filenameVec.end());
  vector<TString> fileList;
  for (string filename : filenameVec)
    fileList.push_back(inDir + TString("/") + filename.data());
  return fileList;
}

pair<float,float> getEfficiency(int nsample, int ntagged) {
  float mean_eff = (nsample !=0) ? (float)ntagged/nsample : 0;

  float statErrorUp=TEfficiency::Normal(nsample,ntagged,0.683,true)-mean_eff;
  float statErrorDown=mean_eff-TEfficiency::Normal(nsample,ntagged,0.683,false);

  pair<float, float> res;
  res.first = mean_eff;
  res.second = 0.5*(statErrorUp+statErrorDown);
  return res;
}

}
