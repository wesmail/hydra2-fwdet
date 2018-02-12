#ifndef HRICH700PMT_H_
#define HRICH700PMT_H_

#include "TObject.h"
#include "TGraph.h"
#include <vector>
#include <map>

#include "hrich700pmttypeenum.h"

using namespace std;

class HRich700PmtQEData {
public:

    HRich700PmtTypeEnum fDetectorType; // pmt type
    Double_t fLambdaMin; // minimum wavelength in QE table
    Double_t fLambdaMax; // maximum wavelength in QE table
    Double_t fLambdaStep; // wavelaength in QE table
    vector<Double_t> fEfficiency; // Array of QE
};

class HRich700Pmt {

private:

    map<HRich700PmtTypeEnum, HRich700PmtQEData*> fPmtDataMap; // store QE for specific PMT type

    static const Double_t c; // speed of light
    static const Double_t h; // Planck constant
    static const Double_t e; // elementary charge
    static const Double_t NRefrac; // refractive index of the gas

   void initQE();
   void clearMap();


   HRich700Pmt(const HRich700Pmt&);

   HRich700Pmt& operator=(const HRich700Pmt&);

public:
   HRich700Pmt();

   virtual ~HRich700Pmt();

   Bool_t isPhotonDetected(HRich700PmtTypeEnum detType, Double_t collEff, Double_t momentum);

   TGraph* getQEGraph(HRich700PmtTypeEnum detType);

   static Double_t getWavelength(Double_t energy); // eV -> nm

};

#endif /* HRICH700PMT_H_ */
