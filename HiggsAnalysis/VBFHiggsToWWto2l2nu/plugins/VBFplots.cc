// $Id: VBFplots.cc,v 1.1 2008/03/17 17:01:18 govoni Exp $
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "HiggsAnalysis/VBFHiggsToWWto2l2nu/plugins/VBFplots.h"
#include <DataFormats/RecoCandidate/interface/RecoChargedCandidate.h>
#include <DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h>
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include <Math/VectorUtil.h>
#include "HiggsAnalysis/VBFHiggsToWWto2l2nu/interface/VBFUtils.h"

VBFplots::VBFplots (const edm::ParameterSet& iConfig) :
  m_jetTagsInputTag (iConfig.getParameter<edm::InputTag> ("jetTagsInputTag")) ,
  m_jetOthersInputTag (iConfig.getParameter<edm::InputTag> ("jetOthersInputTag")) ,
  m_GSFInputTag (iConfig.getParameter<edm::InputTag> ("GSFInputTag")) ,
  m_electronIDInputTag (iConfig.getParameter<edm::InputTag> ("eleIDInputTag")) ,
  m_muInputTag (iConfig.getParameter<edm::InputTag> ("muInputTag")) ,
  m_metInputTag (iConfig.getParameter<edm::InputTag> ("metInputTag")) 
{
  m_evAnalyzed = 0;
  m_evWithTags = 0;
  m_evWithOthers = 0;
}


// --------------------------------------------------------------------


VBFplots::~VBFplots ()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


// --------------------------------------------------------------------


void
VBFplots::analyze (const edm::Event& iEvent, 
                             const edm::EventSetup& iSetup)
{
  m_evAnalyzed++;
  
  //PG get the calo MET
  edm::Handle<reco::CaloMETCollection> metCollectionHandle;
  iEvent.getByLabel (m_metInputTag, metCollectionHandle);
  const CaloMETCollection *calometcol = metCollectionHandle.product();
  const CaloMET *calomet = &(calometcol->front());  

  m_met -> Fill (calomet->pt());

  // Get the tag jets
  edm::Handle<reco::RecoChargedCandidateCollection> jetTagsHandle ;
  iEvent.getByLabel (m_jetTagsInputTag, jetTagsHandle) ;

  if (jetTagsHandle->size () < 2) return ;
  if ((*jetTagsHandle)[0].p4 () == (*jetTagsHandle)[1].p4 ()) return;

  if ((*jetTagsHandle)[0].p4().Eta() * (*jetTagsHandle)[1].p4().Eta() > 0) return ; //temp to require opposite emisphere for the tag jets

  m_evWithTags++;

  // Get remaining jets
  edm::Handle<reco::CaloJetCollection> jetOthersHandle ;
  iEvent.getByLabel (m_jetOthersInputTag, jetOthersHandle) ;

  if (jetOthersHandle->size () > 0) m_evWithOthers++;

  //////////////////// Tag jets plots

  double t_EMax, t_EMin, t_PtMax, t_PtMin;
  if (((*jetTagsHandle)[0].p4 ().E () ) > ((*jetTagsHandle)[1].p4 ().E () ))
    {  
     t_EMax = ((*jetTagsHandle)[0].p4 ().E () );
     t_EMin = ((*jetTagsHandle)[1].p4 ().E () );
    }
  else  
    {
      t_EMax = ((*jetTagsHandle)[1].p4 ().E () );
      t_EMin = ((*jetTagsHandle)[0].p4 ().E () );
    }

  if (((*jetTagsHandle)[0].p4 ().Pt () ) > ((*jetTagsHandle)[1].p4 ().Pt () ))  
    {
     t_PtMax = ((*jetTagsHandle)[0].p4 ().Pt () );
     t_PtMin = ((*jetTagsHandle)[1].p4 ().Pt () );
    }
  else 
    {
      t_PtMax = ((*jetTagsHandle)[1].p4 ().Pt () );
      t_PtMin = ((*jetTagsHandle)[0].p4 ().Pt () );
    }

  m_firstEnergyTagEnergy -> Fill (t_EMax) ;
  m_secondEnergyTagEnergy -> Fill (t_EMin) ;

  double t_deltaEta = (fabs ((*jetTagsHandle)[0].p4 ().Eta () - (*jetTagsHandle)[1].p4 ().Eta ()) );
  m_deltaEta -> Fill (t_deltaEta) ;
  double t_deltaPhi = (vbfhww2l::deltaPhi ((*jetTagsHandle)[0].p4 ().Phi () , (*jetTagsHandle)[1].p4 ().Phi ()) ) ;
  m_deltaPhi -> Fill (t_deltaPhi);
  m_deltaPhiVsMinEnergyTag -> Fill (t_EMin, t_deltaPhi) ;

  LorentzVector summedTags = (*jetTagsHandle)[0].p4 () + (*jetTagsHandle)[1].p4 ();
  m_sumEta -> Fill (summedTags.Eta ()) ;
  double t_mInv = (summedTags.M ());
  m_invMass -> Fill (t_mInv) ;

  m_eta -> Fill ((*jetTagsHandle)[0].p4 ().Eta () ) ;
  m_eta -> Fill ((*jetTagsHandle)[1].p4 ().Eta () ) ;
  m_energy -> Fill ((*jetTagsHandle)[0].p4 ().E () );
  m_energy -> Fill ((*jetTagsHandle)[1].p4 ().E () );
  m_pt -> Fill ((*jetTagsHandle)[0].p4 ().Pt () );
  m_pt -> Fill ((*jetTagsHandle)[1].p4 ().Pt () );


  int t_nJet15, t_nJet20, t_nJet30;
  t_nJet20 = 0;
  t_nJet30 = 0;
  t_nJet15 = jetOthersHandle->size () ;
  m_numOthers15 -> Fill (t_nJet15) ;
  LorentzVector summedOthers (0.0, 0.0, 0.0, 0.0) ;
  LorentzVector summedOthers30 (0.0, 0.0, 0.0, 0.0) ;
  // Other jets plots
  for (reco::CaloJetCollection::const_iterator jetIt = jetOthersHandle->begin () ; 
       jetIt != jetOthersHandle->end () ; 
       ++jetIt) 
    {

      m_etaOthers -> Fill (jetIt->p4 ().Eta () ) ;
      m_energyOthers -> Fill (jetIt->p4 ().E () ) ;
      m_ptOthers -> Fill (jetIt->p4 ().Pt () ) ;

      double deltaR0 = ROOT::Math::VectorUtil::DeltaR (((*jetTagsHandle)[0].momentum ()), jetIt->momentum ()) ;
      double deltaR1 = ROOT::Math::VectorUtil::DeltaR (((*jetTagsHandle)[1].momentum ()), jetIt->momentum ()) ;

      if (deltaR1 < deltaR0)
        {
          m_deltaROthers -> Fill (deltaR1) ;
          m_deltaEtaOthers -> Fill (fabs (jetIt->p4 ().Eta () - (*jetTagsHandle)[1].p4 ().Eta () ) ) ;
        }
      else
	{
          m_deltaROthers -> Fill (deltaR0) ;
          m_deltaEtaOthers -> Fill (fabs (jetIt->p4 ().Eta () - (*jetTagsHandle)[0].p4 ().Eta () ) ) ;
	}

      summedOthers += (jetIt->p4 ());


      if ((jetIt->p4 ().Pt () ) > 20) t_nJet20++;

      if ((jetIt->p4 ().Pt () ) > 30) 
	{//remporary to cut on 30
      t_nJet30++;
      m_etaOthers30 -> Fill (jetIt->p4 ().Eta () ) ;
      m_energyOthers30 -> Fill (jetIt->p4 ().E () ) ;
      m_ptOthers30 -> Fill (jetIt->p4 ().Pt () ) ;

      double deltaR0 = ROOT::Math::VectorUtil::DeltaR (((*jetTagsHandle)[0].momentum ()), jetIt->momentum ()) ;
      double deltaR1 = ROOT::Math::VectorUtil::DeltaR (((*jetTagsHandle)[1].momentum ()), jetIt->momentum ()) ;

      if (deltaR1 < deltaR0)
	{ 
          m_deltaROthers30 -> Fill (deltaR1) ;
          m_deltaEtaOthers30 -> Fill (fabs (jetIt->p4 ().Eta () - (*jetTagsHandle)[1].p4 ().Eta () ) ) ;
	}
      else 
       {
          m_deltaROthers30 -> Fill (deltaR0) ;
          m_deltaEtaOthers30 -> Fill (fabs (jetIt->p4 ().Eta () - (*jetTagsHandle)[0].p4 ().Eta () ) ) ;
       }

      summedOthers30 += (jetIt->p4 ());
	}
    }

  m_numOthers20 -> Fill (t_nJet20) ;
  m_numOthers30 -> Fill (t_nJet30) ;

  m_etaOthersSummed -> Fill (summedOthers.Eta ()) ;
  m_energyOthersSummed -> Fill (summedOthers.E ()) ;
  m_ptOthersSummed -> Fill (summedOthers.Pt ()) ;
  m_invMassOtherSummed -> Fill (summedOthers.M ()) ;

  m_etaOthersSummed30 -> Fill (summedOthers30.Eta ()) ;
  m_energyOthersSummed30 -> Fill (summedOthers30.E ()) ;
  m_ptOthersSummed30 -> Fill (summedOthers30.Pt ()) ;
  m_invMassOtherSummed30 -> Fill (summedOthers30.M ()) ;

  m_etaSummedTagVsEtaSummedOthers -> Fill (summedTags.Eta (), summedOthers.Eta ()) ;



  //m_ntuple -> Fill (t_deltaEta, t_deltaPhi, t_mInv, t_EMax, t_PtMax, t_PtMin, t_nJet15, t_nJet20, t_nJet30) ;
}


// --------------------------------------------------------------------


void 
VBFplots::beginJob (const edm::EventSetup&)
{
  edm::Service<TFileService> fs ;

  //m_ntuple = fs->make <TNtuple> ("ntuple","Some variables","deltaEta:deltaPhi:mInv:Emax:Ptmax:Ptmin:nJet15:nJet20:nJet30");

  m_met = fs->make<TH1F> ("m_met","met",100, 0, 300) ;
  m_firstEnergyTagEnergy = fs->make<TH1F> ("m_firstEnergyTagEnergy","energy of leading tag jets",100, 0, 1200) ;
  m_secondEnergyTagEnergy = fs->make<TH1F> ("m_secondEnergyTagEnergy","energy of softer tag jets",100, 0, 1200) ;
  m_deltaPhiVsMinEnergyTag = fs->make<TH2F> ("m_deltaPhiVsMinEnergyTag","#Delta#phi vs energy of softest tag jet",100, 0, 1200,100,-6.3,6.3) ;

  m_deltaEta = fs->make<TH1F> ("m_deltaEtaTags","#Delta#eta between tag jets",50,0,10) ;
  m_deltaPhi = fs->make<TH1F> ("m_deltaPhiTags","#Delta#phi between tag jets",100,-6.3,6.3) ;
  m_sumEta = fs->make<TH1F> ("m_sumEtaTags","#Sigma#eta of tag jets",50,-12,12) ;
  m_invMass = fs->make<TH1F> ("m_invMassTags","invariant mass of tag jets",100, 0, 3500) ;

  m_eta = fs->make<TH1F> ("m_etaTags","#eta of tag jets",50,-6,6) ;
  m_energy = fs->make<TH1F> ("m_energyTags","energy of tag jets",100, 0, 1200) ;
  m_pt = fs->make<TH1F> ("m_ptTags","pt of tag jets",100, 0, 600) ;

  m_numOthers15 = fs->make<TH1F> ("m_numOthers15","# of other jets with pt > 15 ",15,0,15) ;
  m_numOthers20 = fs->make<TH1F> ("m_numOthers20","# of other jets with pt > 20",15,0,15) ;
  m_numOthers30 = fs->make<TH1F> ("m_numOthers30","# of other jets with pt > 30",15,0,15) ;

  m_etaOthers = fs->make<TH1F> ("m_etaOthers","#eta of other jets",50,-6,6) ;
  m_energyOthers = fs->make<TH1F> ("m_energyOthers","energy of other jets",100, 0, 400) ;
  m_ptOthers = fs->make<TH1F> ("m_ptOthers","pt of other jets",100, 0, 200) ;
  m_deltaROthers = fs->make<TH1F> ("m_deltaROthers","#DeltaR between tag jets and other jets",50,0,12) ;
  m_deltaEtaOthers = fs->make<TH1F> ("m_deltaEtaOthers","#Delta#eta between tag jets and other jets",50,0,12) ;
  m_etaOthersSummed = fs->make<TH1F> ("m_etaOthersSummed","#eta of summed jets",50,-6,6) ;
  m_energyOthersSummed = fs->make<TH1F> ("m_energyOthersSummed","energy of summed jets",100, 0, 4000) ;
  m_ptOthersSummed = fs->make<TH1F> ("m_ptOthersSummed","pt of summed jets",100, 0, 600) ;
  m_invMassOtherSummed = fs->make<TH1F> ("m_invMassOthersSummed","invariant mass of summed jets",100, 0, 4500) ;

  m_etaOthers30 = fs->make<TH1F> ("m_etaOthers30","#eta of other jets with pt > 30",50,-6,6) ;
  m_energyOthers30 = fs->make<TH1F> ("m_energyOthers30","energy of other jets with pt > 30",100, 0, 400) ;
  m_ptOthers30 = fs->make<TH1F> ("m_ptOthers30","pt of other jets with pt > 30",100, 0, 200) ;
  m_deltaROthers30 = fs->make<TH1F> ("m_deltaROthers30","#DeltaR between tag jets and other jets with pt > 30",50,0,12) ;
  m_deltaEtaOthers30 = fs->make<TH1F> ("m_deltaEtaOthers30","#Delta#eta between tag jets and other jets with pt > 30",50,0,12) ;
  m_etaOthersSummed30 = fs->make<TH1F> ("m_etaOthersSummed30","#eta of summed jets with pt > 30",50,-6,6) ;
  m_energyOthersSummed30 = fs->make<TH1F> ("m_energyOthersSummed30","energy of summed jets with pt > 30",100, 0, 4000) ;
  m_ptOthersSummed30 = fs->make<TH1F> ("m_ptOthersSummed30","pt of summed jets with pt > 30",100, 0, 600) ;
  m_invMassOtherSummed30 = fs->make<TH1F> ("m_invMassOthersSummed30","invariant mass of summed jets with pt > 30",100, 0, 4500) ;



  m_etaSummedTagVsEtaSummedOthers  = fs->make<TH2F> ("m_etaSummedTagVsEtaSummedOthers", "#eta summed tags vs #eta summed other jetes", 50, -6, 6, 50, -6, 6) ;
}


// --------------------------------------------------------------------


void 
VBFplots::endJob () 
{
  std::cout << "Analyzed events: " << m_evAnalyzed << std::endl;
  std::cout << "Events with 2 tag jets after EUPU cuts " << m_evWithTags << std::endl;
  std::cout << "Events with other jets after EUPU cuts " << m_evWithOthers << std::endl;
}

