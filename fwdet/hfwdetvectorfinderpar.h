#ifndef HFWDETVECTORFINDERPAR_H
#define HFWDETVECTORFINDERPAR_H

#include "hparcond.h"

class HFwDetVectorFinderPar : public HParCond
{
protected:
    Float_t fCutX;      // cut parameter for X-cord
    Float_t fCutY;      // cut parameter for Y-cord
    Float_t fLRCutChi2; // cut parameter for low-res Chi2
    Float_t fHRCutChi2; // cut parameter for high-res Chi2
    Float_t fLRErrU;    // low-res error of U-coordinate
    Float_t fHRErrU;    // high-res error of U-coordinate
    Float_t fTubesD;    // distance betwen tubes from siliar views
    Int_t   nPass;      // number of reco passess

public:
    HFwDetVectorFinderPar(const Char_t * name = "VectorFinderPar",
                               const Char_t * title = "VectorFinder parameters",
                               const Char_t * context = "VectorFinderParProduction");
    virtual ~HFwDetVectorFinderPar() {}

    void    clear();
    void    putParams (HParamList *);
    Bool_t  getParams (HParamList *);

    inline Float_t    getCutX() const { return fCutX; }
    inline Float_t    getCutY() const { return fCutY; }
    inline Float_t    getLRCutChi2() const { return fLRCutChi2; }
    inline Float_t    getHRCutChi2() const { return fHRCutChi2; }
    inline Float_t    getLRErrU() const { return fLRErrU; }
    inline Float_t    getHRErrU() const { return fHRErrU; }
    inline Float_t    getTubesD() const { return fTubesD; }
    inline Float_t    getNpass() const { return nPass; }

    inline void setCutX(const Float_t cutx) { fCutX = cutx; }
    inline void setCutY(const Float_t cuty) { fCutY = cuty; }
    inline void setLRCutChi2(const Float_t chi2) { fLRCutChi2 = chi2; }
    inline void setHRCutChi2(const Float_t chi2) { fHRCutChi2 = chi2; }
    inline void setLRErrU(const Float_t err) { fLRErrU = err; }
    inline void setHRErrU(const Float_t err) { fHRErrU = err; }
    inline void setTubesD(const Float_t d) { fTubesD = d; }
    inline void setNpass(const Int_t npass) { nPass = npass; }

    ClassDef (HFwDetVectorFinderPar, 1);
};

#endif  /*!HFWDETVECTORFINDERPAR_H*/
