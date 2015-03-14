#include "PurityPlot.hh"

const double smallLimit = 0.0;//1.0e-4;

PurityPlot::PurityPlot(const TH1F& signalHist, const TH1F& backgroundHist) : BasePlot()
{
    _signalHist             = signalHist;
    _backgroundHist         = backgroundHist;
    
    _signalEff     = std::make_shared<EfficiencyPlot>(_signalHist);
    _backgroundEff = std::make_shared<EfficiencyPlot>(_backgroundHist);
}

PurityPlot::~PurityPlot()
{
}

void PurityPlot::Init()
{
    _nBins                  = _signalHist.GetNbinsX();
    _alphaSignal            = 1.0/static_cast<double>(_signalHist.GetEntries());
    _alphaBackground        = 1.0/static_cast<double>(_backgroundHist.GetEntries());
    
    //check they have the same binning
    if(_nBins != _backgroundHist.GetNbinsX())
        return;
    
    //Initialise arrays
    InitVectors();
    
    //Initialise efficiencies
    _signalEff->Init();
    _backgroundEff->Init();
}

const double PurityPlot::CalculatePurity(const int bin)
{
//    const double integratedSignal     = GetIntegratedSignal(bin);
//    const double integratedBackground = GetIntegratedBackground(bin);
//    
//    const double result = _alphaSignal*integratedSignal/(_alphaSignal*integratedSignal + _alphaBackground*integratedBackground);
    
    const double signalEff     = _signalEff->GetEfficiency(bin);
    const double backgroundEff = _backgroundEff->GetEfficiency(bin);
    
    if(signalEff <= smallLimit) return 0.0;
    
    const double result = 1.0/(1.0 + (backgroundEff/signalEff));
    
    return result;
}

const double PurityPlot::CalculatePurityErrorLow(const int bin)
{
    const double signalEff     = _signalEff->GetEfficiency(bin);
    const double backgroundEff = _backgroundEff->GetEfficiency(bin);
    
    if(signalEff <= smallLimit) return 0.0;
    
    const double signalError     = _signalEff->GetEfficiencyErrorLow(bin);
    const double backgroundError = _backgroundEff->GetEfficiencyErrorLow(bin);
    
    const double purity = CalculatePurity(bin);
    
    const double signalErrorCoeff     = backgroundEff*TMath::Power(purity/signalEff,2.0);
    const double backgroundErrorCoeff = TMath::Power(purity,2.0)/signalEff;
    
    double result = TMath::Sqrt( (TMath::Power(signalErrorCoeff*signalError,2.0)) + (TMath::Power(backgroundErrorCoeff*backgroundError,2.0)) );
    
    ValidateLowError(purity, result);
    
    return result;
}

const double PurityPlot::CalculatePurityErrorHigh(const int bin)
{
    const double signalEff     = _signalEff->GetEfficiency(bin);
    const double backgroundEff = _backgroundEff->GetEfficiency(bin);
    
    if(signalEff <= smallLimit) return 0.0;
    
    const double signalError     = _signalEff->GetEfficiencyErrorHigh(bin);
    const double backgroundError = _backgroundEff->GetEfficiencyErrorHigh(bin);
    
    const double purity = CalculatePurity(bin);
    
    const double signalErrorCoeff     = backgroundEff*TMath::Power(purity/signalEff,2.0);
    const double backgroundErrorCoeff = TMath::Power(purity,2.0)/signalEff;
    
    double result = TMath::Sqrt( (TMath::Power(signalErrorCoeff*signalError,2.0)) + (TMath::Power(backgroundErrorCoeff*backgroundError,2.0)) );
    
    ValidateHighError(purity, result);
    
    return result;
}

// Old way of calculating methods - not currently used
const double PurityPlot::CalculatePurityErrorOld(const int bin)
{
    const double alphaSignalError     = TMath::Power(_signalHist.GetEntries(),-3.0/2.0);
    const double alphaBackgroundError = TMath::Power(_backgroundHist.GetEntries(),-3.0/2.0);
    
    const double integratedSignal     = GetIntegratedSignal(bin);
    const double integratedBackground = GetIntegratedBackground(bin);
    
    const double integratedSignalError     = _alphaSignal*GetIntegratedSignalError(bin);
    const double integratedBackgroundError = _alphaBackground*GetIntegratedBackgroundError(bin);
    
    const double purity = CalculatePurity(bin);
    
    const double result = (purity*purity / (_alphaSignal*integratedSignal))*
                            TMath::Sqrt( (TMath::Power(_alphaBackground*integratedBackgroundError,2)) +
                                        (TMath::Power(_alphaBackground*integratedBackground*integratedSignalError/integratedSignal,2))  +
                                        (TMath::Power(integratedBackground*alphaBackgroundError,2))  +
                                        (TMath::Power(_alphaBackground*integratedBackground*alphaSignalError/_alphaSignal,2))  );
    
    //std::cout << "\nError for the " << bin << " th bin: " << result;
    
    return result;
}

const double PurityPlot::GetIntegratedSignal(const int bin)
{
    double result = 0;
    
    for(int b=bin;b<_nBins;b++)
        result += _signalHist.GetBinContent(b);
    
    return result;
}

const double PurityPlot::GetIntegratedBackground(const int bin)
{
    double result = 0;
    
    for(int b=bin;b<_nBins;b++)
        result += _backgroundHist.GetBinContent(b);
    
    return result;
}

const double PurityPlot::GetIntegratedSignalError(const int bin)
{
    double errorSquared = 0;
    
    for(int b=bin;b<_nBins;b++)
        errorSquared += _signalHist.GetBinContent(b);
    
    const double result = TMath::Sqrt(errorSquared);
    return result;
}

const double PurityPlot::GetIntegratedBackgroundError(const int bin)
{
    double errorSquared = 0;
    
    for(int b=bin;b<_nBins;b++)
        errorSquared += _backgroundHist.GetBinContent(b);
    
    const double result = TMath::Sqrt(errorSquared);
    return result;
}

void PurityPlot::CalculateUsingBinomial()
{
    double minXBin      = _signalHist.GetXaxis()->GetXmin();
    double maxXBin      = _signalHist.GetXaxis()->GetXmax();
    
    _signalEff->CalculateUsingBinomial();
    _backgroundEff->CalculateUsingBinomial();
    
    //loop over histogram bins to get bin values
    for(int bin=0;bin<_nBins;bin++)
    {
        double binWidth = _signalHist.GetBinWidth(0);
        
        _xValues[bin]               = minXBin + (bin+0.5)*binWidth;
        _xValueErrors[bin]          = binWidth*0.5;
        _yValues[bin]               = CalculatePurity(bin);
        _yValueErrorsLow[bin]       = CalculatePurityErrorLow(bin);
        _yValueErrorsHigh[bin]      = CalculatePurityErrorHigh(bin);
    }
}

void PurityPlot::CalculateUsingRoot()
{
    double minXBin      = _signalHist.GetXaxis()->GetXmin();
    double maxXBin      = _signalHist.GetXaxis()->GetXmax();
    
    _signalEff->CalculateUsingRoot();
    _backgroundEff->CalculateUsingRoot();
    
    //loop over histogram bins to get bin values
    for(int bin=0;bin<_nBins;bin++)
    {
        double binWidth = _signalHist.GetBinWidth(0);
        
        _xValues[bin]               = minXBin + (bin+0.5)*binWidth;
        _xValueErrors[bin]          = binWidth*0.5;
        _yValues[bin]               = CalculatePurity(bin);
        _yValueErrorsLow[bin]       = CalculatePurityErrorLow(bin);
        _yValueErrorsHigh[bin]      = CalculatePurityErrorHigh(bin);
    }
}

void PurityPlot::ValidateLowError(const double value, double& error)
{
    if((value - error) < 0)
        error = value;
}

void PurityPlot::ValidateHighError(const double value, double& error)
{
    if((value + error) > 1)
        error = 1 - value;
}

const double PurityPlot::GetPurity(const int bin)
{
    double result = 0.0;
    
    if(_yValues.size() > bin)
        result = _yValues[bin];
    
    return result;
}

const double PurityPlot::GetPurityErrorLow(const int bin)
{
    double result = 0.0;
    
    if(_yValueErrorsLow.size() > bin)
        result = _yValueErrorsLow[bin];
    
    return result;
}

const double PurityPlot::GetPurityErrorHigh(const int bin)
{
    double result = 0.0;
    
    if(_yValueErrorsHigh.size() > bin)
        result = _yValueErrorsHigh[bin];
    
    return result;
}
