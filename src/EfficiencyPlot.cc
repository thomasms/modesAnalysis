#include "EfficiencyPlot.hh"

EfficiencyPlot::EfficiencyPlot(const TH1F& hist) : BasePlot(),
                                _hist(hist)
{
}

EfficiencyPlot::~EfficiencyPlot()
{
}

void EfficiencyPlot::Init()
{
    _nBins       = _hist.GetNbinsX();
    _nEntries    = _hist.GetEntries();
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
    int nEntriesInBin = _hist.GetBinContent(bin);//*_nEntries;  // unnormalised histogram
    
    //use binomial errors
    const double eff = GetEfficiency(bin);
    const double result = TMath::Sqrt((eff*(1-eff))/static_cast<double>(nEntriesInBin));
        
    return result;
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
        _yValues[bin]               = GetEfficiency(bin);
        _yValueErrorsLow[bin]       = GetEfficiencyError(bin);
        _yValueErrorsHigh[bin]      = GetEfficiencyError(bin);
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
        int nEntriesInBin   = _hist.GetBinContent(bin);//*_nEntries;  // unnormalised histogram
        
        passHist.SetBinContent(bin,static_cast<int>(GetEfficiency(bin)*nEntriesInBin));
        totalHist.SetBinContent(bin,nEntriesInBin);
    }
    
    TGraphAsymmErrors* graph = new TGraphAsymmErrors(&passHist,&totalHist);
    
    std::copy(graph->GetX(),graph->GetX() +_nBins -1 ,&_xValues[0]);
    std::copy(graph->GetY(),graph->GetY() +_nBins -1,&_yValues[0]);
    std::copy(graph->GetEXlow(),graph->GetEXlow() +_nBins -1,&_xValueErrors[0]);
    std::copy(graph->GetEYlow(),graph->GetEYlow() +_nBins -1,&_yValueErrorsLow[0]);
    std::copy(graph->GetEYhigh(),graph->GetEYhigh() +_nBins -1,&_yValueErrorsHigh[0]);
    
    delete graph;
}

