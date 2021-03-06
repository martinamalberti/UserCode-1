
#include "EcalAnalysis/ConversionStudies/plugins/conversionTree.h"

// system include files
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include <memory>
#include <vector>
#include <map>
#include <set>

#include "Math/Vector4D.h"
#include "Math/Vector3D.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

//----objects----
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElement.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/Particle.h"

#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"

//----geometry----
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoTracker/Record/interface/TrackerRecoGeometryRecord.h"

#include "TrackingTools/GeomPropagators/interface/AnalyticalPropagator.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/GsfTools/interface/GsfPropagatorAdapter.h"
#include "TrackingTools/GsfTools/interface/MultiTrajectoryStateTransform.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"

//----refit----
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "RecoVertex/PrimaryVertexProducer/interface/PrimaryVertexSorter.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/GaussianSumVertexFit/interface/GsfVertexFitter.h"
#include "RecoVertex/GaussianSumVertexFit/interface/AdaptiveGsfVertexFitter.h"
#include "RecoTracker/TrackProducer/plugins/GsfTrackRefitter.h"

#include "RecoVertex/PrimaryVertexProducer/interface/VertexHigherPtSquared.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

using namespace reco;

conversionTree::conversionTree (const edm::ParameterSet& iConfig):
  //---- flags ----
  TriggerEventTag_ (iConfig.getParameter<edm::InputTag>("TriggerEventTag")),
  TriggerResultsTag_ (iConfig.getParameter<edm::InputTag>("TriggerResultsTag")),
  PVTag_          (iConfig.getParameter<edm::InputTag>("PVTag")),
  MCtruthTag_     (iConfig.getParameter<edm::InputTag>("MCtruthTag")),
  PhotonTag_      (iConfig.getParameter<edm::InputTag>("PhotonTag")),
  EBRechitTag_    (iConfig.getParameter<edm::InputTag>("EBRechitTag")),
  EERechitTag_    (iConfig.getParameter<edm::InputTag>("EERechitTag")),

  savePV_        (iConfig.getUntrackedParameter<bool> ("savePV", true)),
  saveHLT_ (iConfig.getUntrackedParameter<bool> ("saveHLT", true)),
  saveConversion_ (iConfig.getUntrackedParameter<bool> ("saveConversion", true)),
  savePhoton_    (iConfig.getUntrackedParameter<bool> ("savePhoton", true)),
  savePFPhoton_ (iConfig.getUntrackedParameter<bool> ("savePFPhoton", true)),
  savePFCluster_ (iConfig.getUntrackedParameter<bool> ("savePFCluster", true)),
  saveIslandCluster_ (iConfig.getUntrackedParameter<bool> ("saveIslandCluster", false)),
  saveMC_ (iConfig.getUntrackedParameter<bool> ("saveMC", false)),
  saveMCPi0_  (iConfig.getUntrackedParameter<bool> ("saveMCPi0", false)),


  verbosity_ (iConfig.getUntrackedParameter<bool>("verbosity", false)),
  eventType_ (iConfig.getUntrackedParameter<int>("eventType", 1))
{
  edm::Service<TFileService> fs ;
  outTree_  = fs->make <TTree>("SimpleTree","SimpleTree"); 

  NtupleFactory_ = new NtupleFactory(outTree_);

  mcAnalysisPi0_ = NULL;

}


conversionTree::~conversionTree ()
{
  delete NtupleFactory_;
}



///-------------
///---- HLT ----

void conversionTree::fillHLTInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
  //---- HLT information ----
  edm::Handle<trigger::TriggerEvent> triggerEventHandle;
  iEvent.getByLabel(TriggerEventTag_, triggerEventHandle);
  const edm::Provenance* provenance = triggerEventHandle.provenance();
  //std::cout << "Trigger process name = " << provenance->processName() << std::endl;
  
  edm::Handle<edm::TriggerResults> triggerResultsHandle;
  iEvent.getByLabel(edm::InputTag(TriggerResultsTag_.label(), TriggerResultsTag_.instance(), provenance->processName()), triggerResultsHandle);
  const edm::TriggerNames& triggerNames = iEvent.triggerNames(*triggerResultsHandle);
  for(unsigned int iHLT = 0; iHLT < triggerResultsHandle->size(); ++iHLT)
  {
    //std::cout << "bit: " << std::fixed << setw(3)<< iHLT << "   name: " << triggerNames.triggerName(iHLT) << std::endl;
    
    if( triggerResultsHandle -> wasrun(iHLT) )
      NtupleFactory_ -> FillFloat("HLT_WasRun", 1);
    else
      NtupleFactory_ -> FillFloat("HLT_WasRun", 0);
    
    if( triggerResultsHandle -> accept(iHLT) )
      NtupleFactory_ -> FillFloat("HLT_Accept", 1);
    else
      NtupleFactory_ -> FillFloat("HLT_Accept", 0);
    
    if( triggerResultsHandle -> error(iHLT) )
      NtupleFactory_->FillFloat("HLT_Error", 1);
    else
      NtupleFactory_->FillFloat("HLT_Error", 0);
    
    NtupleFactory_ -> FillString("HLT_Names", triggerNames.triggerName(iHLT));
  }
}



///---- conversions ----
void conversionTree::fillConversionInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
  //std::cout << "fillConversionInfo" << std::endl;

  edm::Handle<reco::ConversionCollection> pConversionTkOnly ;
  iEvent.getByLabel ("trackerOnlyConversions", pConversionTkOnly) ;

  for (reco::ConversionCollection::const_iterator itr = pConversionTkOnly->begin () ;
       itr != pConversionTkOnly->end () ;
       ++itr)
    {

      //=====test refit gdf vertex=====
//       ESHandle<TransientTrackBuilder> theB;
//       iESetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

//       std::vector<reco::TransientTrack> ttks;
//       for (std::vector<reco::TrackRef>::const_iterator trkItr = convTracks.begin(); trkItr != convTracks.end(); ++ trkItr)
// 	{
// 	  reco::TransientTrack ttk = (*theB).build(&(*trkItr));
// 	  ttks.push_back(ttk);
// 	}

//       GsfVertexFitter gsf(gsfPSet_);
//       TransientVertex tv = gsf.vertex(ttks);

//       std::vector<reco::TransientTrack> refit_tracks;
//       bool has_refit = false;
//       if (tv.isValid())
// 	{
// 	  refit_tracks = tv.refittedTracks();
// 	  if (refit_tracks.size()==2)
// 	    has_refit = true;
// 	}
      //=====end test refit gdf vertex=====


      std::vector<edm::RefToBase<reco::Track> > origTks = itr->tracks();
      const reco::Vertex& origV =  itr->conversionVertex();
      std::vector<reco::Track> refitTks;

      bool has_refit = false;
      if (origV.isValid())
	{
	  refitTks = origV.refittedTracks();
	  if (refitTks.size() == 2)
	    has_refit = true;
	}


      NtupleFactory_->FillFloat("pairHasRefit",has_refit);
      if ( has_refit == true )
	{
	  reco::Track& refitTk1 = refitTks.front();
	  reco::Track& refitTk2 = refitTks.back();
	  
	  math::XYZVector refitPhotonMom = (refitTk1.momentum() + refitTk2.momentum());
	  NtupleFactory_->Fill3V("pairMomentum_refit",refitPhotonMom);
	  NtupleFactory_->Fill3V("pairMomentumFirstTrack_refit",refitTk1.momentum());
	  NtupleFactory_->Fill3V("pairMomentumSecondTrack_refit",refitTk2.momentum());

	}
	  
      
      NtupleFactory_->FillFloat("pairNTracks",itr->nTracks());
      NtupleFactory_->FillFloat("pairIsConverted",itr->isConverted());

      NtupleFactory_->FillFloat("pairInvariantMass",itr->pairInvariantMass());
      
      math::XYZVector myvect_XYZ(itr->pairMomentum().x(),itr->pairMomentum().y(),itr->pairMomentum().z());
      NtupleFactory_->Fill3V("pairMomentum",myvect_XYZ);
      if(myvect_XYZ.eta() >  1.479)
	NtupleFactory_->FillFloat("pairType",+1.);
      else if(myvect_XYZ.eta() < -1.479)
	NtupleFactory_->FillFloat("pairType",-1.);
      else
	NtupleFactory_->FillFloat("pairType",0.);

      NtupleFactory_->FillFloat("pairZOfPrimaryVertexFromTracks",itr->zOfPrimaryVertexFromTracks());

      NtupleFactory_->FillFloat("pairDistOfMinimumApproach",itr->distOfMinimumApproach());

      NtupleFactory_->FillFloat("pairDPhiTracksAtVertex",itr->dPhiTracksAtVtx());

      NtupleFactory_->Fill3V("pairFirstTrackPIn",itr->tracksPin()[0]);
      NtupleFactory_->Fill3V("pairFirstTrackPOut",itr->tracksPout()[0]);

      math::XYZVector firstInnPos(itr->tracksInnerPosition()[0].x(),itr->tracksInnerPosition()[0].y(),itr->tracksInnerPosition()[0].z());
      NtupleFactory_->Fill3V("pairFirstTrackInnerPosition",firstInnPos);

      if(itr->nTracks() == 2)
	{
	  NtupleFactory_->Fill3V("pairSecondTrackPIn",itr->tracksPin()[1]);
	  NtupleFactory_->Fill3V("pairSecondTrackPOut",itr->tracksPout()[1]);
	  
	  math::XYZVector secondInnPos(itr->tracksInnerPosition()[1].x(),itr->tracksInnerPosition()[1].y(),itr->tracksInnerPosition()[1].z());
	  NtupleFactory_->Fill3V("pairSecondTrackInnerPosition",secondInnPos);
	}
    }
}

///-----------------
///---- Photons ----

void conversionTree::fillPhotonInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
 //std::cout << "SimpleNtuple::fillPhotonInfo" << std::endl;
 
  edm::Handle<reco::PhotonCollection> photonHandle;
 iEvent.getByLabel(PhotonTag_,photonHandle);
 
 
 for (reco::PhotonCollection::const_iterator itr = photonHandle->begin () ;
      itr != photonHandle->end () ;
      ++itr)
   {
     reco::Photon photon = *itr;
  
  NtupleFactory_ -> Fill4V   ("photons", photon.p4());
  
  NtupleFactory_ -> FillInt  ("photons_isGap",photon.isEBEtaGap() || photon.isEBPhiGap() || photon.isEERingGap() || photon.isEEDeeGap() || photon.isEBEEGap() );
  NtupleFactory_ -> FillFloat("photons_e1x5",photon.e1x5());           
  NtupleFactory_ -> FillFloat("photons_e2x5",photon.e2x5());         
  NtupleFactory_ -> FillFloat("photons_e3x3",photon.e3x3());         
  NtupleFactory_ -> FillFloat("photons_e5x5",photon.e5x5());         
  NtupleFactory_ -> FillFloat("photons_maxEnergyXtal",photon.maxEnergyXtal());
  NtupleFactory_ -> FillFloat("photons_sigmaEtaEta",photon.sigmaEtaEta());  
  NtupleFactory_ -> FillFloat("photons_sigmaIetaIeta",photon.sigmaIetaIeta());
  NtupleFactory_ -> FillFloat("photons_r1x5",photon.r1x5());        
  NtupleFactory_ -> FillFloat("photons_r2x5",photon.r2x5());        
  NtupleFactory_ -> FillFloat("photons_r9",photon.r9());   
  NtupleFactory_ -> FillFloat("photons_ecalIso",photon.ecalRecHitSumEtConeDR04());   
  NtupleFactory_ -> FillFloat("photons_hcalIso",photon.hcalTowerSumEtConeDR04());   
  NtupleFactory_ -> FillFloat("photons_hadronicOverEm",photon.hadronicOverEm());   
  NtupleFactory_ -> FillFloat("photons_trkSumPtHollowConeDR04",photon.trkSumPtHollowConeDR04());   
  NtupleFactory_ -> FillInt  ("photons_hasPixelSeed",photon.hasPixelSeed());   
  

  //superCluster Info
  reco::SuperClusterRef phoSC = photon.superCluster();
  
  double pos = sqrt(phoSC->x()*phoSC->x() + phoSC->y()*phoSC->y() + phoSC->z()*phoSC->z());
  double ratio = phoSC->energy() / pos;
  ROOT::Math::XYZTVector phoVec(phoSC->x()*ratio, phoSC->y()*ratio, phoSC->z()*ratio, phoSC->energy());
  NtupleFactory_ -> Fill4V("photons_SC", phoVec);


  //recHit time and energy
  TMatrix rechitTime(3,3);
  TMatrix rechitE(3,3);
  for (int i=0;i<3;i++){
    for (int j=0; j< 3; j++){
      rechitTime[i][j]=999;
      rechitE[i][j]=-999;
    }
  }

   // calo topology
  edm::ESHandle<CaloTopology> pTopology;
  iESetup.get<CaloTopologyRecord>().get(pTopology);
  const CaloTopology *topology = pTopology.product();
 // Ecal barrel RecHits 
  edm::Handle<EcalRecHitCollection> pBarrelEcalRecHits ;
  iEvent.getByLabel (EBRechitTag_, pBarrelEcalRecHits) ;
  const EcalRecHitCollection* theBarrelEcalRecHits = pBarrelEcalRecHits.product () ;

  edm::Handle<EcalRecHitCollection> pEndcapEcalRecHits ;
  iEvent.getByLabel (EERechitTag_, pEndcapEcalRecHits) ;
  const EcalRecHitCollection* theEndcapEcalRecHits = pEndcapEcalRecHits.product () ;


  if( photon.isEB() ){
    EBDetId ebid = (EcalClusterTools::getMaximum( photon.superCluster()->hitsAndFractions(), theBarrelEcalRecHits )).first;
    for(int xx = 0; xx < 3; ++xx)
      for(int yy = 0; yy < 3; ++yy)
	{
	  std::vector<DetId> vector =  EcalClusterTools::matrixDetId(topology, ebid, xx-1, xx-1, yy-1, yy-1);
	  if(vector.size() == 0) continue;
	  EcalRecHitCollection::const_iterator iterator = theBarrelEcalRecHits->find (vector.at(0)) ;
	  if(iterator == theBarrelEcalRecHits->end()) continue;
	  rechitE[xx][yy]  = iterator -> energy();
	  rechitTime[xx][yy]  = iterator -> time();
	}
    
  }
  else if (  photon.isEE() ){
    EEDetId ebid = (EcalClusterTools::getMaximum( photon.superCluster()->hitsAndFractions(), theEndcapEcalRecHits )).first;
    for(int xx = 0; xx < 3; ++xx)
      for(int yy = 0; yy < 3; ++yy)
	{
	  std::vector<DetId> vector =  EcalClusterTools::matrixDetId(topology, ebid, xx-1, xx-1, yy-1, yy-1);
	  if(vector.size() == 0) continue;
	  EcalRecHitCollection::const_iterator iterator = theEndcapEcalRecHits->find (vector.at(0)) ;
	  if(iterator == theEndcapEcalRecHits->end()) continue;
	  rechitE[xx][yy]  = iterator -> energy();
	  rechitTime[xx][yy]  = iterator -> time();
	}
  }
  

  NtupleFactory_ -> FillTMatrix("photons_rechitTime",rechitTime);
  NtupleFactory_ -> FillTMatrix("photons_rechitE",rechitE);

 }
 
}


///---- PF photons
void conversionTree::fillPFPhotonInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{

  //std::cout << "fillPFPhotonInfo" << std::endl;

  edm::Handle<reco::PFCandidateCollection> pPFCandidate ;
  //iEvent.getByLabel (edm::InputTag("particleFlow","","RERECO"), pPFCandidate) ;
  iEvent.getByLabel ("particleFlow", pPFCandidate) ;

  edm::Handle<EcalRecHitCollection> pEBrecHit ;
  edm::InputTag pEBrecHitTag("ecalRecHit","EcalRecHitsEB");
  iEvent.getByLabel (pEBrecHitTag, pEBrecHit) ;

  edm::Handle<EcalRecHitCollection> pEErecHit ;
  edm::InputTag pEErecHitTag("ecalRecHit","EcalRecHitsEE");
  iEvent.getByLabel (pEErecHitTag, pEErecHit) ;

  int xtalIndex = 0;
  for (reco::PFCandidateCollection::const_iterator PFItr = pPFCandidate->begin () ;
       PFItr != pPFCandidate->end () ;
       PFItr++)
    {
      reco::PFCandidate::ParticleType id = PFItr->particleId();
      if (id != reco::PFCandidate::gamma) continue;             //photon selection

      NtupleFactory_->FillFloat("PFPhotonEnergy",PFItr->energy());
      NtupleFactory_->FillFloat("PFPhotonPositionEta",PFItr->eta());
      NtupleFactory_->FillFloat("PFPhotonPositionPhi",PFItr->phi());
      NtupleFactory_->Fill4V("PFPhoton",PFItr->p4());
      if(PFItr->eta() > 1.479)
	NtupleFactory_->FillFloat("PFPhotonType",+1.);
      else if(PFItr->eta() < -1.479)
	NtupleFactory_->FillFloat("PFPhotonType",-1.);
      else
	NtupleFactory_->FillFloat("PFPhotonType",0.);
	

      //saving clusters associated to the candidate
      //std::cout << "--->>> start loop over blocks" << std::endl;
      reco::PFClusterRef myCluster;
      const reco::PFCandidate::ElementsInBlocks& theElements = PFItr->elementsInBlocks();
      for (reco::PFCandidate::ElementsInBlocks::const_iterator ieb = theElements.begin();
	   ieb < theElements.end();
	   ++ieb) 
	{
	  const reco::PFBlock& block = *(ieb->first);
	  const reco::PFBlockElement& elem = block.elements()[ieb->second];
	  
	  //only ecal blocks (inutile per fotoni...)
	  reco::PFBlockElement::Type type = elem.type();
	  if(type != reco::PFBlockElement::ECAL) continue;
	  
	  //Filling cluster variables
	  myCluster = elem.clusterRef();  //quando trovo il blocco associato esco! (ce n'e' uno solo per candidate, ho controllato!)
	  break;
	}

      if(!myCluster.isAvailable()) continue;  //puo' essere che non lo trovo il blocco???
      double pos = sqrt(myCluster->x()*myCluster->x() + myCluster->y()*myCluster->y() + myCluster->z()*myCluster->z());
      double ratio = myCluster->energy() / pos;
      
      NtupleFactory_->FillFloat("PFPhotonClusterEnergy",myCluster->energy());
      
      math::XYZVector myvect_XYZ(myCluster->x(),myCluster->y(),myCluster->z());
      NtupleFactory_->Fill3V("PFPhotonClusterPosition",myvect_XYZ);
      
      ROOT::Math::XYZTVector clusVect(ratio*myCluster->x(),ratio*myCluster->y(),ratio*myCluster->z(),myCluster->energy());
      NtupleFactory_->Fill4V("PFPhotonCluster",clusVect);
      
      EcalClusterLazyTools lazyTools( iEvent, iESetup, pEBrecHitTag, pEErecHitTag);
      NtupleFactory_->FillFloat("PFPhotonClusterE3x3", lazyTools.e3x3(*myCluster));
      NtupleFactory_->FillFloat("PFPhotonClusterE5x5", lazyTools.e5x5(*myCluster));
      NtupleFactory_->FillFloat("PFPhotonClusterEMax", lazyTools.eMax(*myCluster));
      NtupleFactory_->FillFloat("PFPhotonClusterE2nd", lazyTools.e2nd(*myCluster));

      std::pair<DetId, float> max =  lazyTools.getMaximum(*myCluster);
      NtupleFactory_->FillFloat("PFPhotonClusterRawIdMax", max.first.rawId());

      bool detIdMatrix3x3_complete_barrel = true;
      bool detIdMatrix5x5_complete_barrel = true;
      bool detIdMatrix3x3_complete_endcap = true;
      bool detIdMatrix5x5_complete_endcap = true;
      std::vector<DetId> detIdMatrix5x5 = lazyTools.matrixDetId (max.first, -2, 2, -2, 2);
      for(std::vector<DetId>::const_iterator matrixItr = detIdMatrix5x5.begin(); matrixItr != detIdMatrix5x5.end(); ++matrixItr)
	{
	  if ( matrixItr->subdetId() == EcalBarrel && max.first.subdetId() == EcalBarrel) 	  
	    {
	      EBDetId matrixItrBarrel(matrixItr->rawId());
	      EBDetId maxBarrel(max.first.rawId());
	      EcalRecHitCollection::const_iterator thishitEB = pEBrecHit->find (matrixItrBarrel) ;
	      if (thishitEB == pEBrecHit->end () && (abs(matrixItrBarrel.ieta() - maxBarrel.ieta()) <= 1 && abs(matrixItrBarrel.iphi() - maxBarrel.iphi()) <= 1))
		{ 
		  detIdMatrix5x5_complete_barrel = false;
		  detIdMatrix3x3_complete_barrel = false;
		  break;
		}
	      else
		detIdMatrix5x5_complete_barrel = false;
	    }
	  else if ( matrixItr->subdetId() == EcalEndcap && max.first.subdetId() == EcalEndcap) 	  
	    {
	      EEDetId matrixItrEndcap(matrixItr->rawId());
	      EEDetId maxEndcap(max.first.rawId());
	      EcalRecHitCollection::const_iterator thishitEE = pEErecHit->find (matrixItrEndcap) ;
	      if (thishitEE == pEErecHit->end () && (abs(matrixItrEndcap.ix() - maxEndcap.ix()) <= 1 && abs(matrixItrEndcap.ix() - maxEndcap.ix()) <= 1))
		{ 
		  detIdMatrix5x5_complete_endcap = false;
		  detIdMatrix3x3_complete_endcap = false;
		  break;
		}
	      else
		detIdMatrix5x5_complete_endcap = false;
	    }
	}
      NtupleFactory_->FillFloat("recHitMatrix3x3Complete_barrel",detIdMatrix3x3_complete_barrel) ;
      NtupleFactory_->FillFloat("recHitMatrix5x5Complete_barrel",detIdMatrix5x5_complete_barrel) ;
      NtupleFactory_->FillFloat("recHitMatrix3x3Complete_endcap",detIdMatrix3x3_complete_endcap) ;
      NtupleFactory_->FillFloat("recHitMatrix5x5Complete_endcap",detIdMatrix5x5_complete_endcap) ;

      //loop on xtals in cluster
      //std::cout << "===========" << std::endl;
      NtupleFactory_->FillFloat("PFPhotonClusterXtalIndex",xtalIndex);       

      double eneTmp = 0.;
      int nXtals = 0;
      double xtalMatrix [3][3];
      for (int xx = 0; xx < 3; ++xx)
	for (int yy = 0; yy < 3; ++yy)
	  xtalMatrix[xx][yy] = -9999.;
     
      std::vector<std::pair<DetId, float> > clusterDetIds = myCluster->hitsAndFractions() ; //get these from the cluster
      for (std::vector<std::pair<DetId, float> >::const_iterator detitr = clusterDetIds.begin () ; 
	   detitr != clusterDetIds.end () ; 
	   ++detitr)
	{
//	  metodo intelligente per prendere le energie, ma non il time!
// 	  double xtalEnergy = lazyTools.matrixEnergy(*myCluster, detitr -> first, 0, 0, 0, 0);
// 	  std::cout << "xtalEnergy lazy = " << xtalEnergy << std::endl;

	  //Here I use the "find" on a digi collection... I have been warned...
	  //dev'essere in ECAL
	  if ( (detitr -> first).det () != DetId::Ecal) 
	    { 
	      //std::cout << " det is " << (detitr -> first).det () << std::endl ;
	      continue ;
	    }
	  
	  //BARREL
	  bool isEB = true;
	  EcalRecHitCollection::const_iterator thishitEB = pEBrecHit->find ( (detitr -> first) ) ;
	  if (thishitEB == pEBrecHit->end ()) 
	    {
	      isEB = false;
	      //std::cout << "sta in EE" << std::endl;
	    }
	  //ENDCAP
	  bool isEE = true;
	  EcalRecHitCollection::const_iterator thishitEE = pEErecHit->find ( (detitr -> first) ) ;
	  if (thishitEE == pEErecHit->end ()) 
	    {
	      isEE = false;
	      //std::cout << "sta in EB" << std::endl;
	    }
	  
	  //o in EE o in EB
	  if((isEB == false && isEE == false) || (isEB == true && isEE == true)) 
	    {
	      std::cout << "PROBLEMS IN RECHITS!!!!" << std::endl;
	      continue;
	    }
	  
	  if(isEB == true)
	    {
	      EcalRecHit myhit = (*thishitEB) ;
	      NtupleFactory_->FillFloat("PFPhotonClusterXtalEnergy",myhit.energy()); 
	      NtupleFactory_->FillFloat("PFPhotonClusterXtalTime",myhit.time()); 
	      //NtupleFactory_->FillFloat("PFPhotonClusterXtalChi2",myhit.chi2()); 
	      NtupleFactory_->FillFloat("PFPhotonClusterXtalRawId",myhit.id().rawId()); 
	      
	      //std::cout << "myhit.energy() = " << myhit.energy() << "; detitr -> second = " << detitr -> second << std::endl;
	      eneTmp += myhit.energy();
	      ++nXtals;
	      
	      EBDetId dummy(myhit.id().rawId());
	      EBDetId dummyMax(max.first.rawId());
	      
	      int deltaIEta = dummyMax.ieta() - dummy.ieta();
	      int deltaIPhi = dummyMax.iphi() - dummy.iphi();
	      if(abs(deltaIEta) > 1 || abs(deltaIPhi) > 1) continue;
	      
	      xtalMatrix[deltaIEta + 1][deltaIPhi + 1] = myhit.energy();
	      //std::cout << "nXtals = " << nXtals <<"; deltaIEta = " << deltaIEta << "; deltaIPhi = " << deltaIPhi << std::endl;
	      
	    }
	  
	  if(isEE == true)
	    {
	      EcalRecHit myhit = (*thishitEE) ;
	      NtupleFactory_->FillFloat("PFPhotonClusterXtalEnergy",myhit.energy()); 
	      NtupleFactory_->FillFloat("PFPhotonClusterXtalTime",myhit.time()); 
	      //NtupleFactory_->FillFloat("PFPhotonClusterXtalChi2",myhit.chi2()); 
	      NtupleFactory_->FillFloat("PFPhotonClusterXtalRawId",myhit.id().rawId()); 

	      //std::cout << "myhit.energy() = " << myhit.energy() << "; detitr -> second = " << detitr -> second << std::endl;
	      eneTmp += myhit.energy();
	      ++nXtals;

	      EEDetId dummy(myhit.id().rawId());
	      EEDetId dummyMax(max.first.rawId());
	      
	      int deltaIx = dummyMax.ix() - dummy.ix();
	      int deltaIy = dummyMax.iy() - dummy.iy();
	      if(abs(deltaIx) > 1 || abs(deltaIy) > 1) continue;
	      
	      xtalMatrix[deltaIx + 1][deltaIy + 1] = myhit.energy();
	      //std::cout << "nXtals = " << nXtals <<"; deltaIx = " << deltaIx << "; deltaIy = " << deltaIy << std::endl;
	    }	  
	} //loop on xtals in cluster

      double eneSum = 0.;
      bool complete = true;
      for (int xx = 0; xx < 3; ++xx)
	for (int yy = 0; yy < 3; ++yy)
	  {
	    //std::cout << "xtalMatrix[" << xx << "][" << yy << "] = " << xtalMatrix[xx][yy] << std::endl;                                                             
	    if (xtalMatrix[xx][yy] == -9999.)
	      complete = false;
	    
	    eneSum += xtalMatrix[xx][yy];
	  }
      if(complete == false) NtupleFactory_->FillFloat("PFPhotonCluster3x3Complete",0.) ;
      else NtupleFactory_->FillFloat("PFPhotonCluster3x3Complete",1.) ;
      
      //std::cout << "complete = " << complete << "; eneSum - e3x3 = " << eneSum - lazyTools.e3x3(*myCluster) << std::endl;
      
      
      
      
      //std::cout << "cluster energy: " << myCluster->energy() << std::endl;
      //std::cout << "sumXtals: " << eneTmp << std::endl;
      
      NtupleFactory_->FillFloat("PFPhotonClusterNXtals",nXtals); 
      xtalIndex += nXtals;
    }

}


///---- PF clusters ----
void conversionTree::fillPFClusterInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
  edm::Handle<reco::PFClusterCollection> pPFCluster ;
  iEvent.getByLabel ("particleFlowClusterECAL", pPFCluster) ;

  for (reco::PFClusterCollection::const_iterator PFItr = pPFCluster->begin () ;
       PFItr != pPFCluster->end () ;
       PFItr++)
    {
      double pos = sqrt(PFItr->x()*PFItr->x() + PFItr->y()*PFItr->y() + PFItr->z()*PFItr->z());
      double ratio = PFItr->energy() / pos;

      NtupleFactory_->FillFloat("PFClusterEnergy",PFItr->energy());
      
      math::XYZVector myvect_XYZ(PFItr->x(),PFItr->y(),PFItr->z());
      NtupleFactory_->Fill3V("PFClusterPosition",myvect_XYZ);

      ROOT::Math::XYZTVector clusVect(ratio*PFItr->x(),ratio*PFItr->y(),ratio*PFItr->z(),PFItr->energy());
      NtupleFactory_->Fill4V("PFCluster",clusVect);

    }
    
}


///---- island clusters ----
void conversionTree::fillIslandClusterInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
  edm::Handle<reco::CaloClusterCollection> pCaloClusterEB ;
  iEvent.getByLabel (edm::InputTag("islandBasicClusters","islandBarrelBasicClusters"), pCaloClusterEB) ;

  edm::Handle<reco::CaloClusterCollection> pCaloClusterEE ;
  iEvent.getByLabel (edm::InputTag("islandBasicClusters","islandEndcapBasicClusters"), pCaloClusterEE) ;
    
  for (reco::CaloClusterCollection::const_iterator caloItr = pCaloClusterEB->begin () ;
       caloItr != pCaloClusterEB->end () ;
       caloItr++)
    {  
      double pos = sqrt(caloItr->x()*caloItr->x() + caloItr->y()*caloItr->y() + caloItr->z()*caloItr->z());
      double ratio = caloItr->energy() / pos;

      NtupleFactory_->FillFloat("IslandClusterType",0.);

      NtupleFactory_->FillFloat("IslandClusterEnergy",caloItr->energy());
      
      math::XYZVector myvect_XYZ(caloItr->x(),caloItr->y(),caloItr->z());
      NtupleFactory_->Fill3V("IslandClusterPosition",myvect_XYZ);

      ROOT::Math::XYZTVector clusVect(ratio*caloItr->x(),ratio*caloItr->y(),ratio*caloItr->z(),caloItr->energy());
      NtupleFactory_->Fill4V("IslandCluster",clusVect);
    }

  for (reco::CaloClusterCollection::const_iterator caloItr = pCaloClusterEE->begin () ;
       caloItr != pCaloClusterEE->end () ;
       caloItr++)
    {  
      double pos = sqrt(caloItr->x()*caloItr->x() + caloItr->y()*caloItr->y() + caloItr->z()*caloItr->z());
      double ratio = caloItr->energy() / pos;
      double type = 0.;
      if(caloItr->eta() > 0.) type = +1.;
      else if(caloItr->eta() < 0.) type = -1.;
      
      NtupleFactory_->FillFloat("IslandClusterType",type);

      NtupleFactory_->FillFloat("IslandClusterEnergy",caloItr->energy());
      
      math::XYZVector myvect_XYZ(caloItr->x(),caloItr->y(),caloItr->z());
      NtupleFactory_->Fill3V("IslandClusterPosition",myvect_XYZ);

      ROOT::Math::XYZTVector clusVect(ratio*caloItr->x(),ratio*caloItr->y(),ratio*caloItr->z(),caloItr->energy());
      NtupleFactory_->Fill4V("IslandCluster",clusVect);
    }
}


///-------------------
///---- Beam Spot ----

void conversionTree::fillBSInfo(const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
  //std::cout << "conversionTree::fillBSInfo::begin" << std::endl;
  
  edm::Handle<reco::BeamSpot> BSHandle;
  iEvent.getByType(BSHandle);
  
  
  // select the BS
  const reco::BeamSpot BS = *BSHandle;
  math::XYZPoint BSPoint(BS.position().x(), BS.position().y(), BS.position().z());
  BSPoint_ = BSPoint;
  
  
  NtupleFactory_ -> FillFloat("BS_x0", BS.x0());
  NtupleFactory_ -> FillFloat("BS_y0", BS.y0());
  NtupleFactory_ -> FillFloat("BS_z0", BS.z0());
  NtupleFactory_ -> FillFloat("BS_sigmaZ", BS.sigmaZ());
  NtupleFactory_ -> FillFloat("BS_dxdz", BS.dxdz());
  NtupleFactory_ -> FillFloat("BS_dydz", BS.dydz());
  NtupleFactory_ -> FillFloat("BS_BeamWidthX", BS.BeamWidthX());
  NtupleFactory_ -> FillFloat("BS_BeamWidthY", BS.BeamWidthY());
  
  //std::cout << "conversionTree::fillPVInfo::end" << std::endl;
}


///------------------------
///---- Primary Vertex ----

void conversionTree::fillPVInfo(const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
  //std::cout << "conversionTree::fillPVInfo::begin" << std::endl;
  
  edm::Handle<reco::VertexCollection> vertexes;
  iEvent.getByLabel(PVTag_, vertexes);
  
  
  // select the primary vertex    
  reco::Vertex PV;
  bool PVfound = (vertexes -> size() != 0);

  if(PVfound)
  {
    //VertexHigherPtSquared vertexTool;
    // sort the primary vertices according to sum of (pt)^2 of tracks (first one -> highest  sum of (pt)^2 )        
    PrimaryVertexSorter PVSorter;
    std::vector<reco::Vertex> sortedVertices = PVSorter.sortedList( *(vertexes.product()) );
    for( unsigned int u = 0 ; u < sortedVertices.size(); u++ ){
      PV = sortedVertices[u];
      
      NtupleFactory_ -> FillFloat("PV_normalizedChi2", PV.normalizedChi2());
      NtupleFactory_ -> FillInt  ("PV_ndof", PV.ndof());
      NtupleFactory_ -> FillInt  ("PV_nTracks", PV.tracksSize());
      NtupleFactory_ -> FillFloat("PV_z", PV.z());
      NtupleFactory_ -> FillFloat("PV_d0", PV.position().Rho());

      float myptsum = 0;
      float myptsum2 = 0;

      for ( Vertex::trackRef_iterator itr = PV.tracks_begin(); itr != PV.tracks_end(); ++itr){
        float pt = (**itr).pt();
        myptsum  += pt;
        myptsum2 += pt*pt;
      }

      NtupleFactory_ -> FillFloat("PV_SumPt" , myptsum);
      NtupleFactory_ -> FillFloat("PV_SumPt2", myptsum2);
    }
    PV = sortedVertices[0];
  }
  
  else
  {
    //creating a dummy PV
    reco::Vertex::Point p(BSPoint_.x(),BSPoint_.y(),BSPoint_.z());
    reco::Vertex::Error e;
    e(0,0) = 0.0015*0.0015;
    e(1,1) = 0.0015*0.0015;
    e(2,2) = 15.*15.;
    PV = reco::Vertex(p, e, 1, 1, 1);
    
    NtupleFactory_ -> FillFloat("PV_normalizedChi2", -1.);
    NtupleFactory_ -> FillInt  ("PV_ndof", -1);
    NtupleFactory_ -> FillInt  ("PV_nTracks", -1);
    NtupleFactory_ -> FillFloat("PV_z", -9999.);
    NtupleFactory_ -> FillFloat("PV_d0", -9999.);
    NtupleFactory_ -> FillFloat("PV_SumPt",-9999.);
    NtupleFactory_ -> FillFloat("PV_SumPt2",-9999.);
  }
  
  math::XYZPoint PVPoint(PV.position().x(), PV.position().y(), PV.position().z());
  PVPoint_ = PVPoint;
  
  //std::cout << "conversionTree::fillPVInfo::end" << std::endl;
}

///---- MC ----
void conversionTree::fillMCPi0Info (const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
 //std::cout << "conversionTree::fillMCPi0DecayInfo" << std::endl; 

 bool isValid = mcAnalysisPi0_ -> isValid();
  
 if( (eventType_ == 0) && (isValid == true) )
 {

   NtupleFactory_->Fill4V("mc_V",mcAnalysisPi0_ -> mcV()->p4());
   NtupleFactory_->FillFloat("mc_V_charge",mcAnalysisPi0_ -> mcV()->charge());
   NtupleFactory_->FillFloat("mcV_pdgId",mcAnalysisPi0_ -> mcV()->pdgId());

   math::XYZPoint p(mcAnalysisPi0_ -> mcV()->vertex());
   ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag> vertex(p.x(), p.y(), p.z());
   NtupleFactory_->Fill3V("mc_V_vertex", vertex);   
   
   NtupleFactory_->Fill4V("mcQ1_tag",mcAnalysisPi0_ -> mcQ1_tag()->p4());
   NtupleFactory_->FillFloat("mcQ1_tag_charge",mcAnalysisPi0_ -> mcQ1_tag()->charge());
   NtupleFactory_->FillFloat("mcQ1_tag_pdgId",mcAnalysisPi0_ -> mcQ1_tag()->pdgId());
   
   NtupleFactory_->Fill4V("mcQ2_tag",mcAnalysisPi0_ -> mcQ2_tag()->p4());
   NtupleFactory_->FillFloat("mcQ2_tag_charge",mcAnalysisPi0_ -> mcQ2_tag()->charge());
   NtupleFactory_->FillFloat("mcQ2_tag_pdgId",mcAnalysisPi0_ -> mcQ2_tag()->pdgId());
   
   NtupleFactory_->Fill4V("mcF1_fromV",mcAnalysisPi0_ -> mcF1_fromV()->p4());
   NtupleFactory_->FillFloat("mcF1_fromV_charge",mcAnalysisPi0_ -> mcF1_fromV()->charge());
   NtupleFactory_->FillFloat("mcF1_fromV_pdgId",mcAnalysisPi0_ -> mcF1_fromV()->pdgId());
   
   NtupleFactory_->Fill4V("mcF2_fromV",mcAnalysisPi0_ -> mcF2_fromV()->p4());
   NtupleFactory_->FillFloat("mcF2_fromV_charge",mcAnalysisPi0_ -> mcF2_fromV()->charge());
   NtupleFactory_->FillFloat("mcF2_fromV_pdgId",mcAnalysisPi0_ -> mcF2_fromV()->pdgId());
 } 
 
}

void conversionTree::fillMCInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByLabel("genParticles", genParticles);
  
  for(reco::GenParticleCollection::const_iterator p = genParticles -> begin();
      p != genParticles -> end(); 
      ++p)
    {
      if(p->pdgId() == 22)
	{
	  NtupleFactory_->Fill4V("MCPhoton",p->p4());
	  NtupleFactory_->FillFloat("MCPhotonMother",p->mother()->pdgId());
	}
    }//loop over genParticles
}



// ------------ method called to for each event  ------------
void conversionTree::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  //std::cout << "analyze" << std::endl;

  NtupleFactory_->FillInt("runId", iEvent.id().run());
  NtupleFactory_->FillInt("lumiId", iEvent.luminosityBlock());
  NtupleFactory_->FillInt("BXId", iEvent.bunchCrossing());
  NtupleFactory_->FillInt("eventId", iEvent.id().event());
  
  edm::Handle<reco::GenParticleCollection> genParticles;

  if(saveMCPi0_)
    {
      iEvent.getByLabel(MCtruthTag_, genParticles);
      mcAnalysisPi0_ = new MCDumperPi0(genParticles, eventType_, verbosity_);
    }

  ///---- fill HLT ----
  if (saveHLT_) fillHLTInfo (iEvent, iSetup);

  ///---- fill muons ----
  if (saveConversion_) fillConversionInfo (iEvent, iSetup);
  
  ///---- fill photoninfo ---
  if(savePhoton_)  fillPhotonInfo (iEvent, iSetup);

  ///---- fill PFPhotons ----
  if (savePFPhoton_)  fillPFPhotonInfo (iEvent, iSetup);
  
  ///---- fill PFCluster ----
  if (savePFCluster_) fillPFClusterInfo (iEvent, iSetup);
  
  ///---- fill fillIslandCluster ---- 
  if (saveIslandCluster_) fillIslandClusterInfo (iEvent, iSetup);
  
  ///---- fill PV ---- 
  if (savePV_) fillPVInfo (iEvent, iSetup);

  ///---- fill BS ---- 
  if(saveBS_) fillBSInfo (iEvent, iSetup);

  ///---- fill MC ---- 
  if (saveMC_) fillMCInfo (iEvent, iSetup);
  
 if (saveMCPi0_) fillMCPi0Info (iEvent, iSetup);

  
  ///---- save the entry of the tree ----
  NtupleFactory_->FillNtuple();
  
}

    
// ------------ method called once each job just before starting event loop  ------------
  
void conversionTree::beginJob()
{

  NtupleFactory_->AddInt("runId"); 
  NtupleFactory_->AddInt("lumiId"); 
  NtupleFactory_->AddInt("BXId"); 
  NtupleFactory_->AddInt("eventId"); 

  if(saveHLT_)
    {
      NtupleFactory_->AddFloat("HLT_WasRun"); 
      NtupleFactory_->AddFloat("HLT_Accept"); 
      NtupleFactory_->AddFloat("HLT_Error"); 
      NtupleFactory_->AddString("HLT_Names"); 
    }
  
  if(saveConversion_)
    {
      NtupleFactory_->AddFloat("pairInvariantMass"); 
      NtupleFactory_->Add3V("pairMomentum"); 
      NtupleFactory_->AddFloat("pairType");       
      NtupleFactory_->AddFloat("pairZOfPrimaryVertexFromTracks"); 
      NtupleFactory_->AddFloat("pairDistOfMinimumApproach"); 
      NtupleFactory_->AddFloat("pairDPhiTracksAtVertex"); 
      NtupleFactory_->Add3V("pairFirstTrackPIn"); 
      NtupleFactory_->Add3V("pairSecondTrackPIn"); 
      NtupleFactory_->Add3V("pairFirstTrackPOut"); 
      NtupleFactory_->Add3V("pairSecondTrackPOut");  
      NtupleFactory_->Add3V("pairFirstTrackInnerPosition");
      NtupleFactory_->Add3V("pairSecondTrackInnerPosition");
      NtupleFactory_->AddFloat("pairNTracks"); 
      NtupleFactory_->AddFloat("pairIsConverted"); 

      NtupleFactory_->AddFloat("pairHasRefit");
      NtupleFactory_->Add3V("pairMomentum_refit");
      NtupleFactory_->Add3V("pairMomentumFirstTrack_refit");
      NtupleFactory_->Add3V("pairMomentumSecondTrack_refit");

    }
  
  if(savePhoton_)
    {
      NtupleFactory_ -> Add4V ("photons");
      NtupleFactory_ -> AddInt  ("photons_isGap");
      NtupleFactory_ -> AddFloat("photons_e1x5");           
      NtupleFactory_ -> AddFloat("photons_e2x5");         
      NtupleFactory_ -> AddFloat("photons_e3x3");         
      NtupleFactory_ -> AddFloat("photons_e5x5");         
      NtupleFactory_ -> AddFloat("photons_maxEnergyXtal");
      NtupleFactory_ -> AddFloat("photons_sigmaEtaEta");  
      NtupleFactory_ -> AddFloat("photons_sigmaIetaIeta");
      NtupleFactory_ -> AddFloat("photons_r1x5");        
      NtupleFactory_ -> AddFloat("photons_r2x5");        
      NtupleFactory_ -> AddFloat("photons_r9");
      NtupleFactory_ -> AddFloat("photons_ecalIso");   
      NtupleFactory_ -> AddFloat("photons_hcalIso");   
      NtupleFactory_ -> AddFloat("photons_hadronicOverEm");   
      NtupleFactory_ -> AddFloat("photons_trkSumPtHollowConeDR04");   
      NtupleFactory_ -> AddInt("photons_hasPixelSeed");   
      NtupleFactory_ -> Add4V("photons_SC");   
      
      NtupleFactory_ ->AddTMatrix("photons_rechitTime");
      NtupleFactory_ ->AddTMatrix("photons_rechitE");
    }
  
  if(savePFPhoton_)
    {
      NtupleFactory_->AddFloat("PFPhotonEnergy");
      NtupleFactory_->AddFloat("PFPhotonPositionEta");
      NtupleFactory_->AddFloat("PFPhotonPositionPhi");
      NtupleFactory_->AddFloat("PFPhotonType");
      NtupleFactory_->Add4V("PFPhoton");

      NtupleFactory_->AddFloat("PFPhotonClusterEnergy");
      NtupleFactory_->Add3V("PFPhotonClusterPosition");
      NtupleFactory_->Add4V("PFPhotonCluster");

      NtupleFactory_->AddFloat("PFPhotonClusterE3x3");
      NtupleFactory_->AddFloat("PFPhotonClusterE5x5");
      NtupleFactory_->AddFloat("PFPhotonClusterEMax");
      NtupleFactory_->AddFloat("PFPhotonClusterE2nd");
      NtupleFactory_->AddFloat("PFPhotonClusterRawIdMax");
      NtupleFactory_->AddFloat("PFPhotonCluster3x3Complete");
      NtupleFactory_->AddFloat("recHitMatrix3x3Complete_barrel");
      NtupleFactory_->AddFloat("recHitMatrix5x5Complete_barrel");
      NtupleFactory_->AddFloat("recHitMatrix3x3Complete_endcap");
      NtupleFactory_->AddFloat("recHitMatrix5x5Complete_endcap");

      NtupleFactory_->AddFloat("PFPhotonClusterXtalEnergy");
      NtupleFactory_->AddFloat("PFPhotonClusterXtalTime");
      //NtupleFactory_->AddFloat("PFPhotonClusterXtalChi2");
      NtupleFactory_->AddFloat("PFPhotonClusterXtalRawId");
      NtupleFactory_->AddFloat("PFPhotonClusterNXtals");
      NtupleFactory_->AddFloat("PFPhotonClusterXtalIndex");

    }
  
  if(savePFCluster_)
    {
      NtupleFactory_->AddFloat("PFClusterEnergy");
      NtupleFactory_->Add3V("PFClusterPosition");
      NtupleFactory_->Add4V("PFCluster");
    }
  
  if(saveIslandCluster_)
    {
      NtupleFactory_->AddFloat("IslandClusterEnergy");
      NtupleFactory_->AddFloat("IslandClusterType");
      NtupleFactory_->Add3V("IslandClusterPosition");
      NtupleFactory_->Add4V("IslandCluster");
    }
  
  if(saveBS_)
    {
      NtupleFactory_ -> AddFloat("BS_x0"); 
      NtupleFactory_ -> AddFloat("BS_y0"); 
      NtupleFactory_ -> AddFloat("BS_z0"); 
      NtupleFactory_ -> AddFloat("BS_sigmaZ"); 
      NtupleFactory_ -> AddFloat("BS_dxdz"); 
      NtupleFactory_ -> AddFloat("BS_dydz"); 
      NtupleFactory_ -> AddFloat("BS_BeamWidthX"); 
      NtupleFactory_ -> AddFloat("BS_BeamWidthY"); 
    }
  
  if(savePV_)
    {
      NtupleFactory_ -> AddFloat("PV_normalizedChi2"); 
      NtupleFactory_ -> AddInt  ("PV_ndof"); 
      NtupleFactory_ -> AddInt  ("PV_nTracks"); 
      NtupleFactory_ -> AddFloat("PV_z"); 
      NtupleFactory_ -> AddFloat("PV_d0"); 
      NtupleFactory_ -> AddFloat("PV_SumPt"); 
      NtupleFactory_ -> AddFloat("PV_SumPt2"); 
    }

 if(saveMCPi0_)
 {
   NtupleFactory_->Add4V("mc_V");    
   NtupleFactory_->AddFloat("mc_V_charge");    
   NtupleFactory_->AddFloat("mcV_pdgId");    
   NtupleFactory_->Add3V("mc_V_vertex");

   NtupleFactory_->Add4V("mcQ1_tag");    
   NtupleFactory_->AddFloat("mcQ1_tag_charge");    
   NtupleFactory_->AddFloat("mcQ1_tag_pdgId");  
   
   NtupleFactory_->Add4V("mcQ2_tag");         
   NtupleFactory_->AddFloat("mcQ2_tag_charge");    
   NtupleFactory_->AddFloat("mcQ2_tag_pdgId");  
   
   NtupleFactory_->Add4V("mcF1_fromV");   
   NtupleFactory_->AddFloat("mcF1_fromV_charge");    
   NtupleFactory_->AddFloat("mcF1_fromV_pdgId");  
     
   NtupleFactory_->Add4V("mcF2_fromV");         
   NtupleFactory_->AddFloat("mcF2_fromV_charge");    
   NtupleFactory_->AddFloat("mcF2_fromV_pdgId");  
 }  

  if(saveMC_)
    {
      NtupleFactory_->Add4V("MCPhoton");
      NtupleFactory_->AddFloat("MCPhotonMother");
    }
  
}


// ------------ method called once each job just after ending the event loop  ------------

void conversionTree::endJob() 
{
  
  NtupleFactory_->WriteNtuple();
  
}
