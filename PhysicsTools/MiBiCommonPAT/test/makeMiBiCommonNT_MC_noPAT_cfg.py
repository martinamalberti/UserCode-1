import FWCore.ParameterSet.Config as cms

from PhysicsTools.MiBiCommonPAT.makeMiBiCommonNT_noPAT_cff import *

process = cms.Process("MiBiCommonNT")

# the MiBiNT: Choose the MC type!!!
# Possible choices: TTBar, Higgs, HiggsWW, HiggsGammaGamma, ZW, Other
makeMiBiCommonNT_noPAT(process, GlobalTag="START42_V14B::All", HLT='', MC=True, MCType='Other')

process.source.fileNames = cms.untracked.vstring(
     'file:/data1/govoni/DY_PAT.root'
#     'file:/media/DATA/WJetFall11_Example.root'
#    'file:/data1/govoni/fastSimTest/test.root'
#    'file:/gwteraz/users/benaglia/pat_42x_fall11_SQWaT_PAT_42X_WJetsToLNu_TuneZ2_7TeV-madgraph-tauola_v1.root'
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))

#process.SimpleNtuple_noPAT.saveSC = cms.untracked.bool (True)

process.TFileService = cms.Service(
    "TFileService", 
    fileName = cms.string("MiBiCommonNT.root"),
    closeFileFast = cms.untracked.bool(True)
)
