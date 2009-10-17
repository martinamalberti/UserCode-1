#ifndef VBFEtMinJetCountFilter_h
#define VBFEtMinJetCountFilter_h

#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/PFJet.h"






template <class TCollection>
class VBFEtMinJetCountFilter : public edm::EDFilter
{
  
 public:
  
  //! ctor
  explicit VBFEtMinJetCountFilter (const edm::ParameterSet&);
  
  //! dtor 
  ~VBFEtMinJetCountFilter();
  
  
  
 private:
  
  void beginJob(const edm::EventSetup&);
  void endJob();
  
  //! the actual filter method 
  bool filter(edm::Event&, const edm::EventSetup&);
  
  
  
 private:
  
  edm::InputTag m_srcJets;
  
  double m_etMin;
  double m_etaMin;
  double m_etaMax;
  int m_minNumber;
  
};

#endif






//! ctor
template <class TCollection>
VBFEtMinJetCountFilter<TCollection>::VBFEtMinJetCountFilter(const edm::ParameterSet& iConfig): 
  m_srcJets  (iConfig.getParameter<edm::InputTag>("srcJets")),
  m_etMin    (iConfig.getParameter<double>("etMin")),
  m_etaMin   (iConfig.getParameter<double>("etaMin")), 
  m_etaMax   (iConfig.getParameter<double>("etaMax")),
  m_minNumber(iConfig.getParameter<int>("minNumber"))
{}

// ----------------------------------------------------------------






//! dtor
template <class TCollection>
VBFEtMinJetCountFilter<TCollection>::~VBFEtMinJetCountFilter()
{}

// ----------------------------------------------------------------






template <class TCollection>
void VBFEtMinJetCountFilter<TCollection>::beginJob(const edm::EventSetup&) 
{}

// ----------------------------------------------------------------






template <class TCollection>
void VBFEtMinJetCountFilter<TCollection>::endJob() 
{}

// ----------------------------------------------------------------






//! loop over the reco particles and count jets
template <class TCollection>
bool VBFEtMinJetCountFilter<TCollection>::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) 
{
  edm::Handle<TCollection> jets;
  iEvent.getByLabel(m_srcJets, jets);
  
  
  
  int nSelected = 0;
  
  
  //PG loop over jets
  for(unsigned int iJet = 0; iJet < jets -> size(); ++iJet)
  {
    if( (jets -> at(iJet).eta() > m_etaMin) &&
        (jets -> at(iJet).eta() < m_etaMax) &&
        (jets -> at(iJet).et () > m_etMin) )
      ++nSelected;
  } //PG loop over jets
  
  
  if(nSelected >= m_minNumber) return true;
  return false;
}
