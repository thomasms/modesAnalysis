#include "TubeData.hh"

TubeData::TubeData(const ChannelData& channel0, const ChannelData& channel1, int tubeId) :
_tubeId(tubeId),
_nEntries(-1),
_nOriginalEntries(-1),
_bkg_psd_hist1(nullptr),
_bkg_psd_hist2(nullptr)
{
    _channel_rawData.first = channel0;
    _channel_rawData.second = channel1;
    
    _channel_processedData.first = channel0;
    _channel_processedData.second = channel1;
    
    //for TRandom3 this will change the seed every call to this
    _rand.SetSeed(0);
}

TubeData::~TubeData()
{
    
}

void TubeData::ProcessData(int meanNrOfEvents, int experiments, bool takeRMS)
{
    // Reset the processed data
    _channel_processedData.first = _channel_rawData.first;
    _channel_processedData.second = _channel_rawData.second;
    
    // Match the number of events per tube and perform basic cuts
    SetEntries();
    RemoveBadEvents();
    
    // Take the mean/RMS of sample events
    RunExperiments(meanNrOfEvents, experiments, takeRMS);
}

void TubeData::RunExperiments(int meanNrOfEvents, int experiments, bool takeRMS)
{
    // Keep running track of position in event array and number of events left
    int remainingEvents = _nEntries;
    int index = 0;
    int nrOfExperiments = 0;
    
    // Generate number of events from poisson distribution
    int eventsInTube = GetPoissonNumberOfEvents(meanNrOfEvents);
    
    // store all mean/RMS data values
    std::vector<float> psd0;
    std::vector<float> psd1;
    std::vector<float> QLong0;
    std::vector<float> QLong1;
    std::vector<float> QShort0;
    std::vector<float> QShort1;
    std::vector<float> Time0;
    std::vector<float> Time1;
    
    while (remainingEvents >= eventsInTube)
    {
        if((nrOfExperiments == experiments) && (experiments != -1))
            break;
        
        std::pair<float,float> psd = (takeRMS) ? GetPsdRMSOfSampleEvents(eventsInTube, index) :
                                                 GetPsdMeanOfSampleEvents(eventsInTube, index);
        psd0.push_back(psd.first);
        psd1.push_back(psd.second);
        
        std::pair<float,float> qlong = (takeRMS) ? GetQLongRMSOfSampleEvents(eventsInTube, index) :
                                                   GetQLongMeanOfSampleEvents(eventsInTube, index);
        QLong0.push_back(qlong.first);
        QLong1.push_back(qlong.second);
        
        std::pair<float,float> qshort = (takeRMS) ? GetQShortRMSOfSampleEvents(eventsInTube, index) :
                                                    GetQShortMeanOfSampleEvents(eventsInTube, index);
        QShort0.push_back(qshort.first);
        QShort1.push_back(qshort.second);
        
        std::pair<float,float> time = (takeRMS) ? GetTimeTagRMSOfSampleEvents(eventsInTube, index) :
                                                  GetTimeTagMeanOfSampleEvents(eventsInTube, index);
        Time0.push_back(time.first);
        Time1.push_back(time.second);
        
        // running totals
        index +=eventsInTube;
        remainingEvents -=eventsInTube;
        if(eventsInTube>0)nrOfExperiments++;
        
        // Generate new number of events
        eventsInTube = GetPoissonNumberOfEvents(meanNrOfEvents);
    }
    
    _channel_processedData.first.SetEntries(QLong0.size());
    _channel_processedData.first.SetPSDValues(psd0);
    _channel_processedData.first.SetQlongValues(QLong0);
    _channel_processedData.first.SetQshortValues(QShort0);
    _channel_processedData.first.SetTimeTagValues(Time0);
    
    _channel_processedData.second.SetEntries(QLong1.size());
    _channel_processedData.second.SetPSDValues(psd1);
    _channel_processedData.second.SetQlongValues(QLong1);
    _channel_processedData.second.SetQshortValues(QShort1);
    _channel_processedData.second.SetTimeTagValues(Time1);
    
    _nEntries = _channel_processedData.first.GetEntries();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

const std::pair<float,float> TubeData::GetQLongRMSOfSampleEvents(int nrOfEvents, int startIndex)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    std::vector<float> sampleQlong0(nrOfEvents,0.0);
    std::vector<float> sampleQlong1(nrOfEvents,0.0);
    for(int i=0;i<nrOfEvents;++i)
    {
        sampleQlong0[i] =_channel_processedData.first.GetQlong(startIndex +i);
        sampleQlong1[i] =_channel_processedData.second.GetQlong(startIndex +i);
        
    }
    
    results.first  = Utils::GetRMS<float>(sampleQlong0);
    results.second = Utils::GetRMS<float>(sampleQlong1);
    
    return results;
}

const std::pair<float,float> TubeData::GetQLongMeanOfSampleEvents(int nrOfEvents, int startIndex)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    std::vector<float> sampleQlong0(nrOfEvents,0.0);
    std::vector<float> sampleQlong1(nrOfEvents,0.0);
    for(int i=0;i<nrOfEvents;++i)
    {
        sampleQlong0[i] =_channel_processedData.first.GetQlong(startIndex +i);
        sampleQlong1[i] =_channel_processedData.second.GetQlong(startIndex +i);
        
    }
    
    results.first  = Utils::GetMean<float>(sampleQlong0);
    results.second = Utils::GetMean<float>(sampleQlong1);
    
    return results;
}

const std::pair<float,float> TubeData::GetQShortRMSOfSampleEvents(int nrOfEvents, int startIndex)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    std::vector<float> sampleQshort0(nrOfEvents,0.0);
    std::vector<float> sampleQshort1(nrOfEvents,0.0);
    for(int i=0;i<nrOfEvents;++i)
    {
        sampleQshort0[i] =_channel_processedData.first.GetQshort(startIndex +i);
        sampleQshort1[i] =_channel_processedData.second.GetQshort(startIndex +i);
    }
    
    results.first  = Utils::GetRMS<float>(sampleQshort0);
    results.second = Utils::GetRMS<float>(sampleQshort1);
    
    return results;
}

const std::pair<float,float> TubeData::GetQShortMeanOfSampleEvents(int nrOfEvents, int startIndex)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    std::vector<float> sampleQshort0(nrOfEvents,0.0);
    std::vector<float> sampleQshort1(nrOfEvents,0.0);
    for(int i=0;i<nrOfEvents;++i)
    {
        sampleQshort0[i] =_channel_processedData.first.GetQshort(startIndex +i);
        sampleQshort1[i] =_channel_processedData.second.GetQshort(startIndex +i);
    }
    
    results.first  = Utils::GetMean<float>(sampleQshort0);
    results.second = Utils::GetMean<float>(sampleQshort1);
    
    return results;
}

const std::pair<float,float> TubeData::GetTimeTagRMSOfSampleEvents(int nrOfEvents, int startIndex)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    std::vector<float> sampleTime0(nrOfEvents,0.0);
    std::vector<float> sampleTime1(nrOfEvents,0.0);
    for(int i=0;i<nrOfEvents;++i)
    {
        sampleTime0[i] =_channel_processedData.first.GetTimeTag(startIndex +i);
        sampleTime1[i] =_channel_processedData.second.GetTimeTag(startIndex +i);
    }
    
    results.first  = Utils::GetRMS<float>(sampleTime0);
    results.second = Utils::GetRMS<float>(sampleTime1);
    
    return results;
}

const std::pair<float,float> TubeData::GetTimeTagMeanOfSampleEvents(int nrOfEvents, int startIndex)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    std::vector<float> sampleTime0(nrOfEvents,0.0);
    std::vector<float> sampleTime1(nrOfEvents,0.0);
    for(int i=0;i<nrOfEvents;++i)
    {
        sampleTime0[i] =_channel_processedData.first.GetTimeTag(startIndex +i);
        sampleTime1[i] =_channel_processedData.second.GetTimeTag(startIndex +i);
    }
    
    results.first  = Utils::GetMean<float>(sampleTime0);
    results.second = Utils::GetMean<float>(sampleTime1);
    
    return results;
}

const std::pair<float,float> TubeData::GetPsdMeanOfSampleEvents(int nrOfEvents, int startIndex)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    std::vector<float> samplePsd0(nrOfEvents,0.0);
    std::vector<float> samplePsd1(nrOfEvents,0.0);
    for(int i=0;i<nrOfEvents;++i)
    {
        samplePsd0[i] =_channel_processedData.first.GetPsd(startIndex +i);
        samplePsd1[i] =_channel_processedData.second.GetPsd(startIndex +i);
    }
    
    results.first  = Utils::GetMean<float>(samplePsd0);
    results.second = Utils::GetMean<float>(samplePsd1);
    
    return results;
}

const std::pair<float,float> TubeData::GetPsdRMSOfSampleEvents(int nrOfEvents, int startIndex)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    std::vector<float> samplePsd0(nrOfEvents,0.0);
    std::vector<float> samplePsd1(nrOfEvents,0.0);
    for(int i=0;i<nrOfEvents;++i)
    {
        samplePsd0[i] =_channel_processedData.first.GetPsd(startIndex +i);
        samplePsd1[i] =_channel_processedData.second.GetPsd(startIndex +i);
    }
    
    results.first  = Utils::GetRMS<float>(samplePsd0);
    results.second = Utils::GetRMS<float>(samplePsd1);
    
    return results;
}

const int TubeData::GetPoissonNumberOfEvents(int meanNrOfEvents)
{
    int result = _rand.Poisson(meanNrOfEvents);
    return result;
}

void TubeData::SetChannelBackgroundPsdHists(TH1F* psd1, TH1F* psd2)
{
    _bkg_psd_hist1 = psd1;
    _bkg_psd_hist2 = psd2;
}

void TubeData::SetEntries()
{
    // Ensure the number of entries per tube are the same
    if(_channel_processedData.second.GetEntries() > _channel_processedData.first.GetEntries())
        _channel_processedData.second.SetEntries(_channel_processedData.first.GetEntries());
    else
        _channel_processedData.first.SetEntries(_channel_processedData.second.GetEntries());
}

void TubeData::RemoveBadEvents()
{
    std::vector<float> psd0, psd1;
    std::vector<float> qlong0, qlong1;
    std::vector<float> qshort0, qshort1;
    std::vector<float> timetag0, timetag1;
    
    _nEntries = _channel_processedData.first.GetEntries();
    
    for(int e=0;e<_nEntries;++e)
    {
        // Skip bad events for whole detector not per channel
        // Remove from both channels
        if((_channel_processedData.first.IsBadEvent(e)) || (_channel_processedData.second.IsBadEvent(e)))
            continue;
        
        //copy values to new vector - cannot erase events as this causes crash
        // just as slow as erasing index - use better container other than vector
        psd0.push_back(_channel_processedData.first.GetPsd(e));
        qlong0.push_back(_channel_processedData.first.GetQlong(e));
        qshort0.push_back(_channel_processedData.first.GetQshort(e));
        timetag0.push_back(_channel_processedData.first.GetTimeTag(e));
        
        psd1.push_back(_channel_processedData.second.GetPsd(e));
        qlong1.push_back(_channel_processedData.second.GetQlong(e));
        qshort1.push_back(_channel_processedData.second.GetQshort(e));
        timetag1.push_back(_channel_processedData.second.GetTimeTag(e));
    }
    
    _channel_processedData.first.SetEntries(qlong0.size());
    _channel_processedData.first.SetPSDValues(psd0);
    _channel_processedData.first.SetQlongValues(qlong0);
    _channel_processedData.first.SetQshortValues(qshort0);
    _channel_processedData.first.SetTimeTagValues(timetag0);
    
    _channel_processedData.second.SetEntries(qlong1.size());
    _channel_processedData.second.SetPSDValues(psd1);
    _channel_processedData.second.SetQlongValues(qlong1);
    _channel_processedData.second.SetQshortValues(qshort1);
    _channel_processedData.second.SetTimeTagValues(timetag1);
    
    _nEntries = _channel_processedData.first.GetEntries();
    _nOriginalEntries = _nEntries;
}