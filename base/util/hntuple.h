// Author: Witold Przygoda, przygoda@if.uj.edu.pl, last modifed on: 1 March 2009

#ifndef HNTUPLE_H
#define HNTUPLE_H


#include <TROOT.h>
#include <TFile.h>
#include <TNtuple.h>
#include <string>
#include <utility>
#include <iterator>
#include <map>


// ****************************************************************************
class HNtuple: public TObject
{
// The aim of this class is to improve behaviour of TNtuple 
// It is inherited from TObject (and not from NTuple) because we want to 
// store as the output original NTuple type (and not HNtuple).
// Basic features:
// You do not need to create any float array of variables you want to fill with.
// You simply write something like: myHtuple["variable_name"] = variable_value
// and you can do it in any order you want (no more remembering of position of
// a variable in an array)

public:

   HNtuple(); // dummy constructor
   HNtuple(const char* name, const char* title, Int_t bufsize = 32000); // late ntuple constructor
   HNtuple(const char* name, const char* title, const char* varlist, Int_t bufsize = 32000); // basic constructor
   virtual ~HNtuple();

   Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0); // virtual function for a polymorphic NTulple Write call
   Float_t& operator[](const std::string& key); // the way of assigning values for variables
   const Float_t& operator[](const std::string& key) const; // the way of variable value reading
   Int_t fill(); // fills tuple, if not defined, constructs it first
   void	SetDirectory(TDirectory* dir); 
   const char* getName() const { return ptrNt->GetName(); }
   const char* getTitle() const { return ptrNt->GetTitle(); }
   void setFile(TFile *ptrF) { outFile = ptrF; }

protected:

   TFile *outFile;

   HNtuple& operator=(const HNtuple& src) const; 
   void setMap(const char* vlist, Bool_t& kPair); // creates a map from variable string

   const char* cname; //!
   const char* ctitle; //!
   Int_t cbufsize; //!
   TNtuple *ptrNt; //!

   Bool_t isNtuple; //! kTRUE if ntuple is defined
   Int_t varArrayN; //! number of ntuple variables
   Float_t *varArray; //! table of values for ntuple to be filled with
   std::string varList; //! list of variables for ntuple
   std::map<std::string, Float_t> vKeyValue; //! pair of a variable name and a value
   std::map<std::string, Int_t> vKeyOrder; //! pair of a variable name and its position in ntuple
   std::map<std::string, Float_t>::iterator mIter; //! iterator for a map variable - value
   //std::map<std::string, Float_t>::const_iterator mcIter; //! const iterator for a map variable - value

ClassDef(HNtuple, 0)
};
// ****************************************************************************


#endif /*!HNTUPLE_H*/

