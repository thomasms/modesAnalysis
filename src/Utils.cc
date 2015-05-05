#include "Utils.hh"

double Utils::ScaleValue(double x, double scale, double shift)
{
  double v;
  v = scale * x + shift; // "linear scaling" function example
  return v;
}

void Utils::ScaleAxis(TAxis *a, double scale, double shift)
{
  if (!a) return; // just a precaution
    
  // an axis with fixed bins only!    
  a->Set( a->GetNbins(),
          ScaleValue(a->GetXmin(), scale, shift), // new Xmin
          ScaleValue(a->GetXmax(), scale, shift) ); // new Xmax
  
}

void Utils::ScaleXaxis(TH1 *h, double scale, double shift)
{
    if (!h) return; // just a precaution
    ScaleAxis(h->GetXaxis(), scale, shift);
    //h->ResetStats();
    h->GetXaxis()->SetRangeUser(0, h->GetXaxis()->GetXmax());
}

void Utils::ScaleYaxis(TH1 *h, double scale, double shift)
{
    if (!h) return; // just a precaution
    ScaleAxis(h->GetYaxis(), scale, shift);
    h->ResetStats();
    h->GetYaxis()->SetRangeUser(0, h->GetYaxis()->GetXmax());
}

void Utils::ScaleZaxis(TH1 *h, double scale, double shift)
{
    if (!h) return; // just a precaution
    ScaleAxis(h->GetZaxis(), scale, shift);
    h->ResetStats();
    h->GetZaxis()->SetRangeUser(0, h->GetZaxis()->GetXmax());
}

double Utils::GetMean(const std::vector<double>& values)
{
    double mean =0;
    if(values.size() == 0) return mean;
    
    for(int i=0;i<values.size();++i)
        mean +=values[i];
    
    mean = mean/static_cast<double>(values.size());
    
    return mean;
}

double Utils::GetRMS(const std::vector<double>& values)
{
    double rms =0;
    if(values.size() == 0) return rms;
    
    double squareSum = 0;
    for(int i=0;i<values.size();++i)
        squareSum += values[i]*values[i];
    
    rms = squareSum/static_cast<double>(values.size());
    
    return rms;
}

double Utils::GetVariance(const std::vector<double>& values)
{
    double var =0;
    if(values.size() <=1) return var;
    
    double mean = GetMean(values);
    for(int i=0;i<values.size();++i)
        var += TMath::Power(mean - values[i], 2);
    
    var = var/static_cast<double>(values.size() - 1);
    
    return var;
}

double Utils::GetSigma(const std::vector<double>& values)
{
    double sigma = TMath::Sqrt(GetVariance(values));
    
    return sigma;
}