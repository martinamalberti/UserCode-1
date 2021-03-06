#ifndef EleEleLooper_h
#define EleEleLooper_h

#include <TVector3.h>
#include <TH1F.h>
#include <TH2F.h>


#include "smallHBaseClass.h"
#include <vector>
#include "hChain.h"

class EleEleLooper : public smallHBaseClass{
public:
  
  EleEleLooper(TChain *tree=0);
  virtual ~EleEleLooper() {};
  void bookHistos();
  void saveHistos(TFile *f1);
  void Loop(string filename);
  bool accept_ele(const int ele_index) const;
  bool accept_QQ(const int QQ_index) const;
  
  
private:
  
  int n_steps;

  float MAX_QQ_S3Dip;
  float MIN_probChi2;

  float WIN_EoP;
  float WIN_dEtaSc;
  float WIN_dPhiSc;
  float MAX_SEtaEta;
  float MAX_EIso;
  float MIN_EIdBDT; 
  float MAX_HoE;
  float MAX_tkIso;
  float MAX_emIso;
  float MAX_hadIso;
  
  float MAX_fbrem;
  float WIN_EseedOP;
  float MIN_PtEle;
  float WIN_SigiEtaiEtaSc;


  float MAX_nLostHits;
  float MIN_flag_mask;
 
  hChain *hC_InvMass_OS;            
  hChain *hC_InvMassSC_OS;            
  hChain *hC_InvMass_SS;            
  hChain *hC_InvMassSC_SS;            
  hChain *hC_InvMass_DF;            

  TH1F *h_StepSummary;
  
};
#endif

