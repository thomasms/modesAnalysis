
#include "GaussianFitter.hh"

GaussianFitter::GaussianFitter(TH1F* hist) : _minNDF(20)
{
    _hist = hist;
    _fit = std::make_shared<TF1>("Fit", "gaus");
}

GaussianFitter::~GaussianFitter()
{
}

std::shared_ptr<TF1> GaussianFitter::Fit()
{
    FitPeak();
    return _fit;
}

void GaussianFitter::FitInRange(double mean, double minX, double maxX)
{
    SetMean(mean);
    _fit->SetRange(minX,maxX);
    _hist->Fit("Fit","QOIR");
}

const double GaussianFitter::GetChiSquare() const
{
    const double value = _fit->GetChisquare();
    return value;
}

const double GaussianFitter::GetChiSquarePerNDF() const
{
    const double value = _fit->GetChisquare() / _fit->GetNDF();
    return value;
}

const double GaussianFitter::GetMean() const
{
    const double value = _fit->GetParameter(1);
    return value;
}

const double GaussianFitter::GetSigma() const
{
    const double value = _fit->GetParameter(2);
    return value;
}

void GaussianFitter::SetMean(const double mean)
{
    _fit->SetParameter(1, mean);
    _fit->FixParameter(1, mean);
    //must set sigma limits otherwise yields wierd results
    _fit->SetParLimits(2, _hist->GetXaxis()->GetXmin(), _hist->GetXaxis()->GetXmax());
}

void GaussianFitter::SetHistogram(TH1F* hist)
{
    _hist = hist;
}

void GaussianFitter::SetSigma(const double sigma)
{
    _fit->SetParameter(2, sigma);
    _fit->FixParameter(2, sigma);
}

void GaussianFitter::SetMinNDFInFit(const int ndf)
{
    if(ndf > 2)
        _minNDF = ndf;
}

void GaussianFitter::PrintDetails()
{
    std::cout << "\n----- Fit details -----"
              << "\nMean:          " <<GetMean()
              << "\nSigma:         " <<GetSigma()
              << "\nChiSquare/NDF: " <<GetChiSquarePerNDF() << std::endl;
}

void GaussianFitter::FitPeak()
{
    TSpectrum peakFinder(200);
    int nPeaksFound = peakFinder.Search(_hist,1,"goff");
    
    double* xPeaks = peakFinder.GetPositionX();
    double highestPeak = 0;
    double peakMean = 0;
    for(int p=0;p<nPeaksFound;++p)
    {
        Float_t mean = xPeaks[p];
        Int_t bin = _hist->GetXaxis()->FindBin(mean);
        Float_t value = _hist->GetBinContent(bin);
        
        if(value > highestPeak)
        {
            highestPeak = value;
            peakMean = mean;
        }
    }
    
    // 70% within the peak value
    const double threshold = highestPeak*0.70;
    
    // Get the bin where value falls below threshold
    int lowBin  = 0;
    int highBin = 0;
    bool lowBinSet = false;
    bool highBinSet = false;
    double prevBinContent = 0;
    for(int bin=1;bin<_hist->GetNbinsX();++bin)
    {
        double binContent = _hist->GetBinContent(bin);
        double nextBinContent = _hist->GetBinContent(bin+1);
        double nextNextBinContent = _hist->GetBinContent(bin+2);
        if((prevBinContent <threshold) && (binContent >=threshold) && (nextBinContent >=threshold) && (nextNextBinContent >=threshold) && (lowBinSet == false))
        {
            lowBin = bin;
            lowBinSet = true;
        }
        if((prevBinContent >threshold) && (binContent <=threshold) && (nextBinContent <=threshold) && (nextNextBinContent <=threshold) && (highBinSet == false))
        {
            highBin = bin;
            highBinSet = true;
        }
        prevBinContent = binContent;
    }
    
    double minX = _hist->GetBinCenter(lowBin) - _hist->GetXaxis()->GetBinWidth(lowBin);
    double maxX = _hist->GetBinCenter(highBin);
    FitInRange(peakMean, minX, maxX);
}

void GaussianFitter::ResetParameterLimits()
{
    _fit->ReleaseParameter(1);
    _fit->ReleaseParameter(2);
    _fit->SetParLimits(1, _hist->GetXaxis()->GetXmin(), _hist->GetXaxis()->GetXmax());
    _fit->SetParLimits(2, _hist->GetXaxis()->GetXmin(), _hist->GetXaxis()->GetXmax());
}

double GaussianFitter::PeakFunction(double *x, double *par)
{
    int npeaks = 30;
    double result = par[0] + par[1]*x[0];
    for (int p=0;p<npeaks;p++)
    {
        double norm  = par[3*p+2];
        double mean  = par[3*p+3];
        double sigma = par[3*p+4];
        result += norm*TMath::Gaus(x[0],mean,sigma);
    }
    return result;
}















