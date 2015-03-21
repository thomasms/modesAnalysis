#include "Plotter.hh"

const static TString PLOTSFILENAME = "./plotsfiletemp.root";
const static TString SPECTRAEXT = "_ch";
const static TString QSHORTEXT = "_qshort_ch";
const static TString PSDEXT = "_psd_ch";
const static TString FOMEXT = "_fom_ch";
const static Color_t FITCOLOR = kBlue;

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

    NormaliseHistogram(hist_signal);
    NormaliseHistogram(hist_background);
    
    SetupHistogram(hist_signal,1);
    PolishHistogram(hist_signal,"Q_{long}","Normalised count");
    cvs_channel_spectra->cd();
    cvs_channel_spectra->cd()->SetTickx();
    cvs_channel_spectra->cd()->SetTicky();
    // Change y axis to show both peaks
    if( showBackground )
        ChangeRangeToFitBoth(hist_signal, hist_background);
    hist_signal.DrawCopy("HIST");
    
    TString histname = sourcePtr->GetSignalName() + SPECTRAEXT + std::to_string(_channel);
    if(_savePlots)
        WriteToFile<TH1F>(hist_signal,histname,PLOTSFILENAME);
    
    cvs_channel_spectra_2D->cd()->SetTickx();
    cvs_channel_spectra_2D->cd()->SetTicky();
    hist_signal_2D.DrawCopy("COLZ");

    if(showBackground)
    {
        SetupHistogram(hist_background,2);
        cvs_channel_spectra->cd()->SetTickx();
        cvs_channel_spectra->cd()->SetTicky();
        hist_background.DrawCopy("SAME");
        
        histname = sourcePtr->GetBackgroundName() + SPECTRAEXT + std::to_string(_channel);
        if(_savePlots)
            WriteToFile<TH1F>(hist_background,histname,PLOTSFILENAME);
    }
    
    PrintSpectraDetails(sourcePtr,_channel);
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
        
        NormaliseHistogram(hist_signal);
        NormaliseHistogram(hist_background);
        
        SetupHistogram(hist_signal,1);
        PolishHistogram(hist_signal,"Q_{long}","Normalised count");
        cvs_channel_spectra->cd(ch+1);
        cvs_channel_spectra->cd(ch+1)->SetTickx();
        cvs_channel_spectra->cd(ch+1)->SetTicky();
        if(hist_signal.GetEntries() >0)
        {
            // Change y axis to show both peaks
            if( showBackground )
                ChangeRangeToFitBoth(hist_signal, hist_background);
            
            hist_signal.DrawCopy("HIST");
        }
        
        // Peak fitter
        GaussianFitter fitter(&hist_signal);
        TF1* fit = fitter.Fit();
        fit->SetLineColor(FITCOLOR);
        fit->SetLineWidth(2);
        fit->DrawCopy("SAME");
        //fitter.PrintDetails();
                
        TString histname = sourcePtr->GetSignalName() + SPECTRAEXT + std::to_string(ch);
        if(_savePlots)
            WriteToFile<TH1F>(hist_signal,histname,PLOTSFILENAME);
        
        cvs_channel_spectra_2D->cd(ch+1)->SetTickx();
        cvs_channel_spectra_2D->cd(ch+1)->SetTicky();
        hist_signal_2D.DrawCopy("COLZ");

        if(showBackground)
        {
            SetupHistogram(hist_background,2);
            cvs_channel_spectra->cd(ch+1)->SetTickx();
            cvs_channel_spectra->cd(ch+1)->SetTicky();
            if(hist_background.GetEntries() >0)hist_background.DrawCopy("SAME");
            
            //Peak fitter
            fitter.SetHistogram(&hist_background);
            fitter.Fit();
            fit = fitter.Fit();
            fit->SetLineColor(FITCOLOR);
            fit->SetLineWidth(2);
            fit->DrawCopy("SAME");
            //fitter.PrintDetails();
 
            histname = sourcePtr->GetBackgroundName() + SPECTRAEXT + std::to_string(ch);
            if(_savePlots)WriteToFile<TH1F>(hist_background,histname,PLOTSFILENAME);
        }
        
        PrintSpectraDetails(sourcePtr,ch);
    }
}

void Plotter::DrawQShort(const std::shared_ptr<Source> sourcePtr, bool showBackground)
{
    int size = sourcePtr->GetSignalQShortHistVtr().size();
    
    if(_channel>=size)
    {
        std::cout << "\nNo qshort for that channel!, continuing.." <<std::endl;
        return;
    }
    
    const TString qshortCavasName1D = "cvs_channel_qshort";
    const int qshortCavasPos = FindCanvas(qshortCavasName1D);
    if(qshortCavasPos < 0)
    {
        cvs_channel_qshort = new TCanvas(qshortCavasName1D,qshortCavasName1D);
        canvases.push_back(cvs_channel_qshort);
    }
    
    if(_channel<0)
    {
        DrawQShortAll(sourcePtr, showBackground);
        return;
    }
    
    std::cout << "\n====================================================="
             << "\n=======                QShort              =========="
                << "\n=====================================================";
    
    TH1F hist_signal      = sourcePtr->GetSignalQShortHistVtr().at(_channel);
    TH1F hist_background  = sourcePtr->GetBackgroundQShortHistVtr().at(_channel);
    
    NormaliseHistogram(hist_signal);
    NormaliseHistogram(hist_background);
    
    SetupHistogram(hist_signal,1);
    PolishHistogram(hist_signal,"Q_{short}","Normalised count");
    cvs_channel_qshort->cd();
    cvs_channel_qshort->cd()->SetTickx();
    cvs_channel_qshort->cd()->SetTicky();
    // Change y axis to show both peaks
    if( showBackground )
        ChangeRangeToFitBoth(hist_signal, hist_background);
    hist_signal.DrawCopy("HIST");
    
    TString histname = sourcePtr->GetSignalName() + QSHORTEXT + std::to_string(_channel);
    if(_savePlots)
        WriteToFile<TH1F>(hist_signal,histname,PLOTSFILENAME);
    
    if(showBackground)
    {
        SetupHistogram(hist_background,2);
        hist_background.DrawCopy("SAME");
        
        histname = sourcePtr->GetBackgroundName() + QSHORTEXT + std::to_string(_channel);
        if(_savePlots)
            WriteToFile<TH1F>(hist_background,histname,PLOTSFILENAME);
    }
    
    PrintQShortDetails(sourcePtr,_channel);
}

void Plotter::DrawQShortAll(const std::shared_ptr<Source> sourcePtr,bool showBackground)
{
    int size = sourcePtr->GetSignalQShortHistVtr().size();
    
    cvs_channel_qshort->SetWindowSize(1600,800);
    cvs_channel_qshort->Divide(4,2);
    
    std::cout << "\n====================================================="
    << "\n=======                Qshort              =========="
    << "\n=====================================================";
    
    for(int ch=0;ch<size;ch++)
    {
        TH1F hist_signal = sourcePtr->GetSignalQShortHistVtr().at(ch);
        TH1F hist_background = sourcePtr->GetBackgroundQShortHistVtr().at(ch);
        
        NormaliseHistogram(hist_signal);
        NormaliseHistogram(hist_background);
        
        SetupHistogram(hist_signal,1);
        PolishHistogram(hist_signal,"Q_{long}","Normalised count");
        cvs_channel_qshort->cd(ch+1);
        cvs_channel_qshort->cd(ch+1)->SetTickx();
        cvs_channel_qshort->cd(ch+1)->SetTicky();
        if(hist_signal.GetEntries() >0)
        {
            // Change y axis to show both peaks
            if( showBackground )
                ChangeRangeToFitBoth(hist_signal, hist_background);
            hist_signal.DrawCopy("HIST");
        }
        
        //Peak fitter
        GaussianFitter fitter(&hist_signal);
        TF1* fit = fitter.Fit();
        fit->SetLineColor(FITCOLOR);
        fit->SetLineWidth(2);
        fit->DrawCopy("SAME");
        //fitter.PrintDetails();
        
        TString histname = sourcePtr->GetSignalName() + QSHORTEXT + std::to_string(ch);
        if(_savePlots)
            WriteToFile<TH1F>(hist_signal,histname,PLOTSFILENAME);
        
        if(showBackground)
        {
            SetupHistogram(hist_background,2);
            cvs_channel_qshort->cd(ch+1)->SetTickx();
            cvs_channel_qshort->cd(ch+1)->SetTicky();
            if(hist_background.GetEntries() >0)hist_background.DrawCopy("SAME");
            
            //Peak fitter
            fitter.SetHistogram(&hist_background);
            fit = fitter.Fit();
            fit->SetLineColor(FITCOLOR);
            fit->SetLineWidth(2);
            fit->DrawCopy("SAME");
            //fitter.PrintDetails();
            
            histname = sourcePtr->GetBackgroundName() + QSHORTEXT + std::to_string(ch);
            if(_savePlots)WriteToFile<TH1F>(hist_background,histname,PLOTSFILENAME);
        }
        
        PrintQShortDetails(sourcePtr,ch);
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
    
    cvs_channel_psd->cd()->SetTickx();
    cvs_channel_psd->cd()->SetTicky();

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
    
    NormaliseHistogram(hist_signal);
    NormaliseHistogram(hist_background);
    
    SetupHistogram(hist_signal,1);
    PolishHistogram(hist_signal,"(Q_{long} - Q_{short})/Q_{long}","Normalised count");
    // Change y axis to show both peaks
    if( showBackground )
        ChangeRangeToFitBoth(hist_signal, hist_background);
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
    
    PrintPSDDetails(sourcePtr,_channel);
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
        
        NormaliseHistogram(hist_signal);
        NormaliseHistogram(hist_background);
        
        SetupHistogram(hist_signal,1);
        PolishHistogram(hist_signal,"(Q_{long} - Q_{short})/Q_{long}","Normalised count");
        hist_signal.GetYaxis()->SetTitleOffset(1.4);
        if(hist_signal.GetEntries()>0)
        {
            // Change y axis to show both peaks
            if( showBackground )
                ChangeRangeToFitBoth(hist_signal, hist_background);
            hist_signal.DrawCopy("HIST");
        }
        
        //Peak fitter
        GaussianFitter fitter(&hist_signal);
        TF1* fit = fitter.Fit();
        fit->SetLineColor(FITCOLOR);
        fit->SetLineWidth(2);
        fit->DrawCopy("SAME");
        //fitter.PrintDetails();
        
        TString histname = sourcePtr->GetSignalName() + PSDEXT + std::to_string(ch);
        if(_savePlots)
            WriteToFile<TH1F>(hist_signal,histname,PLOTSFILENAME);

        if(showBackground)
        {
            SetupHistogram(hist_background,2);
            cvs_channel_psd->cd(ch+1)->SetTickx();
            cvs_channel_psd->cd(ch+1)->SetTicky();
            if(hist_background.GetEntries()>0)hist_background.DrawCopy("SAME");
            
            // Peak fitter
            fitter.SetHistogram(&hist_background);
            fit = fitter.Fit();
            fit->SetLineColor(FITCOLOR);
            fit->SetLineWidth(2);
            fit->DrawCopy("SAME");
            fitter.PrintDetails();
 
            histname = sourcePtr->GetBackgroundName() + PSDEXT + std::to_string(ch);
            if(_savePlots)
                WriteToFile<TH1F>(hist_background,histname,PLOTSFILENAME);
        }
        
        PrintPSDDetails(sourcePtr,ch);
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
    cvs_channel_fom->SetTickx();
    cvs_channel_fom->SetTicky();
    g1_fom_s_b->Draw("AP");
    
    cvs_channel_eff->cd();
    cvs_channel_eff->SetTickx();
    cvs_channel_eff->SetTicky();
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
        cvs_channel_fom->cd(ch+1)->SetTickx();
        cvs_channel_fom->cd(ch+1)->SetTicky();
        cvs_channel_fom->SetTickx();
        cvs_channel_fom->SetTicky();
        SetupGraphs(sourcePtr, ch);
        g1_fom_s_b->Draw("AP");
        
        cvs_channel_eff->cd(ch+1)->SetTickx();
        cvs_channel_eff->cd(ch+1)->SetTicky();
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
    
    double minXBin      = hist_signal.GetXaxis()->GetXmin();
    double maxXBin      = hist_signal.GetXaxis()->GetXmax();
    
    //Efficiency
    EfficiencyPlot eff(hist_signal);
    eff.Init();
    eff.CalculateUsingBinomial();
    //eff.CalculateUsingRoot();
    if(!g1_eff_s_b)delete g1_eff_s_b;
    g1_eff_s_b      = eff.GetGraphCopy();
    g1_eff_s_b->SetTitle("EFF");
    
    // Purity
    PurityPlot pur(hist_signal,hist_background);
    pur.Init();
    pur.CalculateUsingBinomial();
    //eff.CalculateUsingRoot();
    if(!g1_pur_s_b)delete g1_pur_s_b;
    g1_pur_s_b      = pur.GetGraphCopy();
    g1_pur_s_b->SetTitle("PUR");
    
    //FOM
    FOMPlot fom(pur, eff);
    fom.Init();
    fom.Calculate();
    if(!g1_fom_s_b)delete g1_fom_s_b;
    g1_fom_s_b      = fom.GetGraphCopy();
    g1_fom_s_b->SetTitle("FOM");
    
    std::cout   << "\n========= Channel " << channel << " ========="
                << "\nbest f.o.m value: "     << fom.GetBestValue() << " +- " << fom.GetBestValueError()
                << "\nfor long-short/long = " << fom.GetBestCut()   << " +- " << fom.GetXwidth() /2.0
                <<"\n=============================" <<std::endl;
    
    if(!g1_effXpur_s_b)delete g1_effXpur_s_b;
    g1_effXpur_s_b  = fom.GetGraphCopy();
    g1_effXpur_s_b->SetTitle("EffXPur");
    
    //Setup graphs
    g1_fom_s_b->SetTitle(Form("FOM for channel %i",channel));
    g1_fom_s_b->GetXaxis()->SetTitle("(QLong - QShort)/QLong");
    g1_fom_s_b->GetYaxis()->SetTitle("#epsilon^{s} / (#epsilon^{s} + #epsilon^{b})");
    g1_fom_s_b->GetYaxis()->SetTitleOffset(1.4);
    g1_fom_s_b->GetXaxis()->SetRangeUser(minXBin,maxXBin);
    g1_fom_s_b->GetXaxis()->SetRangeUser(-0.05,1.05);
    g1_fom_s_b->GetYaxis()->SetRangeUser(-0.05,1.05);
    g1_fom_s_b->SetMarkerStyle(24);
    g1_fom_s_b->SetMarkerSize(0.5);
    
    //graphs for efficencies and purities
    std::stringstream ss;
    ss << eff.GetXwidth();
    TString yAxisTitle = "#epsilon, #eta, #lambda / " + ss.str();
    g1_eff_s_b->SetTitle(Form("Efficiency and Purity for channel %i",channel));
    g1_eff_s_b->GetXaxis()->SetTitle("(QLong - QShort)/QLong");
    g1_eff_s_b->GetYaxis()->SetTitle(yAxisTitle);
    g1_eff_s_b->GetXaxis()->SetRangeUser(minXBin,maxXBin);
    g1_eff_s_b->GetXaxis()->SetRangeUser(-0.05,1.05);
    g1_eff_s_b->GetYaxis()->SetRangeUser(-0.05,1.05);
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

void Plotter::ResetCanvases()
{
    for(int i=0;i<canvases.size();++i)
    {
        delete canvases[i];
    }
    canvases.resize(0);
}

void Plotter::UpdateCanvases()
{
    for(int i=0;i<canvases.size();++i)
    {
        canvases[i]->SetTickx();
        canvases[i]->SetTicky();
        canvases[i]->Update();
    }
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

//normalise histograms by entries
void Plotter::NormaliseHistogram(TH1F& hist)
{  
    double factor = static_cast<double>(hist.GetEntries());
    hist.Scale(1.0/factor);
    //SetErrorBars(hist,factor);
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

void Plotter::ChangeRangeToFitBoth(TH1F& hist_signal, TH1F& hist_background)
{
    const double signalPeak     = hist_signal.GetBinContent(hist_signal.GetMaximumBin());
    const double backgroundPeak = hist_background.GetBinContent(hist_background.GetMaximumBin());
    if(backgroundPeak > signalPeak)
        hist_signal.GetYaxis()->SetRangeUser(0, 1.1*backgroundPeak);
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

void Plotter::PrintSpectraDetails(const std::shared_ptr<Source> sourcePtr, int channel)
{
    std::cout   << "\nChannel : " << channel << ", Temp: " << sourcePtr->GetTemperature()
    << ", Signal Entries: " << sourcePtr->GetSignalSpectraHistVtr().at(channel).GetEntries()
    << ", Signal Spectra Mean " << sourcePtr->GetSignalSpectraHistVtr().at(channel).GetMean()
    << ", Background Entries: " << sourcePtr->GetBackgroundSpectraHistVtr().at(channel).GetEntries()
    << ", Background Spectra Mean " << sourcePtr->GetBackgroundSpectraHistVtr().at(channel).GetMean();
}

void Plotter::PrintQShortDetails(const std::shared_ptr<Source> sourcePtr, int channel)
{
    std::cout   << "\nChannel : " << channel << ", Temp: " << sourcePtr->GetTemperature()
    << ", Signal Entries: " << sourcePtr->GetSignalQShortHistVtr().at(channel).GetEntries()
    << ", Signal Qshort Mean " << sourcePtr->GetSignalQShortHistVtr().at(channel).GetMean()
    << ", Background Entries: " << sourcePtr->GetBackgroundQShortHistVtr().at(channel).GetEntries()
    << ", Background Qshort Mean " << sourcePtr->GetBackgroundQShortHistVtr().at(channel).GetMean();
}

void Plotter::PrintPSDDetails(const std::shared_ptr<Source> sourcePtr, int channel)
{
    std::cout   << "\nChannel : " << channel << ", Temp: " << sourcePtr->GetTemperature()
    << ", Signal Entries: " << sourcePtr->GetSignalPsdHistVtr().at(channel).GetEntries()
    << ", Signal PSD Mean " << sourcePtr->GetSignalPsdHistVtr().at(channel).GetMean()
    << ", Background Entries: " << sourcePtr->GetBackgroundPsdHistVtr().at(channel).GetEntries()
    << ", Background PSD Mean " << sourcePtr->GetBackgroundPsdHistVtr().at(channel).GetMean();
}
