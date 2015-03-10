#ifndef FOM_PLOT_HH
#define FOM_PLOT_HH

#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"

#include "BasePlot.hh"
#include "PurityPlot.hh"
#include "EfficiencyPlot.hh"
#include "dataLib.hh"

class FOMPlot : public BasePlot
{
public:
    FOMPlot(const PurityPlot& purity, const EfficiencyPlot& efficiency);
    ~FOMPlot();
    
    void Init();
    void Calculate();
    
    const double GetFOM(const int bin);
    const double GetFOMErrorLow(const int bin);
    const double GetFOMErrorHigh(const int bin);
    
private:
    const double CalculateFOM(const int bin);
    const double CalculateFOMErrorLow(const int bin);
    const double CalculateFOMErrorHigh(const int bin);
    
private:
    std::shared_ptr<PurityPlot> _purity;
    std::shared_ptr<EfficiencyPlot> _efficiency;
};

#endif

