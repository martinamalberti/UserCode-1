#ifndef VBFEleIDSelector_h 
#define VBFEleIDSelector_h

#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"

class VBFEleIDSelector  {
 public:
  explicit VBFEleIDSelector(const edm::ParameterSet&);
  ~VBFEleIDSelector();

  // Collections to be selected
  typedef reco::GsfElectronCollection collection;
  typedef std::vector<reco::GsfElectronRef> ::const_iterator const_iterator;

  //define iterators with above typedef
  const_iterator begin () const { return selected_.begin () ; }
  const_iterator end () const { return  selected_.end () ; }
 
  void select (edm::Handle<reco::GsfElectronCollection>,
               const edm::Event&, 
               const edm::EventSetup&) ;

  // ----------member data ---------------------------
 private:

  edm::InputTag electronIdCutsLabel_;
  edm::InputTag electronIdLikelihoodLabel_;
  bool useCuts_;
  double likelihoodThreshold_;

  std::vector<reco::GsfElectronRef> selected_ ;

};

#endif
