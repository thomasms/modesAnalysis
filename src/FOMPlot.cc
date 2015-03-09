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
    
    //Initialise arrays
    InitVectors();
}

const double FOMPlot::GetFOM(const int bin)
{
    const double purityValue     = _purity->GetPurity(bin);
    const double efficiencyValue = _efficiency->GetEfficiency(bin);
    
    const double result = purityValue*efficiencyValue;
    
    return result;
}

const double FOMPlot::GetFOMError(const int bin)
{
    const double purityValue     = _purity->GetPurity(bin);
    const double efficiencyValue = _efficiency->GetEfficiency(bin);
    
    const double purityError     = _purity->GetPurityError(bin);
    const double efficiencyError = _efficiency->GetEfficiencyError(bin);
    
    const double result = TMath::Sqrt( (TMath::Power(purityError*purityValue,2)) +
                                       (TMath::Power(efficiencyError*efficiencyValue,2)) );
    
    //std::cout << "\nError for the " << bin << " th bin: " << result;
    
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
        _yValues[bin]               = GetFOM(bin);
        _yValueErrorsLow[bin]       = GetFOMError(bin);
        _yValueErrorsHigh[bin]      = _yValueErrorsLow[bin];
    }
}