#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h> 
#include <vector>
#include <map>

#include <TApplication.h>
#include "TString.h"
#include "TSystem.h"

#include "dataLib.hh"
#include "dataCards.hh"
#include "dataCardsRecord.hh"
#include "Manager.hh"

int main(int argc, char ** argv){

  //create a stand alone appliction using root libraries - bypass the root arguments
  TApplication* rootapp = new TApplication("application",&argc, argv,0,-1);

  Manager * manager = new Manager();

  manager->InitDataCards();
  manager->GetRunTimeArguments(argc,argv);
  manager->LoadDataCards();
  
  manager->Initialise();
  
  manager->Process();
  manager->Plot();

  std::cout << "\n\n Done! Running in interactive mode, so Ctrl-C will exit  " << std::endl;
  rootapp->Run();


  delete manager;
  return 0;
}
