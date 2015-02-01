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

class Plotter
{
public:
    Plotter(int channel,bool savePlots);
    ~Plotter();
  
    void DrawSpectra(const std::shared_ptr<Source> sourcePtr, bool showBackground=false);
    void DrawSpectraAll(const std::shared_ptr<Source> sourcePtr, bool showBackground=false);

    void DrawPsd(const std::shared_ptr<Source> sourcePtr, bool showBackground=false);
    void DrawPsdAll(const std::shared_ptr<Source> sourcePtr, bool showBackground=false);
    
    void DrawFOM(const std::shared_ptr<Source> sourcePtr);
    void DrawFOMAll(const std::shared_ptr<Source> sourcePtr);
    
private:
    
    const int              GetNumberOfEntries(const TH1F& hist) const;
    const double           GetMean(const TH1F& hist) const;
    const BinaryResult     GetFOM(const TH1F& hist_signal, const TH1F& hist_background, int binMin);
    const BinaryResult     GetPurity(const TH1F& hist_signal, const TH1F& hist_background, int binMin);
    const BinaryResult     GetEfficiency(const TH1F& hist_signal, int binMin);
    const BinaryResult     GetEffXPur(const TH1F& hist_signal, const TH1F& hist_background, int binMin);
    
    const int FindCanvas(const TString& name);
    void SetupHistogram(TH1F& hist,int option);
    void SetupGraphs(const std::shared_ptr<Source> sourcePtr, int channel);
    void WriteGraphs(const std::shared_ptr<Source> sourcePtr, int channel);
    void ResetCanvases();
    void NormaliseHistogram(TH1F& hist,int entries=1);
    void SetErrorBars(TH1F& hist, const double scale);
    void PolishHistogram(TH1F& hist,const TString xtitle,const TString ytitle);
    
    const BinaryResult GetBinaryResult(const double numerator, const double denominator);
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
    bool _savePlots;

    //Legend
    TLegend* legend_;
    
    //Graphs
    TGraphAsymmErrors* g1_fom_s_b;
    TGraphAsymmErrors* g1_eff_s_b;
    TGraphAsymmErrors* g1_pur_s_b;
    TGraphAsymmErrors* g1_effXpur_s_b;
    
    //canvases
    TCanvas * cvs_channel_spectra;
    TCanvas * cvs_channel_spectra_2D;
    TCanvas * cvs_channel_psd;
    TCanvas * cvs_channel_fom;
    TCanvas * cvs_channel_eff;
    
    //Vector of canvases
    std::vector<TCanvas*> canvases;
};
#endif