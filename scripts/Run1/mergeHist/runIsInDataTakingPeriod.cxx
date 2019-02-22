#include <iostream>
using namespace std;

#include <TString.h>

int main(int argc, char** argv) {

  if (argc!=3) {cout<<"Bad syntax"<<endl; return -1;}

  TString projectTag = ""; 
  int run = TString(argv[1]).Atoi();
  TString period = argv[2];
 
  //data11_7TeV
  if (run>=177531&&run<=177965&&period=="A") return 1;
  if (run>=177986&&run<=178109&&period=="B") return 1;
  if (run>=178163&&run<=178264&&period=="C") return 1;
  if (run>=179710&&run<=180481&&period=="D") return 1;
  if (run>=180614&&run<=180776&&period=="E") return 1;
  if (run>=182032&&run<=182519&&period=="F") return 1;
  if (run>=182726&&run<=183462&&period=="G") return 1;
  if (run>=183544&&run<=184169&&period=="H") return 1;
  if (run>=185353&&run<=186493&&period=="I") return 1;
  if (run>=186516&&run<=186755&&period=="J") return 1;
  if (run>=186873&&run<=187815&&period=="K") return 1;
  if (run>=188902&&run<=190343&&period=="L") return 1;
  if (run>=190503&&run<=191933&&period=="M") return 1;
  
  //data12_8TeV
  if (run>=200804&&run<=201556&&period=="A") return 1;
  if (run>=202668&&run<=205113&&period=="B") return 1;
  if (run>=206248&&run<=207332&&period=="C") return 1;
  if (run>=207447&&run<=209025&&period=="D") return 1;
  if (((run>=209074&&run<=209995)||run==210302||run==210308)&&period=="E") return 1;
  if (run>=210184&&run<=210186&&period=="F") return 1;
  if (run>=211522&&run<=212272&&period=="G") return 1;
  if (run>=212619&&run<=213359&&period=="H") return 1;
  if (run>=213431&&run<=213819&&period=="I") return 1;
  if (run>=213900&&run<=215091&&period=="J") return 1;
  if (run>=215414&&run<=215643&&period=="L") return 1;
  if (run>=216399&&run<=216399&&period=="M") return 1; 
  
  return 0;
}
