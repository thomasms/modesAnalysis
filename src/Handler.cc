#include "Handler.hh"

Handler::Handler() : _sameNrOfEventsPerTube(false)
{
}

Handler::Handler(TString signalName,TString backgroundName,int binning,double temp,double press) : _binningFactor(binning), _sameNrOfEventsPerTube(false)
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

void Handler::FillHistograms(int channel,bool signal, bool psdOnly, float Qlong,float Qshort, float shiftQLong, float shiftQShort)
{
    float diff = ( Qlong  + shiftQLong - Qshort - shiftQShort) / (Qlong + shiftQLong);
    //std::cout << "\nQLong: " << Qlong << ",QShort: " << Qshort <<", Diff: "<< diff;
    
    if(signal)
    {
        _h1Vtr_channelsPsdSig.at(channel)->Fill(diff);
        if(!psdOnly)
        {
            _h1Vtr_channelsSpectraSig.at(channel)->Fill(Qlong + shiftQLong);
            _h1Vtr_channelsQShortSig.at(channel)->Fill(Qshort + shiftQShort);
            _h2Vtr_channelsSpectraSig.at(channel)->Fill(Qlong + shiftQLong,diff);
        }
    }
    else
    {
        _h1Vtr_channelsPsdBkg.at(channel)->Fill(diff);
        if(!psdOnly)
        {
            _h1Vtr_channelsSpectraBkg.at(channel)->Fill(Qlong + shiftQLong);
            _h1Vtr_channelsQShortBkg.at(channel)->Fill(Qshort + shiftQShort);
        }
    }
}

void Handler::Process(const std::vector<TTree*>& treePtr, bool signal, float timeCutOffInSecs, bool shift)
{
    InitialiseHistograms(signal);
    
    for(int i=0;i<treePtr.size();++i)
    {
        if(!treePtr.at(i))continue;
        ProcessData(treePtr.at(i),signal, false, timeCutOffInSecs, 0, 0);
    }
    
    std::vector<TH1F*>* histVectorQLong = (signal) ? &_h1Vtr_channelsSpectraSig : &_h1Vtr_channelsSpectraBkg;
    std::vector<TH1F*>* histVectorQShort = (signal) ? &_h1Vtr_channelsQShortSig : &_h1Vtr_channelsQShortBkg;
    std::vector<TH1F*>* histVectorPsd = (signal) ? &_h1Vtr_channelsPsdSig : & _h1Vtr_channelsPsdBkg;
    
    if(shift)
    {
        // What are the best values???
        double valueToShift = (signal) ? 0.769 : 0.504;
        
        auto shifts = ShiftToMeanHistPeak(histVectorQLong, histVectorQShort, valueToShift);
        
        //reset psd histogram
        for(int i=0;i<histVectorPsd->size();i++)
        {
            histVectorPsd->at(i)->Reset("ICES");
        }
        
        for(int i=0;i<treePtr.size();++i)
        {
            if(!treePtr.at(i))continue;
            //redo psd only
            ProcessData(treePtr.at(i),signal, true, timeCutOffInSecs, shifts[i], shifts[i]);
        }
    }
}

void Handler::ProcessData(TTree* treePtr, bool signal, bool psdOnly, float timeCutOffInSecs, float shiftQLong, float shiftQShort)
{
    //setup the branches
    //parameters
    TBranch * br_para = nullptr;
    if(treePtr)br_para = treePtr->GetBranch("acq_params");
    else return;

    if(br_para)br_para->SetAddress(&_parameters);
    else return;

    br_para->GetEntry(0);
    
    std::vector<int> nEvents(CHANNELS,0);

    TBranch* br_channel_data;
    //channel data - get number of events of each channel first
    for(int channel = 0;channel<CHANNELS;channel++)
    {
        br_channel_data = treePtr->GetBranch(Form("acq_ch%i",channel));
        nEvents[channel] = br_channel_data->GetEntries();
        
        if(!_sameNrOfEventsPerTube)
        {
            br_channel_data->SetAddress(&_data);
            
            //loop over entries on each channel
            for (int e=0; e<nEvents[channel]; ++e)
            {
                br_channel_data->GetEntry(e);
                
                float Qlong = _data.qlong;
                float Qshort = _data.qshort;
                
                FillHistograms(channel,signal, psdOnly, Qlong,Qshort, shiftQLong, shiftQShort);
                
            }//end loop over events
        }
        else if(channel%2 == 1)
        {
            if(nEvents[channel] > nEvents[channel-1])
                nEvents[channel] = nEvents[channel-1];
            else
                nEvents[channel-1] = nEvents[channel];
        }
    } //end loop over channels
    
    if(!_sameNrOfEventsPerTube)return;
    
    std::vector< std::vector<int> > badEvents;
    //channel data - loop over again to get bad event indicies
    for(int channel = 0;channel<CHANNELS;channel++)
    {
        std::vector<int> badChannelEvents;
        br_channel_data = treePtr->GetBranch(Form("acq_ch%i",channel));
        
        //Is the branch valid?
        if (br_channel_data!=nullptr && br_channel_data->GetEntries()!=0)
        {
            br_channel_data->SetAddress(&_data);
	            
            //loop over entries on each channel
            for (int e=0; e<nEvents[channel]; ++e)
            {
                br_channel_data->GetEntry(e);
	      
                float time = static_cast<float>(_data.timetag);
                float timeInSecs = time*1e-9;
                float Qlong = _data.qlong;
                float Qshort = _data.qshort;
                float psd = (Qlong - Qshort) /Qlong;
                
                //only count events before time set, -1 indicates all times
                //ignore negative times
                // other cuts
                if( (timeInSecs>timeCutOffInSecs || (timeInSecs <0) || (Qlong <= 0) || (Qshort <= 0) || (psd <= 0)) && (timeCutOffInSecs != -1) )
                    badChannelEvents.push_back(1);
                else
                    badChannelEvents.push_back(0);
                
            }//end loop over events
            
        }//end conditional statement
      
        // record bad events
        badEvents.push_back(badChannelEvents);
        
    } //end loop over channels
    
    //loop over finally to only count good events and fill histograms
    for(int channel = 0;channel<CHANNELS;channel++)
    {
        br_channel_data = treePtr->GetBranch(Form("acq_ch%i",channel));
        
        //Is the branch valid?
        if (br_channel_data!=nullptr && br_channel_data->GetEntries()!=0)
        {
            br_channel_data->SetAddress(&_data);
            
            //loop over entries on each channel
            for (int e=0; e<nEvents[channel]; ++e)
            {
                br_channel_data->GetEntry(e);
                
                float Qlong = _data.qlong;
                float Qshort = _data.qshort;
                
                //Skip bad events for whole detector not per channel
                if(channel%2==0)
                    if((badEvents[channel][e] ==1) || (badEvents[channel+1][e] ==1))continue;
                if(channel%2==1)
                    if((badEvents[channel][e] ==1) || (badEvents[channel-1][e] ==1))continue;
                    
                FillHistograms(channel,signal, psdOnly, Qlong,Qshort, shiftQLong, shiftQShort);
                
            }//end loop over events
            
        }//end conditional statement
        
    } //end loop over channels
}

const std::vector<float> Handler::ShiftToMeanHistPeak(std::vector<TH1F*>* histQlongVector,
                                                      std::vector<TH1F*>* histQshortVector, const double shiftToValue)
{
    std::vector<float> shifts;
    
    if(histQlongVector->size() != histQshortVector->size())
        return shifts;
    
    double qLongMean = 0;
    double qShortMean = 0;
    double shift = 0;
    
    // get the mean of the mean of channels
    for(int i=0;i<histQlongVector->size();++i)
    {
        //Peak fitter
        GaussianFitter fitter(histQlongVector->at(i));
        auto fit = fitter.Fit();
        qLongMean = fitter.GetMean();
        
        fitter.SetHistogram(histQshortVector->at(i));
        fit = fitter.Fit();
        qShortMean = fitter.GetMean();
        
        shift = -((shiftToValue - 1.0)*qLongMean + qShortMean)/shiftToValue;
        shifts.push_back(shift);
        
        std::cout << "\nQlong: " << qLongMean << ", Qshort: " << qShortMean << ", Shift: " << shift;
    }
    
    return shifts;
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

void Handler::ResetHistograms(bool signal)
{
    Option_t* option = "ICES";
    
    if(signal)
    {
        for(int i=0;i<_h1Vtr_channelsSpectraSig.size();i++) _h1Vtr_channelsSpectraSig[i]->Reset(option);
        for(int i=0;i<_h1Vtr_channelsQShortSig.size();i++) _h1Vtr_channelsQShortSig[i]->Reset(option);
        for(int i=0;i<_h2Vtr_channelsSpectraSig.size();i++) _h2Vtr_channelsSpectraSig[i]->Reset(option);
        for(int i=0;i<_h1Vtr_channelsPsdSig.size();i++) _h1Vtr_channelsPsdSig[i]->Reset(option);
    }
    else
    {
        for(int i=0;i<_h1Vtr_channelsSpectraBkg.size();i++) _h1Vtr_channelsSpectraBkg[i]->Reset(option);
        for(int i=0;i<_h1Vtr_channelsQShortBkg.size();i++) _h1Vtr_channelsQShortBkg[i]->Reset(option);
        for(int i=0;i<_h1Vtr_channelsPsdBkg.size();i++) _h1Vtr_channelsPsdBkg[i]->Reset(option);
    }
}


