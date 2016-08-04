#ifndef HGEOMSET_H
#define HGEOMSET_H

#include "hgeomtransform.h"
#include "TNamed.h"
#include "TArrayI.h"
#include "TList.h"
#include <fstream>
#include <iomanip>
using namespace std;

class HGeomNode;
class HGeomShapes;
class HGeomMedia;
class HGeomBuilder;
class HGeomHit;

class  HGeomSet : public TNamed {
protected:
  class HGeomCopyNode : public TNamed {
    public:
      HGeomNode* pNode;
      HGeomCopyNode(const Char_t* name,HGeomNode* node) {
        SetName(name);
        pNode=node;
      }
      ~HGeomCopyNode() {}
  };
  TList* volumes;         // list of volumes
  TList* masterNodes;     // pointer to list of mother volumes from other detector parts
  Int_t maxSectors;       // maximum number of sectors (-1 for detectors outside any sector)
  Int_t maxKeepinVolumes; // maximum number of keepin volumes per sector
  Int_t maxModules;       // maximum number of modules per sector
  TArrayI* modules;       // Module's array.
  HGeomShapes* pShapes;   // pointer to the class HGeomShapes
  TString geoFile;        // name of geometry input file or Oracle
  HGeomHit* pHit;         // pointer to object for hit definition
  TString author;         // author of the media version
  TString description;    // description of the version

  HGeomSet();
  void readInout(fstream&);
  void readTransform(fstream&,HGeomTransform&);
  Bool_t readVolumeParams(fstream&,HGeomMedia*,HGeomNode*,TList* l=0);
  Bool_t readKeepIn(fstream&,HGeomMedia*,TString&);
  Bool_t readModule(fstream&,HGeomMedia*,TString&,TString&,Bool_t a=kFALSE);
public :
  virtual ~HGeomSet();
  void setShapes(HGeomShapes* s) {pShapes=s;}
  void setMasterNodes(TList* m) {masterNodes=m;}
  void setGeomFile(const Char_t* filename) {geoFile=filename;}
  void setHitFile(const Char_t*);
  const Char_t* getGeomFile() { return geoFile.Data(); }
  HGeomHit* getHit() {return pHit;}
  Int_t getMaxSectors(void) {return maxSectors;}
  Int_t getMaxModules(void) {return maxModules;}
  Int_t getMaxKeepinVolumes(void) {return maxKeepinVolumes;}
  void setModules(Int_t,Int_t*);
  Int_t* getModules(void);
  Int_t getModule(Int_t,Int_t);
  HGeomNode* getVolume(const Char_t* name) {return (HGeomNode*)volumes->FindObject(name);}
  HGeomNode* getMasterNode(const Char_t* name) {return (HGeomNode*)masterNodes->FindObject(name);}
  TList* getListOfVolumes() {return volumes;}
  HGeomShapes* getShapes() {return pShapes;}
  void setAuthor(TString& s) {author=s;}
  void setDescription(TString& s) {description=s;}
  TString& getAuthor() {return author;}
  TString& getDescription() {return description;}
  virtual const Char_t* getKeepinName(Int_t,Int_t) {return 0;}  
  virtual const Char_t* getModuleName(Int_t) {return 0;}  
  virtual const Char_t* getEleName(Int_t) {return 0;}  
  virtual Int_t getSecNumInMod(const TString&) {return -1;}
  virtual Int_t getModNumInMod(const TString&) {return 0;}
  virtual Bool_t read(fstream&,HGeomMedia*);
  virtual void addRefNodes() {}
  virtual void write(fstream&);
  virtual void print();
  virtual Bool_t create(HGeomBuilder*, const TString&, HGeomMedia*);
  virtual Bool_t createAdditionalGeometry(HGeomBuilder*, const TString&, HGeomMedia*) {return kTRUE;}
  void compare(HGeomSet&);
  ClassDef(HGeomSet,0) // Base class for geometry of detector parts
};

#endif  /* !HGEOMSET_H */
