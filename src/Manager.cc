#include "Manager.hh"
#include "Plotter.hh"

Manager::Manager()
{
    _cards = nullptr;
    _handler.reset();
}

Manager::~Manager()
{
    CleanUp();
}

void Manager::InitDataCards() 
{
  _cards = dataCards::getInstance();
  _cards->AddDataCardString("signalFile", "../rootfiles/fastNeutron_tests/PuBe-det23_");
  _cards->AddDataCardString("signalName","signal"); 
  _cards->AddDataCardString("backgroundFile","../rootfiles/fastNeutron_tests/60co-det23_");
  _cards->AddDataCardString("backgroundName","background"); 
  _cards->AddDataCardInt("files", 1);
  _cards->AddDataCardString("saveDir", "./images");
  _cards->AddDataCardDouble("timeCutOff", 300.0);
  _cards->AddDataCardDouble("psdCut", 0.64);
  _cards->AddDataCardDouble("contaminationUpperLevel", 0.20);
  _cards->AddDataCardDouble("contaminationLowerLevel", 0.10);
  _cards->AddDataCardBool("showBackground", false);
  _cards->AddDataCardBool("showPeaks", false);
  _cards->AddDataCardBool("savePlots", false);
  _cards->AddDataCardBool("logPlot", false);
  _cards->AddDataCardBool("shiftToPsdSignalPeak", false);
  _cards->AddDataCardBool("sameNrOfEventsPerTube", false);
  _cards->AddDataCardInt("numberOfExperiments", 100);
  _cards->AddDataCardInt("meanEvents", 500);
  _cards->AddDataCardInt("tubes", 4);
  _cards->AddDataCardInt("channel", -1);
  _cards->AddDataCardInt("binning", 4);
  _cards->AddDataCardDouble("temperature", 20.);    //deg C
  _cards->AddDataCardDouble("pressure", 1000.);     //hPa
  _cards->AddDataCardDouble("activity", 500.);      //kBq
}

void Manager::LoadDataCards() 
{
  _signalFile               = _cards->fetchValueString("signalFile");
  _signalName               = _cards->fetchValueString("signalName");
  _backgroundFile           = _cards->fetchValueString("backgroundFile");
  _backgroundName           = _cards->fetchValueString("backgroundName");
  _files                    = _cards->fetchValueInt("files");
  _saveDir                  = _cards->fetchValueString("saveDir");
  _timeCutOff               = _cards->fetchValueDouble("timeCutOff");
  _psdCut                   = _cards->fetchValueDouble("psdCut");
  _contaminationUpperLevel 	= _cards->fetchValueDouble("contaminationUpperLevel");
  _contaminationLowerLevel 	= _cards->fetchValueDouble("contaminationLowerLevel");
  _showBackground           = _cards->fetchValueBool("showBackground");
  _showPeaks                = _cards->fetchValueBool("showPeaks");
  _savePlots                = _cards->fetchValueBool("savePlots");
  _logPlot                  = _cards->fetchValueBool("logPlot");
  _shiftToPsdSignalPeak     = _cards->fetchValueBool("shiftToPsdSignalPeak");
  _sameNrOfEventsPerTube    = _cards->fetchValueBool("sameNrOfEventsPerTube");
  _experiments              = _cards->fetchValueInt("numberOfExperiments");
  _meanNoEvents             = _cards->fetchValueInt("meanEvents");
  _tubes                    = _cards->fetchValueInt("tubes");
  _channel                  = _cards->fetchValueInt("channel");
  _binning                  = _cards->fetchValueInt("binning");
  _temperature              = _cards->fetchValueDouble("temperature");
  _pressure                 = _cards->fetchValueDouble("pressure");
  _activity                 = _cards->fetchValueDouble("activity");
}

void Manager::GetRunTimeArguments(int argc, char ** argv) 
{
    for (int i=1;i<argc;i++)
    {
        if (argv[i])
        {
            std::string cardsfile(argv[i]);
            _cards->readKeys(cardsfile);
        }
        _cards->printToStream(std::cout);
    }
}

void Manager::Initialise()
{
    CleanUp();

    _handler = std::make_shared<Handler>(_signalName,_backgroundName,_binning,_temperature,_pressure);

    //convert the strings to TStrings
    TString signalFileName = _signalFile;
    TString backgrFileName = _backgroundFile;

    //load the files - loop over all in directory with similar names
    for(int i=1;i<=_files;i++)
    {
        AddFileToSignal(signalFileName,i);
        AddFileToBackground(backgrFileName,i);
    }
}

void Manager::Process()
{
    _handler->RequireSameNrOfEventsPerTube(_sameNrOfEventsPerTube);
    _handler->Process(_signalTreeVtr,true,_timeCutOff,_shiftToPsdSignalPeak);
    _handler->Process(_backgrTreeVtr,false,_timeCutOff,_shiftToPsdSignalPeak);
    _handler->SetupSource();
}

void Manager::Plot()
{
    Plotter plotter(_channel, _savePlots, _showPeaks);
    plotter.DrawSpectra(_handler->GetSourcePtr(),_showBackground);
    plotter.DrawQShort(_handler->GetSourcePtr(),_showBackground);
    plotter.DrawPsd(_handler->GetSourcePtr(),_showBackground);
    plotter.DrawFOM(_handler->GetSourcePtr());
    plotter.UpdateCanvases();
}

void Manager::AddFileToSignal(const TString fileToAdd, int index)
{
    std::stringstream ss;
    ss << index;

    TString filename ="";
    if(_files==1) filename = fileToAdd;
    else if(index<10) filename = fileToAdd +"0" + ss.str()+ ".root";
    else filename = fileToAdd + ss.str()+ ".root";

    ifstream ifile(filename);
    if (!ifile)
    {
      std::cout << "\nSignal file(s): " << filename << " does not exist! Not added." <<std::endl;
      _signalTFile = nullptr;
      _signalTTree = nullptr;
    }
    else
    {
      _signalTFile = new TFile(filename);
      std::cout << "\nSignal file added: " << filename;
      _signalTTree = new TTree();
      _signalTTree = (TTree*)_signalTFile->Get("acq_tree_0");
    }

    //add to the signal file and tree vectors
    _signalTreeVtr.push_back(_signalTTree);
    _signalFileVtr.push_back(_signalTFile);
}

void Manager::AddFileToBackground(TString fileToAdd, int index)
{
    std::stringstream ss;
    ss << index;

    TString filename ="";
    if(_files==1) filename = fileToAdd;
    else if(index<10) filename = fileToAdd +"0" + ss.str()+ ".root";
    else filename = fileToAdd + ss.str()+ ".root";

    ifstream ifile(filename);
    if (!ifile)
    {
    	std::cout << "\nBackground file(s): " << filename << " does not exist! Not added."<<std::endl;
        _backgrTFile = nullptr;
    }
    else
    {
        _backgrTFile = new TFile(filename);
        std::cout << "\nBackground file added: " << filename<<std::endl;
        _backgrTTree = new TTree();
        _backgrTTree = (TTree*)_backgrTFile->Get("acq_tree_0");
    }
    
    //add to the vectors
    _backgrTreeVtr.push_back(_backgrTTree);
    _backgrFileVtr.push_back(_backgrTFile);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                          Private Methods                                       /////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Manager::RemoveFileFromSignal(int index)
{
  TTree * tmpTreePtr = _signalTreeVtr.at(index);
  delete tmpTreePtr;
  tmpTreePtr = nullptr;

  TFile  * tmpFilePtr = _signalFileVtr.at(index);
  delete tmpFilePtr;
  tmpFilePtr = nullptr;
}

void Manager::RemoveFileFromBackground(int index)
{
  TTree * tmpTreePtr = _backgrTreeVtr.at(index);
  delete tmpTreePtr;
  tmpTreePtr = nullptr;

  TFile  * tmpFilePtr = _backgrFileVtr.at(index);
  delete tmpFilePtr;
  tmpFilePtr = nullptr;
}

void Manager::CleanUp()
{
  for(int i=0;i<_signalFileVtr.size();i++)
    {
      this->RemoveFileFromSignal(i);
    }

  for(int i=0;i<_backgrFileVtr.size();i++)
    {
      this->RemoveFileFromBackground(i);
    }

  _signalFileVtr.resize(0);
  _backgrFileVtr.resize(0);
}

