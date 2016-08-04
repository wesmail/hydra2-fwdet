#ifndef __HTIME_H__
#define __HTIME_H__

#include "TObject.h"
#include "TString.h"

#include <time.h>
#include <iostream>
#include <cstdio>

using namespace std;

class HTime : public TObject {

public:

    HTime();
    virtual ~HTime();
    static Bool_t  isHldName    (TString name,Bool_t EvtBuilder=kTRUE,Bool_t silent=kTRUE);
    static TString stripFileName(TString name,Bool_t removeEvtBuilder=kFALSE,Bool_t silent=kFALSE);
    static void    dayOfYearToDate(Int_t year,Int_t dayOfYear,Int_t& month,Int_t& day,Bool_t print=kFALSE);
    static void    splitFileName(TString name,TString& type,Int_t& year,Int_t& dayOfYear,Int_t& hour,Int_t& minute,Int_t& second,Int_t& evtbuild,Bool_t print=kFALSE);
    static TString getTypeFileName(TString name,Bool_t print=kFALSE);
    static Int_t   getYearFileName(TString name,Bool_t print=kFALSE);
    static Int_t   getDayFileName(TString name,Bool_t print=kFALSE);
    static void    getTimeFileName(TString name,Int_t& hour,Int_t& minute,Int_t& second,Bool_t print=kFALSE);
    static Int_t   getEvtBuilderFileName(TString name,Bool_t print=kFALSE);
    static time_t  fileNameToTime(TString name, Bool_t print=kFALSE);
    static time_t  runIdToTime(time_t runid, Int_t timezone = 1, Bool_t print = kFALSE);
    static void    runIdToBrokenTime(Int_t runid,Int_t& year,Int_t& month,Int_t& day,Int_t& hour,Int_t& min  ,Int_t& sec,Int_t timezone=1,Bool_t print=kFALSE);
    static Int_t   isDST(TString name);

    ClassDef(HTime,0)
};

#endif //__HTIME_H__
