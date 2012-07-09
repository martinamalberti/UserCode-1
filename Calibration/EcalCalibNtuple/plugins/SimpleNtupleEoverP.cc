// -*- C++ -*-
//
// Package:   SimpleNtupleEoverP
// Class:     SimpleNtupleEoverP
//
#include "Calibration/EcalCalibNtuple/plugins/SimpleNtupleEoverP.h"
#include "RecoEgamma/EgammaTools/interface/EcalClusterLocal.h"
#include "PhysicsTools/NtupleUtils/interface/readJSONFile.h"

#include "Math/Vector4D.h"
#include "Math/Vector3D.h"

#define PI 3.141592654
#define TWOPI 6.283185308


SimpleNtupleEoverP::SimpleNtupleEoverP(const edm::ParameterSet& iConfig)
{
  // Initialize parameters for cluster corrections
  InitializeParams(params);

  edm::Service<TFileService> fs ;
  outTree_  =        fs -> make <TTree>("ntu","ntu"); 

  PVTag_       = iConfig.getParameter<edm::InputTag>("PVTag");
  recHitCollection_EB_ = iConfig.getParameter<edm::InputTag>("recHitCollection_EB");
  recHitCollection_EE_ = iConfig.getParameter<edm::InputTag>("recHitCollection_EE");
  
  EleTag_ = iConfig.getParameter<edm::InputTag>("EleTag");
  

  PFMetTag_ = iConfig.getParameter<edm::InputTag>("PFMetTag");
  
  rhoTag_ = iConfig.getParameter<edm::InputTag>("rhoTag");
      
  eventType_ = iConfig.getUntrackedParameter<int>("eventType", 1);

  jsonFileName_  = iConfig.getParameter<std::string>("jsonFileName");
 
  dataRun_ = iConfig.getParameter<std::string>("dataRun");
  //---- flags ----
  jsonFlag_ = iConfig.getUntrackedParameter<bool>("jsonFlag", false);
  verbosity_ = iConfig.getUntrackedParameter<bool>("verbosity", false);
  doWZSelection_= iConfig.getUntrackedParameter<bool>("doWZSelection", false);
  applyCorrections_ = iConfig.getUntrackedParameter<bool>("applyCorrections", false);
  dataFlag_ = iConfig.getUntrackedParameter<bool>("dataFlag", true) ;
  saveMCPU_     = iConfig.getUntrackedParameter<bool> ("saveMCPU", false);

  eventNaiveId_ = 0;
  //---- Initialize tree branches ----
  
  // event variables
  outTree_ -> Branch("eventId",       &eventId,                 "eventId/L");
  outTree_ -> Branch("lumiId",        &lumiId,                 "lumiId/I");
  outTree_ -> Branch("runId",         &runId,                 "runId/I");
  outTree_ -> Branch("timeStampHigh", &timeStampHigh, "timeStampHigh/I");
  outTree_ -> Branch("isW", &isW, "isW/I");
  outTree_ -> Branch("isZ", &isZ, "isZ/I");  

  outTree_ -> Branch("PV_n",        &PV_n,                 "PV_n/I");
  outTree_ -> Branch("PV_z",        &PV_z,                 "PV_z/F");
  outTree_ -> Branch("PV_d0",       &PV_d0,               "PV_d0/F");
  outTree_ -> Branch("PV_SumPt",    &PV_SumPt,         "PV_SumPt/F");

  outTree_ -> Branch("rho",   &rho,  "rho/F");

 


  // ele1 variables
 
  outTree_ -> Branch("ele1_charge",     &ele1_charge,         "ele1_charge/F");
  outTree_ -> Branch("ele1_p",     &ele1_p,         "ele1_p/F");
  outTree_ -> Branch("ele1_pt",     &ele1_pt,         "ele1_pt/F");
  outTree_ -> Branch("ele1_eta",     &ele1_eta,         "ele1_eta/F");
  outTree_ -> Branch("ele1_phi",     &ele1_phi,         "ele1_phi/F");

  outTree_ -> Branch("ele1_sigmaIetaIeta",     &ele1_sigmaIetaIeta,         "ele1_sigmaIetaIeta/F");
  outTree_ -> Branch("ele1_DphiIn",     &ele1_DphiIn,         "ele1_DphiIn/F");
  outTree_ -> Branch("ele1_DetaIn",     &ele1_DetaIn,         "ele1_DetaIn/F");
  outTree_ -> Branch("ele1_HOverE",     &ele1_HOverE,         "ele1_HOverE/F");
  outTree_ -> Branch("ele1_tkIso",     &ele1_tkIso,         "ele1_tkIso/F");
  outTree_ -> Branch("ele1_emIso",     &ele1_emIso,         "ele1_emIso/F");
  outTree_ -> Branch("ele1_hadIso",     &ele1_hadIso,         "ele1_hadIso/F");

  outTree_ -> Branch("ele1_dxy_PV",     &ele1_dxy_PV,         "ele1_dxy_PV/F");
  outTree_ -> Branch("ele1_dz_PV",     &ele1_dz_PV,         "ele1_dz_PV/F");
  outTree_ -> Branch("ele1_deltaEtaSuperClusterAtVtx",     &ele1_deltaEtaSuperClusterAtVtx,         "ele1_deltaEtaSuperClusterAtVtx/F");
  outTree_ -> Branch("ele1_deltaPhiSuperClusterAtVtx",     &ele1_deltaPhiSuperClusterAtVtx,         "ele1_hadele1_deltaPhiSuperClusterAtVtxIso/F");
  outTree_ -> Branch("ele1_sigmaP",     &ele1_sigmaP,         "ele1_sigmaP/F");
   
  outTree_ -> Branch("ele1_scERaw",     &ele1_scERaw,         "ele1_scERaw/F");
  outTree_ -> Branch("ele1_scEtRaw",    &ele1_scEtRaw,       "ele1_scEtRaw/F");
  outTree_ -> Branch("ele1_scEt",       &ele1_scEt,             "ele1_scEt/F");
  outTree_ -> Branch("ele1_scLocalEta",    &ele1_scLocalEta,         "ele1_scLocalEta/F");
  outTree_ -> Branch("ele1_scLocalPhi",    &ele1_scLocalPhi,         "ele1_scLocalPhi/F");
  outTree_ -> Branch("ele1_scEtaWidth",    &ele1_scEtaWidth,         "ele1_scEtaWidth/F");
  outTree_ -> Branch("ele1_scPhiWidth",    &ele1_scPhiWidth,         "ele1_scPhiWidth/F");
  outTree_ -> Branch("ele1_scCrackCorr",   &ele1_scCrackCorr,        "ele1_scCrackCorr/F");
  outTree_ -> Branch("ele1_scLocalContCorr",    &ele1_scLocalContCorr,         "ele1_scLocalContCorr/F");
 
  outTree_ -> Branch("ele1_scE",    &ele1_scE,         "ele1_scE/F");
  outTree_ -> Branch("ele1_scEta",  &ele1_scEta,     "ele1_scEta/F");
  outTree_ -> Branch("ele1_scPhi",  &ele1_scPhi,     "ele1_scPhi/F");
  outTree_ -> Branch("ele1_scLaserCorr",       &ele1_scLaserCorr,         "ele1_scLaserCorr/F");
  outTree_ -> Branch("ele1_scE_regression",    &ele1_scE_regression,   "ele1_scE_regression/F");
  outTree_ -> Branch("ele1_scEerr_regression",       &ele1_scEerr_regression,         "ele1_scEerr_regression/F");
  outTree_ -> Branch("ele1_scERaw_PUcleaned",    &ele1_scERaw_PUcleaned,   "ele1_scERaw_PUcleaned/F");
  outTree_ -> Branch("ele1_scEtaWidth_PUcleaned",       &ele1_scEtaWidth_PUcleaned,         "ele1_scEtaWidth_PUcleaned/F");
  outTree_ -> Branch("ele1_scPhiWidth_PUcleaned",    &ele1_scPhiWidth_PUcleaned,   "ele1_scPhiWidth_PUcleaned/F");
  outTree_ -> Branch("ele1_fCorrection_PUcleaned",       &ele1_fCorrection_PUcleaned,         "ele1_fCorrection_PUcleaned/F");


  outTree_ -> Branch("ele1_fEta",       &ele1_fEta,   "ele1_fEta/F");
  outTree_ -> Branch("ele1_fEtaCorr",   &ele1_fEtaCorr,         "ele1_fEtaCorr/F");
  outTree_ -> Branch("ele1_tkP",        &ele1_tkP,   "ele1_tkP/F");
  outTree_ -> Branch("ele1_tkPt",       &ele1_tkPt,  "ele1_tkPt/F");
  outTree_ -> Branch("ele1_fbrem",       &ele1_fbrem,  "ele1_fbrem/F");

  outTree_ -> Branch("ele1_e5x5",       &ele1_e5x5,  "ele1_e5x5/F");
  outTree_ -> Branch("ele1_e3x3",       &ele1_e3x3,  "ele1_e3x3/F");
  outTree_ -> Branch("ele1_scNxtal",       &ele1_scNxtal,  "ele1_scNxtal/F");
  outTree_ -> Branch("ele1_5x5LaserCorr",       &ele1_5x5LaserCorr,  "ele1_5x5LaserCorr/F");
  outTree_ -> Branch("ele1_3x3LaserCorr",       &ele1_3x3LaserCorr,  "ele1_3x3LaserCorr/F");
  outTree_ -> Branch("ele1_es",       &ele1_es,  "ele1_es/F");
 
  outTree_ -> Branch("ele1_seedE",       &ele1_seedE,  "ele1_seedE/F");
  outTree_ -> Branch("ele1_seedLaserAlpha",       &ele1_seedLaserAlpha,  "ele1_seedLaserAlpha/F");
  outTree_ -> Branch("ele1_seedLaserCorr",       &ele1_seedLaserCorr,  "ele1_seedLaserCorr/F");
  outTree_ -> Branch("ele1_seedICConstant",       &ele1_seedICConstant,  "ele1_seedICConstant/F");
  outTree_ -> Branch("ele1_seedIeta",       &ele1_seedIeta,  "ele1_seedIeta/I");
  outTree_ -> Branch("ele1_seedIphi",       &ele1_seedIphi,  "ele1_seedIphi/I");
  outTree_ -> Branch("ele1_seedIx",       &ele1_seedIx,  "ele1_seedIx/I");
  outTree_ -> Branch("ele1_seedIy",       &ele1_seedIy,  "ele1_seedIy/I");
  outTree_ -> Branch("ele1_seedZside",       &ele1_seedZside,  "ele1_seedZside/I");
  outTree_ -> Branch("ele1_EOverP",       &ele1_EOverP,  "ele1_EOverP/F");

  outTree_ -> Branch("ele1_recHit_E",              "std::vector<float>", &ele1_recHit_E);
  outTree_ -> Branch("ele1_recHit_hashedIndex", "std::vector<int>",   &ele1_recHit_hashedIndex);
  outTree_ -> Branch("ele1_recHit_ietaORix", "std::vector<int>",   &ele1_recHit_ietaORix);
  outTree_ -> Branch("ele1_recHit_iphiORiy", "std::vector<int>",   &ele1_recHit_iphiORiy);
  outTree_ -> Branch("ele1_recHit_zside", "std::vector<int>",   &ele1_recHit_zside);
  outTree_ -> Branch("ele1_recHit_laserCorrection","std::vector<float>", &ele1_recHit_laserCorrection);
  outTree_ -> Branch("ele1_recHit_ICConstant",     "std::vector<float>", &ele1_recHit_ICConstant);

  outTree_ -> Branch("ele1_nRecHits",       &ele1_nRecHits,  "ele1_nRecHits/I");
  outTree_ -> Branch("ele1_isEB",       &ele1_isEB,  "ele1_isEB/I");
  outTree_ -> Branch("ele1_isEBEEGap",       &ele1_isEBEEGap,  "ele1_isEBEEGap/I");
  outTree_ -> Branch("ele1_isEBEtaGap",       &ele1_isEBEtaGap,  "ele1_isEBEtaGap/I");
  outTree_ -> Branch("ele1_isEBPhiGap",       &ele1_isEBPhiGap,  "ele1_isEBPhiGap/I");
  outTree_ -> Branch("ele1_isEEDeeGap",       &ele1_isEEDeeGap,  "ele1_isEEDeeGap/I");
  outTree_ -> Branch("ele1_isEERingGap",       &ele1_isEERingGap,  "ele1_isEERingGap/I");


  outTree_ -> Branch("ele1_eRegrInput_rawE", &ele1_eRegrInput_rawE, "ele1_eRegrInput_rawE/F");
  outTree_ -> Branch("ele1_eRegrInput_r9", &ele1_eRegrInput_r9, "ele1_eRegrInput_r9/F");
  outTree_ -> Branch("ele1_eRegrInput_eta", &ele1_eRegrInput_eta, "ele1_eRegrInput_eta/F");
  outTree_ -> Branch("ele1_eRegrInput_phi", &ele1_eRegrInput_phi, "ele1_eRegrInput_phi/F");
  outTree_ -> Branch("ele1_eRegrInput_r25", &ele1_eRegrInput_r25, "ele1_eRegrInput_r25/F");
  outTree_ -> Branch("ele1_eRegrInput_hoe", &ele1_eRegrInput_hoe, "ele1_eRegrInput_hoe/F");
  outTree_ -> Branch("ele1_eRegrInput_etaW", &ele1_eRegrInput_etaW, "ele1_eRegrInput_etaW/F");
  outTree_ -> Branch("ele1_eRegrInput_phiW", &ele1_eRegrInput_phiW, "ele1_eRegrInput_phiW/F");
  outTree_ -> Branch("ele1_eRegrInput_Deta_bC_sC", &ele1_eRegrInput_Deta_bC_sC, "ele1_eRegrInput_Deta_bC_sC/F");
  outTree_ -> Branch("ele1_eRegrInput_Dphi_bC_sC", &ele1_eRegrInput_Dphi_bC_sC, "ele1_eRegrInput_Dphi_bC_sC/F");
  outTree_ -> Branch("ele1_eRegrInput_bCE_Over_sCE", &ele1_eRegrInput_bCE_Over_sCE, "ele1_eRegrInput_bCE_Over_sCE/F");
  outTree_ -> Branch("ele1_eRegrInput_e3x3_Over_bCE", &ele1_eRegrInput_e3x3_Over_bCE, "ele1_eRegrInput_e3x3_Over_bCE/F");
  outTree_ -> Branch("ele1_eRegrInput_e5x5_Over_bCE", &ele1_eRegrInput_e5x5_Over_bCE, "ele1_eRegrInput_e5x5_Over_bCE/F");
  outTree_ -> Branch("ele1_eRegrInput_sigietaieta_bC1", &ele1_eRegrInput_sigietaieta_bC1, "ele1_eRegrInput_sigietaieta_bC1/F");
  outTree_ -> Branch("ele1_eRegrInput_sigietaieta_bC1", &ele1_eRegrInput_sigietaieta_bC1, "ele1_eRegrInput_sigietaieta_bC1/F");
  outTree_ -> Branch("ele1_eRegrInput_sigiphiiphi_bC1", &ele1_eRegrInput_sigiphiiphi_bC1, "ele1_eRegrInput_sigiphiiphi_bC1/F");
  outTree_ -> Branch("ele1_eRegrInput_sigietaiphi_bC1", &ele1_eRegrInput_sigietaiphi_bC1, "ele1_eRegrInput_sigietaiphi_bC1/F");
  outTree_ -> Branch("ele1_eRegrInput_bEMax_Over_bCE", &ele1_eRegrInput_bEMax_Over_bCE,
"ele1_eRegrInput_bEMax_Over_bCE/F");
  outTree_ -> Branch("ele1_eRegrInput_log_bE2nd_Over_bEMax", &ele1_eRegrInput_log_bE2nd_Over_bEMax, "ele1_eRegrInput_log_bE2nd_Over_bEMax/F");
  outTree_ -> Branch("ele1_eRegrInput_log_bEtop_Over_bEMax", &ele1_eRegrInput_log_bEtop_Over_bEMax, "ele1_eRegrInput_log_bEtop_Over_bEMax/F");
  outTree_ -> Branch("ele1_eRegrInput_log_bEbot_Over_bEMax", &ele1_eRegrInput_log_bEbot_Over_bEMax, "ele1_eRegrInput_log_bEbot_Over_bEMax/F");
  outTree_ -> Branch("ele1_eRegrInput_log_bEleft_Over_bEMax", &ele1_eRegrInput_log_bEleft_Over_bEMax, "ele1_eRegrInput_log_bEleft_Over_bEMax/F");
  outTree_ -> Branch("ele1_eRegrInput_log_bEright_Over_bEMax", &ele1_eRegrInput_log_bEright_Over_bEMax, "ele1_eRegrInput_log_bEright_Over_bEMax/F");
  outTree_ -> Branch("ele1_eRegrInput_asym_top_bottom", &ele1_eRegrInput_asym_top_bottom, "ele1_eRegrInput_asym_top_bottom/F");
  outTree_ -> Branch("ele1_eRegrInput_asym_left_right", &ele1_eRegrInput_asym_left_right, "ele1_eRegrInput_asym_left_right/F");
  outTree_ -> Branch("ele1_eRegrInput_Deta_bC2_sC", &ele1_eRegrInput_Deta_bC2_sC, "ele1_eRegrInput_Deta_bC2_sC/F");
  outTree_ -> Branch("ele1_eRegrInput_Dphi_bC2_sC", &ele1_eRegrInput_Dphi_bC2_sC, "ele1_eRegrInput_Dphi_bC2_sC/F");
  outTree_ -> Branch("ele1_eRegrInput_bCE2_Over_sCE", &ele1_eRegrInput_bCE2_Over_sCE, "ele1_eRegrInput_bCE2_Over_sCE/F");
  outTree_ -> Branch("ele1_eRegrInput_e3x3_Over_bCE2", &ele1_eRegrInput_e3x3_Over_bCE2, "ele1_eRegrInput_e3x3_Over_bCE2/F");
  outTree_ -> Branch("ele1_eRegrInput_e5x5_Over_bCE2", &ele1_eRegrInput_e5x5_Over_bCE2, "ele1_eRegrInput_e5x5_Over_bCE2/F");
  outTree_ -> Branch("ele1_eRegrInput_sigietaieta_bC2", &ele1_eRegrInput_sigietaieta_bC2, "ele1_eRegrInput_sigietaieta_bC2/F");
  outTree_ -> Branch("ele1_eRegrInput_sigiphiiphi_bC2", &ele1_eRegrInput_sigiphiiphi_bC2, "ele1_eRegrInput_sigiphiiphi_bC2/F");
  outTree_ -> Branch("ele1_eRegrInput_sigietaiphi_bC2", &ele1_eRegrInput_sigietaiphi_bC2, "ele1_eRegrInput_sigietaiphi_bC2/F");
  outTree_ -> Branch("ele1_eRegrInput_bEMax_Over_bCE2", &ele1_eRegrInput_bEMax_Over_bCE2, "ele1_eRegrInput_bEMax_Over_bCE2/F");
  outTree_ -> Branch("ele1_eRegrInput_log_bE2nd_Over_bEMax2", &ele1_eRegrInput_log_bE2nd_Over_bEMax2, "ele1_eRegrInput_log_bE2nd_Over_bEMax2/F");
  outTree_ -> Branch("ele1_eRegrInput_log_bEtop_Over_bEMax2", &ele1_eRegrInput_log_bEtop_Over_bEMax2, "ele1_eRegrInput_log_bEtop_Over_bEMax2/F");
  outTree_ -> Branch("ele1_eRegrInput_log_bEbot_Over_bEMax2", &ele1_eRegrInput_log_bEbot_Over_bEMax2, "ele1_eRegrInput_log_bEbot_Over_bEMax2/F");
  outTree_ -> Branch("ele1_eRegrInput_log_bEleft_Over_bEMax2", &ele1_eRegrInput_log_bEleft_Over_bEMax2, "ele1_eRegrInput_log_bEleft_Over_bEMax2/F");
  outTree_ -> Branch("ele1_eRegrInput_log_bEright_Over_bEMax2", &ele1_eRegrInput_log_bEright_Over_bEMax2, "ele1_eRegrInput_log_bEright_Over_bEMax2/F");
  outTree_ -> Branch("ele1_eRegrInput_asym_top2_bottom2", &ele1_eRegrInput_asym_top2_bottom2, "ele1_eRegrInput_asym_top2_bottom2/F");
  outTree_ -> Branch("ele1_eRegrInput_asym_left2_right2", &ele1_eRegrInput_asym_left2_right2, "ele1_eRegrInput_asym_left2_right2/F");
  outTree_ -> Branch("ele1_eRegrInput_Deta_bCLow_sC", &ele1_eRegrInput_Deta_bCLow_sC, "ele1_eRegrInput_Deta_bCLow_sC/F");
  outTree_ -> Branch("ele1_eRegrInput_Dphi_bCLow_sC", &ele1_eRegrInput_Dphi_bCLow_sC, "ele1_eRegrInput_Dphi_bCLow_sC/F");
  outTree_ -> Branch("ele1_eRegrInput_bCELow_Over_sCE", &ele1_eRegrInput_bCELow_Over_sCE, "ele1_eRegrInput_bCELow_Over_sCE/F");
  outTree_ -> Branch("ele1_eRegrInput_e3x3_Over_bCELow", &ele1_eRegrInput_e3x3_Over_bCELow, "ele1_eRegrInput_e3x3_Over_bCELow/F");
  outTree_ -> Branch("ele1_eRegrInput_e5x5_Over_bCELow", &ele1_eRegrInput_e5x5_Over_bCELow, "ele1_eRegrInput_e5x5_Over_bCELow/F");
  outTree_ -> Branch("ele1_eRegrInput_sigietaieta_bCLow", &ele1_eRegrInput_sigietaieta_bCLow, "ele1_eRegrInput_sigietaieta_bCLow/F");
  outTree_ -> Branch("ele1_eRegrInput_e5x5_Over_bCELow", &ele1_eRegrInput_e5x5_Over_bCELow, "ele1_eRegrInput_e5x5_Over_bCELow/F");
  outTree_ -> Branch("ele1_eRegrInput_sigietaieta_bCLow", &ele1_eRegrInput_sigietaieta_bCLow, "ele1_eRegrInput_sigietaieta_bCLow/F");
  outTree_ -> Branch("ele1_eRegrInput_sigiphiiphi_bCLow", &ele1_eRegrInput_sigiphiiphi_bCLow, "ele1_eRegrInput_e5x5_Over_bCELow/F");
  outTree_ -> Branch("ele1_eRegrInput_sigietaiphi_bCLow", &ele1_eRegrInput_sigietaiphi_bCLow, "ele1_eRegrInput_sigietaiphi_bCLow/F");
  outTree_ -> Branch("ele1_eRegrInput_Deta_bCLow2_sC", &ele1_eRegrInput_Deta_bCLow2_sC, "ele1_eRegrInput_Deta_bCLow2_sC/F");
  outTree_ -> Branch("ele1_eRegrInput_Dphi_bCLow2_sC", &ele1_eRegrInput_Dphi_bCLow2_sC, "ele1_eRegrInput_Dphi_bCLow2_sC/F");
  outTree_ -> Branch("ele1_eRegrInput_bCELow2_Over_sCE", &ele1_eRegrInput_bCELow2_Over_sCE, "ele1_eRegrInput_bCELow2_Over_sCE/F");
  outTree_ -> Branch("ele1_eRegrInput_e3x3_Over_bCELow2", &ele1_eRegrInput_e3x3_Over_bCELow2, "ele1_eRegrInput_e3x3_Over_bCELow2/F");
  outTree_ -> Branch("ele1_eRegrInput_e5x5_Over_bCELow2", &ele1_eRegrInput_e5x5_Over_bCELow2, "ele1_eRegrInput_e5x5_Over_bCELow2/F");
  outTree_ -> Branch("ele1_eRegrInput_sigietaieta_bCLow2", &ele1_eRegrInput_sigietaieta_bCLow2, "ele1_eRegrInput_sigietaieta_bCLow2/F");
  outTree_ -> Branch("ele1_eRegrInput_sigiphiiphi_bCLow2", &ele1_eRegrInput_sigiphiiphi_bCLow2, "ele1_eRegrInput_sigiphiiphi_bCLow2/F");
  outTree_ -> Branch("ele1_eRegrInput_sigietaiphi_bCLow2", &ele1_eRegrInput_sigietaiphi_bCLow2, "ele1_eRegrInput_sigietaiphi_bCLow2/F");
  outTree_ -> Branch("ele1_eRegrInput_seedbC_eta", &ele1_eRegrInput_seedbC_eta, "ele1_eRegrInput_seedbC_eta/F");
  outTree_ -> Branch("ele1_eRegrInput_seedbC_phi", &ele1_eRegrInput_seedbC_phi, "ele1_eRegrInput_seedbC_phi/F");
  outTree_ -> Branch("ele1_eRegrInput_seedbC_eta_p5", &ele1_eRegrInput_seedbC_eta_p5, "ele1_eRegrInput_seedbC_eta_p5/F");
  outTree_ -> Branch("ele1_eRegrInput_seedbC_phi_p2", &ele1_eRegrInput_seedbC_phi_p2, "ele1_eRegrInput_seedbC_phi_p2/F");
  outTree_ -> Branch("ele1_eRegrInput_seedbC_bieta", &ele1_eRegrInput_seedbC_bieta, "ele1_eRegrInput_seedbC_bieta/F");
  outTree_ -> Branch("ele1_eRegrInput_seedbC_phi_p20", &ele1_eRegrInput_seedbC_phi_p20, "ele1_eRegrInput_seedbC_phi_p20/F");
  outTree_ -> Branch("ele1_eRegrInput_seedbC_etacry", &ele1_eRegrInput_seedbC_etacry, "ele1_eRegrInput_seedbC_etacry/F");
  outTree_ -> Branch("ele1_eRegrInput_seedbC_phicry", &ele1_eRegrInput_seedbC_phicry, "ele1_eRegrInput_seedbC_phicry/F");
  outTree_ -> Branch("ele1_eRegrInput_bC2_eta", &ele1_eRegrInput_bC2_eta, "ele1_eRegrInput_bC2_eta/F");
  outTree_ -> Branch("ele1_eRegrInput_bC2_phi", &ele1_eRegrInput_bC2_phi, "ele1_eRegrInput_bC2_phi/F");
  outTree_ -> Branch("ele1_eRegrInput_bC2_eta_p5", &ele1_eRegrInput_bC2_eta_p5, "ele1_eRegrInput_bC2_eta_p5/F");
  outTree_ -> Branch("ele1_eRegrInput_bC2_phi_p2", &ele1_eRegrInput_bC2_phi_p2, "ele1_eRegrInput_bC2_phi_p2/F");
  outTree_ -> Branch("ele1_eRegrInput_bC2_bieta", &ele1_eRegrInput_bC2_bieta, "ele1_eRegrInput_bC2_bieta/F");
  outTree_ -> Branch("ele1_eRegrInput_bC2_phi_p20", &ele1_eRegrInput_bC2_phi_p20, "ele1_eRegrInput_bC2_phi_p20/F");
  outTree_ -> Branch("ele1_eRegrInput_bC2_etacry", &ele1_eRegrInput_bC2_etacry, "ele1_eRegrInput_bC2_etacry/F");
  outTree_ -> Branch("ele1_eRegrInput_bC2_phicry", &ele1_eRegrInput_bC2_phicry, "ele1_eRegrInput_bC2_phicry/F");
  outTree_ -> Branch("ele1_eRegrInput_nPV", &ele1_eRegrInput_nPV, "ele1_eRegrInput_nPV/F");

  // ele2 variables

  outTree_ -> Branch("ele2_charge",     &ele2_charge,         "ele2_charge/F");
  outTree_ -> Branch("ele2_p",     &ele2_p,         "ele2_p/F");
  outTree_ -> Branch("ele2_pt",     &ele2_pt,         "ele2_pt/F");
  outTree_ -> Branch("ele2_eta",     &ele2_eta,         "ele2_eta/F");
  outTree_ -> Branch("ele2_phi",     &ele2_phi,         "ele2_phi/F");

  outTree_ -> Branch("ele2_sigmaIetaIeta",     &ele2_sigmaIetaIeta,         "ele2_sigmaIetaIeta/F");
  outTree_ -> Branch("ele2_DphiIn",     &ele2_DphiIn,         "ele2_DphiIn/F");
  outTree_ -> Branch("ele2_DetaIn",     &ele2_DetaIn,         "ele2_DetaIn/F");
  outTree_ -> Branch("ele2_HOverE",     &ele2_HOverE,         "ele2_HOverE/F");
  outTree_ -> Branch("ele2_tkIso",     &ele2_tkIso,         "ele2_tkIso/F");
  outTree_ -> Branch("ele2_emIso",     &ele2_emIso,         "ele2_emIso/F");
  outTree_ -> Branch("ele2_hadIso",     &ele2_hadIso,         "ele2_hadIso/F");

  outTree_ -> Branch("ele2_dxy_PV",     &ele2_dxy_PV,         "ele2_dxy_PV/F");
  outTree_ -> Branch("ele2_dz_PV",     &ele2_dz_PV,         "ele2_dz_PV/F");
  outTree_ -> Branch("ele2_deltaEtaSuperClusterAtVtx",     &ele2_deltaEtaSuperClusterAtVtx,         "ele2_deltaEtaSuperClusterAtVtx/F");
  outTree_ -> Branch("ele2_deltaPhiSuperClusterAtVtx",     &ele2_deltaPhiSuperClusterAtVtx,         "ele2_hadele1_deltaPhiSuperClusterAtVtxIso/F");
  outTree_ -> Branch("ele2_sigmaP",     &ele2_sigmaP,         "ele2_sigmaP/F");
   
  outTree_ -> Branch("ele2_scERaw",     &ele2_scERaw,         "ele2_scERaw/F");
  outTree_ -> Branch("ele2_scEtRaw",    &ele2_scEtRaw,       "ele2_scEtRaw/F");
  outTree_ -> Branch("ele2_scEt",       &ele2_scEt,             "ele2_scEt/F");
  outTree_ -> Branch("ele2_scLocalEta",    &ele2_scLocalEta,         "ele2_scLocalEta/F");
  outTree_ -> Branch("ele2_scLocalPhi",    &ele2_scLocalPhi,         "ele2_scLocalPhi/F");
  outTree_ -> Branch("ele2_scEtaWidth",    &ele2_scEtaWidth,         "ele2_scEtaWidth/F");
  outTree_ -> Branch("ele2_scPhiWidth",    &ele2_scPhiWidth,         "ele2_scPhiWidth/F");
  outTree_ -> Branch("ele2_scCrackCorr",   &ele2_scCrackCorr,        "ele2_scCrackCorr/F");
  outTree_ -> Branch("ele2_scLocalContCorr",    &ele2_scLocalContCorr,         "ele2_scLocalContCorr/F");

  outTree_ -> Branch("ele2_scE",    &ele2_scE,         "ele2_scE/F");
  outTree_ -> Branch("ele2_scEta",  &ele2_scEta,     "ele2_scEta/F");
  outTree_ -> Branch("ele2_scPhi",  &ele2_scPhi,     "ele2_scPhi/F");
  outTree_ -> Branch("ele2_scLaserCorr",       &ele2_scLaserCorr,         "ele2_scLaserCorr/F");
  outTree_ -> Branch("ele2_scE_regression",    &ele2_scE_regression,   "ele2_scE_regression/F");
  outTree_ -> Branch("ele2_scEerr_regression",       &ele2_scEerr_regression,         "ele2_scEerr_regression/F");
  outTree_ -> Branch("ele2_scERaw_PUcleaned",    &ele2_scERaw_PUcleaned,   "ele2_scERaw_PUcleaned/F");
  outTree_ -> Branch("ele2_scEtaWidth_PUcleaned",       &ele2_scEtaWidth_PUcleaned,         "ele2_scEtaWidth_PUcleaned/F");
  outTree_ -> Branch("ele2_scPhiWidth_PUcleaned",    &ele2_scPhiWidth_PUcleaned,   "ele2_scPhiWidth_PUcleaned/F");
  outTree_ -> Branch("ele2_fCorrection_PUcleaned",       &ele2_fCorrection_PUcleaned,         "ele2_fCorrection_PUcleaned/F");


  outTree_ -> Branch("ele2_fEta",       &ele2_fEta,   "ele2_fEta/F");
  outTree_ -> Branch("ele2_fEtaCorr",   &ele2_fEtaCorr,         "ele2_fEtaCorr/F");
  outTree_ -> Branch("ele2_tkP",        &ele2_tkP,   "ele2_tkP/F");
  outTree_ -> Branch("ele2_tkPt",       &ele2_tkPt,  "ele2_tkPt/F");
  outTree_ -> Branch("ele2_fbrem",       &ele2_fbrem,  "ele2_fbrem/F");

  outTree_ -> Branch("ele2_e5x5",       &ele2_e5x5,  "ele2_e5x5/F");
  outTree_ -> Branch("ele2_e3x3",       &ele2_e3x3,  "ele2_e3x3/F");
  outTree_ -> Branch("ele2_scNxtal",       &ele2_scNxtal,  "ele2_scNxtal/F");
  outTree_ -> Branch("ele2_5x5LaserCorr",       &ele2_5x5LaserCorr,  "ele2_5x5LaserCorr/F");
  outTree_ -> Branch("ele2_3x3LaserCorr",       &ele2_3x3LaserCorr,  "ele2_3x3LaserCorr/F");
  outTree_ -> Branch("ele2_es",       &ele2_es,  "ele2_es/F");
 
  outTree_ -> Branch("ele2_seedE",       &ele2_seedE,  "ele2_seedE/F");
  outTree_ -> Branch("ele2_seedLaserAlpha",       &ele2_seedLaserAlpha,  "ele2_seedLaserAlpha/F");
  outTree_ -> Branch("ele2_seedLaserCorr",       &ele2_seedLaserCorr,  "ele2_seedLaserCorr/F");
  outTree_ -> Branch("ele2_seedICConstant",       &ele2_seedICConstant,  "ele2_seedICConstant/F");
  outTree_ -> Branch("ele2_seedIeta",       &ele2_seedIeta,  "ele2_seedIeta/I");
  outTree_ -> Branch("ele2_seedIphi",       &ele2_seedIphi,  "ele2_seedIphi/I");
  outTree_ -> Branch("ele2_seedIx",       &ele2_seedIx,  "ele2_seedIx/I");
  outTree_ -> Branch("ele2_seedIy",       &ele2_seedIy,  "ele2_seedIy/I");
  outTree_ -> Branch("ele2_seedZside",       &ele2_seedZside,  "ele2_seedZside/I");
  outTree_ -> Branch("ele2_EOverP",       &ele2_EOverP,  "ele2_EOverP/F");

  outTree_ -> Branch("ele2_recHit_E",              "std::vector<float>", &ele2_recHit_E);
  outTree_ -> Branch("ele2_recHit_hashedIndex", "std::vector<int>",   &ele2_recHit_hashedIndex);
  outTree_ -> Branch("ele2_recHit_ietaORix", "std::vector<int>",   &ele2_recHit_ietaORix);
  outTree_ -> Branch("ele2_recHit_iphiORiy", "std::vector<int>",   &ele2_recHit_iphiORiy);
  outTree_ -> Branch("ele2_recHit_zside", "std::vector<int>",   &ele2_recHit_zside);
  outTree_ -> Branch("ele2_recHit_laserCorrection","std::vector<float>", &ele2_recHit_laserCorrection);
  outTree_ -> Branch("ele2_recHit_ICConstant",     "std::vector<float>", &ele2_recHit_ICConstant);

  outTree_ -> Branch("ele2_nRecHits",       &ele2_nRecHits,  "ele2_nRecHits/I");
  outTree_ -> Branch("ele2_isEB",       &ele2_isEB,  "ele2_isEB/I");
  outTree_ -> Branch("ele2_isEBEEGap",       &ele2_isEBEEGap,  "ele2_isEBEEGap/I");
  outTree_ -> Branch("ele2_isEBEtaGap",       &ele2_isEBEtaGap,  "ele2_isEBEtaGap/I");
  outTree_ -> Branch("ele2_isEBPhiGap",       &ele2_isEBPhiGap,  "ele2_isEBPhiGap/I");
  outTree_ -> Branch("ele2_isEEDeeGap",       &ele2_isEEDeeGap,  "ele2_isEEDeeGap/I");
  outTree_ -> Branch("ele2_isEERingGap",       &ele2_isEERingGap,  "ele2_isEERingGap/I");

 outTree_ -> Branch("ele2_eRegrInput_rawE", &ele2_eRegrInput_rawE, "ele2_eRegrInput_rawE/F");
  outTree_ -> Branch("ele2_eRegrInput_r9", &ele2_eRegrInput_r9, "ele2_eRegrInput_r9/F");
  outTree_ -> Branch("ele2_eRegrInput_eta", &ele2_eRegrInput_eta, "ele2_eRegrInput_eta/F");
  outTree_ -> Branch("ele2_eRegrInput_phi", &ele2_eRegrInput_phi, "ele2_eRegrInput_phi/F");
  outTree_ -> Branch("ele2_eRegrInput_r25", &ele2_eRegrInput_r25, "ele2_eRegrInput_r25/F");
  outTree_ -> Branch("ele2_eRegrInput_hoe", &ele2_eRegrInput_hoe, "ele2_eRegrInput_hoe/F");
  outTree_ -> Branch("ele2_eRegrInput_etaW", &ele2_eRegrInput_etaW, "ele2_eRegrInput_etaW/F");
  outTree_ -> Branch("ele2_eRegrInput_phiW", &ele2_eRegrInput_phiW, "ele2_eRegrInput_phiW/F");
  outTree_ -> Branch("ele2_eRegrInput_Deta_bC_sC", &ele2_eRegrInput_Deta_bC_sC, "ele2_eRegrInput_Deta_bC_sC/F");
  outTree_ -> Branch("ele2_eRegrInput_Dphi_bC_sC", &ele2_eRegrInput_Dphi_bC_sC, "ele2_eRegrInput_Dphi_bC_sC/F");
  outTree_ -> Branch("ele2_eRegrInput_bCE_Over_sCE", &ele2_eRegrInput_bCE_Over_sCE, "ele2_eRegrInput_bCE_Over_sCE/F");
  outTree_ -> Branch("ele2_eRegrInput_e3x3_Over_bCE", &ele2_eRegrInput_e3x3_Over_bCE, "ele2_eRegrInput_e3x3_Over_bCE/F");
  outTree_ -> Branch("ele2_eRegrInput_e5x5_Over_bCE", &ele2_eRegrInput_e5x5_Over_bCE, "ele2_eRegrInput_e5x5_Over_bCE/F");
  outTree_ -> Branch("ele2_eRegrInput_sigietaieta_bC1", &ele2_eRegrInput_sigietaieta_bC1, "ele2_eRegrInput_sigietaieta_bC1/F");
  outTree_ -> Branch("ele2_eRegrInput_sigietaieta_bC1", &ele2_eRegrInput_sigietaieta_bC1, "ele2_eRegrInput_sigietaieta_bC1/F");
  outTree_ -> Branch("ele2_eRegrInput_sigiphiiphi_bC1", &ele2_eRegrInput_sigiphiiphi_bC1, "ele2_eRegrInput_sigiphiiphi_bC1/F");
  outTree_ -> Branch("ele2_eRegrInput_sigietaiphi_bC1", &ele2_eRegrInput_sigietaiphi_bC1, "ele2_eRegrInput_sigietaiphi_bC1/F");
  outTree_ -> Branch("ele2_eRegrInput_bEMax_Over_bCE", &ele2_eRegrInput_bEMax_Over_bCE,
"ele2_eRegrInput_bEMax_Over_bCE/F");
  outTree_ -> Branch("ele2_eRegrInput_log_bE2nd_Over_bEMax", &ele2_eRegrInput_log_bE2nd_Over_bEMax, "ele2_eRegrInput_log_bE2nd_Over_bEMax/F");
  outTree_ -> Branch("ele2_eRegrInput_log_bEtop_Over_bEMax", &ele2_eRegrInput_log_bEtop_Over_bEMax, "ele2_eRegrInput_log_bEtop_Over_bEMax/F");
  outTree_ -> Branch("ele2_eRegrInput_log_bEbot_Over_bEMax", &ele2_eRegrInput_log_bEbot_Over_bEMax, "ele2_eRegrInput_log_bEbot_Over_bEMax/F");
  outTree_ -> Branch("ele2_eRegrInput_log_bEleft_Over_bEMax", &ele2_eRegrInput_log_bEleft_Over_bEMax, "ele2_eRegrInput_log_bEleft_Over_bEMax/F");
  outTree_ -> Branch("ele2_eRegrInput_log_bEright_Over_bEMax", &ele2_eRegrInput_log_bEright_Over_bEMax, "ele2_eRegrInput_log_bEright_Over_bEMax/F");
  outTree_ -> Branch("ele2_eRegrInput_asym_top_bottom", &ele2_eRegrInput_asym_top_bottom, "ele2_eRegrInput_asym_top_bottom/F");
  outTree_ -> Branch("ele2_eRegrInput_asym_left_right", &ele2_eRegrInput_asym_left_right, "ele2_eRegrInput_asym_left_right/F");
  outTree_ -> Branch("ele2_eRegrInput_Deta_bC2_sC", &ele2_eRegrInput_Deta_bC2_sC, "ele2_eRegrInput_Deta_bC2_sC/F");
  outTree_ -> Branch("ele2_eRegrInput_Dphi_bC2_sC", &ele2_eRegrInput_Dphi_bC2_sC, "ele2_eRegrInput_Dphi_bC2_sC/F");
  outTree_ -> Branch("ele2_eRegrInput_bCE2_Over_sCE", &ele2_eRegrInput_bCE2_Over_sCE, "ele2_eRegrInput_bCE2_Over_sCE/F");
  outTree_ -> Branch("ele2_eRegrInput_e3x3_Over_bCE2", &ele2_eRegrInput_e3x3_Over_bCE2, "ele2_eRegrInput_e3x3_Over_bCE2/F");
  outTree_ -> Branch("ele2_eRegrInput_e5x5_Over_bCE2", &ele2_eRegrInput_e5x5_Over_bCE2, "ele2_eRegrInput_e5x5_Over_bCE2/F");
  outTree_ -> Branch("ele2_eRegrInput_sigietaieta_bC2", &ele2_eRegrInput_sigietaieta_bC2, "ele2_eRegrInput_sigietaieta_bC2/F");
  outTree_ -> Branch("ele2_eRegrInput_sigiphiiphi_bC2", &ele2_eRegrInput_sigiphiiphi_bC2, "ele2_eRegrInput_sigiphiiphi_bC2/F");
  outTree_ -> Branch("ele2_eRegrInput_sigietaiphi_bC2", &ele2_eRegrInput_sigietaiphi_bC2, "ele2_eRegrInput_sigietaiphi_bC2/F");
  outTree_ -> Branch("ele2_eRegrInput_bEMax_Over_bCE2", &ele2_eRegrInput_bEMax_Over_bCE2, "ele2_eRegrInput_bEMax_Over_bCE2/F");
  outTree_ -> Branch("ele2_eRegrInput_log_bE2nd_Over_bEMax2", &ele2_eRegrInput_log_bE2nd_Over_bEMax2, "ele2_eRegrInput_log_bE2nd_Over_bEMax2/F");
  outTree_ -> Branch("ele2_eRegrInput_log_bEtop_Over_bEMax2", &ele2_eRegrInput_log_bEtop_Over_bEMax2, "ele2_eRegrInput_log_bEtop_Over_bEMax2/F");
  outTree_ -> Branch("ele2_eRegrInput_log_bEbot_Over_bEMax2", &ele2_eRegrInput_log_bEbot_Over_bEMax2, "ele2_eRegrInput_log_bEbot_Over_bEMax2/F");
  outTree_ -> Branch("ele2_eRegrInput_log_bEleft_Over_bEMax2", &ele2_eRegrInput_log_bEleft_Over_bEMax2, "ele2_eRegrInput_log_bEleft_Over_bEMax2/F");
  outTree_ -> Branch("ele2_eRegrInput_log_bEright_Over_bEMax2", &ele2_eRegrInput_log_bEright_Over_bEMax2, "ele2_eRegrInput_log_bEright_Over_bEMax2/F");
  outTree_ -> Branch("ele2_eRegrInput_asym_top2_bottom2", &ele2_eRegrInput_asym_top2_bottom2, "ele2_eRegrInput_asym_top2_bottom2/F");
  outTree_ -> Branch("ele2_eRegrInput_asym_left2_right2", &ele2_eRegrInput_asym_left2_right2, "ele2_eRegrInput_asym_left2_right2/F");
  outTree_ -> Branch("ele2_eRegrInput_Deta_bCLow_sC", &ele2_eRegrInput_Deta_bCLow_sC, "ele2_eRegrInput_Deta_bCLow_sC/F");
  outTree_ -> Branch("ele2_eRegrInput_Dphi_bCLow_sC", &ele2_eRegrInput_Dphi_bCLow_sC, "ele2_eRegrInput_Dphi_bCLow_sC/F");
  outTree_ -> Branch("ele2_eRegrInput_bCELow_Over_sCE", &ele2_eRegrInput_bCELow_Over_sCE, "ele2_eRegrInput_bCELow_Over_sCE/F");
  outTree_ -> Branch("ele2_eRegrInput_e3x3_Over_bCELow", &ele2_eRegrInput_e3x3_Over_bCELow, "ele2_eRegrInput_e3x3_Over_bCELow/F");
  outTree_ -> Branch("ele2_eRegrInput_e5x5_Over_bCELow", &ele2_eRegrInput_e5x5_Over_bCELow, "ele2_eRegrInput_e5x5_Over_bCELow/F");
  outTree_ -> Branch("ele2_eRegrInput_sigietaieta_bCLow", &ele2_eRegrInput_sigietaieta_bCLow, "ele2_eRegrInput_sigietaieta_bCLow/F");
  outTree_ -> Branch("ele2_eRegrInput_e5x5_Over_bCELow", &ele2_eRegrInput_e5x5_Over_bCELow, "ele2_eRegrInput_e5x5_Over_bCELow/F");
  outTree_ -> Branch("ele2_eRegrInput_sigietaieta_bCLow", &ele2_eRegrInput_sigietaieta_bCLow, "ele2_eRegrInput_sigietaieta_bCLow/F");
  outTree_ -> Branch("ele2_eRegrInput_sigiphiiphi_bCLow", &ele2_eRegrInput_sigiphiiphi_bCLow, "ele2_eRegrInput_e5x5_Over_bCELow/F");
  outTree_ -> Branch("ele2_eRegrInput_sigietaiphi_bCLow", &ele2_eRegrInput_sigietaiphi_bCLow, "ele2_eRegrInput_sigietaiphi_bCLow/F");
  outTree_ -> Branch("ele2_eRegrInput_Deta_bCLow2_sC", &ele2_eRegrInput_Deta_bCLow2_sC, "ele2_eRegrInput_Deta_bCLow2_sC/F");
  outTree_ -> Branch("ele2_eRegrInput_Dphi_bCLow2_sC", &ele2_eRegrInput_Dphi_bCLow2_sC, "ele2_eRegrInput_Dphi_bCLow2_sC/F");
  outTree_ -> Branch("ele2_eRegrInput_bCELow2_Over_sCE", &ele2_eRegrInput_bCELow2_Over_sCE, "ele2_eRegrInput_bCELow2_Over_sCE/F");
  outTree_ -> Branch("ele2_eRegrInput_e3x3_Over_bCELow2", &ele2_eRegrInput_e3x3_Over_bCELow2, "ele2_eRegrInput_e3x3_Over_bCELow2/F");
  outTree_ -> Branch("ele2_eRegrInput_e5x5_Over_bCELow2", &ele2_eRegrInput_e5x5_Over_bCELow2, "ele2_eRegrInput_e5x5_Over_bCELow2/F");
  outTree_ -> Branch("ele2_eRegrInput_sigietaieta_bCLow2", &ele2_eRegrInput_sigietaieta_bCLow2, "ele2_eRegrInput_sigietaieta_bCLow2/F");
  outTree_ -> Branch("ele2_eRegrInput_sigiphiiphi_bCLow2", &ele2_eRegrInput_sigiphiiphi_bCLow2, "ele2_eRegrInput_sigiphiiphi_bCLow2/F");
  outTree_ -> Branch("ele2_eRegrInput_sigietaiphi_bCLow2", &ele2_eRegrInput_sigietaiphi_bCLow2, "ele2_eRegrInput_sigietaiphi_bCLow2/F");
  outTree_ -> Branch("ele2_eRegrInput_seedbC_eta", &ele2_eRegrInput_seedbC_eta, "ele2_eRegrInput_seedbC_eta/F");
  outTree_ -> Branch("ele2_eRegrInput_seedbC_phi", &ele2_eRegrInput_seedbC_phi, "ele2_eRegrInput_seedbC_phi/F");
  outTree_ -> Branch("ele2_eRegrInput_seedbC_eta_p5", &ele2_eRegrInput_seedbC_eta_p5, "ele2_eRegrInput_seedbC_eta_p5/F");
  outTree_ -> Branch("ele2_eRegrInput_seedbC_phi_p2", &ele2_eRegrInput_seedbC_phi_p2, "ele2_eRegrInput_seedbC_phi_p2/F");
  outTree_ -> Branch("ele2_eRegrInput_seedbC_bieta", &ele2_eRegrInput_seedbC_bieta, "ele2_eRegrInput_seedbC_bieta/F");
  outTree_ -> Branch("ele2_eRegrInput_seedbC_phi_p20", &ele2_eRegrInput_seedbC_phi_p20, "ele2_eRegrInput_seedbC_phi_p20/F");
  outTree_ -> Branch("ele2_eRegrInput_seedbC_etacry", &ele2_eRegrInput_seedbC_etacry, "ele2_eRegrInput_seedbC_etacry/F");
  outTree_ -> Branch("ele2_eRegrInput_seedbC_phicry", &ele2_eRegrInput_seedbC_phicry, "ele2_eRegrInput_seedbC_phicry/F");
  outTree_ -> Branch("ele2_eRegrInput_bC2_eta", &ele2_eRegrInput_bC2_eta, "ele2_eRegrInput_bC2_eta/F");
  outTree_ -> Branch("ele2_eRegrInput_bC2_phi", &ele2_eRegrInput_bC2_phi, "ele2_eRegrInput_bC2_phi/F");
  outTree_ -> Branch("ele2_eRegrInput_bC2_eta_p5", &ele2_eRegrInput_bC2_eta_p5, "ele2_eRegrInput_bC2_eta_p5/F");
  outTree_ -> Branch("ele2_eRegrInput_bC2_phi_p2", &ele2_eRegrInput_bC2_phi_p2, "ele2_eRegrInput_bC2_phi_p2/F");
  outTree_ -> Branch("ele2_eRegrInput_bC2_bieta", &ele2_eRegrInput_bC2_bieta, "ele2_eRegrInput_bC2_bieta/F");
  outTree_ -> Branch("ele2_eRegrInput_bC2_phi_p20", &ele2_eRegrInput_bC2_phi_p20, "ele2_eRegrInput_bC2_phi_p20/F");
  outTree_ -> Branch("ele2_eRegrInput_bC2_etacry", &ele2_eRegrInput_bC2_etacry, "ele2_eRegrInput_bC2_etacry/F");
  outTree_ -> Branch("ele2_eRegrInput_bC2_phicry", &ele2_eRegrInput_bC2_phicry, "ele2_eRegrInput_bC2_phicry/F");
  outTree_ -> Branch("ele2_eRegrInput_nPV", &ele2_eRegrInput_nPV, "ele2_eRegrInput_nPV/F");
  
  // met variables
  outTree_ -> Branch("met_et",       &met_et,  "met_et/F");
  outTree_ -> Branch("met_phi",       &met_phi,  "met_phi/F");
  outTree_ -> Branch("ele1Met_mt",       &ele1Met_mt,  "ele1Met_mt/F");
  outTree_ -> Branch("ele1Met_Dphi",       &ele1Met_Dphi,  "ele1Met_Dphi/F");
   
  // di-electron variables
  outTree_ -> Branch("ele1ele2_m",       &ele1ele2_m,  "ele1ele2_m/F");
  outTree_ -> Branch("ele1ele2_scM",       &ele1ele2_scM,  "ele1ele2_scM/F");
  outTree_ -> Branch("ele1ele2_scM_regression",       &ele1ele2_scM_regression,  "ele1ele2_scM_regression/F");

  EcalClusterCrackCorrection = EcalClusterFunctionFactory::get()->create("EcalClusterCrackCorrection", iConfig);
  EcalClusterLocalContCorrection = EcalClusterFunctionFactory::get()->create("EcalClusterLocalContCorrection", iConfig);
 
  // JSON file map 
  jsonMap_ = readJSONFile(jsonFileName_);

}

// --------------------------------------------------------------------



SimpleNtupleEoverP::~SimpleNtupleEoverP ()
{
  cout<< "Analyzed " <<  eventNaiveId_ << " events" <<endl;
  // save tree
  outTree_ -> Write();
}

// -----------------------------------------------------------------------------------------



void SimpleNtupleEoverP::analyze (const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  ++eventNaiveId_;
  
  bool isGoodEvent = false;

  // event variables 
  runId = iEvent.id().run();
  lumiId = iEvent.luminosityBlock();
  eventId = iEvent.id().event();
  timeStampHigh = (int)(iEvent.time().value() >> 32);
 
  isW = -1;
  isZ = -1;

  // electron variables  
  ele1_charge =-99.;
  ele1_p =-99.;
  ele1_pt =-99.;
  ele1_eta =-99.;
  ele1_phi=-99.;

  ele1_sigmaIetaIeta =-99.;
  ele1_DphiIn =-99.;
  ele1_DetaIn =-99.;
  ele1_HOverE =-99.;
  ele1_tkIso =-99.;
  ele1_emIso =-99.;
  ele1_hadIso =-99.;

  ele1_dxy_PV = -99. ;
  ele1_dz_PV  = -99. ;
  ele1_deltaEtaSuperClusterAtVtx  = -99.;
  ele1_deltaPhiSuperClusterAtVtx = -99.;
  ele1_sigmaP = -99.;

  ele1_scERaw =-99.;
  ele1_scEtRaw = -99.;
  ele1_scEt = -99.;
  ele1_scLocalEta =-99.;
  ele1_scLocalPhi = -99.;
  ele1_scEtaWidth = -99.;
  ele1_scPhiWidth = -99.;
  ele1_scCrackCorr =-99.;
  ele1_scLocalContCorr =-99.;
  ele1_scE =-99.;
  ele1_scEta =-99.;
  ele1_scPhi =-99.;
  ele1_scLaserCorr =-99.;
  ele1_scE_regression =-99.;
  ele1_scEerr_regression=-99.;
  ele1_scERaw_PUcleaned=-99.;
  ele1_scEtaWidth_PUcleaned=-99.;
  ele1_scPhiWidth_PUcleaned=-99.;
  ele1_fCorrection_PUcleaned=-99.;

  ele1_fEta=-99.;
  ele1_fEtaCorr=-99.;
  ele1_tkP=-99.;
  ele1_tkPt=-99.;
  ele1_fbrem=-99.;
 
  ele1_e5x5=-99.;
  ele1_e3x3=-99.;
  ele1_scNxtal=-99.;
  ele1_5x5LaserCorr=-99.;
  ele1_3x3LaserCorr=-99.;
  ele1_es=-99.;

  ele1_seedE=-99.;
  ele1_seedLaserAlpha=-99.;
  ele1_seedLaserCorr=-99.;
  ele1_seedICConstant=-99.;
  ele1_seedIeta=-9999;
  ele1_seedIphi=-9999;
  ele1_seedIx=-9999;
  ele1_seedIy=-9999;
  ele1_seedZside=-9999;
  ele1_EOverP=-99.;

  ele1_recHit_E.clear();
  ele1_recHit_hashedIndex.clear();
  ele1_recHit_ietaORix.clear();
  ele1_recHit_iphiORiy.clear();
  ele1_recHit_zside.clear();
  ele1_recHit_laserCorrection.clear();
  ele1_recHit_ICConstant.clear();
  ele1_nRecHits = -9999;
 
  ele1_isEB= -9999;
  ele1_isEBEEGap= -9999;
  ele1_isEBEtaGap= -9999;
  ele1_isEBPhiGap= -9999;
  ele1_isEEDeeGap= -9999;
  ele1_isEERingGap= -9999;

   ele1_eRegrInput_rawE = -99.;
  ele1_eRegrInput_r9 = -99.;
  ele1_eRegrInput_eta = -99. ;
  ele1_eRegrInput_phi = -99.;
  ele1_eRegrInput_r25= -99.;
  ele1_eRegrInput_hoe= -99.;
  ele1_eRegrInput_etaW= -99.;
  ele1_eRegrInput_phiW= -99.;

  ele1_eRegrInput_Deta_bC_sC= -99.;
  ele1_eRegrInput_Dphi_bC_sC= -99.;
  ele1_eRegrInput_bCE_Over_sCE= -99.;
  ele1_eRegrInput_e3x3_Over_bCE= -99.;
  ele1_eRegrInput_e5x5_Over_bCE= -99.;
  ele1_eRegrInput_sigietaieta_bC1= -99.;
  ele1_eRegrInput_sigiphiiphi_bC1= -99.;
  ele1_eRegrInput_sigietaiphi_bC1= -99.;
  ele1_eRegrInput_bEMax_Over_bCE= -99.;
  ele1_eRegrInput_log_bE2nd_Over_bEMax= -99.;
  ele1_eRegrInput_log_bEtop_Over_bEMax= -99.;
  ele1_eRegrInput_log_bEbot_Over_bEMax= -99.;
  ele1_eRegrInput_log_bEleft_Over_bEMax= -99.;
  ele1_eRegrInput_log_bEright_Over_bEMax= -99.;
  ele1_eRegrInput_asym_top_bottom= -99.;
  ele1_eRegrInput_asym_left_right= -99.;
  ele1_eRegrInput_Deta_bC2_sC= -99.;
  ele1_eRegrInput_Dphi_bC2_sC= -99.;
  ele1_eRegrInput_bCE2_Over_sCE= -99.;
  ele1_eRegrInput_e3x3_Over_bCE2= -99.;
  ele1_eRegrInput_e5x5_Over_bCE2= -99.;
  ele1_eRegrInput_sigietaieta_bC2= -99.;
  ele1_eRegrInput_sigiphiiphi_bC2= -99.;
  ele1_eRegrInput_sigietaiphi_bC2= -99.;
  ele1_eRegrInput_bEMax_Over_bCE2= -99.;
  ele1_eRegrInput_log_bE2nd_Over_bEMax2= -99.;
  ele1_eRegrInput_log_bEtop_Over_bEMax2= -99.;
  ele1_eRegrInput_log_bEbot_Over_bEMax2= -99.;
  ele1_eRegrInput_log_bEleft_Over_bEMax2= -99.;
  ele1_eRegrInput_log_bEright_Over_bEMax2= -99.;
  ele1_eRegrInput_asym_top2_bottom2= -99.;
  ele1_eRegrInput_asym_left2_right2= -99.;
  ele1_eRegrInput_Deta_bCLow_sC= -99.;
  ele1_eRegrInput_Dphi_bCLow_sC= -99.;
  ele1_eRegrInput_bCELow_Over_sCE= -99.;
  ele1_eRegrInput_e3x3_Over_bCELow= -99.;
  ele1_eRegrInput_e5x5_Over_bCELow= -99.;
  ele1_eRegrInput_sigietaieta_bCLow= -99.;
  ele1_eRegrInput_sigiphiiphi_bCLow= -99.;
  ele1_eRegrInput_sigietaiphi_bCLow= -99.;
  ele1_eRegrInput_Deta_bCLow2_sC= -99.;
  ele1_eRegrInput_Dphi_bCLow2_sC= -99.;
  ele1_eRegrInput_bCELow2_Over_sCE= -99.;
  ele1_eRegrInput_e3x3_Over_bCELow2= -99.;
  ele1_eRegrInput_e5x5_Over_bCELow2= -99.;
  ele1_eRegrInput_sigietaieta_bCLow2= -99.;
  ele1_eRegrInput_sigiphiiphi_bCLow2= -99.;
  ele1_eRegrInput_sigietaiphi_bCLow2= -99.;
  ele1_eRegrInput_seedbC_eta= -99.;
  ele1_eRegrInput_seedbC_phi= -99.;
  ele1_eRegrInput_seedbC_eta_p5= -99.;
  ele1_eRegrInput_seedbC_phi_p2= -99.;
  ele1_eRegrInput_seedbC_bieta= -99.;
  ele1_eRegrInput_seedbC_phi_p20= -99.;
  ele1_eRegrInput_seedbC_etacry= -99.;
  ele1_eRegrInput_seedbC_phicry= -99.;
  ele1_eRegrInput_bC2_eta= -99.;
  ele1_eRegrInput_bC2_phi= -99.;
  ele1_eRegrInput_bC2_eta_p5= -99.;
  ele1_eRegrInput_bC2_phi_p2= -99.;
  ele1_eRegrInput_bC2_bieta= -99.;
  ele1_eRegrInput_bC2_phi_p20= -99.;
  ele1_eRegrInput_bC2_etacry= -99.;
  ele1_eRegrInput_bC2_phicry= -99.;
  ele1_eRegrInput_nPV= -99.;


  ele2_charge =-99.;
  ele2_p =-99.;
  ele2_pt =-99.;
  ele2_eta =-99.;
  ele2_phi=-99.;

  ele2_sigmaIetaIeta =-99.;
  ele2_DphiIn =-99.;
  ele2_DetaIn =-99.;
  ele2_HOverE =-99.;
  ele2_tkIso =-99.;
  ele2_emIso =-99.;
  ele2_hadIso =-99.;


  ele2_dxy_PV = -99. ;
  ele2_dz_PV  = -99. ;
  ele2_deltaEtaSuperClusterAtVtx  = -99.;
  ele2_deltaPhiSuperClusterAtVtx = -99.;
  ele2_sigmaP = -99.;

  ele2_scERaw =-99.;
  ele2_scEtRaw = -99.;
  ele2_scEt = -99.;
  ele2_scLocalEta =-99.;
  ele2_scLocalPhi = -99.;
  ele2_scEtaWidth = -99.;
  ele2_scPhiWidth = -99.;
  ele2_scCrackCorr =-99.;
  ele2_scLocalContCorr =-99.;
  ele2_scE =-99.;
  ele2_scEta =-99.;
  ele2_scPhi =-99.;
  ele2_scLaserCorr =-99.;
  ele2_scE_regression =-99.;
  ele2_scEerr_regression=-99.;
  ele2_scERaw_PUcleaned=-99.;
  ele2_scEtaWidth_PUcleaned=-99.;
  ele2_scPhiWidth_PUcleaned=-99.;
  ele2_fCorrection_PUcleaned=-99.;

  ele2_fEta=-99.;
  ele2_fEtaCorr=-99.;
  ele2_tkP=-99.;
  ele2_tkPt=-99.;
  ele2_fbrem=-99.;
 
  ele2_e5x5=-99.;
  ele2_e3x3=-99.;
  ele2_scNxtal=-99.;
  ele2_5x5LaserCorr=-99.;
  ele2_3x3LaserCorr=-99.;
  ele2_es=-99.;

  ele2_seedE=-99.;
  ele2_seedLaserAlpha=-99.;
  ele2_seedLaserCorr=-99.;
  ele2_seedICConstant=-99.;
  ele2_seedIeta=-9999;
  ele2_seedIphi=-9999;
  ele2_seedIx=-9999;
  ele2_seedIy=-9999;
  ele2_seedZside=-9999;
  ele2_EOverP=-99.;

  ele2_recHit_E.clear();
  ele2_recHit_hashedIndex.clear();
  ele2_recHit_ietaORix.clear();
  ele2_recHit_iphiORiy.clear();
  ele2_recHit_zside.clear();
  ele2_recHit_laserCorrection.clear();
  ele2_recHit_ICConstant.clear();
  ele2_nRecHits = -9999;
 
  ele2_isEB= -9999;
  ele2_isEBEEGap= -9999;
  ele2_isEBEtaGap= -9999;
  ele2_isEBPhiGap= -9999;
  ele2_isEEDeeGap= -9999;
  ele2_isEERingGap= -9999;

ele2_eRegrInput_rawE = -99.;
  ele2_eRegrInput_r9 = -99.;
  ele2_eRegrInput_eta = -99. ;
  ele2_eRegrInput_phi = -99.;
  ele2_eRegrInput_r25= -99.;
  ele2_eRegrInput_hoe= -99.;
  ele2_eRegrInput_etaW= -99.;
  ele2_eRegrInput_phiW= -99.;

  ele2_eRegrInput_Deta_bC_sC= -99.;
  ele2_eRegrInput_Dphi_bC_sC= -99.;
  ele2_eRegrInput_bCE_Over_sCE= -99.;
  ele2_eRegrInput_e3x3_Over_bCE= -99.;
  ele2_eRegrInput_e5x5_Over_bCE= -99.;
  ele2_eRegrInput_sigietaieta_bC1= -99.;
  ele2_eRegrInput_sigiphiiphi_bC1= -99.;
  ele2_eRegrInput_sigietaiphi_bC1= -99.;
  ele2_eRegrInput_bEMax_Over_bCE= -99.;
  ele2_eRegrInput_log_bE2nd_Over_bEMax= -99.;
  ele2_eRegrInput_log_bEtop_Over_bEMax= -99.;
  ele2_eRegrInput_log_bEbot_Over_bEMax= -99.;
  ele2_eRegrInput_log_bEleft_Over_bEMax= -99.;
  ele2_eRegrInput_log_bEright_Over_bEMax= -99.;
  ele2_eRegrInput_asym_top_bottom= -99.;
  ele2_eRegrInput_asym_left_right= -99.;
  ele2_eRegrInput_Deta_bC2_sC= -99.;
  ele2_eRegrInput_Dphi_bC2_sC= -99.;
  ele2_eRegrInput_bCE2_Over_sCE= -99.;
  ele2_eRegrInput_e3x3_Over_bCE2= -99.;
  ele2_eRegrInput_e5x5_Over_bCE2= -99.;
  ele2_eRegrInput_sigietaieta_bC2= -99.;
  ele2_eRegrInput_sigiphiiphi_bC2= -99.;
  ele2_eRegrInput_sigietaiphi_bC2= -99.;
  ele2_eRegrInput_bEMax_Over_bCE2= -99.;
  ele2_eRegrInput_log_bE2nd_Over_bEMax2= -99.;
  ele2_eRegrInput_log_bEtop_Over_bEMax2= -99.;
  ele2_eRegrInput_log_bEbot_Over_bEMax2= -99.;
  ele2_eRegrInput_log_bEleft_Over_bEMax2= -99.;
  ele2_eRegrInput_log_bEright_Over_bEMax2= -99.;
  ele2_eRegrInput_asym_top2_bottom2= -99.;
  ele2_eRegrInput_asym_left2_right2= -99.;
  ele2_eRegrInput_Deta_bCLow_sC= -99.;
  ele2_eRegrInput_Dphi_bCLow_sC= -99.;
  ele2_eRegrInput_bCELow_Over_sCE= -99.;
  ele2_eRegrInput_e3x3_Over_bCELow= -99.;
  ele2_eRegrInput_e5x5_Over_bCELow= -99.;
  ele2_eRegrInput_sigietaieta_bCLow= -99.;
  ele2_eRegrInput_sigiphiiphi_bCLow= -99.;
  ele2_eRegrInput_sigietaiphi_bCLow= -99.;
  ele2_eRegrInput_Deta_bCLow2_sC= -99.;
  ele2_eRegrInput_Dphi_bCLow2_sC= -99.;
  ele2_eRegrInput_bCELow2_Over_sCE= -99.;
  ele2_eRegrInput_e3x3_Over_bCELow2= -99.;
  ele2_eRegrInput_e5x5_Over_bCELow2= -99.;
  ele2_eRegrInput_sigietaieta_bCLow2= -99.;
  ele2_eRegrInput_sigiphiiphi_bCLow2= -99.;
  ele2_eRegrInput_sigietaiphi_bCLow2= -99.;
  ele2_eRegrInput_seedbC_eta= -99.;
  ele2_eRegrInput_seedbC_phi= -99.;
  ele2_eRegrInput_seedbC_eta_p5= -99.;
  ele2_eRegrInput_seedbC_phi_p2= -99.;
  ele2_eRegrInput_seedbC_bieta= -99.;
  ele2_eRegrInput_seedbC_phi_p20= -99.;
  ele2_eRegrInput_seedbC_etacry= -99.;
  ele2_eRegrInput_seedbC_phicry= -99.;
  ele2_eRegrInput_bC2_eta= -99.;
  ele2_eRegrInput_bC2_phi= -99.;
  ele2_eRegrInput_bC2_eta_p5= -99.;
  ele2_eRegrInput_bC2_phi_p2= -99.;
  ele2_eRegrInput_bC2_bieta= -99.;
  ele2_eRegrInput_bC2_phi_p20= -99.;
  ele2_eRegrInput_bC2_etacry= -99.;
  ele2_eRegrInput_bC2_phicry= -99.;
  ele2_eRegrInput_nPV= -99.;

  met_et=-99.;
  met_phi=-99.;

  ele1Met_mt=-99.;
  ele1Met_Dphi=-99.;
  
  
  // di-electron variables
  ele1ele2_m=-99.;
  ele1ele2_scM=-99.;
  ele1ele2_scM_regression=-99.;
 
  // Accept event from json file
  bool skipEvent = false;
  if(AcceptEventByRunAndLumiSection(runId,lumiId,jsonMap_) == false) skipEvent = true;
  if( (jsonFlag_ == true) && (skipEvent == true) ) return;
 
  //************* VERTEXES
  fillPVInfo (iEvent,iSetup) ;

  //************* RHO for ISO
  fillRhoInfo(iEvent,iSetup) ; 

   //************* MC PU
   if (saveMCPU_ && dataFlag_==false) fillMCPUInfo (iEvent, iSetup);
 
  //************* ELECTRONS
  Handle<View<reco::GsfElectron> > electronHandle;
  iEvent.getByLabel(EleTag_,electronHandle);
  View<reco::GsfElectron> electrons = *electronHandle;
 
  ///---- get the number of the electron in the event to know if it's a W or a Z ----
  if(doWZSelection_){

   int nEleTight=0,nEleMedium=0,nEleLoose=0;
   eleIts_.clear();

   for ( unsigned int iEle=0; iEle<electrons.size(); ++iEle ){
    bool isEleTight = TightEle(iEvent,iSetup,iEle);

    if(isEleTight){ nEleTight++;
                    eleIts_[1./electrons.at(iEle).pt()] = iEle;
                    continue;}

    bool isEleMedium = MediumEle(iEvent,iSetup,iEle);

    if(isEleMedium){nEleMedium++;
                    eleIts_[1./electrons.at(iEle).pt()] = iEle;
                    continue;}

    bool isEleLoose = LooseEle(iEvent,iSetup,iEle);

    if(isEleLoose){nEleLoose++; 
                   eleIts_[1./electrons.at(iEle).pt()] = iEle;
                   continue;}
   }
   if( nEleTight < 1 ) return ;
   if( nEleTight > 2 ) return ;
   if( nEleMedium > 1) return ;
   if( nEleLoose > 0 ) return ;
  
   ///---- check if the event is good----
   
   if( (nEleTight == 1) && (nEleMedium == 0) ){
      isW=1;isZ=0;
      std::map<float,int>::const_iterator mapIt = eleIts_.begin();
      fillEleInfo ( iEvent, iSetup, mapIt->second, "ele1" ); 
      fillMetInfo (iEvent, iSetup);
 
    }

    if( (nEleTight == 2) || (nEleTight == 1 && nEleMedium == 1) ){
     isW=0;isZ=1;
     std::map<float,int>::const_iterator mapIt = eleIts_.begin();
     fillEleInfo ( iEvent, iSetup, mapIt->second, "ele1" );
     mapIt++;
     fillEleInfo ( iEvent, iSetup, mapIt->second, "ele2" ); 
     fillDoubleEleInfo (iEvent, iSetup);
     fillMetInfo (iEvent, iSetup);
 
    }

    
    if( (nEleTight == 1) && (nEleMedium == 0) ) isGoodEvent = myWselection ( iEvent, iSetup); 
    if( (nEleTight == 2) || (nEleTight == 1 && nEleMedium == 1) ) isGoodEvent = myZselection ( iEvent, iSetup); 
    ///---- save the entry of the tree only if W/Z event ----
    if ( isGoodEvent )   outTree_ -> Fill();
    
  }
  else{
         int nEle = electrons.size();
         if ( nEle == 1 ) { isW = 1; isZ = 0; }
         if ( nEle == 2 ) { isW = 0; isZ = 1; }
  
         if ( isW == 1 ) fillEleInfo ( iEvent, iSetup, 0, "ele1" ); 
                      
    
         if ( isZ == 1 ) { 
           fillEleInfo ( iEvent, iSetup, 0, "ele1" ); 
           fillEleInfo ( iEvent, iSetup, 1, "ele2" );
           fillDoubleEleInfo (iEvent, iSetup);
          }

         fillMetInfo (iEvent, iSetup);
 
         if (isW==1 || isZ==1) outTree_ -> Fill();
         }
}



//-----------------------------------------------------------------------------------------
bool  SimpleNtupleEoverP::TightEle (const edm::Event & iEvent, const edm::EventSetup & iESetup, const int &iEle){

 //************* ELECTRONS
 Handle<View<reco::GsfElectron> > electronHandle;
 iEvent.getByLabel(EleTag_,electronHandle);
 View<reco::GsfElectron> electrons = *electronHandle;


 bool isTightEle = false; 
 reco::GsfElectron electron = electrons.at(iEle);
 
 edm::InputTag EleBad = edm::InputTag("gsfElectrons");
 
 if(EleTag_== EleBad && !electron.ecalDriven()) return false ; 
 
 float pt = electron.pt();
 float eta = electron.eta();
      
 float tkIso  = electron.dr03TkSumPt();
 float emIso  = electron.dr03EcalRecHitSumEt(); 
 float hadIso = electron.dr03HcalDepth1TowerSumEt()+electron.dr03HcalDepth2TowerSumEt();
 float combIso = tkIso + emIso + hadIso - rho*0.3*0.3*3.14159;
      
 int isEB = electron.isEB();
 float sigmaIetaIeta = electron.sigmaIetaIeta();
 float DetaIn        = electron.deltaEtaSuperClusterTrackAtVtx();
 float DphiIn        = electron.deltaPhiSuperClusterTrackAtVtx();
 float HOverE        = electron.hadronicOverEm();
      
 int mishits             = electron.gsfTrack()->trackerExpectedHitsInner().numberOfHits();
 int nAmbiguousGsfTracks = electron.ambiguousGsfTracksSize();
 float dist = electron.convDist();
 float dcot = electron.convDcot();

 if(
         (pt > 20.) &&
         (fabs(eta) < 2.5) &&
          // EleID WP80 - 2010
         ( ( (isEB == 1) && (combIso/pt    < 0.070) ) || ( (isEB == 0) && (combIso/pt    < 0.060) ) ) &&
         ( ( (isEB == 1) && (sigmaIetaIeta < 0.010) ) || ( (isEB == 0) && (sigmaIetaIeta < 0.030) ) ) &&
         ( ( (isEB == 1) && (fabs(DphiIn)  < 0.060) ) || ( (isEB == 0) && (fabs(DphiIn)  < 0.030) ) ) &&
         ( ( (isEB == 1) && (fabs(DetaIn)  < 0.004) ) || ( (isEB == 0) && (fabs(DetaIn)  < 0.007) ) ) &&
          //( ( (isEB == 1) && (HOverE        < 0.040) ) || ( (isEB == 0) && (HOverE        < 0.025) ) ) &&
         ( mishits == 0 ) &&
         ( nAmbiguousGsfTracks == 0 ) &&
         ( ( fabs(dist) > 0.02 ) || ( fabs(dcot) > 0.02 ) )
        )
   isTightEle=true;

return isTightEle;

}

bool  SimpleNtupleEoverP::MediumEle (const edm::Event & iEvent, const edm::EventSetup & iESetup, const int &iEle){

 //************* ELECTRONS
 Handle<View<reco::GsfElectron> > electronHandle;
 iEvent.getByLabel(EleTag_,electronHandle);
 View<reco::GsfElectron> electrons = *electronHandle;

 bool isMediumEle=false;

 reco::GsfElectron electron = electrons.at(iEle);

 edm::InputTag EleBad = edm::InputTag("gsfElectrons");
 
 if(EleTag_== EleBad && !electron.ecalDriven()) return false ; 
 

 float pt = electron.pt();
 float eta = electron.eta();
      
 float tkIso  = electron.dr03TkSumPt();
 float emIso  = electron.dr03EcalRecHitSumEt();
 float hadIso = electron.dr03HcalDepth1TowerSumEt()+electron.dr03HcalDepth2TowerSumEt();
 float combIso = tkIso + emIso + hadIso - rho*0.3*0.3*3.14159;
      
 int isEB = electron.isEB(); 
 float sigmaIetaIeta = electron.sigmaIetaIeta();
 float DetaIn        = electron.deltaEtaSuperClusterTrackAtVtx();
 float DphiIn        = electron.deltaPhiSuperClusterTrackAtVtx();
 float HOverE        = electron.hadronicOverEm();
      
 int mishits             = electron.gsfTrack()->trackerExpectedHitsInner().numberOfHits();
 int nAmbiguousGsfTracks = electron.ambiguousGsfTracksSize();
 float dist = electron.convDist();
 float dcot = electron.convDcot();

 if(
         (pt > 12.) &&
         (fabs(eta) < 2.5) &&
          // EleID WP80 - 2010
         ( ( (isEB == 1) && (combIso/pt    < 0.070) ) || ( (isEB == 0) && (combIso/pt    < 0.060) ) ) &&
         ( ( (isEB == 1) && (sigmaIetaIeta < 0.010) ) || ( (isEB == 0) && (sigmaIetaIeta < 0.030) ) ) &&
         ( ( (isEB == 1) && (fabs(DphiIn)  < 0.060) ) || ( (isEB == 0) && (fabs(DphiIn)  < 0.030) ) ) &&
         ( ( (isEB == 1) && (fabs(DetaIn)  < 0.004) ) || ( (isEB == 0) && (fabs(DetaIn)  < 0.007) ) ) &&
          //( ( (isEB == 1) && (HOverE        < 0.040) ) || ( (isEB == 0) && (HOverE        < 0.025) ) ) &&
         ( mishits == 0 ) &&
         ( nAmbiguousGsfTracks == 0 ) &&
         ( ( fabs(dist) > 0.02 ) || ( fabs(dcot) > 0.02 ) )
        )
    isMediumEle=true;


return isMediumEle;
}
// ----------------------------------------------------------------------------------------
bool SimpleNtupleEoverP::LooseEle (const edm::Event & iEvent, const edm::EventSetup & iESetup,const int &iEle){

 //************* ELECTRONS
 Handle<View<reco::GsfElectron> > electronHandle;
 iEvent.getByLabel(EleTag_,electronHandle);
 View<reco::GsfElectron> electrons = *electronHandle;
  
 bool isLooseEle = false;

 reco::GsfElectron electron = electrons.at(iEle);
 
 edm::InputTag EleBad = edm::InputTag("gsfElectrons");
 
 if(EleTag_== EleBad && !electron.ecalDriven()) return false; 
 
 float pt = electron.pt();
 float eta = electron.eta();
      
 float tkIso  = electron.dr03TkSumPt();
 float emIso  = electron.dr03EcalRecHitSumEt();
 float hadIso = electron.dr03HcalDepth1TowerSumEt()+electron.dr03HcalDepth2TowerSumEt();
 float combIso = tkIso + emIso + hadIso - rho*0.3*0.3*3.14159;
      
 int isEB = electron.isEB(); 
 float sigmaIetaIeta = electron.sigmaIetaIeta();
 float DetaIn        = electron.deltaEtaSuperClusterTrackAtVtx();
 float DphiIn        = electron.deltaPhiSuperClusterTrackAtVtx();
 float HOverE        = electron.hadronicOverEm();
       
 if( 
          (pt > 10.) &&
          (fabs(eta) < 2.5) &&
          // EleID WP95 - 2010
          ( ( (isEB == 1) && (combIso/pt    < 0.150) ) || ( (isEB == 0) && (combIso/pt    < 0.100) ) ) &&
          ( ( (isEB == 1) && (sigmaIetaIeta < 0.010) ) || ( (isEB == 0) && (sigmaIetaIeta < 0.030) ) ) &&
          ( ( (isEB == 1) && (fabs(DphiIn)  < 0.800) ) || ( (isEB == 0) && (fabs(DphiIn)  < 0.700) ) ) &&
          ( ( (isEB == 1) && (fabs(DetaIn)  < 0.007) ) || ( (isEB == 0) && (fabs(DetaIn)  < 0.010) ) ) &&
          ( ( (isEB == 1) && (HOverE        < 0.150) ) || ( (isEB == 0) && (HOverE        < 0.070) ) )
        )
        isLooseEle=true;

return isLooseEle;
}

// -----------------------------------------------------------------------------------------

bool SimpleNtupleEoverP::myWselection (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{ 

  float combIso = (ele1_tkIso + ele1_emIso + ele1_hadIso) - rho*0.3*0.3*3.14159;

  if( ele1_pt < 30. ) return false;
  if( ( ele1_isEB == 1 ) && ( combIso/ele1_pt > 0.04 ) ) return false;
  if( ( ele1_isEB == 1 ) && ( fabs(ele1_DphiIn) > 0.030 ) ) return false;
  if( ( ele1_isEB == 1 ) && ( fabs(ele1_DetaIn) > 0.004 ) ) return false;
  if( ( ele1_isEB == 1 ) && ( ele1_HOverE > 0.025 ) ) return false;
  if( ( ele1_isEB == 0 ) && ( combIso/ele1_pt > 0.03 ) ) return false;
  if( ( ele1_isEB == 0 ) && ( fabs(ele1_DphiIn) > 0.020 ) ) return false;
  if( ( ele1_isEB == 0 ) && ( fabs(ele1_DetaIn) > 0.005 ) ) return false;
  if( ( ele1_isEB == 0 ) && ( ele1_HOverE > 0.025 ) ) return false;
       
  if( met_et       < 25.00 ) return false;
  if( ele1Met_mt   < 50.00 ) return false;
  if( ele1Met_Dphi <  1.57 ) return false;
  
  return true;
  
}

// -----------------------------------------------------------------------------------------

bool SimpleNtupleEoverP::myZselection (const edm::Event & iEvent, const edm::EventSetup & iSetup)
{ 
   if( met_et     >  40. ) return false;
   if( ele1ele2_m <  60. ) return false;
   if( ele1ele2_m > 120. ) return false;
   if( (ele1_charge * ele2_charge) != -1. ) return false;
      
  return true;
  
}

// -----------------------------------------------------------------------------------------
 
void SimpleNtupleEoverP::fillPVInfo (const edm::Event & iEvent, const edm::EventSetup & iESetup){

edm::Handle<reco::VertexCollection> vertexes;
iEvent.getByLabel(PVTag_, vertexes);
PV_n = vertexes -> size();

reco::Vertex PV;
bool PVfound = (vertexes -> size() != 0);
if(PVfound){
    
    PV = vertexes->at(0);
    PV_z = PV.z();
    PV_d0 = PV.position().Rho();
   
  }
  else {
    //creating a dummy PV
    PV_z=-999.;
    PV_d0=-999.;
   }  

  math::XYZPoint PVPoint(PV.position().x(), PV.position().y(), PV.position().z());
  PVPoint_ = PVPoint;
 
}
//------------------------------------------------------------------------------------------------------------
void SimpleNtupleEoverP::fillRhoInfo(const edm::Event & iEvent, const edm::EventSetup & iSetup) 
{
  
  Handle< double > rhoHandle;
  iEvent.getByLabel(rhoTag_,rhoHandle);
  rho = *rhoHandle;  
}

// ------------------------------------------------------------------------------------------------------------

void SimpleNtupleEoverP::fillEleInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup, const int iEle, const std::string eleName )
{ 
  //*********** CALO TOPOLOGY
  edm::ESHandle<CaloTopology> pTopology;
  iSetup.get<CaloTopologyRecord>().get(pTopology);
  const CaloTopology *topology = pTopology.product();

  //*********** IC CONSTANTS
  edm::ESHandle<EcalIntercalibConstants> theICConstants;
  iSetup.get<EcalIntercalibConstantsRcd>().get(theICConstants);
  const EcalIntercalibConstantMap& ICMap = theICConstants->getMap();
  
   
 //*********** ADCToGeV
  edm::ESHandle<EcalADCToGeVConstant> theADCToGeV;
  iSetup.get<EcalADCToGeVConstantRcd>().get(theADCToGeV);

   
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
    std::cerr << "SimpleNtupleEoverP::analyze --> recHitsEB not found" << std::endl; 
  }
  
  //*********** EE REC HITS
  edm::Handle<EcalRecHitCollection> recHitsEE;
  iEvent.getByLabel( recHitCollection_EE_, recHitsEE );
  const EcalRecHitCollection* theEndcapEcalRecHits = recHitsEE.product () ;
  if ( ! recHitsEE.isValid() ) {
    std::cerr << "SimpleNtupleEoverP::analyze --> recHitsEE not found" << std::endl; 
  }

  //************* ELECTRONS
  Handle<View<reco::GsfElectron> > electronHandle;
  iEvent.getByLabel(EleTag_,electronHandle);
  View<reco::GsfElectron> electrons = *electronHandle;

  //************* VERTEX COLLECTION
  edm::Handle<reco::VertexCollection> hVertexProduct;
  iEvent.getByLabel("offlinePrimaryVerticesWithBS",hVertexProduct);

  //************* CLUSTER LAZY TOOLS
  if( !ecorr_.IsInitialized() ){
   ecorr_.Initialize(iSetup,"/afs/cern.ch/work/r/rgerosa/CMSSW_4_2_8_patch3/src/Calibration/EcalCalibNtuple/test/crab/gbrv2ele.root");
   //ecorr_.Initialize(iSetup,"wgbrph",true); // --- > FIXME : use ele regression!!! weights in DB not meanngful for now
  }
 
  EcalClusterLazyTools lazyTools(iEvent,iSetup,recHitCollection_EB_,recHitCollection_EE_); 
 
 //************* CLUSTER PU CLEANING TOOLS
  EcalClusterPUCleaningTools cleaningTools(iEvent, iSetup, recHitCollection_EB_, recHitCollection_EE_); 
  
  // Take the correct ele
  reco::GsfElectron electron = electrons.at(iEle);
      
  if ( eleName == "ele1") {
 
    edm::InputTag EleBad = edm::InputTag("gsfElectrons");
 
    if(EleTag_== EleBad && !electron.ecalDriven()) return ; 
 
    ele1=electron.p4();
    ele1_charge=electron.charge();
    ele1_p=ele1.P();
    ele1_pt=ele1.Pt();
    ele1_eta=ele1.eta();
    ele1_phi=ele1.phi();

    ele1_isEB=electron.isEB();
    ele1_isEBEEGap=electron.isEBEEGap();
    ele1_isEBEtaGap=electron.isEBEtaGap();
    ele1_isEBPhiGap=electron.isEBPhiGap();
    ele1_isEEDeeGap=electron.isEEDeeGap();
    ele1_isEERingGap=electron.isEERingGap();
 
    ele1_sigmaIetaIeta=electron.sigmaIetaIeta();
    ele1_DphiIn=electron.deltaPhiSuperClusterTrackAtVtx();
    ele1_DetaIn=electron.deltaEtaSuperClusterTrackAtVtx();
    ele1_HOverE=electron.hadronicOverEm();
    ele1_tkIso=electron.dr03TkSumPt();
    ele1_emIso=electron.dr03EcalRecHitSumEt();
    ele1_hadIso=electron.dr03HcalDepth1TowerSumEt()+electron.dr03HcalDepth2TowerSumEt();


    reco::GsfTrackRef eleTrack  = electron.gsfTrack() ; 
    ele1_dxy_PV  =eleTrack->dxy (PVPoint_);
    ele1_dz_PV  =eleTrack->dz (PVPoint_);
    ele1_deltaEtaSuperClusterAtVtx =electron.deltaEtaSuperClusterTrackAtVtx();
    ele1_deltaPhiSuperClusterAtVtx = electron.deltaPhiSuperClusterTrackAtVtx();
    ele1_sigmaP = electron.corrections().trackMomentumError;
   
    reco::SuperClusterRef scRef = electron.superCluster();
    const edm::Ptr<reco::CaloCluster>& seedCluster = scRef->seed();
   
    double R  = TMath::Sqrt(scRef->x()*scRef->x() + scRef->y()*scRef->y() +scRef->z()*scRef->z());
    double Rt = TMath::Sqrt(scRef->x()*scRef->x() + scRef->y()*scRef->y());
   
    std::pair<double,double> cor = ecorr_.CorrectedEnergyWithErrorV2(electron,*hVertexProduct,lazyTools,iSetup);


    ele1_scERaw=scRef->rawEnergy();
    ele1_scEtRaw=scRef->rawEnergy()*(Rt/R);
    ele1_scEt=scRef->energy()*(Rt/R);
    ele1_scEtaWidth=scRef->etaWidth();
    ele1_scPhiWidth=scRef->phiWidth();
    ele1_scE=scRef->energy();
    ele1_scEta=scRef->eta();
    ele1_scPhi=scRef->phi();
    ele1_scE_regression=cor.first;
    ele1_scEerr_regression = cor.second;
 
 
    EcalClusterLocal ecalLocalCoord;
    float bcLocalEta, bcLocalPhi, bcThetatilt, bcPhitilt;  
    int bcIeta, bcIphi;
    bcLocalEta = 0;
   
    if ( electron.isEB() )
     ecalLocalCoord.localCoordsEB(*seedCluster,iSetup,bcLocalEta,bcLocalPhi,bcIeta,bcIphi,bcThetatilt,bcPhitilt);  
    if ( electron.isEE() )
    ecalLocalCoord.localCoordsEE(*seedCluster,iSetup,bcLocalEta,bcLocalPhi,bcIeta,bcIphi,bcThetatilt,bcPhitilt);
     

    ele1_scLocalEta=bcLocalEta;
    ele1_scLocalPhi=bcLocalPhi;
   

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

   ele1_scCrackCorr=crackcor;
   ele1_scLocalContCorr=localContCorr;

  
   reco::SuperCluster cleanedSC   = cleaningTools.CleanedSuperCluster(0.02, *scRef, iEvent );
   reco::CaloClusterPtr myseed = (*scRef).seed();
   if (  !((myseed->seed()).rawId()) || (myseed->seed()).rawId()==0 ) {
    ele1_scERaw_PUcleaned=-9999.;
    ele1_scEtaWidth_PUcleaned=-9999.;
    ele1_scPhiWidth_PUcleaned=-9999.;
    ele1_fCorrection_PUcleaned=-9999.;
   }
   else{
          ele1_scERaw_PUcleaned=cleanedSC.energy();
          ele1_scEtaWidth_PUcleaned=cleanedSC.etaWidth();
          ele1_scPhiWidth_PUcleaned=cleanedSC.phiWidth();   
          float fCorrCleaned = fClusterCorrections(cleanedSC.energy() + scRef->preshowerEnergy(), cleanedSC.eta(),cleanedSC.phiWidth()/cleanedSC.etaWidth(),params)/(cleanedSC.energy()+ scRef->preshowerEnergy());
          ele1_fCorrection_PUcleaned=fCorrCleaned;
  }


  ele1_fEta = scRef->energy()/scRef->rawEnergy();
  ele1_tkP = electron.trackMomentumAtVtx().R();
  ele1_tkPt=electron.trackMomentumAtVtx().Rho();
  ele1_fbrem=electron.fbrem();
  ele1_e5x5=electron.e5x5();
  ele1_es=scRef->preshowerEnergy();
  
  float E3x3 = 0;
  
  if ( electron.isEB() ) {
     E3x3 = EcalClusterTools::e3x3( *scRef, theBarrelEcalRecHits, topology);
  }

  if ( electron.isEE() ){
     E3x3 = EcalClusterTools::e3x3( *scRef, theEndcapEcalRecHits, topology);
  }
 
  ele1_e3x3=E3x3;
  ele1_scNxtal = scRef->hitsAndFractions().size();
 

  float energy=0.;
  int ieta=0;
  int iphi=0;
  int ix=0;
  int iy=0;
  int zside=0;
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
       ieta = (EBDetId(id.first)).ieta();
       iphi = (EBDetId(id.first)).iphi();
       ix = -999;
       iy = -999;
       zside = 0;
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
       ix = (EEDetId(id.first)).ix();
       iy = (EEDetId(id.first)).iy();
       ieta = -999;
       iphi = -999;
       zside = (EEDetId(id.first)).zside();
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

   ele1_seedE=energy;
   ele1_seedLaserAlpha=seedLaserAlpha;
   ele1_seedLaserCorr=seedLaserCorrection;
   ele1_seedICConstant=seedICConstant;
   ele1_seedIeta =ieta;
   ele1_seedIphi = iphi;
   ele1_seedIx=ix;
   ele1_seedIy=iy;
   ele1_seedZside=zside;
   ele1_EOverP=electron.eSuperClusterOverP();
           
   // rechit variables
   int numRecHit = 0;
   float sumRecHitE = 0.;
   float sumLaserCorrectionRecHitE = 0.;
   float sumRecHitE5x5 = 0.;
   float sumLaserCorrectionRecHitE5x5 = 0.;
   float sumRecHitE3x3 = 0.;
   float sumLaserCorrectionRecHitE3x3 = 0.;
    
    bool printOut = false;
    const std::vector<std::pair<DetId,float> >& hits = scRef->hitsAndFractions();

    if( printOut )
    {
      std::cout << "runId: " << iEvent.id().run() 
          << std::fixed
          << "   electron eta: " << std::setprecision(2) << std::setw(5) << electron.eta()
          << "   electron phi: " << std::setprecision(2) << std::setw(5) << electron.phi()
  //           << "   SC energy: "    << std::setprecision(2) << std::setw(6) << scRef -> energy()
          << std::endl;
    } 
        
    for(std::vector<std::pair<DetId,float> >::const_iterator rh = hits.begin(); rh!=hits.end(); ++rh)
    {
      float rhLaserCorrection = 1.;
      float rhICCorrection = 1.;
      float theLaserCorrection = -1.;
      float theICCorrection = -1.;
      
      if ((*rh).first.subdetId()== EcalBarrel)
      {
        EBRecHitCollection::const_iterator itrechit = theBarrelEcalRecHits->find((*rh).first);
        if (itrechit==theBarrelEcalRecHits->end()) continue;
        EBDetId barrelId (itrechit->id ()); 
        ++numRecHit;
                      
        // laser correction
        theLaserCorrection = theLaser->getLaserCorrection(barrelId, iEvent.time());
        if ( applyCorrections_ ) rhLaserCorrection = theLaserCorrection;
        // IC correction
        EcalIntercalibConstantMap::const_iterator ICMapIt = ICMap.find(barrelId);
        theICCorrection = *ICMapIt;
        if ( applyCorrections_ ) rhICCorrection = theICCorrection;
        
        sumRecHitE += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
        sumLaserCorrectionRecHitE += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection;
        // check if rh is inside the 5x5 matrix
        if ( fabs(barrelId.ieta() - ele1_seedIeta) < 3 && fabs(barrelId.iphi() - ele1_seedIphi) < 3 ) {
          sumRecHitE5x5 += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
          sumLaserCorrectionRecHitE5x5 += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection;
        }
        // check if rh is inside the 3x3 matrix
        if ( fabs(barrelId.ieta() - ele1_seedIeta) < 1 && fabs(barrelId.iphi() - ele1_seedIphi) < 1 ) {
          sumRecHitE3x3 += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
          sumLaserCorrectionRecHitE3x3 += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection ;
        }
        // fill recHit variables
        ele1_recHit_E.push_back(itrechit->energy() * rhLaserCorrection);
        ele1_recHit_hashedIndex.push_back(barrelId.hashedIndex());
        ele1_recHit_ietaORix.push_back(barrelId.ieta());
        ele1_recHit_iphiORiy.push_back(barrelId.iphi());
        ele1_recHit_zside.push_back(0);
        ele1_recHit_laserCorrection.push_back(theLaserCorrection);
        ele1_recHit_ICConstant.push_back(theICCorrection);
 
        if( printOut && itrechit->energy() > 1. )
        {
          std::cout << std::fixed
              << "    recHitLC: "    << std::setprecision(6) << std::setw(8) << theLaserCorrection
              << "    recHitIC: "    << std::setprecision(6) << std::setw(8) << theICCorrection
              << std::endl;
        }
      }
        
      if ((*rh).first.subdetId()== EcalEndcap)
      {
        EERecHitCollection::const_iterator itrechit = theEndcapEcalRecHits->find((*rh).first);
        if (itrechit==theEndcapEcalRecHits->end()) continue;
        EEDetId endcapId (itrechit->id ()); 
        ++numRecHit;
          
        // laser correction
        theLaserCorrection = theLaser->getLaserCorrection(endcapId, iEvent.time());
        if ( applyCorrections_ ) rhLaserCorrection = theLaserCorrection;
        // IC correction
        EcalIntercalibConstantMap::const_iterator ICMapIt = ICMap.find(endcapId);
        theICCorrection = *ICMapIt;
        if ( applyCorrections_ ) rhICCorrection = theICCorrection;
        
        sumRecHitE += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
        sumLaserCorrectionRecHitE += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection;
        // check if rh is inside the 5x5 matrix
        if ( fabs(endcapId.ix() - ele1_seedIx) < 3 && fabs(endcapId.iy() - ele1_seedIy) < 3 ) {
          sumRecHitE5x5 += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
          sumLaserCorrectionRecHitE5x5 += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection;
        }
        // check if rh is inside the 3x3 matrix
        if ( fabs(endcapId.ix() - ele1_seedIx) < 1 && fabs(endcapId.iy() - ele1_seedIy) < 1 ) {
          sumRecHitE3x3 += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
          sumLaserCorrectionRecHitE3x3 += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection ;
        }
        // fill recHit variables
        ele1_recHit_E.push_back(itrechit->energy() * rhLaserCorrection);
        ele1_recHit_hashedIndex.push_back(endcapId.hashedIndex());
        ele1_recHit_ietaORix.push_back(endcapId.ix());
        ele1_recHit_iphiORiy.push_back(endcapId.iy());
        ele1_recHit_zside.push_back(endcapId.zside());
        ele1_recHit_laserCorrection.push_back(theLaserCorrection);
        ele1_recHit_ICConstant.push_back(theICCorrection);

        if( printOut && itrechit->energy() > 1. )
        {
          std::cout << std::fixed
              << "    recHitLC: "    << std::setprecision(6) << std::setw(8) << theLaserCorrection
              << "    recHitIC: "    << std::setprecision(6) << std::setw(8) << theICCorrection
              << std::endl;
        }
      }
    }
    
    ele1_scLaserCorr = sumLaserCorrectionRecHitE/sumRecHitE;
    if ( applyCorrections_ ) ele1_scE = scRef->energy()*ele1_fEta*sumRecHitE;
    else ele1_scE = scRef->energy();
    
    ele1_fEtaCorr = fClusterCorrections(sumRecHitE+ele1_es,ele1_scEta,scRef->phiWidth()/scRef->etaWidth(),params)/fClusterCorrections(scRef->rawEnergy()+ele1_es,ele1_scEta,scRef->phiWidth()/scRef->etaWidth(),params);
        
    ele1_5x5LaserCorr = sumLaserCorrectionRecHitE5x5/sumRecHitE5x5;
    ele1_e5x5 = sumRecHitE5x5;
  
    ele1_3x3LaserCorr = sumLaserCorrectionRecHitE3x3/sumRecHitE3x3;
   
    /// add regression input variables
    reco::SuperClusterRef s = electron.superCluster();
    reco::CaloClusterPtr b = s->seed(); //seed  basic cluster
    reco::CaloClusterPtr b2;
    reco::CaloClusterPtr bclast;
    reco::CaloClusterPtr bclast2;
    bool isbarrel =  b->hitsAndFractions().at(0).first.subdetId()==EcalBarrel;

     if( isbarrel){

    ele1_eRegrInput_rawE = s->rawEnergy();
    ele1_eRegrInput_r9 = lazyTools.e3x3(*b)/s->rawEnergy();
    ele1_eRegrInput_eta = s->eta();
    ele1_eRegrInput_phi = s->phi();
    ele1_eRegrInput_r25 = lazyTools.e5x5(*b)/s->rawEnergy();
    ele1_eRegrInput_hoe = electron.hcalOverEcal() ;
    ele1_eRegrInput_etaW = s->etaWidth() ;
    ele1_eRegrInput_phiW = s->phiWidth() ;
  
   
    //seed basic cluster variables
    double bemax = lazyTools.eMax(*b);
    double be2nd = lazyTools.e2nd(*b);
    double betop = lazyTools.eTop(*b);
    double bebottom = lazyTools.eBottom(*b);
    double beleft = lazyTools.eLeft(*b);
    double beright = lazyTools.eRight(*b);
 
    ele1_eRegrInput_Deta_bC_sC = b->eta()-s->eta();
    ele1_eRegrInput_Dphi_bC_sC = reco::deltaPhi(b->phi(),s->phi());
    ele1_eRegrInput_bCE_Over_sCE = lazyTools.e3x3(*b)/b->energy();
    ele1_eRegrInput_e3x3_Over_bCE = lazyTools.e3x3(*b)/b->energy();
    ele1_eRegrInput_e5x5_Over_bCE = lazyTools.e5x5(*b)/b->energy();
    ele1_eRegrInput_sigietaieta_bC1 = sqrt(lazyTools.localCovariances(*b)[0]);
    ele1_eRegrInput_sigiphiiphi_bC1 = sqrt(lazyTools.localCovariances(*b)[2]);
    ele1_eRegrInput_sigietaiphi_bC1 = lazyTools.localCovariances(*b)[1];
    ele1_eRegrInput_bEMax_Over_bCE = bemax/b->energy();
    ele1_eRegrInput_log_bE2nd_Over_bEMax = log(be2nd/bemax);
    ele1_eRegrInput_log_bEtop_Over_bEMax = log(betop/bemax);
    ele1_eRegrInput_log_bEbot_Over_bEMax = log(bebottom/bemax);
    ele1_eRegrInput_log_bEleft_Over_bEMax = log(beleft/bemax);
    ele1_eRegrInput_log_bEright_Over_bEMax = log(beright/bemax);
    ele1_eRegrInput_asym_top_bottom = (betop-bebottom)/(betop+bebottom);
    ele1_eRegrInput_asym_left_right = (beleft-beright)/(beleft+beright);

    //highest energy basic cluster excluding seed basic cluster
    double ebcmax = -99.;
    for (reco::CaloCluster_iterator bit = s->clustersBegin(); bit!=s->clustersEnd(); ++bit) {
     const reco::CaloClusterPtr bc = *bit;
     if (bc->energy() > ebcmax && bc !=b) {
       b2 = bc;
       ebcmax = bc->energy();
     }
    }
  
    //lowest energy basic cluster excluding seed (for pileup mitigation)
    double ebcmin = 1e6;
    for (reco::CaloCluster_iterator bit = s->clustersBegin(); bit!=s->clustersEnd(); ++bit) {
     const CaloClusterPtr bc = *bit;
     if (bc->energy() < ebcmin && bc !=b) {
      bclast = bc;
      ebcmin = bc->energy();
     }
    }

   //2nd lowest energy basic cluster excluding seed (for pileup mitigation)
    ebcmin = 1e6;
    for (reco::CaloCluster_iterator bit = s->clustersBegin(); bit!=s->clustersEnd(); ++bit) {
     const CaloClusterPtr bc = *bit;
     if (bc->energy() < ebcmin && bc !=b && bc!=bclast) {
       bclast2 = bc;
       ebcmin = bc->energy();
     }
    }
 
    bool hasbc2 = b2.isNonnull() && b2->energy()>0.;
    bool hasbclast = bclast.isNonnull() && bclast->energy()>0.;
    bool hasbclast2 = bclast2.isNonnull() && bclast2->energy()>0.;
  
    double bc2emax = hasbc2 ? lazyTools.eMax(*b2) : 0.;
    double bc2e2nd = hasbc2 ? lazyTools.e2nd(*b2) : 0.;
    double bc2etop = hasbc2 ? lazyTools.eTop(*b2) : 0.;
    double bc2ebottom = hasbc2 ? lazyTools.eBottom(*b2) : 0.;
    double bc2eleft = hasbc2 ? lazyTools.eLeft(*b2) : 0.;
    double bc2eright = hasbc2 ? lazyTools.eRight(*b2) : 0.;

    ele1_eRegrInput_Deta_bC2_sC = hasbc2 ? (b2->eta()-s->eta()) : 0.;
    ele1_eRegrInput_Dphi_bC2_sC = hasbc2 ? reco::deltaPhi(b2->phi(),s->phi()) : 0.;
    ele1_eRegrInput_bCE2_Over_sCE = hasbc2 ? b2->energy()/s->rawEnergy() : 0.;
    ele1_eRegrInput_e3x3_Over_bCE2 = hasbc2 ? lazyTools.e3x3(*b2)/b2->energy() : 0.;
    ele1_eRegrInput_e5x5_Over_bCE2 = hasbc2 ? lazyTools.e5x5(*b2)/b2->energy() : 0.;
    ele1_eRegrInput_sigietaieta_bC2 = hasbc2 ? sqrt(lazyTools.localCovariances(*b2)[0]) : 0.;
    ele1_eRegrInput_sigiphiiphi_bC2 = hasbc2 ? sqrt(lazyTools.localCovariances(*b2)[2]) : 0.;
    ele1_eRegrInput_sigietaiphi_bC2 = hasbc2 ? lazyTools.localCovariances(*b)[1] : 0.;
    ele1_eRegrInput_bEMax_Over_bCE2 = hasbc2 ? bc2emax/b2->energy() : 0.;
    ele1_eRegrInput_log_bE2nd_Over_bEMax2 = hasbc2 ? log(bc2e2nd/bc2emax) : 0.;
    ele1_eRegrInput_log_bEtop_Over_bEMax2 = hasbc2 ? log(bc2etop/bc2emax) : 0.;
    ele1_eRegrInput_log_bEbot_Over_bEMax2 = hasbc2 ? log(bc2ebottom/bc2emax) : 0.;
    ele1_eRegrInput_log_bEleft_Over_bEMax2 = hasbc2 ? log(bc2eleft/bc2emax) : 0.;
    ele1_eRegrInput_log_bEright_Over_bEMax2 = hasbc2 ? log(bc2eright/bc2emax) : 0.;
    ele1_eRegrInput_asym_top2_bottom2 = hasbc2 ? (bc2etop-bc2ebottom)/(bc2etop+bc2ebottom) : 0.;
    ele1_eRegrInput_asym_left2_right2 = hasbc2 ? (bc2eleft-bc2eright)/(bc2eleft+bc2eright) : 0.;
    

    ele1_eRegrInput_Deta_bCLow_sC = hasbclast ? (bclast->eta()-s->eta()) : 0.;
    ele1_eRegrInput_Dphi_bCLow_sC = hasbclast ? reco::deltaPhi(bclast->phi(),s->phi()) : 0.;
    ele1_eRegrInput_bCELow_Over_sCE = hasbclast ? bclast->energy()/s->rawEnergy() : 0.;
    ele1_eRegrInput_e3x3_Over_bCELow = hasbclast ? lazyTools.e3x3(*bclast)/bclast->energy() : 0.;
    ele1_eRegrInput_e5x5_Over_bCELow = hasbclast ? lazyTools.e5x5(*bclast)/bclast->energy() : 0.;
    ele1_eRegrInput_sigietaieta_bCLow = hasbclast ? sqrt(lazyTools.localCovariances(*bclast)[0]) : 0.;
    ele1_eRegrInput_sigiphiiphi_bCLow = hasbclast ? sqrt(lazyTools.localCovariances(*bclast)[2]) : 0.;
    ele1_eRegrInput_sigietaiphi_bCLow = hasbclast ? lazyTools.localCovariances(*bclast)[1] : 0.;
  
    ele1_eRegrInput_Deta_bCLow2_sC = hasbclast2 ? (bclast2->eta()-s->eta()) : 0.;
    ele1_eRegrInput_Dphi_bCLow2_sC = hasbclast2 ? reco::deltaPhi(bclast2->phi(),s->phi()) : 0.;
    ele1_eRegrInput_bCELow2_Over_sCE = hasbclast2 ? bclast2->energy()/s->rawEnergy() : 0.;
    ele1_eRegrInput_e3x3_Over_bCELow2 = hasbclast2 ? lazyTools.e3x3(*bclast2)/bclast2->energy() : 0.;
    ele1_eRegrInput_e5x5_Over_bCELow2 = hasbclast2 ? lazyTools.e5x5(*bclast2)/bclast2->energy() : 0.;
    ele1_eRegrInput_sigietaieta_bCLow2 = hasbclast2 ? sqrt(lazyTools.localCovariances(*bclast2)[0]) : 0.;
    ele1_eRegrInput_sigiphiiphi_bCLow2 = hasbclast2 ? sqrt(lazyTools.localCovariances(*bclast2)[2]) : 0.;
    ele1_eRegrInput_sigietaiphi_bCLow2 = hasbclast2 ? lazyTools.localCovariances(*bclast2)[1] : 0.;
  
    // seed cluster
    float betacry, bphicry, bthetatilt, bphitilt;
    int bieta, biphi;
    EcalClusterLocal _ecalLocal;
    _ecalLocal.localCoordsEB(*b,iSetup,betacry,bphicry,bieta,biphi,bthetatilt,bphitilt);
    
    ele1_eRegrInput_seedbC_eta = bieta;
    ele1_eRegrInput_seedbC_phi = biphi;
    ele1_eRegrInput_seedbC_eta_p5 = bieta%5;
    ele1_eRegrInput_seedbC_phi_p2 = biphi%2;
    ele1_eRegrInput_seedbC_bieta = (TMath::Abs(bieta)<=25)*(bieta%25) + (TMath::Abs(bieta)>25)*((bieta-25*TMath::Abs(bieta)/bieta)%20);
    ele1_eRegrInput_seedbC_phi_p20 = biphi%20;
    ele1_eRegrInput_seedbC_etacry = betacry;
    ele1_eRegrInput_seedbC_phicry = bphicry;

    //2nd cluster (meaningful gap corrections for converted photons)
    float bc2etacry, bc2phicry, bc2thetatilt, bc2phitilt;
    int bc2ieta, bc2iphi;
    if (hasbc2) _ecalLocal.localCoordsEB(*b2,iSetup,bc2etacry,bc2phicry,bc2ieta,bc2iphi,bc2thetatilt,bc2phitilt);    
  
    ele1_eRegrInput_bC2_eta = hasbc2 ? bc2ieta : 0.;
    ele1_eRegrInput_bC2_phi = hasbc2 ? bc2iphi : 0.;
    ele1_eRegrInput_bC2_eta_p5 = hasbc2 ? bc2ieta%5 : 0.;
    ele1_eRegrInput_bC2_phi_p2 = hasbc2 ? bc2iphi%2 : 0.;
    ele1_eRegrInput_bC2_bieta = hasbc2 ? (TMath::Abs(bc2ieta)<=25)*(bc2ieta%25) + (TMath::Abs(bc2ieta)>25)*((bc2ieta-25*TMath::Abs(bc2ieta)/bc2ieta)%20) : 0.;
    ele1_eRegrInput_bC2_phi_p20 = hasbc2 ? bc2iphi%20 : 0.;
    ele1_eRegrInput_bC2_etacry = hasbc2 ? bc2etacry : 0.;
    ele1_eRegrInput_bC2_phicry = hasbc2 ? bc2phicry : 0.;


    ele1_eRegrInput_nPV = hVertexProduct->size();
  }
  else{
 
        ele1_eRegrInput_rawE = s->rawEnergy();
        ele1_eRegrInput_r9 = lazyTools.e3x3(*b)/s->rawEnergy();
        ele1_eRegrInput_eta = s->eta();
        ele1_eRegrInput_phi = s->phi();
        ele1_eRegrInput_nPV = hVertexProduct->size();
        ele1_eRegrInput_r25 = lazyTools.e5x5(*b)/s->rawEnergy();
        ele1_eRegrInput_etaW = s->etaWidth() ;
        ele1_eRegrInput_phiW = s->phiWidth() ;
  
        ele1_eRegrInput_hoe = -99.;
        ele1_eRegrInput_Deta_bC_sC = -99.;
        ele1_eRegrInput_Dphi_bC_sC = -99.;
        ele1_eRegrInput_bCE_Over_sCE = -99.;
        ele1_eRegrInput_e3x3_Over_bCE = -99.;
        ele1_eRegrInput_e5x5_Over_bCE = -99.;
        ele1_eRegrInput_sigietaieta_bC1 = -99.;
        ele1_eRegrInput_sigiphiiphi_bC1 = -99.;
        ele1_eRegrInput_sigietaiphi_bC1 = -99.;
        ele1_eRegrInput_bEMax_Over_bCE = -99.;
        ele1_eRegrInput_log_bE2nd_Over_bEMax = -99.;
        ele1_eRegrInput_log_bEtop_Over_bEMax = -99.;
        ele1_eRegrInput_log_bEbot_Over_bEMax = -99.;
        ele1_eRegrInput_log_bEleft_Over_bEMax = -99.;
        ele1_eRegrInput_log_bEright_Over_bEMax = -99.;
        ele1_eRegrInput_asym_top_bottom = -99.;
        ele1_eRegrInput_asym_left_right = -99.;
        ele1_eRegrInput_Deta_bC2_sC = -99.;
        ele1_eRegrInput_Dphi_bC2_sC = -99.;
        ele1_eRegrInput_bCE2_Over_sCE = -99.;
        ele1_eRegrInput_e3x3_Over_bCE2 = -99.;
        ele1_eRegrInput_e5x5_Over_bCE2 = -99.;
        ele1_eRegrInput_sigietaieta_bC2 = -99.;
        ele1_eRegrInput_sigiphiiphi_bC2 = -99.;
        ele1_eRegrInput_sigietaiphi_bC2 = -99.;
        ele1_eRegrInput_bEMax_Over_bCE2 = -99.;
        ele1_eRegrInput_log_bE2nd_Over_bEMax2 = -99.;
        ele1_eRegrInput_log_bEtop_Over_bEMax2 = -99.;
        ele1_eRegrInput_log_bEbot_Over_bEMax2 = -99.;
        ele1_eRegrInput_log_bEleft_Over_bEMax2 = -99.;
        ele1_eRegrInput_log_bEright_Over_bEMax2 = -99.;
        ele1_eRegrInput_asym_top2_bottom2 = -99.;
        ele1_eRegrInput_asym_left2_right2 = -99.;
        ele1_eRegrInput_Deta_bCLow_sC = -99.;
        ele1_eRegrInput_Dphi_bCLow_sC = -99.;
        ele1_eRegrInput_bCELow_Over_sCE = -99.;
        ele1_eRegrInput_e3x3_Over_bCELow = -99.;
        ele1_eRegrInput_e5x5_Over_bCELow = -99.;
        ele1_eRegrInput_sigietaieta_bCLow = -99.;
        ele1_eRegrInput_sigiphiiphi_bCLow = -99.;
        ele1_eRegrInput_sigietaiphi_bCLow = -99.;  
        ele1_eRegrInput_Deta_bCLow2_sC = -99.;
        ele1_eRegrInput_Dphi_bCLow2_sC = -99.;
        ele1_eRegrInput_bCELow2_Over_sCE = -99.;
        ele1_eRegrInput_e3x3_Over_bCELow2 = -99.;
        ele1_eRegrInput_e5x5_Over_bCELow2 = -99.;
        ele1_eRegrInput_sigietaieta_bCLow2 = -99.;
        ele1_eRegrInput_sigiphiiphi_bCLow2 = -99.;
        ele1_eRegrInput_sigietaiphi_bCLow2 = -99.;
        ele1_eRegrInput_seedbC_eta = -99.;
        ele1_eRegrInput_seedbC_phi = -99.;
        ele1_eRegrInput_seedbC_eta_p5 = -99.;
        ele1_eRegrInput_seedbC_phi_p2 = -99.;
        ele1_eRegrInput_seedbC_bieta = -99.;
        ele1_eRegrInput_seedbC_phi_p20 = -99.;
        ele1_eRegrInput_seedbC_etacry = -99.;
        ele1_eRegrInput_seedbC_phicry = -99.;

        ele1_eRegrInput_bC2_eta = -99.;
        ele1_eRegrInput_bC2_phi = -99.;
        ele1_eRegrInput_bC2_eta_p5 = -99.;
        ele1_eRegrInput_bC2_phi_p2 = -99.;
        ele1_eRegrInput_bC2_bieta = -99.;
        ele1_eRegrInput_bC2_phi_p20 = -99.;
        ele1_eRegrInput_bC2_etacry = -99.;
        ele1_eRegrInput_bC2_phicry =-99.; 
  }



}

 if ( eleName == "ele2" ) {

    edm::InputTag EleBad = edm::InputTag("gsfElectrons");
   
    if(EleTag_== EleBad && !electron.ecalDriven()) return ; 
 
    ele2=electron.p4();
    ele2_charge=electron.charge();
    ele2_p=ele2.P();
    ele2_pt=ele2.Pt();
    ele2_eta=ele2.eta();
    ele2_phi=ele2.phi();


    ele2_isEB=electron.isEB();
    ele2_isEBEEGap=electron.isEBEEGap();
    ele2_isEBEtaGap=electron.isEBEtaGap();
    ele2_isEBPhiGap=electron.isEBPhiGap();
    ele2_isEEDeeGap=electron.isEEDeeGap();
    ele2_isEERingGap=electron.isEERingGap();

    ele2_sigmaIetaIeta=electron.sigmaIetaIeta();
    ele2_DphiIn=electron.deltaPhiSuperClusterTrackAtVtx();
    ele2_DetaIn=electron.deltaEtaSuperClusterTrackAtVtx();
    ele2_HOverE=electron.hadronicOverEm();
    ele2_tkIso=electron.dr03TkSumPt();
    ele2_emIso=electron.dr03EcalRecHitSumEt();
    ele2_hadIso=electron.dr03HcalDepth1TowerSumEt()+electron.dr03HcalDepth2TowerSumEt();

    reco::GsfTrackRef eleTrack  = electron.gsfTrack() ; 
    ele2_dxy_PV  =eleTrack->dxy (PVPoint_);
    ele2_dz_PV  =eleTrack->dz (PVPoint_);
    ele2_deltaEtaSuperClusterAtVtx =electron.deltaEtaSuperClusterTrackAtVtx();
    ele2_deltaPhiSuperClusterAtVtx = electron.deltaPhiSuperClusterTrackAtVtx();
    ele2_sigmaP = electron.corrections().trackMomentumError;
   

    reco::SuperClusterRef scRef = electron.superCluster();
    const edm::Ptr<reco::CaloCluster>& seedCluster = scRef->seed();
   
    double R  = TMath::Sqrt(scRef->x()*scRef->x() + scRef->y()*scRef->y() +scRef->z()*scRef->z());
    double Rt = TMath::Sqrt(scRef->x()*scRef->x() + scRef->y()*scRef->y());
   
    std::pair<double,double> cor = ecorr_.CorrectedEnergyWithErrorV2(electron,*hVertexProduct,lazyTools,iSetup);


    ele2_scERaw=scRef->rawEnergy();
    ele2_scEtRaw=scRef->rawEnergy()*(Rt/R);
    ele2_scEt=scRef->energy()*(Rt/R);
    ele2_scEtaWidth=scRef->etaWidth();
    ele2_scPhiWidth=scRef->phiWidth();
    ele2_scE=scRef->energy();
    ele2_scEta=scRef->eta();
    ele2_scPhi=scRef->phi();
    ele2_scE_regression=cor.first;
    ele2_scEerr_regression = cor.second;
 
 
    EcalClusterLocal ecalLocalCoord;
    float bcLocalEta, bcLocalPhi, bcThetatilt, bcPhitilt;  
    int bcIeta, bcIphi;
    bcLocalEta = 0;
   
    if ( electron.isEB() )
     ecalLocalCoord.localCoordsEB(*seedCluster,iSetup,bcLocalEta,bcLocalPhi,bcIeta,bcIphi,bcThetatilt,bcPhitilt);  
    if ( electron.isEE() )
    ecalLocalCoord.localCoordsEE(*seedCluster,iSetup,bcLocalEta,bcLocalPhi,bcIeta,bcIphi,bcThetatilt,bcPhitilt);
     

    ele2_scLocalEta=bcLocalEta;
    ele2_scLocalPhi=bcLocalPhi;
   

   // crack correction variables and local containment corrections
   EcalClusterCrackCorrection -> init(iSetup);
   EcalClusterLocalContCorrection -> init(iSetup);
   double crackcor = 1.;
   double localContCorr = 1.;

   for(reco::CaloCluster_iterator cIt = electron.superCluster()->clustersBegin();
       cIt != electron.superCluster()->clustersEnd(); ++cIt){
       const reco::CaloClusterPtr cc = *cIt; 
       crackcor *= ( (electron.superCluster()->rawEnergy() + (*cIt)->energy()*(EcalClusterCrackCorrection->getValue(*cc)-1.)) / electron.superCluster()->rawEnergy() );
       
     }
   localContCorr = EcalClusterLocalContCorrection->getValue(*electron.superCluster(), 1) ;

   ele2_scCrackCorr=crackcor;
   ele2_scLocalContCorr=localContCorr;

  
   reco::SuperCluster cleanedSC   = cleaningTools.CleanedSuperCluster(0.02, *scRef, iEvent );
   reco::CaloClusterPtr myseed = (*scRef).seed();
   if (  !((myseed->seed()).rawId()) || (myseed->seed()).rawId()==0 ) {
    ele2_scERaw_PUcleaned=-9999.;
    ele2_scEtaWidth_PUcleaned=-9999.;
    ele2_scPhiWidth_PUcleaned=-9999.;
    ele2_fCorrection_PUcleaned=-9999.;
   }
   else{
          ele2_scERaw_PUcleaned=cleanedSC.energy();
          ele2_scEtaWidth_PUcleaned=cleanedSC.etaWidth();
          ele2_scPhiWidth_PUcleaned=cleanedSC.phiWidth();   
          float fCorrCleaned = fClusterCorrections(cleanedSC.energy() + scRef->preshowerEnergy(), cleanedSC.eta(),cleanedSC.phiWidth()/cleanedSC.etaWidth(),params)/(cleanedSC.energy()+ scRef->preshowerEnergy());
          ele2_fCorrection_PUcleaned=fCorrCleaned;
  }


  ele2_fEta = scRef->energy()/scRef->rawEnergy();
  ele2_tkP = electron.trackMomentumAtVtx().R();
  ele2_tkPt=electron.trackMomentumAtVtx().Rho();
  ele2_fbrem=electron.fbrem();
  ele2_e5x5=electron.e5x5();
  ele2_es=scRef->preshowerEnergy();

  float E3x3 = 0;

  if ( electron.isEB() ) {
     E3x3 = EcalClusterTools::e3x3( *scRef, theBarrelEcalRecHits, topology);
  }

  if ( electron.isEE() ){
     E3x3 = EcalClusterTools::e3x3( *scRef, theEndcapEcalRecHits, topology);
  }
 
  ele2_e3x3=E3x3;
  ele2_scNxtal = scRef->hitsAndFractions().size();
 

  float energy=0.;
  int ieta=0;
  int iphi=0;
  int ix=0;
  int iy=0;
  int zside=0;
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
       ieta = (EBDetId(id.first)).ieta();
       iphi = (EBDetId(id.first)).iphi();
       ix = -999;
       iy = -999;
       zside = 0;
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
       ix = (EEDetId(id.first)).ix();
       iy = (EEDetId(id.first)).iy();
       ieta = -999;
       iphi = -999;
       zside = (EEDetId(id.first)).zside();
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

   ele2_seedE=energy;
   ele2_seedLaserAlpha=seedLaserAlpha;
   ele2_seedLaserCorr=seedLaserCorrection;
   ele2_seedICConstant=seedICConstant;
   ele2_seedIeta =ieta;
   ele2_seedIphi = iphi;
   ele2_seedIx=ix;
   ele2_seedIy=iy;
   ele2_seedZside=zside;
   ele2_EOverP=electron.eSuperClusterOverP();
           
   // rechit variables
   int numRecHit = 0;
   float sumRecHitE = 0.;
   float sumLaserCorrectionRecHitE = 0.;
   float sumRecHitE5x5 = 0.;
   float sumLaserCorrectionRecHitE5x5 = 0.;
   float sumRecHitE3x3 = 0.;
   float sumLaserCorrectionRecHitE3x3 = 0.;
    
    bool printOut = false;
    const std::vector<std::pair<DetId,float> >& hits = scRef->hitsAndFractions();

    if( printOut )
    {
      std::cout << "runId: " << iEvent.id().run() 
          << std::fixed
          << "   electron eta: " << std::setprecision(2) << std::setw(5) << electron.eta()
          << "   electron phi: " << std::setprecision(2) << std::setw(5) << electron.phi()
  //           << "   SC energy: "    << std::setprecision(2) << std::setw(6) << scRef -> energy()
          << std::endl;
    } 
        
    for(std::vector<std::pair<DetId,float> >::const_iterator rh = hits.begin(); rh!=hits.end(); ++rh)
    {
      float rhLaserCorrection = 1.;
      float rhICCorrection = 1.;
      float theLaserCorrection = -1.;
      float theICCorrection = -1.;
      
      if ((*rh).first.subdetId()== EcalBarrel)
      {
        EBRecHitCollection::const_iterator itrechit = theBarrelEcalRecHits->find((*rh).first);
        if (itrechit==theBarrelEcalRecHits->end()) continue;
        EBDetId barrelId (itrechit->id ()); 
        ++numRecHit;
                      
        // laser correction
        theLaserCorrection = theLaser->getLaserCorrection(barrelId, iEvent.time());
        if ( applyCorrections_ ) rhLaserCorrection = theLaserCorrection;
        // IC correction
        EcalIntercalibConstantMap::const_iterator ICMapIt = ICMap.find(barrelId);
        theICCorrection = *ICMapIt;
        if ( applyCorrections_ ) rhICCorrection = theICCorrection;
        
        sumRecHitE += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
        sumLaserCorrectionRecHitE += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection;
        // check if rh is inside the 5x5 matrix
        if ( fabs(barrelId.ieta() - ele2_seedIeta) < 3 && fabs(barrelId.iphi() - ele2_seedIphi) < 3 ) {
          sumRecHitE5x5 += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
          sumLaserCorrectionRecHitE5x5 += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection;
        }
        // check if rh is inside the 3x3 matrix
        if ( fabs(barrelId.ieta() - ele2_seedIeta) < 1 && fabs(barrelId.iphi() - ele2_seedIphi) < 1 ) {
          sumRecHitE3x3 += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
          sumLaserCorrectionRecHitE3x3 += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection ;
        }
        // fill recHit variables
        ele2_recHit_E.push_back(itrechit->energy() * rhLaserCorrection);
        ele2_recHit_hashedIndex.push_back(barrelId.hashedIndex());
        ele2_recHit_ietaORix.push_back(barrelId.ieta());
        ele2_recHit_iphiORiy.push_back(barrelId.iphi());
        ele2_recHit_zside.push_back(0);
        ele2_recHit_laserCorrection.push_back(theLaserCorrection);
        ele2_recHit_ICConstant.push_back(theICCorrection);
 
        if( printOut && itrechit->energy() > 1. )
        {
          std::cout << std::fixed
              << "    recHitLC: "    << std::setprecision(6) << std::setw(8) << theLaserCorrection
              << "    recHitIC: "    << std::setprecision(6) << std::setw(8) << theICCorrection
              << std::endl;
        }
      }
        
      if ((*rh).first.subdetId()== EcalEndcap)
      {
        EERecHitCollection::const_iterator itrechit = theEndcapEcalRecHits->find((*rh).first);
        if (itrechit==theEndcapEcalRecHits->end()) continue;
        EEDetId endcapId (itrechit->id ()); 
        ++numRecHit;
          
        // laser correction
        theLaserCorrection = theLaser->getLaserCorrection(endcapId, iEvent.time());
        if ( applyCorrections_ ) rhLaserCorrection = theLaserCorrection;
        // IC correction
        EcalIntercalibConstantMap::const_iterator ICMapIt = ICMap.find(endcapId);
        theICCorrection = *ICMapIt;
        if ( applyCorrections_ ) rhICCorrection = theICCorrection;
        
        sumRecHitE += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
        sumLaserCorrectionRecHitE += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection;
        // check if rh is inside the 5x5 matrix
        if ( fabs(endcapId.ix() - ele2_seedIx) < 3 && fabs(endcapId.iy() - ele2_seedIy) < 3 ) {
          sumRecHitE5x5 += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
          sumLaserCorrectionRecHitE5x5 += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection;
        }
        // check if rh is inside the 3x3 matrix
        if ( fabs(endcapId.ix() - ele2_seedIx) < 1 && fabs(endcapId.iy() - ele2_seedIy) < 1 ) {
          sumRecHitE3x3 += itrechit->energy() * rhLaserCorrection * rhICCorrection ;
          sumLaserCorrectionRecHitE3x3 += theLaserCorrection * itrechit->energy() * rhLaserCorrection * rhICCorrection ;
        }
        // fill recHit variables
        ele2_recHit_E.push_back(itrechit->energy() * rhLaserCorrection);
        ele2_recHit_hashedIndex.push_back(endcapId.hashedIndex());
        ele2_recHit_ietaORix.push_back(endcapId.ix());
        ele2_recHit_iphiORiy.push_back(endcapId.iy());
        ele2_recHit_zside.push_back(endcapId.zside());
        ele2_recHit_laserCorrection.push_back(theLaserCorrection);
        ele2_recHit_ICConstant.push_back(theICCorrection);

        if( printOut && itrechit->energy() > 1. )
        {
          std::cout << std::fixed
              << "    recHitLC: "    << std::setprecision(6) << std::setw(8) << theLaserCorrection
              << "    recHitIC: "    << std::setprecision(6) << std::setw(8) << theICCorrection
              << std::endl;
        }
      }
    }
    
    ele2_scLaserCorr = sumLaserCorrectionRecHitE/sumRecHitE;
    if ( applyCorrections_ ) ele2_scE = scRef->energy()*ele2_fEta*sumRecHitE;
    else ele2_scE = scRef->energy();
    
    ele2_fEtaCorr = fClusterCorrections(sumRecHitE+ele2_es,ele2_scEta,scRef->phiWidth()/scRef->etaWidth(),params)/fClusterCorrections(scRef->rawEnergy()+ele2_es,ele2_scEta,scRef->phiWidth()/scRef->etaWidth(),params);
        
    ele2_5x5LaserCorr = sumLaserCorrectionRecHitE5x5/sumRecHitE5x5;
  
    ele2_3x3LaserCorr = sumLaserCorrectionRecHitE3x3/sumRecHitE3x3;



    /// add regression input variables
    reco::SuperClusterRef s = electron.superCluster();
    reco::CaloClusterPtr b = s->seed(); //seed  basic cluster
    reco::CaloClusterPtr b2;
    reco::CaloClusterPtr bclast;
    reco::CaloClusterPtr bclast2;
    bool isbarrel =  b->hitsAndFractions().at(0).first.subdetId()==EcalBarrel;

     if( isbarrel){

    ele2_eRegrInput_rawE = s->rawEnergy();
    ele2_eRegrInput_r9 = lazyTools.e3x3(*b)/s->rawEnergy();
    ele2_eRegrInput_eta = s->eta();
    ele2_eRegrInput_phi = s->phi();
    ele2_eRegrInput_r25 = lazyTools.e5x5(*b)/s->rawEnergy();
    ele2_eRegrInput_hoe = electron.hcalOverEcal() ;
    ele2_eRegrInput_etaW = s->etaWidth() ;
    ele2_eRegrInput_phiW = s->phiWidth() ;
  
   
    //seed basic cluster variables
    double bemax = lazyTools.eMax(*b);
    double be2nd = lazyTools.e2nd(*b);
    double betop = lazyTools.eTop(*b);
    double bebottom = lazyTools.eBottom(*b);
    double beleft = lazyTools.eLeft(*b);
    double beright = lazyTools.eRight(*b);
 
    ele2_eRegrInput_Deta_bC_sC = b->eta()-s->eta();
    ele2_eRegrInput_Dphi_bC_sC = reco::deltaPhi(b->phi(),s->phi());
    ele2_eRegrInput_bCE_Over_sCE = lazyTools.e3x3(*b)/b->energy();
    ele2_eRegrInput_e3x3_Over_bCE = lazyTools.e3x3(*b)/b->energy();
    ele2_eRegrInput_e5x5_Over_bCE = lazyTools.e5x5(*b)/b->energy();
    ele2_eRegrInput_sigietaieta_bC1 = sqrt(lazyTools.localCovariances(*b)[0]);
    ele2_eRegrInput_sigiphiiphi_bC1 = sqrt(lazyTools.localCovariances(*b)[2]);
    ele2_eRegrInput_sigietaiphi_bC1 = lazyTools.localCovariances(*b)[1];
    ele2_eRegrInput_bEMax_Over_bCE = bemax/b->energy();
    ele2_eRegrInput_log_bE2nd_Over_bEMax = log(be2nd/bemax);
    ele2_eRegrInput_log_bEtop_Over_bEMax = log(betop/bemax);
    ele2_eRegrInput_log_bEbot_Over_bEMax = log(bebottom/bemax);
    ele2_eRegrInput_log_bEleft_Over_bEMax = log(beleft/bemax);
    ele2_eRegrInput_log_bEright_Over_bEMax = log(beright/bemax);
    ele2_eRegrInput_asym_top_bottom = (betop-bebottom)/(betop+bebottom);
    ele2_eRegrInput_asym_left_right = (beleft-beright)/(beleft+beright);

    //highest energy basic cluster excluding seed basic cluster
    double ebcmax = -99.;
    for (reco::CaloCluster_iterator bit = s->clustersBegin(); bit!=s->clustersEnd(); ++bit) {
     const reco::CaloClusterPtr bc = *bit;
     if (bc->energy() > ebcmax && bc !=b) {
       b2 = bc;
       ebcmax = bc->energy();
     }
    }
  
    //lowest energy basic cluster excluding seed (for pileup mitigation)
    double ebcmin = 1e6;
    for (reco::CaloCluster_iterator bit = s->clustersBegin(); bit!=s->clustersEnd(); ++bit) {
     const CaloClusterPtr bc = *bit;
     if (bc->energy() < ebcmin && bc !=b) {
      bclast = bc;
      ebcmin = bc->energy();
     }
    }

   //2nd lowest energy basic cluster excluding seed (for pileup mitigation)
    ebcmin = 1e6;
    for (reco::CaloCluster_iterator bit = s->clustersBegin(); bit!=s->clustersEnd(); ++bit) {
     const CaloClusterPtr bc = *bit;
     if (bc->energy() < ebcmin && bc !=b && bc!=bclast) {
       bclast2 = bc;
       ebcmin = bc->energy();
     }
    }
 
    bool hasbc2 = b2.isNonnull() && b2->energy()>0.;
    bool hasbclast = bclast.isNonnull() && bclast->energy()>0.;
    bool hasbclast2 = bclast2.isNonnull() && bclast2->energy()>0.;
  
    double bc2emax = hasbc2 ? lazyTools.eMax(*b2) : 0.;
    double bc2e2nd = hasbc2 ? lazyTools.e2nd(*b2) : 0.;
    double bc2etop = hasbc2 ? lazyTools.eTop(*b2) : 0.;
    double bc2ebottom = hasbc2 ? lazyTools.eBottom(*b2) : 0.;
    double bc2eleft = hasbc2 ? lazyTools.eLeft(*b2) : 0.;
    double bc2eright = hasbc2 ? lazyTools.eRight(*b2) : 0.;

    ele2_eRegrInput_Deta_bC2_sC = hasbc2 ? (b2->eta()-s->eta()) : 0.;
    ele2_eRegrInput_Dphi_bC2_sC = hasbc2 ? reco::deltaPhi(b2->phi(),s->phi()) : 0.;
    ele2_eRegrInput_bCE2_Over_sCE = hasbc2 ? b2->energy()/s->rawEnergy() : 0.;
    ele2_eRegrInput_e3x3_Over_bCE2 = hasbc2 ? lazyTools.e3x3(*b2)/b2->energy() : 0.;
    ele2_eRegrInput_e5x5_Over_bCE2 = hasbc2 ? lazyTools.e5x5(*b2)/b2->energy() : 0.;
    ele2_eRegrInput_sigietaieta_bC2 = hasbc2 ? sqrt(lazyTools.localCovariances(*b2)[0]) : 0.;
    ele2_eRegrInput_sigiphiiphi_bC2 = hasbc2 ? sqrt(lazyTools.localCovariances(*b2)[2]) : 0.;
    ele2_eRegrInput_sigietaiphi_bC2 = hasbc2 ? lazyTools.localCovariances(*b)[1] : 0.;
    ele2_eRegrInput_bEMax_Over_bCE2 = hasbc2 ? bc2emax/b2->energy() : 0.;
    ele2_eRegrInput_log_bE2nd_Over_bEMax2 = hasbc2 ? log(bc2e2nd/bc2emax) : 0.;
    ele2_eRegrInput_log_bEtop_Over_bEMax2 = hasbc2 ? log(bc2etop/bc2emax) : 0.;
    ele2_eRegrInput_log_bEbot_Over_bEMax2 = hasbc2 ? log(bc2ebottom/bc2emax) : 0.;
    ele2_eRegrInput_log_bEleft_Over_bEMax2 = hasbc2 ? log(bc2eleft/bc2emax) : 0.;
    ele2_eRegrInput_log_bEright_Over_bEMax2 = hasbc2 ? log(bc2eright/bc2emax) : 0.;
    ele2_eRegrInput_asym_top2_bottom2 = hasbc2 ? (bc2etop-bc2ebottom)/(bc2etop+bc2ebottom) : 0.;
    ele2_eRegrInput_asym_left2_right2 = hasbc2 ? (bc2eleft-bc2eright)/(bc2eleft+bc2eright) : 0.;
   

    ele2_eRegrInput_Deta_bCLow_sC = hasbclast ? (bclast->eta()-s->eta()) : 0.;
    ele2_eRegrInput_Dphi_bCLow_sC = hasbclast ? reco::deltaPhi(bclast->phi(),s->phi()) : 0.;
    ele2_eRegrInput_bCELow_Over_sCE = hasbclast ? bclast->energy()/s->rawEnergy() : 0.;
    ele2_eRegrInput_e3x3_Over_bCELow = hasbclast ? lazyTools.e3x3(*bclast)/bclast->energy() : 0.;
    ele2_eRegrInput_e5x5_Over_bCELow = hasbclast ? lazyTools.e5x5(*bclast)/bclast->energy() : 0.;
    ele2_eRegrInput_sigietaieta_bCLow = hasbclast ? sqrt(lazyTools.localCovariances(*bclast)[0]) : 0.;
    ele2_eRegrInput_sigiphiiphi_bCLow = hasbclast ? sqrt(lazyTools.localCovariances(*bclast)[2]) : 0.;
    ele2_eRegrInput_sigietaiphi_bCLow = hasbclast ? lazyTools.localCovariances(*bclast)[1] : 0.;
  
    ele2_eRegrInput_Deta_bCLow2_sC = hasbclast2 ? (bclast2->eta()-s->eta()) : 0.;
    ele2_eRegrInput_Dphi_bCLow2_sC = hasbclast2 ? reco::deltaPhi(bclast2->phi(),s->phi()) : 0.;
    ele2_eRegrInput_bCELow2_Over_sCE = hasbclast2 ? bclast2->energy()/s->rawEnergy() : 0.;
    ele2_eRegrInput_e3x3_Over_bCELow2 = hasbclast2 ? lazyTools.e3x3(*bclast2)/bclast2->energy() : 0.;
    ele2_eRegrInput_e5x5_Over_bCELow2 = hasbclast2 ? lazyTools.e5x5(*bclast2)/bclast2->energy() : 0.;
    ele2_eRegrInput_sigietaieta_bCLow2 = hasbclast2 ? sqrt(lazyTools.localCovariances(*bclast2)[0]) : 0.;
    ele2_eRegrInput_sigiphiiphi_bCLow2 = hasbclast2 ? sqrt(lazyTools.localCovariances(*bclast2)[2]) : 0.;
    ele2_eRegrInput_sigietaiphi_bCLow2 = hasbclast2 ? lazyTools.localCovariances(*bclast2)[1] : 0.;
  
    // seed cluster
    float betacry, bphicry, bthetatilt, bphitilt;
    int bieta, biphi;
    EcalClusterLocal _ecalLocal;
    _ecalLocal.localCoordsEB(*b,iSetup,betacry,bphicry,bieta,biphi,bthetatilt,bphitilt);
    
    ele2_eRegrInput_seedbC_eta = bieta;
    ele2_eRegrInput_seedbC_phi = biphi;
    ele2_eRegrInput_seedbC_eta_p5 = bieta%5;
    ele2_eRegrInput_seedbC_phi_p2 = biphi%2;
    ele2_eRegrInput_seedbC_bieta = (TMath::Abs(bieta)<=25)*(bieta%25) + (TMath::Abs(bieta)>25)*((bieta-25*TMath::Abs(bieta)/bieta)%20);
    ele2_eRegrInput_seedbC_phi_p20 = biphi%20;
    ele2_eRegrInput_seedbC_etacry = betacry;
    ele2_eRegrInput_seedbC_phicry = bphicry;

    //2nd cluster (meaningful gap corrections for converted photons)
    float bc2etacry, bc2phicry, bc2thetatilt, bc2phitilt;
    int bc2ieta, bc2iphi;
    if (hasbc2) _ecalLocal.localCoordsEB(*b2,iSetup,bc2etacry,bc2phicry,bc2ieta,bc2iphi,bc2thetatilt,bc2phitilt);    
  
    ele2_eRegrInput_bC2_eta = hasbc2 ? bc2ieta : 0.;
    ele2_eRegrInput_bC2_phi = hasbc2 ? bc2iphi : 0.;
    ele2_eRegrInput_bC2_eta_p5 = hasbc2 ? bc2ieta%5 : 0.;
    ele2_eRegrInput_bC2_phi_p2 = hasbc2 ? bc2iphi%2 : 0.;
    ele2_eRegrInput_bC2_bieta = hasbc2 ? (TMath::Abs(bc2ieta)<=25)*(bc2ieta%25) + (TMath::Abs(bc2ieta)>25)*((bc2ieta-25*TMath::Abs(bc2ieta)/bc2ieta)%20) : 0.;
    ele2_eRegrInput_bC2_phi_p20 = hasbc2 ? bc2iphi%20 : 0.;
    ele2_eRegrInput_bC2_etacry = hasbc2 ? bc2etacry : 0.;
    ele2_eRegrInput_bC2_phicry = hasbc2 ? bc2phicry : 0.;


    ele2_eRegrInput_nPV = hVertexProduct->size();
  }
  else{
 
        ele2_eRegrInput_rawE = s->rawEnergy();
        ele2_eRegrInput_r9 = lazyTools.e3x3(*b)/s->rawEnergy();
        ele2_eRegrInput_eta = s->eta();
        ele2_eRegrInput_phi = s->phi();
        ele2_eRegrInput_nPV = hVertexProduct->size();
        ele2_eRegrInput_r25 = lazyTools.e5x5(*b)/s->rawEnergy();
        ele2_eRegrInput_etaW = s->etaWidth() ;
        ele2_eRegrInput_phiW = s->phiWidth() ;
  
        ele2_eRegrInput_hoe = -99.;
        ele2_eRegrInput_Deta_bC_sC = -99.;
        ele2_eRegrInput_Dphi_bC_sC = -99.;
        ele2_eRegrInput_bCE_Over_sCE = -99.;
        ele2_eRegrInput_e3x3_Over_bCE = -99.;
        ele2_eRegrInput_e5x5_Over_bCE = -99.;
        ele2_eRegrInput_sigietaieta_bC1 = -99.;
        ele2_eRegrInput_sigiphiiphi_bC1 = -99.;
        ele2_eRegrInput_sigietaiphi_bC1 = -99.;
        ele2_eRegrInput_bEMax_Over_bCE = -99.;
        ele2_eRegrInput_log_bE2nd_Over_bEMax = -99.;
        ele2_eRegrInput_log_bEtop_Over_bEMax = -99.;
        ele2_eRegrInput_log_bEbot_Over_bEMax = -99.;
        ele2_eRegrInput_log_bEleft_Over_bEMax = -99.;
        ele2_eRegrInput_log_bEright_Over_bEMax = -99.;
        ele2_eRegrInput_asym_top_bottom = -99.;
        ele2_eRegrInput_asym_left_right = -99.;
        ele2_eRegrInput_Deta_bC2_sC = -99.;
        ele2_eRegrInput_Dphi_bC2_sC = -99.;
        ele2_eRegrInput_bCE2_Over_sCE = -99.;
        ele2_eRegrInput_e3x3_Over_bCE2 = -99.;
        ele2_eRegrInput_e5x5_Over_bCE2 = -99.;
        ele2_eRegrInput_sigietaieta_bC2 = -99.;
        ele2_eRegrInput_sigiphiiphi_bC2 = -99.;
        ele2_eRegrInput_sigietaiphi_bC2 = -99.;
        ele2_eRegrInput_bEMax_Over_bCE2 = -99.;
        ele2_eRegrInput_log_bE2nd_Over_bEMax2 = -99.;
        ele2_eRegrInput_log_bEtop_Over_bEMax2 = -99.;
        ele2_eRegrInput_log_bEbot_Over_bEMax2 = -99.;
        ele2_eRegrInput_log_bEleft_Over_bEMax2 = -99.;
        ele2_eRegrInput_log_bEright_Over_bEMax2 = -99.;
        ele2_eRegrInput_asym_top2_bottom2 = -99.;
        ele2_eRegrInput_asym_left2_right2 = -99.;
        ele2_eRegrInput_Deta_bCLow_sC = -99.;
        ele2_eRegrInput_Dphi_bCLow_sC = -99.;
        ele2_eRegrInput_bCELow_Over_sCE = -99.;
        ele2_eRegrInput_e3x3_Over_bCELow = -99.;
        ele2_eRegrInput_e5x5_Over_bCELow = -99.;
        ele2_eRegrInput_sigietaieta_bCLow = -99.;
        ele2_eRegrInput_sigiphiiphi_bCLow = -99.;
        ele2_eRegrInput_sigietaiphi_bCLow = -99.;  
        ele2_eRegrInput_Deta_bCLow2_sC = -99.;
        ele2_eRegrInput_Dphi_bCLow2_sC = -99.;
        ele2_eRegrInput_bCELow2_Over_sCE = -99.;
        ele2_eRegrInput_e3x3_Over_bCELow2 = -99.;
        ele2_eRegrInput_e5x5_Over_bCELow2 = -99.;
        ele2_eRegrInput_sigietaieta_bCLow2 = -99.;
        ele2_eRegrInput_sigiphiiphi_bCLow2 = -99.;
        ele2_eRegrInput_sigietaiphi_bCLow2 = -99.;
        ele2_eRegrInput_seedbC_eta = -99.;
        ele2_eRegrInput_seedbC_phi = -99.;
        ele2_eRegrInput_seedbC_eta_p5 = -99.;
        ele2_eRegrInput_seedbC_phi_p2 = -99.;
        ele2_eRegrInput_seedbC_bieta = -99.;
        ele2_eRegrInput_seedbC_phi_p20 = -99.;
        ele2_eRegrInput_seedbC_etacry = -99.;
        ele2_eRegrInput_seedbC_phicry = -99.;

        ele2_eRegrInput_bC2_eta = -99.;
        ele2_eRegrInput_bC2_phi = -99.;
        ele2_eRegrInput_bC2_eta_p5 = -99.;
        ele2_eRegrInput_bC2_phi_p2 = -99.;
        ele2_eRegrInput_bC2_bieta = -99.;
        ele2_eRegrInput_bC2_phi_p20 = -99.;
        ele2_eRegrInput_bC2_etacry = -99.;
        ele2_eRegrInput_bC2_phicry =-99.; 
  }
  }

}

// -----------------------------------------------------------------------------------------

 void SimpleNtupleEoverP::fillMetInfo(const edm::Event & iEvent, const edm::EventSetup & iSetup){
 //std::cout << "SimpleNtupleCalib::fillPFMetInfo" << std::endl;
  
 //*********** MET
  edm::Handle<edm::View<reco::MET> > PFmetHandle;
  iEvent.getByLabel(PFMetTag_,PFmetHandle);
  View<reco::MET>  mets = *PFmetHandle;
  reco::MET MET = mets.at(0);

  ROOT::Math::XYZTVector met = MET.p4();
 
  // correct phi Met in data and MC
  float cx0,cx1;
  float cy0,cy1;

  if( dataFlag_ == true){

      if( dataRun_ == "2011A" ){
        cx1 = +0.004801; cx0 = -0.3365;
        cy1 = -0.006124; cy0 = +0.2578;
      }
      if(dataRun_ == "2011B" ){
        cx1 = +0.005162; cx0 = -0.3265;
        cy1 = -0.006299; cy0 = -0.1956;
      }
    }
  else{

      if( dataRun_ == "2011A" ){
        cx1 = +0.0001815; cx0 = -0.09389;
        cy1 = -0.003710;  cy0 = +0.1571;      
      }
      if( dataRun_ == "2011B" ){
        cx1 = +0.00009587; cx0 = -0.1070;
        cy1 = -0.003357;   cy0 = +0.01517;
      }
  }

  float metx = met.px();
  float mety = met.py();

  metx -= (cx0 + cx1*PV_n);
  mety -= (cy0 + cy1*PV_n);

  met.SetPxPyPzE(metx,mety,0,sqrt(metx*metx+mety*mety));

  p_met = &met;
  met_et = p_met->Et();
  met_phi = p_met->phi();
  
  ele1Met_mt = sqrt( 2. * ele1_pt * met_et * ( 1. - cos( deltaPhi(ele1_phi,met_phi) ) ) );
  ele1Met_Dphi = deltaPhi(ele1_phi,met_phi);




}

//----------------------------------------------------------------------------------------------

void SimpleNtupleEoverP::fillDoubleEleInfo(const edm::Event & iEvent, const edm::EventSetup & iSetup){

  ele1ele2_m = (ele1 + ele2).mass();
  
  ROOT::Math::PtEtaPhiEVector ele1_sc(ele1_scE*sin(2*atan(exp(-1.*ele1_eta))),ele1_eta,ele1_phi,ele1_scE);
  ROOT::Math::PtEtaPhiEVector ele2_sc(ele2_scE*sin(2*atan(exp(-1.*ele2_eta))),ele2_eta,ele2_phi,ele2_scE);
  ele1ele2_scM = (ele1_sc + ele2_sc).mass();

  ROOT::Math::PtEtaPhiEVector ele1_sc_regression(ele1_scE_regression*sin(2*atan(exp(-1.*ele1_eta))),ele1_eta,ele1_phi,ele1_scE_regression);
  ROOT::Math::PtEtaPhiEVector ele2_sc_regression(ele2_scE_regression*sin(2*atan(exp(-1.*ele2_eta))),ele2_eta,ele2_phi,ele2_scE_regression);
  ele1ele2_scM_regression = (ele1_sc_regression + ele2_sc_regression).mass();

}


double SimpleNtupleEoverP::deltaPhi(const double& phi1, const double& phi2){ 
  double deltaphi = fabs(phi1 - phi2);
  if (deltaphi > 3.141592654) deltaphi = 6.283185308 - deltaphi;
  return deltaphi;
}

// -----------------------------------------------------------------------------------------
void SimpleNtupleEoverP::fillMCPUInfo (const edm::Event & iEvent, const edm::EventSetup & iSetup) 
{
 //std::cout << "SimpleNtupleCalib::fillMCPUInfo" << std::endl;

  edm::Handle<std::vector<PileupSummaryInfo> > PupInfo;
  iEvent.getByLabel(MCPileupTag_, PupInfo);
  
  
  // loop on BX
   // loop on BX
  std::vector<PileupSummaryInfo>::const_iterator PVI;
  for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI)
  {
    std::vector<float> temp_mc_PU_zpositions   = PVI->getPU_zpositions();
    std::vector<float> temp_mc_PU_sumpT_lowpT  = PVI->getPU_sumpT_lowpT();
    std::vector<float> temp_mc_PU_sumpT_highpT = PVI->getPU_sumpT_highpT();
    std::vector<int> temp_mc_PU_ntrks_lowpT    = PVI->getPU_ntrks_lowpT();
    std::vector<int> temp_mc_PU_ntrks_highpT   = PVI->getPU_ntrks_highpT();
    
    // in-time pileup
    if( PVI->getBunchCrossing() == 0 )
    {
      PUit_TrueNumInteractions = PVI->getTrueNumInteractions();
      PUit_NumInteractions = PVI->getPU_NumInteractions();    
      
      for(std::vector<float>::const_iterator it = temp_mc_PU_zpositions.begin(); it < temp_mc_PU_zpositions.end(); ++it)
        PUit_zpositions = *it;
      
      for(std::vector<float>::const_iterator it = temp_mc_PU_sumpT_lowpT.begin(); it < temp_mc_PU_sumpT_lowpT.end(); ++it)
        PUit_sumpT_lowpT=*it;
      
      for(std::vector<float>::const_iterator it = temp_mc_PU_sumpT_highpT.begin(); it < temp_mc_PU_sumpT_highpT.end(); ++it)
        PUit_sumpT_highpT = *it;
      
      for(std::vector<int>::const_iterator it = temp_mc_PU_ntrks_lowpT.begin(); it < temp_mc_PU_ntrks_lowpT.end(); ++it)
        PUit_ntrks_lowpT = *it;
      
      for(std::vector<int>::const_iterator it = temp_mc_PU_ntrks_highpT.begin(); it < temp_mc_PU_ntrks_highpT.end(); ++it)
        PUit_ntrks_highpT = *it;
    }
    
    // out-of-time pileup
    else
    {
      if (PVI->getBunchCrossing() < 0)
      {
        PUoot_early_TrueNumInteractions = PVI->getTrueNumInteractions();
        PUoot_early = PVI->getPU_NumInteractions();
        
        for(std::vector<float>::const_iterator it = temp_mc_PU_zpositions.begin(); it < temp_mc_PU_zpositions.end(); ++it)
          PUoot_early_zpositions = *it;
        
        for(std::vector<float>::const_iterator it = temp_mc_PU_sumpT_lowpT.begin(); it < temp_mc_PU_sumpT_lowpT.end(); ++it)
          PUoot_early_sumpT_lowpT = *it;
        
        for(std::vector<float>::const_iterator it = temp_mc_PU_sumpT_highpT.begin(); it < temp_mc_PU_sumpT_highpT.end(); ++it)
          PUoot_early_sumpT_highpT = *it;
        
        for(std::vector<int>::const_iterator it = temp_mc_PU_ntrks_lowpT.begin(); it < temp_mc_PU_ntrks_lowpT.end(); ++it)
          PUoot_early_ntrks_lowpT = *it;
        
        for(std::vector<int>::const_iterator it = temp_mc_PU_ntrks_highpT.begin(); it < temp_mc_PU_ntrks_highpT.end(); ++it)
          PUoot_early_ntrks_highpT = *it;
      }
      else
      {
        PUoot_late_TrueNumInteractions = PVI->getTrueNumInteractions();
        PUoot_late = PVI->getPU_NumInteractions();
        
        for(std::vector<float>::const_iterator it = temp_mc_PU_zpositions.begin(); it < temp_mc_PU_zpositions.end(); ++it)
          PUoot_late_zpositions = *it;
        
        for(std::vector<float>::const_iterator it = temp_mc_PU_sumpT_lowpT.begin(); it < temp_mc_PU_sumpT_lowpT.end(); ++it)
          PUoot_late_sumpT_lowpT = *it;
        
        for(std::vector<float>::const_iterator it = temp_mc_PU_sumpT_highpT.begin(); it < temp_mc_PU_sumpT_highpT.end(); ++it)
          PUoot_late_sumpT_highpT = *it;
        
        for(std::vector<int>::const_iterator it = temp_mc_PU_ntrks_lowpT.begin(); it < temp_mc_PU_ntrks_lowpT.end(); ++it)
          PUoot_late_ntrks_lowpT = *it;
        
        for(std::vector<int>::const_iterator it = temp_mc_PU_ntrks_highpT.begin(); it < temp_mc_PU_ntrks_highpT.end(); ++it)
          PUoot_late_ntrks_highpT = *it;          
      }
    }
  }
}
//----------------------------------------------------------------------------------------------
DEFINE_FWK_MODULE(SimpleNtupleEoverP);
