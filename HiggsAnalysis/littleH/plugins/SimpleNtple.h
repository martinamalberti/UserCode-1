// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "AnalysisDataFormats/Egamma/interface/ElectronID.h"
#include "AnalysisDataFormats/Egamma/interface/ElectronIDAssociation.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "PhysicsTools/NtupleUtils/interface/NtupleFactory.h"

#include <TTree.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TParticle.h>

#include "Math/PtEtaPhiE4D.h"
#include "Math/PtEtaPhiM4D.h"
#include "Math/LorentzVector.h"
#include "Math/Vector3D.h"

using namespace edm;
using namespace std;

//---------------------------
//---- class declaration ----
//---------------------------

class SimpleNtple : public edm::EDAnalyzer {
 public:
  explicit SimpleNtple(const edm::ParameterSet&);
  ~SimpleNtple();
  
 private:
  virtual void beginJob(const edm::EventSetup&) ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
    
  void fillVtxInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillMuInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillTrackInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillEleInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillSCInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillMCInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillTriggerInfo (const Event & iEvent, const EventSetup & iESetup) ;

  template <class T> math::XYZTLorentzVector lorentzMomentum(const T & muon) const;
  std::vector<unsigned int> trackHits(const reco::Track& tr);
    
  TTree* outTree_;
  NtupleFactory* NtupleFactory_;
    
  edm::InputTag TracksTag_;
  edm::InputTag EleTag_;
  edm::InputTag MuTag_;
  edm::InputTag PrimaryVertexTag_;        

  edm::InputTag MCtruthTag_;
  
  edm::InputTag m_eleIDCut_LooseInputTag ;
  edm::InputTag m_eleIDCut_RLooseInputTag ;
  edm::InputTag m_eleIDCut_TightInputTag ;
  edm::InputTag m_eleIDCut_RTightInputTag ;

  edm::InputTag barrelClusterCollection_;
  edm::InputTag endcapClusterCollection_;
  
  string thetriggerEventTag_;
  string theHLTriggerResults_;     // HLT trigger results
  string the8e29ProcName_;
  string the1e31ProcName_;
  
  static const unsigned int Max_trig_size = 10;
  static const int NTRIGGERS = 5;
  
  InputTag hltModules[2][NTRIGGERS]; // in order: L2, L3
  int hltBits[NTRIGGERS];
  
  bool saveVtx_ ;
  bool saveMu_ ;
  bool saveTracks_ ;
  bool saveEle_ ;
  bool saveMC_ ;
  bool saveSC_ ;
  bool saveTrigger_ ;

  int eventType_; //---- 0 = signal      1 = background 
  bool verbosity_; //---- true = loquacious     false = silence  

  //used to save all tracks BUT muons
  std::vector<int> theMuonTrkIndexes_ ;

  int Reco_mu_glb_size;
  int Reco_mu_trk_size;  
};
