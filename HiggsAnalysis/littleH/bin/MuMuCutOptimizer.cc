#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include <TGraph.h>
#include <TH2F.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>

#include <math.h>

#include "MuMuCutOptimizer.hh"

using std::cout;
using std::endl;

MuMuCutOptimizer::MuMuCutOptimizer(TChain *tree) 
  : smallHBaseClass(tree) {

  /// SELECTION CUTS ///
 
  MAX_normchi2_glb = 20.0;
  MIN_vtxprob = 0.001;//0.04;
  MAX_S3Dip = 10.;//1.1;
  MAX_muisol = 1.;// 0.5;
  MIN_muP = 0.;
  minPt = 3.;
  n_step = 1;
  n_signal = 1;

  bookHistos();

}

void MuMuCutOptimizer::bookHistos()
{

  hIsoVar03_glb_TKECAL     = new TH1F("hIsoVar03_glb_TKECAL", "isolation var03 on tk+ecal, glb", 500, 0., 5. );
  hIsoVar03_glb_TKECALHCAL = new TH1F("hIsoVar03_glb_TKECALHCAL", "isolation var03 on tk+ecal+hcal, glb", 500, 0., 5. );
  hIsoVar03_trk_TKECAL     = new TH1F("hIsoVar03_trk_TKECAL", "isolation var03 on tk+ecal, trk", 500, 0., 5. );
  hIsoVar03_trk_TKECALHCAL = new TH1F("hIsoVar03_trk_TKECALHCAL", "isolation var03 on tk+ecal+hcal, trk", 500, 0., 5. );

  hIsoVar03_glb_TKECAL_pt     = new TH1F("hIsoVar03_glb_TKECAL_pt", "isolation var03 on tk+ecal/pt, glb", 500, 0., 5. );
  hIsoVar03_glb_TKECALHCAL_pt = new TH1F("hIsoVar03_glb_TKECALHCAL_pt", "isolation var03 on tk+ecal+hcal/pt, glb", 500, 0., 5. );
  hIsoVar03_trk_TKECAL_pt     = new TH1F("hIsoVar03_trk_TKECAL_pt", "isolation var03 on tk+ecal/pt, trk", 500, 0., 5. );
  hIsoVar03_trk_TKECALHCAL_pt = new TH1F("hIsoVar03_trk_TKECALHCAL_pt", "isolation var03 on tk+ecal+hcal/pt, trk", 500, 0., 5. );

  hIsoVar03_glb_TK = new TH1F("hIsoVar03_glb_TK", "isolation var03 on tk, glb", 500, 0., 5. );
  hIsoVar03_trk_TK = new TH1F("hIsoVar03_trk_TK", "isolation var03 on tk, trk", 500, 0., 5. );
  hIsoVar03_glb_ECAL = new TH1F("hIsoVar03_glb_ECAL", "isolation var03 on ecal, glb", 500, 0., 5. );
  hIsoVar03_trk_ECAL = new TH1F("hIsoVar03_trk_ECAL", "isolation var03 on ecal, trk", 500, 0., 5. );
  hIsoVar03_glb_HCAL = new TH1F("hIsoVar03_glb_HCAL", "isolation var03 on hcal, glb", 500, 0., 5. );
  hIsoVar03_trk_HCAL = new TH1F("hIsoVar03_trk_HCAL", "isolation var03 on hcal, trk", 500, 0., 5. );

  hQQProbChi2 = new TH1F("hQQProbChi2","#chi^2 prob", 1000, 0., 1.);
  hQQS3Dip = new TH1F("hQQS3Dip", "", 100, 0., 30.);
  hQQSTip = new TH1F("hQQSTip", "", 100, 0., 5.);
  hQQEta = new TH1F("hQQEta", "Eta", 100, -5.,5.);
  hQQPt = new TH1F("hQQPt", "Pt", 100, 0., 50.);
  hMuEtaTk = new TH1F("hMuEtaTk","Muon Eta Tk", 100, -5., 5.);
  hMuPtTk = new TH1F("hMuPtTk","Muon Pt Tk", 100, 0., 50.);
  hMuEtaGlb = new TH1F("hMuEtaGlb","Muon Eta Glb", 100, -5., 5.);
  hMuPtGlb = new TH1F("hMuPtGlb","Muon Pt Glb", 100, 0., 50.);

  hMuPGlb = new TH1F("hMuPGlb","Muon P Glb", 100, 0., 50.);
  hMuPTk = new TH1F("hMuPTk","Muon P Tk", 100, 0., 50.);

  hInvMass_all = new TH1F("invMass","#mu-#mu invariant mass",1000,4.,14.);

  for(Int_t i=0; i<n_step; i++){
    cut[i]=0;
    stringstream stream;  stream << i;
    TString counter = stream.str();
    
    TString hName = "hInvMass_"+counter;
    hInvMass[i] = new TH1F(hName,"#mu-#mu invariant mass",1000,4.,14.);
  }
  return;
}

void MuMuCutOptimizer::Loop(string filename, TString flag, TString optVar) {

  if (fChain == 0) return;  
  int nentries = (int)fChain->GetEntries(); 

  // loop over events
  cout << "Number of entries = " << nentries << endl;

  // counters
  int totalEvents = 0;
  int passedTriggers = 0;
  int passedCandidates = 0;
  
  bool accept;

  Float_t signal[n_signal][n_step];
  Float_t m[n_signal];
  Float_t backcont[n_step];
  Float_t ups1S[n_step];
  Float_t ups2S[n_step];
  Float_t ups3S[n_step];
  Float_t cut[n_step];
  Float_t cut2[n_step];
  Float_t sgraph[n_signal][n_step];
  Float_t signifi_max[n_signal];
  Float_t cut_max[n_signal];
  Float_t ssquared[n_signal];  
 
  for(Int_t i=0; i<n_step; i++){
    for(Int_t j=0; j<n_signal; j++){
      m[j]=j+5;
      signal[j][i]=0;
      sgraph[j][i] = 0.;
      signifi_max[j]=0;
      cut_max[j]=0;
      ssquared[j]=0;
    }
    backcont[i]=0;
    ups1S[i]=0;
    ups2S[i]=0;
    ups3S[i]=0;
    cut[i]=0;
    cut2[i]=0;
  }
  
  //===========================
  // === load the HLT tree ====
  //===========================
  std::vector<int> *HLTwasrun=0;
  std::vector<int> *HLTaccept=0;
  std::vector<int> *HLTerror=0;

  TChain *chainHLT = new TChain ("TriggerResults/HLTree") ;

  chainHLT -> SetBranchAddress("HLTwasrun", &HLTwasrun);
  chainHLT -> SetBranchAddress("HLTaccept", &HLTaccept);
  chainHLT -> SetBranchAddress("HLTerror" , &HLTerror);
  chainHLT -> Add(filename.c_str());

  for (int jentry=0; jentry< nentries; jentry++) {
    
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    fChain->GetEntry(jentry);
    chainHLT->GetEntry(jentry);
    
    if (jentry%100000 == 0) cout << ">>> Processing event # " << jentry << endl;
    
    totalEvents++;
    
    // TRIGGER CUTS 
    if( flag.Contains("MC"))  accept = HLTaccept->at(0);
    else
      if ((*nRun)[0] < 147116)
	accept = HLTaccept->at(0);
      else accept = HLTaccept->at(1);
    
    if(accept == 0) continue;
    
    passedTriggers++;
    
    TString filestring(fChain->GetCurrentFile()->GetName());
    
 //    if (filestring.Contains("QCD"))
//       weight = (35.86*406896)/42827261;//these numbers are for QCD in 362
// //48440000000*0.00084//532840//(1000*406896)/2886523; //these are tests for QCD in 397
//     else if (filestring.Contains("Y1S"))
//       weight = (35.86*99900*0.143)/2323818;
//     else if (filestring.Contains("Y2S"))
//       weight = (35.86*75400*0.080)/1148581;
//     else if (filestring.Contains("Y3S")) 
//       weight = (35.86*2850*0.57)/944625;
//     else if (filestring.Contains("Data"))
//       weight = 1.;

    if (filestring.Contains("QCD_Simple"))
      weight = (35.9*40689600)/42827261;//these numbers are for QCD in 362
    else if (filestring.Contains("Y1S_Simple"))
      weight = (35.9*7370)/2323818;
    else if (filestring.Contains("Y2S_Simple"))
      weight = (35.9*1900)/1148581;
    else if (filestring.Contains("Y3S_Simple")) 
      weight = (35.9*1020)/944625;
    else if (filestring.Contains("Data"))
      weight = 1.;

    for (int iqq=0; iqq<(*QQ_size)[0]; iqq++) {//loop on all candidates
      
      //	if (filestring.Contains("QCD") && (*QQ_MC_PDGID)[iqq]==553 )
      //	  continue;//temporary FIX for the QCD sample with the Y1S inside... 

      if ((*QQ_sign)[iqq] != 0) continue;//check if the muons are opposite sign
      
      if((*QQ_type)[iqq] == 0) continue;//check if it is dimuon (not dielectron)

      //if(fabs(((TLorentzVector*)QQ_4mom->At(iqq))->Eta())<2. && ((TLorentzVector*)QQ_4mom->At(iqq))->M() < 14. && ((TLorentzVector*)QQ_4mom->At(iqq))->M() > 4.){
      //if(  (((TLorentzVector*)QQ_4mom->At(iqq))->M() < 8.6 && ((TLorentzVector*)QQ_4mom->At(iqq))->M() > 4.) || (((TLorentzVector*)QQ_4mom->At(iqq))->M() < 14. && ((TLorentzVector*)QQ_4mom->At(iqq))->M() > 10.7)){
	
      //fill histos for QQ quantities
      if(((TLorentzVector*)QQ_4mom->At(iqq))->M() < 14. && ((TLorentzVector*)QQ_4mom->At(iqq))->M() > 4.){
	hQQProbChi2->Fill(QQ_probChi2 -> at(iqq),weight);
	hQQS3Dip -> Fill(QQ_S3Dip -> at(iqq),weight);
	hQQSTip  -> Fill(QQ_STip -> at(iqq),weight );
	hQQEta->Fill(((TLorentzVector*)QQ_4mom->At(iqq))->Eta(),weight);
	hQQPt->Fill(((TLorentzVector*)QQ_4mom->At(iqq))->Pt(),weight);

	hInvMass_all->Fill(((TLorentzVector*)QQ_4mom->At(iqq))->M(), weight);	

	//Fill histos for muons quantities
	
	int thehptMu;
	int thelptMu;
	
	if( QQ_type->at(iqq) == 1 ) {//for GLB-GLB
	  
	  thehptMu = QQ_lepone->at(iqq);    if (thehptMu >= muons_glb_charge->size()) continue;
	  thelptMu = QQ_leptwo->at(iqq);    if (thelptMu >= muons_glb_charge->size()) continue;      
	  
	  fillGlbMuonHistos(thehptMu,weight);
	  fillGlbMuonHistos(thelptMu,weight);
	  
	}else{
	  
	  thehptMu = QQ_lepone->at(iqq);    if (thehptMu >= muons_glb_charge->size()) continue;
	  thelptMu = QQ_leptwo->at(iqq);    if (thelptMu >= muons_trk_charge->size()) continue; 
	  
	  fillGlbMuonHistos(thehptMu,weight);
	  fillTrkMuonHistos(thelptMu,weight);
	}
	
	for(Int_t i=0; i<n_step; i++){
	  //	  cout << i << " " << backcont[i] << " " << ups1S[i] << " " << ups2S[i] << " " << ups3S[i] << endl;
 
	  //set the different cuts
	  if(optVar.Contains("Chi2"))  cut[i] = 0.001 + 0.01*i;
	  else if(optVar.Contains("S3Dip")) cut[i] = .25 + 0.25*i;
	  
	  else if(optVar.Contains("Iso"))
	    cut[i] = 1. - 0.1*i;

	  else if(optVar.Contains("PFwdTk"))
	    cut[i] = 3.+ i*0.5;
	  
	  if(!passCuts(iqq, cut[i], optVar)) {
	    //	    cout << "candidate " << iqq << " did not pass cut " << cut[i] << endl;
	    continue;
	  }
	  passedCandidates++;
	  
	  const float invMass = ((TLorentzVector*)QQ_4mom->At(iqq))->M();
	  
	  // Fill histos
	  hInvMass[i]->Fill(invMass, weight);
	  
	  //Starts calculating the significance
	  
	  if(flag.Contains("MC") && invMass < 11. && invMass > 8.){
	    if(filestring.Contains("QCD_Simple")) backcont[i]+=weight;
	    else if(filestring.Contains("Y1S_Simple")) ups1S[i]+=weight;
	    else if(filestring.Contains("Y2S_Simple")) ups2S[i]+=weight; 
	    else if(filestring.Contains("Y3S_Simple")) ups3S[i]+=weight;
	  }//end of if MC
	  
	  // 	else if(filestring.Contains("5GEV") ) signal[0][i]+=weight;
	  // 	else if(filestring.Contains("6GEV") ) signal[1][i]+=weight;
	  // 	else if(filestring.Contains("7GEV") ) signal[2][i]+=weight;
	  // 	else if(filestring.Contains("8GEV") ) signal[3][i]+=weight;
	  // 	else if(filestring.Contains("9GEV") ) signal[4][i]+=weight;
	  // 	else if(filestring.Contains("10GEV") ) signal[5][i]+=weight;
	  // 	else if(filestring.Contains("10.5GEV") ) signal[6][i]+=weight;
	  
	}//end of loop on step
      }//end of eta <2 selection
    }//end of loop on iqq
  }//end of loop on events

  cout << "###############" << endl;
  cout << "Some statistics " << endl;
  cout << "Total number of processed events = " << totalEvents << endl;
  cout << "Total number of triggered events = " << passedTriggers << endl;
  cout << "Total number of passed candidates = " << passedCandidates << endl;

  if(flag.Contains("MC")){
    for(Int_t j=0; j<n_signal; j++){//AGGIUNGERE loop su secondo taglio!!!!
      for(Int_t i=0; i<n_step; i++){
	ssquared[j] = (ups1S[i]+ups2S[i]+ups3S[i])/sqrt(backcont[i]+ups1S[i]+ups2S[i]+ups3S[i]);//to compare to the one from data
	sgraph[j][i] = ssquared[j];
	cout << "cuts " << cut[i] << " " << ssquared[j] << endl;
	if(ssquared[j] > signifi_max[j]){
	  signifi_max[j] = ssquared[j];
	  cout << "max sign " << signifi_max[j] << " for " << cut[i] << endl;
	  cut_max[j] = cut[i];
	}
      }
    }  
    
  for(Int_t i=0; i<n_step; i++){
    cout << sgraph[0][i] << " for " << cut[i] << endl;    
  }

  TGraph *gr1 = new TGraph(n_step,cut,sgraph[0]);
  gr1->SetTitle(0);
  gr1->GetXaxis()->SetTitle(optVar+" cut");
  gr1->GetYaxis()->SetTitle("S/sqrt(S+B)"); 
  
  TCanvas tmp1;
  tmp1.SetFillColor(kWhite);
  tmp1.cd();

  gr1->SetMarkerStyle(20);
  gr1->SetMarkerColor(kRed);
  gr1->Draw("AP");

  tmp1.Print(flag+"opt"+optVar+".root","root");

  cout << "######### Optimization cuts ########" << endl;
  for(Int_t j=0; j<n_signal; j++){
    cout << "The best cut for " << optVar << " is " << cut_max[j] << " for " << m[j] << " GeV" << endl;
    cout << "Statistical significance is " << signifi_max[j] << endl;    
  }
  }  
  return;
} // end of program

void MuMuCutOptimizer::saveHistos(TFile * f1)
{
  f1->cd();

  hIsoVar03_glb_TKECAL->Write();
  hIsoVar03_glb_TKECALHCAL->Write();
  hIsoVar03_trk_TKECAL->Write();
  hIsoVar03_trk_TKECALHCAL->Write();

  hIsoVar03_glb_TKECAL_pt->Write();
  hIsoVar03_glb_TKECALHCAL_pt->Write();
  hIsoVar03_trk_TKECAL_pt->Write();
  hIsoVar03_trk_TKECALHCAL_pt->Write();

  hIsoVar03_glb_TK->Write();
  hIsoVar03_glb_ECAL->Write();
  hIsoVar03_glb_HCAL->Write();
  hIsoVar03_trk_TK->Write();
  hIsoVar03_trk_ECAL->Write();
  hIsoVar03_trk_HCAL->Write();
  hQQProbChi2->Write();
  hQQSTip->Write();
  hQQS3Dip->Write();
  hQQEta->Write()   ;
  hQQPt->Write()    ;
  hMuEtaTk->Write()   ;
  hMuPtTk->Write()    ;
  hMuEtaGlb ->Write()  ;
  hMuPtGlb->Write()    ;
  hMuPGlb->Write();
  hMuPTk->Write();
  hInvMass_all->Write();

  for(int i=0;i<n_step;i++) hInvMass[i]->Write();

  f1->Close();

  return;
}

bool MuMuCutOptimizer::accept_glb_mu(const int mu_index, float cut, TString optVar) const
{
  TLorentzVector *mu_4mom = (TLorentzVector*)muons_glb_4mom->At(mu_index);

  Float_t isoVar = (muons_glb_tkIsoR03->at(mu_index) + muons_glb_emIsoR03->at(mu_index) + muons_glb_hadIsoR03->at(mu_index))/mu_4mom->Pt();

  if(muons_glb_normChi2->at(mu_index) < MAX_normchi2_glb && isoVar < MAX_muisol && mu_4mom->Pt() > minPt && fabs(mu_4mom->Eta()) <= 2.4){
    
    if (optVar.Contains("Iso"))
      if(isoVar < cut)
	return true;
      else return false;
    
    else  return true;   
  }
  return false;
}

bool MuMuCutOptimizer::accept_trk_mu(const int mu_index, float cut, TString optVar) const
{
  TLorentzVector *mu_4mom = (TLorentzVector*)muons_trk_4mom->At(mu_index);

  Float_t isoVar = (muons_trk_tkIsoR03->at(mu_index) + muons_trk_emIsoR03->at(mu_index) + muons_trk_hadIsoR03->at(mu_index))/mu_4mom->Pt();
  
  if(((((int)muons_trk_PIDmask->at(mu_index)) & (int)pow(2,5))/(int)pow(2,5) > 0 || (((int)muons_trk_PIDmask->at(mu_index)) & (int)pow(2,8))/(int)pow(2,8) > 0) && 
     isoVar < MAX_muisol && mu_4mom->Pt() > minPt && fabs(mu_4mom->Eta()) <= 2.4){
    if(fabs(mu_4mom->Eta()) > 1.1 && mu_4mom->Rho() < MIN_muP) return false;
    
    if(optVar.Contains("PFwdTk"))
      if( fabs(mu_4mom->Eta()) > 1.1 && mu_4mom->Rho() < cut) return false;
      else return true;

    else if (optVar.Contains("Iso"))
      if( isoVar < cut )
	return true;
      else return false;
    
    else return true;
  }
  
  return false;
}

bool MuMuCutOptimizer::passCuts(int iqq, float cut, TString optVar) const
{    
  if (QQ_sign->at(iqq) == 0 && QQ_type->at(iqq) == 1 ) {//check if GLB-GLB
    
    int thehptMu = QQ_lepone->at(iqq);    if (thehptMu >= muons_glb_charge->size()) return false;
    int thelptMu = QQ_leptwo->at(iqq);    if (thelptMu >= muons_glb_charge->size()) return false;
    
    if( ((TLorentzVector*)muons_glb_4mom->At(QQ_lepone->at(iqq))) -> Pt() < ((TLorentzVector*)muons_glb_4mom->At(QQ_leptwo->at(iqq))) -> Pt() )
      {
	int tempiMu = thelptMu ;
	thelptMu = thehptMu ;
	thehptMu = tempiMu ;
      }
    
    if( QQ_probChi2->at(iqq) > MIN_vtxprob && QQ_S3Dip->at(iqq) < MAX_S3Dip ){
      
      if(accept_glb_mu(thehptMu, cut, optVar) && accept_glb_mu(thelptMu, cut, optVar)){
	
	if(optVar.Contains("Chi2"))
	  if(QQ_probChi2->at(iqq) > cut)
	    return true; 
	  else return false;

	if(optVar.Contains("S3Dip"))
	  if(QQ_S3Dip->at(iqq) < cut)
	    return true;
	  else return false;
	
	return true;
      }
    }
  }
  if(QQ_sign->at(iqq) == 0 && QQ_type->at(iqq) == 2 ){

    int thehptMu = QQ_lepone->at(iqq);    if (thehptMu >= muons_glb_charge->size()) return false;
    int thelptMu = QQ_leptwo->at(iqq);    if (thelptMu >= muons_trk_charge->size()) return false; 
    
    if( QQ_probChi2->at(iqq) > MIN_vtxprob && QQ_S3Dip->at(iqq) < MAX_S3Dip ){
      
      if(accept_glb_mu(thehptMu, cut, optVar) && accept_trk_mu(thelptMu, cut, optVar)){
	
	if(optVar.Contains("Chi2"))
	  if(QQ_probChi2->at(iqq) > cut)
	    return true;
	  else return false;

	if(optVar.Contains("S3Dip"))
	  if(QQ_S3Dip->at(iqq) < cut)
	    return true;
	  else return false;

	return true;
      }
    }
  }
  else return false;

} 			       

void MuMuCutOptimizer::fillTrkMuonHistos(const int mu_index, float weight) const
{
  TLorentzVector *mu_4mom = (TLorentzVector*)muons_trk_4mom->At(mu_index);

  hIsoVar03_trk_TKECAL->Fill((muons_trk_tkIsoR03->at(mu_index)+muons_trk_emIsoR03->at(mu_index)),weight);
  hIsoVar03_trk_TKECALHCAL->Fill((muons_trk_tkIsoR03->at(mu_index)+muons_trk_emIsoR03->at(mu_index)+muons_trk_hadIsoR03->at(mu_index)),weight);

  hIsoVar03_trk_TKECAL_pt->Fill((muons_trk_tkIsoR03->at(mu_index)+muons_trk_emIsoR03->at(mu_index))/mu_4mom->Pt(),weight);
  hIsoVar03_trk_TKECALHCAL_pt->Fill((muons_trk_tkIsoR03->at(mu_index)+muons_trk_emIsoR03->at(mu_index)+muons_trk_hadIsoR03->at(mu_index))/mu_4mom->Pt(),weight);

  hIsoVar03_trk_TK->Fill(muons_trk_tkIsoR03->at(mu_index)/mu_4mom->Pt(),weight);
  hIsoVar03_trk_ECAL->Fill(muons_trk_emIsoR03->at(mu_index)/mu_4mom->Pt(),weight);
  hIsoVar03_trk_HCAL->Fill(muons_trk_hadIsoR03->at(mu_index)/mu_4mom->Pt(),weight);

  if(((((int)muons_trk_PIDmask->at(mu_index)) & (int)pow(2,5))/(int)pow(2,5) > 0 || (((int)muons_trk_PIDmask->at(mu_index)) & (int)pow(2,8))/(int)pow(2,8) > 0)){
    hMuPtTk->Fill(mu_4mom->Pt(),weight);
    hMuEtaTk->Fill(mu_4mom->Eta(),weight);
    if(fabs(mu_4mom->Eta()) > 1.3){
      hMuPTk->Fill(mu_4mom->Rho(),weight); 
    }
  }
}

void MuMuCutOptimizer::fillGlbMuonHistos(const int mu_index, float weight) const{
  TLorentzVector *mu_4mom = (TLorentzVector*)muons_glb_4mom->At(mu_index);
  
  hIsoVar03_glb_TKECAL->Fill((muons_glb_tkIsoR03->at(mu_index)+muons_glb_emIsoR03->at(mu_index)),weight);
  hIsoVar03_glb_TKECALHCAL->Fill((muons_glb_tkIsoR03->at(mu_index)+muons_glb_emIsoR03->at(mu_index)+muons_glb_hadIsoR03->at(mu_index)),weight);

  hIsoVar03_glb_TKECAL_pt->Fill((muons_glb_tkIsoR03->at(mu_index)+muons_glb_emIsoR03->at(mu_index))/mu_4mom->Pt(),weight);
  hIsoVar03_glb_TKECALHCAL_pt->Fill((muons_glb_tkIsoR03->at(mu_index)+muons_glb_emIsoR03->at(mu_index)+muons_glb_hadIsoR03->at(mu_index))/mu_4mom->Pt(),weight);

  hIsoVar03_glb_TK->Fill(muons_glb_tkIsoR03->at(mu_index)/mu_4mom->Pt(),weight);
  hIsoVar03_glb_ECAL->Fill(muons_glb_emIsoR03->at(mu_index)/mu_4mom->Pt(),weight);
  hIsoVar03_glb_HCAL->Fill(muons_glb_hadIsoR03->at(mu_index)/mu_4mom->Pt(),weight);
  
  if(muons_glb_normChi2->at(mu_index) < MAX_normchi2_glb){
    hMuPtGlb->Fill(mu_4mom->Pt(),weight);
    hMuEtaGlb->Fill(mu_4mom->Eta(),weight); 
    if(fabs(mu_4mom->Eta()) > 1.3){
      hMuPGlb->Fill(mu_4mom->Rho(),weight); 
    }
  }
}
