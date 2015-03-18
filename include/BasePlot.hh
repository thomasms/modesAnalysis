#ifndef BASE_PLOT_HH
#define BASE_PLOT_HH

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

class BasePlot
{
public:
    BasePlot();
    virtual ~BasePlot();
    
    virtual void Init();
    virtual void InitVectors();
    virtual const double GetBestValue();
    virtual const double GetBestValueError();
    virtual const double GetBestCut();
    
    void      SetNumberOfBins(int nBins) {_nBins = nBins;};
    const int GetNumberOfBins() { return _nBins;};
    
    const double GetXmin();
    const double GetXmax();
    const double GetXwidth();
    
    TGraphAsymmErrors* GetGraphCopy();
    
protected:
    
    int _nBins;
        
    std::vector<double> _xValues;
    std::vector<double> _xValueErrors;
    std::vector<double> _yValues;
    std::vector<double> _yValueErrorsLow;
    std::vector<double> _yValueErrorsHigh;
    
};

#endif
