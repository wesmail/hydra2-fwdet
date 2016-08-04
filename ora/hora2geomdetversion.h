#ifndef HORA2GEOMDETVERSION_H
#define HORA2GEOMDETVERSION_H

#include "TNamed.h"

class HOra2GeomDetVersion : public TNamed {
  Int_t detectorId;       // Id of detector
  Int_t geomVersion;      // geometry version
  Double_t versDate[2];   // Actual date range of version
public:
  HOra2GeomDetVersion(const Char_t* name=0,Int_t id=-1);
  ~HOra2GeomDetVersion() {}
  Int_t getDetectorId() {return detectorId;}
  Int_t getGeomVersion() {return geomVersion;}
  Double_t getSince() {return versDate[0];}
  Double_t getUntil() {return versDate[1];}
  void setDetectorId(Int_t v) {detectorId=v;}
  void setGeomVersion(Int_t v) {geomVersion=v;}
  void setSince(Double_t v) {versDate[0]=v;}  
  void setUntil(Double_t v) {versDate[1]=v;}
  void fill(Int_t,Int_t,Double_t,Double_t);  
  void fill(Int_t,Double_t,Double_t);  
  void clearVersDate();
  ClassDef(HOra2GeomDetVersion,0) // Class for geometry version management in Oracle Hydra2 interface
};

#endif  /* !HORA2GEOMDETVERSION_H */
