import FWCore.ParameterSet.Config as cms

from PhysicsTools.MiBiCommonPAT.makeMiBiCommonNT_cff import *

process = cms.Process("MiBiCommonNT")

# the MiBiPAT
makeMiBiCommonNT(process, GlobalTag="START38_V12::All", HLT='REDIGI38X', MC=True)
  
process.source.fileNames = cms.untracked.vstring(
  'file:/tmp/dimatteo/Fall10_WToENu_TuneZ2_7TeV-pythia6_AODSIM_START38_V12-v1.root'
  #'/store/data/Run2010B/Electron/RECO/PromptReco-v2/000/147/451/FA4BFB16-7AD3-DF11-AD7C-0019B9F72BFF.root',
  #'/store/data/Run2010B/Electron/RECO/PromptReco-v2/000/147/451/F023A43C-83D3-DF11-9EC7-001D09F24047.root',
  #'/store/data/Run2010B/Electron/RECO/PromptReco-v2/000/147/451/E6DEB3D2-81D3-DF11-82A4-001D09F297EF.root',
  #'/store/data/Run2010B/Electron/RECO/PromptReco-v2/000/147/451/E4EE523D-83D3-DF11-9564-001D09F28F1B.root'

  #'file:/gwpool/users/amassiro/VBF/Releases/CMSSW_3_8_3/src/PhysicsTools/MiBiCommonPAT/669A4128-43D0-DF11-AE93-001A92810ACE.root'  

)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False))

process.TFileService = cms.Service(
    "TFileService", 
    fileName = cms.string("MiBiCommonNT.root"),
    closeFileFast = cms.untracked.bool(True)
)

