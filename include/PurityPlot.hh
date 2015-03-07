#ifndef PURITY_PLOT_HH
#define PURITY_PLOT_HH

#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"

#include "BasePlot.hh"
#include "dataLib.hh"

class PurityPlot : public BasePlot
{
public:
    PurityPlot(const TH1F& signalhist, const TH1F& backgroundHist);
    ~PurityPlot();
    
    void Init();
    void Calculate();
        
    const double GetPurity(const int bin);
    const double GetPurityError(const int bin);
    
private:
    const double GetIntegratedSignal(const int bin);
    const double GetIntegratedBackground(const int bin);
    
    const double GetIntegratedSignalError(const int bin);
    const double GetIntegratedBackgroundError(const int bin);
    
    TH1F _signalHist;
    TH1F _backgroundHist;
    
    double _alphaSignal;
    double _alphaBackground;

};

#endif
