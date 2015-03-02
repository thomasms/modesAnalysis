#include "PurityPlot.hh"

PurityPlot::PurityPlot(const TH1F& signalHist, const TH1F& backgroundHist, const int signalHistEntries, const int backgroundHistEntries) : BasePlot()
{
    _signalHist             = signalHist;
    _backgroundHist         = backgroundHist;
    _nSignalEntries         = signalHistEntries;
    _nBackgroundEntries     = backgroundHistEntries;
}

PurityPlot::~PurityPlot()
{

}

void PurityPlot::Init()
{
    _nBins = _signalHist.GetNbinsX();
    
    //check they have the same binning
    if(_nBins != _backgroundHist.GetNbinsX())
        return;
    
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
    const double alphaSignal     = 1.0/static_cast<double>(_nSignalEntries);
    const double alphaBackground = 1.0/static_cast<double>(_nBackgroundEntries);
    
    const double integratedSignal     = GetIntegratedSignal(bin);
    const double integratedBackground = GetIntegratedBackground(bin);
    
    const double integratedSignalError = TMath::Sqrt(integratedSignal*alphaSignal);
    const double integratedBackgroundError = TMath::Sqrt(integratedBackground*alphaBackground);
    
    const double result =
        (1.0/(integratedBackground*TMath::Power( (1 + (integratedSignal/integratedBackground)) ,2)))*
         TMath::Sqrt( (integratedSignalError*integratedSignalError*alphaSignal*alphaSignal) +
                      (integratedSignal*integratedSignal*integratedBackgroundError*integratedBackgroundError*
                       alphaBackground*alphaBackground) );
    
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