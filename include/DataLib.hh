#ifndef DATALIB_HH
#define DATALIB_HH

#include "TH1.h"
#include "TH2.h"
#include "TString.h"

using namespace std;

class EventParameters{

public:
  EventParameters(){};
  ~EventParameters(){};
  
  UInt_t          timeStamp;
  UShort_t        dppVersion;
  UShort_t        nsPerSample;
  UShort_t        nsPerTimeTag;
  UShort_t        numChannels;
  UShort_t        numSamples[32];
  
};

class EventData{
  
public:
  EventData(): timetag(0),baseline(0),qshort(0),qlong(0),pur(0){};
  ~EventData(){};
  
  //variables to output
  ULong64_t       timetag;        //time of the trigger, trigger defined for first sample above threshold (1 unit = 2ns)
  UInt_t          baseline;       //offset for zero level signals
  UShort_t        qshort;         //fast component integral
  UShort_t        qlong;          //total integral
  UShort_t        pur;            //flag: 1 = pile up, 0 = normal
  UShort_t        samples[4096];  //array of samples
  
};

class Source
{

public:
  Source() {};
  Source(TString sigName, TString backName) {signalName = sigName; backgroundName = backName; };
  ~Source() {};
  
  TString GetSignalName()	        {return signalName;};
  TString GetBackgroundName()	    {return backgroundName;};
  
  double GetTemperature()	        {return temperature;};
  double GetPressure()		        {return pressure;};
  
  void SetTemperature(double temp)	{temperature = temp;};
  void SetPressure(double press)	{pressure = press;};
  
  //get histogram vector - one entry for each channel
  std::vector<TH1F> GetSignalSpectraHistVtr()      { return _h1Vtr_signalChannelSpectra;};
  std::vector<TH1F> GetSignalQShortHistVtr()       { return _h1Vtr_signalChannelQShort;};
  std::vector<TH2F> GetSignalSpectra2DHistVtr()    { return _h2Vtr_signalChannelSpectra;};
  std::vector<TH1F> GetSMinusBSpectraHistVtr()     { return _h1Vtr_sMinusbChannelSpectra;};
  std::vector<TH1F> GetBackgroundSpectraHistVtr()  { return _h1Vtr_backgroundChannelSpectra;};
  std::vector<TH1F> GetBackgroundQShortHistVtr()   { return _h1Vtr_backgroundChannelQShort;};
  std::vector<TH1F> GetSignalPsdHistVtr()          { return _h1Vtr_signalChannelPsd;};
  std::vector<TH1F> GetBackgroundPsdHistVtr()      { return _h1Vtr_backgroundChannelPsd;};

    //add histograms to vector - add a channel
  void AddSpectraToSignalHistVtr(const TH1F& hist)     {_h1Vtr_signalChannelSpectra.push_back(hist);};
  void AddQShortToSignalHistVtr(const TH1F& hist)      {_h1Vtr_signalChannelQShort.push_back(hist);};
  void AddSpectraToSignal2DHistVtr(const TH2F& hist)   {_h2Vtr_signalChannelSpectra.push_back(hist);};
  void AddSpectraToSMinusBHistVtr(const TH1F& hist)    {_h1Vtr_sMinusbChannelSpectra.push_back(hist);};
  void AddSpectraToBackgroundHistVtr(const TH1F& hist) {_h1Vtr_backgroundChannelSpectra.push_back(hist);};
  void AddQShortToBackgroundHistVtr(const TH1F& hist)  {_h1Vtr_backgroundChannelQShort.push_back(hist);};

  void AddPsdToSignalHistVtr(const TH1F& hist)         {_h1Vtr_signalChannelPsd.push_back(hist);};
  void AddPsdToBackgroundHistVtr(const TH1F& hist)     {_h1Vtr_backgroundChannelPsd.push_back(hist);};

private:
  TString signalName, backgroundName;
  double activity,aquistionTimeInMins, temperature, pressure;
  double distance;
  double totalCounts,signalCounts,backgroundCounts;
    
  std::vector<TH1F> _h1Vtr_signalChannelSpectra;
  std::vector<TH1F> _h1Vtr_signalChannelQShort;
  std::vector<TH2F> _h2Vtr_signalChannelSpectra;
  std::vector<TH1F> _h1Vtr_sMinusbChannelSpectra;
  std::vector<TH1F> _h1Vtr_backgroundChannelSpectra;
  std::vector<TH1F> _h1Vtr_backgroundChannelQShort;
  
  std::vector<TH1F> _h1Vtr_signalChannelPsd;
  std::vector<TH1F> _h1Vtr_backgroundChannelPsd;

};

#endif
