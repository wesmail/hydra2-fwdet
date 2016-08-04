#ifndef HAFTfunctions_H
#define HAFTfunctions_H

#if !defined (__CINT__) || defined (__MAKECINT__)
#include "Rtypes.h"
#endif

extern "C" void setfilename_(Text_t*, Int_t);
extern "C" void setpairfilename_(Text_t*, Int_t);
extern "C" Int_t readhaftmatrix_(void);
extern "C" Int_t readhaftpairmatrix_(void);
extern "C" Float_t gethadesacceptance_(Int_t*, Float_t*, Float_t*, Float_t*, Int_t*);
extern "C" Float_t gethadespairacceptance_(Float_t*, Float_t*, Float_t*, Int_t*);
extern "C" Float_t getmatrixval_(Int_t*, Int_t*, Int_t*, Int_t*);
extern "C" void getlimits_(Int_t*, Float_t*, Float_t*, Float_t*,
                           Float_t*, Float_t*, Float_t*,
                           Float_t*, Float_t*, Float_t*);
extern "C" void getdimensions_(Int_t*, Int_t*, Int_t*, Int_t*);
extern "C" Float_t ran_(Int_t*);
extern "C" Float_t samplegauss_(Float_t*, Float_t*);
extern "C" void smearhades3momentum_(Float_t*, Int_t*, Int_t*);
extern "C" void smearhades4momentum_(Float_t*, Int_t*, Int_t*);
extern "C" void smearhadesmomentum_(Float_t*, Float_t*, Float_t*, Float_t*, Int_t*, Int_t*);
extern "C" void smearhadespair_(Float_t*,Int_t*);
extern "C" void setresolutionparameters_(Int_t*,Float_t*,Float_t*);
#endif
