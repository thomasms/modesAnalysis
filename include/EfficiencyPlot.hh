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

#include "BasePlot.hh"
#include "dataLib.hh"

class EfficiencyPlot : public BasePlot
{
public:
    EfficiencyPlot(const TH1F& hist1);
    virtual ~EfficiencyPlot();
    
    void Init();
    void CalculateUsingBinomial();
    void CalculateUsingRoot();
        
    const double GetEfficiency(const int bin);
    const double GetEfficiencyError(const int bin);
    
private:
    TH1F _hist;
    
    int _nEntries;
    double _totalSignal;

};

#endif