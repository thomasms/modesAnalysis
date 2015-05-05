#include "ChannelData.hh"

ChannelData::ChannelData() : _channelId(-1), _nEntries(0)
{
}

ChannelData::ChannelData(int channel) : _channelId(channel), _nEntries(0)
{
}

ChannelData::~ChannelData()
{
}

void ChannelData::ReadData(TBranch* branch_data, float timeCutOffInSecs)
{
    ClearAllData();
    
    if( (branch_data->GetEntries() <=0) )
        return;
    
    // Set the number of entries
    _nEntries = branch_data->GetEntries();
    
    // Read the data from the branch
    branch_data->SetAddress(&_data);
    
    float time = 0.;
    float timeInSecs = 0.;
    float Qlong = 0.;
    float Qshort = 0.;
    float psd = 0.;
    
    //loop over entries on each channel
    for (int e=0; e<_nEntries; ++e)
    {
        // Get the event
        branch_data->GetEntry(e);
        
        // Read the values
        time = static_cast<float>(_data.timetag);
        timeInSecs = time*1e-9;
        Qlong = _data.qlong;
        Qshort = _data.qshort;
        psd = (Qlong - Qshort)/Qlong;
        
        // set them in vectors
        _qlongValues.push_back(Qlong);
        _qshortValues.push_back(Qshort);
        _timetagValues.push_back(timeInSecs);
        
        // flag bad events
        if((timeInSecs>timeCutOffInSecs || (timeInSecs <0) || (Qlong <= 0) || (Qshort <= 0) || (psd <= 0)) &&
           (timeCutOffInSecs != -1) )
            _badEventFlags.push_back(1);
        else
            _badEventFlags.push_back(0);
    }
}

void ChannelData::ClearAllData()
{
    _nEntries = 0;
    _qlongValues.resize(_nEntries);
    _qshortValues.resize(_nEntries);
    _timetagValues.resize(_nEntries);
    _badEventFlags.resize(_nEntries);
}

void ChannelData::SetId(int id)
{
    _channelId = id;
}

void ChannelData::SetEntries(int entries)
{
    if(entries > _nEntries)
        return;
    
    _nEntries = entries;
    _qlongValues.resize(entries);
    _qshortValues.resize(entries);
    _timetagValues.resize(entries);
    _badEventFlags.resize(entries);
}

const bool ChannelData::IsBadEvent(int index) const
{
    bool result = true;
    if(index <0 || index >= _nEntries)
        return result;
    
    result = (_badEventFlags[index]) ? true :false;
    return result;
}

const float ChannelData::GetPsd(int index) const
{
    float result = 0;
    if(index <0 || index >= _qlongValues.size())
        return result;
    
    result = (_qlongValues[index] - _qshortValues[index]) / _qlongValues[index];
    return result;
}

const float ChannelData::GetQlong(int index) const
{
    float result = 0;
    if(index <0 || index >= _qlongValues.size())
        return result;
    
    result = _qlongValues[index];
    return result;
}

const float ChannelData::GetQshort(int index) const
{
    float result = 0;
    if(index <0 || index >= _qshortValues.size())
        return result;
    
    result = _qshortValues[index];
    return result;
}

const float ChannelData::GetTimeTag(int index) const
{
    float result = 0;
    if(index <0 || index >= _timetagValues.size())
        return result;
    
    result = _timetagValues[index];
    return result;
    
}