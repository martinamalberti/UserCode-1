import FWCore.ParameterSet.Config as cms

simpleNtupleEoverP = cms.EDAnalyzer(
    "SimpleNtupleEoverP",
    PVTag               = cms.InputTag("offlinePrimaryVerticesWithBS"),
    recHitCollection_EB = cms.InputTag("alCaIsolatedElectrons","alcaBarrelHits","ALCARERECO"),
    recHitCollection_EE = cms.InputTag("alCaIsolatedElectrons","alcaEndcapHits","ALCARERECO"),
    EleTag              = cms.InputTag("electronRecalibSCAssociator","","ALCARERECO"),
    PFMetTag          = cms.InputTag("pfMet"),
    rhoTag          = cms.InputTag("kt6PFJetsForRhoCorrection","rho"),
    verbosity    = cms.untracked.bool(True),
    doTighterSel    = cms.untracked.bool(True),
    applyCorrections    = cms.untracked.bool(False)
    )
