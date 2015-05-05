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
