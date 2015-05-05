#include "Handler.hh"

Handler::Handler() : _meanNrOfEvents(-1)
{
}

Handler::Handler(TString signalName,TString backgroundName,int binning,double temp,double press) : _binningFactor(binning),_meanNrOfEvents(-1)
{
    _source = std::make_shared<Source>(signalName, backgroundName);
    _source->SetTemperature(temp);
    _source->SetPressure(press);
    
    _h1_channelSpectra = nullptr;
    _h1_channelQShort = nullptr;
    _h1_channelPsd = nullptr;
    _h2_channelSpectra = nullptr;
    
    _h1Vtr_channelsSpectraSig.resize(0);
    _h1Vtr_channelsQShortSig.resize(0);
    _h2Vtr_channelsSpectraSig.resize(0);
    _h1Vtr_channelsSpectraBkg.resize(0);
    _h1Vtr_channelsQShortBkg.resize(0);
    _h1Vtr_channelsPsdSig.resize(0);
    _h1Vtr_channelsPsdBkg.resize(0);
    _h1Vtr_channelsSpectraSigMinusBkg.resize(0);
}

Handler::~Handler()
{
}

void Handler::InitialiseHistograms(bool signal)
{
    //channel data - loop over each channel
    for(int channel = 0;channel<CHANNELS;channel++)
    {
        //Histograms names
        TString histname_spectra    = Form("h1_channel_spectra_%i",channel);
        TString histname_qshort     = Form("h1_channel_qshort_%i",channel);
        TString histname_spectra_2D = Form("h2_channel_spectra_%i",channel);
        TString histname_psd        = Form("h1_channel_psd_%i",channel);
        
        if(signal)
        {
            if(_h1Vtr_channelsSpectraSig.size() != channel )
                return;
            
            histname_spectra    += "_sig";
            histname_qshort     += "_sig";
            histname_spectra_2D += "_sig";
            histname_psd        += "_sig";
        }
        else
        {
            if(_h1Vtr_channelsSpectraBkg.size() != channel )
                return;
            
            histname_spectra    += "_bkg";
            histname_qshort     += "_bkg";
            histname_spectra_2D += "_null";
            histname_psd        += "_bkg";
        }
        
        //Create histograms
        _h1_channelSpectra  = new TH1F(histname_spectra,
                                       histname_spectra,
                                       1000/_binningFactor,
                                       0,
                                       10000);
        
        _h1_channelQShort   = new TH1F(histname_qshort,
                                       histname_qshort,
                                       1000/_binningFactor,
                                       0,
                                       2000);
        
        _h1_channelPsd      = new TH1F(histname_psd,
                                       histname_psd,
                                       1000/_binningFactor,
                                       0,
                                       1);
        
        if(signal)
        {
            _h2_channelSpectra  = new TH2F(histname_spectra_2D,
                                           histname_spectra_2D,
                                           1000/_binningFactor,
                                           0,
                                           32768/4,
                                           1000/_binningFactor,
                                           0,
                                           1);
        }
        
        // Add histograms to vectors
        if(signal)
        {
            _h1Vtr_channelsSpectraSig.push_back(_h1_channelSpectra);
            _h1Vtr_channelsQShortSig.push_back(_h1_channelQShort);
            _h2Vtr_channelsSpectraSig.push_back(_h2_channelSpectra);
            _h1Vtr_channelsPsdSig.push_back(_h1_channelPsd);
        }
        else
        {
            _h1Vtr_channelsSpectraBkg.push_back(_h1_channelSpectra);
            _h1Vtr_channelsQShortBkg.push_back(_h1_channelQShort);
            _h1Vtr_channelsPsdBkg.push_back(_h1_channelPsd);
        }
    }
}

void Handler::FillHistograms(const ChannelData& data, bool signal)
{
    for(int i=0;i<data.GetEntries();++i)
    {
        float psd = data.GetPsd(i);
        float Qlong = data.GetQlong(i);
        float Qshort = data.GetQshort(i);
        
        if(signal)
        {
            if(psd>0)_h1Vtr_channelsPsdSig.at(data.GetId())->Fill(psd);
            if(Qlong>0)_h1Vtr_channelsSpectraSig.at(data.GetId())->Fill(Qlong);
            if(Qshort>0)_h1Vtr_channelsQShortSig.at(data.GetId())->Fill(Qshort);
            if(Qlong>0)_h2Vtr_channelsSpectraSig.at(data.GetId())->Fill(Qlong,psd);
        }
        else
        {
            if(psd>0)_h1Vtr_channelsPsdBkg.at(data.GetId())->Fill(psd);
            if(Qlong>0)_h1Vtr_channelsSpectraBkg.at(data.GetId())->Fill(Qlong);
            if(Qshort>0)_h1Vtr_channelsQShortBkg.at(data.GetId())->Fill(Qshort);
        }
    }
}

void Handler::Process(const std::vector<TTree*>& treePtr, bool signal, float timeCutOffInSecs)
{
    InitialiseHistograms(signal);
    
    for(int i=0;i<treePtr.size();++i)
    {
        if(!treePtr.at(i))continue;
        ProcessData(treePtr.at(i),signal,timeCutOffInSecs);
    }
    
}

void Handler::ProcessData(TTree* treePtr, bool signal, float timeCutOffInSecs)
{
    //setup the branches
    //parameters
    TBranch * br_para = nullptr;
    if(treePtr)br_para = treePtr->GetBranch("acq_params");
    else return;

    if(br_para)br_para->SetAddress(&_parameters);
    else return;

    br_para->GetEntry(0);
    
    //channel and tube data
    std::vector<TubeData> tubeData;
    std::vector<ChannelData> channelData(CHANNELS);
    
    // loop over channels
    TBranch* br_channel_data;
    for(int channel = 0;channel<CHANNELS;channel++)
    {
        br_channel_data = GetChannelDataBranch(treePtr, channel);
        ChannelData chData(channel);
        chData.ReadData(br_channel_data, timeCutOffInSecs);
        channelData[channel] = chData;
        
        // Set to tube data
        if(channel%2 ==1)
        {
            TubeData tbData(channelData[channel-1],channelData[channel],(channel +1)/2);
            tubeData.push_back(tbData);
        }
    }
    
    //loop over tubes to fill histograms with processed channel data
    for(int tube = 0;tube<TUBES;tube++)
    {
        tubeData[tube].ProcessData(_meanNrOfEvents);
        
        FillHistograms(tubeData[tube].GetChannelProcessedDataFirst(),signal);
        FillHistograms(tubeData[tube].GetChannelProcessedDataSecond(),signal);
        
    } //end loop over channels
}

void Handler::SetupSource()
{
    //loop over histograms to fill to source
    for(int i=0;i<_h1Vtr_channelsSpectraSig.size();i++)
    {
        _source->AddSpectraToSignalHistVtr(*_h1Vtr_channelsSpectraSig.at(i));
        if(_h1Vtr_channelsQShortSig.at(i))_source->AddQShortToSignalHistVtr(*_h1Vtr_channelsQShortSig.at(i));
        if(_h1Vtr_channelsPsdSig.at(i))_source->AddPsdToSignalHistVtr(*_h1Vtr_channelsPsdSig.at(i));
    }
    for(int i=0;i<_h2Vtr_channelsSpectraSig.size();i++)
    {
        _source->AddSpectraToSignal2DHistVtr(*_h2Vtr_channelsSpectraSig.at(i));
    }
    for(int i=0;i<_h1Vtr_channelsSpectraBkg.size();i++)
    {
        _source->AddSpectraToBackgroundHistVtr(*_h1Vtr_channelsSpectraBkg.at(i));
        if(_h1Vtr_channelsQShortBkg.at(i))_source->AddQShortToBackgroundHistVtr(*_h1Vtr_channelsQShortBkg.at(i));
        if(_h1Vtr_channelsPsdBkg.at(i))_source->AddPsdToBackgroundHistVtr(*_h1Vtr_channelsPsdBkg.at(i));
    }
    for(int i=0;i<_h1Vtr_channelsSpectraSigMinusBkg.size();i++)
    {
        _source->AddSpectraToSMinusBHistVtr(*_h1Vtr_channelsSpectraSigMinusBkg.at(i));
    }
}

void Handler::ResetHistograms(int channel, bool signal)
{
    if(channel <0 || channel >CHANNELS)return;
    
    Option_t* option = "ICES";
    
    if(signal)
    {
        _h1Vtr_channelsSpectraSig[channel]->Reset(option);
        _h1Vtr_channelsQShortSig[channel]->Reset(option);
        _h2Vtr_channelsSpectraSig[channel]->Reset(option);
        _h1Vtr_channelsPsdSig[channel]->Reset(option);
    }
    else
    {
        _h1Vtr_channelsSpectraBkg[channel]->Reset(option);
        _h1Vtr_channelsQShortBkg[channel]->Reset(option);
        _h1Vtr_channelsPsdBkg[channel]->Reset(option);
    }
}


