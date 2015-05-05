#ifndef UTILS_HH
#define UTILS_HH

#include "TAxis.h"
#include "TH1.h"
#include "TArrayD.h"
#include "TMath.h"

class Utils
{

public:

    static void ScaleXaxis(TH1 *h, double scale, double shift);
    static void ScaleYaxis(TH1 *h, double scale, double shift);
    static void ScaleZaxis(TH1 *h, double scale, double shift);
    
    static double GetMean(const std::vector<double>& values);
    static double GetRMS(const std::vector<double>& values);
    static double GetVariance(const std::vector<double>& values);
    static double GetSigma(const std::vector<double>& values);
    
private:
    
    static void ScaleAxis(TAxis *a, double scale, double shift);
    static double ScaleValue(double x, double scale, double shift);
};

#endif
