#include "HiggsAnalysis/VBFHiggsToWWto2l2nu/interface/MCAnalysis.h"

using namespace vbfhww2l;



// -----------------------------------------------------------------
// ctor
// -----------------------------------------------------------------

MCAnalysis::MCAnalysis(edm::Handle<reco::GenParticleCollection>& hGenParticles,
                                         edm::ESHandle<ParticleDataTable>& hPdt,
                                         const bool& verbosity):
 verbosity_p(verbosity),
 isValid_p(true),
 mcHDecayMode_p(""),
 mcHDecayId_p(-1),
 mcV1DecayMode_p(""),
 mcV1DecayId_p(-1),
 mcV2DecayMode_p(""),
 mcV2DecayId_p(-1),
 mcH_p(0),
 mcV1_p(0),
 mcV2_p(0),
 mcF_fromV1_p(0),
 mcFbar_fromV1_p(0),
 mcF_fromV2_p(0),
 mcFbar_fromV2_p(0),
 mcJ_fromV1_p(0),
 mcInvJ_fromV1_p(0),
 mcJbar_fromV1_p(0),
 mcInvJbar_fromV1_p(0),
 mcJ_fromV2_p(0),
 mcInvJ_fromV2_p(0),
 mcJbar_fromV2_p(0),
 mcInvJbar_fromV2_p(0)
{
  if(verbosity_p)
    std::cerr << "*** MCAnalysis::ctor ***" << std::endl;
  
  Analyze(hGenParticles, hPdt);
}






// -----------------------------------------------------------------
// dtor
// -----------------------------------------------------------------

MCAnalysis::~MCAnalysis()
{
  if(verbosity_p)
    std::cerr << "*** MCAnalysis::dtor ***" << std::endl;
}






// -----------------------------------------------------------------
// Analyze
// -----------------------------------------------------------------

void MCAnalysis::Analyze(edm::Handle<reco::GenParticleCollection>& hGenParticles,
                         edm::ESHandle<ParticleDataTable>& hPdt)
{
  if(verbosity_p)
    std::cerr << "*** MCAnalysis::Analyze ***" << std::endl;
  
  
  
  // loop over gen particles
  int counter = 0;
  for(reco::GenParticleCollection::const_iterator p = hGenParticles -> begin();
      p != hGenParticles -> end(); ++p)
  {
    ++counter;
    
    const reco::Candidate* pCurrent = &(*p);
    const reco::Candidate* pMother = 0;
    if(pCurrent -> mother())
      pMother = pCurrent -> mother();
    int pdgId  = p -> pdgId();
    int status = p -> status();
    int charge = p -> charge();
    int motherPdgId = 0;
    if(pCurrent -> mother())
      motherPdgId = pMother -> pdgId();
    
        
    // charged
    if( (abs(charge) != 0) && (status == 1) )
      mcCharged_p.push_back(pCurrent);
    
    // electrons
    if( (abs(pdgId) == 11) && (status == 1) )
      mcEle_p.push_back(pCurrent);
    
    // muons
    if( (abs(pdgId) == 13) && (status == 1) )
      mcMu_p.push_back(pCurrent);
    
    // taus and tauJ
    if( (abs(pdgId) == 16) && (status == 1) && (abs(motherPdgId) == 15))
    {
      mcTau_p.push_back(pMother);
      
      std::vector<const reco::Candidate*> tauJBuffer;
      std::vector<const reco::Candidate*> nuBuffer;
      FindTauDecayParticles(tauJBuffer, nuBuffer, pMother, hPdt, false);
      
      mcTauJ_p.push_back(tauJBuffer);
    }
    
    // higgs
    if( (pdgId == 25) && (status == 3) ) //---- AM -> status == 2 |-> status == 3
    {
      mcH_p = pCurrent;
    }
    
  } // loop over gen particles



  
  
  
  // check if it is signal
  if(!mcH_p)
  {
    if(verbosity_p)
      std::cerr << ">>> MCAnalysis::Analyze::Warning: no H in the event" << std::endl;
    
    mcHDecayMode_p = "bkg";
    
    isValid_p = false;
    return;
  }
  // check if it is signal
  
  



  
  // find vector bosons
  std::vector<const reco::Candidate*> VBuffer;
  FindHDecayParticles(VBuffer, &mcH_p, hPdt, false);
  
  if(VBuffer.size() != 2)
  {
     std::cerr << "MCAnalysis::Analyze::Warning: not a H -> VV decay" << std::endl;

     isValid_p = false;
     return;
  }
  
  mcV1_p = VBuffer.at(0);
  mcV2_p = VBuffer.at(1);

  mcHDecayId_p = FindHDecayMode(mcHDecayMode_p, mcV1_p, mcV2_p);
  // find vector bosons
  
  



  
  // find fermions from vector boson decay
  std::vector<const reco::Candidate*> fFromV1Buffer;
  FindVDecayParticles(fFromV1Buffer, &mcV1_p, hPdt, false);

  if(fFromV1Buffer.size() != 2)
  {
     std::cerr << "MCAnalysis::Analyze::Warning: not a V1 -> ff decay" << std::endl;

     isValid_p = false;
     return;
  }
  
  if(fFromV1Buffer.at(0) -> pdgId() > 0)  
    mcF_fromV1_p    = fFromV1Buffer.at(0);
  else
    mcFbar_fromV1_p    = fFromV1Buffer.at(0);

  if(fFromV1Buffer.at(1) -> pdgId() < 0)  
    mcFbar_fromV1_p    = fFromV1Buffer.at(1);
  else
    mcF_fromV1_p    = fFromV1Buffer.at(1);

  mcV1DecayId_p = FindVDecayMode(mcV1DecayMode_p, mcF_fromV1_p, mcFbar_fromV1_p);


  std::vector<const reco::Candidate*> fFromV2Buffer;
  FindVDecayParticles(fFromV2Buffer, &mcV2_p, hPdt, false);

  if(fFromV2Buffer.size() != 2)
  {
     std::cerr << "MCAnalysis::Analyze::Warning: not a V2 -> ff decay" << std::endl;

     isValid_p = false;
     return;
  }
  
  if(fFromV2Buffer.at(0) -> pdgId() > 0)  
    mcF_fromV2_p    = fFromV2Buffer.at(0);
  else
    mcFbar_fromV2_p    = fFromV2Buffer.at(0);

  if(fFromV2Buffer.at(1) -> pdgId() < 0)  
    mcFbar_fromV2_p    = fFromV2Buffer.at(1);
  else
    mcF_fromV2_p    = fFromV2Buffer.at(1);
  
  mcV2DecayId_p = FindVDecayMode(mcV2DecayMode_p, mcF_fromV2_p, mcFbar_fromV2_p);  
  
  
  
  
  

  // sort vector bosons
  if( ( (mcV1DecayMode_p == "d_dbar") || (mcV1DecayMode_p == "u_ubar") ||
        (mcV1DecayMode_p == "s_sbar") || (mcV1DecayMode_p == "c_cbar") ||
        (mcV1DecayMode_p == "b_bbar") || (mcV1DecayMode_p == "t_tbar") ) &&
      ( (mcV2DecayMode_p == "e_ebar")     || (mcV1DecayMode_p == "nue_nuebar") ||
        (mcV2DecayMode_p == "mu_mubar")   || (mcV1DecayMode_p == "numu_numubar") ||
        (mcV2DecayMode_p == "tau_taubar") || (mcV1DecayMode_p == "nutau_nutaubar") ) )
  {
    const reco::Candidate* buffer;
    
    buffer = mcV1_p;
    mcV1_p = mcV2_p;
    mcV2_p = buffer;

    buffer = mcF_fromV1_p;
    mcF_fromV1_p = mcF_fromV2_p;
    mcF_fromV2_p = buffer;
    
    buffer = mcFbar_fromV1_p;
    mcFbar_fromV1_p = mcFbar_fromV2_p;
    mcFbar_fromV2_p = buffer;
  }

  mcV1DecayId_p = FindVDecayMode(mcV1DecayMode_p, mcF_fromV1_p, mcFbar_fromV1_p);
  mcV2DecayId_p = FindVDecayMode(mcV2DecayMode_p, mcF_fromV2_p, mcFbar_fromV2_p);
  
  



  // find jets from fermion decay
  std::vector<const reco::Candidate*> jFromV1Buffer;
  std::vector<const reco::Candidate*> invJFromV1Buffer;
  std::vector<const reco::Candidate*> jbarFromV1Buffer;
  std::vector<const reco::Candidate*> invJbarFromV1Buffer;
  FindFDecayJets(jFromV1Buffer, invJFromV1Buffer, &mcF_fromV1_p, hPdt, false);
  FindFDecayJets(jbarFromV1Buffer, invJbarFromV1Buffer, &mcFbar_fromV1_p, hPdt, false);

  std::vector<const reco::Candidate*> jFromV2Buffer;
  std::vector<const reco::Candidate*> invJFromV2Buffer;
  std::vector<const reco::Candidate*> jbarFromV2Buffer;
  std::vector<const reco::Candidate*> invJbarFromV2Buffer;
  FindFDecayJets(jFromV2Buffer, invJFromV2Buffer, &mcF_fromV2_p, hPdt, false);
  FindFDecayJets(jbarFromV2Buffer, invJbarFromV2Buffer, &mcFbar_fromV2_p, hPdt, false);


//   //PrintDaughterParticles(mcF_fromV1_p, hPdt); 
//   //PrintDaughterParticles(mcFbar_fromV1_p, hPdt); 
//   //PrintDaughterParticles(mcF_fromV2_p, hPdt); 
//   //PrintDaughterParticles(mcFbar_fromV2_p, hPdt); 



//   if(false)
//   {  
//     std::cout << "********SUMMARY**********" << std::endl;
//     std::cout << "Higgs:   H -> " << mcHDecayMode_p << std::endl;
//     PrintParticleInfo(mcH_p, hPdt);
    
//     std::cout << "Vector bosons:   V1 -> " << mcV1DecayMode_p << "      V2 ->" << mcV2DecayMode_p << std::endl;
//     PrintParticleInfo(mcV1_p, hPdt);
//     PrintParticleInfo(mcV2_p, hPdt);
    
//     std::cout << "Fermions from vector bosons: " << std::endl;
    
//     PrintParticleInfo(mcF_fromV1_p, hPdt);
// //   for(unsigned int ii = 0; ii < jFromV1Buffer.size(); ++ii)
// //   {
// //     std::cout << "***    j = ";
// //     PrintParticleInfo(jFromV1Buffer.at(ii), hPdt);
// //   }
// //   for(unsigned int ii = 0; ii < invJFromV1Buffer.size(); ++ii)
// //   {
// //     std::cout << "*** invJ = ";
// //     PrintParticleInfo(invJFromV1Buffer.at(ii), hPdt);  
// //   }

//     PrintParticleInfo(mcFbar_fromV1_p, hPdt);
// //   for(unsigned int ii = 0; ii < jbarFromV1Buffer.size(); ++ii)
// //   {
// //     std::cout << "***    jbar = ";
// //     PrintParticleInfo(jbarFromV1Buffer.at(ii), hPdt);
// //   }
// //   for(unsigned int ii = 0; ii < invJbarFromV1Buffer.size(); ++ii)
// //   {
// //     std::cout << "***    invJbar = ";
// //     PrintParticleInfo(invJbarFromV1Buffer.at(ii), hPdt);
// //   }

//      PrintParticleInfo(mcF_fromV2_p, hPdt);
//      PrintParticleInfo(mcFbar_fromV2_p, hPdt);
//   }






  return;
}






// -----------------------------------------------------------------
// PrintDecayTree
// -----------------------------------------------------------------

void MCAnalysis::PrintHDecayTree(edm::ESHandle<ParticleDataTable>& hPdt)
{
  if(mcH_p)
    PrintDecayTree(mcH_p, hPdt);

  return;
}






// -----------------------------------------------------------------
// PrintH
// -----------------------------------------------------------------

void MCAnalysis::PrintHInfo(edm::ESHandle<ParticleDataTable>& hPdt)
{
  std::cout << "\n>>> MCAnalysis::PrintHInfo \n" << std::endl;
  
  std::cout << "Higgs:           H  -> " << mcHDecayMode_p << std::endl;
  PrintParticleInfo(mcH_p, hPdt);
  std::cout << std::endl;
  
  std::cout << "Vector bosons:   V1 -> " << mcV1DecayMode_p << std::endl;
  PrintParticleInfo(mcV1_p, hPdt);
  
  std::cout << "Vector bosons:   V2 -> " << mcV2DecayMode_p << std::endl;
  PrintParticleInfo(mcV2_p, hPdt);
  std::cout << std::endl;
  
  std::cout << "Fermions from vector bosons: " << std::endl;
  PrintParticleInfo(mcF_fromV1_p, hPdt);
  PrintParticleInfo(mcFbar_fromV1_p, hPdt);
  PrintParticleInfo(mcF_fromV2_p, hPdt);
  PrintParticleInfo(mcFbar_fromV2_p, hPdt);
}






// -----------------------------------------------------------------
// Match fermions from V2 with jets
// -----------------------------------------------------------------

std::pair<bool, bool> MCAnalysis::GetMatchJets_fromV2(reco::GenJetCollection::const_iterator& jet1,
                                                      reco::GenJetCollection::const_iterator& jet2,
                                                      float& DRmax_mcF,
                                                      float& DRmax_mcFbar,
                                                      edm::Handle<reco::GenJetCollection>& hJets)
{
  float dphi = 0.;
  float deta = 0.;
  float DR = 0.;


 
  // loop on jets
  for(reco::GenJetCollection::const_iterator it = hJets -> begin();
      it != hJets -> end(); ++it)
  {
    // geometric match with f from V2     
    dphi = deltaPhi(it -> phi(), mcF_fromV2_p -> phi());
    deta = fabs(it -> eta() - mcF_fromV2_p -> eta());
    DR = sqrt(dphi*dphi + deta*deta);
    
    if( (DR < DRmax_mcF) )
    {
      jet1 = it;
      DRmax_mcF = DR;
    }

    // geometric match with f from V2     
    dphi = deltaPhi(it -> phi(), mcFbar_fromV2_p -> phi());
    deta = fabs(it -> eta() - mcFbar_fromV2_p -> eta());
    DR = sqrt(dphi*dphi + deta*deta);
    
    if( (DR < DRmax_mcFbar) )
    {
      jet2 = it;
      DRmax_mcFbar = DR;
    }
  
  } // loop on jets
  
  
  
  if( (jet1 != hJets -> end()) &&
      (jet2 != hJets -> end()) && 
      (jet1 != jet2) )
  {
    std::pair<bool, bool> result(true, true);
    return result;
  }

  else if( (jet1 != hJets -> end()) &&
           (jet2 == hJets -> end()) )
  {
    std::pair<bool, bool> result(true, false);
    return result;
  }

  else if( (jet1 == hJets -> end()) &&
           (jet2 != hJets -> end()) )
  {
    std::pair<bool, bool> result(false, true);
    return result;
  }

  else
  {
    std::pair<bool, bool> result(false, false);
    return result;
  }

}

