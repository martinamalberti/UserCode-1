// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TTree.h"
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include "TParticle.h"


#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETFwd.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METFwd.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETFwd.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include "DataFormats/Common/interface/ValueMap.h"

#include "PhysicsTools/NtupleUtils/interface/NtupleFactory.h"
#include "PhysicsTools/NtupleUtils/interface/MCDumper.h"

#include "Math/PtEtaPhiE4D.h"
#include "Math/PtEtaPhiM4D.h"
#include "Math/LorentzVector.h"
#include "Math/Vector3D.h"
#include "Math/GenVector/VectorUtil.h"


//---------------------------
//---- class declaration ----
//---------------------------

class SimpleNtpleTTBar : public edm::EDAnalyzer {
 public:
  typedef edm::AssociationMap<edm::OneToValue<std::vector<reco::GsfElectron>, float> > eleMap;
  typedef edm::AssociationMap<edm::OneToValue<std::vector<reco::Muon>, float> > muMap;

  explicit SimpleNtpleTTBar(const edm::ParameterSet&);
  ~SimpleNtpleTTBar();
  
 private:
  virtual void beginJob(const edm::EventSetup&) ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
    
  void fillMuInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillTrackInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillEleInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillJetInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillPFJetInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillJetBTaggingInfo(const edm::Event & iEvent, const edm::EventSetup & iESetup,
                           const ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >& v1);
  void fillMetInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillGenJetInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillGenMetInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  void fillMCInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) ;
  
  TTree* outTree_;
  NtupleFactory* NtupleFactory_;
    
  
  ///---- input tag ----

  edm::InputTag TracksTag_;
  edm::InputTag EleTag_;
  edm::InputTag Ele3DipSignificanceTag_;
  edm::InputTag EleTipSignificanceTag_;
  edm::InputTag EleLipSignificanceTag_;
  edm::InputTag MuTag_;
  edm::InputTag Mu3DipSignificanceTag_;
  edm::InputTag MuTipSignificanceTag_;
  edm::InputTag MuLipSignificanceTag_;
  edm::InputTag MetTag_;
  edm::InputTag Type1MetTag_;
  edm::InputTag PFMetTag_;
  edm::InputTag JetTag_;
  edm::InputTag MCtruthTag_;
  edm::InputTag genJetTag_;
  edm::InputTag genMetTag_;
  
  edm::InputTag eleIDCut_LooseInputTag_ ;
  edm::InputTag eleIDCut_RLooseInputTag_ ;
  edm::InputTag eleIDCut_TightInputTag_ ;
  edm::InputTag eleIDCut_RTightInputTag_ ;

  bool doEleRefCheck_;
  edm::InputTag EleRefTag_;
  bool doMuRefCheck_;
  edm::InputTag MuRefTag_;
  bool doJetRefCheck_;
  edm::InputTag JetRefTag_;

  bool saveMu_ ;
  bool saveTrack_ ;
  bool saveEle_ ;
  bool saveJet_ ;
  bool savePFJet_ ;
  bool saveJetBTagging_ ;
  bool saveMet_ ;
  bool saveGenJet_ ;
  bool saveGenMet_ ;
  bool saveMC_ ;
 
  int eventType_; //---- 0 = signal      1 = background 
  bool verbosity_; //---- true = loquacious     false = silence  
   
};

