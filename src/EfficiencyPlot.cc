#include "EfficiencyPlot.hh"

EfficiencyPlot::EfficiencyPlot(const TH1F& hist, const int nEntries) :
                                _hist(hist), _nEntries(nEntries)
{
    _xValues            = nullptr;
    _xValueErrors       = nullptr;
    _yValues            = nullptr;
    _yValueErrorsLow    = nullptr;
    _yValueErrorsHigh   = nullptr;
}

EfficiencyPlot::~EfficiencyPlot()
{
    delete _xValues;
    delete _xValueErrors;
    delete _yValues;
    delete _yValueErrorsLow;
    delete _yValueErrorsHigh;
}

void EfficiencyPlot::Init()
{
    _nBins =_hist.GetNbinsX();
    _totalSignal = _hist.Integral(0,_nBins);
    
    //Initialise arrays
    _xValues            = new double[_nBins];
    _xValueErrors       = new double[_nBins];
    _yValues            = new double[_nBins];
    _yValueErrorsLow    = new double[_nBins];
    _yValueErrorsHigh   = new double[_nBins];
}

const double EfficiencyPlot::GetEfficiency(const int bin)
{
    double integratedSignal = 0;
    
    for(int b=bin;b<_nBins;b++)
        integratedSignal += _hist.GetBinContent(b);
    
    const double result = integratedSignal/_totalSignal;
    return result;
}

const double EfficiencyPlot::GetEfficiencyError(const int bin)
{
    double nEntriesInBin = _hist.GetBinContent(bin)*_nEntries;
    
    //use binomial errors
    const double eff = GetEfficiency(bin);
    const double result = TMath::Sqrt((eff*(1-eff))/static_cast<double>(nEntriesInBin));
    
    return result;
}

void EfficiencyPlot::Calculate()
{
    double minXBin      = _hist.GetXaxis()->GetXmin();
    double maxXBin      = _hist.GetXaxis()->GetXmax();
    
    //loop over histogram bins to get bin values
    for(int bin=0;bin<_nBins;bin++)
    {
        double binWidth = _hist.GetBinWidth(0);
        
        _xValues[bin]               = minXBin + (bin+0.5)*binWidth;
        _xValueErrors[bin]          = binWidth*0.5;
        _yValues[bin]               = GetEfficiency(bin);
        _yValueErrorsLow[bin]       = GetEfficiencyError(bin);
        _yValueErrorsHigh[bin]      = GetEfficiencyError(bin);
    }
}

TGraphAsymmErrors* EfficiencyPlot::GetEfficiencyGraphCopy()
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

