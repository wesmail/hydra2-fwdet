//*-- AUTHOR : R. Holzmann
//*-- Modified : 31/03/2004 by 

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
//  HGeantFilter
//
//  Reconstructor to filter the simulated event data produced by HGeant
//  rescaling the multiplcity of particle id by acc.
//
//  This is done by throwing a rundom number for each occurance of a particle
//  id in the kine branch and deleting all associated hits from all categories
//  of type hgeantxxx.
//  The kine branch entries are not removed however, just set 'inactive'.
//
///////////////////////////////////////////////////////////////////////////////

#include "hgeantfilter.h"
#include "hades.h"
#include "hgeantkine.h"
#include "hlinkeddataobject.h"
#include "hgeantmdc.h"
#include "hgeanttof.h"
#include "hgeantshower.h"
#include "hgeantrich.h"

ClassImp(HGeantFilter)

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  This filter class does the actual filtering of HGeant hits that are to be suppressed
//  from the categories
//
HHitFilter::HHitFilter(HCategory* pCat) {
  pKineCat = pCat;
}

HHitFilter::~HHitFilter() {}

Bool_t HHitFilter::check(TObject* obj) {
// check if hit was made by a suppressed track

  Int_t track = ((HLinkedDataObject*)obj)->getTrack();             // get track number
  HGeantKine* pKine = (HGeantKine*)(pKineCat->getObject(track-1)); // get corresponding kine object
  return(!pKine->isSuppressed());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ClassImp(HGeantFilter)

HGeantFilter::HGeantFilter(Text_t *name,Text_t *title, Int_t id, Float_t acc)
                           : HReconstructor(name,title) {
  particleId = id;
  accepted = acc; 

  catKine = NULL;
  catMdc = NULL;
  catTof = NULL;
  catShower = NULL;
  catRichPhoton = NULL;
  catRichDirect = NULL;
  catRichMirror = NULL;

 }
/*
// old code ... made copy contructor private
 HGeantFilter::HGeantFilter(HGeantFilter &filter) {
  // copy constructor not implemented!
  Error("HGeantFilter","Copy constructor not defined");
}
*/
HGeantFilter::~HGeantFilter(void) {
 }

Bool_t HGeantFilter::init(void) {
  //
  // Set up pointers to the Geant cats and their iterators
  //
  HEvent* ev = (HEvent*)(gHades->getCurrentEvent());
  if (!ev) return kFALSE;
  if ((catKine = ev->getCategory(catGeantKine)) == NULL) return kFALSE;

  catMdc = ev->getCategory(catMdcGeantRaw);
  catTof = ev->getCategory(catTofGeantRaw);
  catShower = ev->getCategory(catShowerGeantRaw);
  catRichPhoton = ev->getCategory(catRichGeantRaw);
  catRichDirect = ev->getCategory(catRichGeantRaw+1);
  catRichMirror = ev->getCategory(catRichGeantRaw+2);

  return kTRUE;
}

Int_t HGeantFilter::execute(void) {
  // 
  // Do the filtering for each hit category
  // 
  HGeantKine::suppressTracks(particleId,accepted,(HLinearCategory*)catKine);

  HHitFilter filt;
  filt.setKine(catKine);

  if (catMdc) catMdc->filter(filt);
  if (catTof) catTof->filter(filt);
  if (catShower) catShower->filter(filt);
  if (catRichPhoton) catRichPhoton->filter(filt);
  if (catRichDirect) catRichDirect->filter(filt);
  if (catRichMirror) catRichMirror->filter(filt);

  return 0;
}

Bool_t HGeantFilter::reinit(void) {

  return kTRUE;
}

Bool_t HGeantFilter::finalize(void) {

  return kTRUE;
}

