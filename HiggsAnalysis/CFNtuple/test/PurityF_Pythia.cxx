//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id: P_S_noweight.cxx,v 1.2 2011/02/10 16:16:45 govoni Exp $
//
// Description:
//      Simple example usage of the RooUnfold package using toy MC.
//
// Authors: Tim Adye <T.J.Adye@rl.ac.uk> and Fergus Wilson <fwilson@slac.stanford.edu>
//
//==============================================================================


#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#endif



#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream>
using std::cout;
using std::endl;

#include "TRandom.h"
#include "TH1D.h"
#include "treeReader.h"
#include "hFactory.h"
#include "hFunctions.h"
#include "stdHisto.h"
#include "ConfigParser.h"
#include "ntpleUtils.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TMinuit.h"
#include "Math/GenVector/VectorUtil.h"
#include "TRandom3.h"
#include <time.h>
#include <sstream>
#include "CFNtuple.h"


#include "math.h"

#endif

//==============================================================================
// Global definitions
//==============================================================================

//==============================================================================
// Unfolding for C-F jet analysis
//==============================================================================

void PurityF_Pythia()
{ 
      Float_t lowEdge[11] = {18,22,27,35,45,57,72,90,120,150,1000};
      int NBIN = 10;
      int tresholdBIN= 5;
//    Float_t lowEdge[10] = {18,22,27,35,45,57,72,90,120,150};
//   int NBIN = 9;

//     Float_t lowEdge[8] = {35,45,57,72,90,120,150,1000};
//     int NBIN = 7;  
//     int tresholdBIN= 2;
  
  int debug = 0;
  
  TH2D* hResponseMatrixFJetPythia15 = new TH2D ("hResponseMatrixFJetPythia15", "Response Matrix FJet Pythia15",NBIN,lowEdge,NBIN,lowEdge);
  TH2D* hResponseMatrixFJetPythia30 = new TH2D ("hResponseMatrixFJetPythia30", "Response Matrix FJet Pythia30",NBIN,lowEdge,NBIN,lowEdge);

  // TFile FileTrain1PythiaF("/home/toliman/Dropbox/QCD_CF/Unfolding/NewNtuple/cut_18_35/qcd_15_pythia.root","READ");
   TFile FileTrain1PythiaF("/home/toliman/Dropbox/QCD_CF/Unfolding/NewNtuple/cut_18_35/qcd_15_pythia.root","READ");
   TFile FileTrain2PythiaF("/home/toliman/Dropbox/QCD_CF/Unfolding/NewNtuple/cut_18_35/qcd_30_pythia.root","READ");
  
   
//    TFile FileTrain1PythiaF("../input/BO/rootfiles/mc/qcd_15_pythia.root","READ");
//    TFile FileTrain2PythiaF("../input/BO/rootfiles/mc/qcd_30_pythia.root","READ");
   
     
  TTree* TreeTrain1PythiaF = (TTree*) FileTrain1PythiaF.Get("AnaHiggs");
  TTree* TreeTrain2PythiaF = (TTree*) FileTrain2PythiaF.Get("AnaHiggs");

  
  Float_t G_FJet_Pt; //~~~~ had
  Float_t S_FJet_Pt; //~~~~ reco

  Float_t G_CJet_Pt; //~~~~ had
  Float_t S_CJet_Pt; //~~~~ reco
  
   Float_t G_FJet_Phi; //~~~~ had
  Float_t S_FJet_Phi; //~~~~ reco

  Float_t G_CJet_Phi; //~~~~ had
  Float_t S_CJet_Phi; //~~~~ reco
  
   Float_t G_FJet_Eta; //~~~~ had
  Float_t S_FJet_Eta; //~~~~ reco

  Float_t G_CJet_Eta; //~~~~ had
  Float_t S_CJet_Eta; //~~~~ reco
 
 
 TreeTrain1PythiaF->SetBranchAddress("G_FJet_Pt",&G_FJet_Pt);
 TreeTrain1PythiaF->SetBranchAddress("S_FJet_Pt",&S_FJet_Pt);
 TreeTrain1PythiaF->SetBranchAddress("G_CJet_Pt",&G_CJet_Pt);
 TreeTrain1PythiaF->SetBranchAddress("S_CJet_Pt",&S_CJet_Pt);

 TreeTrain2PythiaF->SetBranchAddress("G_FJet_Pt",&G_FJet_Pt);
 TreeTrain2PythiaF->SetBranchAddress("S_FJet_Pt",&S_FJet_Pt);
 TreeTrain2PythiaF->SetBranchAddress("G_CJet_Pt",&G_CJet_Pt);
 TreeTrain2PythiaF->SetBranchAddress("S_CJet_Pt",&S_CJet_Pt);
 
 TreeTrain1PythiaF->SetBranchAddress("G_FJet_Phi",&G_FJet_Phi);
 TreeTrain1PythiaF->SetBranchAddress("S_FJet_Phi",&S_FJet_Phi);
 TreeTrain1PythiaF->SetBranchAddress("G_CJet_Phi",&G_CJet_Phi);
 TreeTrain1PythiaF->SetBranchAddress("S_CJet_Phi",&S_CJet_Phi);

 TreeTrain2PythiaF->SetBranchAddress("G_FJet_Phi",&G_FJet_Phi);
 TreeTrain2PythiaF->SetBranchAddress("S_FJet_Phi",&S_FJet_Phi);
 TreeTrain2PythiaF->SetBranchAddress("G_CJet_Phi",&G_CJet_Phi);
 TreeTrain2PythiaF->SetBranchAddress("S_CJet_Phi",&S_CJet_Phi);
 
 TreeTrain1PythiaF->SetBranchAddress("G_FJet_Eta",&G_FJet_Eta);
 TreeTrain1PythiaF->SetBranchAddress("S_FJet_Eta",&S_FJet_Eta);
 TreeTrain1PythiaF->SetBranchAddress("G_CJet_Eta",&G_CJet_Eta);
 TreeTrain1PythiaF->SetBranchAddress("S_CJet_Eta",&S_CJet_Eta);

 TreeTrain2PythiaF->SetBranchAddress("G_FJet_Eta",&G_FJet_Eta);
 TreeTrain2PythiaF->SetBranchAddress("S_FJet_Eta",&S_FJet_Eta);
 TreeTrain2PythiaF->SetBranchAddress("G_CJet_Eta",&G_CJet_Eta);
 TreeTrain2PythiaF->SetBranchAddress("S_CJet_Eta",&S_CJet_Eta);



  cout << "=================================================================================" << endl;
  cout << "==================================== Pythia =====================================" << endl;
  
   double DR_F_max =  0.2;
   //double DR_C_max =  0.2;
             
  for (Int_t iEvt= 0; iEvt<TreeTrain1PythiaF->GetEntries(); iEvt++) {
    TreeTrain1PythiaF->GetEntry(iEvt);
   
//     if (S_FJet_Pt>0 && G_FJet_Pt>0 && S_CJet_Pt>0 && G_CJet_Pt>0)  {
//      if (S_FJet_Pt>0 && G_FJet_Pt>18 && S_CJet_Pt>0 && G_CJet_Pt>35)  {
      if (S_FJet_Pt>18 && G_FJet_Pt>18 && S_CJet_Pt>35 && G_CJet_Pt>35)  {
	Delta_R = sqrt((fabs(S_FJet_Eta-G_FJet_Eta))*(fabs(S_FJet_Eta-G_FJet_Eta))+ (fabs(S_FJet_Phi- G_FJet_Phi))*(fabs(S_FJet_Phi- G_FJet_Phi)));
	if (Delta_R<DR_F_max){
	if (debug ==1) std::cerr<<"Delta_R = "<<Delta_R << std::endl;

//    if (S_FJet_Pt>0 && G_FJet_Pt>0)  {     
	hResponseMatrixFJetPythia15->Fill(S_FJet_Pt, G_FJet_Pt);
	}
      }
   }
   
  
  for (Int_t iEvt= 0; iEvt<TreeTrain2PythiaF->GetEntries(); iEvt++) {
    TreeTrain2PythiaF->GetEntry(iEvt);
//      if (S_FJet_Pt>0 && G_FJet_Pt>18 && S_CJet_Pt>0 && G_CJet_Pt>35)  {

      if (S_FJet_Pt>18 && G_FJet_Pt>18 && S_CJet_Pt>35 && G_CJet_Pt>35)  {
	
       Delta_R = sqrt((fabs(S_FJet_Eta-G_FJet_Eta))*(fabs(S_FJet_Eta-G_FJet_Eta))+ (fabs(S_FJet_Phi- G_FJet_Phi))*(fabs(S_FJet_Phi- G_FJet_Phi)));
	if (Delta_R<DR_F_max){
	//if (debug ==1) std::cerr<<"Delta_R = "<<Delta_R << std::endl;
//    if (S_FJet_Pt>0 && G_FJet_Pt>0)  { 
	hResponseMatrixFJetPythia30->Fill(S_FJet_Pt, G_FJet_Pt);
	}
     }
  }
  
          
  cout << "==================================== Purity =====================================" << endl;
  TH1D* Purity15 = new TH1D ("Purity15", "Pythia15",NBIN,lowEdge);
  TH1D* Purity30 = new TH1D ("Purity30", "Pythia30",NBIN,lowEdge);
  
//for forward Jets smaller than 57
  for (int iBinX = 0; iBinX<tresholdBIN; iBinX++){
    double purity = 0;
    double column = 0;
    for (int iBinY = 0; iBinY<NBIN; iBinY++){
      column += hResponseMatrixFJetPythia15->GetBinContent(iBinX+1,iBinY+1);
    }
    double diag = hResponseMatrixFJetPythia15->GetBinContent(iBinX+1,iBinX+1);
    double offd = column - diag;
    purity = diag / column;
    Purity15->SetBinContent(iBinX+1, purity);
    Purity15->SetBinError(iBinX+1,sqrt((offd)/column/column*(offd)/column/column*diag+(diag/column/column)*(diag/column/column)*(offd)));
  }
  

  //for forward Jets greater than 57
  for (int iBinX = tresholdBIN; iBinX<NBIN; iBinX++){
    double purity = 0;
    double column = 0;
    for (int iBinY = 0; iBinY<NBIN; iBinY++){
      column += hResponseMatrixFJetPythia30->GetBinContent(iBinX+1,iBinY+1);
    }
    double diag = hResponseMatrixFJetPythia30->GetBinContent(iBinX+1,iBinX+1);
    double offd = column - diag;
    purity = diag / column;
    Purity30->SetBinContent(iBinX+1, purity);
    Purity30->SetBinError(iBinX+1,sqrt((offd)/column/column*(offd)/column/column*diag+(diag/column/column)*(diag/column/column)*(offd)));
  }
  
//  Purity15->Add(Purity30);
  
  TCanvas* cPurityF_P = new TCanvas("cPurityF_P","cPurityF_P",600,600);  
    
  TH1F* Bop = (TH1F*) _file0->Get("hist_p");
  Bop->GetYaxis()->SetRangeUser(0.,1.0);
  Bop->SetMarkerColor(kRed);
  Bop->SetStats(0);
  Bop->Draw();
  
  Purity15->GetYaxis()->SetRangeUser(0.,1.0);
//  Purity15->GetXaxis()->SetRangeUser(18.,150.);
  Purity15->SetTitle("");
  Purity15->SetMarkerStyle(21);
//   Purity15->SetMarkerSize(1);
  Purity15->GetYaxis()->SetTitle("Purity");
  Purity15->GetXaxis()->SetTitle("forward jet p_{T} [GeV/c]");
  Purity15->SetStats(0);
  Purity15->SetMarkerColor(kBlue);
  Purity15->SetLineColor(kBlue);
  Purity15->DrawCopy("same");
  
  Purity30->SetMarkerColor(kGreen);
  Purity30->SetLineColor(kGreen);
  Purity30->DrawCopy("same");

  //for (int iBinX = 0; iBinX<NBIN; iBinX++){
  // std::cerr << " iBinX " << iBinX << "[" << lowEdge[iBinX] << " ~ " << lowEdge[iBinX+1] << "] :: " << Purity15->GetBinContent(iBinX+1) << " :: " << Purity30->GetBinContent(iBinX+1) << " :: " << Bop->GetBinContent(iBinX+1) << std::endl;
  //}


  /*double diag_v[100];
  double column_v[100];

  for (int iBinX = 0; iBinX<NBIN; iBinX++){
  	  diag_v[iBinX] = 0;
  	  column_v[iBinX] = 0;
   }
  for (Int_t iEvt= 0; iEvt<TreeTrain1PythiaF->GetEntries(); iEvt++) {
    TreeTrain1PythiaF->GetEntry(iEvt);
//    if (S_FJet_Pt>0 && G_FJet_Pt>0 && (S_CJet_Pt>0 || G_CJet_Pt>0))  {
    if (S_FJet_Pt>0 && G_FJet_Pt>0)  {
      if (S_FJet_Pt < 57) {
         for (int iBinX = 0; iBinX<NBIN; iBinX++){
          if (S_FJet_Pt < lowEdge[iBinX+1] && S_FJet_Pt >= lowEdge[iBinX]) {
            column_v[iBinX] ++;
            if (G_FJet_Pt < lowEdge[iBinX+1] && G_FJet_Pt >= lowEdge[iBinX]) {
              diag_v[iBinX] ++;
            }
          }
        }
      }
    }
   }
   
     for (Int_t iEvt= 0; iEvt<TreeTrain2PythiaF->GetEntries(); iEvt++) {
    TreeTrain2PythiaF->GetEntry(iEvt);
//    if (S_FJet_Pt>0 && G_FJet_Pt>0 && (S_CJet_Pt>0 || G_CJet_Pt>0))  {
    if (S_FJet_Pt>0 && G_FJet_Pt>0)  {
     if (S_FJet_Pt >= 57) {
         for (int iBinX = 0; iBinX<NBIN; iBinX++){
          if (S_FJet_Pt < lowEdge[iBinX+1] && S_FJet_Pt >= lowEdge[iBinX]) {
            column_v[iBinX] ++;
            if (G_FJet_Pt < lowEdge[iBinX+1] && G_FJet_Pt >= lowEdge[iBinX]) {
              diag_v[iBinX] ++;
            }
          }
        }
      }
    }
   }
   
   
   std::cerr << " ~~~~~~~~~~~~~~~~~~~~~~~~~~ " << std::endl;

  for (int iBinX = 0; iBinX<NBIN; iBinX++){
   std::cerr << " iBinX " << iBinX << "[" << lowEdge[iBinX] << " ~ " << lowEdge[iBinX+1] << "] :: " << diag_v[iBinX] << " / " << column_v[iBinX] << " = " << diag_v[iBinX] / column_v[iBinX] << " :: " << Purity15->GetBinContent(iBinX+1) << " :: " << Purity30->GetBinContent(iBinX+1) << " ::>> " << Bop->GetBinContent(iBinX+1) << std::endl;
  }*/

   TLegend* leg = new TLegend(0.6,0.7,0.9,0.9,"","brNDC");
   leg->SetBorderSize(0);
   leg->SetTextFont(42);
   leg->SetTextSize(0.04);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(1001);
   
   leg->AddEntry("Purity15","Purity15", "lpf");
   leg->AddEntry("Purity30","Purity30", "lpf");
   leg->AddEntry("Bop","Bop", "lpf");
   leg->Draw();
  
}

