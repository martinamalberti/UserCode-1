// $Id: VBFEleIDMeter.cc,v 1.12 2008/01/24 10:03:05 govoni Exp $
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "HiggsAnalysis/VBFHiggsToWW2e/interface/VBFEleIDMeter.h"
#include "HiggsAnalysis/VBFHiggsToWW2e/interface/VBFUtils.h"
//#include "DataFormats/EgammaCandidates/interface/Electron.h"
//#include "DataFormats/EgammaCandidates/interface/SiStripElectron.h"
//#include "DataFormats/EgammaCandidates/interface/PixelMatchElectron.h"
#include <DataFormats/RecoCandidate/interface/RecoChargedCandidate.h>
#include <DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h>
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include <TLorentzVector.h>
#include <Math/VectorUtil.h>


VBFEleIDMeter::VBFEleIDMeter (const edm::ParameterSet& iConfig) :
  m_GSFInputTag (iConfig.getParameter<edm::InputTag> ("GSFInputTag")) ,
  m_MCtruthInputTag (iConfig.getParameter<edm::InputTag> ("MCtruthInputTag")) ,
  m_eleIDPTDRLooseInputTag (iConfig.getParameter<edm::InputTag> ("eleIDPTDRLooseInputTag")) ,
  m_eleIDPTDRMediumInputTag (iConfig.getParameter<edm::InputTag> ("eleIDPTDRMediumInputTag")) ,
  m_eleIDPTDRTightInputTag (iConfig.getParameter<edm::InputTag> ("eleIDPTDRTightInputTag")) ,
  m_eleIDOTHERRobustInputTag (iConfig.getParameter<edm::InputTag> ("eleIDOTHERRobustInputTag")) ,
  m_eleIDOTHERLooseInputTag (iConfig.getParameter<edm::InputTag> ("eleIDOTHERLooseInputTag")) ,
  m_eleIDOTHERTightInputTag (iConfig.getParameter<edm::InputTag> ("eleIDOTHERTightInputTag")) 
{}


// --------------------------------------------------------------------


VBFEleIDMeter::~VBFEleIDMeter ()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


// --------------------------------------------------------------------


void
VBFEleIDMeter::analyze (const edm::Event& iEvent, 
                             const edm::EventSetup& iSetup)
{

  //PG MC thruth candidates collection  
  edm::Handle<CandidateCollection> genParticles;
  iEvent.getByLabel(m_MCtruthInputTag, genParticles);

  std::vector<TLorentzVector> MCelectrons ; 

  //PG loop over generated particles
  for (CandidateCollection::const_iterator p = genParticles->begin () ; 
       p != genParticles->end () ; 
       ++p) 
    {
        int mumPDG = p->pdgId () ;
        int mumSTATUS = p->status () ;
        if (abs (mumPDG) == 24 &&  mumSTATUS == 3) //W�
            {
                for ( size_t i = 0; i < p->numberOfDaughters () ; ++ i ) 
                    {
                        const Candidate * daughter = p->daughter ( i ) ;
                        int PDG = daughter -> pdgId () ;    
                        if (abs (PDG) == 11) 
                          {
                            TLorentzVector dummy ;
                            vbfhww2l::setMomentum (dummy, *daughter) ;
                            MCelectrons.push_back (dummy) ;
                          }
                    } //PG loop over daughters
            } //PG if W        
    } //PG loop over generated particles

  //PG get the GSF electrons collection
  edm::Handle<reco::PixelMatchGsfElectronCollection> GSFHandle ;
  iEvent.getByLabel (m_GSFInputTag , GSFHandle) ; 

  //PG get the electron ID collections
  std::vector<edm::Handle<reco::ElectronIDAssociationCollection> > eleIdHandles ;
  for (int i=0 ; i<6 ; ++i)
    {
      edm::Handle<reco::ElectronIDAssociationCollection> dummy ;
      eleIdHandles.push_back (dummy) ;
    }
  iEvent.getByLabel (m_eleIDPTDRLooseInputTag, eleIdHandles[0]);
  iEvent.getByLabel (m_eleIDPTDRMediumInputTag, eleIdHandles[1]);
  iEvent.getByLabel (m_eleIDPTDRTightInputTag, eleIdHandles[2]);
  iEvent.getByLabel (m_eleIDOTHERRobustInputTag, eleIdHandles[3]);
  iEvent.getByLabel (m_eleIDOTHERLooseInputTag, eleIdHandles[4]);
  iEvent.getByLabel (m_eleIDOTHERTightInputTag, eleIdHandles[5]);

//  if (MCelectrons.size () > 2)
//    {
//      edm::LogError ("reading") << "There are " << MCelectrons.size () 
//                                << "MC electrons, skipping" ;
//      std::cerr << "There are " << MCelectrons.size () 
//                << "MC electrons, skipping" ;
//      return ;                                
//    } 
  if (MCelectrons.size () == 0) return ; 

  std::map<int,int> matched ;
  match (matched, GSFHandle, MCelectrons) ;

  std::vector<int> eleIDsChoiceFlag (6,0) ; //PG a che serve questo?

  //PG loop over ID electrons
  for (int eleId = 0 ; eleId < MCelectrons.size () ; ++eleId)
    {
      if (matched.count (eleId) == 0) continue ;
 
//      double  elePT  =  gsfTrack->pt () ; 
//      double  eleEta =  (*rawGSFHandle)[i].eta () ;
//      double  elePhi =  (*rawGSFHandle)[i].phi () ;
      double eta = MCelectrons[eleId].Eta () ;
      double pt = MCelectrons[eleId].Pt () ;

      reco::PixelMatchGsfElectronRef ref (GSFHandle, matched[eleId]) ;
      reco::ElectronIDAssociationCollection::const_iterator electronIDAssocItr ;
      //PG loop over the eleID
      for (int i=0 ; i<6 ; ++i)
        {
          electronIDAssocItr = eleIdHandles[i]->find (ref) ;
          if (electronIDAssocItr != eleIdHandles[i]->end ())
            {
              const reco::ElectronIDRef & id = electronIDAssocItr->val ;
              bool cutBasedID = id->cutBasedDecision () ;
              if (cutBasedID == 1) eleIDsChoiceFlag[i] = 1 ;
              else eleIDsChoiceFlag[i] = 0 ;
              m_effVSPt[i]->Fill (pt, eleIDsChoiceFlag[i]) ;
              m_effVSEta[i]->Fill (fabs (eta), eleIDsChoiceFlag[i]) ;
            }
        } //PG loop over the eleID
    } //PG loop over ID electrons

}


// --------------------------------------------------------------------


void 
VBFEleIDMeter::beginJob (const edm::EventSetup&)
{
  edm::Service<TFileService> fs ;

  m_deltaR[0] = fs->make<TH1F> ("m_deltaR_PTDRLoose","m_deltaR_PTDRLoose",100,0,2) ;
  m_deltaR[1] = fs->make<TH1F> ("m_deltaR_PTDRMedium","m_deltaR_PTDRMedium",100,0,2) ;
  m_deltaR[2] = fs->make<TH1F> ("m_deltaR_PTDRTight","m_deltaR_PTDRTight",100,0,2) ;
  m_deltaR[3] = fs->make<TH1F> ("m_deltaR_OTHERRobust","m_deltaR_OTHERRobust",100,0,2) ;
  m_deltaR[4] = fs->make<TH1F> ("m_deltaR_OTHERLoose","m_deltaR_OTHERLoose",100,0,2) ;
  m_deltaR[5] = fs->make<TH1F> ("m_deltaR_OTHERTight","m_deltaR_OTHERTight",100,0,2) ;
  
  m_effVSPt[0] = fs->make<TH2F> ("m_effVSPt_PTDRLoose","m_effVSPt_PTDRLoose",50,0,500,50,0,1.1) ;
  m_effVSPt[1] = fs->make<TH2F> ("m_effVSPt_PTDRMedium","m_effVSPt_PTDRMedium",50,0,500,50,0,1.1) ;
  m_effVSPt[2] = fs->make<TH2F> ("m_effVSPt_PTDRTight","m_effVSPt_PTDRTight",50,0,500,50,0,1.1) ;
  m_effVSPt[3] = fs->make<TH2F> ("m_effVSPt_OTHERRobust","m_effVSPt_OTHERRobust",50,0,500,50,0,1.1) ;
  m_effVSPt[4] = fs->make<TH2F> ("m_effVSPt_OTHERLoose","m_effVSPt_OTHERLoose",50,0,500,50,0,1.1) ;
  m_effVSPt[5] = fs->make<TH2F> ("m_effVSPt_OTHERTight","m_effVSPt_OTHERTight",50,0,500,50,0,1.1) ;     

  m_effVSEta[0] = fs->make<TH2F> ("m_effVSEta_PTDRLoose","m_effVSEta_PTDRLoose",85,0,3,50,0,1.1) ;
  m_effVSEta[1] = fs->make<TH2F> ("m_effVSEta_PTDRMedium","m_effVSEta_PTDRMedium",85,0,3,50,0,1.1) ;
  m_effVSEta[2] = fs->make<TH2F> ("m_effVSEta_PTDRTight","m_effVSEta_PTDRTight",85,0,3,50,0,1.1) ;
  m_effVSEta[3] = fs->make<TH2F> ("m_effVSEta_OTHERRobust","m_effVSEta_OTHERRobust",85,0,3,50,0,1.1) ; 
  m_effVSEta[4] = fs->make<TH2F> ("m_effVSEta_OTHERLoose","m_effVSEta_OTHERLoose",85,0,3,50,0,1.1) ; 
  m_effVSEta[5] = fs->make<TH2F> ("m_effVSEta_OTHERTight","m_effVSEta_OTHERTight",85,0,3,50,0,1.1) ;
}


// --------------------------------------------------------------------


void 
VBFEleIDMeter::endJob () {}


// --------------------------------------------------------------------


void 
VBFEleIDMeter::match (VBFEleIDMeter::matchColl & matched,
                      edm::Handle<reco::PixelMatchGsfElectronCollection>& GSFHandle,
                      const std::vector<TLorentzVector>& MCelectrons) 
{
  std::map<int,int> tempo ;
  
  //PG loop over MC electrons
  for (std::vector<TLorentzVector>::const_iterator itMC = MCelectrons.begin () ;
       itMC != MCelectrons.end () ;
       ++itMC)
    {    
      double deltaMin = 999. ;
      int indexMin = GSFHandle->size () ;

      //PG loop over GSF electrons
      for (int i = 0; i < GSFHandle->size () ; ++i)
        {
          double delta = ROOT::Math::VectorUtil::DeltaR ((*GSFHandle)[i].momentum (), *itMC) ;
          if (delta < deltaMin)
            {
              indexMin = i ;
              deltaMin = delta ;            
            }
        } //PG loop over GSF electrons
      if (indexMin < GSFHandle->size () && tempo.count (indexMin) == 0) 
        tempo[indexMin] = itMC - MCelectrons.begin () ;
    } //PG loop over MC electrons

  for (std::map<int,int>::const_iterator itTempo = tempo.begin () ;
       itTempo != tempo.end () ;
       ++itTempo)
    {
      matched[itTempo->second] = itTempo->first ;      
    }   

}
