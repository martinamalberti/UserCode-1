import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.GeometryExtended_cff import *

SimpleNtupleGen = cms.EDAnalyzer(
    "SimpleNtupleGen",
    
    #-------------------
    #### Input Tags ####
    
    GenParticlesTag = cms.InputTag("genParticles"),
    GenJetTag       = cms.InputTag("ak5GenJets"),
    GenMetTag       = cms.InputTag("genMetTrue"),
    
    
    #--------------
    #### flags ####
    saveMCPtHat    = cms.untracked.bool(True),
    saveGenEle     = cms.untracked.bool(True),
    saveGenMu      = cms.untracked.bool(True),
    saveGenTau     = cms.untracked.bool(True),
    saveGenTauJ    = cms.untracked.bool(True),
    saveGenMet     = cms.untracked.bool(True),
    saveGenJet     = cms.untracked.bool(True),
    saveMCTTBar    = cms.untracked.bool(False),    
    saveMCHiggs    = cms.untracked.bool(False),
    saveMCHiggsWW  = cms.untracked.bool(False),
    
    verbosity = cms.untracked.bool(False)
)
