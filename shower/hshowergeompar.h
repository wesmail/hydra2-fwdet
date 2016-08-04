#ifndef HSHOWERGEOMPAR_H
#define HSHOWERGEOMPAR_H

#include "hdetgeompar.h"

class HShowerGeomPar: public HDetGeomPar{
public:
  HShowerGeomPar(const Char_t* name="ShowerGeomPar",
		 const Char_t* title="Shower (GEANT) geometry parameters and alignment",
		 const Char_t* context="GeomProduction");
  ~HShowerGeomPar() {}					
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo* output);
  inline Int_t getSecNum(const TString&);
  inline Int_t getModNumInMod(const TString&);
  inline Int_t getModNumInComp(const TString&);
  Int_t getCompNum(const TString&) {return 0;}	
  ClassDef(HShowerGeomPar,2)//Class for PreShower geometry container	
};

inline Int_t HShowerGeomPar::getSecNum(const TString& name){
		// Returns the sector index retrieved from the module name SHxMx
		return (Int_t)(name[4]-'0')-1;} 
inline Int_t HShowerGeomPar::getModNumInMod(const TString& name){
		// Returns the module index retrieved from the module name SHxMx
		return (Int_t)(name[2]-'0')-1;} 
inline Int_t HShowerGeomPar::getModNumInComp(const TString& name){
		// Returns the module index retrieved from the  layer of wires name SxSW 
		return (Int_t)name[1]-'0' -1;} 



#endif /* !HSHOWERGEOMPAR_H */
