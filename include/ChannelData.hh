#ifndef CHANNELDATA_HH
#define CHANNELDATA_HH

#include <vector>
#include <iostream>

#include "TRandom3.h"
#include "TTree.h"
#include "TBranch.h"

#include "Utils.hh"
#include "dataLib.hh"

class ChannelData
{
    
public:
    ChannelData();
    ChannelData(int channel);
    ~ChannelData();
    
    void ReadData(TBranch* branchPtr, float timeCutOffInSecs);
    void ClearAllData();
    
    inline const int GetId()  const  {return _channelId;};
    inline const int GetEntries()  const  {return _nEntries;};
    
    inline void SetId(int id) { _channelId = id;};
    
    const bool IsBadEvent(int index) const;
    const float GetPsd(int index) const;
    const float GetQlong(int index) const;
    const float GetQshort(int index) const;
    const float GetTimeTag(int index) const;
    
    const std::vector<float> GetSamplePsdValues(int sampleSize);
    
    void SetEntries(int entries);
    void SetPSDValues(const std::vector<float>& values) {_psdValues = values;};
    void SetQlongValues(const std::vector<float>& values) {_qlongValues = values;};
    void SetQshortValues(const std::vector<float>& values) {_qshortValues = values;};
    void SetTimeTagValues(const std::vector<float>& values) {_timetagValues = values;};
        
private:
    
    EventParameters _parameters;
    EventData       _data;
    
    int _channelId;
    int _nEntries;
    
    TRandom3 _rand;
    
    std::vector<float> _psdValues;
    std::vector<float> _qlongValues;
    std::vector<float> _qshortValues;
    std::vector<float> _timetagValues;
    std::vector<int> _badEventFlags;
};


#endif
