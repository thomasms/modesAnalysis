#include "FOMPlot.hh"

FOMPlot::FOMPlot(const PurityPlot& purity, const EfficiencyPlot& efficiency) : BasePlot()
{
    _purity     = std::make_shared<PurityPlot>(purity);
    _efficiency = std::make_shared<EfficiencyPlot>(efficiency);
}

FOMPlot::~FOMPlot()
{
}

void FOMPlot::Init()
{
    //check they have the same binning
    if(_purity->GetNumberOfBins() != _efficiency->GetNumberOfBins())
        return;
    
    _nBins = _purity->GetNumberOfBins();
    
    InitVectors();
}

const double FOMPlot::CalculateFOM(const int bin)
{
    const double purityValue     = _purity->GetPurity(bin);
    const double efficiencyValue = _efficiency->GetEfficiency(bin);
    
    const double result = purityValue*efficiencyValue;
    
    return result;
}

const double FOMPlot::CalculateFOMErrorLow(const int bin)
{
    const double purityValue     = _purity->GetPurity(bin);
    const double efficiencyValue = _efficiency->GetEfficiency(bin);
    
    const double purityError     = _purity->GetPurityErrorLow(bin);
    const double efficiencyError = _efficiency->GetEfficiencyErrorLow(bin);
    
    const double result = TMath::Sqrt( (TMath::Power(purityError*efficiencyValue,2)) +
                                      (TMath::Power(efficiencyError*purityValue,2)) );
    
    //std::cout << "\nError for the " << bin << " th bin: " << result;
    
    return result;
}

const double FOMPlot::CalculateFOMErrorHigh(const int bin)
{
    const double purityValue     = _purity->GetPurity(bin);
    const double efficiencyValue = _efficiency->GetEfficiency(bin);
    
    const double purityError     = _purity->GetPurityErrorHigh(bin);
    const double efficiencyError = _efficiency->GetEfficiencyErrorHigh(bin);
    
    const double result = TMath::Sqrt( (TMath::Power(purityError*efficiencyValue,2)) +
                                      (TMath::Power(efficiencyError*purityValue,2)) );
    
    return result;
}

void FOMPlot::Calculate()
{
    double minXBin      = _purity->GetXmin();
    double maxXBin      = _purity->GetXmax();
    
    //loop over histogram bins to get bin values
    for(int bin=0;bin<_nBins;bin++)
    {
        double binWidth = _purity->GetXwidth();
        
        _xValues[bin]               = minXBin + (bin+0.5)*binWidth;
        _xValueErrors[bin]          = binWidth*0.5;
        _yValues[bin]               = CalculateFOM(bin);
        _yValueErrorsLow[bin]       = CalculateFOMErrorLow(bin);
        _yValueErrorsHigh[bin]      = CalculateFOMErrorHigh(bin);
    }
}

const double FOMPlot::GetFOM(const int bin)
{
    double result = 0.0;
    
    if(_yValues.size() > bin)
        result = _yValues[bin];
    
    return result;
}

const double FOMPlot::GetFOMErrorLow(const int bin)
{
    double result = 0.0;
    
    if(_yValueErrorsLow.size() > bin)
        result = _yValueErrorsLow[bin];
    
    return result;
}

const double FOMPlot::GetFOMErrorHigh(const int bin)
{
    double result = 0.0;
    
    if(_yValueErrorsHigh.size() > bin)
        result = _yValueErrorsHigh[bin];
    
    return result;
}