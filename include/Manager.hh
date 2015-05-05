#ifndef MANAGER_HH
#define MANAGER_HH

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <utility>

#include "TInterpreter.h"
#include "TFrame.h"
#include "TFile.h"
#include "TTree.h"
#include "TLine.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TMath.h"
#include "TLegend.h"
#include "TNtuple.h"
#include "TPaveText.h"
#include "TPaveLabel.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "THistPainter.h"
#include "TPad.h"
#include "TRandom3.h"

#include "dataLib.hh"
#include "dataCards.hh"
#include "dataCardsRecord.hh"
#include "Handler.hh"

class Manager
{
public:
    Manager();
    ~Manager();
  
    void Initialise();
    void InitDataCards();
    void LoadDataCards();
    void GetRunTimeArguments(int argc,char** argv);
    void Process();
    void Plot();
    void AddFileToSignal(const TString fileToAdd, int index);
    void AddFileToBackground(const TString fileToAdd, int index);
  
    dataCards* GetDataCardPtr() 	{return _cards;};
  
    //Get input parameters
    const TString GetSignalFileName()  	{return _signalFile;};
    const TString GetBackgdFileName()  	{return _backgroundFile;};
    const double GetCutOffTime()	    {return _timeCutOff;};
    const double GetPSDCut()            {return _psdCut;};
    const double GetContamUpperLevel()	{return _contaminationUpperLevel;};
    const double GetContamLowerLevel()	{return _contaminationLowerLevel;};
    const int GetNumberOfFiles()        {return _files;};
    const int GetNumberOfTubes()        {return _tubes;};
    const int GetNumberOfExp()          {return _experiments;};
  
private:  
    void RemoveFileFromSignal(int index);
    void RemoveFileFromBackground(int index);
    void CleanUp();

protected:
    dataCards* _cards;  
    std::shared_ptr<Handler> _handler;

    //file vectors
    std::vector<TFile*> _signalFileVtr;
    std::vector<TFile*> _backgrFileVtr;
  
    //tree vectors
    std::vector<TTree*> _signalTreeVtr;
    std::vector<TTree*> _backgrTreeVtr;
  
    //file pointers
    TFile* _signalTFile;
    TFile* _backgrTFile;

    //tree pointers
    TTree* _signalTTree;
    TTree* _backgrTTree;
  
    //input parameters
    TString _signalFile,_backgroundFile,_signalName,_backgroundName,_saveDir;
    double _timeCutOff,_psdCut,_contaminationUpperLevel,_contaminationLowerLevel;
    bool _savePlots,_logPlot,_showBackground, _showPeaks, _takeRMS;
    int _files,_experiments,_meanNoEvents,_tubes,_binning, _channel;
    double _temperature,_pressure,_activity,_distance;
};
#endif
