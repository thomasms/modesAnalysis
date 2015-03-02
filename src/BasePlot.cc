#include "BasePlot.hh"

BasePlot::BasePlot() : _nBins(0)
{
    _xValues            = nullptr;
    _xValueErrors       = nullptr;
    _yValues            = nullptr;
    _yValueErrorsLow    = nullptr;
    _yValueErrorsHigh   = nullptr;
}

BasePlot::~BasePlot()
{
    delete _xValues;
    delete _xValueErrors;
    delete _yValues;
    delete _yValueErrorsLow;
    delete _yValueErrorsHigh;
}

void BasePlot::Init()
{
    //Initialise arrays
    _xValues            = new double[_nBins];
    _xValueErrors       = new double[_nBins];
    _yValues            = new double[_nBins];
    _yValueErrorsLow    = new double[_nBins];
    _yValueErrorsHigh   = new double[_nBins];
}


TGraphAsymmErrors* BasePlot::GetGraphCopy()
{
    
    TGraphAsymmErrors* graph = new TGraphAsymmErrors(_nBins,
                                                     &_xValues[0],
                                                     &_yValues[0],
                                                     &_xValueErrors[0],
                                                     &_xValueErrors[0],
                                                     &_yValueErrorsLow[0],
                                                     &_yValueErrorsHigh[0]);
    
    return graph;
}