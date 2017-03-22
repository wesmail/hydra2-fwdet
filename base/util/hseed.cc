#include "hseed.h"
#include "TSystem.h"
#include "TMath.h"
#include "TInetAddress.h"
#include "TDatime.h"

#include <iostream>
#include <iomanip>
#include <unistd.h>


using namespace std;

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////////
//
// HSeed
//
// Helper class for random sedd generation on batch farm
// for the available options see the constructor of the class
//
////////////////////////////////////////////////////////////////////////////

ClassImp(HSeed)


HSeed::HSeed(Int_t method,Int_t fallback,Int_t fixedSeed, Bool_t noBlock)
{
    //  method;           0 (default) : /dev/random,
    //                    1 : TRandom3 (local) with systime ,
    //                    2 : TRandom3 (local) with systime + processid + iplast,
    //                    3 : like 1 but gRandom,
    //                    4 : like 2 but gRandom,
    //                    5 : fixed (needs seed to be set)
    //  fallback          case /dev/random is not available : default 2
    //  fixedSeed  ;   //   default -1  used when method 5 is active
    //  noBlock           :  kTRUE switch /dev/radom to /dev/urandom  to avoid blocking (less randomness!)
    //
    //
    //  Int_t     getFixedSeed();     //   default -1
    //  Int_t     getInitialSeed();   //   default -1, stores the seed input gRandom
    //  Int_t     getFirstSeed();     //   default -1, stores the first seed generated
    //  TString   getHostname();      //   hostname of execution
    //  UInt_t    getAddress();       //   4 * 8bit address fields
    //  UInt_t    getPid();           //   process id



    fNoBlock = noBlock;

    fHostname = gSystem->HostName();
    fAddress  = getIP();
    fPid      = getPid();

    frandom = 0;

    if(method < 0 || method > 5)
    {
	Error("HSeed()","method %i :  method has to be 1 ... 5. Use 0 instead.",method);
	fMethod = 0;
    } else {
	fMethod = method;
    }

    if(fMethod != 0 ) fallback = fMethod;

    if(fallback < 1 || fallback > 4)
    {
	Error("HSeed()","fallback %i :  method has to be 1 ... 4. Use 2 instead.",fallback);
	fFallBack = 2;
    } else {
	fFallBack = fallback;
    }

    fFirstSeed   =-1;
    fInitialSeed =-1;
    fFixedSeed   =-1;
    fFileHandle  =-1;

    if(fNoBlock) fFileHandle = open("/dev/urandom", O_RDONLY);
    else         fFileHandle = open("/dev/random" , O_RDONLY);
    if(fFileHandle < 0){
	if(fNoBlock) Error("HSeed()","Could not open /dev/urandom for reading : will take numbers from TRandom3 ( method 2)");
	else         Error("HSeed()","Could not open /dev/random for reading : will take numbers from TRandom3 ( method 2)");
	fMethod   = fallback;
	fFallBack = fallback; // not used
	fFileHandle =-1;
    }

    TDatime datime;
    UInt_t t      = datime.Get();
    Int_t  iplast = getIPPart(0);


    if        (fMethod == 0) {
	if     (fFallBack == 1) {fGenerator.SetSeed(t);                       frandom = &fGenerator; fInitialSeed = t;}
	else if(fFallBack == 2) {fGenerator.SetSeed(t+fPid*1000+iplast*1000); frandom = &fGenerator; fInitialSeed = t+fPid*1000+iplast*1000;}
	else if(fFallBack == 3) {gRandom  ->SetSeed(t);                       frandom = gRandom;     fInitialSeed = t;}
	else if(fFallBack == 4) {gRandom  ->SetSeed(t+fPid*1000+iplast*1000); frandom = gRandom;     fInitialSeed = t+fPid*1000+iplast*1000;}

    } else {

	if     (fMethod == 1) {fGenerator.SetSeed(t);                       frandom = &fGenerator;fInitialSeed = t;}
	else if(fMethod == 2) {fGenerator.SetSeed(t+fPid*1000+iplast*1000); frandom = &fGenerator;fInitialSeed = t+fPid*1000+iplast*1000;}
	else if(fMethod == 3) {gRandom  ->SetSeed(t);                       frandom = gRandom;    fInitialSeed = t;}
	else if(fMethod == 4) {gRandom  ->SetSeed(t+fPid*1000+iplast*1000); frandom = gRandom;    fInitialSeed = t+fPid*1000+iplast*1000;}

	if(fMethod == 5){
	    if(fixedSeed < 0 ) { Error("HSeed()","Option fixedSeed used, but seed is negative! will use Abs()"); }
	    fFixedSeed   = TMath::Abs(fixedSeed);
	    fInitialSeed = fFixedSeed;
	    fFirstSeed   = fFixedSeed;
	}

    }

}
HSeed::~HSeed()
{
    if(fFileHandle!=-1){
	close(fFileHandle);
    }
    fFileHandle=-1;

}

UInt_t HSeed::getPid()
{
    // returns the current process id
    return gSystem->GetPid();
}

UInt_t HSeed::getIP()
{
    // gets the ip address of the host from the system
    TString hostname     = gSystem->HostName();
    TInetAddress address = gSystem->GetHostByName(hostname.Data());
    return address.GetAddress();
}

UInt_t HSeed::getIPPart(UInt_t field)
{
    // return the 4 segmentent ip address (field ==0 the lowest bytes)
    if(field > 3) {
	Error("getIPPart()","address field has to be 0,1,2 or 3 . Take lowest bytes instead.");
	field=0;
    }
    return (0xFF)&(fAddress>>field*8);
}

Int_t HSeed::getSeed()
{
    // generate a new seed from the selected source
    Int_t rndNum =-1;
    if        (fMethod == 0)
    {
	Int_t ret = read(fFileHandle, &rndNum, sizeof(rndNum));
	if(ret < 0){
	    if(fNoBlock) Error("getSeed()"," Could not read from /dev/urandom : will take numbers from fallback");
	    else         Error("getSeed()"," Could not read from /dev/random : will take numbers from fallback");
	    close(fFileHandle);
	    fFileHandle = -1;
	    fMethod     = fFallBack;
	} else {
	    while(rndNum<0){   // make sure they are in rang of int
		ret = read(fFileHandle, &rndNum, sizeof(rndNum));
	    }
	    if(fFirstSeed == -1) fFirstSeed = rndNum;
	    return rndNum;
	}
    } else if (fMethod > 0 && fMethod < 5) {
	while(rndNum<0) {
	    rndNum = frandom->Rndm()*(kMaxInt-1);
	}
	if(fFirstSeed == -1) fFirstSeed = rndNum;
	return rndNum;
    }
    else if(fMethod == 5 ){
	return fFixedSeed;
    }
  return -1;
}

void  HSeed::print()
{
    // print all params in 1 line
    cout<<"HSeed : "<<fHostname<<", ip : "<<Form("%i.%i.%i.%i",getIPPart(3),getIPPart(2),getIPPart(1),getIPPart(0))<<", pid :" <<fPid
	<<", method : "  <<setw(4) <<fMethod
	<<", fallback : "<<setw(4) <<fFallBack
	<<", first : "   <<setw(10)<<fFirstSeed
	<<", init : "    <<setw(10)<<fInitialSeed
	<<", fixed : "   <<setw(10)<<fFixedSeed
	<<endl;

}

