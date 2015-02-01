#ifndef HANDLER_HH
#define HANDLER_HH

#include <vector>
#include <iostream>
#include <memory>
#include <vector>

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

const int CHANNELS = 8;

class Handler
{
  
public:
    Handler();
    Handler(TString signalName,TString backgroundName,int binning,double temp,double pres);
    ~Handler();
  
    void ProcessData(TTree* treePtr, bool signal, float timeCutOffInMins=5);
    void InitialiseHistograms(int channel, bool signal);
    void FillHistograms(int channel, bool signal, float Qlong, float Qshort);
    void SubtractBackground();
    void SetupSource();
    
    TH1F* GetSignalSpectraHistPtr(int channel);
    TH2F* GetSignalSpectra2DHistPtr(int channel);
    TH1F* GetSignalPsdHistPtr(int channel);
  
    EventParameters GetParameters(int event);
    EventData GetData(int event);
    
    std::shared_ptr<Source> GetSourcePtr() {return _source;};
private:
  
    EventParameters          _parameters;
    EventData                _data;
    std::shared_ptr<Source>  _source;
  
    //histograms
    int _binningFactor;
    TH1F* _h1_channelSpectra;
    TH2F* _h2_channelSpectra;
    TH1F* _h1_channelPsd;

    //vectors of histograms for each channel
    std::vector<TH1F*> _h1Vtr_channelsSpectraSig;
    std::vector<TH2F*> _h2Vtr_channelsSpectraSig;
    std::vector<TH1F*> _h1Vtr_channelsSpectraBkg;
    std::vector<TH1F*> _h1Vtr_channelsSpectraSigMinusBkg;
    std::vector<TH1F*> _h1Vtr_channelsPsdSig;
    std::vector<TH1F*> _h1Vtr_channelsPsdBkg;
    
};

#endif
