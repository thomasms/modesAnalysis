#ifndef TUBEDATA_HH
#define TUBEDATA_HH

#include <vector>
#include <memory>
#include <iomanip>
#include <utility>

#include "ChannelData.hh"

class TubeData
{
    
public:
    TubeData(const ChannelData& channel0, const ChannelData& channel1, int id);
    ~TubeData();
    
    void ProcessData();
    
    inline const int GetId()  const  {return _tubeId;};
    inline const int GetEntries()  const  {return _nEntries;};
    
    const ChannelData GetChannelDataFirst() {return _channel0;};
    const ChannelData GetChannelDataSecond() {return _channel1;};
    
private:
    
    int _tubeId;
    int _nEntries;
    
    ChannelData _channel0;
    ChannelData _channel1;
    
};


#endif
