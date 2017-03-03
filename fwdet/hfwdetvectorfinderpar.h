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
    Int_t   nMaxBest;   // maximum best combinations for a single track
    Int_t   nPass;      // number of reco passess

public:
    HFwDetVectorFinderPar(const Char_t * name = "VectorFinderPar",
                               const Char_t * title = "VectorFinder parameters",
                               const Char_t * context = "VectorFinderParProduction");
    virtual ~HFwDetVectorFinderPar() {}

    void    clear();
    void    putParams (HParamList *);
    Bool_t  getParams (HParamList *);

    Float_t  getCutX() const { return fCutX; }
    Float_t  getCutY() const { return fCutY; }
    Float_t  getLRCutChi2() const { return fLRCutChi2; }
    Float_t  getHRCutChi2() const { return fHRCutChi2; }
    Float_t  getLRErrU() const { return fLRErrU; }
    Float_t  getHRErrU() const { return fHRErrU; }
    Float_t  getTubesD() const { return fTubesD; }
    Int_t    getMaxBest() const { return nMaxBest; }
    Int_t    getNpass() const { return nPass; }

    void setCutX(const Float_t cutx) { fCutX = cutx; }
    void setCutY(const Float_t cuty) { fCutY = cuty; }
    void setLRCutChi2(const Float_t chi2) { fLRCutChi2 = chi2; }
    void setHRCutChi2(const Float_t chi2) { fHRCutChi2 = chi2; }
    void setLRErrU(const Float_t err) { fLRErrU = err; }
    void setHRErrU(const Float_t err) { fHRErrU = err; }
    void setTubesD(const Float_t d) { fTubesD = d; }
    void setMaxBest(const Int_t nbest) { nMaxBest = nbest; }
    void setNpass(const Int_t npass) { nPass = npass; }

    ClassDef (HFwDetVectorFinderPar, 1);
};

#endif  /*!HFWDETVECTORFINDERPAR_H*/
