#ifndef HMDCMINIMIZE_H
#define HMDCMINIMIZE_H
//test 3 
#include "TObject.h"
//#include "hmdcclus.h"

class HMdcMinimize : public TObject {
protected:
   Int_t nPar;
   Double_t parIn[200];
   Double_t parBest[200];

   Double_t grad[200];
   Double_t grad2[200][200];
   Double_t scalePar[200];
   Double_t funct0;
   Double_t functIn;
   Double_t functOut;
   Double_t functBest;
  
   TObject* fObjectFit;            // Pointer to object being fitted 
   Double_t  (*fFCN)(TObject* obj, Int_t &nPar, Double_t* par);
public:
   HMdcMinimize(void);
   ~HMdcMinimize(void);
   void setFCN(TObject *obj,Double_t (*fcn)(TObject *, Int_t &, Double_t *));
  
   Int_t minimize(Int_t maxIter, Int_t nParIn,
		 Double_t* par0, Double_t* stepPar, Double_t* parOut);
   Int_t minimize2(Int_t maxIter, Int_t nParIn,
		  Double_t* par0, Double_t* stepPar, Double_t* parOut);
   void calculationOfGradient(Double_t fun0, Double_t* par, Double_t* stepPar, Int_t nPar,
			     Double_t& agrad, Double_t* cosgrad, Int_t iflag);
   void solutionOfLinearEquationsSystem(Double_t* par,
					Int_t nmOfPar);

   Int_t minpar2(Int_t maxIter, Int_t nParIn,
		 Double_t* par0, Double_t* stepPar, Double_t* parOut);  
   Int_t minbar(Int_t maxIter, Int_t nParIn,
		Double_t* par0,  Double_t* stepPar,Double_t* parOut);  
   Int_t scan(Int_t maxIter, Int_t nParIn,
	      Double_t* par0, Double_t* parOut);  
   Int_t scanXYZ(Int_t maxIter, Int_t nParIn,
		 Double_t* par0, Double_t* parOut);  
   Int_t random(Int_t maxIter, Int_t nParIn,
		 Double_t* par0, Double_t* parOut);  
    
   Int_t cog(Int_t maxIter, Int_t nParIn,
		 Double_t* par0, Double_t* parOut);  
    
   Double_t getFunct0(void) {return funct0;}
   Double_t getFunctional(void) {return functOut;}
   ClassDef(HMdcMinimize,0)
};

#endif
