#ifndef PURITY_PLOT_HH
#define PURITY_PLOT_HH

#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <utility>

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"

#include "BasePlot.hh"
#include "PurityPlot.hh"
#include "EfficiencyPlot.hh"
#include "dataLib.hh"

class PurityPlot : public BasePlot
{
public:
    PurityPlot(const TH1F& signalhist, const TH1F& backgroundHist);
    ~PurityPlot();
    
    void Init();
    void CalculateUsingRoot();
    void CalculateUsingBinomial();
        
    const double GetPurity(const int bin);
    const double GetPurityErrorLow(const int bin);
    const double GetPurityErrorHigh(const int bin);
    
private:
    void ValidateLowError(const double value, double& error);
    void ValidateHighError(const double value, double& error);
    
    const double CalculatePurity(const int bin);
    const double CalculatePurityErrorLow(const int bin);
    const double CalculatePurityErrorHigh(const int bin);
    
    //not used
    const double CalculatePurityErrorOld(const int bin);
    
    const double GetIntegratedSignal(const int bin);
    const double GetIntegratedBackground(const int bin);
    
    const double GetIntegratedSignalError(const int bin);
    const double GetIntegratedBackgroundError(const int bin);
    
    TH1F _signalHist;
    TH1F _backgroundHist;
    
    double _alphaSignal;
    double _alphaBackground;
    
    std::shared_ptr<EfficiencyPlot> _signalEff;
    std::shared_ptr<EfficiencyPlot> _backgroundEff;

};

#endif
