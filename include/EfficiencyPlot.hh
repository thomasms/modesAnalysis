#ifndef EFFICIENCY_PLOT_HH
#define EFFICIENCY_PLOT_HH

class EfficiencyPlot
{
    public:
        EfficiencyPlot(const TH1F& hist1, const TH1F& hist2);
        ~EfficiencyPlot();

    
    
    private:
        TH1F hist_signal;
        TH1F hist_total;
    
        TGraphAsymmErrors* graph;

};

