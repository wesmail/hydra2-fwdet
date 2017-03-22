#ifndef HSEED_H
#define HSEED_H

#include "TObject.h"
#include "TString.h"
#include "TRandom3.h"


class HSeed : public TObject {

private:
    Int_t     fMethod;        //   0 (default) : /dev/random,
                              //   1 : TRandom3 (local) with systime ,
                              //   2 : TRandom3 (local) with systime + processid + iplast,
                              //   3 : like 1 but gRandom,
                              //   4 : like 2 but gRandom,
                              //   5 : fixed (needs seed to be set)
    Int_t     fFallBack;      //  case /dev/random is not available : default 2
    Bool_t    fNoBlock;       //   default kTRUE switch /dev/radom to /dev/urandom  to avoid blocking (less randomness!)

    Int_t     fFixedSeed  ;   //   default -1  setFixedSeed()
    Int_t     fInitialSeed;   //   default -1, stores the seed input gRandom
    Int_t     fFirstSeed;     //   default -1, stores the first seed generated
    TString   fHostname;      //   hostname of execution
    UInt_t    fAddress;       //   4 * 8bit address fields
    UInt_t    fPid;           //   process id
    TRandom3  fGenerator;     //!
    Int_t     fFileHandle;    //!  filehandle for /dev/random
    TRandom*  frandom;        //!  pointer to use generator
public:
    HSeed(Int_t method=0,Int_t fallback=2,Int_t fixedseed=-1, Bool_t noBlock = kTRUE) ;
    ~HSeed();
    UInt_t  getPid();
    UInt_t  getIP();
    UInt_t  getIPPart(UInt_t field=0);
    Int_t   getSeed();

    Int_t   getMethod()      { return fMethod;     }
    Int_t   getFallBack()    { return fFallBack;   }

    Int_t   getFixedSeed()   { return fInitialSeed;}
    Int_t   getInitialSeed() { return fInitialSeed;}
    Int_t   getFirstSeed()   { return fFirstSeed;  }

    TString getHostName()    { return fHostname;   }
    UInt_t  getAddress()     { return fAddress;    }

    void    print();

    ClassDef(HSeed,1)
};


#endif   /*! HSEED_H */
