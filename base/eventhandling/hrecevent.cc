#include "hrecevent.h"
#include "hades.h"
#include "haddef.h"
#include "htree.h"
#include "htrack.h"
#include "hpartialevent.h"
#include "hdebug.h"
#include "TBrowser.h"
#include "TBranchClones.h"

//*-- Author : Manuel Sanchez 
//*-- Modified : 03/05/2002 by R. Holzmann
//*-- Modified : 10/01/2001 by Manuel Sanchez
//*-- Modified : 09/03/2000 by Denis Bertini
//*-- Modified : 12/05/1999 by Manuel Sanchez
//*-- Modified : 02/11/98 by Manuel Sanchez
//*-- Modified : 24/03/98 by Manuel Sanchez
//*-- Copyright : GENP (Univ. Santiago de Compostela)

//******************************************************************************
//
// HRecEvent
// 
// A HRecEvent is an event under reconstruction, and, in particular, a 
// reconstructed event. These events can be in different reconstruction levels,
// the reconstruction level of a HRecEvent is controlled by setRecLevel() and
// getRecLevel(); the reconstruction levels are identified by global constants
// (rlRaw, rlHit ...)
//
// As for the data, a HRecEvent holds reconstructed tracks, an event header
// and a list of HPartialEvent objects.
//
// An HRecEvent can be streamed in "expanded" or normal mode. This behaviour is 
// controlled with the setExpandedStreamer method.
//
// fBits assignment:
//   Bit 0: Expanded Streamer
//
//*******************************************************************************

ClassImp(HRecEvent)

HRecEvent::HRecEvent(void) : HEvent("Event","Event under Reconstruction") {
  // Default constructor
  fRecLevel=rlUndefined;
  fTracks=new TClonesArray("HTrack");
  fPartialEvs=new TObjArray(MAX_PART_EVENTS);
  fHeader = new HEventHeader(); 
  fNTracks=0;
  setExpandedStreamer(kFALSE);
}

HRecEvent::~HRecEvent(void) {
  // Destructor
  
  if (fPartialEvs) {fPartialEvs->Delete(); delete fPartialEvs;}
  if (fTracks) {fTracks->Clear(); delete fTracks;}
  if (fHeader) {delete fHeader; fHeader=0;}
}

void HRecEvent::activateBranch(TTree *tree,Int_t splitLevel) {
  // See HEvent::activateBranch()
   HPartialEvent **par;
#if DEBUG_LEVEL > 2
   gDebuger->enterFunc("HRecEvent::activateBranch");
#endif   
   if (tree==NULL) return;

   if (splitLevel==0) {
   } else if (splitLevel==1) {

     Int_t i;
     TObjArray &vector=*fPartialEvs;
     for(i=0;i<=fPartialEvs->GetLast();i++) {
       par=(HPartialEvent **)&vector[i];
       if ((*par)!=NULL) {
#if DEBUG_LEVEL>2
	 gDebuger->message("Activating %s",(*par)->GetName());
	 gDebuger->message("fPartialEvs=%p",fPartialEvs);
#endif
	 tree->SetBranchAddress((*par)->GetName(),par);
	 tree->SetBranchStatus((*par)->GetName(),1);
       }
     }

   } else if (splitLevel==2) {
         
     Int_t i;
     TObjArray &vector=*fPartialEvs;
     TString cad;
     TBranch *brHead=0;
     TBranch *brPartial=0;

     for(i=0;i<=fPartialEvs->GetLast();i++) {
       par=(HPartialEvent **)&(vector[i]);
       if ((*par)!=NULL) {
	 cad=(*par)->GetName();
	 cad+=".";
	 brPartial = tree->GetBranch(cad.Data());
	 if (brPartial) { //Post ROOT3 splitting
	   tree->SetBranchAddress(cad.Data(),par);
	   tree->SetBranchStatus(cad.Data(),1);
	   cad+="*"; //Activate subbranches
	   tree->SetBranchStatus(cad.Data(),1);
	   (*par)->activateBranch(tree,splitLevel);
	 } else {
	   cad=(*par)->GetName();
	   brPartial = tree->GetBranch(cad.Data());
	   if (brPartial) { //Pre ROOT3 Splitting
	     tree->SetBranchAddress(cad.Data(),par);
	     tree->SetBranchStatus(cad.Data(),1);
	     cad+=".*"; //Activate subbranches
	     tree->SetBranchStatus(cad.Data(),1);
	     (*par)->activateBranch(tree,splitLevel);	     
	   } else {
	     Warning("activateBranch","Partial event %s not found in tree",
		     (*par)->GetName());
	   }
	 }
       }
     }

     brHead=tree->GetBranch("EventHeader.");
     if (brHead) { //Post ROOT3 splitting
       tree->SetBranchAddress("EventHeader.",&fHeader);
       tree->SetBranchStatus("EventHeader.",1);
       tree->SetBranchStatus("EventHeader.*",1);
     } else {
       brHead=tree->GetBranch("EventHeader");
       if (brHead) { //Pre ROOT3 splitting
         tree->SetBranchAddress("EventHeader",&fHeader);
         tree->SetBranchStatus("EventHeader",1);
         tree->SetBranchStatus("EventHeader.*",1);
       }
     }
   }
#if DEBUG_LEVEL>2
   gDebuger->leaveFunc("HRecEvent::activateBranch");
#endif
}

void HRecEvent::makeBranch(TBranch *parent, HTree* tree) {
  // See HEvent::makeBranch() 
   HPartialEvent **par;
   TBranch *b=NULL;
   Int_t sl=2;
   Char_t buffer[256];
   
#if DEBUG_LEVEL > 2
   gDebuger->enterFunc("HRecEvent::makeBranch");
#endif
   if (parent==NULL) return;

   sl=gHades->getSplitLevel();
   if (sl==0) {
   } else if (sl==1) {

     Int_t i;
     TObjArray &vector=*fPartialEvs;
     for(i=0;i<=fPartialEvs->GetLast();i++) {
       par=(HPartialEvent **)&vector[i];
       if ((*par)!=NULL) {
	 b=tree->Branch( (*par)->GetName(),
			(*par)->ClassName(),par,gHades->getTreeBufferSize(),0);
       }
     }
  
    } else if (sl==2) {

     Int_t i;
     TObjArray &vector=*fPartialEvs;
     for(i=0;i<=fPartialEvs->GetLast();i++) {
       par=(HPartialEvent **)&(vector[i]);
       if ((*par)!=NULL) {
	 sprintf(buffer,"%s.",(*par)->GetName());
	 b=tree->Branch( buffer,
			(*par)->ClassName(),par,gHades->getTreeBufferSize(),99);
	 (*par)->makeBranch(b,tree);
       }
     }
     tree->Branch("EventHeader.",fHeader->ClassName(),
		  &fHeader,gHades->getTreeBufferSize(),3);
   }
#if DEBUG_LEVEL>2
   gDebuger->leaveFunc("HRecEvent::activateBranch");
#endif
}

void HRecEvent::Clear(Option_t *) {
  // Clears the data in the event (i.e. clears the internal buffers...)
     TIter next(fPartialEvs);
     HPartialEvent *ev;
     while ( (ev=(HPartialEvent *)next())!=NULL) {
              ev->Clear();
     }
   if (fTracks) fTracks->Clear();
   fNTracks=0;
}

void HRecEvent::clearAll(Int_t level) {
  // Clears the data in the event and the event structure (list of subevents....)
  //
  //   level == 0: delete list of partial events
  //   level == 1: delete only all categories in partial events
  //   level > 1:  clear only all categories in partial events
  //
  if (level==0) {
    if (fPartialEvs) fPartialEvs->Delete();
  } else {
    TIter next(fPartialEvs);
    HPartialEvent *ev;
    while ((ev=(HPartialEvent *)next())!=NULL) {
      ev->clearAll(level);
    }
  }
  if (fTracks) fTracks->Clear();
  fNTracks=0;
}

HTrack *HRecEvent::newTrack(void) {
  // Returns a pointer to a new HTrack object.
  TClonesArray &tracks=*fTracks;
  new(tracks[fNTracks++]) HTrack;
  return ((HTrack *)tracks[fNTracks-1]);
}

void HRecEvent::addTrack(HTrack &aTrack) {
  // Adds the track aTrack to the list of reconstructed tracks.
  TClonesArray &tracks=*fTracks;
  new (tracks[fNTracks++]) HTrack(aTrack);

}

HTrack *HRecEvent::getTrack(UInt_t aId) {
  // Returns the track identified by aId (the position in the track list) 
  HTrack *track;
  track=(HTrack *)fTracks->At(aId);
  return(track);
}

void HRecEvent::clearTracks(void) {
  // Clears the track list
  fNTracks=0;
  if (fTracks) fTracks->Clear();
}

Int_t HRecEvent::getRecLevel(void) {
  // Returns the reconstruction level for this event.
 return(fRecLevel);
}

void HRecEvent::setRecLevel(Int_t aRecLevel) {
  // Sets the reconstruction level for the event.
  fRecLevel=aRecLevel;
}

HPartialEvent *HRecEvent::getPartialEvent(Cat_t idx) {
  // Returns a pointer to the partial event with number idx.
  if ((idx>>kBitCategorySize)>fPartialEvs->GetLast()) return NULL;
  return ((HPartialEvent *)fPartialEvs->At(idx>>kBitCategorySize));
}


HCategory *HRecEvent::getCategory(Cat_t aCat) {
  // Returns the category identified by aCat in the correct Partial event.
  HPartialEvent *ev;
  HCategory *cat;
  ev=getPartialEvent(aCat);
  if (!ev) return NULL;
  cat=ev->getCategory(aCat);
  return cat;
}

Bool_t HRecEvent::addCategory(Cat_t aCat,HCategory *cat,Option_t opt[]) {
  // Adds a new category to the event. The partial event it belongs to is
  // determined by aCat; if this partial event doesn't exist, one is created
  // with the name given in opt.
  HPartialEvent *event=0;
  event=getPartialEvent(aCat);
  if (!event) {
    Int_t aCatBase = aCat - aCat%kCategorySize;    // extract base category
    event=addPartialEvent(aCatBase,opt,opt);
    if (!event) return kFALSE;
  }
  return event->addCategory(aCat,cat,opt);
}


Bool_t HRecEvent::removeCategory(Cat_t aCat) {
  // Removes the category aCat from the event. If aCat is the last category in
  // the corresponding partial event, then the partial event is also removed.
  HPartialEvent *event=NULL;
  event=getPartialEvent(aCat);
  if (!event) return kFALSE;
  if (!event->removeCategory(aCat)) return kFALSE;
  if (event->isEmpty()) {
    fPartialEvs->Remove(event); // May be optimized to use the index instead of the pointer
    delete event;
  }
  return kTRUE;
}

Bool_t HRecEvent::removePartialEvent(Cat_t aCat) {
  // Remove partial event aCat from event.
  HPartialEvent *partial=getPartialEvent(aCat);
  if (!partial) return kFALSE;
  fPartialEvs->Remove(partial);
  delete partial;
  return kTRUE;
}

HPartialEvent *HRecEvent::addPartialEvent(Cat_t eventCat,
					  const Text_t *name,
					  const Text_t *title) {
  // Create and Add a new HPartialEvent to the list of HPartialEvent objects
  // in the HRecEvent.
  //
  // Input:
  //  eventCat  ---> Base category for the event (i.e. for Mdc it is catMdc)
  //  name      ---> Name of the new partial event (used to build Root trees)
  //  title     ---> Title of the new partial event
   HPartialEvent *event;
   event=new HPartialEvent(name,title,eventCat);
   if (event) fPartialEvs->AddAtAndExpand(event,eventCat>>kBitCategorySize);
   return event;
}

void HRecEvent::addPartialEvent(HPartialEvent* event) {
  // Add partial event by pointer.
   fPartialEvs->AddAtAndExpand(event,event->getBaseCat()>>kBitCategorySize);
}

void HRecEvent::merge(HRecEvent *targetEv) {
  //
  // Merge this event into target event, i.e. move all partial events
  // with all their categories to target event.
  // (needed by HRootSource in merge mode)
  
   HPartialEvent *partial=0;
   Int_t nPartialEvs=fPartialEvs->GetEntriesFast();
   for (Int_t i=0;i<nPartialEvs;i++) {
     partial=(HPartialEvent*)(fPartialEvs->At(i));
     if (partial) {
        if (targetEv->getPartialEvent(partial->getBaseCat())==0)
                                   targetEv->addPartialEvent(partial);
        fPartialEvs->RemoveAt(i);
     }
   }
   
}

void HRecEvent::Browse(TBrowser *b) {
  // Event browser.
   TIter next(fPartialEvs);
   HPartialEvent *ev;
   
   if (fTracks) b->Add(fTracks,"Tracks");
   while ( (ev=(HPartialEvent *)next())!=NULL) {
      b->Add(ev);
   }
}

void HRecEvent::setExpandedStreamer(Bool_t t) {
  // If the event is in "expanded streamer" mode each partial event and category
  // will go to a different directory.
  TIter iter(fPartialEvs);
  HPartialEvent *pev=0;

  SetBit(32,t);
  while ( (pev=(HPartialEvent *)iter()) != 0) {
    pev->setExpandedStreamer(t);
  }
}

void HRecEvent::Streamer(TBuffer &R__b)
{
  // Stream an object of class HRecEvent.
  TObjArray *partialEvNames;
  Int_t nPartialEvs;
  HPartialEvent *partial=0;
  TDirectory *dir,*top=0;
  Char_t buffer[255],b2[255];

  if (R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion(); if (R__v) { }
    HEvent::Streamer(R__b);
    R__b >> fRecLevel;
    R__b >> fNTracks;
    fHeader->Streamer(R__b);
    fTracks->Streamer(R__b);

    if (hasExpandedStreamer() || R__v == 1) {
      // Version 1 is allways expanded
      // At this point fBits is already read. 
      // And so hasExpandedStreamer makes sense.
      R__b >> partialEvNames;

      top=dir=gDirectory;
      for (Int_t i=0;i<partialEvNames->GetEntriesFast();i++) {
	strcpy(buffer,((TObjString *)partialEvNames->At(i))->GetString().Data());
	if (strcmp(buffer,"none")!=0) {
	  sprintf(b2,"dir%s",buffer);
	  top->cd(b2);
	  dir=gDirectory;
	  partial=(HPartialEvent *)dir->Get(buffer);
	  fPartialEvs->AddAtAndExpand(partial,i);
	}
      }
    } else {
      if (fPartialEvs) fPartialEvs->Delete();
      R__b >> fPartialEvs;
    } 
  } else {
//    printf(" hrecevent::streamer called \n");
    R__b.WriteVersion(HRecEvent::IsA());
    HEvent::Streamer(R__b);
    R__b << fRecLevel;
    R__b << fNTracks;
    fHeader->Streamer(R__b);
    fTracks->Streamer(R__b);
    if (hasExpandedStreamer()) {
      nPartialEvs=fPartialEvs->GetEntriesFast();
      partialEvNames=new TObjArray(nPartialEvs);
      for (Int_t i=0;i<nPartialEvs;i++) {
	partial=(HPartialEvent *)fPartialEvs->At(i);
	if (partial) {
	  partialEvNames->AddAt(new TObjString(partial->GetName()),i);
	} else {
	  partialEvNames->AddAt(new TObjString("none"),i);
	}
      }
      R__b << partialEvNames;
      top=gDirectory;
      for (Int_t i=0;i<nPartialEvs;i++) {
	partial=(HPartialEvent *)fPartialEvs->At(i);
	if (partial) {
	  sprintf(buffer,"dir%s",partial->GetName());
	  if (!(gDirectory->GetKey(buffer))) {
	    dir=gDirectory->mkdir(buffer);
	    dir->cd();
	  }
	  partial->Write();
	  top->cd();
	}
      }
    } else {
      R__b << fPartialEvs;
    }

  }
}  


