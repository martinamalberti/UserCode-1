#include "PhysicsTools/NtupleUtils/interface/treeReader.h"
#include "PhysicsTools/NtupleUtils/interface/ntpleUtils.h"
#include "PhysicsTools/NtupleUtils/interface/readJSONFile.h"

#include "Calibration/EcalCalibNtuple/interface/WZAnalysisVariablesSingleXtal.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include <iostream>
#include <vector>
#include <map>
#include <utility>

#include "TChain.h"
#include "TF1.h"
#include "TH1F.h"



void SetStepNames(std::map<int, std::string>&, const std::string&, const int&, const int&);
bool AcceptHLTPath(treeReader&, const std::pair<std::string,std::pair<int,int> >&);






int main(int argc, char** argv)
{
  //Check if all nedeed arguments to parse are there
  if(argc != 2)
  {
    std::cerr << ">>>>> WZAnalysis::usage: " << argv[0] << " configFileName" << std::endl ;
    return 1;
  }
  
  std::string fileName(argv[1]);
  boost::shared_ptr<edm::ParameterSet> parameterSet = edm::readConfig(fileName);
  
  
  // "Input"
  edm::ParameterSet Input =  parameterSet -> getParameter<edm::ParameterSet>("Input");
  std::string inputFileList = Input.getParameter<std::string>("inputFileList");
  std::string jsonFileName  = Input.getParameter<std::string>("jsonFileName");
  
  
  // "Output"
  edm::ParameterSet Output =  parameterSet -> getParameter<edm::ParameterSet>("Output");
  std::string outputRootFilePath = Output.getParameter<std::string>("outputRootFilePath");
  std::string outputRootFileName = Output.getParameter<std::string>("outputRootFileName");
  std::string outputRootFullFileName = outputRootFilePath + "/" + outputRootFileName + ".root";
  
  
  // "Options"
  edm::ParameterSet Options =  parameterSet -> getParameter<edm::ParameterSet>("Options");
  int entryMIN       = Options.getParameter<int>("entryMIN");
  int entryMAX       = Options.getParameter<int>("entryMAX");
  int entryMODULO    = Options.getParameter<int>("entryMODULO");
  int jsonFlag       = Options.getParameter<int>("jsonFlag");
  int verbosity      = Options.getParameter<int>("verbosity");
  
  
  // Get total number of events
  std::cout << ">>> WZAnalysis::Get number of events" << std::endl;
  std::map<int, int> beginEvents      = GetTotalEvents("AllPassFilterBegin/passedEvents",            inputFileList.c_str());
  std::map<int, int> goodVertexEvents = GetTotalEvents("AllPassFilterGoodVertexFilter/passedEvents", inputFileList.c_str());
  std::map<int, int> noScrapingEvents = GetTotalEvents("AllPassFilterNoScrapingFilter/passedEvents", inputFileList.c_str());
  std::map<int, int> HBHENoiseEvents  = GetTotalEvents("AllPassFilterHBHENoiseFilter/passedEvents",  inputFileList.c_str());
  std::map<int, int> electronEvents   = GetTotalEvents("AllPassFilterElectronFilter/passedEvents",   inputFileList.c_str());
  
  
  
  // Get run/LS map from JSON file
  std::cout << ">>> WZPreselection::Get run/LS map from JSON file" << std::endl;
  std::map<int, std::vector<std::pair<int, int> > > jsonMap;
  jsonMap = readJSONFile(jsonFileName);
  
  
  
  // define HLT paths
  std::vector<std::pair<std::string,std::pair<int,int> > > WHLTPathNames;
  
  std::pair<int,int> WRunRanges1(160404,161176);
  std::pair<std::string,std::pair<int,int> > WHLTPathName1("HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v1",WRunRanges1);
  std::pair<int,int> WRunRanges2(161216,163261);
  std::pair<std::string,std::pair<int,int> > WHLTPathName2("HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v2",WRunRanges2);
  std::pair<int,int> WRunRanges3(163269,163869);
  std::pair<std::string,std::pair<int,int> > WHLTPathName3("HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v3",WRunRanges3);
  std::pair<int,int> WRunRanges4(165088,165633);
  std::pair<std::string,std::pair<int,int> > WHLTPathName4("HLT_Ele32_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v3",WRunRanges4);
  std::pair<int,int> WRunRanges5(165970,166967);
  std::pair<std::string,std::pair<int,int> > WHLTPathName5("HLT_Ele25_WP80_PFMT40_v1",WRunRanges5);
  std::pair<int,int> WRunRanges6(167039,167913);
  std::pair<std::string,std::pair<int,int> > WHLTPathName6("HLT_Ele27_WP80_PFMT50_v1",WRunRanges6);
  std::pair<int,int> WRunRanges7(170249,173198);
  std::pair<std::string,std::pair<int,int> > WHLTPathName7("HLT_Ele32_WP70_PFMT50_v3",WRunRanges7);
  std::pair<int,int> WRunRanges8(173236,999999);
  std::pair<std::string,std::pair<int,int> > WHLTPathName8("HLT_Ele32_WP70_PFMT50_v4",WRunRanges8);
  
  WHLTPathNames.push_back(WHLTPathName1);
  WHLTPathNames.push_back(WHLTPathName2);
  WHLTPathNames.push_back(WHLTPathName3);
  WHLTPathNames.push_back(WHLTPathName4);
  WHLTPathNames.push_back(WHLTPathName5);
  WHLTPathNames.push_back(WHLTPathName6);
  WHLTPathNames.push_back(WHLTPathName7);
  WHLTPathNames.push_back(WHLTPathName8);
  
  
  
  std::vector<std::pair<std::string,std::pair<int,int> > > ZHLTPathNames;
  
  std::pair<int,int> ZRunRanges1(160404,161176);
  std::pair<std::string,std::pair<int,int> > ZHLTPathName1("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v1",ZRunRanges1);
  std::pair<int,int> ZRunRanges2(161216,163261);
  std::pair<std::string,std::pair<int,int> > ZHLTPathName2("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v2",ZRunRanges2);
  std::pair<int,int> ZRunRanges3(163269,163869);
  std::pair<std::string,std::pair<int,int> > ZHLTPathName3("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v3",ZRunRanges3);
  std::pair<int,int> ZRunRanges4(165088,165633);
  std::pair<std::string,std::pair<int,int> > ZHLTPathName4("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v4",ZRunRanges4);
  std::pair<int,int> ZRunRanges5(165970,166967);
  std::pair<std::string,std::pair<int,int> > ZHLTPathName5("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v5",ZRunRanges5);
  std::pair<int,int> ZRunRanges6(167039,167913);
  std::pair<std::string,std::pair<int,int> > ZHLTPathName6("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v6",ZRunRanges6);
  std::pair<int,int> ZRunRanges7(170249,170759);
  std::pair<std::string,std::pair<int,int> > ZHLTPathName7("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v6",ZRunRanges7);
  std::pair<int,int> ZRunRanges8(170826,173198);
  std::pair<std::string,std::pair<int,int> > ZHLTPathName8("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v7",ZRunRanges8);
  std::pair<int,int> ZRunRanges9(173236,999999);
  std::pair<std::string,std::pair<int,int> > ZHLTPathName9("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v8",ZRunRanges9);
  
  ZHLTPathNames.push_back(ZHLTPathName1);
  ZHLTPathNames.push_back(ZHLTPathName2);
  ZHLTPathNames.push_back(ZHLTPathName3);
  ZHLTPathNames.push_back(ZHLTPathName4);
  ZHLTPathNames.push_back(ZHLTPathName5);
  ZHLTPathNames.push_back(ZHLTPathName6);
  ZHLTPathNames.push_back(ZHLTPathName7);
  ZHLTPathNames.push_back(ZHLTPathName8);
  ZHLTPathNames.push_back(ZHLTPathName9);
  
  
  
  // Open tree
  std::cout << ">>> WZAnalysis::Open old tree" << std::endl;
  std::string treeName = "simpleNtuple/SimpleNtuple";
  TChain* chain = new TChain(treeName.c_str());
  if(!FillChain(*chain, inputFileList.c_str())) return 1;
  treeReader reader((TTree*)(chain), false);
  
  
  
  // Open output root file
  outputRootFileName += ".root";
  
  
  
  // define histograms
  std::cout << ">>> WZAnalysis::Define histograms" << std::endl;
  
  int nStep = 10;
  TH1F* events = new TH1F("events", "events", nStep, 0., 1.*nStep);
  std::map<int, int> stepEvents;
  std::map<int, std::string> stepNames;
  
  
  
  // define the reduced ntuple 
  WZAnalysisVariablesSingleXtal vars;
  InitializeWZAnalysisTree(vars,outputRootFullFileName);
  
  
   
  //**********************
  // STEP 1 - Begin events
  int step = 1;
  SetStepNames(stepNames, "All events", step, verbosity);
  stepEvents[step] = beginEvents[1];
  
  
  
  //****************************
  // STEP 2 - Good Vertex events
  step = 2;
  SetStepNames(stepNames, "Good vertex", step, verbosity);
  stepEvents[step] = goodVertexEvents[1];
  
  
  
  //*********************
  // STEP 3 - No Scraping
  step = 3;
  SetStepNames(stepNames, "No scraping", step, verbosity);
  stepEvents[step] = noScrapingEvents[1];
  
  
  
  //*********************
  // STEP 4 - HBHE Noise
  step = 4;
  SetStepNames(stepNames, "HBHE Noise", step, verbosity);
  stepEvents[step] = HBHENoiseEvents[1];
  
  
  
  //******************
  // STEP 5 - Electron
  step = 5;
  SetStepNames(stepNames, "Electron", step, verbosity);
  stepEvents[step] = electronEvents[1];
   
  
  
  //*********************
  // LOOP OVER THE EVENTS
  std::cout << ">>>>> WZAnalysis::Read " << chain -> GetEntries() << " entries" << std::endl;  
  for(int entry = entryMIN ; entry < chain -> GetEntries() ; ++entry)
  {
    reader.GetEntry(entry);
    if((entry%entryMODULO) == 0) std::cout << ">>>>> WZAnalysis::GetEntry " << entry << std::endl;   
    if(entry == entryMAX) break;
      
    // clear variables
    ClearWZAnalysisVariables(vars);    
    
    // event variables
    vars.runId   = reader.GetInt("runId")->at(0);
    vars.lumiId  = reader.GetInt("lumiId")->at(0);
    
    
    //**************************
    // STEP 6 - run/LS selection
    step = 6;
    SetStepNames(stepNames, "run/LS", step, verbosity);
    
    bool skipEvent = false;
    
    if(AcceptEventByRunAndLumiSection(vars.runId,vars.lumiId,jsonMap) == false)
      skipEvent = true;

    if( (jsonFlag == 1) && (skipEvent == true) ) continue;
    stepEvents[step] += 1;
    
    //***********************
    // STEP 7 - HLT selection
    step += 1;
    SetStepNames(stepNames, "HLT", step, verbosity);
    
    
    skipEvent = true;
    bool isWHLT = false;
    bool isZHLT = false;
    
    if( verbosity == 1 )
    {
      std::vector<std::string> HLT_names = *(reader.GetString("HLT_Names"));
      for(unsigned int HLTIt = 0; HLTIt < HLT_names.size(); ++HLTIt)
        std::cout << "HLT bit " << HLTIt 
            << ":   "     << HLT_names.at(HLTIt)
            << std::endl;
    }
    
    // W triggers
    for(unsigned int HLTIt = 0; HLTIt < WHLTPathNames.size(); ++HLTIt)
    {
      if( AcceptHLTPath(reader, WHLTPathNames.at(HLTIt)) == true )
        skipEvent = false;
      isWHLT = true;
    }
    // Z triggers
    for(unsigned int HLTIt = 0; HLTIt < ZHLTPathNames.size(); ++HLTIt)
    {
      if( AcceptHLTPath(reader, ZHLTPathNames.at(HLTIt)) == true )
        skipEvent = false;
      isZHLT = true; 
    }
    
    if( skipEvent == true ) continue;
    stepEvents[step] += 1;    
        
    
    
    
    
    
    //**************************
    // STEP 8 - cut on electrons
    step += 1;
    SetStepNames(stepNames, "1/2 ele", step, verbosity);
    
    
    int nTightEle = 0;
    int nMediumEle = 0;
    int nLooseEle = 0;
    std::map<float,int> eleIts;
    
    // loop on electrons
    for(unsigned int eleIt = 0; eleIt < (reader.Get4V("electrons")->size()); ++eleIt)
    {
      ROOT::Math::XYZTVector ele = reader.Get4V("electrons")->at(eleIt);
      float pt = ele.pt();
      float eta = ele.eta();
      
      float tkIso  = reader.GetFloat("electrons_tkIso03")->at(eleIt);
      float emIso  = reader.GetFloat("electrons_emIso03")->at(eleIt);
      float hadIso = reader.GetFloat("electrons_hadIso03_1")->at(eleIt) + 
          reader.GetFloat("electrons_hadIso03_2")->at(eleIt);
      float combIso = tkIso + emIso + hadIso;
      
      int isEB = reader.GetInt("electrons_isEB")->at(eleIt);
      
      float sigmaIetaIeta = reader.GetFloat("electrons_sigmaIetaIeta")->at(eleIt);
      float DetaIn        = reader.GetFloat("electrons_deltaEtaIn")->at(eleIt);
      float DphiIn        = reader.GetFloat("electrons_deltaPhiIn")->at(eleIt);
      float HOverE        = reader.GetFloat("electrons_hOverE")->at(eleIt);
      
      int mishits             = reader.GetInt("electrons_mishits")->at(eleIt);
      int nAmbiguousGsfTracks = reader.GetInt("electrons_nAmbiguousGsfTracks")->at(eleIt);
      float dist = reader.GetFloat("electrons_dist")->at(eleIt);
      float dcot = reader.GetFloat("electrons_dcot")->at(eleIt);
      
      
      // tight electrons
      bool isTightElectron = false;
      if(
         (pt > 20.) &&
         (fabs(eta) < 2.5) &&
          // EleID WP80 - 2010
         ( ( (isEB == 1) && (combIso/pt    < 0.070) ) || ( (isEB == 0) && (combIso/pt    < 0.060) ) ) &&
         ( ( (isEB == 1) && (sigmaIetaIeta < 0.010) ) || ( (isEB == 0) && (sigmaIetaIeta < 0.030) ) ) &&
         ( ( (isEB == 1) && (fabs(DphiIn)  < 0.060) ) || ( (isEB == 0) && (fabs(DphiIn)  < 0.030) ) ) &&
         ( ( (isEB == 1) && (fabs(DetaIn)  < 0.004) ) || ( (isEB == 0) && (fabs(DetaIn)  < 0.007) ) ) &&
          //( ( (isEB == 1) && (HOverE        < 0.040) ) || ( (isEB == 0) && (HOverE        < 0.025) ) ) &&
         ( mishits == 0 ) &&
         ( nAmbiguousGsfTracks == 0 ) &&
         ( ( fabs(dist) > 0.02 ) || ( fabs(dcot) > 0.02 ) )
        )
      {
        isTightElectron = true;
        ++nTightEle;
        eleIts[1./pt] = eleIt;
      }
      
      
      // semi-tight electrons
      if( isTightElectron == true ) continue;
      //      bool isMediumElectron = false;
      if(
         (pt > 12.) &&
         (fabs(eta) < 2.5) &&
          // EleID WP80 - 2010
         ( ( (isEB == 1) && (combIso/pt    < 0.070) ) || ( (isEB == 0) && (combIso/pt    < 0.060) ) ) &&
         ( ( (isEB == 1) && (sigmaIetaIeta < 0.010) ) || ( (isEB == 0) && (sigmaIetaIeta < 0.030) ) ) &&
         ( ( (isEB == 1) && (fabs(DphiIn)  < 0.060) ) || ( (isEB == 0) && (fabs(DphiIn)  < 0.030) ) ) &&
         ( ( (isEB == 1) && (fabs(DetaIn)  < 0.004) ) || ( (isEB == 0) && (fabs(DetaIn)  < 0.007) ) ) &&
          //( ( (isEB == 1) && (HOverE        < 0.040) ) || ( (isEB == 0) && (HOverE        < 0.025) ) ) &&
         ( mishits == 0 ) &&
         ( nAmbiguousGsfTracks == 0 ) &&
         ( ( fabs(dist) > 0.02 ) || ( fabs(dcot) > 0.02 ) )
        )
      {
	//        isMediumElectron = true;
        ++nMediumEle;
        eleIts[1./pt] = eleIt;
      }
      
      
      // loose electrons
      if( isTightElectron == true ) continue;
      if( 
          (pt > 10.) &&
          (fabs(eta) < 2.5) &&
          // EleID WP95 - 2010
          ( ( (isEB == 1) && (combIso/pt    < 0.150) ) || ( (isEB == 0) && (combIso/pt    < 0.100) ) ) &&
          ( ( (isEB == 1) && (sigmaIetaIeta < 0.010) ) || ( (isEB == 0) && (sigmaIetaIeta < 0.030) ) ) &&
          ( ( (isEB == 1) && (fabs(DphiIn)  < 0.800) ) || ( (isEB == 0) && (fabs(DphiIn)  < 0.700) ) ) &&
          ( ( (isEB == 1) && (fabs(DetaIn)  < 0.007) ) || ( (isEB == 0) && (fabs(DetaIn)  < 0.010) ) ) &&
          ( ( (isEB == 1) && (HOverE        < 0.150) ) || ( (isEB == 0) && (HOverE        < 0.070) ) )
        )
      {
        ++nLooseEle;
      }
      
    } // loop on electrons
    
    
    
    int nLooseMu = 0;
    
    // loop on muons
    for(unsigned int muIt = 0; muIt < (reader.Get4V("muons")->size()); ++muIt)
    {
      ROOT::Math::XYZTVector mu = reader.Get4V("muons")->at(muIt);
      float pt = mu.pt();
      float eta = mu.eta();
      
      float tkIso  = reader.GetFloat("muons_tkIso03")->at(muIt);
      float emIso  = reader.GetFloat("muons_emIso03")->at(muIt);
      float hadIso = reader.GetFloat("muons_hadIso03")->at(muIt);
      float combIso = tkIso + emIso + hadIso;
      
      int global = reader.GetInt("muons_global")->at(muIt);

      if( (pt > 10.) &&
           (fabs(eta) < 2.5) &&
           (combIso/pt < 0.20) &&
           (global == 1) )
      {
        ++nLooseMu;
      }
    }
    
    
    // cuts
    if( verbosity == 1 )
      std::cout << " nTightEle = "  << nTightEle
          << " nMediumEle = " << nMediumEle
          << " nLooseEle = "  << nLooseEle
          << " nLooseMu = "   << nLooseMu
          << std::endl;
    if( nTightEle < 1 ) continue;
    if( nTightEle > 2 ) continue;
    if( nMediumEle > 1 ) continue;
    if( nLooseEle > 0 ) continue;
    if( nLooseMu > 0 ) continue;
    stepEvents[step] += 1;
    
    
    // set electron variables
    std::map<float,int>::const_iterator mapIt = eleIts.begin();

    //PhotonFix::initialise("4_2");

    if( (nTightEle == 1) && (nMediumEle == 0) )
    {
      SetElectron1Variables(vars,reader,mapIt->second);
      //PhotonFix Correction1 (vars.ele1_scE,vars.ele1_scEta,vars.ele1_scPhi,vars.ele1_e3x3/vars.ele1_scE);
      //vars.ele1_scLocalContCorr_DK = Correction1.fixedEnergy()/vars.ele1_scE;
    }
    
    mapIt = eleIts.begin();
    if( (nTightEle == 2) || (nTightEle == 1 && nMediumEle == 1) )
    {
      SetElectron1Variables(vars,reader,mapIt->second);
      //PhotonFix Correction1 (vars.ele1_scE,vars.ele1_scEta,vars.ele1_scPhi,vars.ele1_e3x3/vars.ele1_scE);
      //vars.ele1_scLocalContCorr_DK = Correction1.fixedEnergy()/vars.ele1_scE;
      
      ++mapIt;
      
      SetElectron2Variables(vars,reader,mapIt->second);
      //PhotonFix Correction2 (vars.ele2_scE,vars.ele2_scEta,vars.ele2_scPhi,vars.ele2_e3x3/vars.ele2_scE);
      //vars.ele2_scLocalContCorr_DK = Correction2.fixedEnergy()/vars.ele2_scE;
    }
    
    
    // set met variables
    SetMetVariables(vars,reader);
    
    // set di-electron variables
    if( (nTightEle == 2) || (nTightEle == 1 && nMediumEle == 1) )
    {
      SetDiElectronVariables(vars,reader);
    }
    
    
    
    
    
    
    //***********************
    // STEP 9 - W selection
    step += 1;
    SetStepNames(stepNames, "W selection", step, verbosity);
    
    
    if( (nTightEle == 1) && (nMediumEle == 0) )
    {
      if( isWHLT == false ) continue;
      if( vars.ele1_pt < 30. ) continue;
      // EleID WP70 - 2010
      if( ( vars.ele1_isEB == 1 ) && ( (vars.ele1_tkIso+vars.ele1_emIso+vars.ele1_hadIso)/vars.ele1_pt > 0.04 ) ) continue;
      if( ( vars.ele1_isEB == 1 ) && ( fabs(vars.ele1_DphiIn) > 0.030 ) ) continue;
      if( ( vars.ele1_isEB == 1 ) && ( fabs(vars.ele1_DetaIn) > 0.004 ) ) continue;
      if( ( vars.ele1_isEB == 1 ) && ( vars.ele1_HOverE > 0.025 ) ) continue;
      if( ( vars.ele1_isEB == 0 ) && ( (vars.ele1_tkIso+vars.ele1_emIso+vars.ele1_hadIso)/vars.ele1_pt > 0.03 ) ) continue;
      if( ( vars.ele1_isEB == 0 ) && ( fabs(vars.ele1_DphiIn) > 0.020 ) ) continue;
      if( ( vars.ele1_isEB == 0 ) && ( fabs(vars.ele1_DetaIn) > 0.005 ) ) continue;
      if( ( vars.ele1_isEB == 0 ) && ( vars.ele1_HOverE > 0.025 ) ) continue;
      
      if( vars.met_et       < 25.00 ) continue;
      if( vars.ele1Met_mt   < 50.00 ) continue;
      if( vars.ele1Met_Dphi <  1.57 ) continue;
      stepEvents[step] += 1;
      
      
      vars.isW = 1;
      vars.isZ = 0;
      
      
      // fill the reduced tree
      FillWZAnalysisTree(vars);
    }
    
    
    
    
    
    
    //***********************
    // STEP 10 - Z selection
    step += 1;
    SetStepNames(stepNames, "Z selection", step, verbosity);
    
    
    if( (nTightEle == 2) || (nTightEle == 1 && nMediumEle == 1) )
    {
      if( isZHLT == false ) continue;
      if( vars.met_et     >  40. ) continue;
      if( vars.ele1ele2_m <  70. ) continue;
      if( vars.ele1ele2_m > 110. ) continue;
      if( (vars.ele1_charge * vars.ele2_charge) != -1. ) continue;
      stepEvents[step] += 1;
      
      
      vars.isW = 0;
      vars.isZ = 1;
      
      
      // fill the reduced tree
      FillWZAnalysisTree(vars);
    }
    
    
    
  } // loop over the events
  
  
  
  
  
  
  // save the reduced tree
  DeleteWZAnalysisVariables(vars);
  
  
  // save the histograms
  TFile* outputRootFile = new TFile((outputRootFullFileName).c_str(), "UPDATE");
  outputRootFile -> cd();
  
  for(step = 1; step <= nStep; ++step)
  {
    events -> SetBinContent(step, stepEvents[step]);
    events -> GetXaxis() -> SetBinLabel(step, stepNames[step].c_str());
  }

  events -> Write();
  outputRootFile -> Close();
  
  
  return 0;
}






void SetStepNames(std::map<int, std::string>& stepNames, const std::string& stepName, const int& step, const int& verbosity)
{
  char dummy[5];
  sprintf(dummy, "%d)", step);
  stepNames[step] = std::string(dummy)+" "+stepName;

  if(verbosity)
    std::cout << ">>>>>>>>> " << stepNames[step] << std::endl;
}




bool AcceptHLTPath(treeReader& reader, const std::pair<std::string,std::pair<int,int> >& HLTPathName)
{
  bool acceptEvent = false;

  int runId = reader.GetInt("runId")->at(0);
  if( runId < (HLTPathName.second).first )  return acceptEvent;
  if( runId > (HLTPathName.second).second ) return acceptEvent;

  std::vector<std::string> HLT_names = *(reader.GetString("HLT_Names"));
  for(unsigned int HLTIt = 0; HLTIt < HLT_names.size(); ++HLTIt)
    if( (reader.GetString("HLT_Names")->at(HLTIt) == HLTPathName.first) &&
         (reader.GetFloat("HLT_Accept")->at(HLTIt) == 1) )
      acceptEvent = true;

  return acceptEvent;
}
