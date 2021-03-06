#ifndef PLOTTER1_HH
#define PLOTTER1_HH

#include <iostream>
#include <sstream>
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
#include "TGaxis.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TMath.h"
#include "TLegend.h"
#include "TNtuple.h"
#include "TPaveStats.h"
#include "TPaveText.h"
#include "TPaveLabel.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TSpectrum.h"
#include "THistPainter.h"
#include "TPad.h"
#include "TRandom3.h"
#include "TStyle.h"

#include "dataLib.hh"
#include "EfficiencyPlot.hh"
#include "PurityPlot.hh"
#include "FOMPlot.hh"
#include "GaussianFitter.hh"

class Plotter
{
public:
    Plotter(int channel,bool savePlots, bool showPeaks);
    ~Plotter();
    
    void DrawSpectra(const std::shared_ptr<Source> sourcePtr, bool showBackground=false);
    void DrawSpectraAll(const std::shared_ptr<Source> sourcePtr, bool showBackground=false);
    
    void DrawQShort(const std::shared_ptr<Source> sourcePtr, bool showBackground=false);
    void DrawQShortAll(const std::shared_ptr<Source> sourcePtr, bool showBackground=false);

    void DrawPsd(const std::shared_ptr<Source> sourcePtr, bool showBackground=false);
    void DrawPsdAll(const std::shared_ptr<Source> sourcePtr, bool showBackground=false);
    
    void DrawFOM(const std::shared_ptr<Source> sourcePtr);
    void DrawFOMAll(const std::shared_ptr<Source> sourcePtr);

    void UpdateCanvases();
    
private:
    
    const int FindCanvas(const TString& name);
    std::shared_ptr<TF1> FitHistogram(TH1F* hist);
    void SetupHistogram(TH1F& hist, int channel, int option);
    void ChangeRangeToFitBoth(TH1F& hist_signal, TH1F& hist_background);
    void SetupGraphs(const std::shared_ptr<Source> sourcePtr, int channel);
    void WriteGraphs(const std::shared_ptr<Source> sourcePtr, int channel);
    void ResetCanvases();
    void NormaliseHistogram(TH1F& hist);
    void SetErrorBars(TH1F& hist, const double scale);
    void PolishHistogram(TH1F& hist,const TString xtitle,const TString ytitle);
    void PrintSpectraDetails(const std::shared_ptr<Source> sourcePtr, int channel);
    void PrintQShortDetails(const std::shared_ptr<Source> sourcePtr, int channel);
    void PrintPSDDetails(const std::shared_ptr<Source> sourcePtr, int channel);
    
    template<class T>
    void WriteToFile(const T& hist,const TString histname,const TString filename)
    {
        //create new file to write histgram to, if exists update it
        TFile fileToWrite(filename,"UPDATE");
        fileToWrite.cd();
        hist.Write(histname);
        fileToWrite.Close();
    }

private:

    int _channel;
    bool _savePlots, _showPeaks;
    double _statBoxHeight;
    
    //Legend
    TLegend* legend_;
    
    //Graphs
    TGraphAsymmErrors* g1_fom_s_b;
    TGraphAsymmErrors* g1_eff_s_b;
    TGraphAsymmErrors* g1_pur_s_b;
    TGraphAsymmErrors* g1_effXpur_s_b;
    
    //canvases
    TCanvas * cvs_channel_spectra;
    TCanvas * cvs_channel_qshort;
    TCanvas * cvs_channel_spectra_2D;
    TCanvas * cvs_channel_psd;
    TCanvas * cvs_channel_fom;
    TCanvas * cvs_channel_eff;
    
    //Vector of canvases
    std::vector<TCanvas*> canvases;
};
#endif
