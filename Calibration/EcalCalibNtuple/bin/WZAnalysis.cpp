#include "PhysicsTools/NtupleUtils/interface/treeReader.h"
#include "PhysicsTools/NtupleUtils/interface/ntpleUtils.h"
#include "PhysicsTools/NtupleUtils/interface/readJSONFile.h"

#include "Calibration/EcalCalibNtuple/interface/WZAnalysisVariables.h"

#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include <iostream>
#include <vector>
#include <map>

#include "TChain.h"
#include "TF1.h"
#include "TH1F.h"



void SetStepNames(std::map<int, std::string>&, const std::string&, const int&, const int&);
bool AcceptHLTPath(treeReader&, const std::string&);






int main(int argc, char** argv)
{
  //Check if all nedeed arguments to parse are there
  if(argc != 2)
  {
    std::cerr << ">>>>> WZAnalysis::usage: " << argv[0] << " configFileName" << std::endl ;
    return 1;
  }
  
  std::string fileName(argv[1]);
  boost::shared_ptr<edm::ProcessDesc> processDesc = edm::readConfig(fileName);
  boost::shared_ptr<edm::ParameterSet> parameterSet = processDesc->getProcessPSet();
  
  
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
  int dataFlag       = Options.getParameter<int>("dataFlag");
  int jsonFlag       = Options.getParameter<int>("jsonFlag");
  float crossSection = Options.getParameter<double>("crossSection");
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
  std::vector<std::string> HLTPathNames;
  HLTPathNames.push_back("HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v1");
  HLTPathNames.push_back("HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v2");
  HLTPathNames.push_back("HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_v3");
  
  
  
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
  WZAnalysisVariables vars;
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
    vars.dataFlag = dataFlag;
    vars.totEvents = beginEvents[1];
    vars.crossSection = crossSection;
    vars.eventNaiveId += 1;
    vars.runId   = reader.GetInt("runId")->at(0);
    vars.lumiId  = reader.GetInt("lumiId")->at(0);
    vars.eventId = reader.GetInt("eventId")->at(0);
    vars.timeStampLow  = reader.GetInt("timeStampLow")->at(0);
    vars.timeStampHigh = reader.GetInt("timeStampHigh")->at(0);
    
    
    
    
    
    
    //**************************
    // STEP 6 - run/LS selection
    step = 6;
    SetStepNames(stepNames, "run/LS", step, verbosity);
    
    
    bool skipEvent = false;
    if( vars.dataFlag == 1 )
      if(AcceptEventByRunAndLumiSection(vars.runId,vars.lumiId,jsonMap) == false)
        skipEvent = true;
    
    if( (jsonFlag == 1) && (skipEvent == true) ) continue;
    stepEvents[step] += 1;
    
    
    
    
    
    
    //***********************
    // STEP 7 - HLT selection
    step += 1;
    SetStepNames(stepNames, "HLT", step, verbosity);
    
    
    skipEvent = true;
    
    if( verbosity == 1 )
    {
      std::vector<std::string> HLT_names = *(reader.GetString("HLT_Names"));
      for(unsigned int HLTIt = 0; HLTIt < HLT_names.size(); ++HLTIt)
	std::cout << "HLT bit " << HLTIt 
                  << ":   "     << HLT_names.at(HLTIt)
                  << std::endl;
    }
    
    std::vector<std::string> HLT_names = *(reader.GetString("HLT_Names"));
    for(unsigned int HLTIt = 0; HLTIt < HLTPathNames.size(); ++HLTIt)
    {
      if( AcceptHLTPath(reader, HLTPathNames.at(HLTIt)) == true )
        skipEvent = false;
    }
    
    if( skipEvent == true ) continue;
    stepEvents[step] += 1;    
    
    
    
    
    
    
    //**************************
    // STEP 8 - cut on electrons
    step += 1;
    SetStepNames(stepNames, "1/2 ele", step, verbosity);
    
    
    int nTightEle = 0;
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
      
      
      bool isTightElectron = false;
      
      // tight electrons
      if(
          (pt > 20.) &&
          (fabs(eta) < 2.5) &&
          ( ( (isEB == 1) && (combIso/pt    < 0.070) ) || ( (isEB == 0) && (combIso/pt    < 0.060) ) ) &&
          ( ( (isEB == 1) && (sigmaIetaIeta < 0.010) ) || ( (isEB == 0) && (sigmaIetaIeta < 0.030) ) ) &&
          ( ( (isEB == 1) && (fabs(DphiIn)  < 0.060) ) || ( (isEB == 0) && (fabs(DphiIn)  < 0.030) ) ) &&
          ( ( (isEB == 1) && (fabs(DetaIn)  < 0.004) ) || ( (isEB == 0) && (fabs(DetaIn)  < 0.007) ) ) &&
          ( ( (isEB == 1) && (HOverE        < 0.040) ) || ( (isEB == 0) && (HOverE        < 0.025) ) ) &&
          ( mishits == 0 ) &&
          ( nAmbiguousGsfTracks == 0 ) &&
          ( ( fabs(dist) > 0.02 ) || ( fabs(dcot) > 0.02 ) )
        )
      {
        isTightElectron = true;
        ++nTightEle;
        eleIts[1./pt] = eleIt;
      }
      
      
      // loose electrons
      if( isTightElectron == true ) continue;
      if( 
          (pt > 10.) &&
          (fabs(eta) < 2.5) &&
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
                << " nLooseEle = " << nLooseEle
                << " nLooseMu = "  << nLooseMu
                << std::endl;
    if( nTightEle < 1 ) continue;
    if( nTightEle > 2 ) continue;
    if( nLooseEle > 0 ) continue;
    if( nLooseMu > 0 ) continue;
    stepEvents[step] += 1;
    
    
    
    // set electron variables
    std::map<float,int>::const_iterator mapIt = eleIts.begin();
    
    if( nTightEle == 1 )
    {
      SetElectron1Variables(vars,reader,mapIt->second);
    }
    
    if( nTightEle == 2 )
    {
      SetElectron1Variables(vars,reader,mapIt->second);
      ++mapIt;
      SetElectron2Variables(vars,reader,mapIt->second);
    }
    
    
    // set met variables
    SetMetVariables(vars,reader);
    
    
    // set di-electron variables
    if( nTightEle == 2)
    {
      SetDiElectronVariables(vars,reader);
    }
    
    
    
    
    
    
    //***********************
    // STEP 9 - W selection
    step += 1;
    SetStepNames(stepNames, "W selection", step, verbosity);
    
    
    if( nTightEle == 1 )
    {
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
    
    
    if( nTightEle == 2 )
    {
      if( vars.met_et     > 30. ) continue;
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




bool AcceptHLTPath(treeReader& reader, const std::string& HLTPathName)
{
  bool acceptEvent = false;

  std::vector<std::string> HLT_names = *(reader.GetString("HLT_Names"));
  for(unsigned int HLTIt = 0; HLTIt < HLT_names.size(); ++HLTIt)
    if( (reader.GetString("HLT_Names")->at(HLTIt) == HLTPathName) &&
        (reader.GetFloat("HLT_Accept")->at(HLTIt) == 1) )
      acceptEvent = true;

  return acceptEvent;
}