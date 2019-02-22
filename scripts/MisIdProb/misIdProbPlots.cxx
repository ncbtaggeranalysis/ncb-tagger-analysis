#include <iostream>
using namespace std;
#include <vector>
#include <utility>

#include <TString.h>
#include <TFile.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TGaxis.h>
#include <TLine.h>
#include <TText.h>
#include <TMath.h>

#include "TaggerAnalysis/taggerAnalysisLib.h"
#include "LuminosityTool/LuminosityTool.h"

class Sample {
  public:
                      Sample(TString inputEfficiencyFastHISTFile,
                             TString inputEfficiencyAnalysisDir,
                             TString config,
                             vector<TString> correlationVariables,
                             vector<TString> controlVariables);
                     ~Sample();

    int               runNumber() { return m_runNumber;}
    TString           runNumberStr() { return Form("%i",m_runNumber);}
    TString           projectTag() { return m_projectTag;};
    TString           period() { return m_period;};
    TString           stream() { return m_stream;};
    float             numBunchesColliding() { return m_numBunchesColliding;}
    int               numDijetEvents() { return m_h_ntagged->GetEntries();}
    int               numTaggedEvents(int methodIndex) 
                         { return m_h_ntagged->GetBinContent(methodIndex+1);}
    bool              isData() {return m_projectTag.Contains("data");}

    pair<float,float> misIdProb(int method_index);
    vector<TString>   taggerMethods() { return m_taggerMethods;}
    vector<TH1F*>     correlationVariableHists() { return m_correlationVariableHists;}
    float             correlationVariableMean(int i);
    TString           correlationVariableLabel(TString histtitle);
    vector<TH1F*>     controlVariableHists() { return m_controlVariableHists;}
    float             averageInstLuminosityStableBeams();

    TH1F*             readEfficiencyAnalysisHist(TString histname);

    TH1F*             numberOfTaggedEventsHist() {return m_h_ntagged;};
 
  private:
    TString m_config;
    TString m_inputEfficiencyFastHISTFile;
    TString m_inputEfficiencyAnalysisHISTFile;
    TFile* m_file_efficiencyFast;
    TFile* m_file_efficiencyAnalysis;
    int m_runNumber;
    TString m_projectTag;
    TString m_period;
    TString m_stream;
    TH1F* m_h_ntagged;
    vector<TString> m_correlationVariables;
    vector<TH1F*> m_correlationVariableHists;
    vector<TString> m_controlVariables;
    vector<TH1F*> m_controlVariableHists;
    vector<TString> m_taggerMethods;
    float m_numBunchesColliding;
    LuminosityTool* m_lumiTool;
};

Sample::Sample(TString inputEfficiencyFastHISTFile,
               TString inputEfficiencyAnalysisDir, 
               TString config, 
               vector<TString> correlationVariables,
               vector<TString> controlVariables) {
  cout<<endl;
  cout<<"Sample::Sample: " << inputEfficiencyFastHISTFile << endl;
  m_inputEfficiencyFastHISTFile=inputEfficiencyFastHISTFile;
  m_config=config;
  m_correlationVariables = correlationVariables;
  m_controlVariables = controlVariables;
  m_file_efficiencyFast=TFile::Open(inputEfficiencyFastHISTFile);
  if (!m_file_efficiencyFast) throw "failed to open EfficiencyFast file";

  TaggerAnalysisLib::getDatasetInfo(inputEfficiencyFastHISTFile, 
                                    m_projectTag, m_runNumber, m_period, m_stream);
  cout<<"projectTag="<<m_projectTag<<endl;
  cout<<"runNumber="<<m_runNumber<<endl;
  cout<<"period="<<m_period<<endl;
  vector<TString> inputEfficiencyAnalysisHISTFiles=TaggerAnalysisLib::getListOfFilesInDirectory(inputEfficiencyAnalysisDir,Form("%i",m_runNumber));
  if (inputEfficiencyAnalysisHISTFiles.size()!=1)
    throw Form("wrong number of EfficiencyAnalysis files =%i for run %i",
               inputEfficiencyAnalysisHISTFiles.size(),
               m_runNumber);
  m_inputEfficiencyAnalysisHISTFile = inputEfficiencyAnalysisHISTFiles[0];
  m_file_efficiencyAnalysis = TFile::Open(m_inputEfficiencyAnalysisHISTFile);
  if (!m_file_efficiencyAnalysis)
    throw "failed to open EfficiencyAnalysis file";

  m_h_ntagged = (TH1F*)m_file_efficiencyFast->Get(config+"/h_ntagged");
  if (!m_h_ntagged) throw "failed to read histogram: "+config+"/h_ntagged";
  
  for (int i=1;i<=m_h_ntagged->GetNbinsX();i++)
    m_taggerMethods.push_back(m_h_ntagged->GetXaxis()->GetBinLabel(i));

  m_lumiTool = 0;
  if (isData()) {
    m_lumiTool = new LuminosityTool();
    m_lumiTool->config(m_runNumber);
  }

  m_numBunchesColliding=0;
  for (int i = 0;i<m_correlationVariables.size();i++) {
    TH1F* hist = 0;
    if (m_correlationVariables[i] == "averageInstLuminosityStableBeams") {
      hist = new TH1F(Form("InstLumi_%i",m_runNumber),
                      "instLuminosityStableBeams",
                      m_lumiTool->lastLB(), 0, m_lumiTool->lastLB());
      for (int bin=1; bin<=hist->GetNbinsX();bin++)
        hist->SetBinContent(bin, m_lumiTool->instLuminosity(bin));
    } 
    else {
      TString histname = m_correlationVariables[i];
      hist = (TH1F*)m_file_efficiencyFast->Get(config+"/"+histname);
      if (!hist) throw "failed to read histogram: " +config+"/"+histname;
      hist->SetTitle(correlationVariableLabel(hist->GetTitle()));
      if (histname.Contains("numBunchesColliding"))
        m_numBunchesColliding = hist->GetMean();
    }
    m_correlationVariableHists.push_back(hist);

  }

  for (int i=0;i<m_controlVariables.size();i++) {
    TString histname = m_controlVariables[i];
    TH1F* hist = (TH1F*)m_file_efficiencyFast->Get(config+"/"+histname);
    if (!hist) throw "failed to read histogram: " +config+"/"+histname;
    m_controlVariableHists.push_back(hist);
  }

}

Sample::~Sample() {
  delete m_h_ntagged;
  for (int i=0;i<m_correlationVariableHists.size();i++)
    delete m_correlationVariableHists[i];
  if (m_lumiTool) 
    delete m_lumiTool;
  m_file_efficiencyFast->Close();
  m_file_efficiencyAnalysis->Close();
}

pair<float,float> Sample::misIdProb(int method_index) {
  int nsample = m_h_ntagged->GetEntries();
  int ntagged = m_h_ntagged->GetBinContent(method_index+1);

  //if (m_taggerMethods[method_index].Contains("&&"))
  //  return pair<float,float>{1,1};
  //TString histname = m_config+"/eff_wLumi"+m_taggerMethods[method_index];
  //TEfficiency* eff_wLumi = (TEfficiency*)m_file_efficiencyAnalysis->Get(histname);
  //if (!eff_wLumi) throw "failed to read: "+histname;
  //int nsample = eff_wLumi->GetTotalHistogram()->GetBinContent(1);
  //int ntagged = eff_wLumi->GetPassedHistogram()->GetBinContent(1);

  return TaggerAnalysisLib::getEfficiency(nsample, ntagged);
}

float Sample::correlationVariableMean(int i) {
  if (m_correlationVariables[i]=="averageInstLuminosityStableBeams")
    return m_lumiTool->averageInstLuminosityStableBeams();
  else
    return m_correlationVariableHists[i]->GetMean();
}

TString Sample::correlationVariableLabel(TString histtitle) {
  TString label = histtitle;

  label.ReplaceAll("Sum$(MuonSegments.chamberIndex==7||MuonSegments.chamberIndex==8)",
                   "Number of MDT inner end-cap segments");

  label.ReplaceAll("Sum$(NCB_MuonSegments.chamberIndex==15||NCB_MuonSegments.chamberIndex==16)",
                   "Number of CSC segments");


  label.ReplaceAll("BunchStructure.numBunchesColliding",
                   "numBunchesColliding");

  return label;
}

float Sample::averageInstLuminosityStableBeams() {
  return m_lumiTool->averageInstLuminosityStableBeams();
}

TH1F* Sample::readEfficiencyAnalysisHist(TString histname) {
  TH1F* hist = (TH1F*)m_file_efficiencyAnalysis->Get(histname);
  if (!hist) throw "failed to read histogram: "+histname;
  return hist;
}

class MisIdProbPlots {
  public:
            MisIdProbPlots(TString config,
                          vector<TString> correlationVariables,
                          vector<TString> controlVariables,
                          int minDijetEvents,
                          TString inputTxtNntuple, 
                          TString outputPDFFile);
           ~MisIdProbPlots();

    void    addSample(TString inputEfficiencyFastHISTFile,
                      TString inputEfficiencyAnalysisDir);

    void    addPlots();

    void    addPlotMisIdProbVsRunNumber(int taggerMethodIndex,
                                        int correlationVariableIndex,
                                        bool logy);

    void    addPlotMisIdProbVsCorrelationVariable(int taggerMethodIndex,
                                                  int correlationVariableIndex);

    void    addPlotCorrelationVariablesInSample(int sampleIndex);
    void    addPlotControlVariablesInSample(int sampleIndex);
    void    addPlotTaggedClustersInSample(int sampleIndex, int methodIndex);
    void    addPlotNumMuonSegmentsVsInstLuminosity();

    void    printMisIdProbTable();

    bool    isData() { return m_samples[0]->isData();} 

    int     numEventsInTheAODs();
  private:
    vector<Sample*> m_samples;
    TString m_config;
    vector<TString> m_correlationVariables;
    vector<TString> m_controlVariables;
    int m_minDijetEvents; 
 
    vector<TString> m_taggerMethods;
    TString m_inputTxtNtuple;

    TCanvas* m_canvas;
    TString m_outputPDFFile;
};


MisIdProbPlots::MisIdProbPlots(TString config, 
                             vector<TString> correlationVariables,
                             vector<TString> controlVariables,
                             int minDijetEvents,
                             TString inputTxtNtuple,
                             TString outputPDFFile) {
  m_config=config;
  m_correlationVariables = correlationVariables;
  m_controlVariables = controlVariables;
  m_minDijetEvents = minDijetEvents;
  m_inputTxtNtuple = inputTxtNtuple;
  m_outputPDFFile=outputPDFFile;
  m_canvas = new TCanvas();
  m_canvas->Print(m_outputPDFFile+"(");
  gStyle->SetOptStat("");
}


MisIdProbPlots::~MisIdProbPlots() {
  //for (TH1F* h : m_h_misIdProb_vs_runNumber)
  //  delete h;
  for (Sample* sample: m_samples)
    delete sample;
  m_canvas->Clear();
  m_canvas->Print(m_outputPDFFile+")"); 
}


void MisIdProbPlots::addSample(TString inputEfficiencyFastHISTFile,
                               TString inputEfficiencyAnalysisDir) {

  Sample* sample = new Sample(inputEfficiencyFastHISTFile,
                              inputEfficiencyAnalysisDir,
                              m_config, 
                              m_correlationVariables, 
                              m_controlVariables);

  //LAr with special conditions, timing a bit off, DQ flag LAR_SPECIALSTUDIES 
  //DQ meeting 25/04/2018
  if (sample->runNumber()==348197 ||
      sample->runNumber()==348251 ||
      sample->runNumber()==348354) {
    delete sample;
    return;
  }

  if (m_minDijetEvents != -1 && sample->numDijetEvents() < m_minDijetEvents) {
    delete sample;
    return;
  }

  m_samples.push_back(sample);

  m_taggerMethods = m_samples[0]->taggerMethods();
}


void MisIdProbPlots::addPlots() {
  //for (int j=0;j<m_samples.size();j++)
  //  cout << m_samples[j]->runNumber() << " "
  //       << Form("%6.0f", m_samples[j]->numBunchesColliding()) 
  //       << Form("%10.1f",m_samples[j]->averageInstLuminosityStableBeams()) << endl;
    
  for (int j=0;j<m_samples.size();j++) 
    addPlotControlVariablesInSample(j);

  for (int j=0;j<m_samples.size();j++)
    addPlotCorrelationVariablesInSample(j);

  for (int k=0;k<m_taggerMethods.size();k++) {

    TCanvas* canvas = new TCanvas;
    TLegend legend(.2,.3,.8,.7);
    legend.SetTextColor(kBlue);
    legend.SetBorderSize(0);
    legend.AddEntry((TObject*)0, m_taggerMethods[k],"");
    legend.Draw();
    canvas->Print(m_outputPDFFile);
    delete canvas; 

    if (isData())
    if (!m_taggerMethods[k].Contains("Segment") &&
        !m_taggerMethods[k].Contains("&&")) {
      int j=0;
      int runNumber=350923;
      for (;j<m_samples.size();j++)
        if (m_samples[j]->runNumber()==runNumber)
          break;
      if (m_samples[j]->runNumber()!=runNumber)
        throw Form("failed to find sample with run number=%i", runNumber);
      addPlotTaggedClustersInSample(j, k);
    }

    for (int i=0;i<m_correlationVariables.size();i++) {
      addPlotMisIdProbVsRunNumber(k, i, false);
      if (isData())
        addPlotMisIdProbVsRunNumber(k, i, true);
      addPlotMisIdProbVsCorrelationVariable(k, i);
    }
    
  }

  if (isData())
    addPlotNumMuonSegmentsVsInstLuminosity();
}


void MisIdProbPlots::addPlotMisIdProbVsRunNumber(int methodIndex,
                                                 int correlationVariableIndex,
                                                 bool logy) {
  cout<<endl<<"addPlotMisIdProbVsRunNumber"<< m_taggerMethods[methodIndex]
      <<" " << m_correlationVariables[correlationVariableIndex] 
      << " logy=" << logy << endl;
  int i = correlationVariableIndex;
  int k = methodIndex;
  //cout << "MisIdProbPlots::addPlotsMisIdProbVsRunNumber " << endl
  //     << m_taggerMethods[k] << endl 
  //     << correlationVariables[i] << endl;

  TH1F* h_meanCorrelationVariable_vs_runNumber = new TH1F(Form("var%i",i),
              m_samples[0]->correlationVariableHists()[i]->GetXaxis()->GetTitle(),
              m_samples.size(), 0, m_samples.size()); 
  for (int j=0;j<m_samples.size();j++) 
    h_meanCorrelationVariable_vs_runNumber->SetBinContent(j+1, m_samples[j]->correlationVariableMean(i));

  m_canvas->SetBottomMargin(0.35);

  TH1F* h_misIdProb_vs_runNumber = new TH1F(m_taggerMethods[k]+"_misIdProb",
                                            ";;"+ m_taggerMethods[k] + "  P_{mis-id}",
                                            m_samples.size(), 0, m_samples.size());
  for (int j=0;j<m_samples.size();j++) {
    pair<float,float> misIdProb = m_samples[j]->misIdProb(k);
    //float w = 1/(m_samples[j]->averageInstLuminosityStableBeams()/20000);
    float w=1;
    h_misIdProb_vs_runNumber->SetBinContent(j+1, w*misIdProb.first);
    h_misIdProb_vs_runNumber->SetBinError(j+1, misIdProb.second);
    h_misIdProb_vs_runNumber->GetXaxis()->SetBinLabel(j+1, 
              isData() ? Form("%10.0f",m_samples[j]->numBunchesColliding()) :
                         Form("%i",m_samples[j]->runNumber()) ); 
  }

  h_misIdProb_vs_runNumber->SetMinimum(0);
  h_misIdProb_vs_runNumber->SetMarkerStyle(24);

  h_misIdProb_vs_runNumber->Draw("E0");
  h_misIdProb_vs_runNumber->SetMarkerColor(kBlue);
  h_misIdProb_vs_runNumber->GetYaxis()->SetTitleColor(kBlue);
  h_misIdProb_vs_runNumber->GetXaxis()->LabelsOption("v");
  h_misIdProb_vs_runNumber->GetYaxis()->SetTitleOffset(1.2);

  float xlabelsize=0.6*h_misIdProb_vs_runNumber->GetXaxis()->GetLabelSize();

  float misIdProb_ymin = 0;
  if (logy) {
    misIdProb_ymin = 1;
    for (int bin=1;bin<=h_misIdProb_vs_runNumber->GetNbinsX();bin++)
      if (h_misIdProb_vs_runNumber->GetBinContent(bin))
        if (h_misIdProb_vs_runNumber->GetBinContent(bin) < misIdProb_ymin)
          misIdProb_ymin = h_misIdProb_vs_runNumber->GetBinContent(bin);
    misIdProb_ymin*=0.1;
  }

  float misIdProb_ymax = h_misIdProb_vs_runNumber->GetMaximum();

  h_misIdProb_vs_runNumber->GetYaxis()->SetRangeUser(misIdProb_ymin, misIdProb_ymax);

  //
  float scale = 1.2*h_meanCorrelationVariable_vs_runNumber->GetMaximum();
  if (m_correlationVariables[i]=="BunchStructure.numBunchesColliding")
    scale=3000; 

  TH1F* h_meanCorrelationVariable_vs_runNumberClone = (TH1F*)
           h_meanCorrelationVariable_vs_runNumber->Clone(Form("Clone%i",i)); 
  h_meanCorrelationVariable_vs_runNumberClone->Scale(1./scale*misIdProb_ymax);
  h_meanCorrelationVariable_vs_runNumberClone->SetLineColor(kRed);
  h_meanCorrelationVariable_vs_runNumberClone->SetMarkerStyle(25);
  h_meanCorrelationVariable_vs_runNumberClone->SetMarkerColor(kRed);
  h_meanCorrelationVariable_vs_runNumberClone->Draw("HIST P same");
 
  float xmin=h_misIdProb_vs_runNumber->GetXaxis()->GetXmax();
  float xmax=h_misIdProb_vs_runNumber->GetXaxis()->GetXmax();
  float ymin=misIdProb_ymin;
  float ymax=misIdProb_ymax;
  TGaxis* gaxis = new TGaxis(xmin,0,xmax,ymax,
                             logy? scale*misIdProb_ymin/misIdProb_ymax : 0, scale, 
                             506,
                             logy? "+G" : "+");
  if (logy) gaxis->SetMoreLogLabels();
  gaxis->SetLineColor(kRed);
  gaxis->SetLabelColor(kRed);
  gaxis->SetLabelOffset(0.04);
  gaxis->SetLabelSize(0.023);
  gaxis->SetTitle(m_samples[0]->correlationVariableHists()[i]->GetTitle());
  gaxis->SetTitleSize(0.02);
  gaxis->SetTitleOffset(1.5);
  gaxis->SetTitleColor(kRed);
  gaxis->Draw();

  pair<TString, TString> projTagPeriod0={"",""};

  for (int bin=1; bin<=h_misIdProb_vs_runNumber->GetNbinsX();bin++) {
    int run = m_samples[bin-1]->runNumber();
    
    if (isData()) {
    pair<TString, TString> projTagPeriod = 
                   TaggerAnalysisLib::getDatasetProjectTagAndPeriod(run);
    if ( projTagPeriod.first != projTagPeriod0.first ||
         projTagPeriod.second != projTagPeriod0.second) {

      if (bin!=1) {
        float xmin=h_misIdProb_vs_runNumber->GetXaxis()->GetBinLowEdge(bin);
        float xmax=xmin;
        float ymin=0;
        float ymax=misIdProb_ymax;
        TLine* line=new TLine(xmin,ymin,xmax,ymax);
            line->SetLineStyle(2); 
            line->SetLineColor(17);
            line->Draw();
      }

      TString tmp;
      float y_periodLabel = 1.03*misIdProb_ymax;
      if (logy) {
          float yminLin=TMath::Log10(misIdProb_ymin);
          float ymaxLin=TMath::Log10(misIdProb_ymax);
          float yLin = ymaxLin+0.03*(ymaxLin-yminLin);
          y_periodLabel = TMath::Power(10, yLin);
      }
      TText *periodText = new TText(h_misIdProb_vs_runNumber->GetBinCenter(bin),
                                    y_periodLabel,
                                    (tmp=projTagPeriod.second).ReplaceAll("period",""));
      periodText->SetTextSize(0.025);
      periodText->SetTextColor(13);
      periodText->Draw();

      if (projTagPeriod.second=="periodA" || projTagPeriod.first!=projTagPeriod0.first) {
        TString yearStr = projTagPeriod.first;
        yearStr.ReplaceAll("_13TeV","");
        yearStr.ReplaceAll("data","20");

        float y_yearLabel = 1.1*misIdProb_ymax;
        if (logy) {
          float yminLin=TMath::Log10(misIdProb_ymin);
          float ymaxLin=TMath::Log10(misIdProb_ymax);
          float yLin = ymaxLin+0.1*(ymaxLin-yminLin);
          y_yearLabel = TMath::Power(10, yLin);
        }

        TText* yearText = new TText(h_misIdProb_vs_runNumber->GetBinCenter(bin),
                                    y_yearLabel,
                                    yearStr);
        yearText->SetTextSize(0.025);
        yearText->SetTextColor(13);
        yearText->Draw();
      }

    }
    projTagPeriod0 = projTagPeriod;

    float y_labelRunNum = -0.25*misIdProb_ymax;
    if (logy) {
      float yminLin=TMath::Log10(misIdProb_ymin);
      float ymaxLin=TMath::Log10(misIdProb_ymax);
      float yLin = yminLin-0.25*(ymaxLin-yminLin);
      y_labelRunNum = TMath::Power(10, yLin);
    }
    TText* xlabelRunNum=new TText(h_misIdProb_vs_runNumber->GetBinCenter(bin),
                                  y_labelRunNum,
                                  m_samples[bin-1]->runNumberStr());
    xlabelRunNum->SetTextSize(xlabelsize);
    xlabelRunNum->SetTextAngle(90);
    xlabelRunNum->SetTextAlign(12);
    xlabelRunNum->Draw();
    } // isData

    float y_labelNumEvents = -0.3*misIdProb_ymax;
    if (logy) {
      float yminLin=TMath::Log10(misIdProb_ymin);
      float ymaxLin=TMath::Log10(misIdProb_ymax);
      float yLin = yminLin-0.3*(ymaxLin-yminLin);
      y_labelNumEvents = TMath::Power(10, yLin);
    }
    TText* xlabelNumEvts=new TText(h_misIdProb_vs_runNumber->GetBinCenter(bin),
                              y_labelNumEvents,
                              Form("%8i / %8i",
                              m_samples[bin-1]->numTaggedEvents(k),
                              m_samples[bin-1]->numDijetEvents()));
    xlabelNumEvts->SetTextSize(xlabelsize);
    xlabelNumEvts->SetTextAngle(90);
    xlabelNumEvts->SetTextColor(kBlue);
    xlabelNumEvts->SetTextAlign(32);
    xlabelNumEvts->Draw(); 
  } 

  m_canvas->SetLogy(logy);
  //gaxis->Draw();
  m_canvas->Print(m_outputPDFFile);

  //h_misIdProb_vs_runNumber->SetMinimum(1E-6);
  //m_canvas->SetLogy(1);
  //m_canvas->Print(m_outputPDFFile);
  
  m_canvas->SetLogy(0);
  m_canvas->SetBottomMargin(0.15);
 
  delete gaxis; 
  delete h_meanCorrelationVariable_vs_runNumber;
  delete h_misIdProb_vs_runNumber;
}


void MisIdProbPlots::addPlotMisIdProbVsCorrelationVariable(int methodIndex,
                                                          int correlationVariableIndex) {
  int i = correlationVariableIndex;
  int k = methodIndex;

  cout << endl<<"addPlotMisIdProbVsCorrelationVariable " 
       << m_taggerMethods[k]
       << " " << m_correlationVariables[i] << endl;

  vector<float> varVec;
  vector<float> misIdProbVec;

  TGraphErrors* gr = new TGraphErrors(m_samples.size());
  for (int j=0;j<m_samples.size();j++) {
    gr->SetPoint(j, m_samples[j]->correlationVariableMean(i),
                    m_samples[j]->misIdProb(k).first);
    gr->SetPointError(j, 0, m_samples[j]->misIdProb(k).second);

    varVec.push_back(m_samples[j]->correlationVariableMean(i));
    misIdProbVec.push_back(m_samples[j]->misIdProb(k).first);
  }

  vector<int> rankX;
  vector<int> rankY;

  rankX.assign(varVec.size(), -1);
  rankY.assign(misIdProbVec.size(), -1); 

  int rankCounter = 0;
  do {

    float min_value = FLT_MAX;
    int min_index = -1;

    for (int i=0; i<varVec.size(); i++) 
      if (rankX[i]==-1 && varVec[i] < min_value) {
        min_value = varVec[i];
        min_index = i;
      }

    rankX[min_index] = rankCounter;
    rankCounter++;

  }
  while (rankCounter < varVec.size());

  rankCounter = 0;
  do {

    float min_value = FLT_MAX;
    int min_index = -1;

    for (int i=0; i<misIdProbVec.size(); i++) 
      if (rankY[i]==-1 && misIdProbVec[i] < min_value) {
        min_value = misIdProbVec[i];
        min_index = i;
      }

    rankY[min_index] = rankCounter;
    rankCounter++;

  }
  while (rankCounter < misIdProbVec.size());

  //for (int i = 0;i<rankX.size();i++)
  //  cout << rankX[i] << " " << rankY[i] << endl;
 
  TCanvas canvas;
  canvas.Divide(2,2);
  canvas.cd(1);  
 
  gr->SetMarkerStyle(24);
  gr->SetMarkerColor(kBlack); 
  gr->SetMarkerSize(0.6);
  gr->SetMinimum(0);
  gr->Draw("AP");
  gr->SetTitle("");
  gr->GetXaxis()->SetTitle("<"+TString(m_samples[0]->correlationVariableHists()[i]->GetTitle())+">");
  gr->GetYaxis()->SetTitle(m_taggerMethods[k]+"  P_{mis-id}");

  if (isData()) {
    canvas.cd(2);
    gr->Draw("AP");
    gPad->SetLogy(1);
  }

  canvas.cd(3);
  TGraph* gr_rankCorr = new TGraph(rankX.size());
  gr_rankCorr->SetTitle("");
  for (int i=0;i<rankX.size();i++) 
    gr_rankCorr->SetPoint(i, rankX[i], rankY[i]);
  gr_rankCorr->SetMarkerStyle(25);
  gr_rankCorr->SetMarkerColor(kBlack);
  gr_rankCorr->SetMarkerSize(0.6);
  gr_rankCorr->Draw("AP");

  canvas.cd(4);
  TLegend legend(.2,.2,.8,.8);
  legend.AddEntry(gr_rankCorr,"Rank correlation","P");
  legend.AddEntry(gr_rankCorr,Form("Points=%i",gr_rankCorr->GetN()),"");
  legend.AddEntry(gr_rankCorr,Form("Correlation=%.2f",
                                   gr_rankCorr->GetCorrelationFactor()),"");
  legend.Draw();
  canvas.Print(m_outputPDFFile);
 
  delete gr_rankCorr; 
}


void MisIdProbPlots::addPlotCorrelationVariablesInSample(int sampleIndex) {
  cout<<endl<<"addPlotCorrelationVariablesInSample sample#"<<sampleIndex<<endl;
  TCanvas canvas;
  canvas.Divide(3,3);

  int j = sampleIndex;

  for (int i=0; i<m_correlationVariables.size(); i++) {
    canvas.cd(2+i);
    TH1F* h = m_samples[j]->correlationVariableHists()[i];
    h->Draw();
    TLegend* leg = new TLegend(.3,.5,.9,.9);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->AddEntry(h, Form("Entries=%.0f", h->GetEntries()),"");
    if (m_correlationVariables[i]=="averageInstLuminosityStableBeams")
      leg->AddEntry(h, Form("Int. Lumi weighted mean=%.2f", 
                       m_samples[j]->averageInstLuminosityStableBeams()),"");
    else
      leg->AddEntry(h, Form("Mean=%.2f", h->GetMean()),"");
    leg->AddEntry(h, Form("RMS=%.2f", h->GetRMS()),"");
    leg->Draw();
  }

  canvas.cd(1);
  TLegend leg(.0,.1,1.0,.9);
  leg.AddEntry((TObject*)0, "Correlation variables","");
  leg.AddEntry((TObject*)0, m_samples[j]->runNumberStr(),"l");
  leg.AddEntry((TObject*)0, m_samples[j]->projectTag(),"");
  leg.AddEntry((TObject*)0, m_samples[j]->period(),"");
  leg.AddEntry((TObject*)0, m_samples[j]->stream(),"");
  if (isData())
  leg.AddEntry((TObject*)0, Form("Colliding bunches=%.0f",
                            m_samples[j]->numBunchesColliding()),"");
  leg.Draw();
  canvas.Print(m_outputPDFFile); 
}


void  MisIdProbPlots::addPlotControlVariablesInSample(int sampleIndex) {
  cout<<endl<<"addPlotControlVariablesInSample sample#"<<sampleIndex<<endl;
  TCanvas canvas;
  canvas.Divide(4,4);

  int j = sampleIndex;

  for (int i=0; i<m_controlVariables.size(); i++) {
    canvas.cd(2+i);
    TH1F* h = m_samples[j]->controlVariableHists()[i];
    h->Draw(m_controlVariables[i].Contains(":") ? "colz" : "");
    TLegend* leg = new TLegend(.4,.5,.9,.9);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->AddEntry(h, Form("Entries=%.0f", h->GetEntries()),"");
    leg->AddEntry(h, Form("Mean=%.2f", h->GetMean()),"");
    leg->AddEntry(h, Form("RMS=%.2f", h->GetRMS()),"");
    leg->Draw();
  }

  canvas.cd(1);
  TLegend leg(.1,.1,.9,.9);
  leg.AddEntry((TObject*)0, "Control variables","");
  leg.AddEntry((TObject*)0, m_samples[j]->runNumberStr(),"l");
  leg.AddEntry((TObject*)0, m_samples[j]->projectTag(),"");
  leg.AddEntry((TObject*)0, m_samples[j]->period(),"");
  leg.AddEntry((TObject*)0, m_samples[j]->stream(),"");
  if (isData())
  leg.AddEntry((TObject*)0, Form("Colliding bunches=%.0f",
                            m_samples[j]->numBunchesColliding()),"");
  leg.Draw();
  canvas.Print(m_outputPDFFile);
}


void MisIdProbPlots::addPlotTaggedClustersInSample(int sampleIndex,
                                                              int methodIndex) {

  cout<<endl<<"addPlotTaggedClustersInSample #" << sampleIndex 
      <<" method #" << methodIndex << endl;

  int j = sampleIndex;
  int k = methodIndex;
  TCanvas canvas;
  canvas.Divide(4,3);

  vector<TString> taggedcl_histlist={
    "pt","e","emscale_e","eta","phi","rPerp","z","etatime","fracSamplingMax","fracSamplingMaxIndex"
  };

  canvas.cd(1);
  TLegend leg(0,0,1,1);
  leg.AddEntry((TObject*)0, "Tagged clusters","");
  leg.AddEntry((TObject*)0, m_taggerMethods[k],"");
  leg.AddEntry((TObject*)0, m_samples[j]->runNumberStr(),"l");
  leg.AddEntry((TObject*)0, m_samples[j]->projectTag(),"");
  leg.AddEntry((TObject*)0, m_samples[j]->period(),"");
  leg.AddEntry((TObject*)0, m_samples[j]->stream(),"");
  if (isData())
  leg.AddEntry((TObject*)0, Form("Colliding bunches=%.0f",
                            m_samples[j]->numBunchesColliding()),"");
  leg.Draw();


  for (int ihist=0;ihist<taggedcl_histlist.size();ihist++) {
    canvas.cd(2+ihist);
    TString histname = m_config + "/" + "h_taggedcl_" + m_taggerMethods[k] 
                     + "_"+taggedcl_histlist[ihist];
    TH2F* hist = (TH2F*)m_samples[j]->readEfficiencyAnalysisHist(histname);
    hist->GetXaxis()->SetTitle(taggedcl_histlist[ihist]);  
    if (!hist) 
      throw "addPlotTaggedClustersInSample: failed to read histogram: "+
             histname;
    hist->Draw("colz");
  }
  canvas.Print(m_outputPDFFile);
}

void MisIdProbPlots::addPlotNumMuonSegmentsVsInstLuminosity() {
  cout<<endl<<"addPlotNumMuonSegmentsVsInstLuminosity"<<endl;
  TCanvas canvas;

  int iMDT;
  bool found=false;
  for (iMDT=0;iMDT<m_correlationVariables.size();iMDT++)
    if (m_correlationVariables[iMDT]=="Sum$(MuonSegments.chamberIndex==7||MuonSegments.chamberIndex==8)") {
      found=true;
      break;
  }
  if (!found) throw "MDTI number of segments correlation variable not found";    

  TGraph* grMDT = new TGraph(m_samples.size());
  grMDT->SetTitle("; Inst. Luminosity [1e30 cm-2s-1]; Number of MDT Inner end-cap segments");
  for (int j=0;j<m_samples.size();j++) {
    grMDT->SetPoint(j, m_samples[j]->averageInstLuminosityStableBeams(),
                       m_samples[j]->correlationVariableMean(iMDT));
  }
  grMDT->SetMarkerStyle(25);
  grMDT->SetMarkerSize(0.6);
  grMDT->Draw("AP");
  canvas.Print(m_outputPDFFile);

  int iCSC;
  found=false;
  for (iCSC=0;iCSC<m_correlationVariables.size();iCSC++)
    if (m_correlationVariables[iCSC]=="Sum$(NCB_MuonSegments.chamberIndex==15||NCB_MuonSegments.chamberIndex==16)") {
      found=true;
      break;
  }
  if (!found) throw "CSC number of segments correlation variable not found";
  TGraph* grCSC = new TGraph(m_samples.size());
  grCSC->SetTitle("; Inst. Luminosity [1e30 cm-2s-1]; Number of CSC Inner end-cap segments");
  for (int j=0;j<m_samples.size();j++) {
    grCSC->SetPoint(j, m_samples[j]->averageInstLuminosityStableBeams(),
                       m_samples[j]->correlationVariableMean(iCSC));
  }
  grCSC->SetMarkerStyle(25);
  grCSC->SetMarkerSize(0.6);
  grCSC->Draw("AP");
  canvas.Print(m_outputPDFFile);
}

int MisIdProbPlots::numEventsInTheAODs() {
  fstream finputTxt(m_inputTxtNtuple.Data());
  if (!finputTxt.is_open()) 
    throw "MisIdProbPlots::numEventsInTheAODs failed to open file: "+m_inputTxtNtuple;

  TString filename;
  int totalEventsAODs = 0;
  while (finputTxt >> filename) {
    cout << filename << endl;

    TFile* f = TFile::Open(filename);
    if (!f) {cout<<"failed to open ntuple"<<endl;return 1; }

    TH1F* h_cutflow = (TH1F*)f->Get("cutflow_MisIdProb");
    if (!h_cutflow) {cout<<"failed to read the cutflow histogram"<<endl;return 1;}

    totalEventsAODs += h_cutflow->GetBinContent(1);

    f->Close();
  }
  return totalEventsAODs;
}

void MisIdProbPlots::printMisIdProbTable() {
  TH1F* h_ntagged_sum = (TH1F*)(m_samples[0]->numberOfTaggedEventsHist()->Clone(Form("h_ntagged_sum")));
  for (int j=1;j<m_samples.size();j++) 
    h_ntagged_sum->Add(m_samples[j]->numberOfTaggedEventsHist());

  h_ntagged_sum->Print("all");

  int numEventsAODs = numEventsInTheAODs();
  TString outputTxtFile = m_outputPDFFile;
  outputTxtFile.ReplaceAll(".pdf",".txt"); 
  cout<<"outputTxtFile="<<outputTxtFile<<endl;
  fstream ftxt(outputTxtFile, std::fstream::trunc | std::fstream::out);
  if (!ftxt) 
    throw "MisIdProbPlots::printMisIdProbTable failed to create output txt file";
  ftxt << "----------------------------------------------------------------------"<<endl;
  ftxt << "total events in the AODs: " << numEventsAODs << endl;
  for (int k=0; k<m_taggerMethods.size(); k++)
    ftxt << Form("%-26s%10.0f%20.0f%15.1e",
                  m_taggerMethods[k].Data(),
                  h_ntagged_sum->GetEntries(),
                  h_ntagged_sum->GetBinContent(k+1),
                  h_ntagged_sum->GetBinContent(k+1) / h_ntagged_sum->GetEntries()
                ) << endl;
  ftxt << "----------------------------------------------------------------------"<<endl;
}

int main(int argc, char** argv) {

  TString inputEfficiencyFastDir="";
  TString inputEfficiencyAnalysisDir="";
  TString inputTxtNtuple="";
  TString config="";
  TString outputPDFFile="";
  int minDijetEvents=-1;
  for (int i=1;i<argc;i++) {
    if (TString(argv[i])=="--inputEfficiencyFastDir")
      inputEfficiencyFastDir=argv[i+1];
    if (TString(argv[i])=="--inputEfficiencyAnalysisDir")
      inputEfficiencyAnalysisDir=argv[i+1];
    if (TString(argv[i])=="--inputTxtNtuple")
      inputTxtNtuple=argv[i+1];
    if (TString(argv[i])=="--config")
      config=argv[i+1];
    if (TString(argv[i])=="--outputPDFFile")
      outputPDFFile=argv[i+1];
    if (TString(argv[i])=="--minDijetEvents")
      minDijetEvents=TString(argv[i+1]).Atoi();
  }

  try {

    vector<TString> inputEfficiencyFastHISTFiles=TaggerAnalysisLib::getListOfFilesInDirectory(inputEfficiencyFastDir,"HIST.root");

    bool isData = inputEfficiencyFastHISTFiles[0].Contains("data");

    const vector<TString> controlVariables = {
      "AntiKt4EMTopoJets.n",
      "AntiKt4EMTopoJets.pt[0]over1000",
      "AntiKt4EMTopoJets.phi[0]:AntiKt4EMTopoJets.eta[0]",
      "AntiKt4EMTopoJets.time[0]:AntiKt4EMTopoJets.eta[0]",
      "TMath::Abs(TVector2::Phi_mpi_pi(AntiKt4EMTopoJets.phi[0]-AntiKt4EMTopoJets.phi[1]))_{AntiKt4EMTopoJets.n>=2}",
      "2*(AntiKt4EMTopoJets.pt[0]-AntiKt4EMTopoJets.pt[1])over(AntiKt4EMTopoJets.pt[0]+AntiKt4EMTopoJets.pt[1])_{AntiKt4EMTopoJets.n>=2}",
      "AntiKt4EMTopoJets.emf[0]",
      "AntiKt4EMTopoJets.hecf[0]",
      "AntiKt4EMTopoJets.hecq[0]",
      "AntiKt4EMTopoJets.larq[0]",
      "AntiKt4EMTopoJets.AverageLArQF[0]over65535",
      "AntiKt4EMTopoJets.fracSamplingMax[0]",
      "AntiKt4EMTopoJets.fracSamplingMaxIndex[0]",
      //"AntiKt4EMTopoJets.NumTrkPt1000[0]",
      //"AntiKt4EMTopoJets.isBadLoose[0]",
      "MuonSegments.t0_{MuonSegments.chamberIndex==7||MuonSegments.chamberIndex==8}",
      "NCB_MuonSegments.t0"
    };
 
    vector<TString> correlationVariables;
    if (isData)
      correlationVariables.push_back("averageInstLuminosityStableBeams");
    correlationVariables.push_back("averageInteractionsPerCrossing");
    correlationVariables.push_back("NPV");
    if (isData) 
      correlationVariables.push_back("BunchStructure.numBunchesColliding"); 

    correlationVariables.push_back("Sum$(TopoClusters.emscale_e>10000)");
    //correlationVariables.push_back("Sum$((BeamBackgroundData.resultClus&1)!=0)",

    correlationVariables.push_back("Sum$(TMath::Abs(AntiKt4EMTopoJets.eta)<3.0)");
    
    correlationVariables.push_back("Sum$(MuonSegments.chamberIndex==7||MuonSegments.chamberIndex==8)");
    correlationVariables.push_back("Sum$(NCB_MuonSegments.chamberIndex==15||NCB_MuonSegments.chamberIndex==16)");
  
    MisIdProbPlots analysis(config, correlationVariables, controlVariables, minDijetEvents, inputTxtNtuple, outputPDFFile);

    for (TString inputEfficiencyFastHISTFile : inputEfficiencyFastHISTFiles) {

      TFile* f = TFile::Open(inputEfficiencyFastHISTFile);
      if (!f) { 
        cout << "Warning: failed to open file, skipping: " 
             << inputEfficiencyFastHISTFile<<endl;
        continue;
      }
      if (f->TestBit(TFile::kRecovered)) {
        cout << "Warning: file was recovered, skipping" 
             << inputEfficiencyFastHISTFile<<endl;
        continue;
      }
      if (!f->Get(config)) {
        cout << "Warning: file seems empty" 
             << inputEfficiencyFastHISTFile<< endl;
        continue;
      }
      f->Close();
      analysis.addSample(inputEfficiencyFastHISTFile, 
                         inputEfficiencyAnalysisDir);
    }
    
    analysis.addPlots();
    analysis.printMisIdProbTable();
  }
  catch (const char* err) {cout<<err<<endl;return 1;}
  catch (TString err) {cout<<err<<endl;return 1;} 

  return 0;
}
