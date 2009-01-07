// -*- C++ -*-
//
// Package:    VBFKinematics
// Class:      VBFKinematics
// 
/**\class VBFKinematics VBFKinematics.cc HiggsAnalysis/VBFKinematics/src/VBFKinematics.cc

 Description: <one line class summary>
 
 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Pietro Govoni
//         Created:  Wed Nov 14 17:32:25 CET 2007
// $Id: VBFKinematics.h,v 1.5 2009/01/06 15:27:19 govoni Exp $
//
//

#ifndef VBFKinematics_H
#define VBFKinematics_H

// system include files
#include <memory>
#include <string>
#include <functional>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "AnalysisDataFormats/Egamma/interface/ElectronID.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronIDAssociation.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
//#include "CLHEP/HepMC/GenEvent.h"
#include "HepMC/GenEvent.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

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
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"

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
#include <TH1.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TVector3.h>


class VBFKinematics : public edm::EDAnalyzer {

  public:
    
      typedef math::XYZTLorentzVector LorentzVector ;
      typedef std::vector<LorentzVector> LorentzVectorCollection ;

      //! ctor
      explicit VBFKinematics (const edm::ParameterSet&);
      //! dtor
      ~VBFKinematics ();

   private:

      virtual void beginJob (const edm::EventSetup&);
      virtual void analyze (const edm::Event&, const edm::EventSetup&);
      virtual void endJob ();

   private:

      edm::InputTag m_jetTagsInputTag ;
      edm::InputTag m_GSFInputTag ;
      edm::InputTag m_electronIDInputTag ;
      edm::InputTag m_muInputTag ;
      edm::InputTag m_metInputTag ;

      //PG before selections

      TH1F * m_ele_eta ;
      TH1F * m_mu_eta ;
      TH1F * m_lep_eta ;

      TH1F * m_met_phi ;
      TH1F * m_met_energy ;

      TH1F * m_jet_eta ;
      TH1F * m_jet_multiplicity ;
      TH1F * m_alljets_multiplicity ;

      TH1F * m_jet_maxEInEmTowers ;
      TH1F * m_jet_maxEInHadTowers ;
      
      TH1F * m_ele_pt ;
      TH1F * m_mu_pt ;
      
      TH1F * m_met_maxEInEmTowers ;
      TH1F * m_met_maxEInHadTowers ;

      //PG after selections

      TH1F * m_sel_maxPtLep_pt ;
      TH1F * m_sel_minPtLep_pt ;
      TH1F * m_sel_mu_multiplicity ;
      TH1F * m_sel_ele_multiplicity ;
      TH1F * m_sel_lept_invmass ;
      
    private:

      struct ptSorting : 
        public std::binary_function<const reco::RecoCandidate *,
                                    const reco::RecoCandidate *,bool>
        {
          bool operator () (const reco::RecoCandidate * first,
                            const reco::RecoCandidate * second)
            { 
              return (first->p4 ().Et () < second->p4 ().Et ()) ;
            }
        } ;                            


      template <class T> const T& min ( const T& a, const T& b ) 
        {
          return (a<b)?a:b;  
        }
};

#endif
