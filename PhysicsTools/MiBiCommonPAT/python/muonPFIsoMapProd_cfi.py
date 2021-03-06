import FWCore.ParameterSet.Config as cms

muonPFIsoMapProd = cms.EDProducer('MuonPFIsoMapProd',
    muonLabel = cms.untracked.InputTag("muons"),
    vtxLabel = cms.untracked.InputTag("offlinePrimaryVertices"),
    pfLabel = cms.untracked.InputTag("particleFlow"),
)
