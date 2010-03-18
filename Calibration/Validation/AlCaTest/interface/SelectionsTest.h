#ifndef SelectionsTest_H
#define SelectionsTest_H 

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include <memory>
#include <string>
#include <iostream>
#include <algorithm>
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EgammaReco/interface/BasicClusterShapeAssociation.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TFile.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"


class SelectionsTest : public edm::EDAnalyzer {
  public:
    explicit SelectionsTest (const edm::ParameterSet&) ;
    ~SelectionsTest () {}
     virtual void analyze (const edm::Event& iEvent, 
                           const edm::EventSetup& iSetup) ;
     virtual void beginJob () ;
     virtual void endJob () ;

  private:

    edm::InputTag m_ElectronLabel ;
    std::string m_outputFileName ;
} ;
#endif
