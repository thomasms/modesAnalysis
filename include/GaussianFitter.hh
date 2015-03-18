#ifndef GAUSSIAN_FITTER_HH
#define GAUSSIAN_FITTER_HH

#include "TH1.h"
#include "TF1.h"

class GaussianFitter
{
public:
    GaussianFitter(const TH1F& hist);
    ~GaussianFitter();
    
    void Fit();
    
    TF1* GetFitFunction();
    
    const double GetChiSquare() const;
    const double GetChiSquarePerNDF() const;
    const double GetMean() const;
    const double GetSigma() const;
    
    void SetMean(const double mean);
    void SetSigma(const double sigma);
    
    void PrintDetails();
    
private:
    TF1* _fit;
    TH1F _hist;
    
};

#endif //GAUSSIAN_FITTER_HH
