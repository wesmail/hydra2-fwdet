#ifndef HPARAMLIST_H
#define HPARAMLIST_H

#include "TNamed.h"
#include "TString.h"
#include "TArrayI.h"
#include "TArrayC.h"
#include "TArrayF.h"
#include "TArrayD.h"
#include "TList.h"
#include "TFile.h"
#include "TROOT.h"

class HParamObj : public TNamed {
protected:
  UChar_t* paramValue;    // Pointer to binary array
  Int_t arraySize;        // Size of binary array
  TString paramType;      // Type of parameter value or class name
  Bool_t basicType;       // kTRUE for C-types and C-type parameter arrays, kFALSE for classes
  Int_t bytesPerValue;    // number of bytes per value
  Int_t classVersion;     // Code version of classes stored as binary
  UChar_t* streamerInfo;  // Pointer to binary array container the streamer info
  Int_t streamerInfoSize; // Size of streamer info array
public:
  HParamObj(const Text_t* name="");
  HParamObj(HParamObj&);
  HParamObj(const Text_t*,Int_t);
  HParamObj(const Text_t*,UInt_t);
  HParamObj(const Text_t*,Float_t);
  HParamObj(const Text_t*,Double_t);
  HParamObj(const Text_t*,const Int_t*,const Int_t);
  HParamObj(const Text_t*,const UInt_t*,const Int_t);
  HParamObj(const Text_t*,const Float_t*,const Int_t);
  HParamObj(const Text_t*,const Double_t*,const Int_t);
  HParamObj(const Text_t*,const Text_t*);
  HParamObj(const Text_t*,const Char_t*,const Int_t);
  HParamObj(const Text_t*,const UChar_t*,const Int_t);
  ~HParamObj();
  void setParamType(const Text_t* t);
  UChar_t* setLength(Int_t l);
  void setParamValue(UChar_t*,const Int_t);
  void setClassVersion(const Int_t v) { classVersion=v; }
  UChar_t* setStreamerInfoSize(Int_t);
  void setStreamerInfo(UChar_t*,const Int_t);
  UChar_t* getParamValue() { return paramValue; }
  Bool_t isBasicType() { return basicType; }
  const char* getParamType() { return paramType.Data(); }
  Int_t getBytesPerValue() { return bytesPerValue; }
  Int_t getClassVersion() { return classVersion; }
  Int_t getLength() { return arraySize; }
  Int_t getNumParams();
  UChar_t* getStreamerInfo() { return streamerInfo; }
  Int_t getStreamerInfoSize() { return streamerInfoSize; }
  void print();  
protected:
  template <class type> void printData(type*,Int_t);
  void printHexData(UInt_t*,Int_t);

  ClassDef(HParamObj,0) // Class for binary parameter object (name + binary array)
};


class HParamList : public TObject {
protected:
  TList* paramList;      // List for parameters stored as string
  class HParamTFile : public TFile {
    public:
      HParamTFile() {
        // Create StreamerInfo index
        Int_t lenIndex = gROOT->GetListOfStreamerInfo()->GetSize()+1;
        if (lenIndex < 5000) lenIndex = 5000;
        fClassIndex = new TArrayC(lenIndex);
      }
      ~HParamTFile() {
        delete fClassIndex;
        fClassIndex=0;
      }
  };
public:
  HParamList();
  ~HParamList();
  void add(HParamObj&);
  void add(const Text_t*,const Text_t*);
  void add(const Text_t*,Int_t);
  void add(const Text_t*,UInt_t);
  void add(const Text_t*,Float_t);
  void add(const Text_t*,Double_t);
  void add(const Text_t*,TArrayI&);
  void add(const Text_t*,TArrayC&);
  void add(const Text_t*,TArrayF&);
  void add(const Text_t*,TArrayD&);
  void add(const Text_t*,const UChar_t*,const Int_t);
  void add(const Text_t*,const Int_t*,const Int_t);
  void add(const Text_t*,const UInt_t*,const Int_t);
  void add(const Text_t*,const Float_t*,const Int_t);
  void add(const Text_t*,const Double_t*,const Int_t);
  void addObject(const Text_t*,TObject*);
  Bool_t fill(const Text_t*,Text_t*,const Int_t);
  Bool_t fill(const Text_t*,Int_t*,const Int_t nValues=1);  
  Bool_t fill(const Text_t*,UInt_t*,const Int_t nValues=1);  
  Bool_t fill(const Text_t*,Float_t*,const Int_t nValues=1);  
  Bool_t fill(const Text_t*,Double_t*,const Int_t nValues=1);  
  Bool_t fill(const Text_t*,UChar_t*,const Int_t nValues=1);
  Bool_t fill(const Text_t*,TArrayI*);  
  Bool_t fill(const Text_t*,TArrayC*);  
  Bool_t fill(const Text_t*,TArrayF*);  
  Bool_t fill(const Text_t*,TArrayD*);  
  Bool_t fillObject(const Text_t*,TObject*);
  Int_t replace(const Text_t*,UChar_t*);
  Int_t replace(const Text_t*,Int_t*);
  Int_t replace(const Text_t*,UInt_t*);
  Int_t replace(const Text_t*,Float_t*);
  Int_t replace(const Text_t*,Double_t*);
  void print();
  HParamObj* find(const Text_t* name) {
    return (HParamObj*)paramList->FindObject(name);
  }
  TList* getList() { return paramList; }
  ClassDef(HParamList,0) // Class for lists of parameters (of type HParamObj)
};

#endif  /* !HPARAMLIST_H */
 
