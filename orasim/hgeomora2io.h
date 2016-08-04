#ifndef HGEOMORA2IO_H
#define HGEOMORA2IO_H

#include "hgeomio.h"
#include "hgeomtransform.h"
#include "TString.h"
#include "TObjArray.h"
#include "TList.h"

class HGeomOra2Conn;
class HGeomMedia;
class HGeomMedium;
class HGeomSet;
class HGeomNode;
class HGeomHit;
class HGeomInterface;

class HGeomOra2Io : public HGeomIo {
private:
  HGeomOra2Conn* pConn; // pointer to the connection class
  TList* detVersions;   // list of detector versions
  Double_t maxSince;    // lower limit for actual time range of versions
  Double_t minUntil;    // upper limit for actual time range of versions
  class HOra2Obj : public TObject {  // Container utility class
    public:
      TObject* pObj;
      Int_t    oraId;
      HOra2Obj(TObject* p=0,Int_t i=-1) {
        pObj=p;
        oraId=i;
      }
      ~HOra2Obj() {}
  };
  class HOra2DetVers : public TNamed { // Container class for detector version 
    public:
      Int_t detectorId;       // Id of detector
      Int_t geomVersion;      // geometry version
      HOra2DetVers(const Char_t* pName=0) {
        SetName(pName);
        detectorId=-1;
        geomVersion=-1;
      }
      ~HOra2DetVers() {}
  };
  class HOra2TransObj : public TNamed {  // Container class for transformation
    public:
      TObject*       pObj;
      Int_t          oraId;
      TString        refObj;
      HGeomTransform refTransform;
      HOra2TransObj(TObject* p=0,Int_t i=-1) {
        if (p) {
          SetName(p->GetName());
          pObj=p;
        }
        oraId=i;
      }
      ~HOra2TransObj() {}
  };
  class HGeomOra2CopyNode : public TNamed {
    public:
      HGeomNode* pNode;
      HGeomOra2CopyNode(const Char_t* name,HGeomNode* node) {
        SetName(name);
        pNode=node;
      }
      ~HGeomOra2CopyNode() {}
  };
public:
  HGeomOra2Io();
  ~HGeomOra2Io();
  Bool_t open();
  Bool_t open(const Char_t*,const Text_t* status="in");
  void close();
  void print();
  Bool_t isOpen();
  Bool_t isWritable();
  Bool_t setSimulRefRun(const Char_t*);
  Bool_t setRunId(Int_t);
  Bool_t setHistoryDate(const Char_t*);
  const Char_t* getSimulRefRun();
  Int_t getCurrentRunId();
  const Char_t* getHistoryDate();
  Bool_t read(HGeomMedia*);
  Bool_t read(HGeomSet*,HGeomMedia*);
  Bool_t read(HGeomHit*);
  Bool_t write(HGeomMedia*);
  Bool_t write(HGeomSet*);
  Bool_t write(HGeomHit*);
  Bool_t readGeomConfig(HGeomInterface*);
  Int_t checkRunidExistence(Int_t);
private:
  Int_t readGeomSetup();
  Bool_t readMaterialComposition(HGeomMedia*,TObjArray*,Int_t);
  Bool_t readOpticalProperties(HGeomMedia*,TObjArray*,Int_t);
  Bool_t readTarget(HGeomSet*,HGeomMedia*,Int_t,Int_t);
  Bool_t readStart(HGeomSet*,HGeomMedia*,Int_t,Int_t);
  Bool_t readVolumes(HGeomSet*,HGeomMedia*,TObjArray*,Int_t);
  Bool_t readPoints(TObjArray*,Int_t); 
  Bool_t readTransform(TList*);
  Int_t createVersion(const Char_t*,TString&,TString&);
  Int_t createHitVersion(const Char_t* part,const Char_t* hitSet);
  Bool_t insertMaterialData(Int_t,HGeomMedium*);
  Bool_t insertOpticalData(Int_t,HGeomMedium*);
  Bool_t insertVolumePoints(Int_t,HGeomNode*);
  const Char_t* calcRefTransform(HGeomNode*,HGeomTransform&);
  ClassDef(HGeomOra2Io,0) // Class for geometry I/O from Oracle
};

#endif  /* !HGEOMORA2IO_H */
