#ifndef TUBEDATA_HH
#define TUBEDATA_HH

#include <vector>
#include <memory>
#include <iomanip>
#include <utility>

#include "TRandom3.h"

#include "ChannelData.hh"
#include "Utils.hh"

class TubeData
{
    
public:
    TubeData(const ChannelData& channel0, const ChannelData& channel1, int id);
    ~TubeData();
    
    void ProcessData(int meanNrOfEvents, int experiments, bool takeRMS=false);
    
    void SetChannelBackgroundPsdHists(TH1F* psd1, TH1F* psd2);
    
    inline const int GetId()  const  {return _tubeId;};
    inline const int GetEntries()  const  {return _nEntries;};
    inline const int GetOriginalEntries()  const  {return _nOriginalEntries;};
    
    const ChannelData GetChannelRawDataFirst() {return _channel_rawData.first;};
    const ChannelData GetChannelRawDataSecond() {return _channel_rawData.second;};
    
    const ChannelData GetChannelProcessedDataFirst() {return _channel_processedData.first;};
    const ChannelData GetChannelProcessedDataSecond() {return _channel_processedData.second;};

private:
    
    void ContaminateTube(int meanNrOfEvents, double level);
    void SetEntries();
    void RemoveBadEvents();
    void RunExperiments(int meanNrOfEvents, int experiments, bool takeRMS);
    const int GetPoissonNumberOfEvents(int meanNrOfEvents);
    const std::pair<float,float> GetQLongMeanOfSampleEvents(int nrOfEvents, int startIndex);
    const std::pair<float,float> GetQLongRMSOfSampleEvents(int nrOfEvents, int startIndex);
    const std::pair<float,float> GetQShortMeanOfSampleEvents(int nrOfEvents, int startIndex);
    const std::pair<float,float> GetQShortRMSOfSampleEvents(int nrOfEvents, int startIndex);
    const std::pair<float,float> GetTimeTagMeanOfSampleEvents(int nrOfEvents, int startIndex);
    const std::pair<float,float> GetTimeTagRMSOfSampleEvents(int nrOfEvents, int startIndex);
    const std::pair<float,float> GetPsdMeanOfSampleEvents(int nrOfEvents, int startIndex);
    const std::pair<float,float> GetPsdRMSOfSampleEvents(int nrOfEvents, int startIndex);

private:
    
    int _tubeId;
    int _nEntries;
    int _nOriginalEntries;
    
    TH1F* _bkg_psd_hist1;
    TH1F* _bkg_psd_hist2;
    
    TRandom3 _rand;
    
    std::pair<ChannelData,ChannelData> _channel_rawData;
    std::pair<ChannelData,ChannelData> _channel_processedData;
    
};


#endif
