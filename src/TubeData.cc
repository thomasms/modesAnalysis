#include "TubeData.hh"

TubeData::TubeData(const ChannelData& channel0, const ChannelData& channel1, int tubeId) : _tubeId(tubeId)
{
    _channel0 = channel0;
    _channel1 = channel1;
}

TubeData::~TubeData()
{
    
}

void TubeData::ProcessData()
{    
    // Ensure the number of entries per tube are the same
    if(_channel1.GetEntries() > _channel0.GetEntries())
        _channel1.SetEntries(_channel0.GetEntries());
    else
        _channel0.SetEntries(_channel1.GetEntries());
    
    _nEntries = _channel0.GetEntries();
    
    std::vector<float> qlong0, qlong1;
    std::vector<float> qshort0, qshort1;
    std::vector<float> timetag0, timetag1;
    
    for(int e=0;e<_nEntries;++e)
    {
        // Skip bad events for whole detector not per channel
        // Remove from both channels
        if((_channel0.IsBadEvent(e)) || (_channel1.IsBadEvent(e)))
            continue;
        
        //copy values to new vector - cannot erase events as this causes crash
        qlong0.push_back(_channel0.GetQlong(e));
        qshort0.push_back(_channel0.GetQshort(e));
        timetag0.push_back(_channel0.GetTimeTag(e));
        
        qlong1.push_back(_channel1.GetQlong(e));
        qshort1.push_back(_channel1.GetQshort(e));
        timetag1.push_back(_channel1.GetTimeTag(e));
    }
    
    _channel0.SetEntries(qlong0.size());
    _channel0.SetQlongValues(qlong0);
    _channel0.SetQshortValues(qshort0);
    _channel0.SetTimeTagValues(timetag0);
    
    _channel1.SetEntries(qlong1.size());
    _channel1.SetQlongValues(qlong1);
    _channel1.SetQshortValues(qshort1);
    _channel1.SetTimeTagValues(timetag1);
    
    std::cout << "\nEntries0: " <<_channel0.GetEntries();
    std::cout << "\nEntries1: " <<_channel1.GetEntries();
}
