#include "EfficiencyPlot.hh"

EfficiencyPlot::EfficiencyPlot(const TH1F& hist) : BasePlot(), _hist(hist)
{
}

EfficiencyPlot::~EfficiencyPlot()
{
}

void EfficiencyPlot::Init()
{
    _nBins       = _hist.GetNbinsX();
    _nEntries    = _hist.GetEntries();
    _totalSignal = _hist.Integral(0, _nBins); // they are the same
    
    //Initialise arrays
    InitVectors();
}

const double EfficiencyPlot::CalculateEfficiency(const int bin)
{
    double integratedSignal = 0;
    
    for(int b=bin;b<_nBins;b++)
        integratedSignal += _hist.GetBinContent(b);
    
    const double result = integratedSignal/_totalSignal;
    return result;
}

const double EfficiencyPlot::CalculateEfficiencyErrorLow(const int bin)
{
    int nEntriesInBin = _hist.GetBinContent(bin);
    
    //use binomial errors
    const double eff = GetEfficiency(bin);
    const double result = TMath::Sqrt((eff*(1-eff))/_totalSignal);
    
    return result;
}

const double EfficiencyPlot::CalculateEfficiencyErrorHigh(const int bin)
{
    return CalculateEfficiencyErrorLow(bin);
}

void EfficiencyPlot::CalculateUsingBinomial()
{
    double minXBin      = _hist.GetXaxis()->GetXmin();
    double maxXBin      = _hist.GetXaxis()->GetXmax();
    
    //loop over histogram bins to get bin values
    for(int bin=0;bin<_nBins;bin++)
    {
        double binWidth = _hist.GetBinWidth(0);
        
        _xValues[bin]               = minXBin + (bin+0.5)*binWidth;
        _xValueErrors[bin]          = binWidth*0.5;
        _yValues[bin]               = CalculateEfficiency(bin);
        _yValueErrorsLow[bin]       = CalculateEfficiencyErrorLow(bin);
        _yValueErrorsHigh[bin]      = CalculateEfficiencyErrorHigh(bin);
    }
}

void EfficiencyPlot::CalculateUsingRoot()
{
    double minXBin      = _hist.GetXaxis()->GetXmin();
    double maxXBin      = _hist.GetXaxis()->GetXmax();
    
    TH1F passHist("passHist","passHist",_nBins, minXBin,maxXBin);
    TH1F totalHist("totalHist","totalHist",_nBins, minXBin,maxXBin);
    
    //loop over histogram bins to set bin values
    for(int bin=0;bin<_nBins;bin++)
    {
        int nPass   = _hist.Integral(bin,_nBins);
        
        passHist.SetBinContent(bin, nPass);
        totalHist.SetBinContent(bin,_nEntries);
        
    }
    
    TGraphAsymmErrors* graph = new TGraphAsymmErrors(&passHist,&totalHist);
    
    std::copy(graph->GetX(),graph->GetX() +_nBins -1 ,&_xValues[0]);
    std::copy(graph->GetY(),graph->GetY() +_nBins -1,&_yValues[0]);
    std::copy(graph->GetEXlow(),graph->GetEXlow() +_nBins -1,&_xValueErrors[0]);
    std::copy(graph->GetEYlow(),graph->GetEYlow() +_nBins -1,&_yValueErrorsLow[0]);
    std::copy(graph->GetEYhigh(),graph->GetEYhigh() +_nBins -1,&_yValueErrorsHigh[0]);
    
    delete graph;
}

const double EfficiencyPlot::GetEfficiency(const int bin)
{
    double result = 0.0;
    
    if(_yValues.size() > bin)
        result = _yValues[bin];
    
    return result;
}

const double EfficiencyPlot::GetEfficiencyErrorAverage(const int bin)
{
    double result = 0.0;
    
    if((_yValueErrorsLow.size() > bin) && (_yValueErrorsHigh.size() > bin))
        result = (GetEfficiencyErrorLow(bin) + GetEfficiencyErrorHigh(bin))/2.0;
    
    return result;
}

const double EfficiencyPlot::GetEfficiencyErrorLow(const int bin)
{
    double result = 0.0;
    
    if(_yValueErrorsLow.size() > bin)
        result = _yValueErrorsLow[bin];
    
    return result;
}

const double EfficiencyPlot::GetEfficiencyErrorHigh(const int bin)
{
    double result = 0.0;
    
    if(_yValueErrorsHigh.size() > bin)
        result = _yValueErrorsHigh[bin];
    
    return result;
}
