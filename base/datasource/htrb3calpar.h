#ifndef HTRB3CALPAR_H
#define HTRB3CALPAR_H

#include "TObjArray.h"
#include "TObject.h"
#include "TArrayF.h"

#include "hparset.h"
#include "htrbnetdef.h"

#include <fstream>

using namespace Trbnet;
using namespace std;

class HTrb3CalparTdc: public TObject {
protected:
  Int_t   subEvtId;        // subevent id the TDC belongs to
  Int_t   nChannels;       // number of channels
  Int_t   nBinsPerChannel; // number of bins per channel
  Int_t   nEdgesMask;      // 1 - only rising edges, 2 - only falling edges, 3 - both edges
  TArrayF binsPar;         // all bins
public:
  HTrb3CalparTdc(void);
  ~HTrb3CalparTdc(void) {}
  Int_t getSubEvtId(void)        { return subEvtId; }
  Int_t getEdgesMask(void)       { return nEdgesMask; }
  Int_t getNChannels(void)       { return nChannels; }
  Int_t getNBinsPerChannel(void) { return nBinsPerChannel; }
  Float_t* getBinsPar(void)      { return binsPar.GetArray(); }
  Int_t getArraySize(void)       { return binsPar.GetSize(); }

  Float_t* getRisingArr(Int_t chan) {
    return nEdgesMask & 1 ? getBinsPar() + chan*nBinsPerChannel : 0;
  }

  Float_t* getFallingArr(Int_t chan) {
     switch (nEdgesMask & 3) {
        case 2: return getBinsPar() + chan*nBinsPerChannel;
        case 3: return getBinsPar() + (chan+nChannels)*nBinsPerChannel;
        default: return 0;
     }
  }

  Float_t getRisingPar(Int_t chan,Int_t bin) const {
    return nEdgesMask & 1 ? binsPar[chan*nBinsPerChannel+bin] : 0.F;
  }

  Float_t getFallingPar(Int_t chan,Int_t bin) const {
     switch (nEdgesMask & 3) { 
        case 2: return binsPar[chan*nBinsPerChannel+bin];
        case 3: return binsPar[(chan+nChannels)*nBinsPerChannel+bin];
        default: return 0.F;
     }
    return 0.F;
  }

  void clear(void);
  Int_t makeArray(Int_t,Int_t,Int_t,Int_t);
  Bool_t fillArray(Float_t*,Int_t);
  Bool_t loadFromBinaryFile(const char* filename, Int_t subevtid, Int_t numBins=600, Int_t nEdgesMask=1);
  void setSimpleFineCalibration(UInt_t, UInt_t);
  void print(void);
  void write(fstream&);
  ClassDef(HTrb3CalparTdc, 1) // TDC level of the TRB3 TDC calibration parameters
};


class HTrb3Calpar : public HParSet {
protected:
   TObjArray* array;       // array of pointers of type HTrb3CalparTdc
   Int_t      arrayOffset; // offset to calculate the index
public:
   HTrb3Calpar(const Char_t* name = "",
               const Char_t* title = "",
               const Char_t* context = "",
               Int_t minTrbnetAddress = -1,
               Int_t maxTrbnetAddress = -1);
   virtual ~HTrb3Calpar();
   HTrb3CalparTdc* getTdc(Int_t trbnetAddress) {
      return (trbnetAddress<arrayOffset) ? 0 : (HTrb3CalparTdc*)(array->At(trbnetAddress - arrayOffset));
   }
   HTrb3CalparTdc* operator[](Int_t i) {
      return static_cast<HTrb3CalparTdc*>((*array)[i]);
   }
   Int_t getSize() {
      return array->GetLast() + 1;
   }
   Int_t getArrayOffset() {
      return arrayOffset;
   }
   virtual Bool_t init(HParIo* input, Int_t* set) {return kFALSE;}
   virtual Int_t write(HParIo* output)            {return -1;}
   void clear();
   HTrb3CalparTdc* addTdc(Int_t);
   void printParam();
   virtual void putAsciiHeader(TString&);
   void write(fstream&);

   Bool_t loadFromBinaryFiles(const char* basefname, Int_t subevtid, Int_t numBins=600, Int_t nEdgesMask=1);

   ClassDef(HTrb3Calpar, 1) // Base class for the TRB3 TDC calibration parameters
};

#endif  /*!HTRB3CALPAR_H*/
