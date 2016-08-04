#ifndef HShowerTaskSet_H
#define HShowerTaskSet_H

#include "htaskset.h"
#include "TString.h"

class HShowerTaskSet : public HTaskSet {
private:
  	
static  Int_t ianalysisSimOrReal; // counter for arguments of sim/real 	
static  Int_t ianalysisLevel; // counter for arguments of analysis level 	
static  Int_t imagnet; // counter for arguments of magnet 	

static  Int_t analysisSimOrReal; // analysis sim/real	
static  Int_t analysisLevel;	// analysis level leprecogn=1 
static  Bool_t isCoilOff; // kTRUE=magnet off, kFALSE=magnet on 
static  Bool_t isSimulation; // kTRUE=simulation , kFALSE=real 
static  Bool_t isLowShowerEfficiency; // kTRUE=low , kFALSE=high 
  
		

public:
  HShowerTaskSet(void);
  HShowerTaskSet(const Text_t name[],const Text_t title[]);
  ~HShowerTaskSet(void);

 // functions to set analysis
   void setAnalysisSimOrReal(Option_t*);
   void setAnalysisLevel(Option_t*);
   void passArguments(TString s1);
   void printOptions(void);	   

 // function to be used for derived task sets (HShowerTaskSet, etc.)
  HTask *make(const Char_t *select="",const Option_t *option="");

  ClassDef(HShowerTaskSet,2) // Set of tasks
};

#endif /* !HShowerTaskSet_H */
