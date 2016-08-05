// Author: Witold Przygoda, przygoda@if.uj.edu.pl, last modifed on: 1 March 2009

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include "hntuple.h"


ClassImp(HNtuple)
// ---------------------------------------------------------------------------------
// Dummy constructor
// ---------------------------------------------------------------------------------
HNtuple::HNtuple() : outFile(0), cname(0), ctitle(0), cbufsize(0), ptrNt(0), isNtuple(kFALSE), varArrayN(0), varArray(0)
{
}

// ---------------------------------------------------------------------------------
// This is ntuple constructor with "lazy"-delayed construction
// ntuple is created only after first fill attempt based on 
// variables which have been set 
// ---------------------------------------------------------------------------------
HNtuple::HNtuple(const char* name, const char* title, Int_t bufsize) : 
	outFile(0), cname(name), ctitle(title), cbufsize(bufsize), ptrNt(0), isNtuple(kFALSE), varArrayN(0), varArray(0)
{
}

// ---------------------------------------------------------------------------------
// Basic ntuple constructor with exactly the same parameters as in the case of ROOT TNtuple
// ---------------------------------------------------------------------------------
HNtuple::HNtuple(const char* name, const char* title, const char* varlist, Int_t bufsize) :
	outFile(0), cname(name), ctitle(title), cbufsize(bufsize), ptrNt(0), isNtuple(kFALSE), varArrayN(0), varArray(0)
{
   ptrNt = new TNtuple(name, title, varlist, bufsize);
   setMap(varlist, isNtuple);
}

// ---------------------------------------------------------------------------------
void HNtuple::setMap(const char* vlist, Bool_t& kPair)
{
   Int_t i = 0;
   varList = vlist;
   std::string sName = "";
   std::string::iterator sIter;
   for (sIter = varList.begin(); sIter != varList.end(); ++sIter)
   {
       if (*sIter != ':')
       {
          sName += *sIter;
       }
       else
       {
          // create a new pair ntuple variable - value
          vKeyOrder.insert(make_pair(sName, i++));
          if (kPair==kFALSE) vKeyValue.insert(make_pair(sName, 0.));
          sName.clear();
       }
   }
          // create the last new pair ntuple variable - value
          vKeyOrder.insert(make_pair(sName, i++));
          if (kPair==kFALSE) vKeyValue.insert(make_pair(sName, 0.));
          sName.clear();

   // create Float_t array based on variables string and number of ':' separators
   varArrayN = 1 + count_if(varList.begin(), varList.end(), std::bind2nd(std::equal_to<char>(), ':'));
   varArray = new Float_t[varArrayN];
   kPair = kTRUE;
}

// ---------------------------------------------------------------------------------
HNtuple::~HNtuple()
{
   if (ptrNt != 0) delete ptrNt;
   if (varArray != 0) delete [] varArray;
}


// ---------------------------------------------------------------------------------
Int_t HNtuple::Write(const char* name, Int_t option, Int_t bufsize) 
{
   return ptrNt->Write(name, option, bufsize); 
}

// ---------------------------------------------------------------------------------
void HNtuple::SetDirectory(TDirectory* dir)
{
   return ptrNt->SetDirectory( dir );
}

// ---------------------------------------------------------------------------------
Float_t& HNtuple::operator[](const std::string& key)
{
   if (isNtuple)
   {
      mIter = vKeyValue.find(key);
      if (mIter != vKeyValue.end())
      {
         return mIter->second;
      }
      throw std::invalid_argument(Form("An unknown variable : %s name in HNtuple tried to be assigned",key.data()));
   }

return vKeyValue[key]; 
}

// ---------------------------------------------------------------------------------
const Float_t& HNtuple::operator[](const std::string& key) const
{
   std::map<std::string, Float_t>::const_iterator mcIter = vKeyValue.find(key);
   if (mcIter != vKeyValue.end())
   {
      return (mcIter->second); 
   }
   throw std::invalid_argument(Form("An unknown variable : %s name in HNtuple tried to be assigned",key.data()));
}

// ---------------------------------------------------------------------------------
Int_t HNtuple::fill()
{
// This function is similar to Fill(...) from NTuple class besides
// the fact that is has no parameter and is small "f" :)

   Int_t i;
   if (isNtuple==kTRUE)
   {
      for (i = 0; i < varArrayN; ++i) varArray[i] = 0.;
      for (mIter = vKeyValue.begin(); mIter != vKeyValue.end(); ++mIter)
      {
        varArray[ vKeyOrder[(*mIter).first] ] = (*mIter).second;
        // reset of map array
        (*mIter).second = 0.;
      }
   }
   else
   {
      // ntuple not booked yet, we create it here based on variables 
      // set with the function setVal
      
      std::string vList;
      for (mIter = vKeyValue.begin(); mIter != vKeyValue.end(); ++mIter)
      {
         vList += (*mIter).first + ":";
      }
      vList.erase(vList.find_last_of(":"),1);

      //-------- here a part of NTuple Ctor
      if (outFile) outFile->cd();
       else std::cerr << "WARNING: NTuple booked but not attached to any file. Storing in memory!" << std::endl;
      ptrNt = new TNtuple(cname, ctitle, vList.c_str(), cbufsize);
      isNtuple = kTRUE;
      setMap(vList.c_str(), isNtuple);
	 //-------- fill
         for (i = 0; i < varArrayN; ++i) varArray[i] = 0.;
         for (mIter = vKeyValue.begin(); mIter != vKeyValue.end(); ++mIter)
         {
           varArray[ vKeyOrder[(*mIter).first] ] = (*mIter).second;
           // reset of map array
           (*mIter).second = 0.;
         }
   }

   // filling the ROOT ntuple
   return ptrNt->Fill(varArray);
}
// ****************************************************************************



