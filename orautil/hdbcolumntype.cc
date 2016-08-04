//*-- AUTHOR : Ilse Koenig
//*-- Modified : 11/09/2000

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////
// HDbColumnType
//
// Base class for Oracle column types
//
// Derived classes are:
//   HDbColCharType  for Oracle type CHAR
//   HDbColVcType                    VARCHAR2
//   HDbColNumType                   NUMBER
//   HDbColDateType                  DATE
//   HDbColRowidType                 ROWID
//   HDbColLongType                  LONG
//   HDbColRawType                   RAW
//
//   Because of problems to convert LONG and RAW types to VARCHAR2, the
//   datatypes LONG and RAW are actually not supported.
//   
/////////////////////////////////////////////////////////////////////////

#include "hdbcolumntype.h"

ClassImp(HDbColumnType)
ClassImp(HDbColCharType)
ClassImp(HDbColVcType)
ClassImp(HDbColNumType)
ClassImp(HDbColDateType)
ClassImp(HDbColLongType)
ClassImp(HDbColRowidType)
ClassImp(HDbColRawType)

HDbColumnType::HDbColumnType(const Char_t* n, Int_t l, Int_t p, Int_t s ) {
  // Constructor
  SetName(n);
  varLength=l;
  dataPrecision=p;
  dataScale=s;
}  

TString HDbColumnType::getSelectString(const Char_t* col, const Char_t*) {
  // Base class funtion returns an emtpy string because the current type is not
  // supported
  Error("HDbColumnType::getNvlSelectString",
        "\n  Type %s of columnn %s actually not supported",
        GetName(), col);
  TString s("");
  return s;
}

TString HDbColumnType::getNvlSelectString(const Char_t* col, const Char_t* td,
                                          const Char_t*){
  // Base class funtion returns an emtpy string because the current type is not
  // supported
  TString s(getSelectString(col,td));
  return s;
}

HDbColCharType::HDbColCharType(Int_t l) : HDbColumnType("CHAR",l) {
  // Constructor for type CHAR (default length 1)
  if (l<=0) varLength=1;
}

TString HDbColCharType::getTypeString() {
  // Returns the type string, e.g. "CHAR(1)"
  TString s=("CHAR(");
  Char_t buf[10];
  sprintf(buf,"%i",varLength);
  s=s + buf + ")";
  return s;
}

TString HDbColCharType::getSelectString(const Char_t* col, const Char_t* td) {
  // Returns the name of the column enclosed in given text delimiter
  TString s;
  if (varLength>1) {
    if (strcmp(td,"'")==0) s=s + "''''||" + col + "||''''";
    else s=s + "'" + td + "'||" + col + "||'" + td + "'";
  }
  else s=col;
  return s;
}

TString HDbColCharType::getNvlSelectString(const Char_t* col, const Char_t* td,
                                           const Char_t* sNull) {
  // Returns the name of the column enclosed in given text delimiter or specified
  // null string if column value is null
  TString s;
  if (varLength>1) {
    if (strcmp(td,"'")==0)
       s=s + "''''||nvl(" + col + ",'" + sNull + "')||''''";
    else s=s + "'" + td + "'||nvl(" + col + ",'" + sNull + "')||'"
         + td + "'";
  }
  else s=s + "nvl(" + col + ",'" + sNull + "')";
  return s;
}

HDbColVcType::HDbColVcType(Int_t l) : HDbColumnType("VARCHAR2",l) {
  // Constructor for type VARCHAR2 (default length 80)
  if (l<=0) varLength=80;
}

TString HDbColVcType::getTypeString() {
  // Returns the type string, e.g. "VARCHAR2(80)"
  TString s=("VARCHAR2(");
  Char_t buf[10];
  sprintf(buf,"%i",varLength);
  s=s + buf + ")";
  return s;
}

TString HDbColVcType::getSelectString(const Char_t* col, const Char_t* td) {
  // Returns the name of the column enclosed in given text delimiter
  TString s;
  if (varLength>1) {
    if (strcmp(td,"'")==0) s=s + "''''||" + col + "||''''";
    else s=s + "'" + td + "'||" + col + "||'" + td + "'";
  }
  else s=col;
  return s;
}

TString HDbColVcType::getNvlSelectString(const Char_t* col, const Char_t* td,
                                           const Char_t* sNull) {
  // Returns the name of the column enclosed in given text delimiter or specified
  // null string if column value is null
  TString s;
  if (varLength>1) {
    if (strcmp(td,"'")==0)
       s=s + "''''||nvl(" + col + ",'" + sNull + "')||''''";
    else s=s + "'" + td + "'||nvl(" + col + ",'" + sNull + "')||'"
         + td + "'";
  }
  else s=s + "nvl(" + col + ",'" + sNull + "')";
  return s;
}

HDbColNumType::HDbColNumType(Int_t p, Int_t s)
               : HDbColumnType("NUMBER",sizeof(Double_t),p,s) {
  // Constructor for type NUMBER with specified precision and scale (default precision 40)
  if (p<=0) dataPrecision=40;
}

TString HDbColNumType::getTypeString() {
  // Returns the type string "NUMBER(p,s)"
  TString s=("NUMBER");
  if (dataPrecision==40) return s;
  Char_t buf[100];
  sprintf(buf,"%i",dataPrecision);
  s=s + "(" + buf;
  if (dataScale>=0) {
    sprintf(buf,"%i",dataScale);
    s=s + "," + buf;
  }
  s=s + ")";
  return s;
}

TString HDbColNumType::getSelectString(const Char_t* col, const Char_t*) {
  // Returns the string to convert the number column to characters
  // null string if column value is null
  TString s("to_char(");
  s=s + col + ")";
  return s;
}

TString HDbColNumType::getNvlSelectString(const Char_t* col, const Char_t*, const Char_t* sNull) {
  // Returns the string to convert the number column to characters or to replace by the
  // specified null string if the column value is null
  TString s("nvl(to_char(");
  s=s + col + "),'" + sNull + "')";
  return s;
}

HDbColDateType::HDbColDateType(Int_t l) : HDbColumnType("DATE",l) {
  // Constructor for type DATE
  if (l<=0) varLength=75;
}

TString HDbColDateType::getSelectString(const Char_t* col, const Char_t* td) {
  // Returns the string to convert the date column to characters enclosed in specified
  // text delimiter to allow a blank between the date and the time
  TString s;
  if (strcmp(td,"'")==0)
     s=s + "''''||to_char(" + col + ")||''''";
  else s=s + "'" + td + "'||to_char(" + col + ")||'" + td + "'";
  return s;
}

TString HDbColDateType::getNvlSelectString(const Char_t* col, const Char_t* td,
                                           const Char_t* sNull) {
  // Returns the string to convert the date column to characters enclosed in specified
  // text delimiter to allow a blank between the date and the time
  // or the specified null string if the column value is null
  TString s;
  if (strcmp(td,"'")==0)
     s=s + "''''||nvl(to_char(" + col + "),'" + sNull + "')||''''";
  else s=s + "'" + td + "'||nvl(to_char(" + col + "),'" + sNull + "')||'"
         + td + "'";
  return s;
}

HDbColLongType::HDbColLongType(Int_t l) : HDbColumnType("LONG",l) {
  // Constructor for type LONG (default length 32767)
  // WARNING: This datatype is actually not supported!
  if (l<=0) varLength=32767;
}

TString HDbColRowidType::getSelectString(const Char_t* col, const Char_t*) {
  // Returns the string to convert the rowid column to characters
  TString s("rowidtochar(");
  s=s + col + ")";
  return s;
}

TString HDbColRowidType::getNvlSelectString(const Char_t* col, const Char_t*,
                                            const Char_t* sNull) {
  // Returns the string to convert the rowid column to characters or the specified
  // null string if the column value is null
  TString s("nvl(rowidtochar(");
  s=s + col + "),'" + sNull + "')";
  return s;
}

HDbColRawType::HDbColRawType(Int_t l) : HDbColumnType("RAW",l) {
  // Constructor for type RAW (default length 255)
  // WARNING: This datatype is actually not supported!
  if (l<=0) varLength=255;
}

TString HDbColRawType::getTypeString() {
  // Returns the type string, e.g. "RAW(255)"
  TString s=("RAW(");
  Char_t buf[10];
  sprintf(buf,"%i",varLength);
  s=s + buf + ")";
  return s;
}
