#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>

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

#define PI 3.14159265

int main (int argc, char** argv)
{
 std::map<int, TH1F *> dEdx_Histos ;

 int nEntries = -1 ;

  //---- AM Command line input ----
  
//  for (int i=0; i<argc; i++)
 
 std::string outputRootName = "outputHistos.root";
 std::string testNameOutput = "-o";
//  if (argc>2){
//   if (argv[1] == testNameOutput) {
//    outputRootName = argv[2];
//   }
//  }
  
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
  for (int i=0; i< 46; i++) {
   sprintf (inputRootName,"/castor/cern.ch/user/m/mattia/50908Global/EcalCosmicsTree-50908%i.tree.root",i); 
   //   sprintf (inputRootName,"/castor/cern.ch/user/m/mattia/50908Cosmic/EcalCosmicsTree-50908%i.tree.root",i); 
   chain->Add (inputRootName);
   std::cout << "Open: " << inputRootName << std::endl;
  }
  // }
 
 EcalCosmicsTreeContent treeVars ; 
 setBranchAddresses (chain, treeVars) ;
 nEntries = chain->GetEntries () ;
   
 TH1F dEdx_Tutti("dEdx_Tutti","dEdx_Tutti",1000,0,2);
 TH1F dE_Tutti("dE_Tutti","dE_Tutti",1000,0,2);
 TH1F dX_Tutti("dX_Tutti","dX_Tutti",1000,0.,25.);  

 // TH2F dEvsAlpha_Tutti("dEvsAlpha_Tutti","dEvsAlpha_Tutti",200, 0., 90., 1000,0. ,2. );
 TProfile dEvsAlpha_Tutti("dEvsAlpha_Tutti","dEvsAlpha_Tutti",100, 0., 90.);
 TProfile dXvsAlpha_Tutti("dXvsAlpha_Tutti","dXvsAlpha_Tutti",100, 0., 90.);

 // TProfile dEdx_alpha("dEdx_alpha", "dEdx_alpha", 20, 0., 1.6 );
 TProfile dEdx_alpha("dEdx_alpha", "dEdx_alpha", 100, 0., 90. );
 // TH2F dEdx_alpha("dEdx_alpha", "dEdx_alpha", 200, 0., 90., 100, 0., 3. );
 //TH1F dEdx_alpha("dEdx_alpha", "dEdx_alpha", 20, 0., PI/2 );
 TH1F xtalY("xtalY", "xtalY", 300, -150., 150. );
 TH1F muonY("muonY", "muonY", 300, -150., 150. );

 TH2F occupancyEnergy ("occupancyEnergy","single crystals counting Energy",180,0,360,171,-85,86) ;
 TH2F occupancy ("occupancy","single crystals counting",360,1,361,171,-85,86) ;
 
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
 
	 //PG loop over crystals <-- in case the calib is based on single
	 //PG                        xtal deposit
	 for (int XTLindex = treeVars.xtalIndexInSuperCluster[SCindex] ;
	      XTLindex < treeVars.xtalIndexInSuperCluster[SCindex] +
		treeVars.nXtalsInSuperCluster[SCindex] ; ++XTLindex)
	   {
	     if(treeVars.xtalTkLength[XTLindex] == -1) continue;
	
	     //PG find a unique index for each xtal --> rawId()
	     EBDetId dummy = EBDetId::unhashIndex (treeVars.xtalHashedIndex[XTLindex]) ;   
	     
	     //taglio in phi o eta?
	     //	     if( (dummy.iphi() < 80) || (dummy.iphi() > 100) ) continue; 

	     //get Xtal direction
	     TVector3 Xtal_pos (0., 0., 0.) ;
	     setVectorOnECAL (Xtal_pos, dummy.ieta(), dummy.iphi(), 130);

	     int MUindex = associations.at (i).first  ;
		     		     
	     //get Muon direction at ECAL
	     //GlobalPoint AtEcal;
	     //setMuonTkAtECAL(AtEcal, MUindex, treeVars);
	  
	     // get muon direction
	     //TVector3 MuonDir (AtEcal.x(), AtEcal.y(), AtEcal.z());
	     TVector3 MuonDir (treeVars.muonMomentumX[MUindex], treeVars.muonMomentumY[MUindex], treeVars.muonMomentumZ[MUindex]);
	     
	     // calc angle 
	     float angle = MuonDir.Angle( Xtal_pos ) ;
	     if( angle > PI/2. ) angle = PI - angle; // angle belongs to [0:90]
	     dEdx_alpha.Fill(angle*180./PI, treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
	     
	     //control plots
	     xtalY.Fill(Xtal_pos.y());
	     muonY.Fill(MuonDir.y());
	
	     dEvsAlpha_Tutti.Fill(angle*180./PI, treeVars.xtalEnergy[XTLindex]);
	     dXvsAlpha_Tutti.Fill(angle*180./PI, treeVars.xtalTkLength[XTLindex]);
	     //define alpha cuts - to be done

	     
	     //----------------------------------------
	     
	     //PG verify if the xtal index is already present in the dEdx_Histos map
	     //PG    (if not, add it)         
	     
	     std::map<int, TH1F *>::iterator dEdx_Histos_iter = dEdx_Histos.find(dummy.rawId());
	     if (dEdx_Histos_iter == dEdx_Histos.end()) {
	       std::ostringstream stm;
	       stm << dummy.rawId();
	       std::string TH1FName = "dEdX_" +  stm.str();
	       std::string TH1FNameDescription = "dE over dX -> rawId = " + stm.str();
	       dEdx_Histos[dummy.rawId()] = new TH1F(TH1FName.c_str(),TH1FNameDescription.c_str(),1000,0,2);
	     }
	     //    std::cout << " hashedIndex = " << dummy.hashedIndex();
	     
	     //PG fill the histo with the E/L for the xtal
	     //	     if (treeVars.xtalTkLength[XTLindex] != 0) {
	     dEdx_Tutti.Fill(treeVars.xtalEnergy[XTLindex] / treeVars.xtalTkLength[XTLindex]);
	     //	     }
	     //	       if(treeVars.xtalTkLength[XTLindex] >0 ) 
	     dEdx_Histos[dummy.rawId()]->Fill(treeVars.xtalEnergy[XTLindex]/treeVars.xtalTkLength[XTLindex]);
	  
	     dE_Tutti.Fill(treeVars.xtalEnergy[XTLindex]);
	     dX_Tutti.Fill(treeVars.xtalTkLength[XTLindex]);

	     occupancy.Fill (dummy.iphi (), dummy.ieta ()) ;
	     occupancyEnergy.Fill (dummy.iphi (),dummy.ieta (),treeVars.xtalEnergy[XTLindex]);
	   }
       }
   } //PG loop over entries


 //---- AM Initiate output file ----
 TFile saving (outputRootName.c_str(),"recreate") ;
 saving.cd () ;

 //dE/dx map from the fit
 std::map<int, float> XtalCoeff_map ;
 
 //PG loop over dEdx_Histos
 for (std::map<int, TH1F*>::iterator mapIt = dEdx_Histos.begin () ;
      mapIt != dEdx_Histos.end () ;
      ++mapIt)
   {
     mapIt->second->Write();
     //   mapIt->second->Fit ("gaus") ;

     // get the peak of dEondX (da controllare con piu' statistica anche i "puntatori")
     // XtalCoeff_map[mapIt->first] = fitdEdx(mapIt->second);
     
   } //PG loop over dEdx_Histos
 
std::cout<<fitdEdx(&dEdx_Tutti)<<std::endl;

 // dEdx_Histos.
 dEdx_alpha.GetXaxis()->SetTitle("#alpha");
 dEdx_alpha.Write();

 xtalY.Write();
 muonY.Write();
 dEvsAlpha_Tutti.Write();
 dXvsAlpha_Tutti.Write();

 dEdx_Tutti.GetXaxis()->SetTitle("dE/dX");
 dEdx_Tutti.Write();
 
 dE_Tutti.GetXaxis()->SetTitle("dE");
 dE_Tutti.Write();
 
 dX_Tutti.GetXaxis()->SetTitle("dX");
 dX_Tutti.Write();
 
 occupancyEnergy.GetXaxis()->SetTitle("i#phi");
 occupancyEnergy.GetYaxis()->SetTitle("i#eta");
 occupancyEnergy.Write () ;

 occupancy.GetXaxis()->SetTitle("i#phi");
 occupancy.GetYaxis()->SetTitle("i#eta");
 occupancy.Write () ;
  
 saving.Close () ;
 delete chain ;
 return 0 ;

}
