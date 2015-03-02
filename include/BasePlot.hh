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
    
    void Init();
    void SetNumberOfBins(int nBins) {_nBins = nBins;};
    
    TGraphAsymmErrors* GetGraphCopy();
    
protected:
    
    int _nBins;
        
    double* _xValues;
    double* _xValueErrors;
    double* _yValues;
    double* _yValueErrorsLow;
    double* _yValueErrorsHigh;
    
};

#endif
