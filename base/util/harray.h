//*-- Author : Jochen Markert

#ifndef  __HARRAY_H__
#define  __HARRAY_H__

#include "TNamed.h"
#include "TArrayC.h"
#include "TArrayS.h"
#include "TArrayI.h"
#include "TArrayL.h"
#include "TArrayL64.h"
#include "TArrayF.h"
#include "TArrayD.h"

#include <iostream>
#include <iomanip>

using namespace std;


class HArrayC : public TNamed {
public:
    TArrayC* data;
    HArrayC()                      { data = NULL;};
    HArrayC(Int_t n)               { data = new TArrayC(n);}
    HArrayC(Int_t n , Char_t* dat) { data = new TArrayC(n,dat);}
    HArrayC(TArrayC& dat)          { data = new TArrayC(dat.GetSize(),dat.GetArray());}
    ~HArrayC()                     { if(data) delete data; }
    Char_t&  operator[](Int_t i)       { return (*data)[i]; };
    Char_t   operator[](Int_t i) const { return (*data)[i]; };
    TArrayC* getData()             { return data;}
    Char_t*  getArray()            { if(data) return data->GetArray(); else return NULL;}
    Int_t    getSize()             { if(data) return data->GetSize();  else return 0;}
    void     reset(Char_t val=0)   { if(data) data->Reset(val); }
    void     print()               { if(data) { cout<<GetName()<<"---------------------"<<endl; for(Int_t i=0;i < getSize();i++) {cout<<setw(5)<<i<<(*data)[i]<<endl;}}}
    ClassDef(HArrayC,1)
};

class HArrayS : public TNamed {
public:
    TArrayS* data;
    HArrayS()                       { data = NULL;};
    HArrayS(Int_t n)                { data = new TArrayS(n);}
    HArrayS(Int_t n , Short_t* dat) { data = new TArrayS(n,dat);}
    HArrayS(TArrayS& dat)           { data = new TArrayS(dat.GetSize(),dat.GetArray());}
    ~HArrayS()                      { if(data) delete data; }
    Short_t&  operator[](Int_t i)       { return (*data)[i]; };
    Short_t   operator[](Int_t i) const { return (*data)[i]; };
    TArrayS* getData()              { return data;}
    Short_t* getArray()             { if(data) return data->GetArray(); else return NULL;}
    Int_t    getSize()              { if(data) return data->GetSize();  else return 0;}
    void     reset(Short_t val=0)   { if(data) data->Reset(val); }
    void     print()                { if(data) { cout<<GetName()<<"---------------------"<<endl; for(Int_t i=0;i < getSize();i++) {cout<<setw(5)<<i<<(*data)[i]<<endl;}}}
    ClassDef(HArrayS,1)
};

class HArrayI : public TNamed {
public:
    TArrayI* data;
    HArrayI()                     { data = NULL;};
    HArrayI(Int_t n)              { data = new TArrayI(n);}
    HArrayI(Int_t n , Int_t* dat) { data = new TArrayI(n,dat);}
    HArrayI(TArrayI& dat)         { data = new TArrayI(dat.GetSize(),dat.GetArray());}
    ~HArrayI()                    { if(data) delete data; }
    Int_t&  operator[](Int_t i)       { return (*data)[i]; };
    Int_t   operator[](Int_t i) const { return (*data)[i]; };
    TArrayI* getData()            { return data;}
    Int_t*   getArray()           { if(data) return data->GetArray(); else return NULL;}
    Int_t    getSize()            { if(data) return data->GetSize();  else return 0;}
    void     reset(Int_t val=0)   { if(data) data->Reset(val); }
    void     print()              { if(data) { cout<<GetName()<<"---------------------"<<endl; for(Int_t i=0;i < getSize();i++) {cout<<setw(5)<<i<<(*data)[i]<<endl;}}}
    ClassDef(HArrayI,1)
};

class HArrayL : public TNamed {
public:
    TArrayL* data;
    HArrayL()                      { data = NULL;};
    HArrayL(Int_t n)               { data = new TArrayL(n);}
    HArrayL(Int_t n , Long_t* dat) { data = new TArrayL(n,dat);}
    HArrayL(TArrayL& dat)          { data = new TArrayL(dat.GetSize(),dat.GetArray());}
    ~HArrayL()                     { if(data) delete data; }
    Long_t&  operator[](Int_t i)       { return (*data)[i]; };
    Long_t   operator[](Int_t i) const { return (*data)[i]; };
    TArrayL* getData()             { return data;}
    Long_t*  getArray()            { if(data) return data->GetArray(); else return NULL;}
    Int_t    getSize()             { if(data) return data->GetSize();  else return 0;}
    void     reset(Long_t val=0)   { if(data) data->Reset(val); }
    void     print()               { if(data) { cout<<GetName()<<"---------------------"<<endl; for(Int_t i=0;i < getSize();i++) {cout<<setw(5)<<i<<(*data)[i]<<endl;}}}
    ClassDef(HArrayL,1)
};

class HArrayL64 : public TNamed {
public:
    TArrayL64* data;
    HArrayL64()                        { data = NULL;};
    HArrayL64(Int_t n)                 { data = new TArrayL64(n);}
    HArrayL64(Int_t n , Long64_t* dat) { data = new TArrayL64(n,dat);}
    HArrayL64(TArrayL64& dat)          { data = new TArrayL64(dat.GetSize(),dat.GetArray());}
    ~HArrayL64()                       { if(data) delete data; }
    Long64_t&  operator[](Int_t i)       { return (*data)[i]; };
    Long64_t   operator[](Int_t i) const { return (*data)[i]; };
    TArrayL64* getData()               { return data;}
    Long64_t*  getArray()              { if(data) return data->GetArray(); else return NULL;}
    Int_t      getSize()               { if(data) return data->GetSize();  else return 0;}
    void       reset(Long64_t val=0)   { if(data) data->Reset(val); }
    void       print()                 { if(data) { cout<<GetName()<<"---------------------"<<endl; for(Int_t i=0;i < getSize();i++) {cout<<setw(5)<<i<<(*data)[i]<<endl;}}}
    ClassDef(HArrayL64,1)
};

class HArrayF : public TNamed {
public:
    TArrayF* data;
    HArrayF()                       { data = NULL;};
    HArrayF(Int_t n)                { data = new TArrayF(n);}
    HArrayF(Int_t n , Float_t* dat) { data = new TArrayF(n,dat);}
    HArrayF(TArrayF& dat)           { data = new TArrayF(dat.GetSize(),dat.GetArray());}
    ~HArrayF()                      { if(data) delete data; }
    Float_t&  operator[](Int_t i)       { return (*data)[i]; };
    Float_t   operator[](Int_t i) const { return (*data)[i]; };
    TArrayF*  getData()             { return data;}
    Float_t*  getArray()            { if(data) return data->GetArray(); else return NULL;}
    Int_t     getSize()             { if(data) return data->GetSize();  else return 0;}
    void      reset(Float_t val=0)  { if(data) data->Reset(val); }
    void      print()               { if(data) { cout<<GetName()<<"---------------------"<<endl; for(Int_t i=0;i < getSize();i++) {cout<<setw(5)<<i<<(*data)[i]<<endl;}}}
    ClassDef(HArrayF,1)
};

class HArrayD : public TNamed {
public:
    TArrayD* data;
    HArrayD()                        { data = NULL;};
    HArrayD(Int_t n)                 { data = new TArrayD(n);}
    HArrayD(Int_t n , Double_t* dat) { data = new TArrayD(n,dat);}
    HArrayD(TArrayD& dat)            { data = new TArrayD(dat.GetSize(),dat.GetArray());}
    ~HArrayD()                       { if(data) delete data; }
    Double_t&  operator[](Int_t i)       { return (*data)[i]; };
    Double_t   operator[](Int_t i) const { return (*data)[i]; };
    TArrayD*   getData()             { return data;}
    Double_t*  getArray()            { if(data) return data->GetArray(); else return NULL;}
    Int_t      getSize()             { if(data) return data->GetSize();  else return 0;}
    void       reset(Double_t val=0) { if(data) data->Reset(val); }
    void       print()               { if(data) { cout<<GetName()<<"---------------------"<<endl; for(Int_t i=0;i < getSize();i++) {cout<<setw(5)<<i<<(*data)[i]<<endl;}}}
    ClassDef(HArrayD,1)
};

#endif /* !__HARRAY_H__ */




