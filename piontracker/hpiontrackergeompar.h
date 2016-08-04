#ifndef HPIONTRACKERGEOMPAR_H
#define HPIONTRACKERGEOMPAR_H

#include "hparcond.h"
#include "hgeomtransform.h"
#include "TObjArray.h"

class HPionTrackerDetGeomPar :  public TObject {
private:
  Int_t          numStrips;     // number of strips in a detector plane           
  Float_t        stripDistance; // distance between strip centers
  HGeomTransform labTransform;  // lab transformation of the detector front plane
public:
 HPionTrackerDetGeomPar(void) : numStrips(0), stripDistance(0.F) {}
  ~HPionTrackerDetGeomPar(void) {}
  HGeomTransform& getLabTransform(void) {return labTransform;}
  void    print(void);
  Float_t getNumStrips(void)     const {return numStrips;}
  Float_t getStripDistance(void) const {return stripDistance;}
  void    setNumStrips(const Int_t n)       {numStrips = n;}
  void    setStripDistance(const Float_t d) {stripDistance = d;}
  Float_t getStripPos(const Float_t p) {
    // returns the strip position in local coordinate system (0.0 between strip 63 and 64)
    if (p>=0 && p<numStrips) return (p-63.5)*stripDistance;
    else return -1000.F;
  }
  ClassDef(HPionTrackerDetGeomPar,1) // Geometry of a Pion Tracker
}; 

class HPionTrackerGeomPar : public HParCond {
private:
  TObjArray* pDetectors;    // linear array of all detectors (type HPionTrackerDetGeomPar)
  Int_t      numDetectors;  // number of detectors
public:
  HPionTrackerGeomPar(const Char_t* name = "PionTrackerGeomPar",
                      const Char_t* title = "Geometry parameters of the PionTracker",
                      const Char_t* context = "GeomProduction");
  ~HPionTrackerGeomPar(void);

  void   clear(void);
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);
  void   printParams(void);

  Float_t getNumDetectors(void) const {return numDetectors;}
  Bool_t  setNumDetectors(const Int_t);
  HPionTrackerDetGeomPar* getDetector(const Int_t);

  ClassDef(HPionTrackerGeomPar, 1) // Container for the geometry parameters of the PionTracker
};

#endif  /*!HPIONTRACKERGEOMPAR_H*/
