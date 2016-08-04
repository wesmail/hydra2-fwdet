#ifndef HShowerCriterium_H
#define HShowerCriterium_H
#pragma interface

#include "TObject.h"

class HShowerHit;
class HShowerHitFPar;

class HShowerCriterium : public TObject {
public:
        HShowerCriterium(){m_fHitFPar = NULL;}
        HShowerCriterium(HShowerHitFPar* par){setParams(par);}

        Float_t showerCriterium(HShowerHit* pHit, Int_t& ret)
                              {return showerCriterium(pHit, ret, m_fHitFPar);}
        Float_t showerCriterium(HShowerHit* pHit, Int_t& ret,
                               HShowerHitFPar* pParams);
        void setParams(HShowerHitFPar* par){m_fHitFPar = par;}
        HShowerHitFPar* getParams(){return m_fHitFPar;}

        ClassDef(HShowerCriterium,1) //ROOT extension
private:

        HShowerHitFPar* m_fHitFPar;
};

#endif
