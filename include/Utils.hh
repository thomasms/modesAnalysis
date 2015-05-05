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
    
    template<typename T>
    static T GetMean(const std::vector<T>& values)
    {
        T mean =0;
        if(values.size() == 0) return mean;
        
        for(int i=0;i<values.size();++i)
            mean +=values[i];
        
        mean = mean/static_cast<T>(values.size());
        
        return mean;
    }
    
    template<typename T>
    static T GetRMS(const std::vector<T>& values)
    {
        T rms =0;
        if(values.size() == 0) return rms;
        
        T squareSum = 0;
        for(int i=0;i<values.size();++i)
            squareSum += values[i]*values[i];
        
        rms = TMath::Sqrt(squareSum/static_cast<T>(values.size()));
        
        return rms;
    }
    
    template<typename T>
    static T GetVariance(const std::vector<T>& values)
    {
        T var =0;
        if(values.size() <=1) return var;
        
        T mean = GetMean<T>(values);
        for(int i=0;i<values.size();++i)
            var += TMath::Power(mean - values[i], 2);
        
        var = var/static_cast<T>(values.size() - 1);
        
        return var;
    }
    
    template<typename T>
    static T GetSigma(const std::vector<T>& values)
    {
        T sigma = TMath::Sqrt(GetVariance<T>(values));
        return sigma;
    }
    
private:
    
    static void ScaleAxis(TAxis *a, double scale, double shift);
    static double ScaleValue(double x, double scale, double shift);
};

#endif
