#ifndef __HDST_H__
#define __HDST_H__

#include "TObject.h"
#include "TString.h"


class HDst : public TObject {

public:

    HDst(){;}
    ~HDst(){;}

    static void setupSpectrometer    (TString beamtime ,Int_t mdcsetup[6][4],TString detectors = "rich,mdc,tof,rpc,shower,wall,tbox,start");
    static void setupParameterSources(TString parsource = "oracle",TString asciiParFile = "",TString rootParFile = "", TString histDate="now");
    static void setDataSource        (Int_t sourceType = -1,TString inDir = "",TString inFile = "", Int_t refId = -1 ,TString eventbuilder = "lxhadeb02.gsi.de");
    static void setSecondDataSource  (TString inDir = "",TString inFile = "", Int_t refId = -1);
    static void setupUnpackers       (TString beamtime = "aug11",TString detectors = "rich,mdc,tof,rpc,shower,wall,tbox,latch,start",Bool_t correctINL=kTRUE);

    ClassDef(HDst,0)
};

#endif //__HDst_H__
