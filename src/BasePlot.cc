#include "BasePlot.hh"

BasePlot::BasePlot() : _nBins(0)
{
}

BasePlot::~BasePlot()
{
}

void BasePlot::Init()
{
    InitVectors();
}

void BasePlot::InitVectors()
{
    _xValues.resize(_nBins,0.0);
    _xValueErrors.resize(_nBins,0.0);
    _yValues.resize(_nBins,0.0);
    _yValueErrorsLow.resize(_nBins,0.0);
    _yValueErrorsHigh.resize(_nBins,0.0);
}

const double BasePlot::GetBestValue()
{
    double highestValue = 0.0;
    
    for(int i=0;i<_yValues.size();++i)
    {
        if(_yValues[i] >=highestValue)
            highestValue = _yValues[i];
    }
    return highestValue;
}

const double BasePlot::GetBestCut()
{
    double highestValue = 0.0;
    double bestCut = 0.0;
    
    for(int i=0;i<_yValues.size();++i)
    {
        if(_yValues[i] >=highestValue)
        {
            highestValue = _yValues[i];
            bestCut = _xValues[i];
        }
    }
    return bestCut;
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


const double BasePlot::GetXmin()
{
    double result = 0.0;
    if(_xValues.size() <= 0)
        return result;
    
    //sort vector
    std::sort(_xValues.begin(), _xValues.end());
    result = _xValues[0];
    
    return result;
}

const double BasePlot::GetXmax()
{
    double result = 0.0;
    if(_xValues.size() <= 0)
        return result;
    
    //sort vector
    std::sort(_xValues.begin(), _xValues.end());
    result = _xValues[_xValues.size() - 1];
    
    return result;
}

const double BasePlot::GetXwidth()
{
    double result = 0.0;
    if(_nBins <= 0)
        return result;
    
    result = std::abs(2*_xValueErrors[0]);
    
    return result;
}