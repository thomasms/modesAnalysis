#include "Handler.hh"

Handler::Handler()
{
}

Handler::Handler(TString signalName,TString backgroundName,int binning,double temp,double press) : _binningFactor(binning)
{
    _source = std::make_shared<Source>(signalName, backgroundName);
    _source->SetTemperature(temp);
    _source->SetPressure(press);
}

Handler::~Handler()
{
}

void Handler::InitialiseHistograms(int channel,bool signal)
{
    //Histograms names
    TString histname_spectra    = Form("h1_channel_spectra_%i",channel);
    TString histname_spectra_2D = Form("h2_channel_spectra_%i",channel);
    TString histname_psd        = Form("h1_channel_psd_%i",channel);

    if(signal)
    {
        if(_h1Vtr_channelsSpectraSig.size() != channel )
            return;

        histname_spectra    += "_sig";
        histname_spectra_2D += "_sig";
        histname_psd        += "_sig";
    }
    else
    {
        if(_h1Vtr_channelsSpectraBkg.size() != channel )
            return;

        histname_spectra    += "_bkg";
        histname_spectra_2D += "_null";
        histname_psd        += "_bkg";
    }

    //Create histograms
    _h1_channelSpectra  = new TH1F(histname_spectra,
                                   histname_spectra,
                                   1024/_binningFactor,
                                   0,
                                   32768/4);
    
    _h1_channelPsd      = new TH1F(histname_psd,
                                   histname_psd,
                                   400/_binningFactor,
                                   0,
                                   1);
    
    _h2_channelSpectra  = new TH2F(histname_spectra_2D,
                                   histname_spectra_2D,
                                   1024/_binningFactor,
                                   0,
                                   32768/4,
                                   1000/_binningFactor,
                                   0,
                                   1);
    

    // Add histograms to vectors
    if(signal)
    {
        _h1Vtr_channelsSpectraSig.push_back(_h1_channelSpectra);
        _h2Vtr_channelsSpectraSig.push_back(_h2_channelSpectra);
        _h1Vtr_channelsPsdSig.push_back(_h1_channelPsd);
    }
    else
    {
        _h1Vtr_channelsSpectraBkg.push_back(_h1_channelSpectra);
        _h1Vtr_channelsPsdBkg.push_back(_h1_channelPsd);
    }
}

void Handler::FillHistograms(int channel,bool signal,float Qlong,float Qshort)
{
    float diff = ( Qlong - Qshort ) / Qlong;
    //std::cout << "\nQLong: " << Qlong << ",QShort: " << Qshort <<", Diff: "<< diff;
    
    //it should not be possible to have negative psd as qlong is total integral of waveform
    if(diff <0)return;
    
    if(signal)
    {
        _h1Vtr_channelsSpectraSig.at(channel)->Fill(Qlong);
        _h2Vtr_channelsSpectraSig.at(channel)->Fill(Qlong,diff);
        _h1Vtr_channelsPsdSig.at(channel)->Fill(diff);
    }
    else
    {
        _h1Vtr_channelsSpectraBkg.at(channel)->Fill(Qlong);
        _h1Vtr_channelsPsdBkg.at(channel)->Fill(diff);
    }
}

TH1F* Handler::GetSignalSpectraHistPtr(int channel)
{
  TH1F* h1_spec = nullptr;

  if( (_h1Vtr_channelsSpectraSig.size() < channel) || (channel <0) )return nullptr;

  if( _h1Vtr_channelsSpectraSig.at(channel) )
    {  
      h1_spec = _h1Vtr_channelsSpectraSig.at(channel);
    }
  
  return h1_spec;
}

TH2F* Handler::GetSignalSpectra2DHistPtr(int channel)
{
  TH2F* h2_spec = nullptr;

  if( (_h2Vtr_channelsSpectraSig.size() < channel) || (channel <0) )return nullptr;

  if( _h2Vtr_channelsSpectraSig.at(channel) )
    {  
      h2_spec = _h2Vtr_channelsSpectraSig.at(channel);
    }
  
  return h2_spec;
}

TH1F* Handler::GetSignalPsdHistPtr(int channel)
{
  TH1F* h1_psd = nullptr;

  if( (_h1Vtr_channelsPsdSig.size() < channel) || (channel <0) )return nullptr;

  if( _h1Vtr_channelsPsdSig.at(channel) )
    {  
      h1_psd = _h1Vtr_channelsPsdSig.at(channel);
    }
  
  return h1_psd;
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

    //channel data - loop over each channel
    for(int channel = 0;channel<CHANNELS;channel++)
    {
        TBranch* br_channel_data = treePtr->GetBranch(Form("acq_ch%i",channel));
        InitialiseHistograms(channel,signal);
      
        //Is the branch valid?
        if (br_channel_data!=nullptr && br_channel_data->GetEntries()!=0)
        {
            br_channel_data->SetAddress(&_data);
	
            //loop over entries on each channel
            for (int e=0; e<br_channel_data->GetEntries(); e++)
            {
                br_channel_data->GetEntry(e);
	      
                float time = (float)_data.timetag;
                float timeInSecs = time*1e-9;

                //only count events before time set, -1 indicates all times
                if(timeInSecs>timeCutOffInSecs && timeCutOffInSecs != -1)
                    continue;

                float Qlong = _data.qlong;
                float Qshort = _data.qshort;
	      
                FillHistograms(channel,signal,Qlong,Qshort);
	      
            }//end loop over events
	  
        }//end conditional statement
      
    } //end loop over channels
}

void Handler::SubtractBackground()
{

  for(int ch =0;ch<_h1Vtr_channelsSpectraSig.size();ch++)
    {
      TString histname_spectra = Form("h1_channel_spectra_%i_s-b",ch);
      _h1_channelSpectra = new TH1F(histname_spectra,histname_spectra,1024/_binningFactor,0,32768/4);
      
      //normalise them first
      //NormaliseHistogram(h1_vector_channels_spectra_sig.at(ch));
      //NormaliseHistogram(h1_vector_channels_spectra_bkg.at(ch));
      
      //loop over bins and take value from each
      for(int bin=1;bin<_h1Vtr_channelsSpectraSig.at(ch)->GetNbinsX();bin++)
	{
	  int bincontentT = _h1Vtr_channelsSpectraSig.at(ch)->GetBinContent(bin);
	  int bincontentB = _h1Vtr_channelsSpectraBkg.at(ch)->GetBinContent(bin);
	  
	  int bincontentDiff = bincontentT - bincontentB;
	  int bincontentDiffSq = TMath::Power((bincontentT - bincontentB),2);
	  //only positive difference
	  if(bincontentDiff<0)bincontentDiff = -bincontentDiff;
	  
	  _h1_channelSpectra->SetBinContent(bin,bincontentDiff);
	  
	  // 	  std::cout << "\nBin: " << bin << ", Total entries: " << bincontentT 
	  // 			<< ", Background entries: " << bincontentB;
	  
	}
      /*
  	if(h1_vector_channels_spectra_bkg.size()>=ch){
	h1_channel_spectra->Add(h1_vector_channels_spectra_sig.at(ch),
	h1_vector_channels_spectra_bkg.at(ch),1,-1);
	}
 	else h1_channel_spectra->Add(h1_vector_channels_spectra_sig.at(ch),1);
      */
      //NormaliseHistogram(h1_channel_spectra);
      _h1Vtr_channelsSpectraSigMinusBkg.push_back(_h1_channelSpectra);
      
    }
}

void Handler::SetupSource()
{
    //loop over histograms to fill to source
    for(int i=0;i<_h1Vtr_channelsSpectraSig.size();i++)
    {
        _source->AddSpectraToSignalHistVtr(*_h1Vtr_channelsSpectraSig.at(i));
        if(_h1Vtr_channelsPsdSig.at(i))_source->AddPsdToSignalHistVtr(*_h1Vtr_channelsPsdSig.at(i));
    }
    for(int i=0;i<_h2Vtr_channelsSpectraSig.size();i++)
    {
        _source->AddSpectraToSignal2DHistVtr(*_h2Vtr_channelsSpectraSig.at(i));
    }
    for(int i=0;i<_h1Vtr_channelsSpectraBkg.size();i++)
    {
        _source->AddSpectraToBackgroundHistVtr(*_h1Vtr_channelsSpectraBkg.at(i));
        if(_h1Vtr_channelsPsdBkg.at(i))_source->AddPsdToBackgroundHistVtr(*_h1Vtr_channelsPsdBkg.at(i));
    }
    for(int i=0;i<_h1Vtr_channelsSpectraSigMinusBkg.size();i++)
    {
        _source->AddSpectraToSMinusBHistVtr(*_h1Vtr_channelsSpectraSigMinusBkg.at(i));
    }
}


