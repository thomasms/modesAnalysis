#ifndef GAUSSIAN_FITTER_HH
#define GAUSSIAN_FITTER_HH

#include "TH1.h"
#include "TMath.h"
#include "TF1.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"

class GaussianFitter
{
public:
    /*
     Histogram must not be scaled!
     */
    GaussianFitter(TH1F* hist);
    ~GaussianFitter();
    
    TF1* Fit();
    void FitInRange(double mean, double minX, double maxX);
    
    TF1* GetFitFunction();
    
    const double GetChiSquare() const;
    const double GetChiSquarePerNDF() const;
    const double GetMean() const;
    const double GetSigma() const;
    
    void SetHistogram(TH1F* hist);
    void SetMean(const double mean);
    void SetSigma(const double sigma);
    void SetMinNDFInFit(const int ndf);
    
    void PrintDetails();
    
private:
    static double PeakFunction(double *x, double *par);
    void FitPeak();
    void ResetParameterLimits();
    
private:
    TF1* _fit;
    TH1F* _hist;
    
    int _minNDF;
    
};

#endif //GAUSSIAN_FITTER_HH
