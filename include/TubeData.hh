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
    
    void ProcessData(int meanNrOfEvents, int experiments, double contaminationLevel, bool takeRMS=false);
    
    void SetChannelBackgroundPsdHists(TH1F* psd1, TH1F* psd2);
    
    inline const int GetId()  const  {return _tubeId;};
    inline const int GetEntries()  const  {return _nEntries;};
    inline const int GetOriginalEntries()  const  {return _nOriginalEntries;};
    
    const ChannelData GetChannelRawDataFirst() {return _channel_rawData.first;};
    const ChannelData GetChannelRawDataSecond() {return _channel_rawData.second;};
    
    const ChannelData GetChannelProcessedDataFirst() {return _channel_processedData.first;};
    const ChannelData GetChannelProcessedDataSecond() {return _channel_processedData.second;};

private:
    
    const std::vector<int> GetEventIndices(int nrOfEvents);
    const std::pair< std::vector<float>, std::vector<float> > GetContaminatedPsdSample(const std::vector<int>& indices, double level);
    void SetEntries();
    void RemoveBadEvents();
    void RunExperiments(int meanNrOfEvents, int experiments, double contaminationLevel, bool takeRMS);
    const int GetPoissonNumberOfEvents(int meanNrOfEvents);
    const std::pair<float,float> GetQLongMeanOfSampleEvents(const std::vector<int>& indices);
    const std::pair<float,float> GetQLongRMSOfSampleEvents(const std::vector<int>& indices);
    const std::pair<float,float> GetQShortMeanOfSampleEvents(const std::vector<int>& indices);
    const std::pair<float,float> GetQShortRMSOfSampleEvents(const std::vector<int>& indices);
    const std::pair<float,float> GetTimeTagMeanOfSampleEvents(const std::vector<int>& indices);
    const std::pair<float,float> GetTimeTagRMSOfSampleEvents(const std::vector<int>& indices);
    const std::pair<float,float> GetPsdMeanOfSampleEvents(const std::pair< std::vector<float>, std::vector<float> >& psdValues);
    const std::pair<float,float> GetPsdRMSOfSampleEvents(const std::pair< std::vector<float>, std::vector<float> >& psdValues);

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
