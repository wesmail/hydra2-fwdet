#ifndef HDETPARORA2IO_H
#define HDETPARORA2IO_H

#include "hdetpario.h"
#include "hgeomtransform.h"

class HOra2Conn;
class HParOra2Set;
class HRun;
class HDetGeomPar;
class HSpecGeomPar;
class HGeomVolume;
class HOra2GeomDetVersion;

class HDetParOra2Io : public HDetParIo {
private:
  HOra2Conn* pConn;         // pointer to Oracle connection class
  HRun*      actContVers;   // pointer to the actual list of container versions
  Int_t      actRunId;      // actual runId (can be -1 if there are no data in Oracle)
  TList*     containerList; // list of parameter containers
protected:
  Int_t      runStart;      // start time of current run
public:
  HDetParOra2Io(HOra2Conn* p=0);
  virtual ~HDetParOra2Io(void);
  void commit(void);
  void rollback(void);
  void showSqlError(const Char_t*);
  Int_t getActRunId(void) { return actRunId; }
  Int_t getRunStart(HParSet* pPar=0);
  const Char_t* getExpLocation();
  Int_t getPredefVersion(HParSet*);
  const Char_t* getHistoryDate();
  Bool_t readDetectorGeometry(HDetGeomPar*,Int_t*,HOra2GeomDetVersion*);
  Int_t writeAlignment(HDetGeomPar*);
protected:
  void setChanged(HParSet*);
  void setChanged(HParSet*,Int_t);
  void addGeomOraSet(HDetGeomPar*);
  Int_t getDetectorId(const Char_t*);
  Int_t getGeomVersion(const Char_t*,HOra2GeomDetVersion*);
  Int_t getGeomVersion(Int_t,HOra2GeomDetVersion*);
  virtual void addGeomRefComponents(HDetGeomPar*,TList*);
  virtual Bool_t transformGeomCompositeComponents(HDetGeomPar*) {return kTRUE;}
  Bool_t readIdealGeometry(TList*,HOra2GeomDetVersion*);
  Bool_t readGeomPoints(TList*);
  Bool_t readGeomTransform(TList*);
  Int_t getAlignmentVersion(HParOra2Set*,const Char_t*);
  Int_t readAlignmentTransform(TList*,Int_t);
  HParOra2Set* getOraSet(HParSet*);
  Int_t getContextId(const Char_t*,const Char_t*);
  Int_t getAlignmentOutputVersion(HParSet*,const Char_t*);
  Bool_t writeTransform(Int_t,const Char_t*,const HGeomTransform&);
private:
  ClassDef(HDetParOra2Io,0) // base class for the detector interface to Oracle
};

#endif  /* !HDETPARORA2IO_H */
