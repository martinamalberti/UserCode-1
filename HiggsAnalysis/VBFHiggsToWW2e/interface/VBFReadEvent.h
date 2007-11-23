// -*- C++ -*-
//
// Package:    VBFReadEvent
// Class:      VBFReadEvent
// 
/**\class VBFReadEvent VBFReadEvent.cc HiggsAnalysis/VBFReadEvent/src/VBFReadEvent.cc

 Description: <one line class summary>
 
 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Pietro Govoni
//         Created:  Wed Nov 14 17:32:25 CET 2007
// $Id: VBFReadEvent.h,v 1.10 2007/11/22 17:53:56 tancini Exp $
//
//

#ifndef VBFReadEvent_H
#define VBFReadEvent_H

// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "AnalysisDataFormats/Egamma/interface/ElectronID.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronIDAssociation.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "CLHEP/HepMC/GenEvent.h"

#include "DataFormats/Common/interface/TriggerResults.h"

#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"

#include "DataFormats/HLTReco/interface/HLTFilterObject.h"
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

//root include
#include <TTree.h>
#include <TFile.h>
#include <TLorentzVector.h>


class VBFReadEvent : public edm::EDAnalyzer {

  public:
    
      //! ctor
      explicit VBFReadEvent (const edm::ParameterSet&) ;
      //! dtor
      ~VBFReadEvent () ;

   private:

      virtual void beginJob (const edm::EventSetup&) ;
      virtual void analyze (const edm::Event&, const edm::EventSetup&) ;
      virtual void endJob () ;
      void setMomentum (TLorentzVector & vector, const reco::Candidate & gen) ;
      void findGenParticles (edm::Handle<CandidateCollection> &genParticles,
                                         TLorentzVector &m_genHiggs,
                                         TLorentzVector &m_genWm,
                                         TLorentzVector &m_genWp,
                                         TLorentzVector &m_genLepPlus,
                                         TLorentzVector &m_genLepMinus,
                                         TLorentzVector &m_genMetPlus,
                                         TLorentzVector &m_genMetMinus,
                                         TLorentzVector &m_genqTagF,
                                         TLorentzVector &m_genqTagB) ;
    

   private:

      edm::InputTag m_metInputTag ;
      edm::InputTag m_genMetInputTag ;
      edm::InputTag m_jetInputTag ;
      edm::InputTag m_genJetInputTag ;
      edm::InputTag m_GSFInputTag ;
      edm::InputTag m_electronIDInputTag ;
      edm::InputTag m_MCtruthInputTag ;
      edm::InputTag m_MC ;
      edm::InputTag m_muInputTag ;
      edm::InputTag m_trackInputTag ;
      edm::InputTag m_hcalRecHitProducer ;
      edm::InputTag m_emObjectProducer ;
    
      double m_ptMin ;
      double m_intRadius ;
      double m_extRadius ;
      double m_maxVtxDist ;

      double m_egHcalIsoPtMin ;
      double m_egHcalIsoConeSizeIn ; 
      double m_egHcalIsoConeSizeOut ;
    
      TFile *m_outfile;
      TTree* m_genTree;
      TLorentzVector *m_genHiggs;
      TLorentzVector *m_genWm;
      TLorentzVector *m_genWp;
      TLorentzVector *m_genLepPlus;
      TLorentzVector *m_genLepMinus;
      TLorentzVector *m_genMetPlus;
      TLorentzVector *m_genMetMinus;
      TLorentzVector *m_genqTagF;
      TLorentzVector *m_genqTagB;
      int m_LepPlusFlavour;
      int m_LepMinusFlavour;
} ;

#endif
