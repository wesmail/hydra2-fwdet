#ifndef HDBCOLUMNTYPE
#define HDBCOLUMNTYPE

#include "TNamed.h"

class HDbColumnType : public TNamed {
friend class HDbColumn;
protected:
  Int_t   varLength;        // Length in bytes
  Int_t   dataPrecision;    // Data precision
  Int_t   dataScale;        // Data scale
public:
  HDbColumnType() {}
  HDbColumnType(const Char_t*, Int_t l=0, Int_t p=0, Int_t s=0 );
  virtual ~HDbColumnType() {}
  virtual TString getTypeString() { return TString(GetName()); }
  virtual TString getSelectString(const Char_t*, const Char_t*);
  virtual TString getNvlSelectString(const Char_t*, const Char_t*, const Char_t*);
  ClassDef(HDbColumnType,0) // Base class for Oracle column types
};

class HDbColCharType : public HDbColumnType {
friend class HDbColumn;
public:
  HDbColCharType(Int_t l=1);
  ~HDbColCharType() {}
  TString getTypeString();
  TString getSelectString(const Char_t*, const Char_t*);
  TString getNvlSelectString(const Char_t*, const Char_t*, const Char_t*);
  ClassDef(HDbColCharType,0) // Class for Oracle type CHAR
};

class HDbColVcType : public HDbColumnType {
friend class HDbColumn;
public:
  HDbColVcType(Int_t l=80);
  ~HDbColVcType() {}
  TString getTypeString();
  TString getSelectString(const Char_t*, const Char_t*);
  TString getNvlSelectString(const Char_t*, const Char_t*, const Char_t*);
  ClassDef(HDbColVcType,0) // Class for Oracle type VARCHAR2
};

class HDbColNumType : public HDbColumnType {
friend class HDbColumn;
public:
  HDbColNumType(Int_t p=40, Int_t s=0);
  ~HDbColNumType() {}
  TString getTypeString();
  TString getSelectString(const Char_t*, const Char_t*);
  TString getNvlSelectString(const Char_t*, const Char_t*, const Char_t*);
  ClassDef(HDbColNumType,0) // Class for Oracle type NUMBER
};

class HDbColDateType : public HDbColumnType {
friend class HDbColumn;
public:
  HDbColDateType(Int_t l=75);
  ~HDbColDateType() {}
  TString getSelectString(const Char_t*, const Char_t*);
  TString getNvlSelectString(const Char_t*, const Char_t*, const Char_t*);
  ClassDef(HDbColDateType,0) // Class for Oracle type DATE
};

class HDbColLongType : public HDbColumnType {
friend class HDbColumn;
public:
  HDbColLongType(Int_t l=32767);
  ~HDbColLongType() {}
  ClassDef(HDbColLongType,0) // Class for Oracle type LONG
};

class HDbColRowidType : public HDbColumnType {
friend class HDbColumn;
public:
  HDbColRowidType() : HDbColumnType("ROWID",18) {}
  ~HDbColRowidType() {}
  TString getSelectString(const Char_t*, const Char_t*);
  TString getNvlSelectString(const Char_t*, const Char_t*, const Char_t*);
  ClassDef(HDbColRowidType,0) // Class for Oracle type ROWID
};

class HDbColRawType : public HDbColumnType {
friend class HDbColumn;
public:
  HDbColRawType(Int_t l=255);
  ~HDbColRawType() {}
  TString getTypeString();
  ClassDef(HDbColRawType,0) // Class for Oracle type RAW
};

#endif /* !HDBCOLUMNTYPE_H */
