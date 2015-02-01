#include "Plotter.hh"

const static TString PLOTSFILENAME = "./plotsfiletemp.root";
const static TString SPECTRAEXT = "_ch";
const static TString PSDEXT = "_psd_ch";
const static TString FOMEXT = "_fom_ch";

Plotter::Plotter(int channel, bool savePlots) : 
		_channel(channel),
		_savePlots(savePlots)
{
    this->ResetCanvases();
    g1_fom_s_b = nullptr;
    g1_eff_s_b = nullptr;
    g1_pur_s_b = nullptr;
    g1_effXpur_s_b = nullptr;
}

Plotter::~Plotter()
{
}

void Plotter::DrawSpectra(const std::shared_ptr<Source> sourcePtr, bool showBackground)
{
    int size = sourcePtr->GetSignalSpectraHistVtr().size();

    if(_channel>=size)
    {
        std::cout << "\nNo spectra for that channel!, continuing.." <<std::endl;
        return;
    }
    
    // 1D histogram
    const TString spectraCavasName1D = "cvs_channel_spectra";
    const int spectraCavasPos1D = FindCanvas(spectraCavasName1D);
    if(spectraCavasPos1D < 0)
    {
        cvs_channel_spectra = new TCanvas(spectraCavasName1D,spectraCavasName1D);
        canvases.push_back(cvs_channel_spectra);
    }

    // 2D histogram
    const TString spectraCavasName2D = "cvs_channel_spectra_2D";
    const int spectraCavasPos2D = FindCanvas(spectraCavasName2D);
    if(spectraCavasPos2D < 0)
    {
        cvs_channel_spectra_2D = new TCanvas(spectraCavasName2D,spectraCavasName2D);
        canvases.push_back(cvs_channel_spectra_2D);
    }

    if(_channel<0)
    {
        DrawSpectraAll(sourcePtr, showBackground);
        return;
    }

    std::cout << "\n====================================================="
              << "\n=======                Spectra             =========="
              << "\n=====================================================";

    TH1F hist_signal      = sourcePtr->GetSignalSpectraHistVtr().at(_channel);
    TH2F hist_signal_2D   = sourcePtr->GetSignalSpectra2DHistVtr().at(_channel);
    TH1F hist_background  = sourcePtr->GetBackgroundSpectraHistVtr().at(_channel);
    TH1F hist_sigMinusBkg = sourcePtr->GetSMinusBSpectraHistVtr().at(_channel);

    SetupHistogram(hist_signal,1);
    PolishHistogram(hist_signal,"Q_{long}","Normalised count");
    cvs_channel_spectra->cd();
    hist_signal.DrawCopy("HIST");
    
    TString histname = sourcePtr->GetSignalName() + SPECTRAEXT + std::to_string(_channel);
    if(_savePlots)
        WriteToFile<TH1F>(hist_signal,histname,PLOTSFILENAME);

    cvs_channel_spectra_2D->cd();
    hist_signal_2D.DrawCopy("COLZ");

    if(showBackground)
    {
        SetupHistogram(hist_background,2);
        cvs_channel_spectra->cd();
        hist_background.DrawCopy("SAME");
      
        histname = sourcePtr->GetBackgroundName() + SPECTRAEXT + std::to_string(_channel);
        if(_savePlots)
            WriteToFile<TH1F>(hist_background,histname,PLOTSFILENAME);

        SetupHistogram(hist_sigMinusBkg,3);
        //cvs_channel_spectra->cd();
        //hist_sigMinusBkg.DrawCopy("SAME");
    }

    std::cout << "\nChannel : " <<_channel << ", Temp: " << sourcePtr->GetTemperature()
               << ", signal entries: " << hist_signal.GetEntries()
               << " ,mean " << GetMean(hist_signal)
               << ", background entries: " << GetNumberOfEntries(hist_background)
               << " ,mean " << GetMean(hist_background)
               << ", signal - background entries: " << hist_sigMinusBkg.GetEntries();
}

void Plotter::DrawSpectraAll(const std::shared_ptr<Source> sourcePtr,bool showBackground)
{
    cvs_channel_spectra->SetWindowSize(1600,800);
    cvs_channel_spectra->Divide(4,2);
    cvs_channel_spectra_2D->SetWindowSize(1600,800);
    cvs_channel_spectra_2D->Divide(4,2);

    int size = sourcePtr->GetSignalSpectraHistVtr().size();

    std::cout << "\n====================================================="
              << "\n=======                Spectra             =========="
              << "\n=====================================================";
      
    for(int ch=0;ch<size;ch++)
    {
        TH1F hist_signal = sourcePtr->GetSignalSpectraHistVtr().at(ch);
        TH2F hist_signal_2D = sourcePtr->GetSignalSpectra2DHistVtr().at(ch);
        TH1F hist_background = sourcePtr->GetBackgroundSpectraHistVtr().at(ch);
        TH1F hist_sigMinusBkg = sourcePtr->GetSMinusBSpectraHistVtr().at(ch);

        SetupHistogram(hist_signal,1);
        PolishHistogram(hist_signal,"Q_{long}","Normalised count");
        cvs_channel_spectra->cd(ch+1);
        hist_signal.DrawCopy("HIST");
        
        TString histname = sourcePtr->GetSignalName() + SPECTRAEXT + std::to_string(ch);
        if(_savePlots)
            WriteToFile<TH1F>(hist_signal,histname,PLOTSFILENAME);

        cvs_channel_spectra_2D->cd(ch+1);
        hist_signal_2D.DrawCopy("COLZ");

        if(showBackground)
        {
          SetupHistogram(hist_background,2);
          cvs_channel_spectra->cd(ch+1);
            hist_background.DrawCopy("SAME");
 
            histname = sourcePtr->GetBackgroundName() + SPECTRAEXT + std::to_string(ch);
            if(_savePlots)WriteToFile<TH1F>(hist_background,histname,PLOTSFILENAME);

            SetupHistogram(hist_sigMinusBkg,3);
            //hist_sigMinusBkg.DrawCopy("SAME");
        }

        std::cout<< "\nChannel : " <<ch << ", Temp: " << sourcePtr->GetTemperature()
                << ", signal entries: " << hist_signal.GetEntries()
                << " ,mean " << GetMean(hist_signal)
                << ", background entries: " << GetNumberOfEntries(hist_background)
                << " ,mean " << GetMean(hist_background)
                << ", signal - background entries: " << hist_sigMinusBkg.GetEntries();
    }
}

void Plotter::DrawPsd(const std::shared_ptr<Source> sourcePtr,bool showBackground)
{
    int size = sourcePtr->GetSignalPsdHistVtr().size();

    if(_channel>=size)
    {
        std::cout << "\nNo psd for that channel!, continuing.." <<std::endl;
        return;
    }
    
    const TString psdCavasName1D = "cvs_channel_psd";
    const int psdCavasPos1D = FindCanvas(psdCavasName1D);
    if(psdCavasPos1D < 0)
    {
        cvs_channel_psd = new TCanvas(psdCavasName1D,psdCavasName1D);
        canvases.push_back(cvs_channel_psd);
    }
    
    cvs_channel_psd->cd();

    if(_channel<0)
    {
        DrawPsdAll(sourcePtr, showBackground);
        return;
    }
  
    std::cout << "\n====================================================="
                << "\n=======                PSD                 =========="
                << "\n=====================================================";
  
    TH1F hist_signal     = sourcePtr->GetSignalPsdHistVtr().at(_channel);
    TH1F hist_background = sourcePtr->GetBackgroundPsdHistVtr().at(_channel);
      
    SetupHistogram(hist_signal,1);
    PolishHistogram(hist_signal,"(Q_{long} - Q_{short})/Q_{long}","Normalised count");
    hist_signal.DrawCopy("HIST");
    
    TString histname = sourcePtr->GetSignalName() + PSDEXT + std::to_string(_channel);
    if(_savePlots)
        WriteToFile<TH1F>(hist_signal,histname,PLOTSFILENAME);

    if(showBackground)
    {
        SetupHistogram(hist_background,2);
        hist_background.DrawCopy("SAME");
        histname = sourcePtr->GetBackgroundName() + PSDEXT + std::to_string(_channel);
        if(_savePlots)
            WriteToFile<TH1F>(hist_background,histname,PLOTSFILENAME);
    }
 
    std::cout<< "\nChannel : " <<_channel << ", Temp: " << sourcePtr->GetTemperature()
                << ", signal entries: " << hist_signal.GetEntries()
                << " ,mean " << GetMean(hist_signal)
                << ", background entries: " << GetNumberOfEntries(hist_background)
                << " ,mean " << GetMean(hist_background);
}

void Plotter::DrawPsdAll(const std::shared_ptr<Source> sourcePtr,bool showBackground)
{
    int size = sourcePtr->GetSignalPsdHistVtr().size();

    cvs_channel_psd->SetWindowSize(1600,800);
    cvs_channel_psd->Divide(4,2);
  
    std::cout << "\n====================================================="
                << "\n=======                PSD                 =========="
                << "\n=====================================================";
  
    for(int ch=0;ch<size;ch++)
    {
        TH1F hist_signal = sourcePtr->GetSignalPsdHistVtr().at(ch);
        TH1F hist_background = sourcePtr->GetBackgroundPsdHistVtr().at(ch);
      
        cvs_channel_psd->cd(ch+1);

        SetupHistogram(hist_signal,1);
        PolishHistogram(hist_signal,"(Q_{long} - Q_{short})/Q_{long}","Normalised count");
        hist_signal.DrawCopy("HIST");
      
        TString histname = sourcePtr->GetSignalName() + PSDEXT + std::to_string(ch);
        if(_savePlots)
            WriteToFile<TH1F>(hist_signal,histname,PLOTSFILENAME);

        if(showBackground)
        {
            SetupHistogram(hist_background,2);
            cvs_channel_psd->cd(ch+1);
            hist_background.DrawCopy("SAME");
 
            histname = sourcePtr->GetBackgroundName() + PSDEXT + std::to_string(ch);
            if(_savePlots)
                WriteToFile<TH1F>(hist_background,histname,PLOTSFILENAME);
        }

        std::cout << "\nChannel : " <<ch << ", Temp: " << sourcePtr->GetTemperature()
                    << ", signal entries: " << hist_signal.GetEntries()
                    << " ,mean " << GetMean(hist_signal)
                    << ", background entries: " << GetNumberOfEntries(hist_background)
                    << " ,mean " << GetMean(hist_background);
    }
}

void Plotter::DrawFOM(const std::shared_ptr<Source> sourcePtr)
{
    int sizeSignal = sourcePtr->GetSignalPsdHistVtr().size();
    int sizeBackground = sourcePtr->GetBackgroundPsdHistVtr().size();
    
    if(sizeSignal != sizeBackground) return;
    
    if(_channel>=sizeSignal)
    {
        std::cout << "\nNo psd for that channel!, continuing.." <<std::endl;
        return;
    }
    
    const TString fomCavasName1D = "cvs_channel_fom";
    const int fomCavasPos1D = FindCanvas(fomCavasName1D);
    if(fomCavasPos1D < 0)
    {
        cvs_channel_fom = new TCanvas(fomCavasName1D,fomCavasName1D);
        canvases.push_back(cvs_channel_fom);
    }
    
    const TString effCavasName1D = "cvs_channel_eff";
    const int effCavasPos1D = FindCanvas(effCavasName1D);
    if(effCavasPos1D < 0)
    {
        cvs_channel_eff = new TCanvas(effCavasName1D,effCavasName1D);
        canvases.push_back(cvs_channel_eff);
    }
    
    if(_channel<0)
    {
        DrawFOMAll(sourcePtr);
        return;
    }
    
    std::cout   << "\n====================================================="
                << "\n=======                FOM                 =========="
                << "\n=====================================================";
    
    SetupGraphs(sourcePtr, _channel);
    
    cvs_channel_fom->cd();
    g1_fom_s_b->Draw("AP");
    
    cvs_channel_eff->cd();
    g1_eff_s_b->Draw("AP");
    g1_pur_s_b->Draw("P");
    g1_effXpur_s_b->Draw("P");
    legend_->Draw();
    
    if(_savePlots)
        WriteGraphs(sourcePtr,_channel);
    
}

void Plotter::DrawFOMAll(const std::shared_ptr<Source> sourcePtr)
{
    
    int size = sourcePtr->GetSignalPsdHistVtr().size();
    
    cvs_channel_fom->SetWindowSize(1600,800);
    cvs_channel_fom->Divide(4,2);
    cvs_channel_eff->SetWindowSize(1600,800);
    cvs_channel_eff->Divide(4,2);
    
    std::cout   << "\n====================================================="
                << "\n=======                FOM                 =========="
                << "\n=====================================================";
    
    for(int ch=0;ch<size;ch++)
    {
        cvs_channel_fom->cd(ch+1);
        SetupGraphs(sourcePtr, ch);
        g1_fom_s_b->Draw("AP");
        
        cvs_channel_eff->cd(ch+1);
        g1_eff_s_b->Draw("AP");
        g1_pur_s_b->Draw("P");
        g1_effXpur_s_b->Draw("P");
        legend_->Draw();
        
        if(_savePlots)
            WriteGraphs(sourcePtr,ch);
    }
}

void Plotter::WriteGraphs(const std::shared_ptr<Source> sourcePtr, int channel)
{
    TString sourceName = "sig" + sourcePtr->GetSignalName() +"_bkg" + sourcePtr->GetBackgroundName();
    WriteToFile<TGraph>(*g1_fom_s_b, sourceName + g1_fom_s_b->GetTitle() + std::to_string(channel),PLOTSFILENAME);
    WriteToFile<TGraph>(*g1_eff_s_b, sourceName + g1_eff_s_b->GetTitle() + std::to_string(channel),PLOTSFILENAME);
    WriteToFile<TGraph>(*g1_pur_s_b, sourceName + g1_pur_s_b->GetTitle() + std::to_string(channel),PLOTSFILENAME);
    WriteToFile<TGraph>(*g1_effXpur_s_b, sourceName + g1_effXpur_s_b->GetTitle() + std::to_string(channel),PLOTSFILENAME);
}

void Plotter::SetupGraphs(const std::shared_ptr<Source> sourcePtr, int channel)
{
    TH1F hist_signal     = sourcePtr->GetSignalPsdHistVtr().at(channel);
    TH1F hist_background = sourcePtr->GetBackgroundPsdHistVtr().at(channel);
    
    NormaliseHistogram(hist_signal,1);
    NormaliseHistogram(hist_background,1);
    
    double highestFOM = 0.0;
    double bestCut = 0.0;
    
    double minXBin      = hist_signal.GetXaxis()->GetXmin();
    double maxXBin      = hist_signal.GetXaxis()->GetXmax();
    int    numberOfBins = hist_signal.GetNbinsX();
    
    //array for graphs
    double xValue[numberOfBins];
    double xValue_err[numberOfBins];
    double yValue_fom[numberOfBins];
    double yValue_fom_lowErr[numberOfBins];
    double yValue_fom_highErr[numberOfBins];
    double yValue_eff[numberOfBins];
    double yValue_eff_lowErr[numberOfBins];
    double yValue_eff_highErr[numberOfBins];
    double yValue_pur[numberOfBins];
    double yValue_pur_lowErr[numberOfBins];
    double yValue_pur_highErr[numberOfBins];
    double yValue_effXpur[numberOfBins];
    double yValue_effXpur_lowErr[numberOfBins];
    double yValue_effXpur_highErr[numberOfBins];
    
    //loop over histgram bins to get bin values
    for(int bin=0;bin<numberOfBins;bin++)
    {
        double binWidth = hist_signal.GetBinWidth(0);
        
        xValue[bin]     = minXBin + (bin+0.5)*binWidth;
        xValue_err[bin] = binWidth*0.5;
        
        BinaryResult fom     = GetFOM(hist_signal, hist_background,bin);
        BinaryResult pur     = GetPurity(hist_signal,hist_background,bin);
        BinaryResult eff     = GetEfficiency(hist_signal,bin);
        BinaryResult effXpur = GetEffXPur(hist_signal,hist_background, bin);
        
        if(fom.GetResult() == -1 || eff.GetResult() == -1 || pur.GetResult() == -1)
        {
            std::cout << "\n\nError! Number of bins do not match both histograms, please check code.\n" <<std::endl;
            return;
        }
        
        //find the best value for the f.o.m
        if(fom.GetResult()>=highestFOM)
        {
            highestFOM = fom.GetResult();
            bestCut = xValue[bin];
        }
        
        yValue_fom[bin]             = fom.GetResult();
        yValue_fom_lowErr[bin]      = fom.GetLowError();
        yValue_fom_highErr[bin]     = fom.GetHighError();
        yValue_eff[bin]             = eff.GetResult();
        yValue_eff_lowErr[bin]      = eff.GetLowError();
        yValue_eff_highErr[bin]     = eff.GetHighError();
        yValue_pur[bin]             = pur.GetResult();
        yValue_pur_lowErr[bin]      = pur.GetLowError();
        yValue_pur_highErr[bin]     = pur.GetHighError();
        yValue_effXpur[bin]         = effXpur.GetResult();
        yValue_effXpur_lowErr[bin]  = effXpur.GetLowError();
        yValue_effXpur_highErr[bin] = effXpur.GetHighError();
    }
    
    std::cout   << "\n========= Channel " << channel << " ========="
                << "\nbest f.o.m value: " << highestFOM
                << "\nfor long-short/long = " << bestCut
                <<"\n=============================" <<std::endl;
    
    //FOM graphs
    if(!g1_fom_s_b)delete g1_fom_s_b;
    g1_fom_s_b      = new TGraphAsymmErrors(numberOfBins,
                                            &xValue[0],
                                            &yValue_fom[0],
                                            &xValue_err[0],
                                            &xValue_err[0],
                                            &yValue_fom_lowErr[0],
                                            &yValue_fom_highErr[0]);
    g1_fom_s_b->SetTitle("FOM");
    
    if(!g1_eff_s_b)delete g1_eff_s_b;
    g1_eff_s_b      = new TGraphAsymmErrors(numberOfBins,
                                            &xValue[0],
                                            &yValue_eff[0],
                                            &xValue_err[0],
                                            &xValue_err[0],
                                            &yValue_eff_lowErr[0],
                                            &yValue_eff_highErr[0]);
    g1_eff_s_b->SetTitle("EFF");
    
    if(!g1_pur_s_b)delete g1_pur_s_b;
    g1_pur_s_b      = new TGraphAsymmErrors(numberOfBins,
                                            &xValue[0],
                                            &yValue_pur[0],
                                            &xValue_err[0],
                                            &xValue_err[0],
                                            &yValue_pur_lowErr[0],
                                            &yValue_pur_highErr[0]);
    g1_pur_s_b->SetTitle("PUR");
    
    if(!g1_effXpur_s_b)delete g1_effXpur_s_b;
    g1_effXpur_s_b  = new TGraphAsymmErrors(numberOfBins,
                                            &xValue[0],
                                            &yValue_effXpur[0],
                                            &xValue_err[0],
                                            &xValue_err[0],
                                            &yValue_effXpur_lowErr[0],
                                            &yValue_effXpur_highErr[0]);
    g1_effXpur_s_b->SetTitle("EffXPur");
    
    //Setup graphs
    //FOM
    g1_fom_s_b->SetTitle(Form("FOM for channel %i",channel));
    g1_fom_s_b->GetXaxis()->SetTitle("(QLong - QShort)/QLong");
    g1_fom_s_b->GetYaxis()->SetTitle("s / #sqrt{s+b}");
    g1_fom_s_b->GetYaxis()->SetTitleOffset(1.4);
    g1_fom_s_b->GetYaxis()->SetRangeUser(minXBin,maxXBin);
    g1_fom_s_b->SetMarkerStyle(24);
    g1_fom_s_b->SetMarkerSize(0.5);
    
    //graphs for efficencies and purities
    g1_eff_s_b->SetTitle(Form("Efficiency and Purity for channel %i",channel));
    g1_eff_s_b->GetXaxis()->SetTitle("(QLong - QShort)/QLong");
    g1_eff_s_b->GetYaxis()->SetRangeUser(minXBin,maxXBin);
    g1_eff_s_b->SetMarkerStyle(24);
    g1_eff_s_b->SetMarkerSize(0.5);
    g1_pur_s_b->SetMarkerStyle(24);
    g1_pur_s_b->SetMarkerSize(0.5);
    g1_pur_s_b->SetMarkerColor(kRed);
    g1_effXpur_s_b->SetMarkerStyle(24);
    g1_effXpur_s_b->SetMarkerSize(0.5);
    g1_effXpur_s_b->SetMarkerColor(kGreen);
    
    //legend
    legend_ = new TLegend(0.1,0.1,0.48,0.3);
    legend_->TAttFill::SetFillColor(0);
    legend_->AddEntry(g1_eff_s_b,"Efficiency","p");
    legend_->AddEntry(g1_pur_s_b,"Purity","p");
    legend_->AddEntry(g1_effXpur_s_b,"Eff #times Purity","p");

}

const BinaryResult Plotter::GetFOM(const TH1F& hist_signal, const TH1F& hist_background, int binMin)
{
    int numberOfSignalBins      = hist_signal.GetNbinsX();
    int numberOfBackgroundBins  = hist_background.GetNbinsX();
    
    //check they have the same binning
    if(numberOfSignalBins != numberOfBackgroundBins)return BinaryResult(-1,0,0);
    
    double integratedSignal = 0;
    double integratedBackground = 0;
    
    //binMin is the bin to cut on
    for(int bin=binMin;bin<numberOfSignalBins;bin++)
    {
        integratedSignal     += hist_signal.GetBinContent(bin+1);
        integratedBackground += hist_background.GetBinContent(bin+1);
    }
    
    //figure of merit
    BinaryResult FOM = GetBinaryResult(integratedSignal, TMath::Sqrt(integratedSignal + integratedBackground));
    return FOM;
}

const BinaryResult Plotter::GetPurity(const TH1F& hist_signal, const TH1F& hist_background, int binMin)
{
    int numberOfSignalBins      = hist_signal.GetNbinsX();
    int numberOfBackgroundBins  = hist_background.GetNbinsX();
    
    //check they have the same binning
    if(numberOfSignalBins != numberOfBackgroundBins)return BinaryResult(-1,0,0);
    
    double integratedSignal = 0;
    double integratedBackground = 0;
    
    //binMin is the bin to cut on
    for(int bin=binMin;bin<numberOfSignalBins;bin++)
    {
        integratedSignal     += hist_signal.GetBinContent(bin+1);
        integratedBackground += hist_background.GetBinContent(bin+1);
    }
    
    //purity
    BinaryResult PUR = GetBinaryResult(integratedSignal, (integratedSignal + integratedBackground));
    return PUR;
}

const BinaryResult Plotter::GetEfficiency(const TH1F& hist_signal, int binMin)
{
    int numberOfSignalBins      = hist_signal.GetNbinsX();
    
    double integratedSignal = 0;
    double totalSignal = hist_signal.Integral(0,numberOfSignalBins);
    
    //binMin is the bin to cut on
    for(int bin=binMin;bin<numberOfSignalBins;bin++)
    {
        integratedSignal     += hist_signal.GetBinContent(bin+1);
    }
    
    //efficiency
    BinaryResult EFF = GetBinaryResult(integratedSignal, totalSignal);
    return EFF;
    
}

const BinaryResult Plotter::GetEffXPur(const TH1F& hist_signal, const TH1F& hist_background, int binMin)
{
    int numberOfSignalBins      = hist_signal.GetNbinsX();
    int numberOfBackgroundBins  = hist_background.GetNbinsX();
    
    //check they have the same binning
    if(numberOfSignalBins != numberOfBackgroundBins)return BinaryResult(-1,0,0);
    
    double integratedSignal     = 0;
    double integratedBackground = 0;
    double totalSignal          = hist_signal.Integral(0,numberOfSignalBins);
    
    //binMin is the bin to cut on
    for(int bin=binMin;bin<numberOfSignalBins;bin++)
    {
        integratedSignal     += hist_signal.GetBinContent(bin+1);
        integratedBackground += hist_background.GetBinContent(bin+1);
    }
    
    //efficiency
    BinaryResult EFFxPUR = GetBinaryResult(integratedSignal*integratedSignal, totalSignal*(integratedSignal + integratedBackground));
    return EFFxPUR;
    
}

const BinaryResult Plotter::GetBinaryResult(const double numerator, const double denominator)
{
    //figure of merit
    TH1F numeritorHist("numeritorHist","numeritorHist",1,0,1);
    TH1F denominatorHist("denominatorHist","denominatorHist",1,0,1);
    numeritorHist.SetBinContent(0,numerator);
    denominatorHist.SetBinContent(0,denominator);
    
    //calculate error
    TGraphAsymmErrors error(&numeritorHist,&denominatorHist);
    
    const double result     = numerator/denominator;
    const double lowError   = error.GetErrorYlow(0);
    const double highError  = error.GetErrorYhigh(0);
    
    BinaryResult binaryResult(result, lowError, highError);
    return binaryResult;
}

const double Plotter::GetMean(const TH1F& hist) const
{
    double total = 0.;
    for(int bin=1;bin<hist.GetNbinsX();bin++)
    {      
        total += hist.GetXaxis()->GetBinCenter(bin);
    }

    return total/static_cast<double>(hist.GetEntries());
}

const int Plotter::GetNumberOfEntries(const TH1F& hist) const
{
  return hist.GetEntries();
}

void Plotter::ResetCanvases()
{
    for(int i=0;i<canvases.size();++i)
    {
        delete canvases[i];
    }
    canvases.resize(0);
}

const int Plotter::FindCanvas(const TString& name)
{
    int result = -1;
    
    for(int i=0;i<canvases.size();++i)
    {
        TString cvs_name = canvases[i]->GetName();
        if(name == cvs_name)
        {
            result = i;
            break;
        }
    }
    return result;
}

void Plotter::SetupHistogram(TH1F& hist,int option)
{

  NormaliseHistogram(hist);

  hist.SetLineStyle(1);
  hist.SetOption("HIST");

  //signal is 1 - black
  if(option <= 1)
    {
      hist.SetLineColor(kBlack);
      hist.SetLineWidth(2);
    }
  //background is 2 - red
  else if(option <=2)
    {
      hist.SetLineColor(kRed);
      hist.SetLineWidth(2);
    }
  //everything else is green
  else
    {
      hist.SetLineColor(kGreen);
      hist.SetLineWidth(1);
    }
}

//normalise histograms
void Plotter::NormaliseHistogram(TH1F& hist,int entries)
{  
  double factor = 1.0;

  if(entries > 1)
    {
      factor = static_cast<double>(entries);
    }
  else
    {
      factor = static_cast<double>(hist.GetEntries());
    }

  if(factor >0)
  {
	SetErrorBars(hist,factor);
	hist.Scale(1/factor);
  }
}

void Plotter::SetErrorBars(TH1F& hist, const double scale)
{
  //loop over the bins
  for(int i=0;i<hist.GetNbinsX();++i)
  {
	//get old error
	const double oldError = hist.GetBinError(i);

	//set the scaled error
	hist.SetBinError(i,oldError/scale);
  }

}

void Plotter::PolishHistogram(TH1F& hist,const TString xtitle,const TString ytitle)
{
  hist.GetXaxis()->SetTitle(xtitle);
  hist.GetXaxis()->SetTitleOffset(1.1);
  hist.GetXaxis()->SetLabelSize(0.04);
  hist.GetXaxis()->SetTitleSize(0.042);

  std::stringstream ss;
  ss << hist.GetBinWidth(0);
  TString yTitleFull = ytitle + " / " + ss.str();
  hist.GetYaxis()->SetTitle(yTitleFull);
  hist.GetYaxis()->SetTitleOffset(1.0);
  hist.GetYaxis()->SetLabelSize(0.04);
  hist.GetYaxis()->SetTitleSize(0.042);
}
