#include "PurityPlot.hh"

PurityPlot::PurityPlot(const TH1F& signalHist, const TH1F& backgroundHist) : BasePlot()
{
    _signalHist             = signalHist;
    _backgroundHist         = backgroundHist;
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
 
    // Normalise histograms
    _signalHist.Scale(_alphaSignal);
    _backgroundHist.Scale(_alphaBackground);
    
    //Initialise arrays
    _xValues            = new double[_nBins];
    _xValueErrors       = new double[_nBins];
    _yValues            = new double[_nBins];
    _yValueErrorsLow    = new double[_nBins];
    _yValueErrorsHigh   = new double[_nBins];
}

const double PurityPlot::GetPurity(const int bin)
{
    const double integratedSignal     = GetIntegratedSignal(bin);
    const double integratedBackground = GetIntegratedBackground(bin);
    
    const double result = integratedSignal/(integratedSignal + integratedBackground);
    
    return result;
}

const double PurityPlot::GetPurityError(const int bin)
{    
    const double alphaSignalError     = 0.0;
    const double alphaBackgroundError = 0.0;
    
    const double integratedSignal     = GetIntegratedSignal(bin);
    const double integratedBackground = GetIntegratedBackground(bin);
    
    const double integratedSignalError     = GetIntegratedSignalError(bin);
    const double integratedBackgroundError = GetIntegratedBackgroundError(bin);
    
    const double result = TMath::Sqrt( (TMath::Power(_alphaBackground*integratedSignal*integratedBackgroundError,2)) +
                                       (TMath::Power(_alphaSignal*integratedBackground*integratedSignalError,2)) ) /
                                       (integratedBackground + integratedSignal);
    
    std::cout << "\nresult: " << result;
    
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
    
    const double result = _alphaSignal*TMath::Sqrt(errorSquared);
    return result;
}

const double PurityPlot::GetIntegratedBackgroundError(const int bin)
{
    double errorSquared = 0;
    
    for(int b=bin;b<_nBins;b++)
        errorSquared += _backgroundHist.GetBinContent(b);
    
    const double result = _alphaBackground*TMath::Sqrt(errorSquared);
    return result;
}

void PurityPlot::Calculate()
{
    double minXBin      = _signalHist.GetXaxis()->GetXmin();
    double maxXBin      = _signalHist.GetXaxis()->GetXmax();
    
    //loop over histogram bins to get bin values
    for(int bin=0;bin<_nBins;bin++)
    {
        double binWidth = _signalHist.GetBinWidth(0);
        
        _xValues[bin]               = minXBin + (bin+0.5)*binWidth;
        _xValueErrors[bin]          = binWidth*0.5;
        _yValues[bin]               = GetPurity(bin);
        _yValueErrorsLow[bin]       = GetPurityError(bin);
        _yValueErrorsHigh[bin]      = _yValueErrorsLow[bin];
    }
}