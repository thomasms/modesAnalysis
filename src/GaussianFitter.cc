#include "GaussianFitter.hh"

GaussianFitter::GaussianFitter(TH1F* hist) : _minNDF(20)
{
    _hist = hist;
    _fit = new TF1("Fit","gaus");
}

GaussianFitter::~GaussianFitter()
{
    delete _fit;
}

void GaussianFitter::Fit()
{
    OptimiseFit();
}

void GaussianFitter::FitInRange(double mean, double minX, double maxX)
{
    SetMean(mean);
    _fit->SetRange(minX,maxX);
    _hist->Fit("Fit","Q0BR");
}

TF1* GaussianFitter::GetFitFunction()
{
    return _fit;
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

void GaussianFitter::OptimiseFit()
{
    double firstMeanGuess = _hist->GetXaxis()->GetBinCenter(_hist->GetMaximumBin());
    double binWidth = _hist->GetXaxis()->GetBinWidth(0);
    double minX = _hist->GetXaxis()->GetXmin();
    double maxX = _hist->GetXaxis()->GetXmax();
    
    // Initial fit
    FitInRange(firstMeanGuess, minX,maxX);
    
    //refine the range to within 3 sigma
    minX = firstMeanGuess - 3*TMath::Abs(GetSigma());
    maxX = firstMeanGuess + 3*TMath::Abs(GetSigma());
    _fit->SetRange(minX,maxX);
    
    double bestChiSquare = 0;
    double ndf =_fit->GetNDF();

    //Now alter mean
    _fit->ReleaseParameter(1);
    ResetParameterLimits();
    
    int counter = 0;
    const int limit = 20;
    
    while( ndf >_minNDF )
    {
        if(bestChiSquare==0)
            bestChiSquare = GetChiSquarePerNDF();

        minX += binWidth/2.0;
        maxX -= binWidth/2.0;
        
        _fit->SetRange(minX,maxX);
        _hist->Fit("Fit","QNBR");
        ndf =_fit->GetNDF();

        if((bestChiSquare <= GetChiSquarePerNDF()) || (counter == limit))
            break;

        counter++;
    }
    _hist->Fit("Fit","Q0BR");
    
}

void GaussianFitter::ResetParameterLimits()
{
    _fit->SetParLimits(1, _hist->GetXaxis()->GetXmin(), _hist->GetXaxis()->GetXmax());
    _fit->SetParLimits(2, _hist->GetXaxis()->GetXmin(), _hist->GetXaxis()->GetXmax());
}

















