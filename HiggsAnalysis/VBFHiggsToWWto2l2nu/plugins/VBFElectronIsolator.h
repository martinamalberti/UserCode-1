// $Id: VBFElectronIsolator.h,v 1.3 2008/07/09 14:13:35 govoni Exp $
#ifndef VBFElectronIsolator_h
#define VBFElectronIsolator_h

#include <memory>
#include <vector>
#include <math.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "HiggsAnalysis/VBFHiggsToWWto2l2nu/interface/VBFEleTrackerIsolationAlgo.h"

#include <TH1F.h>

class VBFElectronIsolator{

 public:

   explicit VBFElectronIsolator (const edm::ParameterSet&) ;
   ~VBFElectronIsolator () ;

   // Collections to be selected
//   typedef edm::View<reco::PixelMatchGsfElectron> electronCollection ;
   typedef edm::View<reco::PixelMatchGsfElectron> collection ;
   typedef reco::PixelMatchGsfElectronRef electronRef ;
   typedef edm::View<reco::Track> trackCollection ; 

//   typedef reco::PixelMatchGsfElectronCollection collection ;
   typedef std::vector<reco::PixelMatchGsfElectronRef> ::const_iterator const_iterator ;

   //define iterators with above typedef
   const_iterator begin () const { return m_selected.begin () ; }
   const_iterator end () const { return  m_selected.end () ; }

   void select (const edm::Handle<collection>, const edm::Event&, const edm::EventSetup&) ;
	
 private:	
 
   //! all the tracks
   edm::InputTag m_TrackInputTag ;

   //! the isolation algorithm
   VBFEleTrackerIsolationAlgo m_tkIsolationAlgo ;

   //! the isolation threshold
   double m_trackIsolationCut ; 

   //! isolated electrons   
   std::vector<reco::PixelMatchGsfElectronRef> m_selected ;

};

#endif
