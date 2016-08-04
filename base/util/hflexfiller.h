//*-- Author : Jochen Markert 18.07.2007

#ifndef  __HFLEXFILLER_H__
#define  __HFLEXFILLER_H__

#include "hreconstructor.h"
#include "hcategory.h"
#include "hhistmap.h"

#include "TObjArray.h"


class HFlexFiller : public HReconstructor {

protected:
    Bool_t createCat;
    HHistMap*  hM;
    TObjArray* parameters;
    Int_t (*pUserFill)(HHistMap* hmap,TObjArray* pars); //! user provided function pointer to fill the HFlex Category

    void   clear(void);
public:
    HFlexFiller(void);
    HFlexFiller(const Text_t *name,const Text_t *title);
    ~HFlexFiller(void);
    Bool_t init      (void);
    Int_t  execute   (void);
    Bool_t finalize  (void);

    void   setUserFill(Int_t (*function)(HHistMap* ,TObjArray* ),HHistMap* histmap=0,TObjArray* pars=0,Bool_t makeCat=kTRUE){
        createCat  = makeCat;
	hM         = histmap;
        parameters = pars;
	pUserFill  = function;
    }
    ClassDef(HFlexFiller,0);
};
#endif /* !__HFLEXFILLER_H__ */







