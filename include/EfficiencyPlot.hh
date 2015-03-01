#ifndef EFFICIENCY_PLOT_HH
#define EFFICIENCY_PLOT_HH

#include <iostream>
#include <algorithm>    // std::copy

#include "TTree.h"
#include "TLine.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TMath.h"
#include "TSystem.h"

#include "dataLib.hh"
class EfficiencyPlot
{
public:
    EfficiencyPlot(const TH1F& hist1, const int nEntries);
    ~EfficiencyPlot();
    
    void Init();
    void CalculateUsingBinomial();
    void CalculateUsingRoot();
    
    TGraphAsymmErrors* GetEfficiencyGraphCopy();
    
    const double GetEfficiency(const int bin);
    const double GetEfficiencyError(const int bin);
    
private:
    TH1F _hist;
    
    int _nBins;
    int _nEntries;
    double _totalSignal;
    
    double* _xValues;
    double* _xValueErrors;
    double* _yValues;
    double* _yValueErrorsLow;
    double* _yValueErrorsHigh;

};

#endif