//*-- Author : Jochen Markert 18.07.2007

#ifndef  __HLOOP_H__
#define  __HLOOP_H__

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TArrayI.h"
#include "TSystem.h"
#include "TString.h"
#include "TObject.h"

#include "htree.h"
#include "heventheader.h"
#include "hrecevent.h"
#include "hpartialevent.h"
#include "hgeantheader.h"
#include "hgeantmedia.h"
#include "hcategory.h"

#include <map>
#include <fstream>
#include <iomanip>
using namespace std;




class HSectorSelector : public TObject {

private:
    map<TString,TArrayI> goodSectors;

    Int_t defaultVal;
public:
    HSectorSelector(Bool_t val=kFALSE) {
        defaultVal=val;
    };
    ~HSectorSelector() { goodSectors.clear();};

    void setDefaultVal(Bool_t val) { defaultVal = val;}


    Bool_t readInput(TString infile="",Bool_t print=kFALSE)
    {
	// format: file  1 1 1 1 1 1  (2char+11digit ,hld name without eventbuilder +  6 digits (0 or 1) per line)
        goodSectors.clear();

	if(gSystem->AccessPathName(infile.Data())==1){
	    Error("readInput()","Inputfile %s could not be read!",infile.Data());
            return kFALSE;
	}

	TArrayI sectors(6);
	sectors.Reset(0);

        TString file;

        ifstream input;
	input.open(infile.Data());
	while (!input.eof()&&input.good()) {
	    input>>file>>sectors[0]>>sectors[1]>>sectors[2]>>sectors[3]>>sectors[4]>>sectors[5];
	    if (input.good()) {
		if(print)cout<<file<<":  "<<sectors[0]<<" "<<sectors[1]<<" "<<sectors[2]<<" "<<sectors[3]<<" "<<sectors[4]<<" "<<sectors[5]<<endl;
		goodSectors[file] = sectors;
	    }
	}
	input.close();

        return kTRUE;
    }

    Int_t  getNFiles() {return goodSectors.size();}
    Bool_t getSectors(Int_t* sectors,TString filename)
    {
	map<TString,TArrayI>::iterator iter = goodSectors.find(filename);
	if (iter == goodSectors.end()) {
	    // file not in list
	    for(Int_t s=0;s<6;s++){
		sectors[s] = defaultVal;
	    }
	    return kFALSE;

	} else {
	    for(Int_t s=0;s<6;s++){
		sectors[s]=iter->second.At(s);
	    }
	}
	return kTRUE;
    }

    void printFile(TString filename) {
	map<TString,TArrayI>::iterator iter = goodSectors.find(filename);
	if (iter == goodSectors.end()) {
	    // file not in list
	    ::Error("printFile()","File %s not found in map!",filename.Data());
	} else {
	    cout<<setw(30)<<filename
		<<" "<<iter->second.At(0)
		<<" "<<iter->second.At(1)
		<<" "<<iter->second.At(2)
		<<" "<<iter->second.At(3)
		<<" "<<iter->second.At(4)
		<<" "<<iter->second.At(5)
		<<endl;
	}
    }

    void print(){
        Int_t ct=0;
	for(map< TString, TArrayI>::iterator iter = goodSectors.begin(); iter != goodSectors.end(); ++iter ) {
	    ct++;
	    cout<<setw(5)<<ct
		<<" "<<setw(30)<<iter->first
		<<" "<<iter->second.At(0)
		<<" "<<iter->second.At(1)
		<<" "<<iter->second.At(2)
		<<" "<<iter->second.At(3)
		<<" "<<iter->second.At(4)
		<<" "<<iter->second.At(5)
		<<endl;
	}
    }
    ClassDef(HSectorSelector,0) // simple file to valide sector map
};

class HLoop;

R__EXTERN HLoop *gLoop;

class HLoop : public TObject {

private:

    TChain*                        fChain; // chain
    HEventHeader*                   fHead; // pointer to HADES event header
    HGeantMedia*              fGeantMedia; // pointer to HGEANT MEDIA (NULL if not avvailable!)
    map <TString,HCategory*>       fEvent; // map category name -> HCategory pointer
    map <TString,HPartialEvent*> fPartial; // map partial event name -> HPartialEvent pointer
    map <TString,Int_t>           fStatus; // map category name -> status
    map <TString,TString>       fPartialN; // map category name -> partial event name
    map <TString,Short_t>      fNameToCat; // map category name -> category number
    Long64_t                  fMaxEntries; // total number of entries in the chain
    Long64_t                fCurrentEntry; // current entry in chain
    TString                  fCurrentName; // current file name
    TFile*                   fFileCurrent; // current file pointer
    TTree*                          fTree; // current Tree of the chain
    HRecEvent*                  fRecEvent; // HADES event under reconstruction

    HSectorSelector       fSectorSelector; // selection of sectors from filelist
    Int_t                     fsectors[6]; // array of sector status form filelist


    Int_t                          fRefID; // reference ID for init of params
    Bool_t                    fFirstEvent; // true if first event called
    Bool_t               fHasCreatedHades; // remember if this class has created HADES
    Bool_t                     fIsNewFile; // == kTRUE for first entry after a new file is opened
    Bool_t                     fIsSkipped; // == kTRUE event was skipped by tasklist
    Bool_t                    fUseTaskSet; // == kTRUE if hades tasksets should be executed
    Long64_t               fTreeCacheSize; //
    Long64_t        fTreeCacheDefaultSize; // default 8MB
    Bool_t                    fIsCacheSet; // kTRUE if user has set the the cache.
    Bool_t        setStatus(TString catname, Int_t stat );
    Bool_t        addCatName(TString catname,Short_t catNum);

public:

    HLoop(Bool_t createHades=kFALSE);

    ~HLoop();
    void           setCurrentLoop() { gLoop=this;}

    Bool_t         addFile     (TString infile);
    Bool_t         addFiles    (TString expression);
    Bool_t         addFilesList(TString filelist);
    Bool_t         addMultFiles(TString commaSeparatedList);

    Bool_t           readSectorFileList(TString filename,Bool_t defaultVal=kFALSE,Bool_t print=kFALSE)  { fSectorSelector.setDefaultVal(defaultVal); return fSectorSelector.readInput(filename,print); }
    HSectorSelector& getSectorSelector()                                       { return fSectorSelector;}
    void             getSectors(Int_t* sectors)                                { for(Int_t s=0;s<6;s++) sectors[s]=fsectors[s];}
    Bool_t           goodSector(Int_t sector)                                  { return ( (sector < 6 && sector >=0 ) ? (fsectors[sector]==1) : kFALSE );}
    HEventHeader*  getEventHeader()            { return fHead; }
    HGeantMedia*   getGeantMedia ()            { return fGeantMedia; }
    TChain*        getChain      ()            { return fChain; }
    TTree*         getTree       ()            { return fTree; }
    Long64_t       getEntries    ()            { return fMaxEntries; }
    HCategory*     getCategory      (TString catname,Bool_t silent = kFALSE);
    Bool_t         getCategoryStatus(TString catname,Bool_t silent = kFALSE);
    HPartialEvent* getPartialEvent  (TString catname,Bool_t silent = kFALSE);
    HGeantHeader*  getGeantHeader   (Bool_t silent=kFALSE);
    TObject*       getFromInputFile(TString name="");
    Bool_t         isSkippedEvent()            { return fIsSkipped; }
    Bool_t         isNewFile(TString& name);
    Bool_t         setInput(TString readCategories = "");
    void           clearCategories();
    Int_t          nextEvent(Int_t iev);
    void           setRefID(Int_t id)          { fRefID = id;}
    void           setTreeCacheSize(Long64_t cs = 8000000);
    void           printCategories();
    void           printChain();
    void           printBranchStatus();

    ClassDef(HLoop,0) // simple class to fastly loop dst's
};

#endif /* !__HLOOP_H__ */







