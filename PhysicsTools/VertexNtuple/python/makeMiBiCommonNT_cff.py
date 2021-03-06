import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.tools.metTools import *
from PhysicsTools.PatAlgos.tools.jetTools import *
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.pfTools import *

from PhysicsTools.PatAlgos.selectionLayer1.leptonCountFilter_cfi import *
from PhysicsTools.PatAlgos.selectionLayer1.photonCountFilter_cfi import *
from PhysicsTools.PatAlgos.selectionLayer1.jetCountFilter_cfi import *



def makeMiBiCommonNT(process, GlobalTag, HLT='HLT', MC=False, MCType='Other'):

    # Setup the process
    process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
    process.load("FWCore.MessageService.MessageLogger_cfi")
    process.MessageLogger.cerr.FwkReport.reportEvery = 1000
    process.load('Configuration.StandardSequences.GeometryExtended_cff')
    process.load("Configuration.StandardSequences.MagneticField_cff")
    process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
    process.GlobalTag.globaltag = GlobalTag

    
    # Source
    process.source = cms.Source(
        "PoolSource",
        fileNames = cms.untracked.vstring()
    )
    
    # Out
    process.out = cms.OutputModule(
      "PoolOutputModule",
      fileName = cms.untracked.string('file:./MiBiCommonPAT.root'),
      outputCommands = cms.untracked.vstring()
    )


    process.load("PhysicsTools.NtupleUtils.AllPassFilter_cfi")

    #--------------------------
    # Counter1: All read events
    process.AllEvents = process.AllPassFilter.clone()
    


    #--------------------------
    # Counter2: Scraping filter
    process.scrapingFilter = cms.EDFilter("FilterOutScraping",
        applyfilter = cms.untracked.bool(True),
        debugOn = cms.untracked.bool(False),
        numtrack = cms.untracked.uint32(10),
        thresh = cms.untracked.double(0.25)
    )
    
    process.NonScrapedEvents = process.AllPassFilter.clone()
    
    #-----------------------------------
    # Counter3: Filter on primary vertex
    process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
        vertexCollection = cms.InputTag('offlinePrimaryVertices'),
        minimumNDOF = cms.uint32(4) ,
        maxAbsZ = cms.double(24),
        maxd0 = cms.double(2)
    )
    
    process.GoodVtxEvents = process.AllPassFilter.clone()
    
    #------------------
    #Load PAT sequences
    process.load("PhysicsTools.PatAlgos.patSequences_cff")
    process.load("PhysicsTools.PatAlgos.tools.pfTools")
    postfix = "PFlow"
    usePF2PAT(process, runPF2PAT=True, jetAlgo='AK5', runOnMC=MC, postfix=postfix)  
    
    if not MC:
     removeMCMatching(process, ['All'])
    
    process.patJets.addTagInfos = cms.bool(False)    #bugfix related to btagging
    
    process.load("PhysicsTools.MiBiCommonPAT.simpleEleIdSequence_cff")
    
    process.patElectrons.addElectronID = cms.bool(True)
    process.patElectrons.electronIDSources = cms.PSet(
      simpleEleId95relIso= cms.InputTag("simpleEleId95relIso"),
      simpleEleId90relIso= cms.InputTag("simpleEleId90relIso"),
      simpleEleId85relIso= cms.InputTag("simpleEleId85relIso"),
      simpleEleId80relIso= cms.InputTag("simpleEleId80relIso"),
      simpleEleId70relIso= cms.InputTag("simpleEleId70relIso"),
      simpleEleId60relIso= cms.InputTag("simpleEleId60relIso"),
      simpleEleId95cIso= cms.InputTag("simpleEleId95cIso"),
      simpleEleId90cIso= cms.InputTag("simpleEleId90cIso"),
      simpleEleId85cIso= cms.InputTag("simpleEleId85cIso"),
      simpleEleId80cIso= cms.InputTag("simpleEleId80cIso"),
      simpleEleId70cIso= cms.InputTag("simpleEleId70cIso"),
      simpleEleId60cIso= cms.InputTag("simpleEleId60cIso"),
    )

    process.patElectronsPFlow.addElectronID = cms.bool(True)
    process.patElectronsPFlow.electronIDSources = cms.PSet(
      simpleEleId95relIso= cms.InputTag("simpleEleId95relIso"),
      simpleEleId90relIso= cms.InputTag("simpleEleId90relIso"),
      simpleEleId85relIso= cms.InputTag("simpleEleId85relIso"),
      simpleEleId80relIso= cms.InputTag("simpleEleId80relIso"),
      simpleEleId70relIso= cms.InputTag("simpleEleId70relIso"),
      simpleEleId60relIso= cms.InputTag("simpleEleId60relIso"),
      simpleEleId95cIso= cms.InputTag("simpleEleId95cIso"),
      simpleEleId90cIso= cms.InputTag("simpleEleId90cIso"),
      simpleEleId85cIso= cms.InputTag("simpleEleId85cIso"),
      simpleEleId80cIso= cms.InputTag("simpleEleId80cIso"),
      simpleEleId70cIso= cms.InputTag("simpleEleId70cIso"),
      simpleEleId60cIso= cms.InputTag("simpleEleId60cIso"),
    )
    
    process.patElectronIDs = cms.Sequence(process.simpleEleIdSequence)
    process.makePatElectrons = cms.Sequence(
      process.patElectronIDs*
      process.patElectronIsolation*
      process.electronMatch*
      process.patElectrons
    )
    
    process.patElectronIDsPFlow = cms.Sequence(process.simpleEleIdSequence)
    process.makePatElectronsPFlow = cms.Sequence(
      process.patElectronIDsPFlow*
      process.patElectronIsolation*
      process.electronMatchPFlow*
      process.patElectronsPFlow
    )
     
    if not MC:
        process.makePatElectrons.remove(process.electronMatch)

    if not MC:
        process.makePatElectronsPFlow.remove(process.electronMatchPFlow)


    
    # ---------------
    # add collections
    addTcMET(process, 'TC')
    addPfMET(process, 'PF')
    
    
    if not MC:
        addJetCollection(
            process,
            cms.InputTag('ak5CaloJets'),
            'AK5',
            'Calo',
            doJTA        = True,
            doBTagging   = True,
            jetCorrLabel = ('AK5Calo', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
            doType1MET   = True,
            doL1Cleaning = True,
            doL1Counters = False,
            genJetCollection=cms.InputTag("ak5GenJets"),
            doJetID      = True,
            jetIdLabel   = "ak5"
            )
            
        addJetCollection(
            process,
            cms.InputTag('ak5PFJets'),
            'AK5',
            'PF',
            doJTA        = True,
            doBTagging   = True,
            jetCorrLabel = ('AK5PF', cms.vstring(['L2Relative', 'L3Absolute', 'L2L3Residual'])),
            doType1MET   = True,
            doL1Cleaning = True,
            doL1Counters = False,
            genJetCollection=cms.InputTag("ak5GenJets"),
            doJetID      = True,
            jetIdLabel   = "ak5"
            )

    
    if MC:
        addJetCollection(
            process,
            cms.InputTag('ak5CaloJets'),
            'AK5',
            'Calo',
            doJTA        = True,
            doBTagging   = True,
            jetCorrLabel = ('AK5Calo', cms.vstring(['L2Relative', 'L3Absolute'])),
            doType1MET   = True,
            doL1Cleaning = True,
            doL1Counters = False,
            genJetCollection=cms.InputTag("ak5GenJets"),
            doJetID      = True,
            jetIdLabel   = "ak5"
            )
            
        addJetCollection(
            process,
            cms.InputTag('ak5PFJets'),
            'AK5',
            'PF',
            doJTA        = True,
            doBTagging   = True,
            jetCorrLabel = ('AK5PF', cms.vstring(['L2Relative', 'L3Absolute'])),
            doType1MET   = True,
            doL1Cleaning = True,
            doL1Counters = False,
            genJetCollection=cms.InputTag("ak5GenJets"),
            doJetID      = True,
            jetIdLabel   = "ak5"
            )        
            

    #---------
    #PG PF2PAT sequence from /CMSSW/PhysicsTools/PatAlgos/test/patTuple_PATandPF2PAT_cfg.py
    # An empty postfix means that only PF2PAT is run,
    # otherwise both standard PAT and PF2PAT are run. In the latter case PF2PAT
    # collections have standard names + postfix (e.g. patElectronPFlow)  
    
    
    
    # -------------------
    # pat selection layer
    process.selectedPatElectrons.cut      = cms.string("pt > 20. & abs(eta) < 2.5")
    process.selectedPatElectronsPFlow.cut = cms.string("pt > 20. & abs(eta) < 2.5")    

    process.selectedPatMuons.cut      = cms.string("pt > 20. & abs(eta) < 2.5")
    process.selectedPatMuonsPFlow.cut = cms.string("pt > 20. & abs(eta) < 2.5")

    process.selectedPatJets.cut        = cms.string("pt > 15. & abs(eta) < 5")
    process.selectedPatJetsPFlow.cut   = cms.string("pt > 15. & abs(eta) < 5")    
    process.selectedPatJetsAK5Calo.cut = cms.string("pt > 15. & abs(eta) < 5")
    process.selectedPatJetsAK5PF.cut   = cms.string("pt > 15. & abs(eta) < 5")

    process.selectedPatPhotons.cut      = cms.string("pt > 10. & abs(eta) < 5")
    process.selectedPatPhotonsPFlow.cut = cms.string("pt > 10. & abs(eta) < 5")    
    
    # the HCAL Noise Filter
    #process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')

    process.makeElectronLessVertices = cms.EDProducer ("ElectronLessVertexReProducer",
        EleTag               = cms.InputTag("gsfElectrons"),
        PVTag                = cms.InputTag("offlinePrimaryVertices"),
        MatchingConeTreshold = cms.double(0.05),
        ElectronPtCut        = cms.double(1000000.),
        MuonPtCut        = cms.double(1000000.)                                        
        )
    
    # the MiBiPAT path
    process.MiBiCommonPAT = cms.Sequence(
        process.AllEvents * # -> Counter
        process.scrapingFilter *
        process.NonScrapedEvents * # -> Counter
        process.primaryVertexFilter *
        process.GoodVtxEvents * # -> Counter
        getattr(process,"patPF2PATSequence"+postfix) *
        process.patDefaultSequence 
#        process.makeElectronLessVertices
    )
    

    
    #----------------------
    # Lepton/Photon Filters
    process.load('PhysicsTools.PatAlgos.selectionLayer1.leptonCountFilter_cfi')
    process.LeptonsFilter = countPatLeptons.clone(
      electronSource = cms.InputTag("selectedPatElectrons"),
      muonSource     = cms.InputTag("selectedPatMuons"),
      minNumber      = cms.uint32(1)
     )

    
    process.LeptonsFilterPFlow = countPatLeptons.clone(
      electronSource = cms.InputTag("selectedPatElectronsPFlow"),
      muonSource     = cms.InputTag("selectedPatMuonsPFlow"),
      minNumber      = cms.uint32(1)
     )
    
    
    
    process.load('PhysicsTools.PatAlgos.selectionLayer1.photonCountFilter_cfi')
    process.PhotonsFilter = countPatPhotons.clone(
      src       = cms.InputTag("selectedPatPhotons"),
      minNumber = cms.uint32(2)
     )

    
    
    #------------
    # Jet Filters
    process.load('PhysicsTools.PatAlgos.selectionLayer1.jetCountFilter_cfi')
    process.JetFilterAK5Calo = countPatJets.clone(
      src = cms.InputTag("selectedPatJetsAK5Calo"),
      minNumber      = cms.uint32(2)
    )
    
    
    process.JetFilterAK5PF = countPatJets.clone(
      src = cms.InputTag("selectedPatJetsAK5PF"),
      minNumber      = cms.uint32(2)
    )
    
     
    process.JetFilterPFlow = countPatJets.clone(
      src = cms.InputTag("selectedPatJetsPFlow"),
      minNumber      = cms.uint32(2)
    )
    
    
    
    #-----------
    # Sequences & Other counters
    process.LeptonsFilterEvents = process.AllPassFilter.clone()
    process.LeptonsFilterPFlowEvents = process.AllPassFilter.clone()
    process.JetFilterAK5CaloEvents = process.AllPassFilter.clone()
    process.JetFilterAK5PFEvents = process.AllPassFilter.clone()
    process.JetFilterPFlowEvents = process.AllPassFilter.clone()
    process.PhotonsFilterEvents = process.AllPassFilter.clone()
    
    process.OneLeptonTwoJetsAK5CaloSeq = cms.Sequence(
        process.LeptonsFilter*
        process.LeptonsFilterEvents*
        process.JetFilterAK5Calo*
        process.JetFilterAK5CaloEvents
       )

    
    process.OneLeptonTwoJetsAK5PFSeq = cms.Sequence(
        process.LeptonsFilter*
        process.LeptonsFilterEvents*
        process.JetFilterAK5PF*
        process.JetFilterAK5PFEvents
        )
    
    
    process.OneLeptonTwoJetsPFlowSeq = cms.Sequence(
        process.LeptonsFilterPFlow*
        process.LeptonsFilterPFlowEvents*
        process.JetFilterPFlow*
        process.JetFilterPFlowEvents
        )

    process.TwoPhotonsSeq = cms.Sequence(
        #process.PhotonsFilter*  #fede
        process.PhotonsFilterEvents
        )
        
        
    # the MiBiNTUPLE
    process.load("PhysicsTools.MiBiCommonPAT.SimpleNtuple_cfi")
    process.MiBiCommonNT = process.SimpleNtuple.clone()
    process.MiBiCommonNT.TriggerResultTag      = cms.InputTag("TriggerResults","",HLT)
    process.MiBiCommonNT.saveGenJet            = cms.untracked.bool (MC)
    process.MiBiCommonNT.saveMCPtHat           = cms.untracked.bool (MC)
    process.MiBiCommonNT.savePUSummaryInfo     = cms.untracked.bool (MC)
    
    process.MiBiCommonNT.saveMCTTBar           = cms.untracked.bool (False)    
    process.MiBiCommonNT.saveMCHiggs           = cms.untracked.bool (False)
    process.MiBiCommonNT.saveMCHiggsWW         = cms.untracked.bool (False)
    process.MiBiCommonNT.saveMCHiggsGammaGamma = cms.untracked.bool (False)
    process.MiBiCommonNT.EleLessPVTag          = cms.InputTag("makeElectronLessVertices")
    process.MiBiCommonNT.verbosity             = cms.untracked.bool (False)
    
    if MCType == 'TTBar':
        process.MiBiCommonNT.saveMCTTBar = cms.untracked.bool (True)
    if MCType == 'Higgs':
        process.MiBiCommonNT.saveMCHiggs = cms.untracked.bool (True)
    if MCType == 'HiggsWW':
        process.MiBiCommonNT.saveMCHiggs   = cms.untracked.bool (True)
        process.MiBiCommonNT.saveMCHiggsWW = cms.untracked.bool (True)
    if MCType == 'HiggsGammaGamma':
        process.MiBiCommonNT.saveMCHiggs           = cms.untracked.bool (True)
        process.MiBiCommonNT.saveMCHiggsGammaGamma = cms.untracked.bool (True)                
    if MCType == 'ZW':
        process.MiBiCommonNT.saveMCZW = cms.untracked.bool (True)                
        
    process.MiBiCommonNTOneLeptonTwoJetsAK5PF = process.MiBiCommonNT.clone()
    process.MiBiCommonNTOneLeptonTwoJetsAK5PF.JetTag    = cms.InputTag("patJetsAK5PF")
    
    process.MiBiCommonNTOneLeptonTwoJetsAK5Calo = process.MiBiCommonNT.clone()
    process.MiBiCommonNTOneLeptonTwoJetsAK5Calo.JetTag    = cms.InputTag("patJetsAK5Calo")
    process.MiBiCommonNTOneLeptonTwoJetsAK5Calo.MetTag    = cms.InputTag("patMETsAK5Calo")
    
    process.MiBiCommonNTOneLeptonTwoJetsPFlow = process.MiBiCommonNT.clone()
    process.MiBiCommonNTOneLeptonTwoJetsPFlow.MuTag     = cms.InputTag("patMuonsPFlow")
    process.MiBiCommonNTOneLeptonTwoJetsPFlow.EleTag    = cms.InputTag("patElectronsPFlow")
    process.MiBiCommonNTOneLeptonTwoJetsPFlow.JetTag    = cms.InputTag("patJetsPFlow")
    process.MiBiCommonNTOneLeptonTwoJetsPFlow.MetTag    = cms.InputTag("patMETsPFlow")

    ## #cone: 0.1 ptcut: 8
    process.MiBiCommonNTTwoPhotons_01_8 = process.MiBiCommonNT.clone()
    process.MiBiCommonNTTwoPhotons_01_8.JetTag = cms.InputTag("patJetsAK5PF")
    process.MiBiCommonNTTwoPhotons_01_8.saveEleLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_01_8.saveMuonLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_01_8.MatchingConeTreshold = cms.double (0.1)
    process.MiBiCommonNTTwoPhotons_01_8.ElectronPtCut = cms.double (8.)
    process.MiBiCommonNTTwoPhotons_01_8.MuonPtCut = cms.double (8.)

    ## #cone: 0.2 ptcut: 8
    process.MiBiCommonNTTwoPhotons_02_8 = process.MiBiCommonNT.clone()
    process.MiBiCommonNTTwoPhotons_02_8.JetTag = cms.InputTag("patJetsAK5PF")
    process.MiBiCommonNTTwoPhotons_02_8.saveEleLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_02_8.saveMuonLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_02_8.MatchingConeTreshold = cms.double (0.2)
    process.MiBiCommonNTTwoPhotons_02_8.ElectronPtCut = cms.double (8.)
    process.MiBiCommonNTTwoPhotons_02_8.MuonPtCut = cms.double (8.)

    ## #cone: 0.05 ptcut: 5
    process.MiBiCommonNTTwoPhotons_005_5 = process.MiBiCommonNT.clone()
    process.MiBiCommonNTTwoPhotons_005_5.JetTag = cms.InputTag("patJetsAK5PF")
    process.MiBiCommonNTTwoPhotons_005_5.saveEleLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_005_5.saveMuonLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_005_5.MatchingConeTreshold = cms.double (0.05)
    process.MiBiCommonNTTwoPhotons_005_5.ElectronPtCut = cms.double (5.)
    process.MiBiCommonNTTwoPhotons_005_5.MuonPtCut = cms.double (5.)
    
    #cone: 0.05 ptcut: 8
    process.MiBiCommonNTTwoPhotons_005_8 = process.MiBiCommonNT.clone()
    process.MiBiCommonNTTwoPhotons_005_8.JetTag = cms.InputTag("patJetsAK5PF")
    process.MiBiCommonNTTwoPhotons_005_8.saveEleLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_005_8.saveMuonLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_005_8.MatchingConeTreshold = cms.double (0.05)
    process.MiBiCommonNTTwoPhotons_005_8.ElectronPtCut = cms.double (8.)
    process.MiBiCommonNTTwoPhotons_005_8.MuonPtCut = cms.double (8.)
    
    #cone: 0.05 ptcut: 10
    process.MiBiCommonNTTwoPhotons_005_10 = process.MiBiCommonNT.clone()
    process.MiBiCommonNTTwoPhotons_005_10.JetTag = cms.InputTag("patJetsAK5PF")
    process.MiBiCommonNTTwoPhotons_005_10.saveEleLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_005_10.saveMuonLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_005_10.MatchingConeTreshold = cms.double (0.05)
    process.MiBiCommonNTTwoPhotons_005_10.ElectronPtCut = cms.double (10.)
    process.MiBiCommonNTTwoPhotons_005_10.MuonPtCut = cms.double (10.)
    
    ## #cone: 0.04 ptcut: 8
    process.MiBiCommonNTTwoPhotons_004 = process.MiBiCommonNT.clone()
    process.MiBiCommonNTTwoPhotons_004.JetTag = cms.InputTag("patJetsAK5PF")
    process.MiBiCommonNTTwoPhotons_004.saveEleLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_004.saveMuonLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_004.MatchingConeTreshold = cms.double (0.04)
    
    ## #cone: 0.06 ptcut: 8
    process.MiBiCommonNTTwoPhotons_006 = process.MiBiCommonNT.clone()
    process.MiBiCommonNTTwoPhotons_006.JetTag = cms.InputTag("patJetsAK5PF")
    process.MiBiCommonNTTwoPhotons_006.saveEleLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_006.saveMuonLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_006.MatchingConeTreshold = cms.double (0.06)
    
    ## #cone: 0.08 ptcut: 8
    process.MiBiCommonNTTwoPhotons_008 = process.MiBiCommonNT.clone()
    process.MiBiCommonNTTwoPhotons_008.JetTag = cms.InputTag("patJetsAK5PF")
    process.MiBiCommonNTTwoPhotons_008.saveEleLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_008.saveMuonLessPV = cms.untracked.bool (True)
    process.MiBiCommonNTTwoPhotons_008.MatchingConeTreshold = cms.double (0.08)
    
    
#    process.MiBiPathAK5PF = cms.Path(process.MiBiCommonPAT*process.OneLeptonTwoJetsAK5PFSeq*process.MiBiCommonNTOneLeptonTwoJetsAK5PF)
#    process.MiBiPathAK5Calo = cms.Path(process.MiBiCommonPAT*process.OneLeptonTwoJetsAK5CaloSeq*process.MiBiCommonNTOneLeptonTwoJetsAK5Calo)
#    process.MiBiPathPFlow = cms.Path(process.MiBiCommonPAT*process.OneLeptonTwoJetsPFlowSeq*process.MiBiCommonNTOneLeptonTwoJetsPFlow)

    process.MiBiPathPhotons = cms.Path(process.MiBiCommonPAT*process.TwoPhotonsSeq*
                                       #process.MiBiCommonNTTwoPhotons_004*
                                       #process.MiBiCommonNTTwoPhotons_006*
                                       #process.MiBiCommonNTTwoPhotons_008*
                                       #process.MiBiCommonNTTwoPhotons_01_8*
                                       #process.MiBiCommonNTTwoPhotons_02_8*
                                       #process.MiBiCommonNTTwoPhotons_005_5*
                                       process.MiBiCommonNTTwoPhotons_005_8
                                       #process.MiBiCommonNTTwoPhotons_005_10
                                    )

