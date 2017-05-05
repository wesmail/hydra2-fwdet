#ifndef HFWDETVECTORFINDERPAR_H
#define HFWDETVECTORFINDERPAR_H

#include "hparcond.h"

class HFwDetVectorFinderPar : public HParCond
{
protected:
    Float_t fMatchR;        // cut parameter for vectors distance match
    Float_t fMatchCross;    // cut parameter for vectors cross match
    Float_t fCutX;          // cut parameter for X-cord
    Float_t fCutY;          // cut parameter for Y-cord
    Float_t fLRCutChi2;     // cut parameter for low-res Chi2
    Float_t fHRCutChi2;     // cut parameter for high-res Chi2
    Float_t fTanCut;        // cut parameter for Tangens cut
    Float_t fTxyCut;        // cut parameter for Tx,y vector matching
    Float_t fLRErrU;        // low-res error of U-coordinate
    Float_t fHRErrU;        // high-res error of U-coordinate
    Int_t   nMaxBest;       // maximum best combinations for a single track
    Int_t   nPass;          // number of reco passess

public:
    HFwDetVectorFinderPar(const Char_t * name = "VectorFinderPar",
                               const Char_t * title = "VectorFinder parameters",
                               const Char_t * context = "VectorFinderParProduction");
    virtual ~HFwDetVectorFinderPar() {}

    void    clear();
    void    putParams (HParamList *);
    Bool_t  getParams (HParamList *);

    Float_t  getMatchR() const { return fMatchR; }
    Float_t  getMatchCross() const { return fMatchCross; }
    Float_t  getCutX() const { return fCutX; }
    Float_t  getCutY() const { return fCutY; }
    Float_t  getLRCutChi2() const { return fLRCutChi2; }
    Float_t  getHRCutChi2() const { return fHRCutChi2; }
    Float_t  getTanCut() const { return fTanCut; }
    Float_t  getTxyCut() const { return fTxyCut; }
    Float_t  getLRErrU() const { return fLRErrU; }
    Float_t  getHRErrU() const { return fHRErrU; }
    Int_t    getMaxBest() const { return nMaxBest; }
    Int_t    getNpass() const { return nPass; }

    void setMatchR(const Float_t r) { fMatchR = r; }
    void setMatchCross(const Float_t cross) { fMatchCross = cross; }
    void setCutX(const Float_t cutx) { fCutX = cutx; }
    void setCutY(const Float_t cuty) { fCutY = cuty; }
    void setLRCutChi2(const Float_t chi2) { fLRCutChi2 = chi2; }
    void setHRCutChi2(const Float_t chi2) { fHRCutChi2 = chi2; }
    void setTanCut(const Float_t cut) { fTanCut = cut; }
    void setTxyCut(const Float_t cut) { fTxyCut = cut; }
    void setLRErrU(const Float_t err) { fLRErrU = err; }
    void setHRErrU(const Float_t err) { fHRErrU = err; }
    void setMaxBest(const Int_t nbest) { nMaxBest = nbest; }
    void setNpass(const Int_t npass) { nPass = npass; }

    ClassDef (HFwDetVectorFinderPar, 1);
};

#endif  /*!HFWDETVECTORFINDERPAR_H*/
