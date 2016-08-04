#ifndef HMDCTRACKGFIELDPAR_H
#define HMDCTRACKGFIELDPAR_H
#include "TString.h"
#include "TObject.h"
#include "TNamed.h"
#include "hparcond.h"


//CORRECTION PARAMETERS FOR MOMENTUM RECONSTRUCTION
class HMdcTrackGField;

class HMdcTrackGFieldPar:public HParCond {
protected:
    HMdcTrackGField* field;
public:
    HMdcTrackGFieldPar(const Char_t* name="MdcTrackGFieldPar",
		     const Char_t* title="Fieldmap",
		     const Char_t* context="MdcTrackGFieldParProduction");
    ~HMdcTrackGFieldPar();
    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);//{return kTRUE;}
    void    clear();
    void initFieldMap(TString );
    void calcField(Double_t *,Double_t *, Double_t );
    HMdcTrackGField* getPointer(){return field;}
    ClassDef(HMdcTrackGFieldPar,1)
};
#endif
