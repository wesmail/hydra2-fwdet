//*-- AUTHOR : Ilse Koenig
//*-- Last modified : 03/06/2009 by Ilse Koenig

#include "hparamlist.h"
#include "TClass.h"
#include "TStreamerInfo.h"
#include "RVersion.h"
#include "TBuffer.h"

#if ROOT_VERSION_CODE  > ROOT_VERSION(4,4,2)
   #include "TBufferFile.h"
#endif

#include <iostream>
#include <iomanip>

using namespace std;

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////////////////
//
//  HParamObj
//
//  Class for parameters stored in binary format in Oracle and used as list
//  elements in HParamList::paramList.
//
//  The overloaded constructors and fill-functions accept single values or arrays of type
//    UChar_t, Int_t, UInt_t, Float_t, Double_t, char_t, Text_t.
//  The arguments for arrays are
//      the name of the parameter
//      the pointer to the array
//      the length of the array
//  The data are automatically converted to an UChar_t array. 
//  For classes also the class version is stored, for ROOT classes also the TStreamerInfo.
//
//  The input and output data format of UInt_t variables or arrays is HEX format
//    with leadings zeros, for example 0x0000ffff.
//
//  -------------------------------------------------------------------------------------
//
//  HParamList
//
//  Class for the generic Oracle and ASCII interface for parameter containers
//  derived from HParCond
//
//  The class contains a list to stores objects of type HParamObj
//  The list objects store the name, the value, the parameter type and some
//  additional information depending for the object type.  
//    
//  All add/addObject functions add an initialized parameter to the list for
//  writing. The functions create a new list element and copy the data into
//  this object.
//  Add functions:
//    1. accepted basic types: Int_t, UInt_t, Float_t, Double_t, UChar_t
//    2. accepted ROOT arrays: TArrayI, TArrayF, TArrayD, TArrayC
//    3. accepted string: Text_t*
//       This can be any text, for example also numbers in hexadecimal or
//       scientific format. The number of characters must be specified (default 1). 
//  AddObject function:
//    Accepts classes derived from TObject.
//    The persistent data elements are streamed into an UChar_t array using the
//    class streamer. For ROOT classes, for example histograms, the ROOT streamer
//    info is stored in an additional binary array. 
//       
//  All fill/fillObject functions convert the data in the list element back
//  to the type of the parameter and copy them into the data element in the
//  initialization process.
//    1. Single parameters of basic type:
//       The functions return kFALSE, if the parameter is not in the list.
//    2. Arrays:
//      a) If the array size is specified (return code Bool_t), the functions return
//         kFALSE, if the number of data elements in the list objects is not the same.
//      b) If the array size is not specified (return code Int_t), the array is
//         recreated with the size of the number of data elements in the list object.
//         The functions return the number of data elements or 0, if the parameter
//         was not found.
//    3. Classes:  
//       The class version is checked and a warning printed, if it is not identical
//       with the current version (typically class version of list object higher than
//       version in the actual parameter container). The class streamer takes care
//       for backward compatibility. A warning is printed, if the ROOT version is
//       different from the current version.
//       The function returns the number of bytes in the list object or 0, if the
//       parameter was not found in the list. 
//
//////////////////////////////////////////////////////////////////////////////////////


ClassImp(HParamObj)
ClassImp(HParamList)
 
HParamObj::HParamObj(const Text_t* name) {
  // Default constructor with parameter type "UChar_t"
  SetName(name);
  paramValue=0;
  arraySize=0;
  paramType="UChar_t";
  basicType=kFALSE;
  bytesPerValue=1;
  classVersion=-1;
  streamerInfo=0;
  streamerInfoSize=0;
}

HParamObj::HParamObj(HParamObj& o):TNamed(o) {
  // Copy constructor
  SetName(o.GetName());
  arraySize=o.getLength();
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,o.getParamValue(),arraySize);
  paramType=o.getParamType();
  if (o.isBasicType()) basicType=kTRUE;
  else basicType=kFALSE;
  bytesPerValue=o.getBytesPerValue();
  classVersion=o.getClassVersion();
  streamerInfoSize=o.getStreamerInfoSize();
  if (streamerInfoSize>0) {
    memcpy(streamerInfo,o.getStreamerInfo(),streamerInfoSize);
  }
}

HParamObj::HParamObj(const Text_t* name,Int_t value) {
  // Constructor for an Int_t value
  SetName(name);
  setParamType("Int_t");
  arraySize=bytesPerValue;
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,&value,arraySize);
}

HParamObj::HParamObj(const Text_t* name,UInt_t value) {
  // Constructor for an UInt_t value
  SetName(name);
  setParamType("UInt_t");
  arraySize=bytesPerValue;
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,&value,arraySize);
}

HParamObj::HParamObj(const Text_t* name,Float_t value) {
  // Constructor for a Float_t value
  SetName(name);
  setParamType("Float_t");
  arraySize=bytesPerValue;
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,&value,arraySize);
}

HParamObj::HParamObj(const Text_t* name,Double_t value) {
  // Constructor for a Double_t value
  SetName(name);
  setParamType("Double_t");
  arraySize=bytesPerValue;
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,&value,arraySize);
}

HParamObj::HParamObj(const Text_t* name,const Int_t* value,const Int_t nValues) {
  // Constructor for an array with nValues elements of type Int_t
  SetName(name);
  setParamType("Int_t");
  arraySize=bytesPerValue*nValues;
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,value,arraySize);
}

HParamObj::HParamObj(const Text_t* name,const UInt_t* value,const Int_t nValues) {
  // Constructor for an array with nValues elements of type UInt_t
  SetName(name);
  setParamType("UInt_t");
  arraySize=bytesPerValue*nValues;
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,value,arraySize);
}

HParamObj::HParamObj(const Text_t* name,const Float_t* value,const Int_t nValues) {
  // Constructor for an array with nValues elements of type Float_t
  SetName(name);
  setParamType("Float_t");
  arraySize=bytesPerValue*nValues;
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,value,arraySize);
}

HParamObj::HParamObj(const Text_t* name,const Double_t* value,const Int_t nValues) {
  // Constructor for an array with nValues elements of type Double_t
  SetName(name);
  setParamType("Double_t");
  arraySize=bytesPerValue*nValues;
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,value,arraySize);
}

HParamObj::HParamObj(const Text_t* name,const Text_t* value) {
  // Constructor for a string value
  SetName(name);
  setParamType("Text_t");
  arraySize=strlen(value);
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,value,arraySize);
}

HParamObj::HParamObj(const Text_t* name,const Char_t* value,const Int_t nValues) {
  // Constructor for an array with nValues elements of type Char_t
  SetName(name);
  setParamType("Char_t");
  arraySize=bytesPerValue*nValues;
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,value,arraySize);
}

HParamObj::HParamObj(const Text_t* name,const UChar_t* value,const Int_t nValues) {
  // Constructor for an array with nValues elements of type UChar_t
  SetName(name);
  setParamType("UChar_t");
  arraySize=bytesPerValue*nValues;
  paramValue=new UChar_t[arraySize];
  memcpy(paramValue,value,arraySize);
}

HParamObj::~HParamObj() {
  // Destructor
  if (paramValue) {
    delete [] paramValue;
    paramValue=0;
  }
  if (streamerInfo) {
    delete [] streamerInfo;
    streamerInfo=0;
  }
}

void HParamObj::setParamType(const Text_t* t) {
  // Sets the parameter type. Accepted type names are:
  //     UChar_t (default)
  //     Char_t
  //     Int_t
  //     Float_t
  //     Double_t
  //     Text_t
  //     class name
  paramType=t;
  if (strcmp(t,"Char_t")==0) {
    basicType=kTRUE;
    bytesPerValue=sizeof(Char_t);
  } else if (strcmp(t,"Int_t")==0) {
    basicType=kTRUE;
    bytesPerValue=sizeof(Int_t);
  } else if (strcmp(t,"UInt_t")==0) {
    basicType=kTRUE;
    bytesPerValue=sizeof(Int_t);
  } else if (strcmp(t,"Float_t")==0) {
    basicType=kTRUE;
    bytesPerValue=sizeof(Float_t);
  } else if (strcmp(t,"Double_t")==0) {
    basicType=kTRUE;
    bytesPerValue=sizeof(Double_t);
  } else if (strcmp(t,"Text_t")==0) {
    basicType=kTRUE;
    bytesPerValue=sizeof(Char_t);
  } else if (strcmp(t,"UChar_t")==0) {
    basicType=kTRUE;
    bytesPerValue=sizeof(UChar_t);
  } else {
    basicType=kFALSE;
    bytesPerValue=1;
  }
  if (basicType==kTRUE) {
    classVersion=-1;
    streamerInfoSize=0;
    streamerInfo=0;
  }
}

UChar_t* HParamObj::setLength(Int_t l) {
  // Sets the length of the binary array
  if (paramValue) delete [] paramValue;
  arraySize=l;
  if (l>0) {
    paramValue=new UChar_t[arraySize];
  } else {
    paramValue=0;
  }
  return paramValue;
}

void HParamObj::setParamValue(UChar_t* value,const Int_t length) {
  // Sets the parameter value (the array is not copied!)
  if (paramValue) delete [] paramValue;
  arraySize=length;
  paramValue=value;
}

UChar_t* HParamObj::setStreamerInfoSize(Int_t l) {
  // Sets the length of the streamer info
  if (streamerInfo) delete [] streamerInfo;
  streamerInfoSize=l;
  if (l>0) {
    streamerInfo=new UChar_t[streamerInfoSize];
  } else {
    streamerInfo=0;
  }
  return streamerInfo;
}

void HParamObj::setStreamerInfo(UChar_t* array,const Int_t length) {
  // Sets the streamer info of ROOT classes (the array is not copied!)
  if (streamerInfo) delete [] streamerInfo;
  streamerInfoSize=length;
  streamerInfo=array;
}

Int_t HParamObj::getNumParams() {
  // Returns the number of values in the array, respectively 1 for classes and strings
  Int_t n=1;
  if (basicType) {
    n=arraySize/bytesPerValue;
  }
  return n;
}

void HParamObj::print() {
  // Prints the name and type of the parameters, respectively class name and version.
  // Prints also the numbers for an array of type Int_t, Float_t, Double_t.
  cout<<GetName()<<": ";
  if (classVersion>=0) {
    cout<<"\n  Class Type:    "<<paramType.Data()<<"\n  Class Version: "
             <<classVersion<<endl;
  } else if (strcmp(paramType,"Text_t")==0) {
    TString val((Char_t*)paramValue,arraySize);
    val.ReplaceAll("\n","\n  ");
    cout<<paramType<<"\n  "<<val.Data()<<endl;
  } else {
    Int_t nParams=getNumParams();
    if (nParams==1) {
      cout<<paramType<<"   ";    
    } else {
      cout<<paramType<<" array, nValues: "<<nParams<<"\n  ";
    }  
    if (strcmp(paramType,"Char_t")==0) {
      Char_t* val=(Char_t*)paramValue;
      printData(val,nParams);
    } else if (strcmp(paramType,"Int_t")==0) {
      Int_t* val=(Int_t*)paramValue;
      printData(val,nParams);
    } else if (strcmp(paramType,"UInt_t")==0) {
      UInt_t* val=(UInt_t*)paramValue;
      printHexData(val,nParams);
    } else if (strcmp(paramType,"Float_t")==0) {
      Float_t* val=(Float_t*)paramValue;
      printData(val,nParams);
    } else if (strcmp(paramType,"Double_t")==0) {
      Double_t* val=(Double_t*)paramValue;
      printData(val,nParams);
    } else {
      cout<<"Type: "<<paramType<<"  Array  length: "<<arraySize<<endl;
    }
  }
}

template <class type> void HParamObj::printData(type* val, Int_t nParams) {
  // template function to print data of type Char_t, Int_t, Float_t, Double_t
  Int_t i=0, k=0;
  while (k<nParams) {
    if (i==10) {
      cout<<"\n  "; 
      i=0;
    }
    cout<<val[k]<<" ";
    i++;
    k++;
    if (k>50) {
      cout<<"...";
      break;
    }
  }
  cout<<endl;
}

void HParamObj::printHexData(UInt_t* val, Int_t nParams) {
  // function to print data of type UInt_t in HEX format with leading zeros
  Int_t i=0, k=0;
  while (k<nParams) {
    if (i==10) {
      printf("\n  "); 
      i=0;
    }
    printf("0x%08x ",val[k]);
    i++;
    k++;
    if (k>50) {
      printf("...");
      break;
    }
  }
  printf("\n"); 
}

//-----------------------------------------------------------------------------------

HParamList::HParamList() {
  // Constructor
  paramList=new TList;
}

HParamList::~HParamList() {
  // Destructor
  if (paramList) {
    paramList->Delete();
    delete paramList;
    paramList=0;
  }
}

void HParamList::add(HParamObj& p) {
  // Adds a HParamObj object to the list
  paramList->Add(new HParamObj(p));
}

void HParamList::add(const Text_t* name,const Text_t* value) {
  // Adds a string parameter to the list
  // name  = name of the parameter
  // value = string value
  paramList->Add(new HParamObj(name,value));
}

void HParamList::add(const Text_t* name,const Int_t value) {
  // Adds a parameter of type Int_t to the list
  paramList->Add(new HParamObj(name,value));
}

void HParamList::add(const Text_t* name,const UInt_t value) {
  // Adds a parameter of type UInt_t to the list
  paramList->Add(new HParamObj(name,value));
}

void HParamList::add(const Text_t* name,const Float_t value) {
  // Adds a parameter of type Float_t to the list
  paramList->Add(new HParamObj(name,value));
}

void HParamList::add(const Text_t* name,const Double_t value) {
  // Adds a parameter of type Double_t to the list
  paramList->Add(new HParamObj(name,value));
}

void HParamList::add(const Text_t* name,TArrayI& value) {
  // Adds a parameter of type TArrayI to the list
  paramList->Add(new HParamObj(name,value.GetArray(),value.GetSize()));
}

void HParamList::add(const Text_t* name,TArrayC& value) {
  // Adds a parameter of type TArrayC  to the list
  paramList->Add(new HParamObj(name,value.GetArray(),value.GetSize()));
}

void HParamList::add(const Text_t* name,TArrayF& value) {
  // Adds a parameter of type TArrayF to the list
  paramList->Add(new HParamObj(name,value.GetArray(),value.GetSize()));
}

void HParamList::add(const Text_t* name,TArrayD& value) {
  // Adds a parameter of type TArrayD to the list
  paramList->Add(new HParamObj(name,value.GetArray(),value.GetSize()));
}

void HParamList::add(const Text_t* name,const UChar_t* values,const Int_t nValues) {
  // Adds a binary array of size nValues to the list
  paramList->Add(new HParamObj(name,values,nValues));
}

void HParamList::add(const Text_t* name,const Int_t* values,const Int_t nValues) {
  // Adds an array of type Int_t and of size nValues as binary to the list
  paramList->Add(new HParamObj(name,values,nValues));
}

void HParamList::add(const Text_t* name,const UInt_t* values,const Int_t nValues) {
  // Adds an array of type UInt_t and of size nValues as binary to the list
  paramList->Add(new HParamObj(name,values,nValues));
}

void HParamList::add(const Text_t* name,const Float_t* values,const Int_t nValues) {
  // Adds an array of type Float_t and of size nValues as binary to the list
  paramList->Add(new HParamObj(name,values,nValues));
}

void HParamList::add(const Text_t* name,const Double_t* values,const Int_t nValues) {
  // Adds an array of type Double_t and of size nValues as binary to the list
  paramList->Add(new HParamObj(name,values,nValues));
}

void HParamList::addObject(const Text_t* name,TObject* obj) {
  // Adds a TObject to the list, sets the class version and the streamer info for
  // ROOT classes
  if (!obj) return;
  HParamObj* o=new HParamObj(name);
  o->setParamType(obj->IsA()->GetName());
  o->setClassVersion(obj->IsA()->GetClassVersion());
  TFile* filesave=gFile;
  HParamTFile* paramFile=new HParamTFile();
  gFile=paramFile;
  const Int_t bufsize=10000;

#if ROOT_VERSION_CODE  > ROOT_VERSION(4,4,2)
  TBufferFile* buffer=new TBufferFile(TBuffer::kWrite,bufsize);
#else
  TBuffer* buffer=new TBuffer(TBuffer::kWrite,bufsize);
#endif

  buffer->SetParent(paramFile);
  buffer->MapObject(obj);
  obj->Streamer(*buffer);
  Int_t len=buffer->Length();
  Char_t* buf=new char[len];
  memcpy(buf,buffer->Buffer(),len);
  o->setParamValue((UChar_t*)buf,len);
  TArrayC* fClassIndex=paramFile->GetClassIndex();
  if (fClassIndex&&fClassIndex->fArray[0] != 0) {
    TIter next(gROOT->GetListOfStreamerInfo());
    TStreamerInfo *info;
    TList list;
    while ((info=(TStreamerInfo*)next())) {
      Int_t uid=info->GetNumber();
      if (fClassIndex->fArray[uid]) list.Add(info);
    }
    if (list.GetSize()>0) {
      list.Sort();
      fClassIndex->fArray[0]=2; //to prevent adding classes in TStreamerInfo::TagFile

#if ROOT_VERSION_CODE  > ROOT_VERSION(4,4,2)
  TBufferFile* infoBuffer=new TBufferFile(TBuffer::kWrite,bufsize);
#else
  TBuffer* infoBuffer=new TBuffer(TBuffer::kWrite,bufsize);
#endif

      infoBuffer->MapObject(&list);
      list.Streamer(*infoBuffer);
      Int_t infolen=infoBuffer->Length();
      Char_t* infobuf=new char[infolen];
      memcpy(infobuf,infoBuffer->Buffer(),infolen);
      o->setStreamerInfo((UChar_t*)infobuf,infolen);
      delete infoBuffer;
    } else {
      o->setStreamerInfo(0,0);
    }
  }
  fClassIndex->fArray[0]=0;
  delete paramFile;
  paramList->Add(o);
  delete buffer;
  gFile=filesave;
}

void HParamList::print() {
  // Prints the parameter list including values
  TIter next(paramList);
  HParamObj* o;
  while ((o=(HParamObj*)next())!=0) o->print();
}

Bool_t HParamList::fill(const Text_t* name,Text_t* value,const Int_t length) {
  // Copies the data from the list object into the parameter value of type string
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (value==0) return kFALSE;
  if (o!=0 && strcmp(o->getParamType(),"Text_t")==0) {
    Int_t l=o->getLength();
    if (l<length-1) {
      memcpy(value,(Char_t*)o->getParamValue(),l);
      value[l]='\0';
      return kTRUE;
    } else {
      Error("HParamList::fill(const Text_t*,Text_t*)","char array too small");
    }
  }
  Error("HParamList::fill","Not found: %s",name);
  return kFALSE;
}

Bool_t HParamList::fill(const Text_t* name,UChar_t* values,const Int_t nValues) {
  // Copies the data from the list object into the parameter array of type UChar_t of size nValues.
  // The function returns an error, if the array size of the list object is not equal
  // to nValues. 
  if (values==0) return kFALSE;
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"UChar_t")==0) {
    Int_t n=o->getLength();  
    if (n==nValues) {
      memcpy(values,o->getParamValue(),n);
      return kTRUE;
    } else {
      Error("HParamList::fill", "\nDifferent array sizes for parameter %s",name);
      return kFALSE;
    }
  }
  Error("HParamList::fill","Not found: %s",name);
  return kFALSE;
}

Bool_t HParamList::fill(const Text_t* name,Int_t* values,const Int_t nValues) {
  // Copies the data from the list object into the parameter array of type Int_t.
  // The function returns an error, if the array size of the list object is not equal
  // to nValues. 
  if (values==0) return kFALSE;
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"Int_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (n==nValues) {
      memcpy(values,o->getParamValue(),l);
      return kTRUE;
    } else {
      Error("HParamList::fill","\nDifferent array sizes for parameter %s",name);
      return kFALSE;
    }
  }
  Error("HParamList::fill","Not found: %s",name);
  return kFALSE;
}

Bool_t HParamList::fill(const Text_t* name,UInt_t* values,const Int_t nValues) {
  // Copies the data from the list object into the parameter array of type Int_t.
  // The function returns an error, if the array size of the list object is not equal
  // to nValues. 
  if (values==0) return kFALSE;
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"UInt_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (n==nValues) {
      memcpy(values,o->getParamValue(),l);
      return kTRUE;
    } else {
      Error("HParamList::fill","\nDifferent array sizes for parameter %s",name);
      return kFALSE;
    }
  }
  Error("HParamList::fill","Not found: %s",name);
  return kFALSE;
}

Bool_t HParamList::fill(const Text_t* name,Float_t* values,const Int_t nValues) {
  // Copies the data from the list object into the parameter array of type Float_t.
  // The function returns an error, if the array size of the list object is not equal
  // to nValues. 
  if (values==0) return kFALSE;
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"Float_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (n==nValues) {
      memcpy(values,o->getParamValue(),l);
      return kTRUE;
    } else {
      Error("HParamList::fill","\nDifferent array sizes for parameter %s",name);
      return kFALSE;
    }
  }
  Error("HParamList::fill","Not found: %s",name);
  return kFALSE;
}

Bool_t HParamList::fill(const Text_t* name,Double_t* values,const Int_t nValues) {
  // Copies the data from the list object into the parameter array of type Double_t.
  // The function returns an error, if the array size of the list object is not equal
  // to nValues. 
  if (values==0) return kFALSE;
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"Double_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (n==nValues) {
      memcpy(values,o->getParamValue(),l);
      return kTRUE;
    } else {
      Error("HParamList::fill","\nDifferent array sizes for parameter %s",name);
      return kFALSE;
    }
  }
  Error("HParamList::fill","Not found: %s",name);
  return kFALSE;
}

Bool_t HParamList::fill(const Text_t* name,TArrayI* value) {
  // Copies the data from the list object into the parameter value of type TArrayI
  // The array is resized, if the number of data is different.
  if (value==0) return kFALSE;
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"Int_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (value->GetSize()!=n) value->Set(n);
    memcpy(value->GetArray(),o->getParamValue(),l);
    return kTRUE;
  }
  Error("HParamList::fill","Not found: %s",name);
  return kFALSE;
}

Bool_t HParamList::fill(const Text_t* name,TArrayC* value) {
  // Copies the data from the list object into the parameter value of type TArrayC
  // The array is resized, if the number of data is different.
  if (value==0) return kFALSE;
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"Char_t")==0) {
    Int_t l=o->getLength();
    if (value->GetSize()!=l) value->Set(l);
    memcpy(value->GetArray(),o->getParamValue(),l);
    return kTRUE;
  }
  Error("HParamList::fill","Not found: %s",name);
  return kFALSE;
}

Bool_t HParamList::fill(const Text_t* name,TArrayF* value) {
  // Copies the data from the list object into the parameter value of type TArrayF
  // The array is resized, if the number of data is different.
  if (value==0) return kFALSE;
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"Float_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (value->GetSize()!=n) value->Set(n);
    memcpy(value->GetArray(),o->getParamValue(),l);
    return kTRUE;
  }
  Error("HParamList::fill","Not found: %s",name);
  return kFALSE;
}

Bool_t HParamList::fill(const Text_t* name,TArrayD* value) {
  // Copies the data from the list object into the parameter value of type TArrayD
  // The array is resized, if the number of data is different.
  if (value==0) return kFALSE;
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"Double_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (value->GetSize()!=n) value->Set(n);
    memcpy(value->GetArray(),o->getParamValue(),l);
    return kTRUE;
  }
  Error("HParamList::fill","Not found: %s",name);
  return kFALSE;
}

Int_t HParamList::replace(const Text_t* name,UChar_t* values) {
  // Copies the data from the list object into the parameter array of type UChar_t.
  // Recreates the array, if existing, and returns the number of array elements.
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"UChar_t")==0) {
    Int_t l=o->getLength();
    if (values) delete values;
    values=new UChar_t[l];
    memcpy(values,o->getParamValue(),l);
    return l;
  }
  Error("HParamList::fill","Not found: %s",name);
  return 0;
}

Int_t HParamList::replace(const Text_t* name,Int_t* values) {
  // Copies the data from the list object into the parameter array of type Int_t.
  // Recreates the array, if existing, and returns the number of array elements.
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"Int_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (values) delete values;
    values=new Int_t[n];
    memcpy(values,o->getParamValue(),l);
    return n;
  }
  Error("HParamList::fill","Not found: %s",name);
  return 0;
}

Int_t HParamList::replace(const Text_t* name,UInt_t* values) {
  // Copies the data from the list object into the parameter array of type UInt_t.
  // Recreates the array, if existing, and returns the number of array elements.
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"UInt_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (values) delete values;
    values=new UInt_t[n];
    memcpy(values,o->getParamValue(),l);
    return n;
  }
  Error("HParamList::fill","Not found: %s",name);
  return 0;
}

Int_t HParamList::replace(const Text_t* name,Float_t* values) {
  // Copies the data from the list object into the parameter array of type Float_t.
  // Recreates the array, if existing, and returns the number of array elements.
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"Float_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (values) delete values;
    values=new Float_t[n];
    memcpy(values,o->getParamValue(),l);
    return n;
  }
  Error("HParamList::fill","Not found: %s",name);
  return 0;
}

Int_t HParamList::replace(const Text_t* name,Double_t* values) {
  // Copies the data from the list object into the parameter array of type Double_t.
  // Recreates the array, if existing, and returns the number of array elements.
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),"Double_t")==0) {
    Int_t l=o->getLength();
    Int_t n=o->getNumParams();  
    if (values) delete values;
    values=new Double_t[n];
    memcpy(values,o->getParamValue(),l);
    return n;
  }
  Error("HParamList::fill","Not found: %s",name);
  return 0;
}

Bool_t HParamList::fillObject(const Text_t* name,TObject* obj) {
  // Fills the object obj (must exist!) via the Streamer and returns the class version.
  // Prints a warning if the class version in the list objects differs from the actual
  // class version.
  if (!obj) return 0;
  HParamObj* o=(HParamObj*)paramList->FindObject(name);
  if (o!=0 && strcmp(o->getParamType(),obj->IsA()->GetName())==0) {
    if (o->getClassVersion()!=obj->IsA()->GetClassVersion())
      Warning("HParamList::fillObject",
              "\n       Read Class Version = %i does not match actual version = %i",
              o->getClassVersion(),obj->IsA()->GetClassVersion());
    TFile* filesave=gFile;
    gFile=0;
#if ROOT_VERSION_CODE  > ROOT_VERSION(4,4,2)
    TBufferFile* buf=0;
#else
    TBuffer* buf=0;
#endif

    Int_t len=o->getStreamerInfoSize();
    if (len>0&&o->getStreamerInfo()!=0) {
#if ROOT_VERSION_CODE  > ROOT_VERSION(4,4,2)
      buf=new TBufferFile(TBuffer::kRead,len);
#else
      buf=new TBuffer(TBuffer::kRead,len);
#endif
      memcpy(buf->Buffer(),(Char_t*)o->getStreamerInfo(),len);
      buf->SetBufferOffset(0);
      TList list;
      buf->MapObject(&list);
      list.Streamer(*buf);
      delete buf;
      TStreamerInfo *info;
      TIter next(&list);
      while ((info = (TStreamerInfo*)next())) {
        if (info->IsA() != TStreamerInfo::Class()) {
	  Warning("HParamList::fillObject","not a TStreamerInfo object");
          continue;
        }
        info->BuildCheck();
      }
      list.Clear();  //this will delete all TStreamerInfo objects with kCanDelete 
    }
    len=o->getLength();
#if ROOT_VERSION_CODE  > ROOT_VERSION(4,4,2)
      buf=new TBufferFile(TBuffer::kRead,len);
#else
      buf=new TBuffer(TBuffer::kRead,len);
#endif
    memcpy(buf->Buffer(),(Char_t*)o->getParamValue(),len);
    buf->SetBufferOffset(0);
    buf->MapObject(obj);
    obj->Streamer(*buf);
    delete buf;
    gFile=filesave;
    return len;
  }
  Error("HParamList::fillObject","Not found: %s",name);
  return 0;
}
