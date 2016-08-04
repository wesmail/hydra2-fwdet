 #ifndef HSTART2TRB3CALPAR_H
#define HSTART2TRB3CALPAR_H

#include "htrb3calpar.h"

class HStart2Trb3Calpar : public HTrb3Calpar {
public:
   HStart2Trb3Calpar(const Char_t* name = "Start2Trb3Calpar",
                     const Char_t* title = "TRB3 TDC calibration parameters of the Start detector",
                     const Char_t* context = "Trb3CalparProduction",
                     Int_t minTrbnetAddress = Trbnet::kStartTrb3MinTrbnetAddress,
                     Int_t maxTrbnetAddress = Trbnet::kStartTrb3MaxTrbnetAddress)
     : HTrb3Calpar(name,title,context,minTrbnetAddress,maxTrbnetAddress) {}
   Bool_t init(HParIo* input, Int_t* set);
   Int_t write(HParIo* output);
   ClassDef(HStart2Trb3Calpar, 1) // START2 TRB3 TDC calibration parameters
};

#endif  /*!HSTART2TRB3CALPAR_H*/
