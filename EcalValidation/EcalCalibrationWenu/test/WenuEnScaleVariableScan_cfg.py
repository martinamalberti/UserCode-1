import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

process.inputTree = cms.PSet(
  treeNameDATA = cms.string("ntupleEcalCalibration/myTree"),
  inputFileDATA = cms.string("EcalCalibrationJoin.root"),
  treeNameMC = cms.string("myTree"),
  inputFileMC = cms.string("Wenu_WSUMMED_4analysis.root")
)

process.outputTree = cms.PSet(
  outputFile = cms.string("out.root")
)


process.options = cms.PSet(
  EEorEB = cms.int32(5),
    
  numToyMC = cms.int32(100),
  MinScan = cms.double(-0.1),
  MaxScan = cms.double(0.05),
  iNoSteps = cms.int32(200),

  MinScanRange = cms.double(-0.04),
  MaxScanRange = cms.double(0.01),

  cut = cms.string("1"),
  
  minET = cms.double(20.),

  minBINS      = cms.double(0.8),
  maxBINS      = cms.double(2.2),
  numBINS      = cms.int32(30)
)




process.ScanOptions = cms.PSet(
   ScanVariables = cms.vstring("abs(eleFBrem)","abs(eta)"),
   ScanVariablesId = cms.vstring("abseleFBrem","eta"),
   minValueabseleFBrem = cms.vdouble(0.0,0.5),
   maxValueabseleFBrem = cms.vdouble(0.5,1.0),
   minValueeta = cms.vdouble(0.000,0.435,0.783,1.131),
   maxValueeta = cms.vdouble(0.435,0.783,1.131,1.479),
   variablesName = cms.vstring("ESC/p"),
   variablesNameId = cms.vstring("ESCp")
)



