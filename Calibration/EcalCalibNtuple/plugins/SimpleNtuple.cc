// -*- C++ -*-
//
// Package:   SimpleNtuple
// Class:     SimpleNtuple
//
#include "Calibration/EcalCalibNtuple/plugins/SimpleNtuple.h"

#define PI 3.141592654
#define TWOPI 6.283185308



SimpleNtuple::SimpleNtuple(const edm::ParameterSet& iConfig)
{
  edm::Service<TFileService> fs ;
  outTree_  =        fs -> make <TTree>("SimpleNtuple","SimpleNtuple"); 
  outTreeNameEleId = fs -> make <TTree>("NameEleId","NameEleId");
  outTreeNameEleId->Branch("eleId_names",&eleId_names_);
  
  L1Tag_ = iConfig.getParameter<edm::InputTag>("L1Tag");
  HLTTag_ = iConfig.getParameter<edm::InputTag>("HLTTag");
  
  PVTag_ = iConfig.getParameter<edm::InputTag>("PVTag");
  
  recHitCollection_EB_ = iConfig.getParameter<edm::InputTag>("recHitCollection_EB");
  recHitCollection_EE_ = iConfig.getParameter<edm::InputTag>("recHitCollection_EE");
  
  EleTag_ = iConfig.getParameter<edm::InputTag>("EleTag");
  MuTag_ = iConfig.getParameter<edm::InputTag>("MuTag");
  
  JetTag_ = iConfig.getParameter<edm::InputTag>("JetTag");
  
  CALOMetTag_ = iConfig.getParameter<edm::InputTag>("CALOMetTag");
  TCMetTag_ = iConfig.getParameter<edm::InputTag>("TCMetTag");
  PFMetTag_ = iConfig.getParameter<edm::InputTag>("PFMetTag");
  
  eleId_names_  = iConfig.getParameter< std::vector<std::string> >("eleId_names");
  outTreeNameEleId->Fill();
  
  
  
  //---- flags ----
  dataFlag_     = iConfig.getUntrackedParameter<bool> ("dataFlag_", false);
  saveL1_       = iConfig.getUntrackedParameter<bool> ("saveL1", true);
  saveBS_       = iConfig.getUntrackedParameter<bool> ("saveBS", true);
  savePV_       = iConfig.getUntrackedParameter<bool> ("savePV", true);
  saveL1_       = iConfig.getUntrackedParameter<bool> ("saveL1", true);
  saveHLT_      = iConfig.getUntrackedParameter<bool> ("saveHLT", true);
  saveEle_      = iConfig.getUntrackedParameter<bool> ("saveEle", true);
  saveMu_       = iConfig.getUntrackedParameter<bool> ("saveMu", true);
  saveJet_      = iConfig.getUntrackedParameter<bool> ("saveJet", true);
  saveCALOMet_  = iConfig.getUntrackedParameter<bool> ("saveCALOMet", true);
  saveTCMet_    = iConfig.getUntrackedParameter<bool> ("saveTCMet", true);
  savePFMet_    = iConfig.getUntrackedParameter<bool> ("savePFMet", true);
  
  verbosity_ = iConfig.getUntrackedParameter<bool>("verbosity","False");
  
  
  
  NtupleFactory_ = new NtupleFactory(outTree_);
  
  NtupleFactory_->AddInt("runId"); 
  NtupleFactory_->AddInt("lumiId"); 
  NtupleFactory_->AddInt("BXId"); 
  NtupleFactory_->AddInt("eventId"); 
  NtupleFactory_->AddInt("eventNaiveId"); 
  NtupleFactory_->AddInt("timeStampLow"); 
  NtupleFactory_->AddInt("timeStampHigh"); 
  eventNaiveId_ = 0;
  
  if(saveL1_)
  {
    NtupleFactory_->AddFloat("L1_techL1Bit"); 
    NtupleFactory_->AddFloat("L1_algoL1Bit"); 
  }
  
  if(saveHLT_)
  {
    NtupleFactory_ -> AddFloat ("HLT_WasRun");
    NtupleFactory_ -> AddFloat ("HLT_Accept");
    NtupleFactory_ -> AddFloat ("HLT_Error");
    NtupleFactory_ -> AddString("HLT_Names");
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
  }
  
  if(saveMu_)
  {
    NtupleFactory_->Add4V("muons");
    
    NtupleFactory_->AddInt("muons_global");
    NtupleFactory_->AddInt("muons_goodMuon");
    NtupleFactory_->AddFloat("muons_charge"); 
    NtupleFactory_->AddFloat("muons_nTkIsoR03"); 
    NtupleFactory_->AddFloat("muons_nTkIsoR05"); 
    NtupleFactory_->AddFloat("muons_tkIsoR03"); 
    NtupleFactory_->AddFloat("muons_tkIsoR05"); 
    NtupleFactory_->AddFloat("muons_emIsoR03"); 
    NtupleFactory_->AddFloat("muons_emIsoR05"); 
    NtupleFactory_->AddFloat("muons_hadIsoR03"); 
    NtupleFactory_->AddFloat("muons_hadIsoR05"); 
  }
  
  if(saveEle_)
  {
    f = EcalClusterFunctionFactory::get()->create("EcalClusterCrackCorrection", iConfig);
    
    // general variables
    NtupleFactory_->Add4V("electrons");
    NtupleFactory_->AddFloat("electrons_charge"); 
    NtupleFactory_->AddInt("electrons_isEB"); 
    NtupleFactory_->AddInt("electrons_isEBEEGap");
    NtupleFactory_->AddInt("electrons_isEBEtaGap");
    NtupleFactory_->AddInt("electrons_isEBPhiGap");
    NtupleFactory_->AddInt("electrons_isEEDeeGap");
    NtupleFactory_->AddInt("electrons_isEERingGap");
    
    // track variables
    NtupleFactory_->AddFloat("electrons_dxy_BS");
    NtupleFactory_->AddFloat("electrons_dz_BS");
    NtupleFactory_->AddFloat("electrons_dxy_PV");
    NtupleFactory_->AddFloat("electrons_dz_PV");
    NtupleFactory_->Add3V("electrons_p_atVtx");
    NtupleFactory_->Add3V("electrons_p_out");
    NtupleFactory_->Add3V("electrons_p_atCalo");
    NtupleFactory_->Add3PV("electrons_position_atVtx");
    NtupleFactory_->Add3PV("electrons_position_atCalo");
    NtupleFactory_->AddFloat("electrons_deltaEtaSuperClusterAtVtx");
    NtupleFactory_->AddFloat("electrons_deltaEtaSeedClusterAtCalo");
    NtupleFactory_->AddFloat("electrons_deltaEtaEleClusterAtCalo");
    NtupleFactory_->AddFloat("electrons_deltaPhiEleClusterAtCalo");
    NtupleFactory_->AddFloat("electrons_deltaPhiSuperClusterAtVtx");
    NtupleFactory_->AddFloat("electrons_deltaPhiSeedClusterAtCalo");
    
    // supercluster variables
    NtupleFactory_->Add3PV("electrons_scPosition");    
    NtupleFactory_->AddFloat("electrons_scE");
    NtupleFactory_->AddFloat("electrons_scEt");
    NtupleFactory_->AddFloat("electrons_scERaw");
    NtupleFactory_->AddFloat("electrons_scEtRaw");
    NtupleFactory_->AddFloat("electrons_scEta");
    NtupleFactory_->AddFloat("electrons_scPhi");
    NtupleFactory_->AddFloat("electrons_scPhiWidth");
    NtupleFactory_->AddFloat("electrons_scEtaWidth");
    NtupleFactory_->AddFloat("electrons_avgLaserCorrection");
    
    // cluster variables
    NtupleFactory_->AddInt("electrons_basicClustersSize");    
    NtupleFactory_->AddFloat("electrons_e1x5");
    NtupleFactory_->AddFloat("electrons_e2x5Max");
    NtupleFactory_->AddFloat("electrons_e5x5");
    NtupleFactory_->AddFloat("electrons_e3x3");
    NtupleFactory_->AddFloat("electrons_e2x2");
    
    // rechit variables
    NtupleFactory_->AddFloat("recHit_E"); 
    NtupleFactory_->AddInt("recHit_ieta");
    NtupleFactory_->AddInt("recHit_iphi");
    NtupleFactory_->AddInt("recHit_ix");
    NtupleFactory_->AddInt("recHit_iy");
    NtupleFactory_->AddInt("recHit_n");
    
    // seed variables
    NtupleFactory_->AddInt("electrons_seedSeverityLevel");
    NtupleFactory_->AddInt("electrons_seedFlag");
    NtupleFactory_->AddFloat("electrons_seedEnergy");
    NtupleFactory_->AddFloat("electrons_seedTime");
    NtupleFactory_->AddFloat("electrons_seedE1OverE9");
    NtupleFactory_->AddFloat("electrons_seedSwissCross");
    NtupleFactory_->AddFloat("electrons_seedLaserAlpha");
    NtupleFactory_->AddFloat("electrons_seedLaserCorrection");
    
    // preshower variables
    NtupleFactory_->AddFloat("electrons_ES");
    
    // id variables
    NtupleFactory_->AddFloat("electrons_classification");
    NtupleFactory_->AddFloat("electrons_eOverP");
    NtupleFactory_->AddFloat("electrons_eSeed");
    NtupleFactory_->AddFloat("electrons_fbrem");
    NtupleFactory_->AddFloat("electrons_sigmaIetaIeta");
    NtupleFactory_->AddFloat("electrons_hOverE");
    NtupleFactory_->AddFloat("electrons_deltaPhiIn");
    NtupleFactory_->AddFloat("electrons_deltaEtaIn");
    for( std::vector<std::string>::const_iterator iEleId = eleId_names_.begin(); iEleId != eleId_names_.end(); iEleId++ )
      NtupleFactory_->AddFloat(*iEleId);
    
    // isolation variables
    NtupleFactory_->AddFloat("electrons_tkIso03"); 
    NtupleFactory_->AddFloat("electrons_tkIso04"); 
    NtupleFactory_->AddFloat("electrons_emIso03"); 
    NtupleFactory_->AddFloat("electrons_emIso04"); 
    NtupleFactory_->AddFloat("electrons_hadIso03_1"); 
    NtupleFactory_->AddFloat("electrons_hadIso03_2"); 
    NtupleFactory_->AddFloat("electrons_hadIso04_1"); 
    NtupleFactory_->AddFloat("electrons_hadIso04_2"); 
    
    // conversion rejection variables
    NtupleFactory_->AddInt("electrons_convFlag");
    NtupleFactory_->AddInt("electrons_mishits");
    NtupleFactory_->AddInt("electrons_nAmbiguousGsfTracks");
    NtupleFactory_->AddFloat("electrons_dist");
    NtupleFactory_->AddFloat("electrons_dcot");
    
    // crack correction variables
    NtupleFactory_->AddFloat("electrons_crackCorrection");
  }
  
  if(saveJet_)
  {
    NtupleFactory_->Add4V("jets");
  }
  
  if(saveCALOMet_)
  {
    NtupleFactory_->Add4V("CALOMet");
    NtupleFactory_->AddFloat("CALOSumEt");
  }
  
  if(saveTCMet_)
  {
    NtupleFactory_->Add4V("TCMet");
    NtupleFactory_->AddFloat("TCSumEt");
  }
  
  if(savePFMet_)
  {
    NtupleFactory_->Add4V("PFMet");
    NtupleFactory_->AddFloat("PFSumEt");
  }
  
  
}

// --------------------------------------------------------------------



SimpleNtuple::~SimpleNtuple ()
{
  cout<< "Analyzed " <<  eventNaiveId_ << " events" <<endl;
  NtupleFactory_->WriteNtuple();
  delete NtupleFactory_;
}

// -----------------------------------------------------------------------------------------



void SimpleNtuple::analyze (const edm::Event& iEvent, const edm::EventSetup& iSetup) {

 ++eventNaiveId_;
 
 NtupleFactory_->FillInt("runId", iEvent.id().run());
 NtupleFactory_->FillInt("lumiId", iEvent.luminosityBlock());
 NtupleFactory_->FillInt("BXId", iEvent.bunchCrossing());
 NtupleFactory_->FillInt("eventId", iEvent.id().event());
 NtupleFactory_->FillInt("eventNaiveId", eventNaiveId_);
 NtupleFactory_->FillInt("timeStampLow", (int)(0xFFFFFFFF& iEvent.time().value()));
 NtupleFactory_->FillInt("timeStampHigh", (int)(iEvent.time().value() >> 32));

 ///---- fill L1 ----
 if (saveL1_) fillL1Info (iEvent, iSetup);

 ///---- fill HLT ----
 if (saveHLT_) fillHLTInfo (iEvent, iSetup);
 
 ///---- fill PV ----
 if(saveBS_) fillBSInfo (iEvent, iSetup);

 ///---- fill PV ----
 if(savePV_) fillPVInfo (iEvent, iSetup);
  
 ///---- fill electrons ----
 if (saveEle_)  fillEleInfo (iEvent, iSetup);

 ///---- fill muons ----
 if (saveMu_) fillMuInfo (iEvent, iSetup);

 ///---- fill jets ---- 
 if (saveJet_) fillJetInfo (iEvent, iSetup);

 ///---- fill CALOMet ---- 
 if (saveCALOMet_) fillCALOMetInfo (iEvent, iSetup);

 ///---- fill TCMet ---- 
 if (saveTCMet_) fillTCMetInfo (iEvent, iSetup);

 ///---- fill PFMet ---- 
 if (savePFMet_) fillPFMetInfo (iEvent, iSetup);

 ///---- save the entry of the tree ----
 NtupleFactory_->FillNtuple();

}



// -----------------------------------------------------------------------------------------


//---- dump L1Info
void SimpleNtuple::fillL1Info (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 edm::ESHandle<L1GtTriggerMenu> menuRcd;
 iSetup.get<L1GtTriggerMenuRcd>().get(menuRcd) ;

 edm::Handle<L1GlobalTriggerReadoutRecord> gtRecord;
 iEvent.getByLabel(L1Tag_, gtRecord);

 DecisionWord AlgoWord = gtRecord->decisionWord();
 TechnicalTriggerWord TechWord = gtRecord->technicalTriggerWord();
 
  // Loop over the technical bits
 for (unsigned int ibit = 0; ibit < TechWord.size(); ibit++) 
 {
  NtupleFactory_->FillFloat("L1_techL1Bit",TechWord[ibit]);
 }
  
  // Loop over the algo bits
 for (unsigned int ibit = 0; ibit < AlgoWord.size(); ibit++) 
 {
  NtupleFactory_->FillFloat("L1_algoL1Bit",AlgoWord[ibit]);
 }

 return ;

}

// -----------------------------------------------------------------------------------------



void SimpleNtuple::fillHLTInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
  //std::cout << "SimpleNtuple::fillHLTInfo::begin" << std::endl;
  
  
  edm::Handle<edm::TriggerResults> triggerResultsHandle;
  iEvent.getByLabel(HLTTag_, triggerResultsHandle);
  
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

// -----------------------------------------------------------------------------------------



void SimpleNtuple::fillBSInfo(const edm::Event & iEvent, const edm::EventSetup & iESetup)
{
  //std::cout << "SimpleNtuple::fillBSInfo::begin" << std::endl;
  
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

  //std::cout << "SimpleNtuple::fillBSInfo::end" << std::endl;
}

// -----------------------------------------------------------------------------------------



void SimpleNtuple::fillPVInfo(const edm::Event & iEvent, const edm::EventSetup & iESetup)
{
  //std::cout << "SimpleNtuple::fillPVInfo::begin" << std::endl;
    
  edm::Handle<reco::VertexCollection> vertexes;
  iEvent.getByLabel(PVTag_, vertexes);
  
  
  // select the primary vertex
  reco::Vertex PV;
  bool PVfound = (vertexes -> size() != 0);
  
  if(PVfound)
  {
    VertexHigherPtSquared vertexTool;
    // sort the primary vertices according to sum of (pt)^2 of tracks (first one -> highest  sum of (pt)^2 )
    PrimaryVertexSorter PVSorter;
    std::vector<reco::Vertex> sortedVertices = PVSorter.sortedList( *(vertexes.product()) );
    for( unsigned int u = 0 ; u < sortedVertices.size(); u++ )
    {
      PV = sortedVertices[u];
      
      NtupleFactory_ -> FillFloat("PV_normalizedChi2", PV.normalizedChi2());
      NtupleFactory_ -> FillInt  ("PV_ndof", PV.ndof());
      NtupleFactory_ -> FillInt  ("PV_nTracks", PV.tracksSize());
      NtupleFactory_ -> FillFloat("PV_z", PV.z());
      NtupleFactory_ -> FillFloat("PV_d0", PV.position().Rho());
      NtupleFactory_ -> FillFloat("PV_SumPt",vertexTool.sumPtSquared(PV));
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
  }
  
  math::XYZPoint PVPoint(PV.position().x(), PV.position().y(), PV.position().z());
  PVPoint_ = PVPoint;
  
  //std::cout << "SimpleNtuple::fillPVInfo::end" << std::endl;
}

// -----------------------------------------------------------------------------------------



void SimpleNtuple::fillEleInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{ 
 //*********** CALO TOPOLOGY
 edm::ESHandle<CaloTopology> pTopology;
 iSetup.get<CaloTopologyRecord>().get(pTopology);
 const CaloTopology *topology = pTopology.product();

 //*********** CHANNEL STATUS
 edm::ESHandle<EcalChannelStatus> theChannelStatus;
 iSetup.get<EcalChannelStatusRcd>().get(theChannelStatus);
 
 //*********** LASER ALPHAS
 edm::ESHandle<EcalLaserAlphas> theEcalLaserAlphas;
 iSetup.get<EcalLaserAlphasRcd>().get(theEcalLaserAlphas);
 const EcalLaserAlphaMap* theEcalLaserAlphaMap = theEcalLaserAlphas.product();
 
 //*********** LASER CORRECTION
 edm::ESHandle<EcalLaserDbService> theLaser;
 iSetup.get<EcalLaserDbRecord>().get(theLaser);
 
 //*********** EB REC HITS
 edm::Handle<EcalRecHitCollection> recHitsEB;
 iEvent.getByLabel( recHitCollection_EB_, recHitsEB );
 const EcalRecHitCollection* theBarrelEcalRecHits = recHitsEB.product () ;
 if ( ! recHitsEB.isValid() ) {
  std::cerr << "SimpleNtuple::analyze --> recHitsEB not found" << std::endl; 
 }
  
 //*********** EE REC HITS
 edm::Handle<EcalRecHitCollection> recHitsEE;
 iEvent.getByLabel( recHitCollection_EE_, recHitsEE );
 const EcalRecHitCollection* theEndcapEcalRecHits = recHitsEE.product () ;
 if ( ! recHitsEE.isValid() ) {
  std::cerr << "SimpleNtuple::analyze --> recHitsEE not found" << std::endl; 
 }

 //************* ELECTRONS
 Handle<View<pat::Electron> > electronHandle;
 iEvent.getByLabel(EleTag_,electronHandle);
 View<pat::Electron> electrons = *electronHandle;
 
 
 
 // Loop over electrons
 for ( unsigned int i=0; i<electrons.size(); ++i )
 {
   pat::Electron electron = electrons.at(i);
   
   
   // basic variables
   NtupleFactory_->Fill4V("electrons",electron.p4());
   NtupleFactory_->FillFloat("electrons_charge",(electron.charge()));
   NtupleFactory_->FillInt("electrons_isEB",(electron.isEB()));
   NtupleFactory_->FillInt("electrons_isEBEEGap",(electron.isEBEEGap()));
   NtupleFactory_->FillInt("electrons_isEBEtaGap",(electron.isEBEtaGap()));
   NtupleFactory_->FillInt("electrons_isEBPhiGap",(electron.isEBPhiGap()));
   NtupleFactory_->FillInt("electrons_isEEDeeGap",(electron.isEEDeeGap()));
   NtupleFactory_->FillInt("electrons_isEERingGap",(electron.isEERingGap()));
   
   
   // track variables
   reco::GsfTrackRef eleTrack  = electron.gsfTrack() ; 
   NtupleFactory_->FillFloat("electrons_dxy_BS", eleTrack->dxy (BSPoint_));
   NtupleFactory_->FillFloat("electrons_dz_BS", eleTrack->dz (BSPoint_));
   NtupleFactory_->FillFloat("electrons_dxy_PV", eleTrack->dxy (PVPoint_));
   NtupleFactory_->FillFloat("electrons_dz_PV", eleTrack->dz (PVPoint_));
   NtupleFactory_->Fill3V("electrons_p_atVtx",electron.trackMomentumAtVtx());
   NtupleFactory_->Fill3V("electrons_p_out",electron.trackMomentumOut());
   NtupleFactory_->Fill3V("electrons_p_atCalo",electron.trackMomentumAtCalo());
   NtupleFactory_->Fill3PV("electrons_position_atVtx",electron.trackPositionAtVtx());
   NtupleFactory_->Fill3PV("electrons_position_atCalo",electron.trackPositionAtCalo());
   NtupleFactory_->FillFloat("electrons_deltaEtaSuperClusterAtVtx",electron.deltaEtaSuperClusterTrackAtVtx());
   NtupleFactory_->FillFloat("electrons_deltaEtaSeedClusterAtCalo",electron.deltaEtaSeedClusterTrackAtCalo());
   NtupleFactory_->FillFloat("electrons_deltaEtaEleClusterAtCalo",electron.deltaEtaEleClusterTrackAtCalo());
   NtupleFactory_->FillFloat("electrons_deltaPhiEleClusterAtCalo",electron.deltaPhiEleClusterTrackAtCalo());
   NtupleFactory_->FillFloat("electrons_deltaPhiSuperClusterAtVtx",electron.deltaPhiSuperClusterTrackAtVtx());
   NtupleFactory_->FillFloat("electrons_deltaPhiSeedClusterAtCalo",electron.deltaPhiSeedClusterTrackAtCalo());
   
   
   // supercluster variables
   reco::SuperClusterRef scRef = electron.superCluster();
   const edm::Ptr<reco::CaloCluster>& seedCluster = scRef->seed();
   
   double R  = TMath::Sqrt(scRef->x()*scRef->x() + scRef->y()*scRef->y() +scRef->z()*scRef->z());
   double Rt = TMath::Sqrt(scRef->x()*scRef->x() + scRef->y()*scRef->y());
   
   NtupleFactory_->Fill3PV("electrons_scPosition",electron.superClusterPosition());
   NtupleFactory_->FillFloat("electrons_scE",scRef->energy());
   NtupleFactory_->FillFloat("electrons_scEt",scRef->energy()*(Rt/R));
   NtupleFactory_->FillFloat("electrons_scERaw",scRef->rawEnergy());
   NtupleFactory_->FillFloat("electrons_scEtRaw",scRef->rawEnergy()*(Rt/R));
   NtupleFactory_->FillFloat("electrons_scEta",scRef->eta());
   NtupleFactory_->FillFloat("electrons_scPhi",scRef->phi());   
   NtupleFactory_->FillFloat("electrons_scPhiWidth",scRef->phiWidth());
   NtupleFactory_->FillFloat("electrons_scEtaWidth",scRef->etaWidth());
   
   
   // cluster variables
   float E3x3 = 0;
   float E2x2 = 0;
   
   if ( electron.isEB() )
   {
     E3x3 = EcalClusterTools::e3x3( *scRef, theBarrelEcalRecHits, topology);
     E2x2 = EcalClusterTools::e2x2( *scRef, theBarrelEcalRecHits, topology);
   }
   if ( electron.isEE() )
   {
     E3x3 = EcalClusterTools::e3x3( *scRef, theEndcapEcalRecHits, topology);
     E2x2 = EcalClusterTools::e2x2( *scRef, theEndcapEcalRecHits, topology);
   }
   
   NtupleFactory_->FillInt("electrons_basicClustersSize",electron.basicClustersSize());
   NtupleFactory_->FillFloat("electrons_e1x5",electron.e1x5());
   NtupleFactory_->FillFloat("electrons_e2x5Max",electron.e2x5Max());
   NtupleFactory_->FillFloat("electrons_e2x2",E2x2);
   NtupleFactory_->FillFloat("electrons_e3x3",E3x3);
   NtupleFactory_->FillFloat("electrons_e5x5",electron.e5x5());
   
   
   // rechit variables
   int numRecHit = 0;
   float sumRecHitE = 0.;
   float sumLaserCorrectionRecHitE = 0.;
   
   const std::vector<std::pair<DetId,float> >& hits = scRef->hitsAndFractions();
   for(std::vector<std::pair<DetId,float> >::const_iterator rh = hits.begin(); rh!=hits.end(); ++rh)
   {
     float rhLaserCorrection = -1.;
     
     if ((*rh).first.subdetId()== EcalBarrel)
     {
       EBRecHitCollection::const_iterator itrechit = theBarrelEcalRecHits->find((*rh).first);
       if (itrechit==theBarrelEcalRecHits->end()) continue;
       EBDetId barrelId (itrechit->id ()); 
       NtupleFactory_->FillFloat("recHit_E",itrechit->energy());
       NtupleFactory_->FillInt("recHit_ieta",barrelId.ieta());
       NtupleFactory_->FillInt("recHit_iphi",barrelId.iphi());
       NtupleFactory_->FillInt("recHit_ix",-9999);
       NtupleFactory_->FillInt("reHit_iy",-9999);
       ++numRecHit;
       
       // laser correction
       rhLaserCorrection = theLaser->getLaserCorrection(barrelId, iEvent.time());
       sumRecHitE += itrechit->energy();
       sumLaserCorrectionRecHitE += itrechit->energy() * rhLaserCorrection;
     }
       
     if ((*rh).first.subdetId()== EcalEndcap)
     {
       EERecHitCollection::const_iterator itrechit = theEndcapEcalRecHits->find((*rh).first);
       if (itrechit==theEndcapEcalRecHits->end()) continue;
       EEDetId endcapId (itrechit->id ()); 
       NtupleFactory_->FillFloat("recHit_E",itrechit->energy());
       NtupleFactory_->FillInt("recHit_ix",endcapId.ix());
       NtupleFactory_->FillInt("recHit_iy",endcapId.iy());
       NtupleFactory_->FillInt("recHit_ieta",-9999);
       NtupleFactory_->FillInt("recHit_iphi",-9999);
       ++numRecHit;
       
       // laser correction
       rhLaserCorrection = theLaser->getLaserCorrection(endcapId, iEvent.time());
       sumRecHitE += itrechit->energy();
       sumLaserCorrectionRecHitE += itrechit->energy() * rhLaserCorrection;
     }
   }
   
   NtupleFactory_->FillInt("recHit_n",numRecHit);
   NtupleFactory_->FillFloat("electrons_scAvgLaserCorrection", sumLaserCorrectionRecHitE/sumRecHitE);
   
   
   // seed variables
   int sev = -1;
   int flag = -1;
   float energy;
   float time; 
   float E1OverE9;
   float swissCross;
   float seedLaserAlpha = -1.;
   float seedLaserCorrection = -1.;
   
   if(electron.isEB())
   {
     std::pair<DetId, float> id = EcalClusterTools::getMaximum(seedCluster->hitsAndFractions(), theBarrelEcalRecHits);
     
     // severity level - SwissCross 
     sev = EcalSeverityLevelAlgo::severityLevel(id.first, *theBarrelEcalRecHits, *(theChannelStatus.product()));
     
     // flag - OutOfTime
     EcalRecHitCollection::const_iterator it = theBarrelEcalRecHits->find(id.first);
     
     if( it != theBarrelEcalRecHits->end() )
     {
       const EcalRecHit& rh = (*it);
       flag = rh.recoFlag();
       energy = rh.energy();
       time = rh.time();
       E1OverE9 = EcalSeverityLevelAlgo::E1OverE9(id.first, *theBarrelEcalRecHits);
       swissCross = EcalSeverityLevelAlgo::swissCross(id.first, *theBarrelEcalRecHits);
     }
     
     // laser alphas
     EcalLaserAlphaMap::const_iterator italpha = theEcalLaserAlphaMap->find(id.first);
     if( italpha != theEcalLaserAlphaMap->end() )
       seedLaserAlpha = (*italpha);
     
     // laser correction
     seedLaserCorrection = theLaser->getLaserCorrection(EBDetId(id.first), iEvent.time());
   }
   
   else
   {
     std::pair<DetId, float> id = EcalClusterTools::getMaximum(seedCluster->hitsAndFractions(), theEndcapEcalRecHits);
     
     // severity level - SwissCross 
     sev = EcalSeverityLevelAlgo::severityLevel(id.first, *theEndcapEcalRecHits, *(theChannelStatus.product()));
     
     // flag - OutOfTime
     EcalRecHitCollection::const_iterator it = theEndcapEcalRecHits->find(id.first);
     
     if( it != theEndcapEcalRecHits->end() )
     {
       const EcalRecHit& rh = (*it);
       flag = rh.recoFlag();
       energy = rh.energy();
       time = rh.time();
       E1OverE9 = EcalSeverityLevelAlgo::E1OverE9(id.first, *theEndcapEcalRecHits);
       swissCross = EcalSeverityLevelAlgo::swissCross(id.first, *theEndcapEcalRecHits);
     }
     
     // laser alphas
     EcalLaserAlphaMap::const_iterator italpha = theEcalLaserAlphaMap->find(id.first);
     if( italpha != theEcalLaserAlphaMap->end() )
       seedLaserAlpha = (*italpha);
     
     // laser correction
     seedLaserCorrection = theLaser->getLaserCorrection(EEDetId(id.first), iEvent.time());
   }

   NtupleFactory_->FillInt("electrons_seedSeverityLevel", sev);
   NtupleFactory_->FillInt("electrons_seedFlag", flag);
   NtupleFactory_->FillFloat("electrons_seedEnergy", energy);
   NtupleFactory_->FillFloat("electrons_seedTime", time);
   NtupleFactory_->FillFloat("electrons_seedE1OverE9", E1OverE9);
   NtupleFactory_->FillFloat("electrons_seedSwissCross", swissCross);
   NtupleFactory_->FillFloat("electrons_seedLaserAlpha", seedLaserAlpha);
   NtupleFactory_->FillFloat("electrons_seedLaserCorrection", seedLaserCorrection);
   
   
   // preshower variables 
   NtupleFactory_->FillFloat("electrons_ES",scRef->preshowerEnergy());
   
      
   // eleid variables
   NtupleFactory_->FillInt("electrons_classification",electron.classification());
   NtupleFactory_->FillFloat("electrons_eOverP",electron.eSuperClusterOverP());
   NtupleFactory_->FillFloat("electrons_eSeed",electron.superCluster()->seed()->energy());
   NtupleFactory_->FillFloat("electrons_fbrem",electron.fbrem());
   NtupleFactory_->FillFloat("electrons_sigmaIetaIeta",electron.sigmaIetaIeta());
   NtupleFactory_->FillFloat("electrons_hOverE",electron.hadronicOverEm());
   NtupleFactory_->FillFloat("electrons_deltaPhiIn",electron.deltaPhiSuperClusterTrackAtVtx());
   NtupleFactory_->FillFloat("electrons_deltaEtaIn",electron.deltaEtaSuperClusterTrackAtVtx());
   for(std::vector<std::string>::const_iterator iEleId = eleId_names_.begin(); iEleId != eleId_names_.end(); iEleId++)
     NtupleFactory_->FillFloat(*iEleId, electron.electronID(*iEleId));
   
   
   // isolation variables
   NtupleFactory_->FillFloat("electrons_tkIso03",(electron.dr03TkSumPt()));
   NtupleFactory_->FillFloat("electrons_tkIso04",(electron.dr04TkSumPt()));
   NtupleFactory_->FillFloat("electrons_emIso03",(electron.dr03EcalRecHitSumEt()));
   NtupleFactory_->FillFloat("electrons_emIso04",(electron.dr04EcalRecHitSumEt()));
   NtupleFactory_->FillFloat("electrons_hadIso03_1",(electron.dr03HcalDepth1TowerSumEt()));
   NtupleFactory_->FillFloat("electrons_hadIso03_2",(electron.dr03HcalDepth2TowerSumEt()));
   NtupleFactory_->FillFloat("electrons_hadIso04_1",(electron.dr04HcalDepth1TowerSumEt()));
   NtupleFactory_->FillFloat("electrons_hadIso04_2",(electron.dr04HcalDepth2TowerSumEt()));
   
   
   // conversion rejection variables
   NtupleFactory_->FillInt("electrons_convFlag",electron.convFlags());
   NtupleFactory_->FillInt("electrons_mishits",electron.gsfTrack()->trackerExpectedHitsInner().numberOfHits());
   NtupleFactory_->FillInt("electrons_nAmbiguousGsfTracks",electron.ambiguousGsfTracksSize());
   NtupleFactory_->FillFloat("electrons_dist", electron.convDist());
   NtupleFactory_->FillFloat("electrons_dcot", electron.convDcot());
   
   
   // crack correction variables
   f -> init(iSetup);
   double crackcor = 1.;
   
   for(reco::CaloCluster_iterator cIt = electron.superCluster()->clustersBegin();
       cIt != electron.superCluster()->clustersEnd(); ++cIt)
   {
     const reco::CaloClusterPtr cc = *cIt; 
     crackcor *= ( (electron.superCluster()->rawEnergy() + (*cIt)->energy()*(f->getValue(*cc)-1.)) / electron.superCluster()->rawEnergy() );
   }
   
   NtupleFactory_->FillFloat("electrons_crackCorrection", crackcor);
   
   
 } // end loop over electron candidates

}

// -----------------------------------------------------------------------------------------



// ---- MET ----
void SimpleNtuple::fillCALOMetInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 //std::cout << "SimpleNtuple::fillCALOMetInfo" << std::endl;
 
 edm::Handle<edm::View<pat::MET> > calometHandle;
 iEvent.getByLabel(CALOMetTag_,calometHandle);
 View<pat::MET>  mets = *calometHandle;
 //pat::MET met = mets.at(mets.size()-1);
 //std::cout << " mets.size()-1 = " << mets.size()-1 << std::endl;
 pat::MET met = mets.at(0);
 NtupleFactory_->Fill4V("CALOMet",met.p4());
 NtupleFactory_->FillFloat("CALOSumEt",met.sumEt());
}

void SimpleNtuple::fillTCMetInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 //std::cout << "SimpleNtuple::fillTCMetInfo" << std::endl;
 
 edm::Handle<edm::View<pat::MET> > tcmetHandle;
 iEvent.getByLabel(TCMetTag_,tcmetHandle);
 View<pat::MET>  mets = *tcmetHandle;
 //pat::MET met = mets.at(mets.size()-1);
 //std::cout << " mets.size()-1 = " << mets.size()-1 << std::endl;
 pat::MET met = mets.at(0);
 NtupleFactory_->Fill4V("TCMet",met.p4());
 NtupleFactory_->FillFloat("TCSumEt",met.sumEt());
}

void SimpleNtuple::fillPFMetInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 //std::cout << "SimpleNtuple::fillPFMetInfo" << std::endl;
 
 edm::Handle<edm::View<pat::MET> > PFmetHandle;
 iEvent.getByLabel(PFMetTag_,PFmetHandle);
 View<pat::MET>  mets = *PFmetHandle;
 //pat::MET met = mets.at(mets.size()-1);
 //std::cout << " mets.size()-1 = " << mets.size()-1 << std::endl;
 pat::MET met = mets.at(0);
 NtupleFactory_->Fill4V("PFMet",met.p4());
 NtupleFactory_->FillFloat("PFSumEt",met.sumEt());
}

// -----------------------------------------------------------------------------------------



void SimpleNtuple::fillJetInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 //std::cout << "SimpleNtuple::fillJetInfo" << std::endl;
 //************* JETS
 Handle<View<pat::Jet> > jetHandle;
 iEvent.getByLabel(JetTag_,jetHandle);
 View<pat::Jet> jets = *jetHandle;
 
 for ( unsigned int i=0; i<jets.size(); ++i ) {
    pat::Jet jet = jets.at(i);
    if ( jet.pt()< 5. ) continue; 
    NtupleFactory_->Fill4V("jets",jet.p4());
 }
} // dumpJetInfo 

// -----------------------------------------------------------------------------------------



///---- muons ----
void SimpleNtuple::fillMuInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 //std::cout << "SimpleNtuple::fillMuInfo" << std::endl;
 //************* MUONS
 
 Handle<View<pat::Muon> > muonHandle;
 iEvent.getByLabel(MuTag_,muonHandle);
 View<pat::Muon> muons = *muonHandle;

 for ( unsigned int i=0; i<muons.size(); ++i ) {
    pat::Muon muon = muons.at(i);

    NtupleFactory_->Fill4V("muons",muon.p4());
    NtupleFactory_->FillInt("muons_global",(muon.isGlobalMuon()));
    NtupleFactory_->FillInt("muons_goodMuon",muon::isGoodMuon(muon, muon::GlobalMuonPromptTight));

    NtupleFactory_->FillFloat("muons_charge",(muon.charge()));
    NtupleFactory_->FillFloat("muons_tkIsoR03",(muon.isolationR03()).sumPt);
    NtupleFactory_->FillFloat("muons_nTkIsoR03",(muon.isolationR03()).nTracks);
    NtupleFactory_->FillFloat("muons_emIsoR03",(muon.isolationR03()).emEt);
    NtupleFactory_->FillFloat("muons_hadIsoR03",(muon.isolationR03()).hadEt);

    NtupleFactory_->FillFloat("muons_tkIsoR05",(muon.isolationR05()).sumPt);
    NtupleFactory_->FillFloat("muons_nTkIsoR05",(muon.isolationR05()).nTracks);
    NtupleFactory_->FillFloat("muons_emIsoR05",(muon.isolationR05()).emEt);
    NtupleFactory_->FillFloat("muons_hadIsoR05",(muon.isolationR05()).hadEt);
  }
} // dumpMuonInfo 



// -----------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(SimpleNtuple);