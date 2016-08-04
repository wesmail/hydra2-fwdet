//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
// HPionTrackerGeomPar
//
// Container class for PionTracker geometry parameters
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hpiontrackergeompar.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hpiontrackerdetector.h"
#include "hparamlist.h"
#include <iostream>

ClassImp(HPionTrackerDetGeomPar)
ClassImp(HPionTrackerGeomPar)

void HPionTrackerDetGeomPar::print(void)
{
	// prints the geometry parameters of a single detector
	cout<<"Number of strips:        "<<numStrips<<'\n';
	cout<<"Distance between strips: "<<stripDistance<<'\n';
	cout<<"Lab transformation:\n"<<"  ";
	labTransform.print();
}

HPionTrackerGeomPar::HPionTrackerGeomPar(const Char_t* name, const Char_t* title,
                                         const Char_t* context)
                   : HParCond(name, title, context) {
  // constructor
  numDetectors = 0;
  pDetectors = NULL; 
}


HPionTrackerGeomPar::~HPionTrackerGeomPar(void) {
  // destructor deletes the array of detectors
  clear();
}

void HPionTrackerGeomPar::clear(void) {
  // deletes the array of detectors
  if (pDetectors) pDetectors->Delete();
  delete pDetectors;
  numDetectors = 0;
}

void HPionTrackerGeomPar::putParams(HParamList* l) {
  // add the parameters to the list for writing
  if (!l) return;
  TArrayI nStrips(numDetectors);
  TArrayF dist(numDetectors);
  TArrayD transform(numDetectors*12);
  for(Int_t i=0;i<numDetectors;i++) {
    HPionTrackerDetGeomPar* p=(HPionTrackerDetGeomPar*)pDetectors->At(i);
    nStrips.SetAt(p->getNumStrips(),i);
    dist.SetAt(p->getStripDistance(),i);
    HGeomTransform& t = p->getLabTransform();
    const HGeomRotation& r = t.getRotMatrix();
    const HGeomVector& v = t.getTransVector();
    for(Int_t k=0;k<9;k++) transform.SetAt(r(k), i*12 + k);
    transform.SetAt(v.getX(), i*12 + 9);
    transform.SetAt(v.getY(), i*12 + 10);
    transform.SetAt(v.getZ(), i*12 + 11);
  }
  l->add("numDetectors",  numDetectors);
  l->add("numStrips",     nStrips);
  l->add("stripDistance", dist);
  l->add("labTransform",  transform);
}
 
Bool_t HPionTrackerGeomPar::getParams(HParamList* l) {
  // gets the parameters from the list (read from input)
  if (!l) return kFALSE;
  Int_t nDet;
  if (!l->fill("numDetectors",  &nDet))      return kFALSE;
  TArrayI nStrips(nDet);
  if (!l->fill("numStrips",     &nStrips))   return kFALSE;
  if (nStrips.GetSize()!=nDet) {
    Error("HPionTrackerGeomPar::getParams(HParamList* l)",
          "Array size of numStrips does not fit to number of detectors");
    return kFALSE;
  }
  TArrayF dist(nDet);
  if (!l->fill("stripDistance", &dist))      return kFALSE;
  if (dist.GetSize()!=nDet) {
    Error("HPionTrackerGeomPar::getParams(HParamList* l)",
          "Array size of stripDistance does not fit to number of detectors");
    return kFALSE;
  }
  Int_t nData = nDet*12;
  Double_t* tValues = new Double_t[nData];
  Bool_t rc = l->fill("labTransform",tValues,nData);
  if (!rc) {
    Error("HPionTrackerGeomPar::getParams(HParamList* l)",
          "Array size of labTransform does not fit to number of detectors");
    delete [] tValues;
    return kFALSE;
  }
  rc = setNumDetectors(nDet);
  if (rc) {
    for(Int_t i=0;i<numDetectors;i++) {
      HPionTrackerDetGeomPar* p=(HPionTrackerDetGeomPar*)pDetectors->At(i);  
      p->setNumStrips(nStrips.At(i));
      p->setStripDistance(dist.At(i));
      HGeomTransform& tLab = p->getLabTransform();
      Double_t r[9], v[3];
      for(Int_t k=0;k<9;k++) r[k] = tValues[i*12+k];
      for(Int_t k=0;k<3;k++) v[k] = tValues[i*12+9+k];
      tLab.setRotMatrix(r);
      tLab.setTransVector(v);
    }
  }
  delete [] tValues;
  return rc; 
}

void HPionTrackerGeomPar::printParams(void) {
  // prints the parameters (overloads HParCond::printParams(void))
  cout<<"----- Geometry of Pion Tracker detectors ------------------"<<endl;
  for(Int_t i=0;i<numDetectors;i++) {
    HPionTrackerDetGeomPar* p=(HPionTrackerDetGeomPar*)pDetectors->At(i);  
    cout<<"Detector number:         "<<i<<'\n';
    p->print();
    cout<<"-------------------------"<<endl;
  }
}

Bool_t HPionTrackerGeomPar::setNumDetectors(const Int_t nDet) {
  if (nDet > 0) {
    Int_t maxDetectors = 0;
    HPionTrackerDetector* pDet= (HPionTrackerDetector*)gHades->getSetup()->getDetector("PionTracker");
    if (pDet) maxDetectors = (Int_t)((pDet->getMaxModInSetup()-1) / 2) + 1;
    if (nDet >= maxDetectors) {
      numDetectors = nDet;
      pDetectors = new TObjArray(nDet);
      for (Int_t i=0;i<nDet;++i) {
        pDetectors->AddAt(new HPionTrackerDetGeomPar(),i);
      }
      return kTRUE;
    } else {
      Error("HPionTrackerGeomPar::setNumDetectors(const Int_t nDet)",
            "Number of detectors too small for number of active modules in the setup");
      return kFALSE;
    }
  }
  Error("HPionTrackerGeomPar::setNumDetectors(const Int_t nDet","nDet must be larger than 0");
  return kFALSE;
}

HPionTrackerDetGeomPar* HPionTrackerGeomPar::getDetector(const Int_t n) {
  if (pDetectors && n<numDetectors) return (HPionTrackerDetGeomPar*)pDetectors->At(n);
  return NULL;
}
