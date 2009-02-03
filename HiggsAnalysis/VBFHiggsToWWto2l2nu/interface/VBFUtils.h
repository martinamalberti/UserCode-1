#ifndef VBFUtils_h#define VBFUtils_h// -*- C++ -*-//// Package:    VBFProcessFilter// Class:      VBFProcessFilter// /*  Description: filter events based on the Pythia ProcessID and the Pt_hat
Implementation: inherits from generic EDFilter

*/
//
// $Id: VBFUtils.h,v 1.2 2008/03/18 17:44:07 govoni Exp $
//
//
// system include files
#include <memory>

#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/Common/interface/View.h"
#include "TLorentzVector.h"


//PG NAMESPACE DEFINITION
namespace vbfhww2l
{

typedef reco::CaloJetCollection::const_iterator VBFjetIt ;

void 
setMomentum (TLorentzVector & myvector, 
             const reco::Candidate & gen) ;

// --------------------------------------------------------------------------------

std::pair<VBFjetIt,VBFjetIt>	
findTagJets (VBFjetIt begin, VBFjetIt end,
             double jetPtMin, double jetEtaMax) ;

// --------------------------------------------------------------------------------

std::pair<VBFjetIt,VBFjetIt>
findMaxPtJetsPair (VBFjetIt begin, VBFjetIt end,
		   double jetPtMin, double jetEtaMax) ;

// --------------------------------------------------------------------------------

double deltaPhi (double phi1, double phi2) ;

// --------------------------------------------------------------------------------

template<typename T>
struct ptSorting
{
  typedef T first_argument_type;
  typedef T second_argument_type;
  bool operator()( const T &t1, const T &t2 ) const {
  return t1.pt()> t2.pt();
}
};

// --------------------------------------------------------------------------------

//! find a RefToBase in a edm::View
template <typename T>
typename edm::View<T>::const_iterator
findInView (typename edm::Handle<edm::View<T> > collection,
            typename edm::RefToBase<T> element) 
  {
    for (typename edm::View<T>::const_iterator it = collection->begin () ; 
         it != collection->end () ;
         ++it)
      {
        if (collection->refAt (it - collection->begin ()) == element) return it ;
      }   
    return collection->end () ;             
  }

// --------------------------------------------------------------------------------

//! find the tagging jet after selections
template <typename T>
std::pair<VBFjetIt,VBFjetIt>	
findTagJetsInSelected (VBFjetIt begin, VBFjetIt end
                       /*,selection variables*/)
  {
    std::vector<std::pair<VBFjetIt,VBFjetIt> > coupleCandidates ;

    //PG first loop over jets
    for (VBFjetIt firstJet = begin ;
         firstJet != end ;
         ++firstJet )
      {
//        if (firstJet->pt () < jetPtMin ||
//            fabs (firstJet->eta ()) > jetEtaMax) continue ;
        math::XYZTLorentzVector firstLV = firstJet->p4 () ;
        //PG second loop over jets
        for (VBFjetIt secondJet = firstJet + 1 ;
             secondJet != end ;
             ++secondJet )
          {
            if (1/* selezioni sulla coppia NON passate*/) continue ;
            coupleCandidates.push_back (
              make_pair (firstJet, secondJet)) ;
          } //PG second loop over jets
      } //PG first loop over jets
   
    sort (coupleCandidates.begin () , coupleCandidates.end () , T ()) ;
    return coupleCandidates.back () ;
  }             



// ------------------------------------------------------------------


  struct MinvSorting : public
  std::binary_function< std::pair<VBFjetIt,VBFjetIt>,
                        std::pair<VBFjetIt,VBFjetIt>, bool>
   {
     bool operator() (const std::pair<VBFjetIt,VBFjetIt> & couple1,
                      const std::pair<VBFjetIt,VBFjetIt> & couple2)
       {
         return false /* algoritmo */ ;
       }
   };


// ------------------------------------------------------------------


  struct MaxPtSorting : public
  std::binary_function< std::pair<VBFjetIt,VBFjetIt>,
                        std::pair<VBFjetIt,VBFjetIt>, bool>
   {
     bool operator() (const std::pair<VBFjetIt,VBFjetIt> & couple1,
                      const std::pair<VBFjetIt,VBFjetIt> & couple2)
       {
         return couple1.first->pt () < couple2.first->pt () &&
                couple1.second->pt () < couple2.second->pt () ;
       }
   };


// ------------------------------------------------------------------


  struct MaxPtSumSorting : public
  std::binary_function< std::pair<VBFjetIt,VBFjetIt>,
                        std::pair<VBFjetIt,VBFjetIt>, bool>
   {
     bool operator() (const std::pair<VBFjetIt,VBFjetIt> & couple1,
                      const std::pair<VBFjetIt,VBFjetIt> & couple2)
       {
         return couple1.first->pt () + couple1.second->pt () <
                couple2.first->pt () + couple2.second->pt () ;
       }
   };



} //PG namespace vbfhww2l

#endif


