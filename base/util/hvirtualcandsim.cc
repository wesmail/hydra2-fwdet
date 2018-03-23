#include "hvirtualcandsim.h"
#include "hades.h"
#include <iostream>
#include <iomanip>

using namespace std;

// ROOT's IO and RTTI stuff is added here
ClassImp(HVirtualCandSim)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HVirtualCandSim
//
// Simulation object keeping HGeant information in addition to the normal
// HVirtualCand. For documentation of enum constants see hparticledef.h
////////////////////////////////////////////////////////////////////////////////

HVirtualCandSim::~HVirtualCandSim() {}

void HVirtualCandSim::print(UInt_t selection)
{
    // print one candidate object and the flags which have been
    // already set to HVirtualCand
    //
    // print option bits
    // bit   1 : print hit indices                         (selection == 1)
    //       2 : print hit indices                         (selection == 2)
    //       3 : print chi2s and matching vars             (selection == 4)
    //       4 : print selection flags                     (selection == 8)
    //       5 : print Geant infos                         (selection == 16)


    // default : print all


//     if( (selection&0xF) > 0 ) HVirtualCand::print(selection); FIXME
//     else cout<<"    "<<" --------------------------------------------"<<endl;

    if( (selection>>4) & 0x01)
    {
        cout<<"Geant PID     : "<<setw(12)<<fGeantPID         <<", trk      : "<<setw(12)<<fGeantTrack            <<", parPID     : "<<setw(12)<< fGeantParentPID           <<", parTrk     : "<<setw(12)<<fGeantParentTrackNum       <<", grndParPID : "<<setw(12)<<fGeantGrandParentPID  <<", grndParTrk : "<<setw(12)<<fGeantGrandParentTrackNum <<endl;
        cout<<"Generator     : "<<setw(12)<<fGeantgeninfo     <<", 1        : "<<setw(12)<<fGeantgeninfo1         <<", 2          : "<<setw(12)<<fGeantgeninfo2             <<", weight     : "<<setw(12)<<fGeantgenweight<<endl;
        cout<<"Geant Mom     : "<<setw(12)<<getGeantTotalMom()<<", xmom     : "<<setw(12)<<fGeantxMom             <<", ymom       : "<<setw(12)<< fGeantyMom                <<", zmom       : "<<fGeantzMom<<endl;
        cout<<"Geant Vertex  : "<<setw(12)<<fGeantxVertex     <<", "<<setw(12)<< fGeantyVertex <<", "<< fGeantzVertex <<endl;
    }
}

void HVirtualCandSim::Streamer(TBuffer &R__b)
{
   // Stream an object of class HVirtualCandSim.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      R__b >> fGeantPID;
      R__b >> fGeantTrack;
      R__b >> fGeantCorrTrackIds;
      R__b >> fGeantxMom;
      R__b >> fGeantyMom;
      R__b >> fGeantzMom;
      R__b >> fGeantxVertex;
      R__b >> fGeantyVertex;
      R__b >> fGeantzVertex;
      R__b >> fGeantParentTrackNum;
      R__b >> fGeantParentPID;
      R__b >> fGeantGrandParentTrackNum;
      R__b >> fGeantGrandParentPID;
      R__b >> fGeantCreationMechanism;
      R__b >> fGeantMediumNumber;
      R__b >> fGeantgeninfo;
      R__b >> fGeantgeninfo1;
      R__b >> fGeantgeninfo2;
      R__b >> fGeantgenweight;
     R__b.CheckByteCount(R__s, R__c, HVirtualCandSim::IsA());
   } else {
      R__c = R__b.WriteVersion(HVirtualCandSim::IsA(), kTRUE);
      R__b << fGeantPID;
      R__b << fGeantTrack;
      R__b << fGeantCorrTrackIds;
      R__b << fGeantxMom;
      R__b << fGeantyMom;
      R__b << fGeantzMom;
      R__b << fGeantxVertex;
      R__b << fGeantyVertex;
      R__b << fGeantzVertex;
      R__b << fGeantParentTrackNum;
      R__b << fGeantParentPID;
      R__b << fGeantGrandParentTrackNum;
      R__b << fGeantGrandParentPID;
      R__b << fGeantCreationMechanism;
      R__b << fGeantMediumNumber;
      R__b << fGeantgeninfo;
      R__b << fGeantgeninfo1;
      R__b << fGeantgeninfo2;
      R__b << fGeantgenweight;
      R__b.SetByteCount(R__c, kTRUE);
   }
}
