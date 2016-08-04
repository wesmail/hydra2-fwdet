#ifndef _HPIONTRACKERBEAMPAR_H_
#define _HPIONTRACKERBEAMPAR_H_

#include "hparcond.h"
#include "HBeam.h"



class HPionTrackerBeamPar : public  HParCond {

    HBeam beam;                //
    Int_t ftargetElementNum;   // element ID of target
public:
    HPionTrackerBeamPar(const Char_t * name    = "PionTrackerBeamPar",
			const Char_t * title   = "parameters of the PionBeam",
			const Char_t * context = "PionTrackerBeamParProduction");
    virtual ~HPionTrackerBeamPar() {}

    void           clear();
    void           putParams (HParamList *);
    Bool_t         getParams (HParamList *);

    void           printParam();
    Bool_t         initBeamLine   (TString filename,Int_t targetElementNum=32,Bool_t debug=kFALSE);
    void           createAsciiFile(TString filename);

    HBeam&         getBeam()                 { return beam; }
    Int_t          getTargetElementNumber()  { return ftargetElementNum; }
    Int_t          getNBeamElements()        { return beam.getElements().size();}
    HBeamElement*  getBeamElement(UInt_t id) { if(id<beam.getElements().size()) return &(beam.getElements()[id]); else return NULL;}
    ClassDef (HPionTrackerBeamPar, 1) // Container for the beam parameters of the PionTracker

};


#endif
