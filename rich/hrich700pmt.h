#ifndef HRICH700PMT_H_
#define HRICH700PMT_H_

#include "TObject.h"

class HRich700Pmt {

private:
    Double_t fCollectionEfficiency; // collection efficiency. Final QE = QE * fCollectionEfficiency
    Int_t fDetectorType; // pmt type
    Double_t fLambdaMin; // minimum wavelength in QE table
    Double_t fLambdaMax; // maximum wavelength in QE table
    Double_t fLambdaStep; // wavelaength in QE table
    Double_t fEfficiency[200]; // Array of QE

    static const Double_t c; // speed of light
    static const Double_t h; // Planck constant
    static const Double_t e; // elementary charge
    static const Double_t NRefrac; // refractive index of the gas
 
   void InitQE();


   HRich700Pmt(const HRich700Pmt&);

   HRich700Pmt& operator=(const HRich700Pmt&);

public:
   HRich700Pmt();

   virtual ~HRich700Pmt();

   Bool_t isPhotonDetected(Double_t momentum);

   void SetDetectorType(Int_t detType){
      fDetectorType = detType;
      InitQE();
   }

   void SetCollectionEfficiency(Double_t collEff){fCollectionEfficiency = collEff;}

   static Double_t getWavelength(Double_t energy); // eV -> nm

};

#endif /* HRICH700PMT_H_ */
