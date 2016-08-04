//*-- AUTHOR : Ilse Koenig
//*-- Created : 20/11/2002 by I.Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HCondParAsciiFileIo
//
// Interface class to ASCII file for input/output of parameters derived
// from HParCond
//
//////////////////////////////////////////////////////////////////////////////

#include "hcondparasciifileio.h"
#include "hparcond.h"
#include "hparamlist.h"
#include "TClass.h"

ClassImp(HCondParAsciiFileIo)

HCondParAsciiFileIo::HCondParAsciiFileIo(fstream* f) : HDetParAsciiFileIo(f) {
  // constructor
  // sets the name of the I/O class "HCondParIo"
  // gets the pointer to the ASCII file
  fName="HCondParIo";
}

Bool_t HCondParAsciiFileIo::init(HParSet* pPar,Int_t* set) {
  // readCond(HParCond*)
  if (!pFile) return kFALSE; 
  if (pPar->InheritsFrom("HParCond"))
    return readCond((HParCond*)pPar);
  Error("HCondParAsciiFileIo::init(HParSet*,Int_t*)",
        "%s does not inherit from HParCond",pPar->GetName());
  return kFALSE;
}

Int_t HCondParAsciiFileIo::write(HParSet* pPar) {
  // calls writeCond(HParCond*)
  if (!pFile) return -1; 
  if (pPar->InheritsFrom("HParCond"))
    return writeCond((HParCond*)pPar);
  Error("HCondParAsciiFileIo::write(HParSet*)",
        "%s does not inherit from HParCond",pPar->GetName());
  return -1;
}

Bool_t HCondParAsciiFileIo::readCond(HParCond* pPar) {
  // reads condition-stype parameter containers from ASCII file
  if (!pFile) return kFALSE;
  pFile->clear();
  pFile->seekg(0,ios::beg);
  Text_t* name=(Char_t*)pPar->GetName();
  if (!findContainer(name)) return kFALSE;
  HParamList* paramList = new HParamList;
  const Int_t maxbuf=8000;
  Text_t buf[maxbuf];
  buf[0]='\0';
  TString s, pName, pVal, pType;
  Ssiz_t n, m;
  while (buf[0]!='#' && !pFile->eof()) {             
    pFile->getline(buf,maxbuf);
    if (buf[0]!='/' && buf[0]!='#') {
      s=buf;
      n=s.First(':');
      if (n==-1) {
        Error("readCond(HParCond*)",
              "%s:\n  Missing backslash for parameter %s",name,pName.Data());
        delete paramList;
        return  kFALSE;
      } 
      pName=s(0,n);
      s=s(n+1,s.Length()-n-1);
      s=s.Strip(s.kLeading);
      if (pName.CompareTo("author")==0 || pName.CompareTo("description")==0) {
        m=s.Last('\\');
        if (m<=0) {
          pVal=s;
        } else {
          pVal=s(0,m);
          pVal+="\n";
          while (m>0) {
            pFile->getline(buf,maxbuf);
            if (buf[0]!='/') {
              s=buf;
              m=s.Last('\\');
              if (m>0) {
                pVal+=s(0,m);
                pVal+="\n";
              } else {
                pVal+=s;
              }
            }
          }
	}
        if (pName.CompareTo("author")==0) {
          pPar->setAuthor(pVal.Data());
        } else if (pName.CompareTo("description")==0) {
          pPar->setDescription(pVal.Data());
        }
      } else {
        n=s.First(' ');
        pType=s(0,n);
        s=s(n+1,s.Length()-n-1);
        s=s.Strip(s.kLeading);
    	if (pType.CompareTo("Text_t")==0) {
          m=s.Last('\\');
          if (m<0) {
            pVal=s;
          } else {
            pVal="";
            while (m>=0) {
              pFile->getline(buf,maxbuf);
              if (buf[0]!='/') {
                s=buf;
                m=s.Last('\\');
                if (m>0) {
                  pVal+=s(0,m);
                  pVal+="\n";
                } else {
                  pVal+=s;
                }
              }
            }
          }
          pVal=pVal.Strip(pVal.kLeading);
          if (pVal.Length()>0) {
            paramList->add(pName.Data(),pVal.Data());
          }
        } else {
          UChar_t* val=0;
          Int_t length=0;
          if (pType.CompareTo("Int_t")==0) {
            Int_t v=0;
            val=readData(v,"%i",s,length);
          } else if (pType.CompareTo("UInt_t")==0) {
            UInt_t v=0;
            val=readData(v,"0x%x",s,length);
          } else if  (pType.CompareTo("Float_t")==0){
            Float_t v=0.F;
            val=readData(v,"%f",s,length);
          } else if (pType.CompareTo("Double_t")==0) {
            Double_t v=0.;
            val=readData(v,"%lf",s,length);
          } else if (pType.CompareTo("Char_t")==0) {
            Char_t v='0';
            val=readData(v,"%c",s,length);
          } else {
            Error("readCond(HParCond*)",
              "%s:\n  Parameter %s with unsupported type %s",
              name,pName.Data(),pType.Data());
            delete paramList;
            return kFALSE;
          }
          HParamObj* obj=new HParamObj(pName.Data());
          obj->setParamType(pType.Data());
          UChar_t* pValue=obj->setLength(length);
          memcpy(pValue,val,length);
          paramList->getList()->Add(obj);          
          if (val) delete [] val;
	}
      }
    }
  }
  Bool_t allFound=pPar->getParams(paramList);
  if (allFound) { 
    pPar->setInputVersion(1,inputNumber);
    pPar->setChanged();
    printf("%s initialized from Ascii file\n",name);
  } else pPar->setInputVersion(-1,inputNumber);
  delete paramList;
  return allFound;
}

template <class type> UChar_t* HCondParAsciiFileIo::readData(
              type t,const Char_t* format,TString& line, Int_t& length) {
  // reads c-type single data and arrays
  const Int_t st=sizeof(t);
  const Int_t maxbuf=8000;
  const Int_t bufSizeExt=10000*st;
  const Char_t d[]=" ";
  Text_t buf[maxbuf];
  TString s;
  Int_t l=0, bufSize=bufSizeExt;
  UChar_t* val=new UChar_t[bufSize];
  Ssiz_t m=line.Last('\\');
  if (m<0) {
    sscanf(line.Data(),format,&t);
    memcpy(&val[l],&t,st);
    length=st;
  } else {
    do {             
      pFile->getline(buf,maxbuf);
      if (buf[0]!='/' && buf[0]!='#') {
        TString s=buf;
        m=s.Last('\\');
        if (m>0) s=s(0,s.Length()-2);
        if ((bufSize-1000)<l) {
          bufSize+=bufSizeExt;
          UChar_t* va=new UChar_t[bufSize];
          memcpy(va,val,l);
          delete [] val;
          val=va;
        }
        Char_t* ss=strtok((Char_t*)s.Data(),d);
        while (ss!=0) {
          sscanf(ss,format,&t);
          memcpy(&val[l],&t,st);
          l+=st;
          ss=strtok(NULL,d);
        }
      }
    } while (buf[0]!='#' && !pFile->eof() && m>0);
    length=l;
  }
  return val;
}

Int_t HCondParAsciiFileIo::writeCond(HParCond* pPar) {
  // writes condition-stype parameter containers to ASCII file
  if (pFile) {
    const Text_t* name=pPar->GetName();
    const Text_t* context=pPar->getParamContext();
    *pFile<<sepLine;
    *pFile<<"# Class:   "<<pPar->IsA()->GetName()<<"\n# Context: "<<context<<"\n";
    *pFile<<sepLine;
    *pFile<<"["<<name<<"]\n";
    writeComment(pPar);
    *pFile<<"//-----------------------------------------------------------"
            "-----------------\n";
    HParamList* paramList = new HParamList;
    pPar->putParams(paramList);
    TList* pList=paramList->getList();
    TIter next(pList);
    HParamObj* po;
    while ((po=(HParamObj*)next())) {
      const Char_t* pType=po->getParamType();
      UChar_t* pValue=po->getParamValue();
      if (po->isBasicType()&&strcmp(pType,"UChar_t")!=0) {
        if (strcmp(pType,"Text_t")==0) {
          TString val((Char_t*)pValue,po->getLength());
          val.ReplaceAll("\n"," \\\n");
          *pFile<<po->GetName()<<":  "<<pType<<" \\\n  "<<val.Data()<<endl;
        } else {
          Int_t nParams=po->getNumParams();
          if (nParams==1) {
            *pFile<<po->GetName()<<":  "<<pType<<"  ";
          } else {
            *pFile<<po->GetName()<<":  "<<pType<<" \\\n  ";
          }
          if (strcmp(pType,"Char_t")==0) {
            writeData((Char_t*)pValue,nParams);
          } else if (strcmp(pType,"Int_t")==0) {
            writeData((Int_t*)pValue,nParams);
          } else if (strcmp(pType,"UInt_t")==0) {
            writeHexData((UInt_t*)pValue,nParams);
          } else if (strcmp(pType,"Float_t")==0) {
            writeData((Float_t*)pValue,nParams);
          } else if (strcmp(pType,"Double_t")==0) {
            writeData((Double_t*)pValue,nParams);
          }
        }
      } else {
        Error("writeCond(HParCond*)",
              "Type %s of parameter %s not supported by ASCII I/O",
              pType,po->GetName());
      }
    }
    *pFile<<sepLine;
    delete paramList;
    pPar->setChanged(kFALSE);
    return 1;
  }
  Error("writeCond(HParCond*)",
              "Output is not writable");
  return -1;
}  

template <class type> void HCondParAsciiFileIo::writeData(type* val, Int_t nParams) {
  // writes c-type arrays
  Int_t i=0, k=0;
  while (k<nParams) {
    if (i==10) {
      *pFile<<" \\\n  "; 
      i=0;
    }
    *pFile<<val[k]<<" ";
    i++;
    k++;
  }
  *pFile<<endl;
}

void HCondParAsciiFileIo::writeHexData(UInt_t* val, Int_t nParams) {
  // writes UInt_t data in HEX format with leading zeros, for example 0x0000ffff
  Text_t buf[11];
  Int_t i=0, k=0;
  while (k<nParams) {
    if (i==10) {
      *pFile<<" \\\n  "; 
      i=0;
    }
    sprintf(buf,"0x%08x",val[k]);
    *pFile<<buf<<" ";
    i++;
    k++;
  }
  *pFile<<endl;
}
