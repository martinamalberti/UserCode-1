import FWCore.ParameterSet.Config as cms

process = cms.Process("AlCaElectronsTest")
process.load("FWCore.MessageLogger.MessageLogger_cfi")

#process.load("Configuration.StandardSequences.Geometry_cff")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.options = cms.untracked.PSet(
        wantSummary = cms.untracked.bool(True)
        )

readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)

secFiles.extend( (
                   ) )


process.alCaElectronsTest = cms.EDAnalyzer("AlCaElectronsTest",
  electronLabel=cms.InputTag("gsfElectrons"),
  alcaBarrelHitCollection=cms.InputTag("alCaIsolatedElectrons:alcaBarrelHits"),
  alcaEndcapHitCollection=cms.InputTag("alCaIsolatedElectrons:alcaEndcapHits"),
  HistOutFile = cms.untracked.string("NAMEOUTPUT"),
  EcalIsoTag = cms.InputTag("egammaEcalIsolation"),
  ElePtTkIsoTag = cms.InputTag("egammaElectronSqPtTkIsolation"),
  EleTkIsoTag = cms.InputTag("egammaElectronTkIsolation"),
  HoETag = cms.InputTag("egammaHOE"),
  HCalIsoTag = cms.InputTag("egammaHcalIsolation"),
  METTag = cms.InputTag("met")
  )

process.pathALCARECOEcalCalElectron = cms.Path(process.alCaElectronsTest)


