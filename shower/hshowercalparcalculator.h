#ifndef HShowerCalParCalculator_H
#define HShowerCalParCalculator_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"

class HCategory;
class HIterator;

//------------------------------------------------------------------------------

#define HSCPC_DEF_METHOD      0
#define HSCPC_DEF_PARAM_1   0.0f
#define HSCPC_DEF_PARAM_2   0.0f

//------------------------------------------------------------------------------

class HShowerCalParCalculator : public HReconstructor
{
public:
    HShowerCalParCalculator(Int_t iMethod,
                            Float_t fParam1 = HSCPC_DEF_PARAM_1,
                            Float_t fParam2 = HSCPC_DEF_PARAM_2);

    HShowerCalParCalculator(const Text_t *name,const Text_t *title,
                            Int_t iMethod,
                            Float_t fParam1 = HSCPC_DEF_PARAM_1,
                            Float_t fParam2 = HSCPC_DEF_PARAM_2);

    HShowerCalParCalculator(const Text_t *name,const Text_t *title,
                            const Text_t *nameOfMethod,
                            Float_t fParam1 = HSCPC_DEF_PARAM_1,
                            Float_t fParam2 = HSCPC_DEF_PARAM_2);

   ~HShowerCalParCalculator();

    Bool_t      init(void);
    Bool_t      reinit(void);
    Bool_t      finalize(void);
    Int_t       execute(void);

    HParSet*    getCalPar()     { return m_pCalPar;     }
    HParSet*    getCalParHist() { return m_pCalParHist; }

    void        setCalPar(HParSet*);
    void        setCalParHist(HParSet*);

    void        initCalPar();

    // -------------------------------------------------------------------------

    void        setMethod(Int_t iMethod);
    void        setMethod(const Char_t* pMethod);

    Int_t       getMethod(void)         { return m_iMethod; }
    const Char_t* getMethodName(void);

    void        setParam1(Float_t fP)   { m_fParam1 = fP;    }
    Float_t     getParam1(void)         { return m_fParam1;  }

    void        setParam2(Float_t fP)   { m_fParam2 = fP;    }
    Float_t     getParam2(void)         { return m_fParam2;  }

    // -------------------------------------------------------------------------

private:
    void        clearAtBegin(Int_t iMethod, Float_t fParam1, Float_t fParam2);

    // -------------------------------------------------------------------------

private:
    Int_t       m_nExecutes;
    Int_t       m_nPeaksNumber;

    Int_t       m_iMethod;
    Float_t     m_fParam1;
    Float_t     m_fParam2;

    HLocation   m_zeroLoc;
    HLocation   m_loc;

    HCategory  *m_pRawCat; //!Pointer to the raw data category

    HParSet    *m_pCalPar; //!Pointer to the cal data category
    HParSet    *m_pCalParHist;

    HIterator  *fIter;

    ClassDef(HShowerCalParCalculator, 0) //ROOT extension
};

#endif
