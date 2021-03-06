#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <iostream>
#include <strstream>

#include "TH1.h"
#include "TChain.h"

//PG for the cluster-based calibration 
#include "Calibration/EcalCalibAlgos/interface/VEcalCalibBlock.h"
#include "Calibration/EcalCalibAlgos/interface/IMACalibBlock.h"
#include "Calibration/EcalCalibAlgos/interface/L3CalibBlock.h"


//---- AM added
#include "CaloOnlineTools/EcalTools/interface/EcalCosmicsTreeUtils.h"
#include "CaloOnlineTools/EcalTools/interface/EcalCosmicsTreeContent.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "CRUZET/Calibration/interface/CRUtils.h"
#include "TFile.h"
#include <iostream>
#include "TH2.h"
#include "TProfile.h"
#include "TSystem.h"


#define PI 3.14159265

int main (int argc, char** argv)
{

 bool WriteTH1F = false; //---- save fitted histograms
 bool WriteTH1dEdX = true; //---- save fitted histograms
 bool flag_Mu_Sky_cut = false; //---- Cut on the angle Mu - Sky ----

 double Cut_Angle_Mu_Xtal = 180.; //---- degrees ----
 double Cut_D0 = 5000.; //----
 double Cut_Dz = 5000.; //----
 double EnergyMaxCrystal_Min_Cut = 0.025; //---- 0.05 GeV --> 50 MeV
 double EnergyMaxCrystal_Max_Cut = 8.0; //---- 2 GeV

 double EnergyPerCrystal_Min_Cut = 0.0250; //---- 0.025 GeV --> 25 MeV
 double EnergyPerCrystal_Max_Cut = 8.0; //---- 2 GeV


 std::map<int, TH1F *> dEdx_Histos ;
 std::map<int, TH1F *> dEdx_Ring_Histos ;
 std::map<int, TH1F *> dEdx_Ring_1M_Up_Histos ;
 std::map<int, TH1F *> dEdx_Ring_1M_Down_Histos ;

 std::map<int, TH1F *> dEdx_Ring_UR_Histos ;
 std::map<int, TH1F *> dEdx_Ring_UL_Histos ;
 std::map<int, TH1F *> dEdx_Ring_DR_Histos ;
 std::map<int, TH1F *> dEdx_Ring_DL_Histos ;

 std::map<int, TH1F *> dE_Histos ;
 std::map<int, TH1F *> dE_Ring_Histos ;
 std::map<int, TH1F *> dE_Ring_1M_Up_Histos ;
 std::map<int, TH1F *> dE_Ring_1M_Down_Histos ;

 std::map<int, TH1F *> dEdx_Ring_Histos_EnergyCut ;
 std::map<int, TH1F *> dEdx_Ring_1M_Up_Histos_EnergyCut ;
 std::map<int, TH1F *> dEdx_Ring_1M_Down_Histos_EnergyCut ;

 int nEntries = -1 ;

  //---- AM Command line input ----

  //  for (int i=0; i<argc; i++)

 std::string outputRootName = "outputHistos8SepXCastor.root";
 std::string testNameOutput = "-o";
 if (argc>2){
  if (argv[1] == testNameOutput) {
   outputRootName = argv[2];
  }
 }

 std::cout << outputRootName << std::endl;



//  std::string testHelp = "--help";
//  if (argc==2){
//   if (argv[1] == testHelp) {
//    std::cout << "Help" << std::endl ;
//    std::cout << " --help : display help" << std::endl ;
//    std::cout << " -o : output root file name (eg histograms.root)" << std::endl ;
//    std::cout << " -i : input root file name (eg histograms.root)" << std::endl ;
//    std::cout << " name of input file : list name of input files ntuples" << std::endl ;     
//    exit(1);
//   }
//  }


//  if (argc < 2)
//  {
//   std::cerr << "ERROR : ntuple name missing" << std::endl ;
//   exit (1) ;
//  }



  //---- Tree construction: sum over different trees ----
 std::string testNameInput = "-i";
 TChain * chain = new TChain ("EcalCosmicsAnalysis") ;

//  if (argv[1] != testNameInput && argv[3] != testNameInput){
//    if (argv[1] == testNameOutput) {
//      for (int i=3; i< (argc); i++) {
//        chain->Add (argv[i]) ;
//        std::cout << "Open: " << argv[i] << std::endl;
//      }
//    }
//    else{
//      for (int i=1; i< (argc); i++) {
//        chain->Add (argv[i]) ;
//        std::cout << "Open: " << argv[i] << std::endl;
//      }
//    }
//  }
// else {
 char inputRootName[100];
//  for (int i=1; i< 10; i++) {







 //---------------------------------------------------------
 //---------------------- Input Files ----------------------
 //---------------------------------------------------------



 char buffer[1000];
 char inputName[1000];

 //---------------------
 //---- Cloned Tree ----
 //---------------------

//  std::string inputNameDirectory = "/tmp/govoni/50908Cosmic";
 std::string inputNameDirectory = "";
 std::string inputNameSearch = "/data/CRUZET/ntuples4/cloned_G*";

 std::string Command2Line = "ls " + inputNameSearch + " > temp.txt";
 std::cout << Command2Line << std::endl;
 gSystem->Exec(Command2Line.c_str());

 ifstream fileC_50908_Cloned("temp.txt");
 std::cerr << " Reading  " << inputNameDirectory << " ... " << std::endl;
 while (!fileC_50908_Cloned.eof()){
  fileC_50908_Cloned.getline (&buffer[0],1000);
  std::istrstream line(buffer);
  std::string uselessString;
  int uselessInt;
  std::string treeName; 
  line >> treeName;
  sprintf(inputName,"%s%s",inputNameDirectory.c_str(),treeName.c_str());
  std::cerr << "  File ->  " << treeName;
  std::cerr << "  File ->  " << inputName << std::endl;
  chain->Add (inputName);
 }

 Command2Line = "rm temp.txt";
 gSystem->Exec(Command2Line.c_str());

 //------------------------------------------------------------
 //---------------------- End Input Files ----------------------
 //-------------------------------------------------------------

 // da castor pochi eventi Global || Cosmic  
 /*
 for (int i=1; i< 46; i++) {
 sprintf (inputRootName,"/castor/cern.ch/user/m/mattia/50908Global/EcalCosmicsTree-50908%i.tree.root",i); 
   //   sprintf (inputRootName,"/castor/cern.ch/user/m/mattia/50908Cosmic/EcalCosmicsTree-50908%i.tree.root",i); 
 chain->Add (inputRootName);
 std::cout << "Open: " << inputRootName << std::endl;
}

 for (int i=1; i< 48; i++) {
 if (i!=28 && i!=35){
 sprintf (inputRootName,"/castor/cern.ch/user/m/mattia/50911Global/50911_%d.tree.root",i); 
   //   sprintf (inputRootName,"/castor/cern.ch/user/m/mattia/50908Cosmic/EcalCosmicsTree-50908%i.tree.root",i); 
 chain->Add (inputRootName);
 std::cout << "Open: " << inputRootName << std::endl;
}
}
 for (int i=1; i< 39; i++) {
 sprintf (inputRootName,"/castor/cern.ch/user/m/mattia/51503Global/EcalCosmicsTree-51503%d.tree.root",i); 
   //   sprintf (inputRootName,"/castor/cern.ch/user/m/mattia/50908Cosmic/EcalCosmicsTree-50908%i.tree.root",i); 
 chain->Add (inputRootName);
 std::cout << "Open: " << inputRootName << std::endl;
}
 */


 //---- da castor nuove NTuple (cloned)



 /*
 sprintf(inputName,"/afs/cern.ch/user/a/amartell/public/perAmassi/CRUZET_elenco/listaCosmic.txt");
 //sprintf(inputName,"/afs/cern.ch/user/a/amartell/public/perAmassi/CRUZET_elenco/listaGlobal.txt");
 ifstream fileC_51047(inputName);
 std::cerr << " Reading  " << inputName << " ... " << std::endl;
 while (!fileC_51047.eof()){
 fileC_51047.getline (&buffer[0],1000);
 std::istrstream line(buffer);
 std::string uselessString;
 int uselessInt;
  //---- eg: -rw-r--r--   1 govoni   zh                     305501 Sep 07 18:12 0007034F-6E4F-DD11-BFD3-000423D99160.ntuple.root-50908.tree.root
 line >> uselessString; //---- -rw-r--r--
 std::cerr << "  uselessString ->  " << uselessString << std::endl;
 line >> uselessInt; //---- 1
 std::cerr << "  uselessInt ->  " << uselessInt << std::endl;
 line >> uselessString; //---- govoni
 std::cerr << "  uselessString ->  " << uselessString << std::endl;
 line >> uselessString; //---- zh
 std::cerr << "  uselessString ->  " << uselessString << std::endl;
 line >> uselessInt; //---- 305501
 std::cerr << "  uselessInt ->  " << uselessInt << std::endl;
 line >> uselessString; //---- Sep
 std::cerr << "  uselessString ->  " << uselessString << std::endl;
 line >> uselessInt; //---- 07
 std::cerr << "  uselessInt ->  " << uselessInt << std::endl;
 line >> uselessString; //---- 18:12
 std::cerr << "  uselessString ->  " << uselessString << std::endl;
 std::string treeName; 
 line >> treeName;
  //  sprintf(inputName,"/data/cosmics/%s",treeName.c_str());
 sprintf(inputName,"/data/cosmics/%s",treeName.c_str());
 std::cerr << "  File ->  " << treeName << std::endl;
 std::cerr << "  File ->  " << inputName << std::endl;
 chain->Add (inputName);
}
 */


 std::cerr << " Starting analysis ..." << std::endl; 

 //---- AM Initiate output file ----
 TFile saving (outputRootName.c_str(),"recreate") ;
//  saving.cd ();

 EcalCosmicsTreeContent treeVars ; 
 setBranchAddresses (chain, treeVars) ;
 nEntries = chain->GetEntries () ;

 TH1F dEdxSingleXtal_Tutti("dEdxSingleXtal_Tutti","dEdxSingleXtal_Tutti",1000,0,0.2);
 TH1F dEdxSingleXtal_Tutti_40("dEdxSingleXtal_Tutti_40","dEdxSingleXtal_Tutti",1000,0,0.2);
 TH1F dEdxSingleXtal_Tutti_30("dEdxSingleXtal_Tutti_30","dEdxSingleXtal_Tutti",1000,0,0.2);
 TH1F dEdxSingleXtal_Tutti_20("dEdxSingleXtal_Tutti_20","dEdxSingleXtal_Tutti",1000,0,0.2);
 TH1F dEdxSingleXtal_Tutti_10("dEdxSingleXtal_Tutti_10","dEdxSingleXtal_Tutti",1000,0,0.2);
 
 TH1F dEdxSingleXtal_Tutti_40Up("dEdxSingleXtal_Tutti_40Up","dEdxSingleXtal_Tutti",1000,0,0.2);
 TH1F dEdxSingleXtal_Tutti_30Up("dEdxSingleXtal_Tutti_30Up","dEdxSingleXtal_Tutti",1000,0,0.2);
 TH1F dEdxSingleXtal_Tutti_20Up("dEdxSingleXtal_Tutti_20Up","dEdxSingleXtal_Tutti",1000,0,0.2);
 TH1F dEdxSingleXtal_Tutti_10Up("dEdxSingleXtal_Tutti_10Up","dEdxSingleXtal_Tutti",1000,0,0.2);

 TH1F dEdxSingleXtal_Tutti_40Down("dEdxSingleXtal_Tutti_40Down","dEdxSingleXtal_Tutti",1000,0,0.2);
 TH1F dEdxSingleXtal_Tutti_30Down("dEdxSingleXtal_Tutti_30Down","dEdxSingleXtal_Tutti",1000,0,0.2);
 TH1F dEdxSingleXtal_Tutti_20Down("dEdxSingleXtal_Tutti_20Down","dEdxSingleXtal_Tutti",1000,0,0.2);
 TH1F dEdxSingleXtal_Tutti_10Down("dEdxSingleXtal_Tutti_10Down","dEdxSingleXtal_Tutti",1000,0,0.2);
 
 //TH2F dEvsdX 
 
 TH1F dEdx_Tutti("dEdx_Tutti","dEdx_Tutti",1000,0,0.2);
 TH1F dEdx_down("dEdx_down","dEdx_down",1000,0,0.2);
 TH1F dEdx_up("dEdx_up","dEdx_up",1000,0,0.2);

 TH1F dEdx_down_pos("dEdx_down_pos","dEdx_down_pos",1000,0,0.2);
 TH1F dEdx_down_neg("dEdx_down_neg","dEdx_down_neg",1000,0,0.2);

 TH1F dEdx_up_pos("dEdx_up_pos","dEdx_up_pos",1000,0,0.2);
 TH1F dEdx_up_neg("dEdx_up_neg","dEdx_up_neg",1000,0,0.2);

 TH1F dEdx_down_dietro("dEdx_down_dietro","dEdx_down_dietro",1000,0,0.2);
 TH1F dEdx_down_davanti("dEdx_down_davanti","dEdx_down_davanti",1000,0,0.2);

 TH1F dEdx_up_dietro("dEdx_up_dietro","dEdx_up_dietro",1000,0,0.2);
 TH1F dEdx_up_davanti("dEdx_up_davanti","dEdx_up_davanti",1000,0,0.2);

 TH1F dE_Tutti("dE_Tutti","dE_Tutti",1000,0,2);
 TH1F dX_Tutti("dX_Tutti","dX_Tutti",1000,0.,25.);  

 TH1F d0 ("d0", "d0", 1000, -200., 200);
 TH1F dz ("dz", "dz", 1000, -200., 200);
 
 TH1F alpha_up("alpha_up", "alpha_up", 180, 0., 180.);
 TH1F alpha_down("alpha_down", "alpha_down", 180, 0., 180.);

 TH1F muon_up("muon_up", "muon_up", 180, 0., 180.);
 TH1F muon_down("muon_down", "muon_down", 180, 0., 180.);

 //---------------------------- PERCHE' IL PRIMO NON ERA TPROFILE ??? -------------------
 TProfile2D EtaPhi_events("EtaPhi_events", "EtaPhi_events", 171, -85., 86, 360, 1., 361.);
 TProfile2D EtaPhi_alpha_xtal("EtaPhi_alpha_xtal", "EtaPhi_alpha_xtal", 171, -85., 86, 360, 1., 361.);
 TProfile2D EtaPhi_alpha_xtalMU("EtaPhi_alpha_xtalMU", "EtaPhi_alpha_xtalMU", 171, -85., 86, 360, 1., 361.);
 TProfile2D EtaPhi_alpha_MU("EtaPhi_alpha_MU", "EtaPhi_alpha_MU", 171, -85., 86, 360, 1., 361.);


 TProfile dEvsAlpha_Tutti("dEvsAlpha_Tutti","dEvsAlpha_Tutti",100, 0., 90.);
 TProfile dXvsAlpha_Tutti("dXvsAlpha_Tutti","dXvsAlpha_Tutti",100, 0., 90.);
 TH2F dEdx_alpha("dEdx_alpha", "dEdx_alpha", 100, 0., 90., 100, 0., 0.2 );
 TH2F dEdx_alpha_up("dEdx_alpha_up", "dEdx_alpha_up", 100, 0., 90., 100,0., 0.2 );
 TH2F dEdx_alpha_down("dEdx_alpha_down", "dEdx_alpha_down", 100, 0., 90., 100, 0., 0.2 );

 TProfile2D dEdx("dEdx", "dEdx",171, -85., 86, 360, 1., 361.);

 TH2F dEdxVsEnergy ("dEdxVsEnergy","dE/dx versus Crystal Energy",10000,0,2,1000,0,2);
 TH2F dEdxVsEnergy_up ("dEdxVsEnergy_up","dE/dx versus Crystal Energy #phi - up",10000,0,2,1000,0,2);
 TH2F dEdxVsEnergy_down ("dEdxVsEnergy_down","dE/dx versus Crystal Energy #phi - down",10000,0,2,1000,0,2);
 TH2F dEdx_vs_angle_Mu_Sky ("dEdx_vs_angle_Mu_Sky","dE/dx versus Angle Mu-Sky", 100, 0., 90., 100, 0., 0.2);

 TH2F dE_vs_dX ("dE_vs_dX", "dE_vs_dX", 1000, 0., 2., 1000, 0., 25.);
 TH2F dE_vs_dX_up ("dE_vs_dX_up", "dE_vs_dX_up", 1000, 0., 2., 1000, 0., 25.);
 TH2F dE_vs_dX_down ("dE_vs_dX_down", "dE_vs_dX_down", 1000, 0., 2., 1000, 0., 25.);

 TH2F dEdxVsdX ("dEdxVsdX","dE/dx versus dX",10000,0,2,1000,0,25);
 TH2F dEdxVsdXUP ("dEdxVsdXUP","dE/dx versus dX",10000,0,2,1000,0,25);
 TH2F dEdxVsdXDOWN ("dEdxVsdXDOWN","dE/dx versus dX",10000,0,2,1000,0,25);
 TH2F occupancyEnergy ("occupancyEnergy","single crystals counting Energy",180,0,360,171,-85,86) ;
 TH2F occupancy ("occupancy","single crystals counting",360,1,361,171,-85,86) ;

 TH1F dEdx_DLB("dEdx_DLB","dEdx_DLB",1000,0,0.2);
 TH1F dEdx_DLF("dEdx_DLF","dEdx_DLF",1000,0,0.2);
 TH1F dEdx_DRB("dEdx_DRB","dEdx_DRB",1000,0,0.2);
 TH1F dEdx_DRF("dEdx_DRF","dEdx_DRF",1000,0,0.2);

 TH1F dE_DLB("dE_DLB","dE_DLB",1000,0,2.);
 TH1F dE_DLF("dE_DLF","dE_DLF",1000,0,2.);
 TH1F dE_DRB("dE_DRB","dE_DRB",1000,0,2.);
 TH1F dE_DRF("dE_DRF","dE_DRF",1000,0,2.);

 TH1F dX_DLB("dX_DLB","dX_DLB",1000,0,2.);
 TH1F dX_DLF("dX_DLF","dX_DLF",1000,0,2.);
 TH1F dX_DRB("dX_DRB","dX_DRB",1000,0,2.);
 TH1F dX_DRF("dX_DRF","dX_DRF",1000,0,2.);

 TH1F dEdx_ULB("dEdx_ULB","dEdx_ULB",1000,0,0.2);
 TH1F dEdx_ULF("dEdx_ULF","dEdx_ULF",1000,0,0.2);
 TH1F dEdx_URB("dEdx_URB","dEdx_URB",1000,0,0.2);
 TH1F dEdx_URF("dEdx_URF","dEdx_URF",1000,0,0.2);

 TH1F dE_ULB("dE_ULB","dE_ULB",1000,0,2.);
 TH1F dE_ULF("dE_ULF","dE_ULF",1000,0,2.);
 TH1F dE_URB("dE_URB","dE_URB",1000,0,2.);
 TH1F dE_URF("dE_URF","dE_URF",1000,0,2.);

 TH1F dX_ULB("dX_ULB","dX_ULB",1000,0,2.);
 TH1F dX_ULF("dX_ULF","dX_ULF",1000,0,2.);
 TH1F dX_URB("dX_URB","dX_URB",1000,0,2.);
 TH1F dX_URF("dX_URF","dX_URF",1000,0,2.);

 TH1F dummyDistrib("dummyDistrib","dummyDistrib", 1000, -100., 100);

 //PG loop over entries
 for (int entry = 0 ; entry < nEntries ; ++entry)
 {
  chain->GetEntry (entry) ;
  if (entry%10000 == 0)  std::cout << "------> reading entry " << entry << " <------\n" ; 


     //fill association map muon-supercluster
  std::vector<ect::association> associations ;
  ect::fillAssocVector (associations, treeVars) ;
  ect::selectOnDR (associations, treeVars, 0.3) ;

     // find the muon associated to the Xtal-supercluster 
  for (unsigned int i = 0 ; i < associations.size () ; ++i)
  {
   int SCindex = associations.at (i).second ;
   float dummyEmax = 0.;
   float dummyLmax = 0.;
   int numCrystalEMax = -1;
   int numCrystalLMax = -1;
   bool SclOk = false;
   double dummyLength = 0;

   for (int XTLindex = treeVars.xtalIndexInSuperCluster[SCindex] ;
        XTLindex < treeVars.xtalIndexInSuperCluster[SCindex] +
          treeVars.nXtalsInSuperCluster[SCindex] ; ++XTLindex)
   {
    if(treeVars.xtalTkLength[XTLindex] == -1) continue;

    dummyLength+= treeVars.xtalTkLength[XTLindex];

    //---- check the link Xtal with max energy  == Xtal with max length ----
    if(treeVars.xtalEnergy[XTLindex] > dummyEmax) 
    {
     numCrystalEMax = XTLindex;
    }
    if(treeVars.xtalTkLength[XTLindex] > dummyLmax) {
     numCrystalLMax = XTLindex;
    }
   }

   //   if( abs(treeVars.muonTkLengthInEcalDetail[associations.at(i).first] - dummyLength) > 0.5) continue;

   if ( (numCrystalEMax != numCrystalLMax) && (numCrystalEMax != -1) && (numCrystalLMax != -1)) 
   { //---------------------- CHE TEST E' ? --------------------------------
    if(3.*treeVars.xtalEnergy[numCrystalLMax] < treeVars.xtalTkLength[numCrystalLMax] * 0.0125) SclOk = false;
   }
   else SclOk = true;
   
   if((numCrystalEMax == -1) || (numCrystalLMax == -1)) SclOk = false;

   if(SclOk == false) continue;

   dummyDistrib.Fill( treeVars.muonTkLengthInEcalDetail[associations.at(i).first] - dummyLength);


   //---- cut on min/max energy of the maximum energy crytal ----
   if ( treeVars.xtalEnergy[numCrystalLMax] < EnergyMaxCrystal_Min_Cut || treeVars.xtalEnergy[numCrystalLMax] > EnergyMaxCrystal_Max_Cut) continue;
   //---- end cut on min/max energy of the maximum energy crytal ----

   for (int XTLindex = treeVars.xtalIndexInSuperCluster[SCindex] ;
        XTLindex < treeVars.xtalIndexInSuperCluster[SCindex] +
          treeVars.nXtalsInSuperCluster[SCindex] ; ++XTLindex)
   {
    if(treeVars.xtalTkLength[XTLindex] == -1) continue;

    //---- PG find a unique index for each xtal --> rawId()
    EBDetId dummy = EBDetId::unhashIndex (treeVars.xtalHashedIndex[XTLindex]) ;


    //---- look for crossing muons (2 Scl associastions)
    //---- get Xtal direction
    TVector3 Xtal_pos (0., 0., 0.) ;
    double xtalEta = (dummy.ieta() - 0.5) * 0.0175;
    if(dummy.ieta() < 0.) xtalEta = (dummy.ieta() + 0.5) * 0.0175;
    double xtalPhi = (dummy.iphi() - 10) * 0.0175;
    if(xtalPhi > PI) xtalPhi = - 2*PI + xtalPhi;
    setVectorOnECAL (Xtal_pos, xtalEta, xtalPhi, 131.);
    //---- Crystal direction ----
    TVector3 MuonDirf (0., 1., 0.);
    float angle = MuonDirf.Angle( Xtal_pos ) ;
    if(angle > PI ) angle -= PI ;
    if( angle > PI/2. ) angle = PI - angle; // angle belongs to [0:90]
    EtaPhi_alpha_xtal.Fill(dummy.ieta(), dummy.iphi(), angle*180./PI); //--- angle between Vertical and Xtal ----

    //---- Muon and Crystal direction ----
    int MUindex = associations.at(i).first  ;
    TVector3 MuonDir (treeVars.muonMomentumX[MUindex], treeVars.muonMomentumY[MUindex], treeVars.muonMomentumZ[MUindex]);
    double angle_Mu_Xtal = MuonDir.Angle( Xtal_pos ) ;
    if(angle_Mu_Xtal > PI ) angle_Mu_Xtal -= PI ;
    if( angle_Mu_Xtal > PI/2. ) angle_Mu_Xtal = PI - angle_Mu_Xtal; // angle belongs to [0:90]

    if(dummy.iphi() < 10 || dummy.iphi() > 190) alpha_down.Fill(angle_Mu_Xtal*180. /PI);
    else alpha_up.Fill(angle_Mu_Xtal*180. /PI);
    EtaPhi_alpha_xtalMU.Fill(dummy.ieta(), dummy.iphi(), angle_Mu_Xtal*180./PI);
    dEdx_alpha.Fill(angle_Mu_Xtal*180./PI, treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
    if(dummy.iphi() < 10 || dummy.iphi() > 190) dEdx_alpha_down.Fill(angle_Mu_Xtal*180./PI, treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
    else dEdx_alpha_up.Fill(angle_Mu_Xtal*180./PI, treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
    EtaPhi_events.Fill(dummy.ieta(), dummy.iphi(), angle_Mu_Xtal*180./PI);
    dEdx.Fill(dummy.ieta(), dummy.iphi(), treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);

    //---- Muon direction ---- angle Muon / Sky ----
    TVector3 Xtal_posf (0., 1., 0.) ;
    double angle_Mu_Sky = MuonDir.Angle( Xtal_posf ) ;
    if(angle_Mu_Sky > PI ) angle_Mu_Sky -= PI ;
    if( angle_Mu_Sky > PI/2. ) angle_Mu_Sky = PI - angle_Mu_Sky; // angle belongs to [0:90]
    EtaPhi_alpha_MU.Fill(dummy.ieta(), dummy.iphi(), angle_Mu_Sky*180./PI);
    if(dummy.iphi() < 10 || dummy.iphi() > 190) muon_down.Fill(angle_Mu_Sky*180. /PI);
    else muon_up.Fill(angle_Mu_Sky*180. /PI);

    if (treeVars.xtalTkLength[XTLindex] > 0) dEdx_vs_angle_Mu_Sky.Fill(angle_Mu_Sky * 180. / PI,treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);

    //---- Cut Angle Mu-Sky ----
    if (flag_Mu_Sky_cut == true)
     if (angle_Mu_Sky * 180. / PI > 15. || angle_Mu_Sky * 180. / PI < 5. ) continue;

    //---- Cut angle Muon - XTal ---- 
    if (angle_Mu_Xtal * 180. / PI  > Cut_Angle_Mu_Xtal) continue;

    //---- Cut on Crystal Energy ---- minimum and maximum energy ----
    if (treeVars.xtalTkLength[XTLindex] > 0) {
     dEdxVsEnergy.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex],treeVars.xtalEnergy[XTLindex]);
     dEdxVsdX.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex],treeVars.xtalTkLength[XTLindex]);
     if(dummy.iphi() < 10 || dummy.iphi() > 190) {
      dEdxVsEnergy_down.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex],treeVars.xtalEnergy[XTLindex]);
      dEdxVsdXDOWN.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex],treeVars.xtalTkLength[XTLindex]);
     }
     else {
      dEdxVsdXUP.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex],treeVars.xtalTkLength[XTLindex]);
      dEdxVsEnergy_up.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex],treeVars.xtalEnergy[XTLindex]);
     }
    }
    //---- Cut on Crystal Energy ---- minimum and maximum energy per crystal ----
    double dummyEnergy = treeVars.xtalEnergy[XTLindex];
    if ( dummyEnergy < EnergyPerCrystal_Min_Cut || dummyEnergy > EnergyPerCrystal_Max_Cut) continue;


    //---- Cut on d0 parameter ----
    d0.Fill(treeVars.muond0[MUindex]);
    dz.Fill(treeVars.muondz[MUindex]);
//     std::cerr << " Mu D0 = " << treeVars.muond0[MUindex] << " Cut_D0 = " << Cut_D0;
    if (treeVars.muond0[MUindex] * treeVars.muond0[MUindex] > Cut_D0 * Cut_D0) continue;
//     std::cerr << " Passed ..." << std::endl;
    if (treeVars.muondz[MUindex] * treeVars.muondz[MUindex] > Cut_Dz * Cut_Dz) continue;

    //---- It has passed all the cuts -> fill the histograms ----

    //---- Declaration of the histograms ----
    //---- AM Single crystal ----
    std::map<int, TH1F *>::iterator dEdx_Histos_iter = dEdx_Histos.find(dummy.rawId());
    if (dEdx_Histos_iter == dEdx_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.rawId();
     std::string TH1FName = "dEdX_" +  stm.str();
     std::string TH1FNameDescription = "dE over dX -> rawId = " + stm.str();
     dEdx_Histos[dummy.rawId()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Histos[dummy.rawId()]->SetDirectory(0);
    }

    //---- AM Ring ---- dE/dx ----
    //---- up ring ----
    std::map<int, TH1F *>::iterator dEdx_Ring_1M_Up_Histos_iter = dEdx_Ring_1M_Up_Histos.find(dummy.ieta());
    if (dEdx_Ring_1M_Up_Histos_iter == dEdx_Ring_1M_Up_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dEdX_Ring_Up" +  stm.str();
     std::string TH1FNameDescription = "dE over dX Ring Up -> ieta = " + stm.str();
     dEdx_Ring_1M_Up_Histos[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Ring_1M_Up_Histos[dummy.ieta()]->SetDirectory(0);
    }
    //---- ---- + Energy Cut ----
    std::map<int, TH1F *>::iterator dEdx_Ring_1M_Up_Histos_EnergyCut_iter = dEdx_Ring_1M_Up_Histos_EnergyCut.find(dummy.ieta());
    if (dEdx_Ring_1M_Up_Histos_EnergyCut_iter == dEdx_Ring_1M_Up_Histos_EnergyCut.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dEdX_Ring_Up_EnergyCut" +  stm.str();
     std::string TH1FNameDescription = "dE over dX Ring Up EnergyCut -> ieta = " + stm.str();
     dEdx_Ring_1M_Up_Histos_EnergyCut[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Ring_1M_Up_Histos_EnergyCut[dummy.ieta()]->SetDirectory(0);
    }

    //---- down ring ----
    std::map<int, TH1F *>::iterator dEdx_Ring_1M_Down_Histos_iter = dEdx_Ring_1M_Down_Histos.find(dummy.ieta());
    if (dEdx_Ring_1M_Down_Histos_iter == dEdx_Ring_1M_Down_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dEdX_Ring_Down" +  stm.str();
     std::string TH1FNameDescription = "dE over dX Ring Down -> ieta = " + stm.str();
     dEdx_Ring_1M_Down_Histos[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Ring_1M_Down_Histos[dummy.ieta()]->SetDirectory(0);
    }
    //---- ---- + Energy Cut ----
    std::map<int, TH1F *>::iterator dEdx_Ring_1M_Down_Histos_EnergyCut_iter = dEdx_Ring_1M_Down_Histos_EnergyCut.find(dummy.ieta());
    if (dEdx_Ring_1M_Down_Histos_EnergyCut_iter == dEdx_Ring_1M_Down_Histos_EnergyCut.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dEdX_Ring_Down_EnergyCut" +  stm.str();
     std::string TH1FNameDescription = "dE over dX Ring Down EnergyCut -> ieta = " + stm.str();
     dEdx_Ring_1M_Down_Histos_EnergyCut[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Ring_1M_Down_Histos_EnergyCut[dummy.ieta()]->SetDirectory(0);
    }
    //---- all ring ----
    std::map<int, TH1F *>::iterator dEdx_Ring_Histos_iter = dEdx_Ring_Histos.find(dummy.ieta());
    if (dEdx_Ring_Histos_iter == dEdx_Ring_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dEdX_Ring" +  stm.str();
     std::string TH1FNameDescription = "dE over dX Ring -> ieta = " + stm.str();
     dEdx_Ring_Histos[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Ring_Histos[dummy.ieta()]->SetDirectory(0);
    }
    std::map<int, TH1F *>::iterator dEdx_Ring_Histos_EnergyCut_iter = dEdx_Ring_Histos_EnergyCut.find(dummy.ieta());
    if (dEdx_Ring_Histos_EnergyCut_iter == dEdx_Ring_Histos_EnergyCut.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dEdX_Ring_EnergyCut" +  stm.str();
     std::string TH1FNameDescription = "dE over dX Ring EnergyCut-> ieta = " + stm.str();
     dEdx_Ring_Histos_EnergyCut[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Ring_Histos_EnergyCut[dummy.ieta()]->SetDirectory(0);
    }

    //---- Different Supermodules ----
    //---- Up Right ----
    std::map<int, TH1F *>::iterator dEdx_Ring_UR_Histos_iter = dEdx_Ring_UR_Histos.find(dummy.ieta());
    if (dEdx_Ring_UR_Histos_iter == dEdx_Ring_UR_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dEdx_Ring_UR_Histos" +  stm.str();
     std::string TH1FNameDescription = "dE over dX Ring Up Right -> ieta = " + stm.str();
     dEdx_Ring_UR_Histos[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Ring_UR_Histos[dummy.ieta()]->SetDirectory(0);
    }
    //---- Up Left ----
    std::map<int, TH1F *>::iterator dEdx_Ring_UL_Histos_iter = dEdx_Ring_UL_Histos.find(dummy.ieta());
    if (dEdx_Ring_UL_Histos_iter == dEdx_Ring_UL_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dEdx_Ring_UL_Histos" +  stm.str();
     std::string TH1FNameDescription = "dE over dX Ring Up Left -> ieta = " + stm.str();
     dEdx_Ring_UL_Histos[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Ring_UL_Histos[dummy.ieta()]->SetDirectory(0);
    }
    //---- Down Right ----
    std::map<int, TH1F *>::iterator dEdx_Ring_DR_Histos_iter = dEdx_Ring_DR_Histos.find(dummy.ieta());
    if (dEdx_Ring_DR_Histos_iter == dEdx_Ring_DR_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dEdx_Ring_DR_Histos" +  stm.str();
     std::string TH1FNameDescription = "dE over dX Ring Down Right -> ieta = " + stm.str();
     dEdx_Ring_DR_Histos[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Ring_DR_Histos[dummy.ieta()]->SetDirectory(0);
    }
    //---- Down Left ----
    std::map<int, TH1F *>::iterator dEdx_Ring_DL_Histos_iter = dEdx_Ring_DL_Histos.find(dummy.ieta());
    if (dEdx_Ring_DL_Histos_iter == dEdx_Ring_DL_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dEdx_Ring_DL_Histos" +  stm.str();
     std::string TH1FNameDescription = "dE over dX Ring Down Left -> ieta = " + stm.str();
     dEdx_Ring_DL_Histos[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dEdx_Ring_DL_Histos[dummy.ieta()]->SetDirectory(0);
    }

    //---- dE ----
    std::map<int, TH1F *>::iterator dE_Histos_iter = dE_Histos.find(dummy.rawId());
    if (dE_Histos_iter == dE_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.rawId();
     std::string TH1FName = "dE_" +  stm.str();
     std::string TH1FNameDescription = "dE -> rawId = " + stm.str();
     dE_Histos[dummy.rawId()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dE_Histos[dummy.rawId()]->SetDirectory(0);
    }

    //---- AM ---- Ring ---- dE ----
    //---- up ring ----
    std::map<int, TH1F *>::iterator dE_Ring_1M_Up_Histos_iter = dE_Ring_1M_Up_Histos.find(dummy.ieta());
    if (dE_Ring_1M_Up_Histos_iter == dE_Ring_1M_Up_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dE_Ring_Up" +  stm.str();
     std::string TH1FNameDescription = "dE Ring Up -> ieta = " + stm.str();
     dE_Ring_1M_Up_Histos[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dE_Ring_1M_Up_Histos[dummy.ieta()]->SetDirectory(0);
    }
    //---- down ring ----
    std::map<int, TH1F *>::iterator dE_Ring_1M_Down_Histos_iter = dE_Ring_1M_Down_Histos.find(dummy.ieta());
    if (dE_Ring_1M_Down_Histos_iter == dE_Ring_1M_Down_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dE_Ring_Down" +  stm.str();
     std::string TH1FNameDescription = "dE Ring Down -> ieta = " + stm.str();
     dE_Ring_1M_Down_Histos[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dE_Ring_1M_Down_Histos[dummy.ieta()]->SetDirectory(0);
    }
    //---- all ring ----
    std::map<int, TH1F *>::iterator dE_Ring_Histos_iter = dE_Ring_Histos.find(dummy.ieta());
    if (dE_Ring_Histos_iter == dE_Ring_Histos.end()) {
     std::ostringstream stm;
     stm << dummy.ieta();
     std::string TH1FName = "dE_Ring" +  stm.str();
     std::string TH1FNameDescription = "dE Ring -> ieta = " + stm.str();
     dE_Ring_Histos[dummy.ieta()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),100,0,0.2);
     dE_Ring_Histos[dummy.ieta()]->SetDirectory(0);
    }

    //---- Fill the histograms ----
    //---- PG fill the histo with the E/L for the xtal
    dEdx_Tutti.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
    if(dummy.iphi() < 10 || dummy.iphi() > 190) {
     dEdx_down.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
     if(dummy.iphi()>280)dEdx_down_dietro.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
     else if(dummy.iphi() < 280)dEdx_down_davanti.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
     if(dummy.ieta() > 0. && dummy.ieta() < 40.) dEdx_down_pos.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
     else if(dummy.ieta() < 0. && dummy.ieta() > -40.)dEdx_down_neg.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
    }
    else {
     dEdx_up.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
     if(dummy.iphi()>80)dEdx_up_davanti.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
     else if(dummy.iphi() < 80)dEdx_up_dietro.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
     if(dummy.ieta() > 0. && dummy.ieta() < 40.) dEdx_up_pos.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
     else if(dummy.ieta() < 0. && dummy.ieta() > -40.)dEdx_up_neg.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
    }

    //---- AM ---- dE/dx ---- Single crystal ----
    dEdx_Histos[dummy.rawId()]->Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
    //---- AM ---- dE ---- Single crystal ----
    dE_Histos[dummy.rawId()]->Fill(treeVars.xtalEnergy[XTLindex]);
    //---- AM Ring ---- dE/dx ----
    dEdx_Ring_Histos[dummy.ieta()]->Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
    if (dummy.iphi() < 120 && dummy.iphi() > 80) 
     dEdx_Ring_1M_Up_Histos[dummy.ieta()]->Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
    if (dummy.iphi() < 300 && dummy.iphi() > 260) 
     dEdx_Ring_1M_Down_Histos[dummy.ieta()]->Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);

    dE_Ring_Histos[dummy.ieta()]->Fill(treeVars.xtalEnergy[XTLindex]);
    if (dummy.iphi() < 120 && dummy.iphi() > 80) 
     dE_Ring_1M_Up_Histos[dummy.ieta()]->Fill(treeVars.xtalEnergy[XTLindex]);
    if (dummy.iphi() < 300 && dummy.iphi() > 260) 
     dE_Ring_1M_Down_Histos[dummy.ieta()]->Fill(treeVars.xtalEnergy[XTLindex]);

    dE_vs_dX.Fill(treeVars.xtalEnergy[XTLindex], treeVars.xtalTkLength[XTLindex]);
    if (dummy.iphi() < 120 && dummy.iphi() > 80) 
     dE_vs_dX_up.Fill(treeVars.xtalEnergy[XTLindex], treeVars.xtalTkLength[XTLindex]);
    if (dummy.iphi() < 300 && dummy.iphi() > 260) 
     dE_vs_dX_down.Fill(treeVars.xtalEnergy[XTLindex], treeVars.xtalTkLength[XTLindex]);

    //---- Up / Down ---- Left / Right ----
    if (dummy.iphi() < 100 && dummy.iphi() > 80){
     dEdx_Ring_UL_Histos[dummy.ieta()]->Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() > 0 && dummy.ieta() < 40) dEdx_ULB.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dEdx_ULF.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);

     if (dummy.ieta() > 0 && dummy.ieta() < 40) dE_ULB.Fill(treeVars.xtalEnergy[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dE_ULF.Fill(treeVars.xtalEnergy[XTLindex]);

     if (dummy.ieta() > 0 && dummy.ieta() < 40) dX_ULB.Fill(treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dX_ULF.Fill(treeVars.xtalTkLength[XTLindex]);
    }
    if (dummy.iphi() < 120 && dummy.iphi() > 100){
     dEdx_Ring_UR_Histos[dummy.ieta()]->Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);

     if (dummy.ieta() > 0 && dummy.ieta() < 40) dEdx_URB.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dEdx_URF.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);

     if (dummy.ieta() > 0 && dummy.ieta() < 40) dE_URB.Fill(treeVars.xtalEnergy[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dE_URF.Fill(treeVars.xtalEnergy[XTLindex]);

     if (dummy.ieta() > 0 && dummy.ieta() < 40) dX_URB.Fill(treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dX_URF.Fill(treeVars.xtalTkLength[XTLindex]);
    }

    if (dummy.iphi() < 280 && dummy.iphi() > 260){ 
     dEdx_Ring_DL_Histos[dummy.ieta()]->Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() > 0 && dummy.ieta() < 40) dEdx_DLB.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dEdx_DLF.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);

     if (dummy.ieta() > 0 && dummy.ieta() < 40) dE_DLB.Fill(treeVars.xtalEnergy[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dE_DLF.Fill(treeVars.xtalEnergy[XTLindex]);

     if (dummy.ieta() > 0 && dummy.ieta() < 40) dX_DLB.Fill(treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dX_DLF.Fill(treeVars.xtalTkLength[XTLindex]);
    }
    if (dummy.iphi() < 300 && dummy.iphi() > 280){
     dEdx_Ring_DR_Histos[dummy.ieta()]->Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() > 0 && dummy.ieta() < 40) dEdx_DRB.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dEdx_DRF.Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);

     if (dummy.ieta() > 0 && dummy.ieta() < 40) dE_DRB.Fill(treeVars.xtalEnergy[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dE_DRF.Fill(treeVars.xtalEnergy[XTLindex]);

     if (dummy.ieta() > 0 && dummy.ieta() < 40) dX_DRB.Fill(treeVars.xtalTkLength[XTLindex]);
     if (dummy.ieta() < 0 && dummy.ieta() > -40) dX_DRF.Fill(treeVars.xtalTkLength[XTLindex]);
    }

    dE_Tutti.Fill(treeVars.xtalEnergy[XTLindex]);
    dX_Tutti.Fill(treeVars.xtalTkLength[XTLindex]);
    occupancy.Fill (dummy.iphi (), dummy.ieta ());
    occupancyEnergy.Fill (dummy.iphi (),dummy.ieta (),treeVars.xtalEnergy[XTLindex]);
   }
  }
 } //PG loop over entries



 //---- Analyze filled histograms ----
 //---- dE/dx map from the fit ---- single crystal ----
 std::map<int, double> XtalCoeff_map ;
 std::map<int, double> XtalCoeffError_map ;

 TH2F dEdXEtaPhi ("dEdXEtaPhi","dEdXEtaPhi",360,1,361,171,-85,86) ;
 int MinXtal_cut = 20;
//  for (MinXtal_cut = 10; MinXtal_cut<100; MinXtal_cut++){
//  //AM ---- loop over dEdx_Histos ---- single crystal ----
//   for (std::map<int, TH1F*>::iterator mapIt = dEdx_Histos.begin (); mapIt != dEdx_Histos.end ();++mapIt)
//   {
//      //   mapIt->second->Fit ("gaus") ;
//    if( (mapIt->second->GetEntries()) > MinXtal_cut ){
//     std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
//     mapIt->second->Write();
//     double mean_pair = MeanAndErrorPair.first;
//     double Error_pair = MeanAndErrorPair.second;
//     XtalCoeff_map[mapIt->first] = mean_pair;
//     XtalCoeffError_map[mapIt->first] = Error_pair;
//    //---- fill histogram ----
//     EBDetId dummyMy(mapIt->first);
//     std::cerr << " iphi =  " << dummyMy.iphi() << " ieta = " << dummyMy.ieta() << " -> dEdX = " << mean_pair << std::endl;
//     dEdXEtaPhi.SetBinContent(dummyMy.iphi(),dummyMy.ieta(),mean_pair);
//    }
//    else {
//     XtalCoeff_map[mapIt->first] = 0;
//     XtalCoeffError_map[mapIt->first] = 0;
//    }
//   }  //AM ---- end loop over dEdx_Histos single crystal ----
 // 
//   char titleGraph[1000];
//   sprintf(titleGraph,"MinXtal_cut=%i",MinXtal_cut);
//   dEdXEtaPhi.SetTitle(titleGraph);
//   dEdXEtaPhi.Draw("colz");
//   gPad->Update();
//   gPad->SaveAs("3D.gif+");
//   gPad->SaveAs("3D.gif+");
//   gPad->SaveAs("3D.gif+");
//   gPad->SaveAs("3D.gif+");
//   gPad->SaveAs("3D.gif+");
//   gPad->SaveAs("3D.gif+");
 //   
//   for (int iieta = -85; iieta<86; iieta++)
//    for (int iiphi = 0; iiphi<361; iiphi++)
//     dEdXEtaPhi.SetBinContent(iiphi,iieta,0);
//  }
 //  
 
 MinXtal_cut = 30;
 for (std::map<int, TH1F*>::iterator mapIt = dEdx_Histos.begin (); mapIt != dEdx_Histos.end ();++mapIt)
 {
     //   mapIt->second->Fit ("gaus") ;
  if( (mapIt->second->GetEntries()) > MinXtal_cut ){
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1dEdX) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   XtalCoeff_map[mapIt->first] = mean_pair;
   XtalCoeffError_map[mapIt->first] = Error_pair;
   //---- fill histogram ----
   EBDetId dummyMy(mapIt->first);
//    std::cerr << " iphi =  " << dummyMy.iphi() << " ieta = " << dummyMy.ieta() << " -> dEdX = " << mean_pair << std::endl;
   dEdXEtaPhi.Fill(dummyMy.iphi(),dummyMy.ieta(),mean_pair);
   dEdxSingleXtal_Tutti.Fill(mean_pair);
   
   if (((dummyMy.iphi() < 300 && dummyMy.iphi() > 260) || (dummyMy.iphi() < 120 && dummyMy.iphi() > 80)) && dummyMy.ieta() * dummyMy.ieta() < 40*40)
     dEdxSingleXtal_Tutti_40.Fill(mean_pair);
   if (((dummyMy.iphi() < 300 && dummyMy.iphi() > 260) || (dummyMy.iphi() < 120 && dummyMy.iphi() > 80)) && dummyMy.ieta() * dummyMy.ieta() < 30*30) 
     dEdxSingleXtal_Tutti_30.Fill(mean_pair);
   if (((dummyMy.iphi() < 300 && dummyMy.iphi() > 260) || (dummyMy.iphi() < 120 && dummyMy.iphi() > 80)) && dummyMy.ieta() * dummyMy.ieta() < 20*20) 
     dEdxSingleXtal_Tutti_20.Fill(mean_pair);
   if (((dummyMy.iphi() < 300 && dummyMy.iphi() > 260) || (dummyMy.iphi() < 120 && dummyMy.iphi() > 80)) && dummyMy.ieta() * dummyMy.ieta() < 10*10) 
     dEdxSingleXtal_Tutti_10.Fill(mean_pair);
   
   
   
   if (((dummyMy.iphi() < 300 && dummyMy.iphi() > 260)) && dummyMy.ieta() * dummyMy.ieta() < 40*40) dEdxSingleXtal_Tutti_40Down.Fill(mean_pair);
   if (((dummyMy.iphi() < 300 && dummyMy.iphi() > 260)) && dummyMy.ieta() * dummyMy.ieta() < 30*30) dEdxSingleXtal_Tutti_30Down.Fill(mean_pair);
   if (((dummyMy.iphi() < 300 && dummyMy.iphi() > 260)) && dummyMy.ieta() * dummyMy.ieta() < 20*20) dEdxSingleXtal_Tutti_20Down.Fill(mean_pair);
   if (((dummyMy.iphi() < 300 && dummyMy.iphi() > 260)) && dummyMy.ieta() * dummyMy.ieta() < 10*10) dEdxSingleXtal_Tutti_10Down.Fill(mean_pair);
   
   
   if (((dummyMy.iphi() < 120 && dummyMy.iphi() > 80)) && dummyMy.ieta() * dummyMy.ieta() < 40*40) dEdxSingleXtal_Tutti_40Up.Fill(mean_pair);
   if (((dummyMy.iphi() < 120 && dummyMy.iphi() > 80)) && dummyMy.ieta() * dummyMy.ieta() < 30*30) dEdxSingleXtal_Tutti_30Up.Fill(mean_pair);
   if (((dummyMy.iphi() < 120 && dummyMy.iphi() > 80)) && dummyMy.ieta() * dummyMy.ieta() < 20*20) dEdxSingleXtal_Tutti_20Up.Fill(mean_pair);
   if (((dummyMy.iphi() < 120 && dummyMy.iphi() > 80)) && dummyMy.ieta() * dummyMy.ieta() < 10*10) dEdxSingleXtal_Tutti_10Up.Fill(mean_pair);
   
   
   
  }
  else {
   XtalCoeff_map[mapIt->first] = 0;
   XtalCoeffError_map[mapIt->first] = 0;
  }
 }  //AM ---- end loop over dEdx_Histos single crystal ----

 //---- dE/dx slice of the Ring ----
 std::map<int, double> RingCoeffUp_map ;
 std::map<int, double> RingCoeffUpError_map ;

 std::map<int, double> RingCoeffDown_map ;
 std::map<int, double> RingCoeffDownError_map ;

 //AM ---- loop over dEdx_Histos ---- Slice Ring ----
 //---- UP ----
 for (std::map<int, TH1F*>::iterator mapIt = dEdx_Ring_1M_Up_Histos.begin (); mapIt != dEdx_Ring_1M_Up_Histos.end ();++mapIt)
 {
  if (mapIt->second->GetEntries() > 15) {
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   RingCoeffUp_map[mapIt->first] = mean_pair;
   RingCoeffUpError_map[mapIt->first] = Error_pair;
  }
  else {
   RingCoeffUp_map[mapIt->first] = 0;
   RingCoeffUpError_map[mapIt->first] = 0;
  }
 }
 //---- DOWN ----
 for (std::map<int, TH1F*>::iterator mapIt = dEdx_Ring_1M_Down_Histos.begin (); mapIt != dEdx_Ring_1M_Down_Histos.end ();++mapIt)
 {
  if (mapIt->second->GetEntries() > 15) {
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   RingCoeffDown_map[mapIt->first] = mean_pair;
   RingCoeffDownError_map[mapIt->first] = Error_pair;
  }
  else {
   RingCoeffDown_map[mapIt->first] = 0;
   RingCoeffDownError_map[mapIt->first] = 0;
  }
 } //AM ---- end loop over dEdx_Histos ---- Slice Ring ----

 //---- TGraphErrors UP ----
 TGraphErrors CoeffUP;
 CoeffUP.SetName("CoeffUP");
 CoeffUP.SetTitle("CoeffUP");
 int counterCoeffUP = 0;
 for (int kk=0; kk<171;kk++){
  int numberIEta = kk - 85;
  std::map<int, double>::iterator RingCoeffUp_map_iter = RingCoeffUp_map.find(numberIEta);
  std::map<int, double>::iterator RingCoeffUpError_map_iter = RingCoeffUpError_map.find(numberIEta);
  if (RingCoeffUp_map_iter != RingCoeffUp_map.end()){
   CoeffUP.SetPoint(counterCoeffUP,numberIEta,RingCoeffUp_map_iter->second);
   CoeffUP.SetPointError(counterCoeffUP,0,RingCoeffUpError_map_iter->second);
   counterCoeffUP++;
  }
  else {
   CoeffUP.SetPoint(counterCoeffUP,numberIEta,0);
   CoeffUP.SetPointError(counterCoeffUP,0,0);
   counterCoeffUP++;
  }
 }

 //---- TGraphErrors DOWN ----
 TGraphErrors CoeffDOWN;
 CoeffDOWN.SetName("CoeffDOWN");
 CoeffDOWN.SetTitle("CoeffDOWN");
 int counterCoeffDOWN = 0;
 for (int kk=0; kk<171;kk++){
  int numberIEta = kk - 85;
  std::map<int, double>::iterator RingCoeffDown_map_iter = RingCoeffDown_map.find(numberIEta);
  std::map<int, double>::iterator RingCoeffDownError_map_iter = RingCoeffDownError_map.find(numberIEta);
  if (RingCoeffDown_map_iter != RingCoeffDown_map.end()){
   CoeffDOWN.SetPoint(counterCoeffDOWN,numberIEta,RingCoeffDown_map_iter->second);
   CoeffDOWN.SetPointError(counterCoeffDOWN,0,RingCoeffDownError_map_iter->second);
   counterCoeffDOWN++;
  }
  else {
   CoeffDOWN.SetPoint(counterCoeffDOWN,numberIEta,0);
   CoeffDOWN.SetPointError(counterCoeffDOWN,0,0);
   counterCoeffDOWN++;
  }
 }


//  //---- TGraphErrors COeff vs Eta ----
//  TGraphErrors CoeffEtaTrend;
//  CoeffEtaTrend.SetName("CoeffEtaTrend");
//  CoeffEtaTrend.SetTitle("CoeffEtaTrend");
//  int counterCoeffEtaTrend = 0;
//  for (int kk=0; kk<171;kk++){
//   int numberIEta = kk - 85;
//   std::map<int, double>::iterator RingCoeffDown_map_iter = RingCoeffDown_map.find(numberIEta);
//   std::map<int, double>::iterator RingCoeffDownError_map_iter = RingCoeffDownError_map.find(numberIEta);
//   if (RingCoeffDown_map_iter != RingCoeffDown_map.end()){
//    CoeffDOWN.SetPoint(counterCoeffDOWN,numberIEta,RingCoeffDown_map_iter->second);
//    CoeffDOWN.SetPointError(counterCoeffDOWN,0,RingCoeffDownError_map_iter->second);
//    counterCoeffEtaTrend++;
//   }
//   else {
//    CoeffDOWN.SetPoint(counterCoeffEtaTrend,numberIEta,0);
//    CoeffDOWN.SetPointError(counterCoeffEtaTrend,0,0);
//    counterCoeffEtaTrend++;
//   }
//  }



 //---- dE/dx slice of the Ring ---- Up / Down ---- Left / Right ----

//  std::map<int, double> RingCoeffUR_map ;
//  std::map<int, double> RingCoeffURError_map ;
//  std::map<int, double> RingCoeffUL_map ;
//  std::map<int, double> RingCoeffULError_map ;
 //
//  std::map<int, double> RingCoeffDR_map ;
//  std::map<int, double> RingCoeffDRError_map ;
//  std::map<int, double> RingCoeffDL_map ;
//  std::map<int, double> RingCoeffDLError_map ;

 int counterTOT = 0;

 TGraphErrors CoeffUL;
 CoeffUL.SetName("CoeffUL");
 CoeffUL.SetTitle("CoeffUL");

 TGraphErrors CoeffUR;
 CoeffUR.SetName("CoeffUR");
 CoeffUR.SetTitle("CoeffUR");

 TGraphErrors CoeffDL;
 CoeffDL.SetName("CoeffDL");
 CoeffDL.SetTitle("CoeffDL");

 TGraphErrors CoeffDR;
 CoeffDR.SetName("CoeffDR");
 CoeffDR.SetTitle("CoeffDR");

 //AM ---- loop over dEdx_Histos ---- Slice Ring ----
 //---- Up Left ----
 counterTOT = 0;
 for (std::map<int, TH1F*>::iterator mapIt = dEdx_Ring_UL_Histos.begin (); mapIt != dEdx_Ring_UL_Histos.end ();++mapIt)
 {
  if (mapIt->second->GetEntries() > 15) {
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   int numberIEta = mapIt->first;
   CoeffUL.SetPoint(counterTOT,numberIEta,mean_pair);
   CoeffUL.SetPointError(counterTOT,0,Error_pair);
   counterTOT++;
  }
 }

  //---- Up Right ----
 counterTOT = 0;
 for (std::map<int, TH1F*>::iterator mapIt = dEdx_Ring_UR_Histos.begin (); mapIt != dEdx_Ring_UR_Histos.end ();++mapIt)
 {
  if (mapIt->second->GetEntries() > 15) {
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   int numberIEta = mapIt->first;
   CoeffUR.SetPoint(counterTOT,numberIEta,mean_pair);
   CoeffUR.SetPointError(counterTOT,0,Error_pair);
   counterTOT++;
  }
 }


  //---- Down Left ----
 counterTOT = 0;
 for (std::map<int, TH1F*>::iterator mapIt = dEdx_Ring_DL_Histos.begin (); mapIt != dEdx_Ring_DL_Histos.end ();++mapIt)
 {
  if (mapIt->second->GetEntries() > 15) {
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   int numberIEta = mapIt->first;
   CoeffDL.SetPoint(counterTOT,numberIEta,mean_pair);
   CoeffDL.SetPointError(counterTOT,0,Error_pair);
   counterTOT++;
  }
 }

  //---- Down Right ----
 counterTOT = 0;
 for (std::map<int, TH1F*>::iterator mapIt = dEdx_Ring_DR_Histos.begin (); mapIt != dEdx_Ring_DR_Histos.end ();++mapIt)
 {
  if (mapIt->second->GetEntries() > 15) {
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   int numberIEta = mapIt->first;
   CoeffDR.SetPoint(counterTOT,numberIEta,mean_pair);
   CoeffDR.SetPointError(counterTOT,0,Error_pair);
   counterTOT++;
  }
 }

 //------------------------------------------------
 //---------------------- dE ----------------------
 //------------------------------------------------

 //---- dE map from the fit ---- single crystal ----
 std::map<int, double> dEXtalCoeff_map ;
 std::map<int, double> dEXtalCoeffError_map ;

 TH2F dEEtaPhi ("dEEtaPhi","dEEtaPhi",360,1,361,171,-85,86) ;

 MinXtal_cut = 20;
 //AM ---- loop over dE_Histos single crystal ----
 for (std::map<int, TH1F*>::iterator mapIt = dE_Histos.begin (); mapIt != dE_Histos.end ();++mapIt)
 {
  if( (mapIt->second->GetEntries()) > MinXtal_cut ){
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   dEXtalCoeff_map[mapIt->first] = mean_pair;
   dEXtalCoeffError_map[mapIt->first] = Error_pair;
   //---- fill histogram ----
   EBDetId dummyMy(mapIt->first);
//    std::cerr << " iphi =  " << dummyMy.iphi() << " ieta = " << dummyMy.ieta() << " -> dE = " << mean_pair << std::endl;
   dEEtaPhi.Fill(dummyMy.iphi(),dummyMy.ieta(),mean_pair);
  }
  else {
   dEXtalCoeff_map[mapIt->first] = 0;
   dEXtalCoeffError_map[mapIt->first] = 0;
  }
 }  //AM ---- end loop over dE_Histos single crystal ----

  //---- dE slice of the Ring ----
 std::map<int, double> dERingCoeffUp_map ;
 std::map<int, double> dERingCoeffUpError_map ;

 std::map<int, double> dERingCoeffDown_map ;
 std::map<int, double> dERingCoeffDownError_map ;

 //AM ---- loop over dE_Histos ---- Slice Ring ----
 //---- UP ----
 for (std::map<int, TH1F*>::iterator mapIt = dE_Ring_1M_Up_Histos.begin (); mapIt != dE_Ring_1M_Up_Histos.end ();++mapIt)
 {
  if (mapIt->second->GetEntries() > 15) {
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   dERingCoeffUp_map[mapIt->first] = mean_pair;
   dERingCoeffUpError_map[mapIt->first] = Error_pair;
  }
  else {
   dERingCoeffUp_map[mapIt->first] = 0;
   dERingCoeffUpError_map[mapIt->first] = 0;
  }
 }
 //---- DOWN ----
 for (std::map<int, TH1F*>::iterator mapIt = dE_Ring_1M_Down_Histos.begin (); mapIt != dE_Ring_1M_Down_Histos.end ();++mapIt)
 {
  if (mapIt->second->GetEntries() > 15) {
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   dERingCoeffDown_map[mapIt->first] = mean_pair;
   dERingCoeffDownError_map[mapIt->first] = Error_pair;
  }
  else {
   dERingCoeffDown_map[mapIt->first] = 0;
   dERingCoeffDownError_map[mapIt->first] = 0;
  }
 } //AM ---- end loop over dE_Histos ---- Slice Ring ----

 //---- TGraphErrors UP ----
 TGraphErrors dECoeffUP;
 dECoeffUP.SetName("dECoeffUP");
 dECoeffUP.SetTitle("dECoeffUP");
 int dECounterCoeffUP = 0;
 for (int kk=0; kk<171;kk++){
  int numberIEta = kk - 85;
  std::map<int, double>::iterator dERingCoeffUp_map_iter = dERingCoeffUp_map.find(numberIEta);
  std::map<int, double>::iterator dERingCoeffUpError_map_iter = dERingCoeffUpError_map.find(numberIEta);
  if (dERingCoeffUp_map_iter != dERingCoeffUp_map.end()){
   dECoeffUP.SetPoint(dECounterCoeffUP,numberIEta,dERingCoeffUp_map_iter->second);
   dECoeffUP.SetPointError(dECounterCoeffUP,0,dERingCoeffUpError_map_iter->second);
   dECounterCoeffUP++;
  }
  else {
   dECoeffUP.SetPoint(dECounterCoeffUP,numberIEta,0);
   dECoeffUP.SetPointError(dECounterCoeffUP,0,0);
   dECounterCoeffUP++;
  }
 }

 //---- TGraphErrors DOWN ----
 TGraphErrors dECoeffDOWN;
 dECoeffDOWN.SetName("dECoeffDOWN");
 dECoeffDOWN.SetTitle("dECoeffDOWN");
 int dECounterCoeffDOWN = 0;
 for (int kk=0; kk<171;kk++){
  int numberIEta = kk - 85;
  std::map<int, double>::iterator dERingCoeffDown_map_iter = dERingCoeffDown_map.find(numberIEta);
  std::map<int, double>::iterator dERingCoeffDownError_map_iter = dERingCoeffDownError_map.find(numberIEta);
  if (dERingCoeffDown_map_iter != dERingCoeffDown_map.end()){
   dECoeffDOWN.SetPoint(dECounterCoeffDOWN,numberIEta,dERingCoeffDown_map_iter->second);
   dECoeffDOWN.SetPointError(dECounterCoeffDOWN,0,dERingCoeffDownError_map_iter->second);
   dECounterCoeffDOWN++;
  }
  else {
   dECoeffDOWN.SetPoint(dECounterCoeffDOWN,numberIEta,0);
   dECoeffDOWN.SetPointError(dECounterCoeffDOWN,0,0);
   dECounterCoeffDOWN++;
  }
 }


 //---- dE map from the fit ---- All Ring ----
 std::map<int, double> dERingCoeff_map ;
 std::map<int, double> dERingCoeffError_map ;
 
 //AM ---- loop over dE_Histos ---- Ring ----
 for (std::map<int, TH1F*>::iterator mapIt = dE_Ring_Histos.begin (); mapIt != dE_Ring_Histos.end ();++mapIt)
 {
  if (mapIt->second->GetEntries() > 15) {
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;
   dERingCoeff_map[mapIt->first] = mean_pair;
   dERingCoeffError_map[mapIt->first] = Error_pair;
  }
  else {
   dERingCoeff_map[mapIt->first] = 0;
   dERingCoeffError_map[mapIt->first] = 0;
  }
 }  //AM ---- end loop over dE_Histos Ring ----

 TGraphErrors dECoeff;
 dECoeff.SetName("dECoeff");
 dECoeff.SetTitle("dECoeff");
 int dECounterCoeff = 0;
 for (int kk=0; kk<171;kk++){
  int numberIEta = kk - 85;
  std::map<int, double>::iterator dERingCoeff_map_iter = dERingCoeff_map.find(numberIEta);
  std::map<int, double>::iterator dERingCoeffError_map_iter = dERingCoeffError_map.find(numberIEta);
  if (dERingCoeff_map_iter != dERingCoeff_map.end()){
   dECoeff.SetPoint(dECounterCoeff,numberIEta,dERingCoeff_map_iter->second);
   dECoeff.SetPointError(dECounterCoeff,0,dERingCoeffError_map_iter->second);
   dECounterCoeff++;
  }
  else {
   dECoeff.SetPoint(dECounterCoeff,numberIEta,0);
   dECoeff.SetPointError(dECounterCoeff,0,0);
   dECounterCoeff++;
  }
 }

 //----------------------------------------------------
 //---------------------- End dE ----------------------
 //----------------------------------------------------

 //---- dE/dx map from the fit ---- Ring ----
 std::map<int, double> RingCoeff_map ;
 std::map<int, double> RingCoeffError_map ;

 //AM ---- loop over dEdx_Histos ---- Ring ----
 for (std::map<int, TH1F*>::iterator mapIt = dEdx_Ring_Histos.begin (); mapIt != dEdx_Ring_Histos.end ();++mapIt)
 {
  // get the peak of dEondX (da controllare con piu' statistica anche i "puntatori")
  if (mapIt->second->GetEntries() > 15) {
   std::pair<double,double> MeanAndErrorPair = fitdEdx(mapIt->second);
   if (WriteTH1F) mapIt->second->Write();
   double mean_pair = MeanAndErrorPair.first;
   double Error_pair = MeanAndErrorPair.second;

   RingCoeff_map[mapIt->first] = mean_pair;
   RingCoeffError_map[mapIt->first] = Error_pair;
  }
  else {
   RingCoeff_map[mapIt->first] = 0;
   RingCoeffError_map[mapIt->first] = 0;
  }
 }  //AM ---- end loop over dEdx_Histos Ring ----




 //---- transform a map in histogram ----

 std::cerr << "     Creo TProfile ...." << std::endl;

 TProfile Coeff("Coeff","Coefficients in the ring",171, -85, 85);
 Double_t Xcoeff[171];
 Double_t errXcoeff[171];
 Double_t Ycoeff[171];
 Double_t errYcoeff[171];

 for (int kk=0; kk<171;kk++){
  int numberIEta = kk - 85;
  Xcoeff[kk] = numberIEta;
  errXcoeff[kk] = 0;
  std::map<int, double>::iterator RingCoeff_map_iter = RingCoeff_map.find(numberIEta);
  std::map<int, double>::iterator RingCoeffError_map_iter = RingCoeffError_map.find(numberIEta);
  if (RingCoeff_map_iter != RingCoeff_map.end()){
//    std::cerr << "   Sono entrato con ieta = " << numberIEta << std::endl;
   Ycoeff[kk] = RingCoeff_map_iter->second;
//    std::cerr << "   Y =  " << Ycoeff[kk];
   errYcoeff[kk] = RingCoeffError_map_iter->second;
  }
  else {
   Ycoeff[kk] = 0;
   errYcoeff[kk] = 0;
  }
 }

 std::cerr << "     Creo TGraphErrors ...." << std::endl;

 TGraphErrors TGraphErrorsCoefficients;
 TGraphErrorsCoefficients.SetName("TGraphErrorsCoefficients");
 TGraphErrorsCoefficients.SetTitle("TGraphErrorsCoefficients");
 int counterCoeff = 0;
 for (int kk=0; kk<171;kk++){
  TGraphErrorsCoefficients.SetPoint(counterCoeff,Xcoeff[kk],Ycoeff[kk]);
  TGraphErrorsCoefficients.SetPointError(counterCoeff,errXcoeff[kk],errYcoeff[kk]);
  counterCoeff++;
//   std::cerr << "     numberIEta = " << Xcoeff[kk] << " Ycoeff[kk] = " << Ycoeff[kk];
//   std::cerr << "   errYcoeff[numberIEta] = " << errYcoeff[kk] << std::endl;
 }



 for (std::map<int, TH1F*>::iterator mapIt = dEdx_Ring_Histos.begin (); mapIt != dEdx_Ring_Histos.end ();++mapIt)
 {
  Coeff.Fill(mapIt->first,RingCoeff_map[mapIt->first]);
 }

 //---- Saving histograms ----
 //---- dEdx_Histos.

 d0.GetXaxis()->SetTitle("d0");
 d0.Write();
 
 dz.GetXaxis()->SetTitle("dz");
 dz.Write();

 dEdx_Tutti.GetXaxis()->SetTitle("dE/dX");
 dEdx_Tutti.Write();

 dEdx_down.GetXaxis()->SetTitle("dE/dX");
 dEdx_down.Write();

 dEdx_up.GetXaxis()->SetTitle("dE/dX");
 dEdx_up.Write();

 dEdx_down_pos.GetXaxis()->SetTitle("dE/dX");
 dEdx_down_pos.Write();

 dEdx_up_pos.GetXaxis()->SetTitle("dE/dX");
 dEdx_up_pos.Write();

 dEdx_down_neg.GetXaxis()->SetTitle("dE/dX");
 dEdx_down_neg.Write();

 dEdx_up_neg.GetXaxis()->SetTitle("dE/dX");
 dEdx_up_neg.Write();

 dEdx_down_davanti.GetXaxis()->SetTitle("dE/dX");
 dEdx_down_davanti.Write();

 dEdx_up_davanti.GetXaxis()->SetTitle("dE/dX");
 dEdx_up_davanti.Write();

 dEdx_down_dietro.GetXaxis()->SetTitle("dE/dX");
 dEdx_down_dietro.Write();

 dEdx_up_davanti.GetXaxis()->SetTitle("dE/dX");
 dEdx_up_davanti.Write();


 dE_Tutti.GetXaxis()->SetTitle("dE");
 dE_Tutti.Write();

 dX_Tutti.GetXaxis()->SetTitle("dX");
 dX_Tutti.Write();

 occupancyEnergy.GetXaxis()->SetTitle("i#phi");
 occupancyEnergy.GetYaxis()->SetTitle("i#eta");
 occupancyEnergy.Write () ;

 occupancy.GetXaxis()->SetTitle("i#phi");
 occupancy.GetYaxis()->SetTitle("i#eta");
 occupancy.Write ();

 Coeff.GetXaxis()->SetTitle("i#eta");
 Coeff.GetYaxis()->SetTitle("dE/dx");
 Coeff.Write();

 TGraphErrorsCoefficients.GetXaxis()->SetTitle("i#eta");
 TGraphErrorsCoefficients.GetYaxis()->SetTitle("dE/dx");
 TGraphErrorsCoefficients.Write();

 dEdXEtaPhi.GetXaxis()->SetTitle("i#phi");
 dEdXEtaPhi.GetYaxis()->SetTitle("i#eta");
 dEdXEtaPhi.Write();

 CoeffDOWN.GetXaxis()->SetTitle("i#eta");
 CoeffDOWN.GetYaxis()->SetTitle("dE/dx");
 CoeffDOWN.Write();

 CoeffUP.GetXaxis()->SetTitle("i#eta");
 CoeffUP.GetYaxis()->SetTitle("dE/dx");
 CoeffUP.Write();

 CoeffDR.GetXaxis()->SetTitle("i#eta");
 CoeffDR.GetYaxis()->SetTitle("dE/dx");
 CoeffDR.Write();

 CoeffDL.GetXaxis()->SetTitle("i#eta");
 CoeffDL.GetYaxis()->SetTitle("dE/dx");
 CoeffDL.Write();

 CoeffUR.GetXaxis()->SetTitle("i#eta");
 CoeffUR.GetYaxis()->SetTitle("dE/dx");
 CoeffUR.Write();

 CoeffUL.GetXaxis()->SetTitle("i#eta");
 CoeffUL.GetYaxis()->SetTitle("dE/dx");
 CoeffUL.Write();

 dEdxVsEnergy.GetXaxis()->SetTitle("dE/dx");
 dEdxVsEnergy.GetYaxis()->SetTitle("E");
 dEdxVsEnergy.Write();

 dEdxVsEnergy_up.GetXaxis()->SetTitle("dE/dx");
 dEdxVsEnergy_up.GetYaxis()->SetTitle("E");
 dEdxVsEnergy_up.Write();

 dEdxVsEnergy_down.GetXaxis()->SetTitle("dE/dx");
 dEdxVsEnergy_down.GetYaxis()->SetTitle("E");
 dEdxVsEnergy_down.Write();

 dEdxVsdX.GetXaxis()->SetTitle("dE/dx");
 dEdxVsdX.GetYaxis()->SetTitle("dX");
 dEdxVsdX.Write();

 dEdxVsdXUP.GetXaxis()->SetTitle("dE/dx");
 dEdxVsdXUP.GetYaxis()->SetTitle("dX");
 dEdxVsdXUP.Write();

 dEdxVsdXDOWN.GetXaxis()->SetTitle("dE/dx");
 dEdxVsdXDOWN.GetYaxis()->SetTitle("dX");
 dEdxVsdXDOWN.Write();

 //---- dE Graphs ----

 dECoeff.GetXaxis()->SetTitle("i#eta");
 dECoeff.GetYaxis()->SetTitle("dE");
 dECoeff.Write();

 dECoeffDOWN.GetXaxis()->SetTitle("i#eta");
 dECoeffDOWN.GetYaxis()->SetTitle("dE");
 dECoeffDOWN.Write();

 dECoeffUP.GetXaxis()->SetTitle("i#eta");
 dECoeffUP.GetYaxis()->SetTitle("dE");
 dECoeffUP.Write();

 dEEtaPhi.GetXaxis()->SetTitle("i#phi");
 dEEtaPhi.GetYaxis()->SetTitle("i#eta");
 dEEtaPhi.Write();

 dE_vs_dX.GetXaxis()->SetTitle("dE");
 dE_vs_dX.GetYaxis()->SetTitle("dX");
 dE_vs_dX.Write();

 dE_vs_dX_up.GetXaxis()->SetTitle("dE");
 dE_vs_dX_up.GetYaxis()->SetTitle("dX");
 dE_vs_dX_up.Write();

 dE_vs_dX_down.GetXaxis()->SetTitle("dE");
 dE_vs_dX_down.GetYaxis()->SetTitle("dX");
 dE_vs_dX_down.Write();


 //---- Alpha Graphs ----

 dEdx_vs_angle_Mu_Sky.GetXaxis()->SetTitle("#alpha Mu Sky");
 dEdx_vs_angle_Mu_Sky.GetYaxis()->SetTitle("dE/dx");
 dEdx_vs_angle_Mu_Sky.Write();

 dEdx_alpha.GetXaxis()->SetTitle("#alpha");
 dEdx_alpha.Write();

 dEdx_alpha_up.GetXaxis()->SetTitle("#alpha");
 dEdx_alpha_up.Write();

 dEdx_alpha_down.GetXaxis()->SetTitle("#alpha");
 dEdx_alpha_down.Write();

 alpha_up.GetXaxis()->SetTitle("#alpha");
 alpha_up.Write();

 alpha_down.GetXaxis()->SetTitle("#alpha");
 alpha_down.Write();

 muon_up.GetXaxis()->SetTitle("#alpha");
 muon_up.Write();

 muon_down.GetXaxis()->SetTitle("#alpha");
 muon_down.Write();

 EtaPhi_events.GetXaxis()->SetTitle("i #eta");
 EtaPhi_events.GetYaxis()->SetTitle("i #phi");
 EtaPhi_events.Write();

 EtaPhi_alpha_xtal.GetXaxis()->SetTitle("i #eta");
 EtaPhi_alpha_xtal.GetYaxis()->SetTitle("i #phi");
 EtaPhi_alpha_xtal.Write();

 EtaPhi_alpha_xtalMU.GetXaxis()->SetTitle("i #eta");
 EtaPhi_alpha_xtalMU.GetYaxis()->SetTitle("i #phi");
 EtaPhi_alpha_xtalMU.Write();

 EtaPhi_alpha_MU.GetXaxis()->SetTitle("i #eta");
 EtaPhi_alpha_MU.GetYaxis()->SetTitle("i #phi");
 EtaPhi_alpha_MU.Write();


 //---- SM Graphs ----

 //---- Down ----
 dEdx_DLB.GetXaxis()->SetTitle("dE/dx");
 dEdx_DLB.Write();

 dEdx_DLF.GetXaxis()->SetTitle("dE/dx");
 dEdx_DLF.Write();

 dEdx_DRB.GetXaxis()->SetTitle("dE/dx");
 dEdx_DRB.Write();

 dEdx_DRF.GetXaxis()->SetTitle("dE/dx");
 dEdx_DRF.Write();

 dE_DLB.GetXaxis()->SetTitle("dE");
 dE_DLB.Write();

 dE_DLF.GetXaxis()->SetTitle("dE");
 dE_DLF.Write();

 dE_DRB.GetXaxis()->SetTitle("dE");
 dE_DRB.Write();

 dE_DRF.GetXaxis()->SetTitle("dE");
 dE_DRF.Write();

 dX_DLB.GetXaxis()->SetTitle("dx");
 dX_DLB.Write();

 dX_DLF.GetXaxis()->SetTitle("dx");
 dX_DLF.Write();

 dX_DRB.GetXaxis()->SetTitle("dx");
 dX_DRB.Write();

 dX_DRF.GetXaxis()->SetTitle("dx");
 dX_DRF.Write();

 //---- Up ----
 dEdx_ULB.GetXaxis()->SetTitle("dE/dx");
 dEdx_ULB.Write();

 dEdx_ULF.GetXaxis()->SetTitle("dE/dx");
 dEdx_ULF.Write();

 dEdx_URB.GetXaxis()->SetTitle("dE/dx");
 dEdx_URB.Write();

 dEdx_URF.GetXaxis()->SetTitle("dE/dx");
 dEdx_URF.Write();

 dE_ULB.GetXaxis()->SetTitle("dE");
 dE_ULB.Write();

 dE_ULF.GetXaxis()->SetTitle("dE");
 dE_ULF.Write();

 dE_URB.GetXaxis()->SetTitle("dE");
 dE_URB.Write();

 dE_URF.GetXaxis()->SetTitle("dE");
 dE_URF.Write();

 dX_ULB.GetXaxis()->SetTitle("dx");
 dX_ULB.Write();

 dX_ULF.GetXaxis()->SetTitle("dx");
 dX_ULF.Write();

 dX_URB.GetXaxis()->SetTitle("dx");
 dX_URB.Write();

 dX_URF.GetXaxis()->SetTitle("dx");
 dX_URF.Write();


 
 dEdxSingleXtal_Tutti.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti.Write();

 dEdxSingleXtal_Tutti_40.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_40.Write();
 dEdxSingleXtal_Tutti_30.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_30.Write();
 dEdxSingleXtal_Tutti_20.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_20.Write();
 dEdxSingleXtal_Tutti_10.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_10.Write();
 
 dEdxSingleXtal_Tutti_40Up.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_40Up.Write();
 dEdxSingleXtal_Tutti_30Up.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_30Up.Write();
 dEdxSingleXtal_Tutti_20Up.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_20Up.Write();
 dEdxSingleXtal_Tutti_10Up.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_10Up.Write();
 
 dEdxSingleXtal_Tutti_40Down.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_40Down.Write();
 dEdxSingleXtal_Tutti_30Down.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_30Down.Write();
 dEdxSingleXtal_Tutti_20Down.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_20Down.Write();
 dEdxSingleXtal_Tutti_10Down.GetXaxis()->SetTitle("dE/dx");
 dEdxSingleXtal_Tutti_10Down.Write();

 dummyDistrib.Write(); 
 dEdx.Write();
 //---- end Graphs ----

 saving.Close () ;
 delete chain ;
 return 0 ;

}


