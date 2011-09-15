// -*- C++ -*-
//
// Package:   SimpleNtupleCalib
// Class:     SimpleNtupleCalib
//
#include "Calibration/EcalCalibNtuple/plugins/SimpleNtupleCalib.h"

#include "Math/Vector4D.h"
#include "Math/Vector3D.h"

#define PI 3.141592654
#define TWOPI 6.283185308



SimpleNtupleCalib::SimpleNtupleCalib(const edm::ParameterSet& iConfig)
{
  edm::Service<TFileService> fs ;
  outTree_  =        fs -> make <TTree>("SimpleNtupleCalib","SimpleNtupleCalib"); 
  outTreeNameEleId = fs -> make <TTree>("NameEleId","NameEleId");
  outTreeNameEleId->Branch("eleId_names",&eleId_names_);

  MCPileupTag_ = iConfig.getParameter<edm::InputTag>("MCPileupTag");

  L1Tag_ = iConfig.getParameter<edm::InputTag>("L1Tag");
  TriggerEventTag_ = iConfig.getParameter<edm::InputTag>("TriggerEventTag");
  TriggerResultsTag_ = iConfig.getParameter<edm::InputTag>("TriggerResultsTag");
  
  PVTag_ = iConfig.getParameter<edm::InputTag>("PVTag");
  
  recHitCollection_EB_ = iConfig.getParameter<edm::InputTag>("recHitCollection_EB");
  recHitCollection_EE_ = iConfig.getParameter<edm::InputTag>("recHitCollection_EE");
  
  EleTag_ = iConfig.getParameter<edm::InputTag>("EleTag");
  PhotonTag_      = iConfig.getParameter<edm::InputTag>("PhotonTag");
  MuTag_ = iConfig.getParameter<edm::InputTag>("MuTag");
  
  JetTag_ = iConfig.getParameter<edm::InputTag>("JetTag");
  
  CALOMetTag_ = iConfig.getParameter<edm::InputTag>("CALOMetTag");
  TCMetTag_ = iConfig.getParameter<edm::InputTag>("TCMetTag");
  PFMetTag_ = iConfig.getParameter<edm::InputTag>("PFMetTag");
  
  MCtruthTag_ = iConfig.getParameter<edm::InputTag>("MCtruthTag");
  eventType_ = iConfig.getUntrackedParameter<int>("eventType", 1);

  eleId_names_  = iConfig.getParameter< std::vector<std::string> >("eleId_names");
  outTreeNameEleId->Fill();
  
  mcAnalysisZW_ = NULL;

  
  //---- flags ----
  useTriggerEvent_ = iConfig.getUntrackedParameter<bool> ("useTriggerEvent_", true);
  dataFlag_     = iConfig.getUntrackedParameter<bool> ("dataFlag_", true);
  saveL1_       = iConfig.getUntrackedParameter<bool> ("saveL1", true);
  saveBS_       = iConfig.getUntrackedParameter<bool> ("saveBS", true);
  savePV_       = iConfig.getUntrackedParameter<bool> ("savePV", true);
  saveL1_       = iConfig.getUntrackedParameter<bool> ("saveL1", true);
  saveHLT_      = iConfig.getUntrackedParameter<bool> ("saveHLT", true);
  saveEle_      = iConfig.getUntrackedParameter<bool> ("saveEle", true);
  savePho_      = iConfig.getUntrackedParameter<bool> ("savePho", true);
  saveMu_       = iConfig.getUntrackedParameter<bool> ("saveMu", true);
  saveJet_      = iConfig.getUntrackedParameter<bool> ("saveJet", true);
  saveCALOMet_  = iConfig.getUntrackedParameter<bool> ("saveCALOMet", true);
  saveTCMet_    = iConfig.getUntrackedParameter<bool> ("saveTCMet", true);
  savePFMet_    = iConfig.getUntrackedParameter<bool> ("savePFMet", true);
  saveMCPU_     = iConfig.getUntrackedParameter<bool> ("saveMCPU", false);
  saveMCZW_     = iConfig.getUntrackedParameter<bool> ("saveMCZW", false);
  

  verbosity_ = iConfig.getUntrackedParameter<bool>("verbosity", false);
  
  
  
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
    NtupleFactory_->AddFloat("muons_nTkIso03"); 
    NtupleFactory_->AddFloat("muons_nTkIso05"); 
    NtupleFactory_->AddFloat("muons_tkIso03"); 
    NtupleFactory_->AddFloat("muons_tkIso05"); 
    NtupleFactory_->AddFloat("muons_emIso03"); 
    NtupleFactory_->AddFloat("muons_emIso05"); 
    NtupleFactory_->AddFloat("muons_hadIso03"); 
    NtupleFactory_->AddFloat("muons_hadIso05"); 
  }
  
  if(saveEle_)
  {
    EcalClusterCrackCorrection = EcalClusterFunctionFactory::get()->create("EcalClusterCrackCorrection", iConfig);
    EcalClusterLocalContCorrection = EcalClusterFunctionFactory::get()->create("EcalClusterLocalContCorrection", iConfig);
    //SC coordinates wrt the ECAL local system
    PositionCalc dummy(iConfig.getParameter<edm::ParameterSet>("posCalcParameters"));
    positionCalculator = dummy;

    // general variables
    NtupleFactory_->Add4V("electrons");
    NtupleFactory_->AddFloat("electrons_charge"); 
    NtupleFactory_->AddInt("electrons_isEB"); 
    NtupleFactory_->AddInt("electrons_isEBEEGap");
    NtupleFactory_->AddInt("electrons_isEBEtaGap");
    NtupleFactory_->AddInt("electrons_isEBPhiGap");
    NtupleFactory_->AddInt("electrons_isEEDeeGap");
    NtupleFactory_->AddInt("electrons_isEERingGap");
    NtupleFactory_->AddFloat("electrons_ADCToGeVEB");
    NtupleFactory_->AddFloat("electrons_ADCToGeVEE");
    
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
    NtupleFactory_->Add3PV("electrons_scLocalPosition");    
    NtupleFactory_->AddFloat("electrons_scLocalPositionEtaCry");
    NtupleFactory_->AddFloat("electrons_scLocalPositionPhiCry");
    NtupleFactory_->AddFloat("electrons_scE");
    NtupleFactory_->AddFloat("electrons_scEt");
    NtupleFactory_->AddFloat("electrons_scERaw");
    NtupleFactory_->AddFloat("electrons_scEtRaw");
    NtupleFactory_->AddFloat("electrons_scEta");
    NtupleFactory_->AddFloat("electrons_scPhi");
    NtupleFactory_->AddFloat("electrons_scPhiWidth");
    NtupleFactory_->AddFloat("electrons_scEtaWidth");
    NtupleFactory_->AddFloat("electrons_scAvgLaserCorrection");
    NtupleFactory_->AddFloat("electrons_scCrackCorrection");
    NtupleFactory_->AddFloat("electrons_scLocalContCorrection");
    
    // cluster variables
    NtupleFactory_->AddInt("electrons_basicClustersSize");    
    NtupleFactory_->AddFloat("electrons_e1x5");
    NtupleFactory_->AddFloat("electrons_e2x5Max");
    NtupleFactory_->AddFloat("electrons_e5x5");
    NtupleFactory_->AddFloat("electrons_e3x3");
    NtupleFactory_->AddFloat("electrons_e2x2");
    
    // rechit variables
    NtupleFactory_->AddFloat("recHit_E"); 
    NtupleFactory_->AddFloat("recHit_time");
    NtupleFactory_->AddFloat("recHit_ICConstant");
    NtupleFactory_->AddFloat("recHit_laserCorrection");
    NtupleFactory_->AddInt("recHit_ism");
    NtupleFactory_->AddInt("recHit_ieta");
    NtupleFactory_->AddInt("recHit_iphi");
    NtupleFactory_->AddInt("recHit_ix");
    NtupleFactory_->AddInt("recHit_iy");
    NtupleFactory_->AddInt("recHit_zside");
    NtupleFactory_->AddInt("recHit_hashedIndex");
    NtupleFactory_->AddInt("recHit_flag");
    NtupleFactory_->AddInt("recHit_n");
    
    // seed variables
    NtupleFactory_->AddFloat("electrons_seedE");
    NtupleFactory_->AddInt("electrons_seedIsm");
    NtupleFactory_->AddInt("electrons_seedIeta");
    NtupleFactory_->AddInt("electrons_seedIphi");
    NtupleFactory_->AddInt("electrons_seedIx");
    NtupleFactory_->AddInt("electrons_seedIy");
    NtupleFactory_->AddInt("electrons_seedZside");
    NtupleFactory_->AddInt("electrons_seedHashedIndex");
    NtupleFactory_->AddFloat("electrons_seedTime");
    NtupleFactory_->AddInt("electrons_seedFlag");
    NtupleFactory_->AddFloat("electrons_seedICConstant");
    NtupleFactory_->AddFloat("electrons_seedLaserAlpha");
    NtupleFactory_->AddFloat("electrons_seedLaserCorrection");
    NtupleFactory_->AddFloat("electrons_seedSwissCross");
    
    // preshower variables
    NtupleFactory_->AddFloat("electrons_ES");
    
    // id variables
    NtupleFactory_->AddInt("electrons_classification");
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
  }

  if(savePho_)
  {
    EcalClusterCrackCorrection = EcalClusterFunctionFactory::get()->create("EcalClusterCrackCorrection", iConfig);
    EcalClusterLocalContCorrection = EcalClusterFunctionFactory::get()->create("EcalClusterLocalContCorrection", iConfig);
    //SC coordinates wrt the ECAL local system
    PositionCalc dummy(iConfig.getParameter<edm::ParameterSet>("posCalcParameters"));
    positionCalculator = dummy;
    
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
   NtupleFactory_ -> Add3V("photons_SCpos");   

   NtupleFactory_ -> Add3V("photons_convVtx");
   NtupleFactory_ -> AddInt("photons_convNtracks");
   NtupleFactory_ -> AddInt("photons_convVtxIsValid");
   NtupleFactory_ -> AddFloat("photons_convVtxChi2");
   NtupleFactory_ -> AddFloat("photons_convVtxNDOF");
   NtupleFactory_ -> AddFloat("photons_convEoverP");

   NtupleFactory_ ->AddTMatrix("photons_rechitTime");
   NtupleFactory_ ->AddTMatrix("photons_rechitE");
   NtupleFactory_ ->AddTMatrix("photons_rechitIC");
   NtupleFactory_ ->AddTMatrix("photons_rechitLC");

   NtupleFactory_ -> AddFloat("photons_scCrackCorrection");
   NtupleFactory_ -> AddFloat("photons_scLocalContCorrection");
   NtupleFactory_ -> AddFloat("photons_scLocalPositionEtaCry");
   NtupleFactory_ -> AddFloat("photons_scLocalPositionPhiCry");
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
  
  if(saveMCPU_)
  {
    NtupleFactory_ -> AddInt  ("mc_PUit_NumInteractions");
    NtupleFactory_ -> AddFloat("mc_PUit_zpositions");
    NtupleFactory_ -> AddFloat("mc_PUit_sumpT_lowpT");
    NtupleFactory_ -> AddFloat("mc_PUit_sumpT_highpT");
    NtupleFactory_ -> AddInt  ("mc_PUit_ntrks_lowpT");
    NtupleFactory_ -> AddInt  ("mc_PUit_ntrks_highpT");
    
    NtupleFactory_ -> AddInt  ("mc_PUoot_NumInteractions");
    NtupleFactory_ -> AddFloat("mc_PUoot_zpositions");
    NtupleFactory_ -> AddFloat("mc_PUoot_sumpT_lowpT");
    NtupleFactory_ -> AddFloat("mc_PUoot_sumpT_highpT");
    NtupleFactory_ -> AddInt  ("mc_PUoot_ntrks_lowpT");
    NtupleFactory_ -> AddInt  ("mc_PUoot_ntrks_highpT");
  }

  if(saveMCZW_)
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

}

// --------------------------------------------------------------------



SimpleNtupleCalib::~SimpleNtupleCalib ()
{
  cout<< "Analyzed " <<  eventNaiveId_ << " events" <<endl;
  NtupleFactory_->WriteNtuple();
  delete NtupleFactory_;
}

// -----------------------------------------------------------------------------------------



void SimpleNtupleCalib::analyze (const edm::Event& iEvent, const edm::EventSetup& iSetup) {

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

 ///---- fill photons ----
 if (savePho_)  fillPhoInfo (iEvent, iSetup);

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


 ///---- fill MC Pileup information ---- 
 if (saveMCPU_) fillMCPUInfo (iEvent, iSetup);

 //fill W/Z MC information
 if(saveMCZW_)
   {
     edm::Handle<reco::GenParticleCollection> genParticles;
     iEvent.getByLabel(MCtruthTag_, genParticles);
     mcAnalysisZW_ = new MCDumperZW(genParticles, eventType_, verbosity_);
     fillMCZWInfo (iEvent, iSetup);
   }


 ///---- save the entry of the tree ----
 NtupleFactory_->FillNtuple();

}



// -----------------------------------------------------------------------------------------


//---- dump L1Info
void SimpleNtupleCalib::fillL1Info (const edm::Event & iEvent, const edm::EventSetup & iSetup)
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



void SimpleNtupleCalib::fillHLTInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
  //std::cout << "SimpleNtupleCalib::fillHLTInfo::begin" << std::endl;
  
  edm::Handle<trigger::TriggerEvent> triggerEventHandle;
  iEvent.getByLabel(TriggerEventTag_, triggerEventHandle);
  
  const edm::Provenance* provenance = triggerEventHandle.provenance();
  //std::cout << "Trigger process name = " << provenance->processName() << std::endl;
  bool changed(true);
  hltConfig_.init(iEvent.getRun(),iSetup,TriggerResultsTag_.process(),changed);
  
  edm::Handle<edm::TriggerResults> triggerResultsHandle;
  
  if(useTriggerEvent_)
    iEvent.getByLabel(edm::InputTag(TriggerResultsTag_.label(), TriggerResultsTag_.instance(), provenance->processName()), triggerResultsHandle);
  else
    iEvent.getByLabel(TriggerResultsTag_, triggerResultsHandle);
  
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



void SimpleNtupleCalib::fillBSInfo(const edm::Event & iEvent, const edm::EventSetup & iESetup)
{
  //std::cout << "SimpleNtupleCalib::fillBSInfo::begin" << std::endl;
  
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

  //std::cout << "SimpleNtupleCalib::fillBSInfo::end" << std::endl;
}

// -----------------------------------------------------------------------------------------



void SimpleNtupleCalib::fillPVInfo(const edm::Event & iEvent, const edm::EventSetup & iESetup)
{
  //std::cout << "SimpleNtupleCalib::fillPVInfo::begin" << std::endl;
    
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
    NtupleFactory_ -> FillFloat("PV_z", -999.);
    NtupleFactory_ -> FillFloat("PV_d0", -999.);
    NtupleFactory_ -> FillFloat("PV_SumPt",-999.);
  }
  
  math::XYZPoint PVPoint(PV.position().x(), PV.position().y(), PV.position().z());
  PVPoint_ = PVPoint;
  
  //std::cout << "SimpleNtupleCalib::fillPVInfo::end" << std::endl;
}

// -----------------------------------------------------------------------------------------



void SimpleNtupleCalib::fillEleInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{ 
 //*********** CALO TOPOLOGY
 edm::ESHandle<CaloTopology> pTopology;
 iSetup.get<CaloTopologyRecord>().get(pTopology);
 const CaloTopology *topology = pTopology.product();
 
 //*********** CALO GEOM
 edm::ESHandle<CaloGeometry> theCaloGeom;
 iSetup.get<CaloGeometryRecord>().get(theCaloGeom);
 const CaloGeometry *caloGeometry = theCaloGeom.product();

 //*********** IC CONSTANTS
 edm::ESHandle<EcalIntercalibConstants> theICConstants;
 iSetup.get<EcalIntercalibConstantsRcd>().get(theICConstants);
  
 //*********** LASER ALPHAS
 edm::ESHandle<EcalLaserAlphas> theEcalLaserAlphas;
 iSetup.get<EcalLaserAlphasRcd>().get(theEcalLaserAlphas);
 const EcalLaserAlphaMap* theEcalLaserAlphaMap = theEcalLaserAlphas.product();
 
 //*********** ADCToGeV
 edm::ESHandle<EcalADCToGeVConstant> theADCToGeV;
 iSetup.get<EcalADCToGeVConstantRcd>().get(theADCToGeV);
 
 //*********** LASER CORRECTION
 edm::ESHandle<EcalLaserDbService> theLaser;
 iSetup.get<EcalLaserDbRecord>().get(theLaser);
 
 //*********** EB REC HITS
 edm::Handle<EcalRecHitCollection> recHitsEB;
 iEvent.getByLabel( recHitCollection_EB_, recHitsEB );
 const EcalRecHitCollection* theBarrelEcalRecHits = recHitsEB.product () ;
 if ( ! recHitsEB.isValid() ) {
  std::cerr << "SimpleNtupleCalib::analyze --> recHitsEB not found" << std::endl; 
 }
  
 //*********** EE REC HITS
 edm::Handle<EcalRecHitCollection> recHitsEE;
 iEvent.getByLabel( recHitCollection_EE_, recHitsEE );
 const EcalRecHitCollection* theEndcapEcalRecHits = recHitsEE.product () ;
 if ( ! recHitsEE.isValid() ) {
  std::cerr << "SimpleNtupleCalib::analyze --> recHitsEE not found" << std::endl; 
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
   NtupleFactory_->FillFloat("electrons_ADCToGeVEB",theADCToGeV->getEBValue());
   NtupleFactory_->FillFloat("electrons_ADCToGeVEE",theADCToGeV->getEEValue());
   
   
   // track variables
   reco::GsfTrackRef eleTrack  = electron.gsfTrack() ; 
   NtupleFactory_->FillFloat("electrons_dxy_BS", eleTrack->dxy (BSPoint_));
   NtupleFactory_->FillFloat("electrons_dz_BS", eleTrack->dz (BSPoint_));
   NtupleFactory_->FillFloat("electrons_dxy_PV", eleTrack->dxy (PVPoint_));
   NtupleFactory_->FillFloat("electrons_dz_PV", eleTrack->dz (PVPoint_));
   NtupleFactory_->Fill3V("electrons_p_atVtx",(math::XYZVectorD)(electron.trackMomentumAtVtx()));
   NtupleFactory_->Fill3V("electrons_p_out",(math::XYZVectorD)(electron.trackMomentumOut()));
   NtupleFactory_->Fill3V("electrons_p_atCalo",(math::XYZVectorD)(electron.trackMomentumAtCalo()));
   NtupleFactory_->Fill3PV("electrons_position_atVtx",(math::XYZPointD)(electron.trackPositionAtVtx()));
   NtupleFactory_->Fill3PV("electrons_position_atCalo",(math::XYZPointD)(electron.trackPositionAtCalo()));
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
   
   const std::vector<std::pair<DetId,float> >& hits = scRef->hitsAndFractions();
   
   // cluster variables
   float E3x3 = 0;
   float E2x2 = 0;
   math::XYZPoint SClocalPos(0.,0.,0.);

   std::pair<double,double> localPosition;
   localPosition.first = 0.;
   localPosition.second = 0.;

   if ( electron.isEB() )
   {
     E3x3 = EcalClusterTools::e3x3( *scRef, theBarrelEcalRecHits, topology);
     E2x2 = EcalClusterTools::e2x2( *scRef, theBarrelEcalRecHits, topology);
     SClocalPos = positionCalculator.Calculate_Location(hits, theBarrelEcalRecHits, caloGeometry->getSubdetectorGeometry(DetId::Ecal,EcalBarrel));

     localPosition = getLocalPosition(caloGeometry, seedCluster);
     
   }
   if ( electron.isEE() )
   {
     E3x3 = EcalClusterTools::e3x3( *scRef, theEndcapEcalRecHits, topology);
     E2x2 = EcalClusterTools::e2x2( *scRef, theEndcapEcalRecHits, topology);
     SClocalPos = positionCalculator.Calculate_Location(hits, theEndcapEcalRecHits, caloGeometry->getSubdetectorGeometry(DetId::Ecal,EcalEndcap));
   }

   

   //NtupleFactory_->FillFloat("electrons_scLocalPositionEtaCry",EtaCry);
   //NtupleFactory_->FillFloat("electrons_scLocalPositionPhiCry",PhiCry);
   NtupleFactory_->FillFloat("electrons_scLocalPositionEtaCry",localPosition.first);
   NtupleFactory_->FillFloat("electrons_scLocalPositionPhiCry",localPosition.second);
   NtupleFactory_->Fill3PV("electrons_scLocalPosition",SClocalPos);
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
   
   bool printOut = false;
   if( printOut )
   {
     std::cout << "runId: " << iEvent.id().run() 
	       << std::fixed
	       << "   electron eta: " << std::setprecision(2) << std::setw(5) << electron.eta()
	       << "   electron phi: " << std::setprecision(2) << std::setw(5) << electron.phi()
	       << "   SC energy: "    << std::setprecision(2) << std::setw(6) << scRef -> energy()
	       << std::endl;
   } 
   
   const EcalIntercalibConstantMap& ICMap = theICConstants->getMap();
   
   for(std::vector<std::pair<DetId,float> >::const_iterator rh = hits.begin(); rh!=hits.end(); ++rh)
   {
     float rhICConstant = -1.;
     float rhLaserCorrection = -1.;
     
     if ((*rh).first.subdetId()== EcalBarrel)
     {
       EBRecHitCollection::const_iterator itrechit = theBarrelEcalRecHits->find((*rh).first);
       if (itrechit==theBarrelEcalRecHits->end()) continue;
       EBDetId barrelId (itrechit->id ()); 
       NtupleFactory_->FillFloat("recHit_E",itrechit->energy());
       NtupleFactory_->FillFloat("recHit_time",itrechit->time());
       NtupleFactory_->FillInt("recHit_ism",int(barrelId.ism()-1));
       NtupleFactory_->FillInt("recHit_ieta",barrelId.ieta());
       NtupleFactory_->FillInt("recHit_iphi",barrelId.iphi());
       NtupleFactory_->FillInt("recHit_ix",-999);
       NtupleFactory_->FillInt("recHit_iy",-999);
       NtupleFactory_->FillInt("recHit_zside",0);
       NtupleFactory_->FillInt("recHit_hashedIndex",barrelId.hashedIndex());
       NtupleFactory_->FillInt("recHit_flag",itrechit->recoFlag());
       ++numRecHit;
       
       // intercalib constant
       EcalIntercalibConstantMap::const_iterator ICMapIt = ICMap.find(barrelId);
       if( ICMapIt != ICMap.end() )
	 rhICConstant = *ICMapIt;
       NtupleFactory_->FillFloat("recHit_ICConstant",rhICConstant);
              
       // laser correction
       rhLaserCorrection = theLaser->getLaserCorrection(barrelId, iEvent.time());
       NtupleFactory_->FillFloat("recHit_laserCorrection",rhLaserCorrection);
       
       sumRecHitE += itrechit->energy();
       sumLaserCorrectionRecHitE += itrechit->energy() * rhLaserCorrection;
       
       if( printOut && itrechit->energy() > 1. )
       {
         std::cout << std::fixed
		   << ">>> recHitIeta: "  << std::setprecision(0) << std::setw(4) << barrelId.ieta()
		   << "    recHitIphi: "  << std::setprecision(0) << std::setw(4) << barrelId.iphi()
		   << "    recHitE: "     << std::setprecision(2) << std::setw(6) << itrechit->energy()
		   << "    recHitIC: "    << std::setprecision(6) << std::setw(8) << rhICConstant
		   << "    recHitLC: "    << std::setprecision(6) << std::setw(8) << rhLaserCorrection
		   << std::endl;
       }
     }
       
     if ((*rh).first.subdetId()== EcalEndcap)
     {
       EERecHitCollection::const_iterator itrechit = theEndcapEcalRecHits->find((*rh).first);
       if (itrechit==theEndcapEcalRecHits->end()) continue;
       EEDetId endcapId (itrechit->id ()); 
       NtupleFactory_->FillFloat("recHit_E",itrechit->energy());
       NtupleFactory_->FillFloat("recHit_time",itrechit->time());
       NtupleFactory_->FillInt("recHit_ism",int(endcapId.ix()/51+(endcapId.zside()<0 ? 0 : 2 )));
       NtupleFactory_->FillInt("recHit_ix",endcapId.ix());
       NtupleFactory_->FillInt("recHit_iy",endcapId.iy());
       NtupleFactory_->FillInt("recHit_ieta",-999);
       NtupleFactory_->FillInt("recHit_iphi",-999);
       NtupleFactory_->FillInt("recHit_zside",endcapId.zside());
       NtupleFactory_->FillInt("recHit_hashedIndex",endcapId.hashedIndex());
       NtupleFactory_->FillInt("recHit_flag",itrechit->recoFlag());
       ++numRecHit;
       
       // intercalib constant
       EcalIntercalibConstantMap::const_iterator ICMapIt = ICMap.find(endcapId);
       if( ICMapIt != ICMap.end() )
	 rhICConstant = *ICMapIt;
       NtupleFactory_->FillFloat("recHit_ICConstant",rhICConstant);
       
       // laser correction
       rhLaserCorrection = theLaser->getLaserCorrection(endcapId, iEvent.time());
       NtupleFactory_->FillFloat("recHit_laserCorrection",rhLaserCorrection);
       
       sumRecHitE += itrechit->energy();
       sumLaserCorrectionRecHitE += itrechit->energy() * rhLaserCorrection;
       
       if( printOut && itrechit->energy() > 1. )
       {
         std::cout << std::fixed
		   << ">>> recHitIx: "    << std::setprecision(0) << std::setw(4) << endcapId.ix()
		   << "    recHitIy: "    << std::setprecision(0) << std::setw(4) << endcapId.iy()
		   << "    recHitZside: " << std::setprecision(0) << std::setw(4) << endcapId.zside()
		   << "    recHitE: "     << std::setprecision(2) << std::setw(6) << itrechit->energy()
		   << "    recHitIC: "    << std::setprecision(6) << std::setw(8) << rhICConstant
		   << "    recHitLC: "    << std::setprecision(6) << std::setw(8) << rhLaserCorrection
		   << std::endl;
       }
     }
   }
   
   NtupleFactory_->FillInt("recHit_n",numRecHit);
   NtupleFactory_->FillFloat("electrons_scAvgLaserCorrection", sumLaserCorrectionRecHitE/sumRecHitE);
   
   
   // seed variables
   float energy;
   int ism;
   int ieta;
   int iphi;
   int ix;
   int iy;
   int zside;
   int hashedIndex;
   float time; 
   int flag = -1;
   float swissCross;
   float seedICConstant = -1.;
   float seedLaserAlpha = -1.;
   float seedLaserCorrection = -1.;
   
   if(electron.isEB())
   {
     std::pair<DetId, float> id = EcalClusterTools::getMaximum(seedCluster->hitsAndFractions(), theBarrelEcalRecHits);
     
     // flag
     EcalRecHitCollection::const_iterator it = theBarrelEcalRecHits->find(id.first);
     
     if( it != theBarrelEcalRecHits->end() )
     {
       const EcalRecHit& rh = (*it);
       energy = rh.energy();
       ism = int((EBDetId(id.first)).ism() - 1);
       ieta = (EBDetId(id.first)).ieta();
       iphi = (EBDetId(id.first)).iphi();
       ix = -999;
       iy = -999;
       zside = 0;
       hashedIndex = (EBDetId(id.first)).hashedIndex();
       time = rh.time();
       flag = rh.recoFlag();
       swissCross = EcalTools::swissCross(id.first,*theBarrelEcalRecHits,0.);
     }
     
     // intercalib constant
     EcalIntercalibConstantMap::const_iterator ICMapIt = ICMap.find(EBDetId(id.first));
     if( ICMapIt != ICMap.end() )
       seedICConstant = *ICMapIt;
     
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
     
     // flag - OutOfTime
     EcalRecHitCollection::const_iterator it = theEndcapEcalRecHits->find(id.first);
     
     if( it != theEndcapEcalRecHits->end() )
     {
       const EcalRecHit& rh = (*it);
       energy = rh.energy();
       ism = int((EEDetId(id.first)).ix()/51 + ( (EEDetId(id.first)).zside()<0 ? 0 : 2 ));
       ix = (EEDetId(id.first)).ix();
       iy = (EEDetId(id.first)).iy();
       ieta = -999;
       iphi = -999;
       zside = (EEDetId(id.first)).zside();
       hashedIndex = (EEDetId(id.first)).hashedIndex();
       time = rh.time();
       flag = rh.recoFlag();
       swissCross = EcalTools::swissCross(id.first,*theEndcapEcalRecHits,0.);
     }
     
     // intercalib constant
     EcalIntercalibConstantMap::const_iterator ICMapIt = ICMap.find(EEDetId(id.first));
     if( ICMapIt != ICMap.end() )
       seedICConstant = *ICMapIt;
     
     // laser alphas
     EcalLaserAlphaMap::const_iterator italpha = theEcalLaserAlphaMap->find(id.first);
     if( italpha != theEcalLaserAlphaMap->end() )
       seedLaserAlpha = (*italpha);
     
     // laser correction
     seedLaserCorrection = theLaser->getLaserCorrection(EEDetId(id.first), iEvent.time());
   }

   NtupleFactory_->FillFloat("electrons_seedE", energy);
   NtupleFactory_->FillInt("electrons_seedIsm", ism);
   NtupleFactory_->FillInt("electrons_seedIeta", ieta);
   NtupleFactory_->FillInt("electrons_seedIphi", iphi);
   NtupleFactory_->FillInt("electrons_seedIx", ix);
   NtupleFactory_->FillInt("electrons_seedIy", iy);
   NtupleFactory_->FillInt("electrons_seedZside", zside);
   NtupleFactory_->FillInt("electrons_seedHashedIndex", hashedIndex);
   NtupleFactory_->FillFloat("electrons_seedTime", time);
   NtupleFactory_->FillInt("electrons_seedFlag", flag);
   NtupleFactory_->FillFloat("electrons_seedSwissCross", swissCross);
   NtupleFactory_->FillFloat("electrons_seedICConstant", seedICConstant);
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
   
   
   // crack correction variables and local containment corrections
   EcalClusterCrackCorrection -> init(iSetup);
   EcalClusterLocalContCorrection -> init(iSetup);
   double crackcor = 1.;
   double localContCorr = 1.;

   for(reco::CaloCluster_iterator cIt = electron.superCluster()->clustersBegin();
       cIt != electron.superCluster()->clustersEnd(); ++cIt)
     {
       const reco::CaloClusterPtr cc = *cIt; 
       crackcor *= ( (electron.superCluster()->rawEnergy() + (*cIt)->energy()*(EcalClusterCrackCorrection->getValue(*cc)-1.)) / electron.superCluster()->rawEnergy() );
       
     }
   localContCorr = EcalClusterLocalContCorrection->getValue(*electron.superCluster(), 1) ;

   NtupleFactory_->FillFloat("electrons_scCrackCorrection", crackcor);
   NtupleFactory_->FillFloat("electrons_scLocalContCorrection", localContCorr);
   
      
   
 } // end loop over electron candidates

}

// ---- PHOTONS ----
void SimpleNtupleCalib::fillPhoInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup) 
{
 //std::cout << "SimpleNtupleCalib::fillPhotonInfo" << std::endl;
  //*********** CALO TOPOLOGY
 edm::ESHandle<CaloTopology> pTopology;
 iSetup.get<CaloTopologyRecord>().get(pTopology);
 const CaloTopology *topology = pTopology.product();
 
 //*********** CALO GEOM
 edm::ESHandle<CaloGeometry> theCaloGeom;
 iSetup.get<CaloGeometryRecord>().get(theCaloGeom);
 const CaloGeometry *caloGeometry = theCaloGeom.product();

 //*********** IC CONSTANTS
 edm::ESHandle<EcalIntercalibConstants> theICConstants;
 iSetup.get<EcalIntercalibConstantsRcd>().get(theICConstants);
  
 //*********** LASER ALPHAS
 edm::ESHandle<EcalLaserAlphas> theEcalLaserAlphas;
 iSetup.get<EcalLaserAlphasRcd>().get(theEcalLaserAlphas);
 const EcalLaserAlphaMap* theEcalLaserAlphaMap = theEcalLaserAlphas.product();
 
 //*********** ADCToGeV
 edm::ESHandle<EcalADCToGeVConstant> theADCToGeV;
 iSetup.get<EcalADCToGeVConstantRcd>().get(theADCToGeV);
 
 //*********** LASER CORRECTION
 edm::ESHandle<EcalLaserDbService> theLaser;
 iSetup.get<EcalLaserDbRecord>().get(theLaser);

 //*********** PHOTONS
 edm::Handle<edm::View<pat::Photon> > photonHandle;
 iEvent.getByLabel(PhotonTag_,photonHandle);
 edm::View<pat::Photon> photons = *photonHandle;

 //*********** Ecal barrel RecHits 
 edm::Handle<EcalRecHitCollection> pBarrelEcalRecHits ;
 iEvent.getByLabel (recHitCollection_EB_, pBarrelEcalRecHits) ;
 const EcalRecHitCollection* theBarrelEcalRecHits = pBarrelEcalRecHits.product () ;
 
 //*********** Ecal endcap RecHits 
 edm::Handle<EcalRecHitCollection> pEndcapEcalRecHits ;
 iEvent.getByLabel (recHitCollection_EE_, pEndcapEcalRecHits) ;
 const EcalRecHitCollection* theEndcapEcalRecHits = pEndcapEcalRecHits.product () ;

 
 bool printOut = false;
 for ( unsigned int i=0; i<photons.size(); ++i )
 {
  pat::Photon photon = photons.at(i);
  
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
  NtupleFactory_ -> FillFloat("photons_ecalIso",photon.ecalIso());   
  NtupleFactory_ -> FillFloat("photons_hcalIso",photon.hcalIso());   
  NtupleFactory_ -> FillFloat("photons_hadronicOverEm",photon.hadronicOverEm());   
  NtupleFactory_ -> FillFloat("photons_trkSumPtHollowConeDR04",photon.trkSumPtHollowConeDR04());   
  NtupleFactory_ -> FillInt  ("photons_hasPixelSeed",photon.hasPixelSeed());  


  //conversion info
  reco::ConversionRefVector conversions = photon.conversions();
  //if (conversions.size() > 0) std::cout << "conversions.size() = " << conversions.size() << std::endl;
  
  if ( conversions.size() == 1 )
    {
      reco::Conversion conversion = *( conversions.at(0) );

      ROOT::Math::XYZVector conversionVertex(conversion.conversionVertex().x(), conversion.conversionVertex().y(), conversion.conversionVertex().z());
      NtupleFactory_ -> Fill3V   ("photons_convVtx", conversionVertex);
      NtupleFactory_ -> FillInt  ("photons_convVtxIsValid", conversion.conversionVertex().isValid() );
      NtupleFactory_ -> FillFloat("photons_convVtxChi2", conversion.conversionVertex().chi2() );
      NtupleFactory_ -> FillFloat("photons_convVtxNDOF", conversion.conversionVertex().ndof() );
      NtupleFactory_ -> FillFloat("photons_convEoverP", conversion.EoverP() );
      NtupleFactory_ -> FillInt  ("photons_convNtracks", conversion.nTracks());
    }
  else 
    {
      
      ROOT::Math::XYZVector conversionVertex( -999 , -999 , -999);
      NtupleFactory_ -> Fill3V   ("photons_convVtx", conversionVertex);
      NtupleFactory_ -> FillInt  ("photons_convVtxIsValid", 0.);
      NtupleFactory_ -> FillFloat("photons_convVtxChi2", -999 );
      NtupleFactory_ -> FillFloat("photons_convVtxNDOF", -999 );
      NtupleFactory_ -> FillFloat("photons_convEoverP", -999);
      NtupleFactory_ -> FillInt  ("photons_convNtracks", 0);
    }


  //superCluster Info
  reco::SuperClusterRef phoSC = photon.superCluster();
  
  double pos = sqrt(phoSC->x()*phoSC->x() + phoSC->y()*phoSC->y() + phoSC->z()*phoSC->z());
  double ratio = phoSC->energy() / pos;
  ROOT::Math::XYZTVector phoVec(phoSC->x()*ratio, phoSC->y()*ratio, phoSC->z()*ratio, phoSC->energy());
  NtupleFactory_ -> Fill4V("photons_SC", phoVec);
  ROOT::Math::XYZVector phoPos(phoSC->x(), phoSC->y(), phoSC->z());
  NtupleFactory_ -> Fill3V("photons_SCpos", phoPos);

  //recHit time and energy
  TMatrix rechitTime(5,5);
  TMatrix rechitE(5,5);
  TMatrix rechitIC(5,5);
  TMatrix rechitLC(5,5);
  for (int i=0;i<5;i++){
    for (int j=0; j< 5; j++){
      rechitTime[i][j]=999;
      rechitE[i][j]=-999;
      rechitIC[i][j]=-999;
      rechitLC[i][j]=-999;
    }
  }

  //save LC and IC
  float rhICConstant = -1.;
  float rhLaserCorrection = -1.;

  const EcalIntercalibConstantMap& ICMap = theICConstants->getMap();

  if( photon.isEB() ){
    EBDetId ebid = (EcalClusterTools::getMaximum( photon.superCluster()->hitsAndFractions(), theBarrelEcalRecHits )).first;
    for(int xx = 0; xx < 5; ++xx)
      for(int yy = 0; yy < 5; ++yy)
	{
	  std::vector<DetId> vector =  EcalClusterTools::matrixDetId(topology, ebid, xx-2, xx-2, yy-2, yy-2);
	  if(vector.size() == 0) continue;
	  EcalRecHitCollection::const_iterator iterator = theBarrelEcalRecHits->find (vector.at(0)) ;
	  if(iterator == theBarrelEcalRecHits->end()) continue;
	  rechitE[xx][yy]  = iterator -> energy();
	  rechitTime[xx][yy]  = iterator -> time();

	  // intercalib constants
	  EcalIntercalibConstantMap::const_iterator ICMapIt = ICMap.find(vector.at(0));
	  if( ICMapIt != ICMap.end() )
	    rhICConstant = *ICMapIt;
	  rechitIC[xx][yy]  = rhICConstant;

	  // intercalib constants
	  rhLaserCorrection = theLaser->getLaserCorrection(vector.at(0), iEvent.time());
	  rechitLC[xx][yy]  = rhLaserCorrection;

	  if( printOut && iterator->energy() > 1. )
	    {
	      std::cout << std::fixed
			<< ">>> ix: "  << std::setprecision(0) << std::setw(4) << xx
			<< "    iy: "  << std::setprecision(0) << std::setw(4) << yy
			<< "    recHitE: "     << std::setprecision(2) << std::setw(6) << iterator -> energy()
			<< "    recHitIC: "    << std::setprecision(6) << std::setw(8) << rhICConstant
			<< "    recHitLC: "    << std::setprecision(6) << std::setw(8) << rhLaserCorrection
			<< std::endl;
	    }
	}
  }
  else if (  photon.isEE() ){
    EEDetId ebid = (EcalClusterTools::getMaximum( photon.superCluster()->hitsAndFractions(), theEndcapEcalRecHits )).first;
    for(int xx = 0; xx < 5; ++xx)
      for(int yy = 0; yy < 5; ++yy)
	{
	  std::vector<DetId> vector =  EcalClusterTools::matrixDetId(topology, ebid, xx-2, xx-2, yy-2, yy-2);
	  if(vector.size() == 0) continue;
	  EcalRecHitCollection::const_iterator iterator = theEndcapEcalRecHits->find (vector.at(0)) ;
	  if(iterator == theEndcapEcalRecHits->end()) continue;
	  rechitE[xx][yy]  = iterator -> energy();
	  rechitTime[xx][yy]  = iterator -> time();

	  // intercalib constants
	  EcalIntercalibConstantMap::const_iterator ICMapIt = ICMap.find(vector.at(0));
	  if( ICMapIt != ICMap.end() )
	    rhICConstant = *ICMapIt;
	  rechitIC[xx][yy]  = rhICConstant;

	  // intercalib constants
	  rhLaserCorrection = theLaser->getLaserCorrection(vector.at(0), iEvent.time());
	  rechitLC[xx][yy]  = rhLaserCorrection;
	}
  }
  

  NtupleFactory_ -> FillTMatrix("photons_rechitTime",rechitTime);
  NtupleFactory_ -> FillTMatrix("photons_rechitE",rechitE);
  NtupleFactory_ -> FillTMatrix("photons_rechitIC",rechitIC);
  NtupleFactory_ -> FillTMatrix("photons_rechitLC",rechitLC);

  // crack correction variables and local containment corrections
  EcalClusterCrackCorrection -> init(iSetup);
  EcalClusterLocalContCorrection -> init(iSetup);
  double crackcor = 1.;
  double localContCorr = 1.;
  
  for(reco::CaloCluster_iterator cIt = photon.superCluster()->clustersBegin();
      cIt != photon.superCluster()->clustersEnd(); ++cIt)
     {
       const reco::CaloClusterPtr cc = *cIt; 
       crackcor *= ( (photon.superCluster()->rawEnergy() + (*cIt)->energy()*(EcalClusterCrackCorrection->getValue(*cc)-1.)) / photon.superCluster()->rawEnergy() );
       
     }
   localContCorr = EcalClusterLocalContCorrection->getValue(*photon.superCluster(), 1) ;

   const edm::Ptr<reco::CaloCluster>& seedCluster = phoSC->seed();
   std::pair<double,double> localPosition;
   localPosition.first = 0.;
   localPosition.second = 0.;
   if (photon.isEB()) localPosition = getLocalPosition(caloGeometry, seedCluster);


   NtupleFactory_->FillFloat("photons_scCrackCorrection", crackcor);
   NtupleFactory_->FillFloat("photons_scLocalContCorrection", localContCorr);
   NtupleFactory_->FillFloat("photons_scLocalPositionEtaCry",localPosition.first);
   NtupleFactory_->FillFloat("photons_scLocalPositionPhiCry",localPosition.second);
 }
 
}





// ---- MET ----
void SimpleNtupleCalib::fillCALOMetInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 //std::cout << "SimpleNtupleCalib::fillCALOMetInfo" << std::endl;
 
 edm::Handle<edm::View<pat::MET> > calometHandle;
 iEvent.getByLabel(CALOMetTag_,calometHandle);
 View<pat::MET>  mets = *calometHandle;
 //pat::MET met = mets.at(mets.size()-1);
 //std::cout << " mets.size()-1 = " << mets.size()-1 << std::endl;
 pat::MET met = mets.at(0);
 NtupleFactory_->Fill4V("CALOMet",met.p4());
 NtupleFactory_->FillFloat("CALOSumEt",met.sumEt());
}

void SimpleNtupleCalib::fillTCMetInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 //std::cout << "SimpleNtupleCalib::fillTCMetInfo" << std::endl;
 
 edm::Handle<edm::View<pat::MET> > tcmetHandle;
 iEvent.getByLabel(TCMetTag_,tcmetHandle);
 View<pat::MET>  mets = *tcmetHandle;
 //pat::MET met = mets.at(mets.size()-1);
 //std::cout << " mets.size()-1 = " << mets.size()-1 << std::endl;
 pat::MET met = mets.at(0);
 NtupleFactory_->Fill4V("TCMet",met.p4());
 NtupleFactory_->FillFloat("TCSumEt",met.sumEt());
}

void SimpleNtupleCalib::fillPFMetInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 //std::cout << "SimpleNtupleCalib::fillPFMetInfo" << std::endl;
 
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



void SimpleNtupleCalib::fillJetInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 //std::cout << "SimpleNtupleCalib::fillJetInfo" << std::endl;
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
void SimpleNtupleCalib::fillMuInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{
 //std::cout << "SimpleNtupleCalib::fillMuInfo" << std::endl;
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
    NtupleFactory_->FillFloat("muons_tkIso03",(muon.isolationR03()).sumPt);
    NtupleFactory_->FillFloat("muons_nTkIso03",(muon.isolationR03()).nTracks);
    NtupleFactory_->FillFloat("muons_emIso03",(muon.isolationR03()).emEt);
    NtupleFactory_->FillFloat("muons_hadIso03",(muon.isolationR03()).hadEt);

    NtupleFactory_->FillFloat("muons_tkIso05",(muon.isolationR05()).sumPt);
    NtupleFactory_->FillFloat("muons_nTkIso05",(muon.isolationR05()).nTracks);
    NtupleFactory_->FillFloat("muons_emIso05",(muon.isolationR05()).emEt);
    NtupleFactory_->FillFloat("muons_hadIso05",(muon.isolationR05()).hadEt);
  }
} // dumpMuonInfo 



// --pileup---------------------------------------------------------------------------------------

void SimpleNtupleCalib::fillMCPUInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
 //std::cout << "SimpleNtupleCalib::fillMCPUInfo" << std::endl;

  edm::Handle<std::vector<PileupSummaryInfo> > PupInfo;
  iEvent.getByLabel(MCPileupTag_, PupInfo);
  
  // loop on BX
  std::vector<PileupSummaryInfo>::const_iterator PVI;
  for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI)
  {
    // in-time pileup
    if( PVI->getBunchCrossing() == 0 )
    {
      NtupleFactory_->FillInt("mc_PUit_NumInteractions",PVI->getPU_NumInteractions());    
      
      std::vector<float> temp_mc_PU_zpositions   = PVI->getPU_zpositions();
      std::vector<float> temp_mc_PU_sumpT_lowpT  = PVI->getPU_sumpT_lowpT();
      std::vector<float> temp_mc_PU_sumpT_highpT = PVI->getPU_sumpT_highpT();
      std::vector<int> temp_mc_PU_ntrks_lowpT    = PVI->getPU_ntrks_lowpT();
      std::vector<int> temp_mc_PU_ntrks_highpT   = PVI->getPU_ntrks_highpT();
      
     for(std::vector<float>::const_iterator it = temp_mc_PU_zpositions.begin(); it < temp_mc_PU_zpositions.end(); ++it)
       NtupleFactory_->FillFloat("mc_PUit_zpositions",*it);
     
     for(std::vector<float>::const_iterator it = temp_mc_PU_sumpT_lowpT.begin(); it < temp_mc_PU_sumpT_lowpT.end(); ++it)
       NtupleFactory_->FillFloat("mc_PUit_sumpT_lowpT",*it);
     
     for(std::vector<float>::const_iterator it = temp_mc_PU_sumpT_highpT.begin(); it < temp_mc_PU_sumpT_highpT.end(); ++it)
       NtupleFactory_->FillFloat("mc_PUit_sumpT_highpT",*it);
     
     for(std::vector<int>::const_iterator it = temp_mc_PU_ntrks_lowpT.begin(); it < temp_mc_PU_ntrks_lowpT.end(); ++it)
       NtupleFactory_->FillInt("mc_PUit_ntrks_lowpT",*it);
     
     for(std::vector<int>::const_iterator it = temp_mc_PU_ntrks_highpT.begin(); it < temp_mc_PU_ntrks_highpT.end(); ++it)
       NtupleFactory_->FillInt("mc_PUit_ntrks_highpT",*it);
    }
    
    // out-of-time pileup
    else
    {
      NtupleFactory_->FillInt("mc_PUoot_NumInteractions",PVI->getPU_NumInteractions());
      
      std::vector<float> temp_mc_PU_zpositions   = PVI->getPU_zpositions();
      std::vector<float> temp_mc_PU_sumpT_lowpT  = PVI->getPU_sumpT_lowpT();
      std::vector<float> temp_mc_PU_sumpT_highpT = PVI->getPU_sumpT_highpT();
      std::vector<int> temp_mc_PU_ntrks_lowpT    = PVI->getPU_ntrks_lowpT();
      std::vector<int> temp_mc_PU_ntrks_highpT   = PVI->getPU_ntrks_highpT();
      
     for(std::vector<float>::const_iterator it = temp_mc_PU_zpositions.begin(); it < temp_mc_PU_zpositions.end(); ++it)
       NtupleFactory_->FillFloat("mc_PUoot_zpositions",*it);
     
     for(std::vector<float>::const_iterator it = temp_mc_PU_sumpT_lowpT.begin(); it < temp_mc_PU_sumpT_lowpT.end(); ++it)
       NtupleFactory_->FillFloat("mc_PUoot_sumpT_lowpT",*it);
     
     for(std::vector<float>::const_iterator it = temp_mc_PU_sumpT_highpT.begin(); it < temp_mc_PU_sumpT_highpT.end(); ++it)
       NtupleFactory_->FillFloat("mc_PUoot_sumpT_highpT",*it);
     
     for(std::vector<int>::const_iterator it = temp_mc_PU_ntrks_lowpT.begin(); it < temp_mc_PU_ntrks_lowpT.end(); ++it)
       NtupleFactory_->FillInt("mc_PUoot_ntrks_lowpT",*it);
     
     for(std::vector<int>::const_iterator it = temp_mc_PU_ntrks_highpT.begin(); it < temp_mc_PU_ntrks_highpT.end(); ++it)
       NtupleFactory_->FillInt("mc_PUoot_ntrks_highpT",*it);
    }
      
  } // loop on BX
  
}// dump MC PU info



void SimpleNtupleCalib::fillMCZWInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup) 
{
 //std::cout << "SimpleNtupleCalib::fillMCZWDecayInfo" << std::endl; 

 bool isValid = mcAnalysisZW_ -> isValid();
  
 if( (eventType_ == 0) && (isValid == true) )
 {

   NtupleFactory_->Fill4V("mc_V",mcAnalysisZW_ -> mcV()->p4());
   NtupleFactory_->FillFloat("mc_V_charge",mcAnalysisZW_ -> mcV()->charge());
   NtupleFactory_->FillFloat("mcV_pdgId",mcAnalysisZW_ -> mcV()->pdgId());

   math::XYZPoint p(mcAnalysisZW_ -> mcV()->vertex());
   ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag> vertex(p.x(), p.y(), p.z());
   NtupleFactory_->Fill3V("mc_V_vertex", vertex);   
   
   NtupleFactory_->Fill4V("mcQ1_tag",mcAnalysisZW_ -> mcQ1_tag()->p4());
   NtupleFactory_->FillFloat("mcQ1_tag_charge",mcAnalysisZW_ -> mcQ1_tag()->charge());
   NtupleFactory_->FillFloat("mcQ1_tag_pdgId",mcAnalysisZW_ -> mcQ1_tag()->pdgId());
   
   NtupleFactory_->Fill4V("mcQ2_tag",mcAnalysisZW_ -> mcQ2_tag()->p4());
   NtupleFactory_->FillFloat("mcQ2_tag_charge",mcAnalysisZW_ -> mcQ2_tag()->charge());
   NtupleFactory_->FillFloat("mcQ2_tag_pdgId",mcAnalysisZW_ -> mcQ2_tag()->pdgId());
   
   NtupleFactory_->Fill4V("mcF1_fromV",mcAnalysisZW_ -> mcF1_fromV()->p4());
   NtupleFactory_->FillFloat("mcF1_fromV_charge",mcAnalysisZW_ -> mcF1_fromV()->charge());
   NtupleFactory_->FillFloat("mcF1_fromV_pdgId",mcAnalysisZW_ -> mcF1_fromV()->pdgId());
   
   NtupleFactory_->Fill4V("mcF2_fromV",mcAnalysisZW_ -> mcF2_fromV()->p4());
   NtupleFactory_->FillFloat("mcF2_fromV_charge",mcAnalysisZW_ -> mcF2_fromV()->charge());
   NtupleFactory_->FillFloat("mcF2_fromV_pdgId",mcAnalysisZW_ -> mcF2_fromV()->pdgId());
 } 
 
} 


std::pair<double,double> SimpleNtupleCalib::getLocalPosition(const CaloGeometry *caloGeometry, const edm::Ptr<reco::CaloCluster>& seedCluster)
{
  //--------------if barrel calculate local position wrt xtal center -------------------
  const CaloSubdetectorGeometry* geom = caloGeometry->getSubdetectorGeometry(DetId::Ecal,EcalBarrel);//EcalBarrel = 1
  
  const math::XYZPoint position_ = seedCluster->position(); 
  double Theta = -position_.theta()+0.5*TMath::Pi();
  double Eta = position_.eta();
  double Phi = TVector2::Phi_mpi_pi(position_.phi());
  
     //Calculate expected depth of the maximum shower from energy (like in PositionCalc::Calculate_Location()):
     // The parameters X0 and T0 are hardcoded here because these values were used to calculate the corrections:
     const float X0 = 0.89; const float T0 = 7.4;
     double depth = X0 * (T0 + log(seedCluster->energy()));
  
  
     //search which crystal is closest to the cluster position and call it crystalseed:
     //std::vector<DetId> crystals_vector = *scRef.getHitsByDetId();   //deprecated
     std::vector< std::pair<DetId, float> > crystals_vector = seedCluster->hitsAndFractions();
     float dphimin=999.;
     float detamin=999.;
     int ietaclosest = 0;
     int iphiclosest = 0;
     for (unsigned int icry=0; icry!=crystals_vector.size(); ++icry) 
       {    
	 EBDetId crystal(crystals_vector[icry].first);
	 const CaloCellGeometry* cell=geom->getGeometry(crystal);
	 GlobalPoint center_pos = (dynamic_cast<const TruncatedPyramid*>(cell))->getPosition(depth);
	 double EtaCentr = center_pos.eta();
	 double PhiCentr = TVector2::Phi_mpi_pi(center_pos.phi());
	 if (TMath::Abs(EtaCentr-Eta) < detamin) {
	   detamin = TMath::Abs(EtaCentr-Eta); 
	   ietaclosest = crystal.ieta();
	 }
	 if (TMath::Abs(TVector2::Phi_mpi_pi(PhiCentr-Phi)) < dphimin) {
	   dphimin = TMath::Abs(TVector2::Phi_mpi_pi(PhiCentr-Phi)); 
	   iphiclosest = crystal.iphi();
	 }
       }
     EBDetId crystalseed(ietaclosest, iphiclosest);
  
     // Get center cell position from shower depth
     const CaloCellGeometry* cell=geom->getGeometry(crystalseed);
     GlobalPoint center_pos = (dynamic_cast<const TruncatedPyramid*>(cell))->getPosition(depth);
     
     //PHI
     double PhiCentr = TVector2::Phi_mpi_pi(center_pos.phi());
     double PhiWidth = (TMath::Pi()/180.);
     double PhiCry = (TVector2::Phi_mpi_pi(Phi-PhiCentr))/PhiWidth;
     if (PhiCry>0.5) PhiCry=0.5;
     if (PhiCry<-0.5) PhiCry=-0.5;
     //flip to take into account ECAL barrel symmetries:
     if (ietaclosest<0) PhiCry *= -1.;
   
     //ETA
      double ThetaCentr = -center_pos.theta()+0.5*TMath::Pi();
      double ThetaWidth = (TMath::Pi()/180.)*TMath::Cos(ThetaCentr);
      double EtaCry = (Theta-ThetaCentr)/ThetaWidth;    
      if (EtaCry>0.5) EtaCry=0.5;
      if (EtaCry<-0.5) EtaCry=-0.5;
      //flip to take into account ECAL barrel symmetries:
      if (ietaclosest<0) EtaCry *= -1.;
      
      //-------------- end calculate local position -------------

      std::pair<double,double> etaphi(EtaCry,PhiCry);
      return etaphi;
}


// -----------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(SimpleNtupleCalib);