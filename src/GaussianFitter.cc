#include "GaussianFitter.hh"

GaussianFitter::GaussianFitter(const TH1F& hist)
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
    _hist.Fit("gaus","0");
    _fit = (TF1*)_hist.GetFunction("gaus");
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
    _fit->SetParameter(0, mean);
}

void GaussianFitter::SetSigma(const double sigma)
{
    _fit->SetParameter(1, sigma);
}

void GaussianFitter::PrintDetails()
{
    std::cout << "\n----- Fit details -----"
              << "\nMean: " <<GetMean()
              << "\nSigma: " <<GetSigma()
              << "\nChiSquare/NDF: " <<GetChiSquarePerNDF() << std::endl;
}

