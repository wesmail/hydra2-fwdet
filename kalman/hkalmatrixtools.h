#ifndef HKALMATRIXTOOLS_H_
#define HKALMATRIXTOOLS_H_

#include "TMatrixD.h"

class HKalMatrixTools : public TObject {

public:
    HKalMatrixTools() { }

    virtual ~HKalMatrixTools() { }

    static Int_t  checkSymmetry  (TMatrixD &M, Double_t tol);

    static Bool_t checkCond      (const TMatrixD &M);

    static Bool_t checkValidElems(const TMatrixD &M);

    static Bool_t decomposeUD    (TMatrixD &M);

    static Bool_t isPosDef       (const TMatrixD &M);

    static Bool_t makeSymmetric  (TMatrixD &M);

    static Bool_t resolveUD      (TMatrixD &U, TMatrixD &D, const TMatrixD &UD);

    ClassDef(HKalMatrixTools, 0)
};

#endif /* HKALMATRIXTOOLS_H_ */
