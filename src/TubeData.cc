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

void TubeData::ProcessData(int meanNrOfEvents, int experiments, double contaminationLevel, bool takeRMS)
{
    // Reset the processed data
    _channel_processedData.first = _channel_rawData.first;
    _channel_processedData.second = _channel_rawData.second;
    
    // Match the number of events per tube and perform basic cuts
    SetEntries();
    RemoveBadEvents();
    
    // Take the mean/RMS of sample events
    RunExperiments(meanNrOfEvents, experiments, contaminationLevel, takeRMS);
}

void TubeData::RunExperiments(int meanNrOfEvents, int experiments, double contaminationLevel, bool takeRMS)
{
    
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
    
    int nrOfExperiments = 0;
    while (nrOfExperiments < experiments)
    {
        auto eventIndices = GetEventIndices(eventsInTube);
        
        // add contamination
        auto psdValues = GetContaminatedPsdSample(eventIndices, contaminationLevel);
        
        std::pair<float,float> psd = (takeRMS) ? GetPsdRMSOfSampleEvents(psdValues) :
                                                 GetPsdMeanOfSampleEvents(psdValues);
        psd0.push_back(psd.first);
        psd1.push_back(psd.second);
        
        std::pair<float,float> qlong = (takeRMS) ? GetQLongRMSOfSampleEvents(eventIndices) :
                                                   GetQLongMeanOfSampleEvents(eventIndices);
        QLong0.push_back(qlong.first);
        QLong1.push_back(qlong.second);
        
        std::pair<float,float> qshort = (takeRMS) ? GetQShortRMSOfSampleEvents(eventIndices) :
                                                    GetQShortMeanOfSampleEvents(eventIndices);
        QShort0.push_back(qshort.first);
        QShort1.push_back(qshort.second);
        
        std::pair<float,float> time = (takeRMS) ? GetTimeTagRMSOfSampleEvents(eventIndices) :
                                                  GetTimeTagMeanOfSampleEvents(eventIndices);
        Time0.push_back(time.first);
        Time1.push_back(time.second);
        
        // running totals
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

const std::pair<float,float> TubeData::GetQLongRMSOfSampleEvents(const std::vector<int>& indices)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    int size = static_cast<int>(indices.size());
    std::vector<float> sampleQlong0(size,0.0);
    std::vector<float> sampleQlong1(size,0.0);
    for(int i=0;i<size;++i)
    {
        sampleQlong0[i] =_channel_processedData.first.GetQlong(indices[i]);
        sampleQlong1[i] =_channel_processedData.second.GetQlong(indices[i]);
    }
    
    results.first  = Utils::GetRMS<float>(sampleQlong0);
    results.second = Utils::GetRMS<float>(sampleQlong1);
    
    return results;
}

const std::pair<float,float> TubeData::GetQLongMeanOfSampleEvents(const std::vector<int>& indices)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    int size = static_cast<int>(indices.size());
    std::vector<float> sampleQlong0(size,0.0);
    std::vector<float> sampleQlong1(size,0.0);
    for(int i=0;i<size;++i)
    {
        sampleQlong0[i] =_channel_processedData.first.GetQlong(indices[i]);
        sampleQlong1[i] =_channel_processedData.second.GetQlong(indices[i]);
    }
    
    results.first  = Utils::GetMean<float>(sampleQlong0);
    results.second = Utils::GetMean<float>(sampleQlong1);
    
    return results;
}

const std::pair<float,float> TubeData::GetQShortRMSOfSampleEvents(const std::vector<int>& indices)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    int size = static_cast<int>(indices.size());
    std::vector<float> sampleQshort0(size,0.0);
    std::vector<float> sampleQshort1(size,0.0);
    for(int i=0;i<size;++i)
    {
        sampleQshort0[i] =_channel_processedData.first.GetQshort(indices[i]);
        sampleQshort1[i] =_channel_processedData.second.GetQshort(indices[i]);
    }
    
    results.first  = Utils::GetRMS<float>(sampleQshort0);
    results.second = Utils::GetRMS<float>(sampleQshort1);
    
    return results;
}

const std::pair<float,float> TubeData::GetQShortMeanOfSampleEvents(const std::vector<int>& indices)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    int size = static_cast<int>(indices.size());
    std::vector<float> sampleQshort0(size,0.0);
    std::vector<float> sampleQshort1(size,0.0);
    for(int i=0;i<size;++i)
    {
        sampleQshort0[i] =_channel_processedData.first.GetQshort(indices[i]);
        sampleQshort1[i] =_channel_processedData.second.GetQshort(indices[i]);
    }
    
    results.first  = Utils::GetMean<float>(sampleQshort0);
    results.second = Utils::GetMean<float>(sampleQshort1);
    
    return results;
}

const std::pair<float,float> TubeData::GetTimeTagRMSOfSampleEvents(const std::vector<int>& indices)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events from start index
    int size = static_cast<int>(indices.size());
    std::vector<float> sampleTime0(size,0.0);
    std::vector<float> sampleTime1(size,0.0);
    for(int i=0;i<size;++i)
    {
        sampleTime0[i] =_channel_processedData.first.GetTimeTag(indices[i]);
        sampleTime1[i] =_channel_processedData.second.GetTimeTag(indices[i]);
    }
    
    results.first  = Utils::GetRMS<float>(sampleTime0);
    results.second = Utils::GetRMS<float>(sampleTime1);
    
    return results;
}

const std::pair<float,float> TubeData::GetTimeTagMeanOfSampleEvents(const std::vector<int>& indices)
{
    std::pair<float,float> results(0,0);
    
    // Take number of events
    int size = static_cast<int>(indices.size());
    std::vector<float> sampleTime0(size,0.0);
    std::vector<float> sampleTime1(size,0.0);
    for(int i=0;i<size;++i)
    {
        sampleTime0[i] =_channel_processedData.first.GetTimeTag(indices[i]);
        sampleTime1[i] =_channel_processedData.second.GetTimeTag(indices[i]);
    }
    
    results.first  = Utils::GetMean<float>(sampleTime0);
    results.second = Utils::GetMean<float>(sampleTime1);
    
    return results;
}

const std::pair<float,float> TubeData::GetPsdMeanOfSampleEvents(const std::pair <std::vector<float>, std::vector<float> >& psdSample)
{
    std::pair<float,float> results(0,0);
    
    results.first  = Utils::GetMean<float>(psdSample.first);
    results.second = Utils::GetMean<float>(psdSample.second);
    
    return results;
}

const std::pair<float,float> TubeData::GetPsdRMSOfSampleEvents(const std::pair <std::vector<float>, std::vector<float> >& psdSample)
{
    std::pair<float,float> results(0,0);
    
    results.first  = Utils::GetRMS<float>(psdSample.first);
    results.second = Utils::GetRMS<float>(psdSample.second);
    
    return results;
}

const std::vector<int> TubeData::GetEventIndices(int nrOfEvents)
{
    int index = -1;
    std::vector<int> indices(nrOfEvents,0);
    
    for(int i=0;i<nrOfEvents;++i)
        indices[i] = _rand.Rndm()*(_nEntries -1);
    
    return indices;
}

const std::pair< std::vector<float>, std::vector<float> > TubeData::GetContaminatedPsdSample(const std::vector<int>& indices,
                                                                                             double level)
{
    std::pair< std::vector<float>, std::vector<float> > psdValues;
    int nrOfEvents = static_cast<int>(indices.size());
    
    // take random number of events - the same for both channels in tube
    // must be equal to or lower than total number of events
    int nrOfBkgEvents = nrOfEvents+1;
    while(nrOfBkgEvents > nrOfEvents)
    {
        nrOfBkgEvents = _rand.Poisson(nrOfEvents*level);
    }
    
    // Get signal events
    int nrOfSigEvents = nrOfEvents - nrOfBkgEvents;
    std::vector<float> psdCh1(nrOfEvents,0.0);
    std::vector<float> psdCh2(nrOfEvents,0.0);
    for(int i=0;i<nrOfEvents;++i)
    {
        psdCh1[i] =_channel_processedData.first.GetPsd(indices[i]);
        psdCh2[i] =_channel_processedData.second.GetPsd(indices[i]);
    }
    
    psdValues.first = psdCh1;
    psdValues.second = psdCh2;
    
    if(level == 0.0)return psdValues;
    
    if(_bkg_psd_hist1 == nullptr ||
       _bkg_psd_hist2 == nullptr)
        return psdValues;
    
    // Add background events
    for(int i=0;i<nrOfBkgEvents;++i)
    {
        psdCh1[i] = _bkg_psd_hist1->GetRandom();
        psdCh2[i] = _bkg_psd_hist2->GetRandom();
    }
    
    psdValues.first = psdCh1;
    psdValues.second = psdCh2;
    
    return psdValues;
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