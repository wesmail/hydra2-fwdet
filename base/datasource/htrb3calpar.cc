//*-- AUTHOR Ilse Koenig
//*-- created : 12/03/2014 by Ilse Koenig
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
//
// HTrb3Calpar
//
// Calibration parameters of TRB3 TDC
//
///////////////////////////////////////////////////////////////////////

using namespace std;
#include "htrb3calpar.h"
#include "htrb3tdcmessage.h"
#include "TBuffer.h"
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

ClassImp(HTrb3CalparTdc)
ClassImp(HTrb3Calpar)

HTrb3CalparTdc::HTrb3CalparTdc(void) {
  // constructor
  clear();
}

void HTrb3CalparTdc::clear(void) {
  // clears the data
  subEvtId        = 0;
  nEdgesMask      = 1;
  nChannels       = 0;
  nBinsPerChannel = 0;
  binsPar.Set(0);
}

Int_t HTrb3CalparTdc::makeArray(Int_t subevtid, Int_t edges, Int_t nChan, Int_t nBin) {
  // creates the array for nChan channels with nBin bins each
  subEvtId        = subevtid;
  nEdgesMask      = edges;
  nChannels       = nChan;
  nBinsPerChannel = nBin;
  Int_t nData = nChannels * nBinsPerChannel;
  if (nEdgesMask == 3) nData*=2;
  binsPar.Set(nData);
  binsPar.Reset();
  return nData;
}

Bool_t HTrb3CalparTdc::fillArray(Float_t* data,Int_t nData) {
  // copies the data into the arrays
  Int_t nd = binsPar.GetSize();
  if (nd == nData) {
    memcpy(binsPar.GetArray(),&data[0],nd*sizeof(Float_t));
    return kTRUE;
  } else {
    Error("fillArrays(Float_t*,Int_t)",
          "Invalid number of data (nData: %i, binsPar: %i)",nData,nd);
    return kFALSE;
  }
}

void HTrb3CalparTdc::print(void) {
  // prints the array
  cout<<"   subEvtId: "<<"0x"<<setw(4)<<left<<hex<<subEvtId<<dec<<"   nEdgesMask: "<<nEdgesMask<<"   nChannels: "<<nChannels<<"   nBinsPerChannel: "<<nBinsPerChannel<<'\n';
  cout<<"-------------------------------------------------------------------------------------"<<endl;
  Int_t l=0;
  for(Int_t i=0;i<binsPar.GetSize();i++) {
    l++;
    cout<<setw(13)<<right<<fixed<<setprecision(3)<<binsPar[i];
    if (l==10) {
      cout<<endl;
      l=0;
    }
  }
  if (l>0) cout<<endl;
  cout<<"-------------------------------------------------------------------------------------"<<endl;
}

void HTrb3CalparTdc::write(fstream& fout) {
  // writes the array to file
  fout<<"   subEvtId: "<<"0x"<<setw(4)<<left<<hex<<subEvtId<<dec<<"   nEdgesMask: "<<nEdgesMask<<"   nChannels: "<<nChannels<<"   nBinsPerChannel: "<<nBinsPerChannel<<endl;
  Int_t l=0;
  for(Int_t i=0;i<binsPar.GetSize();i++) {
    l++;
    fout<<setw(13)<<right<<fixed<<setprecision(3)<<binsPar[i];
    if (l==10) {
      fout<<endl;
      l=0;
    }
  }
  if (l>0) fout<<endl;
}

void HTrb3CalparTdc::setSimpleFineCalibration(UInt_t fineMinValue, UInt_t fineMaxValue) {
   // initialize calibrations with linear function
   // before this method is used, the fine limits must be set in HTrb3TdcMessage 
   HTrb3TdcMessage::setFineLimits(fineMinValue,fineMaxValue);
   Float_t* arr = new Float_t[nBinsPerChannel];
   for (Int_t n=0;n<nBinsPerChannel;n++) {
     arr[n] = HTrb3TdcMessage::simpleFineCalibr(n) * 1.e12F;
   }

   for (Int_t nch=0;nch<nChannels;nch++) {
      Float_t* risingArr = getRisingArr(nch);
      if (risingArr) memcpy(risingArr,arr,nBinsPerChannel*sizeof(Float_t));
      Float_t* fallingArr = getFallingArr((UInt_t)nch);
      if (fallingArr) memcpy(fallingArr,arr,nBinsPerChannel*sizeof(Float_t));
   }

   delete [] arr;
}

Bool_t HTrb3CalparTdc::loadFromBinaryFile(const char* filename, Int_t subevtid, Int_t numBins, Int_t nEdgesMask) {
  // reads the parameters from binary file
  // here the file name must be specified
   clear();
   FILE* f = fopen(filename,"r");
   if (f==0) {
      Error("loadFromBinaryFile", "Cannot open file %s for reading calibration", filename);
      return kFALSE;
   }

   ULong64_t numch(0);
   fread(&numch, sizeof(numch), 1, f);

   makeArray(subevtid, nEdgesMask, (Int_t)numch, numBins);

   Float_t* rising_calibr  = new Float_t[numBins];
   Float_t* falling_calibr = new Float_t[numBins];

   for (Int_t ch=0;ch<(Int_t)numch;ch++) {
      fread(rising_calibr, numBins * sizeof(Float_t), 1, f);
      fread(falling_calibr,numBins * sizeof(Float_t), 1, f);

      if (nEdgesMask & 1) {
         Float_t *tgt = getRisingArr(ch);
         if (tgt!=0) {
            for(Int_t bin=0; bin<numBins; bin++)
            tgt[bin] = rising_calibr[bin] * 1.e12F;
         }
      }
      if (nEdgesMask & 2) {
         Float_t *tgt = getFallingArr(ch);
         if (tgt!=0) {
            for(Int_t bin=0; bin<numBins; bin++)
            tgt[bin] = falling_calibr[bin] * 1.e12F;
         }
      }
   }
   delete [] rising_calibr;
   delete [] falling_calibr;
   fclose(f);

   Info("loadFromBinaryFile", "reading calibration from %s done", filename);
   return kTRUE;
}

// ================================================================================

HTrb3Calpar::HTrb3Calpar(const Char_t* name,
                     const Char_t* title,
                     const Char_t* context,
                     Int_t minTrbnetAddress,
		     Int_t maxTrbnetAddress)
           : HParSet(name,title,context) {
  // constructor creates an empty array
  arrayOffset=minTrbnetAddress;
  array = new TObjArray(maxTrbnetAddress-minTrbnetAddress+1);
}

HTrb3Calpar::~ HTrb3Calpar(void) {
  // destructor
  array->Delete();
  delete array;
}

void HTrb3Calpar::clear() {
  // deletes all HTrb2Correction objects from the array and resets the input versions
  array->Delete();
  status=kFALSE;
  resetInputVersions();
}

HTrb3CalparTdc* HTrb3Calpar::addTdc(Int_t address) {
  // adds a new HTrb3CalparTdc objects in the array at position (address-arrayOffset)
  HTrb3CalparTdc* p = getTdc(address);
  if (p != NULL) {
     Error("addTdc","TDC exists already for trbnet-address 0x%04x",address);
  } else {
    p = new HTrb3CalparTdc();
    array->AddAt(p,address-arrayOffset);
  }
  return p;
}

void HTrb3Calpar::printParam() {
  // prints the calibration parameters
  cout<<"Trb3 TDC calibration parameters\n";
  cout<<"-------------------------------------------------------------------------------------\n";
  for(Int_t i=0;i<=array->GetLast();i++) {
    HTrb3CalparTdc* tdc=(*this)[i];
    if (tdc) {
      cout<<"trbnet-address: "<<"0x"<<setw(4)<<left<<hex<<(arrayOffset+i);
      tdc->print();
    }
  }
}

void HTrb3Calpar::putAsciiHeader(TString& header) {
  // header in ASCII file
  header=
    "# TRB3 TDC calibration parameters\n"
    "# Format: trbnetAddress   subEvtId   mask for number of edges   number of channels   number of bins per channel \n"
    "#         rising calpar and/or  falling calpar \n";
}

void HTrb3Calpar::write(fstream& fout) {
  // writes the information of all non-zero HTrb3CalparTdc objects to the ASCII file
  for(Int_t i=0;i<=array->GetLast();i++) {
    HTrb3CalparTdc* tdc=(*this)[i];
    if (tdc) {
      fout<<"trbnet-address:  "<<"0x"<<setw(4)<<left<<hex<<(arrayOffset+i);
      tdc->write(fout);
    }
  }
}

Bool_t HTrb3Calpar::loadFromBinaryFiles(const char* basefname, Int_t subevtid, Int_t numBins, Int_t nEdgesMask) {
  // reads the parameters from binary file
  // here directory and base file name must be specified
   resetInputVersions();
   TString filename;
   Bool_t res = kTRUE;
   for(Int_t i=0;i<=array->GetLast();i++) {
      HTrb3CalparTdc* tdc=(*this)[i];
      if (tdc==0) continue;
      filename.Form("%s%04x.cal", basefname, (unsigned) (arrayOffset+i));
      if (!tdc->loadFromBinaryFile(filename.Data(),subevtid,numBins,nEdgesMask)) res = kFALSE;
   }
   if (res) {
     setChanged();
     setInputVersion(1, 1);
     setStatic();
   }
   return res;
}
