
#include "hkalmatrixtools.h"

// from ROOT
#include "TDecompSVD.h"
#include "TMatrixDEigen.h"
#include "TMath.h"

// C/C++ libraries
#include <limits>
using namespace std;

ClassImp(HKalMatrixTools)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// Some helper functions for matrices.
//
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


Int_t HKalMatrixTools::checkSymmetry(TMatrixD &M, Double_t tol) {
    // Mind the meaning of the return value!
    //
    // Checks if input matrix M is (close to) symmetric.
    // Returns the amount of off-diagonal elements that were not
    // symmetric. Therefore a return value of 0 means the matrix
    // is symmetric!
    // For non quadratic matrices -1 will be returned.
    //
    // Input:
    // M:   The matrix to check.
    // tol: The allowed relative difference between diagonal
    //      elements so that the matrix is still seen as symmetric.

    Int_t nAsymCells = 0;
    if(M.GetNrows() != M.GetNcols()) {
        return -1;
    }

    Int_t dim = M.GetNrows();

    for(Int_t i = 0; i < dim; i++) {
        for(Int_t j = i+1; j < dim; j++) {
	    if(!TMath::AreEqualRel(M(i,j), M(j,i), tol)) {
                nAsymCells++;
            }
        }
    }

    return nAsymCells;
}

Bool_t HKalMatrixTools::checkCond(const TMatrixD &M) {
    // Calculate matrix condition using singular values.

    Bool_t bCondOk = kTRUE;

    // Machine precision:
    // 1 + eps   evaluates to 1 + eps
    // 1 + eps/2 evaluates to 1.
    Double_t eps = numeric_limits<Double_t>::epsilon();
    // A matrix is considered ill-conditioned for inversion if adding one to
    // its condition number in computer arithmetic has no effect.
    Double_t illCond = 2./eps;

    TDecompSVD decomp(M);
    Bool_t decompSuccess = decomp.Decompose();
    if(decompSuccess) {
        Double_t cond = decomp.Condition();
        if(cond >= illCond) {
            bCondOk = kFALSE;
        }
    } else {
        bCondOk = kFALSE;
        ::Warning("checkCond()", "Failed at SVD decomposition for matrix");
    }
    return bCondOk;
}

Bool_t HKalMatrixTools::decomposeUD(TMatrixD &M) {
    // A matrix can be written as the product M = U*D*U^T where
    // U is an upper triangular and D a diagonal matrix.
    // This function calculates the U and D factors of the input
    // matrix M. Both U and D can be stored in a single matrix.
    // Since the diagonal elements of U are always 1, they are
    // not explicitly stored and replaced by the elements of D.
    //
    // The input matrix M will be overwritten.

    Bool_t bNoErr = kTRUE;

#if kalDebug > 0
    if(checkSymmetry(M, 1.e-3) != 0) {
        ::Warning("decomposeUD()", "Input matrix is not symmetric.");
        M.Print();
        bNoErr = kFALSE;
    }
#endif

    Int_t dim = M.GetNrows();
    for(Int_t j = dim - 1; j >= 0; j--) {
        for(Int_t i = j; i >= 0; i--) {
            Double_t sigma = M(i,j);
            for(Int_t k = j+1; k < dim; k++) {
                sigma -= M(i,k) * M(k,k) * M(j,k);
            }
            if(i == j) {
                M(i,i) = sigma;
            } else {
                if(M(j,j) != 0.) {
                    M(i,j) = sigma / M(j,j);
                } else {
                    M(i,j) = 0.;
                }
            }
        }
    }

    return bNoErr;
}

Bool_t HKalMatrixTools::isPosDef(const TMatrixD &M) {
    // Returns true if matrix is positive definite, i.e. all eigenvalues are
    // real and greater than zero.

    Bool_t bPosDef = kTRUE;

    TMatrixDEigen eig(M);
    TVectorD eigRe  = eig.GetEigenValuesRe();
    TVectorD eigIm  = eig.GetEigenValuesIm();

    for(Int_t i = 0; i < eigRe.GetNrows(); i++) {
        if(eigRe(i) <= 0.) {
            bPosDef = kFALSE;
        }
        if(eigIm(i) != 0.) {
            bPosDef = kFALSE;
        }
    }

#if kalDebug > 0
    if(!bPosDef) {
        ::Warning("isPosDef()", "Matrix is not positive definite. An eigenvalue is not positive or is imaginary.");
    }
#endif
    return bPosDef;
}

Bool_t HKalMatrixTools::checkValidElems(const TMatrixD &M) {
    // Check for INFs and NaNs in matrix.

    for(Int_t iRow = 0; iRow < M.GetNrows(); iRow++) {
        for(Int_t iCol = 0; iCol < M.GetNcols(); iCol++) {
	    if(M(iRow, iCol) == numeric_limits<Double_t>::infinity() ||
	       TMath::IsNaN(M(iRow, iCol))) {
		return kFALSE;
            }
        }
    }
    return kTRUE;
}

Bool_t HKalMatrixTools::makeSymmetric(TMatrixD &M) {
    // Produce a valid symmetric matrix out of an almost symmetric TMatrixD
    // Input matrix M will be overwritten!

    Bool_t matSym = kTRUE;
    Int_t idx = TMath::Min(M.GetNrows(), M.GetNcols());

    for(Int_t i = 0; i < idx; i++) {
        for(Int_t j = i+1; j < idx; j++) {
            if(matSym && !TMath::AreEqualRel(M(i,j), M(j,i), 1.e-3)) {
                matSym = kFALSE;
            }
            Double_t average = (M(i,j) + M(j,i)) / 2.;
            M(i,j) = average;
            M(j,i) = average;
        }
    }
    return matSym;
}


Bool_t HKalMatrixTools::resolveUD(TMatrixD &U, TMatrixD &D, const TMatrixD &UD) {
    // Input is a matrix where the U and D factors are stored in a single
    // matrix.
    //
    // Extracts the U and D matrices as two separate objects from the input
    // matrix.
    // output:
    // U: upper triangular matrix.
    // D: diagonal matrix.
    // input:
    // UD: The U and D factors are stored in this single matrix.

    if(UD.GetNrows() != UD.GetNcols()) {
        ::Error("resolveUD()", "Input matrix is not quadratic.");
        return kFALSE;
    }
    Int_t dim = UD.GetNrows();
    if(U.GetNrows() != dim || U.GetNcols() != dim) {
	::Warning("resolveUD()",
		  "Output parameter for upper triangular matrix has wrong dimensions and has been resized.");
        U.ResizeTo(dim, dim);
    }
    if(D.GetNrows() != dim || D.GetNcols() != dim) {
	::Warning("resolveUD()",
		  "Output parameter for diagonal matrix has wrong dimensions and has been resized.");
        D.ResizeTo(dim, dim);
    }

#ifdef kalDebug
        Bool_t bWarnU = kFALSE;
        Bool_t bWarnD = kFALSE;
#endif

    U.UnitMatrix();
    for(Int_t i = 0; i < dim; i++) {
        D(i,i) = UD(i,i);
#ifdef kalDebug
        for(Int_t j = 0; j < dim; j++) {
            if(i < j) {
                U(i,j) = UD(i,j);
            } else {
                if(i != j && U(i,j) != 0.) {
                    if(!bWarnU) {
			::Warning("resolveUD()",
				  "Input matrix U is not an upper triangular matrix.");
                    }
                    bWarnU = kTRUE;
                    U.Print();
                }
            }
            if(i != j && D(i,j) != 0.) {
                if(!bWarnD) {
		    ::Warning("resolveUD()",
			      "Input matrix D is not diagonal.");
                }
                bWarnD = kTRUE;
                D.Print();
            }
        }
    }
    return !(bWarnU || bWarnD);
#else
        for(Int_t j = i + 1; j < dim; j++) {
            U(i,j) = UD(i,j);
        }
    }

    return kTRUE;
#endif
}
