#include <iostream>
#include <string>

//PG CMSSW includes
#include "CaloOnlineTools/EcalTools/interface/EcalCosmicsTreeContent.h"
#include "CaloOnlineTools/EcalTools/interface/EcalCosmicsTreeUtils.h"
//#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

//PG root includes
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

//! main program
int main (int argc, char** argv)
{
  
  std::string outputRootName = "changeme.root" ;
  std::string testName = "-o";
  if (argc>2){
      if (argv[1] == testName) {
        outputRootName = argv[2] ;
      }
  }
  
  std::string testHelp = "--help";
  if (argc==2){
      if (argv[1] == testHelp) {
          std::cout << "Help" << std::endl ;
          std::cout << " --help : display help" << std::endl ;
          std::cout << " -o : output root file name (eg histograms.root)" << std::endl ;
          std::cout << " name of input file : list name of input files ntuples" << std::endl ;     
          exit(1);
      }
  }
    
  if (argc < 2)
    {
      std::cerr << "ERROR : ntuple name missing" << std::endl ;
      exit (1) ;
    }

  TH2F matchEffSC ("matchEffSC","matchEffSC",10,0,10,10,0,10) ;
  TH2F matchEffMU ("matchEffMU","matchEffMU",10,0,10,10,0,10) ;
           
  // Tree construction
  TChain * chain = new TChain ("EcalCosmicsAnalysis") ;
  
  if (argv[1] == testName) {
      for (int i=3; i< (argc); i++) chain->Add (argv[i]) ;
  }
  else{
      for (int i=1; i< (argc); i++) chain->Add (argv[i]) ;
  }
        
  //   chain->Add (argv[1]) ;

  EcalCosmicsTreeContent treeVars ; 
  setBranchAddresses (chain, treeVars) ;

  int nEntries = chain->GetEntries () ;
  std::cout << "FOUND " << nEntries << " ENTRIES\n" ;    

  //PG loop over entries
  for (int entry = 0 ; entry < nEntries ; ++entry)
    {
      chain->GetEntry (entry) ;

      std::vector<ect::association> associations ;
      ect::fillAssocVector (associations, treeVars) ;
      ect::selectOnDR (associations, treeVars, 0.3) ;

      matchEffSC.Fill (treeVars.nSuperClusters, associations.size ()) ;
      matchEffMU.Fill (treeVars.nRecoMuons, associations.size ()) ;


    } //PG loop over entries

  TFile saving (outputRootName.c_str (),"recreate") ;
  saving.cd () ;  
  matchEffSC.Write () ;
  matchEffMU.Write () ;
  saving.Close () ;

  return 0 ;
}

