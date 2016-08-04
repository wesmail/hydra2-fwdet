//*-- AUTHOR : Ilse Koenig
//*-- Modified : 11/09/2000

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HDbColumn
//
// Class for a column in an Oracle table
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hdbcolumn.h"
#include "hdbcolumntype.h"
#include <stdio.h>
#include <iostream> 
#include <iomanip> 

ClassImp(HDbColumn)

HDbColumn::HDbColumn() {
  // Default constructor
  colType=0;
  colId=0;
  colNullable=kTRUE;
}

HDbColumn::HDbColumn(const Char_t* cName) {
  // Constructor creates a column with name cName and default type varchar2(80)
  // By default the column is nullable
  TString c(cName);
  c=c.Strip(c.kBoth);
  c.ToUpper();
  SetName(c.Data());
  colType=new HDbColVcType(80);
  colId=0;
  colNullable=kTRUE;
}

HDbColumn::HDbColumn(HDbColumn& c) {
  // Copy constructor
  SetName(c.GetName());
  colType=0;
  copyColumn(&c);
}  

HDbColumn::~HDbColumn() {
  // Destructor
  if (colType) delete colType;
}

void HDbColumn::setColType(const Char_t* tName, Int_t dlen, Int_t prec, Int_t scale) {
  // Sets the column type
  // Parameters are:  tName = Name of the type, dlen = length in bytes,
  //                  prec = data precision, scale = data scale
  TString t(tName);
  t=t.Strip(t.kBoth);
  if (t.Length()==0) return;
  if (colType) {
    delete colType;
    colType=0;
  }
  t.ToUpper();
  Char_t ta[7][10]={"VARCHAR","CHAR","NUMBER","DATE","LONG","ROWID","RAW"};
  Int_t i=0;
  for(;i<7;i++) {
    if (t.Contains(ta[i])) break;
  }
  switch (i) {
    case 0:   colType=new HDbColVcType(dlen);        break;
    case 1:   colType=new HDbColCharType(dlen);      break;
    case 2:   if (prec==0) prec=dlen;
              colType=new HDbColNumType(prec,scale); break;
    case 3:   colType=new HDbColDateType(dlen);      break;
    case 4:   colType=new HDbColLongType(dlen);      break;
    case 5:   colType=new HDbColRowidType();         break;
    case 6:   colType=new HDbColRawType(dlen);       break;
    default:  colType=new HDbColumnType(t,dlen);     break;    
  }
}

void HDbColumn::setColType(const Char_t* c) {
  // Sets the column type (e.g  varchar3(20) or number(4,2))
  TString cType(c);
  Int_t n1=cType.First("(");
  if (n1<0) return setColType(cType.Data(),0);
  TString ctn=cType(0,(n1));
  Int_t n2=cType.First(")");
  TString ss=cType((n1+1),(n2-n1-1));
  n1=ss.First(",");
  if (n1<0) {
    sscanf(ss.Data(),"%i",&n2);
    return setColType(ctn,n2);
  }
  TString ss1=ss(0,n1);
  ss=ss((n1+1),(ss.Length()-n1-1));
  sscanf(ss1.Data(),"%i",&n1);
  sscanf(ss.Data(),"%i",&n2);
  return setColType(ctn,0,n1,n2); 
}

void HDbColumn::copyColumn(HDbColumn* p) {
  // Copies the parameters of a column
  if (!p) return;
  colId=p->getColumnId();
  colNullable=p->isNullable();
  HDbColumnType* t=p->getColType();
  setColType(t->GetName(),t->varLength,t->dataPrecision,t->dataScale);
}

void HDbColumn::show() {
  // Shows a column
  Char_t c='Y';
  if (colNullable) c='N';
  cout<<setiosflags(ios::left)<<setw(30)<<GetName()<<c<<"   ";
  if (colType) cout<<setiosflags(ios::left)
                   <<setw(12)<<colType->GetName()
                   <<setw(6) <<colType->varLength
                   <<setw(12)<<colType->dataPrecision
                   <<setw(12)<<colType->dataScale<<endl;
  else cout<<endl;
}
