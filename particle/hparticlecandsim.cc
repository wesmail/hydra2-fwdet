#include "hparticlecandsim.h"
#include "hades.h"
#include <iostream>
#include <iomanip>

using namespace std;

// ROOT's IO and RTTI stuff is added here
ClassImp(HParticleCandSim)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleCandSim
//
// Simulation object keeping HGeant information in addition to the normal
// HParticleCand. For documentation of enum constants see hparticledef.h
////////////////////////////////////////////////////////////////////////////////

Bool_t  HParticleCandSim::isGoodMDCWires(Int_t tr,Int_t minInner,Int_t minOuter,Bool_t checkall,Bool_t requireOuter)
{
    // checks if track tr is contributing to innner/outer segments (tr=-1 : any combination):
    // minInner     : (default 7) minimum required wires in inner seg from this track
    // minOuter     : (default 7) minimum required wires in outer seg from this track
    // checkall     : kFALSE (default) check only the most contributing track
    //                kTRUE            check both tracks stored
    // requireOuter : kTRUE  (default) check track in outer seg allways (tracks without outer seg will fail allways)
    //                kFALSE           check track in outer seg if outer segment exists (tracks without outer seg 
    //                                 will survive if track was found in inner seg)
    // to check inner seg only :  minOuter <=0
    // to check outer seg only :  minInner <=0
    Bool_t outerOnly = (minInner <=0)? kTRUE : kFALSE;
    Bool_t innerOnly = (minOuter <=0)? kTRUE : kFALSE;


    if(tr>0 || tr == Hades::getEmbeddingRealTrackId())
    {
	Bool_t good1=kFALSE;
	Bool_t good2=kFALSE;

	if(!outerOnly){       // inner or inner+outer
	    // inner seg
	    for(Int_t i=0;i<2;i++){
		if(!checkall && i >0) continue;
		if(fGeantTrackInnerMdc[i] == tr && fGeantTrackInnerMdcN[i]>=minInner) { good1=kTRUE;  break;}
	    }
	    if(!good1 || innerOnly || (!requireOuter && fOuterSegInd==-1)) return  good1;      // if inner failed we don't look to outer
            
	} else {  // only check outer seg
	    good1 = kTRUE;
	}

        // outer seg
	for(Int_t i=0;i<2;i++){
	    if(!checkall && i >0) continue;
	    if(fGeantTrackOuterMdc[i] == tr && fGeantTrackOuterMdcN[i]>=minOuter) { good2=kTRUE;  break;}
	}
	if( good1 && good2) return kTRUE;

    } else {  // check if any track combination fullfill the requirements
	Int_t tracks[2]={-1,-1};

	Bool_t good1=kFALSE;
	Bool_t good2=kFALSE;

	if(!outerOnly){     // inner or inner+outer
	    // inner seg
            Int_t ct=0;
	    for(Int_t i=0;i<2;i++){
		if(!checkall && i >0) continue;
		if(fGeantTrackInnerMdcN[i]>=minInner) { good1=kTRUE; tracks[i]=fGeantTrackInnerMdc[i]; ct++;}
	    }
	    if(!good1 || innerOnly || (!requireOuter && fOuterSegInd==-1) ) return good1;

	    for(Int_t j=0;j<ct;j++){
		for(Int_t i=0;i<2;i++){
		    if(!checkall && i >0) continue;
		    if(fGeantTrackOuterMdc[i] == tracks[j] && fGeantTrackOuterMdcN[i]>=minOuter) { good2=kTRUE;  break;}
		}
	    }
            return good2;

	} else {  // only check outer seg

	    for(Int_t i=0;i<2;i++){
		if(!checkall && i >0) continue;
		if(fGeantTrackOuterMdcN[i]>=minOuter) { good2=kTRUE;  break;}
	    }
            return good2;
	}
    }
    return kFALSE;
}

Bool_t   HParticleCandSim::isGoodMDCWeight(Int_t tr,Float_t minInner,Float_t minOuter,Bool_t checkall,Bool_t requireOuter)
{
    // checks if track tr is contributing to innner/outer segments (tr=-1 : any combination):
    // minInner     : (default 0.5) minimum required fraction of wires in inner seg from this track
    // minOuter     : (default 0.5) minimum required fraction wires in outer seg from this track
    // checkall     : kFALSE (default) check only the most contributing track
    //                kTRUE            check both tracks stored
    // requireOuter : kTRUE  (default) check track in outer seg allways (tracks without outer seg will fail allways)
    //                kFALSE           check track in outer seg if outer segment exists (tracks without outer seg 
    //                                 will survive if track was found in inner seg)
    // to check inner seg only :  minOuter <=0
    // to check outer seg only :  minInner <=0

    Bool_t outerOnly = (minInner <=0)? kTRUE : kFALSE;
    Bool_t innerOnly = (minOuter <=0)? kTRUE : kFALSE;


    if(tr>0 || tr == Hades::getEmbeddingRealTrackId())
    {
	Bool_t good1=kFALSE;
	Bool_t good2=kFALSE;

	if(!outerOnly){       // inner or inner+outer
	    // inner seg
	    for(Int_t i=0;i<2;i++){
		if(!checkall && i >0) continue;
		if(fGeantTrackInnerMdc[i] == tr && getGeantTrackInnerMdcWeight(i)>=minInner) { good1=kTRUE;  break;}
	    }
	    if(!good1 || innerOnly || (!requireOuter && fOuterSegInd==-1) ) return  good1;      // if inner failed we don't look to outer

	} else {  // only check outer seg
	    good1 = kTRUE;
	}

        // outer seg
	for(Int_t i=0;i<2;i++){
	    if(!checkall && i >0) continue;
	    if(fGeantTrackOuterMdc[i] == tr && getGeantTrackOuterMdcWeight(i)>=minOuter) { good2=kTRUE;  break;}
	}
	if( good1 && good2) return kTRUE;

    } else {  // check if any track combination fullfill the requirements
	Int_t tracks[2]={-1,-1};

	Bool_t good1=kFALSE;
	Bool_t good2=kFALSE;

	if(!outerOnly){     // inner or inner+outer
	    // inner seg
            Int_t ct=0;
	    for(Int_t i=0;i<2;i++){
		if(!checkall && i >0) continue;
		if(getGeantTrackInnerMdcWeight(i)>=minInner) { good1=kTRUE; tracks[i]=fGeantTrackInnerMdc[i]; ct++;}
	    }
	    if(!good1 || innerOnly || (!requireOuter && fOuterSegInd==-1) ) return good1;

	    for(Int_t j=0;j<ct;j++){
		for(Int_t i=0;i<2;i++){
		    if(!checkall && i >0) continue;
		    if(fGeantTrackOuterMdc[i] == tracks[j] && getGeantTrackOuterMdcWeight(i)>=minOuter) { good2=kTRUE;  break;}
		}
	    }
            return good2;

	} else {  // only check outer seg

	    for(Int_t i=0;i<2;i++){
		if(!checkall && i >0) continue;
		if(getGeantTrackOuterMdcWeight(i)>=minOuter) { good2=kTRUE;  break;}
	    }
            return good2;
	}
    }
    return kFALSE;
}

Bool_t   HParticleCandSim::isGoodMDC(Int_t tr,Float_t minInner,Float_t minOuter,Int_t minInnerW,Int_t minOuterW,Bool_t checkall,Bool_t requireOuter)
{
    // checks if track tr is contributing to innner/outer segments (tr=-1 : any combination):
    // minInner     : (default 0.5) minimum required fraction of wires in inner seg from this track
    // minOuter     : (default 0.5) minimum required fraction wires in outer seg from this track
    // minInnerW    : (default 5) minimum required wires in inner seg from this track
    // minOuterW    : (default 5) minimum required wires in outer seg from this track
    // checkall     : kFALSE (default) check only the most contributing track
    //                kTRUE            check both tracks stored
    // requireOuter : kTRUE  (default) check track in outer seg allways (tracks without outer seg will fail allways)
    //                kFALSE           check track in outer seg if outer segment exists (tracks without outer seg 
    //                                 will survive if track was found in inner seg)
    // to check inner seg only :  minOuter <=0  || minOuterW <=0
    // to check outer seg only :  minInner <=0  || minInnerW <=0


    Bool_t outerOnly = (minInner <=0)? kTRUE : kFALSE;
    Bool_t innerOnly = (minOuter <=0)? kTRUE : kFALSE;


    if(tr>0 || tr == Hades::getEmbeddingRealTrackId())
    {
	Bool_t good1=kFALSE;
	Bool_t good2=kFALSE;

	if(!outerOnly){       // inner or inner+outer
	    // inner seg
	    for(Int_t i=0;i<2;i++){
		if(!checkall && i >0) continue;
		if(fGeantTrackInnerMdc[i] == tr && fGeantTrackInnerMdcN[i]>=minInnerW && getGeantTrackInnerMdcWeight(i)>=minInner) { good1=kTRUE;  break;}
	    }
	    if(!good1 || innerOnly || (!requireOuter && fOuterSegInd==-1) ) return  good1;      // if inner failed we don't look to outer

	} else {  // only check outer seg
	    good1 = kTRUE;
	}

        // outer seg
	for(Int_t i=0;i<2;i++){
	    if(!checkall && i >0) continue;
	    if(fGeantTrackOuterMdc[i] == tr && fGeantTrackOuterMdcN[i]>=minOuterW && getGeantTrackOuterMdcWeight(i)>=minOuter) { good2=kTRUE;  break;}
	}
	if( good1 && good2) return kTRUE;

    } else {  // check if any track combination fullfill the requirements
	Int_t tracks[2]={-1,-1};

	Bool_t good1=kFALSE;
	Bool_t good2=kFALSE;

	if(!outerOnly){     // inner or inner+outer
	    // inner seg
            Int_t ct=0;
	    for(Int_t i=0;i<2;i++){
		if(!checkall && i >0) continue;
		if(fGeantTrackInnerMdcN[i]>=minInnerW && getGeantTrackInnerMdcWeight(i)>=minInner) { good1=kTRUE; tracks[i]=fGeantTrackInnerMdc[i]; ct++;}
	    }
	    if(!good1 || innerOnly || (!requireOuter && fOuterSegInd==-1) ) return good1;

	    for(Int_t j=0;j<ct;j++){
		for(Int_t i=0;i<2;i++){
		    if(!checkall && i >0) continue;
		    if(fGeantTrackOuterMdc[i] == tracks[j] && fGeantTrackOuterMdcN[i]>=minOuterW && getGeantTrackOuterMdcWeight(i)>=minOuter) { good2=kTRUE;  break;}
		}
	    }
            return good2;

	} else {  // only check outer seg

	    for(Int_t i=0;i<2;i++){
		if(!checkall && i >0) continue;
		if(fGeantTrackOuterMdcN[i]>=minOuterW && getGeantTrackOuterMdcWeight(i)>=minOuter) { good2=kTRUE;  break;}
	    }
            return good2;
	}
    }
    return kFALSE;

}
void HParticleCandSim::print(UInt_t selection)
{
    // print one candidate object and the flags which have been
    // already set to HParticleCand
    //
    // print option bits
    // bit   1 : print hit indices                         (selection == 1)
    //       2 : print hit indices                         (selection == 2)
    //       3 : print chi2s and matching vars             (selection == 4)
    //       4 : print selection flags                     (selection == 8)
    //       5 : print Geant infos                         (selection == 16)


    // default : print all


    if( (selection&0xF) > 0 ) HParticleCand::print(selection);
    else cout<<"    "<<" --------------------------------------------"<<endl;

    if( (selection>>4) & 0x01){

	cout<<"Geant PID     : "<<setw(12)<<fGeantPID         <<", trk      : "<<setw(12)<<fGeantTrack            <<", parPID     : "<<setw(12)<< fGeantParentPID           <<", parTrk     : "<<setw(12)<<fGeantParentTrackNum       <<", grndParPID : "<<setw(12)<<fGeantGrandParentPID  <<", grndParTrk : "<<setw(12)<<fGeantGrandParentTrackNum <<endl;
	cout<<"Generator     : "<<setw(12)<<fGeantgeninfo     <<", 1        : "<<setw(12)<<fGeantgeninfo1         <<", 2          : "<<setw(12)<<fGeantgeninfo2             <<", weight     : "<<setw(12)<<fGeantgenweight<<endl;
	cout<<"ghost track   : "<<setw(12)<<isGhostTrack()    <<", in RICH  : "<<setw(12)<<isInDetector(kIsInRICH)<<", in iMDC    : "<<setw(12)<<isInDetector(kIsInInnerMDC)<<", in oMDC    : "<<setw(12)<<isInDetector(kIsInOuterMDC)<<", in TOF     : "<<setw(12)<<isInDetector(kIsInTOF)<<", in RPC     : "<<setw(12)<<isInDetector(kIsInRPC)<<", in SHOWER: "<<setw(12)<<isInDetector(kIsInSHOWER)<<endl;
	cout<<"Geant Mom     : "<<setw(12)<<getGeantTotalMom()<<", xmom     : "<<setw(12)<<fGeantxMom             <<", ymom       : "<<setw(12)<< fGeantyMom                <<", zmom       : "<<fGeantzMom<<endl;
	cout<<"Geant Vertex  : "<<setw(12)<<fGeantxVertex     <<", "<<setw(12)<< fGeantyVertex <<", "<< fGeantzVertex <<endl;
        cout<<"Geant MDC i tr: "<<setw(6) <<fGeantTrackInnerMdc[0]   <<" ("<<setw(3)<<(Int_t)fGeantTrackInnerMdcN[0] <<"), "<<setw(6)<<fGeantTrackInnerMdc  [1] <<" ( "<<setw(3)<<(Int_t)fGeantTrackInnerMdcN  [1] <<")" <<endl;
        cout<<"Geant MDC o tr: "<<setw(6) <<fGeantTrackOuterMdc[0]   <<" ("<<setw(3)<<(Int_t)fGeantTrackOuterMdcN[0] <<"), "<<setw(6)<<fGeantTrackOuterMdc  [1] <<" ( "<<setw(3)<<(Int_t)fGeantTrackOuterMdcN  [1] <<")" <<endl;
        cout<<"Geant META  tr: "<<setw(12)<<fGeantTrackMeta  [0]     <<", "<<setw(12)<<fGeantTrackMeta  [1] <<", "<<setw(12)<<fGeantTrackMeta  [2] <<", "<<setw(12)<<fGeantTrackMeta  [3]  <<endl;
        cout<<"Geant SHR   tr: "<<setw(12)<<fGeantTrackShower[0]     <<", "<<setw(12)<<fGeantTrackShower[1] <<", "<<setw(12)<<fGeantTrackShower[2] <<", "<<setw(12)<<fGeantTrackShower[3]  <<endl;
        cout<<"Geant RICH  tr: "<<setw(12)<<fGeantTrackRich  [0]     <<", "<<setw(12)<<fGeantTrackRich  [1] <<", "<<setw(12)<<fGeantTrackRich  [2] <<endl;

    }

}

void HParticleCandSim::Streamer(TBuffer &R__b)
{
   // Stream an object of class HParticleCandSim.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HParticleCand::Streamer(R__b);
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
      if(R__v > 1){
	  R__b.ReadStaticArray((int*)fGeantTrackRich);
	  R__b.ReadStaticArray((int*)fGeantTrackMeta);
          R__b.ReadStaticArray((int*)fGeantTrackShower);
      } else {
	  for (Int_t i = 0; i < 3; ++i) fGeantTrackRich[i] = -1;
	  for (Int_t i = 0; i < 4; ++i) {
	      fGeantTrackMeta  [i] = -1;
              fGeantTrackShower[i] = -1;
          }
      }

      if(R__v > 2){
	  R__b.ReadStaticArray((int*)fGeantTrackInnerMdc);
	  R__b.ReadStaticArray((int*)fGeantTrackOuterMdc);
	  R__b.ReadStaticArray((char*)fGeantTrackInnerMdcN);
	  R__b.ReadStaticArray((char*)fGeantTrackOuterMdcN);
      } else {
	  for (Int_t i = 0; i < 2; ++i) {
	      fGeantTrackInnerMdc[i] = -1;
	      fGeantTrackOuterMdc[i] = -1;
	      fGeantTrackInnerMdcN[i] = 0;
	      fGeantTrackOuterMdcN[i] = 0;
	  }
      }
     R__b.CheckByteCount(R__s, R__c, HParticleCandSim::IsA());
   } else {
      R__c = R__b.WriteVersion(HParticleCandSim::IsA(), kTRUE);
      HParticleCand::Streamer(R__b);
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
      R__b.WriteArray(fGeantTrackRich, 3);
      R__b.WriteArray(fGeantTrackMeta, 4);
      R__b.WriteArray(fGeantTrackShower, 4);
      R__b.WriteArray(fGeantTrackInnerMdc,2);
      R__b.WriteArray(fGeantTrackOuterMdc,2);
      R__b.WriteArray(fGeantTrackInnerMdcN,2);
      R__b.WriteArray(fGeantTrackOuterMdcN,2);
      R__b.SetByteCount(R__c, kTRUE);
   }
}


