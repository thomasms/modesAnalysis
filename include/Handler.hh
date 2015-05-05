#ifndef HANDLER_HH
#define HANDLER_HH

#include <vector>
#include <iostream>
#include <memory>

#include "TInterpreter.h"
#include "TFrame.h"
#include "TFile.h"
#include "TTree.h"
#include "TLine.h"
#include "TString.h"
#include "TF1.h"
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

#include "ChannelData.hh"
#include "TubeData.hh"
#include "GaussianFitter.hh"
#include "Utils.hh"
#include "dataLib.hh"

const int TUBES = 4;
const int CHANNELS = 8;

class Handler
{
  
public:
    Handler();
    Handler(TString signalName,TString backgroundName,int binning,double temp,double pres);
    ~Handler();
  
    void Process(const std::vector<TTree*>& treePtr, bool signal, float timeCutOffInMins=5);
    void SetupSource();
    void RequireSameNrOfEventsPerTube(bool option)      {_sameNrOfEventsPerTube = option;};
      
    EventParameters GetParameters(int event);
    EventData GetData(int event);
    
    std::shared_ptr<Source> GetSourcePtr() {return _source;};
private:
    void InitialiseHistograms(bool signal);
    void ProcessData(TTree* treePtr, bool signal, float timeCutOffInMins);
    void FillHistograms(const ChannelData& data, bool signal);
    void ResetHistograms(int channel, bool signal);
    
    TBranch* GetChannelDataBranch(TTree* treePtr, int channel) { return treePtr->GetBranch(Form("acq_ch%i",channel));};
    
private:
  
    EventParameters          _parameters;
    EventData                _data;
    std::shared_ptr<Source>  _source;
  
    //flags
    bool _sameNrOfEventsPerTube;
    
    //histograms
    int _binningFactor;
    TH1F* _h1_channelSpectra;
    TH1F* _h1_channelQShort;
    TH2F* _h2_channelSpectra;
    TH1F* _h1_channelPsd;

    //vectors of histograms for each channel
    std::vector<TH1F*> _h1Vtr_channelsSpectraSig;
    std::vector<TH1F*> _h1Vtr_channelsQShortSig;
    std::vector<TH2F*> _h2Vtr_channelsSpectraSig;
    std::vector<TH1F*> _h1Vtr_channelsSpectraBkg;
    std::vector<TH1F*> _h1Vtr_channelsQShortBkg;
    std::vector<TH1F*> _h1Vtr_channelsSpectraSigMinusBkg;
    std::vector<TH1F*> _h1Vtr_channelsPsdSig;
    std::vector<TH1F*> _h1Vtr_channelsPsdBkg;
    
    
};

#endif
