// -*- C++ -*-
//
// Package:    VBFElePtFilter
// Class:      VBFElePtFilter
// 
/**\class VBFElePtFilter VBFElePtFilter.cc HiggsAnalysis/VBFElePtFilter/src/VBFElePtFilter.cc

 Description: <one line class summary>
 
 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Pietro Govoni
//         Created:  Wed Nov 14 17:32:25 CET 2007
// $Id: VBFElePtFilter.h,v 1.2 2008/12/31 16:39:31 govoni Exp $
//
//

#ifndef VBFElePtFilter_H
#define VBFElePtFilter_H

// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "AnalysisDataFormats/Egamma/interface/ElectronID.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronIDAssociation.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "HepMC/GenEvent.h"
#include "DataFormats/MuonReco/interface/Muon.h"

#include "DataFormats/Common/interface/TriggerResults.h"

#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"

//#include "DataFormats/HLTReco/interface/HLTFilterObject.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "DataFormats/TrackCandidate/interface/TrackCandidate.h"
#include "DataFormats/TrackCandidate/interface/TrackCandidateCollection.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"


#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "RecoEgamma/EgammaIsolationAlgos/interface/ElectronTkIsolation.h"
#include "RecoEgamma/EgammaIsolationAlgos/interface/EgammaHcalIsolation.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandAssociation.h"
#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"

#include "Geometry/Records/interface/IdealGeometryRecord.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "RecoCaloTools/MetaCollections/interface/CaloRecHitMetaCollections.h"

#include "RecoBTag/MCTools/interface/JetFlavourIdentifier.h"

//root include
#include <TTree.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TVector3.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TNtuple.h>


class VBFElePtFilter : public edm::EDFilter 
{
  public:
    
      typedef math::XYZTLorentzVector LorentzVector ;
      typedef std::vector<LorentzVector> LorentzVectorCollection ;
      typedef edm::View<reco::PixelMatchGsfElectron> electronCollection ;
      typedef edm::View<reco::Muon> muonCollection ;

      //! ctor
      explicit VBFElePtFilter (const edm::ParameterSet&);
      //! dtor
      ~VBFElePtFilter ();

   private:

      virtual void beginJob (const edm::EventSetup&);
      virtual bool filter (edm::Event&, const edm::EventSetup&);
      virtual void endJob ();

      template <class iterator, class handle>
      void findLeptons (iterator & firstIt, iterator & secondIt,
                        handle & inputColl)
        {
          switch (inputColl->size ())
            {
              case 0 :
                secondIt = firstIt = inputColl->end () ;
                break ;
              case 1 :
                firstIt = inputColl->begin () ;
                secondIt = inputColl->end () ;
                break ;
              default :
                double firstItMaxPt = 0 ;
                double secondItMaxPt = 0 ;
                //PG loop over leptons
                for (iterator leptIt = inputColl->begin () ; 
                     leptIt != inputColl->end () ; 
                     ++leptIt) 
                  {
                    if (firstItMaxPt < leptIt->pt ())
                      {
                        secondItMaxPt = firstItMaxPt ;
                        secondIt = firstIt ;
                        firstItMaxPt = leptIt->pt () ;
                        firstIt = leptIt ;
                      }
                    else if (secondItMaxPt < leptIt->pt ())
                      {
                        secondItMaxPt = leptIt->pt () ;
                        secondIt = leptIt ;
                      }  
                  } //PG loop over leptons 
            } //PG switch
          return ;
        }

   private:

      edm::InputTag m_GSFInputTag ;
      double m_firstPtMin ;
      double m_secondPtMin ;
      bool m_testCharge ;

} ;


#endif
