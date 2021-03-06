import FWCore.ParameterSet.Config as cms

process = cms.Process("SelectionsTest")
process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.options = cms.untracked.PSet(
        wantSummary = cms.untracked.bool(True)
        )

readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)

readFiles.extend( [
#	'/store/relval/CMSSW_3_1_0_pre8/RelValZEE/ALCARECO/STARTUP_31X_StreamALCARECOEcalCalElectron_v1/0006/C2CF2E13-DB4D-DE11-A5B1-001D09F2437B.root'
     '/store/relval/CMSSW_3_5_4/RelValZEE/ALCARECO/START3X_V24_EcalCalElectron-v1/0004/2AB6B40D-2D2C-DF11-814B-002618943948.root'	
        ] )


process.alCaRecHitsTest = cms.EDAnalyzer("AlCaRecHitsTest",
  electronLabel        = cms.InputTag ("gsfElectrons") ,
  alcaBarrelRecHits    = cms.InputTag ("alCaIsolatedElectrons:alcaBarrelHits") ,
  alcaEndcapRecHits    = cms.InputTag ("alCaIsolatedElectrons:alcaEndcapHits") ,
  alcaPreshowerRecHits = cms.InputTag ("alCaIsolatedElectrons:alcaPreshowerHits") ,
  HistOutFile          = cms.untracked.string ("alCaRecHitsTest.root")
  )

process.path = cms.Path(process.alCaRecHitsTest)


