import FWCore.ParameterSet.Config as cms

from PhysicsTools.MiBiCommonPAT.makeMiBiCommonNT_cff import *

process = cms.Process("MiBiCommonNT")

# the MiBiNT
makeMiBiCommonNT(process, GlobalTag="GR_R_39X_V5::All", HLT='HLT', MC=False)
  
process.source.fileNames = cms.untracked.vstring(
  #'file:/gwtera5/users/data/NTUPLES/VBF/WENu_38X_AOD.root'
  #'/store/relval/CMSSW_3_8_4/RelValZEE/GEN-SIM-RECO/START38_V12-v1/0025/CA5F22F2-9AC2-DF11-9CC4-00304867915A.root'
  #'file:/gwtera5/users/data/NTUPLES/VBF/WENu_38X_AOD.root'
  #'/store/data/Run2010B/Electron/RECO/PromptReco-v2/000/147/451/FA4BFB16-7AD3-DF11-AD7C-0019B9F72BFF.root',
  #'/store/data/Run2010B/Electron/RECO/PromptReco-v2/000/147/451/F023A43C-83D3-DF11-9EC7-001D09F24047.root',
  #'/store/data/Run2010B/Electron/RECO/PromptReco-v2/000/147/451/E6DEB3D2-81D3-DF11-82A4-001D09F297EF.root',
  #'/store/data/Run2010B/Electron/RECO/PromptReco-v2/000/147/451/E4EE523D-83D3-DF11-9564-001D09F28F1B.root'
  #'file:/gwpool/users/amassiro/VBF/Releases/CMSSW_3_8_3/src/PhysicsTools/MiBiCommonPAT/669A4128-43D0-DF11-AE93-001A92810ACE.root'

    'file:/tmp/deguio/step2_RAW2DIGI_L1Reco_RECO_9_1_Ob9.root'

)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(2000) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))

process.TFileService = cms.Service(
    "TFileService", 
    fileName = cms.string("MiBiCommonNT.root"),
    closeFileFast = cms.untracked.bool(True)
)

process.output = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *'),
    fileName = cms.untracked.string('/tmp/ghezzi/AAA.root'),
        dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-RAW'),
#        filterName = cms.untracked.string('ecalProblematicFEDProducer')
    )
)

#process.out_step = cms.EndPath(process.output)
