//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomSet
//
// Base class for geometry of detector parts
//
/////////////////////////////////////////////////////////////

#include "hgeomset.h"
#include "hgeomnode.h"
#include "hgeomshapes.h"
#include "hgeombasicshape.h"
#include "hgeommedia.h"
#include "hgeommedium.h"
#include "hgeomhit.h"
#include "hgeombuilder.h"
#include "TString.h"
#include <ctype.h>

ClassImp(HGeomSet)

HGeomSet::HGeomSet() {
  // Constructor
  volumes=new TList();
  masterNodes=0;
  modules=0;
  pShapes=0;
  maxSectors=0;
  maxKeepinVolumes=0;
  maxModules=0;
  pHit=0;
}

HGeomSet::~HGeomSet() {
  // Destructor
  if (volumes) {
    volumes->Delete();
    delete volumes;
    volumes=0;
  }
  if (modules) {
    delete modules;
    modules=0;
  }
  if (pHit) {
    delete pHit;
    pHit=0;
  }
}

void HGeomSet::setModules(Int_t s,Int_t* m){
  // Stores the modules given in 'm' as active modules in sector 's'
  // May be called with s=-1 for detectors not belonging to a sector
  if (!modules) {
    if (maxSectors==0&&maxModules>0) modules=new TArrayI(maxModules);
    if (maxSectors>0&&maxModules>0) modules=new TArrayI(maxSectors*maxModules);
  }
  Int_t sec=0;
  if (s>0) sec=s;
  for(Int_t i=0;i<maxModules;i++) {
    if (modules) modules->AddAt(m[i],(sec*maxModules+i));
  }
}

Int_t* HGeomSet::getModules(){
  // Returns a linear array of all modules
  if (modules) return modules->GetArray();
  return 0;
}

Int_t HGeomSet::getModule(Int_t s,Int_t m) {
  // Returns 1, if the module is not explicitly set inactive
  // (in this case it returns 0)
  // May be called with s=-1 for detectors not belonging to a sector
  if (!modules) return 1;
  if (m<maxModules&&s<0) return (*modules)[m];
  if (m<maxModules&&s<maxSectors) return (*modules)[(s*maxModules+m)];
  return 0;
}

void HGeomSet::setHitFile(const Char_t* filename) {
  // Sets the name of the hit definition file
  if (pHit) pHit->setHitFile(filename);
}

Bool_t HGeomSet::read(fstream& fin,HGeomMedia* media) {
  // Reads the geometry from file
  Int_t s1=-1,s2=0;
  if (maxSectors>0) {
    s1=0;
    s2=maxSectors;
  }
  Bool_t rc=kTRUE;
  for (Int_t s=s1;s<s2&&rc;s++) {
    for (Int_t k=0;k<maxKeepinVolumes&&rc;k++) {
      TString keepinName=getKeepinName(s,k);
      rc=readKeepIn(fin,media,keepinName);
    }
  }
  for (Int_t m=0;m<maxModules&&rc;m++) {
    Bool_t containsActiveModule=kFALSE;
    for (Int_t s=s1;s<s2;s++) {
      if (getModule(s,m)) containsActiveModule=kTRUE;
    }
    TString modName=getModuleName(m);
    TString eleName=getEleName(m);
    rc=readModule(fin,media,modName,eleName,containsActiveModule);
  }
  return rc;
}

void HGeomSet::readInout(fstream& fin) {
  // Reads the inout flag (in old files)
  Char_t c=' ';
  do {
    c=fin.get();
  } while (c==' ' || c=='\n');
  if (c!='0'&&c!='1') fin.putback(c);
  else do {
    c=fin.get();
  } while (c!='\n');
  return;
}

void HGeomSet::readTransform(fstream& fin,HGeomTransform& tf) {
  // Reads the transformation from file
  Double_t r[9], t[3];
  fin>>t[0]>>t[1]>>t[2];
  for(Int_t i=0;i<9;i++) fin>>r[i];
  tf.setRotMatrix(r);
  tf.setTransVector(t);
}

Bool_t HGeomSet::readVolumeParams(fstream& fin,HGeomMedia* media,
                                 HGeomNode* volu,TList* refVolumes) {
  // Reads the volume definition from file
  if (volu==0||media==0||refVolumes==0) return kFALSE;
  const Int_t maxbuf=256;
  Char_t buf[maxbuf];
  TString name(volu->GetName());
  Int_t nameLength=name.Length();
  Bool_t isCopy=kFALSE;
  fin>>buf;
  HGeomNode* mother=0;
  HGeomCopyNode* refNode=0;
  TString refName;
  if (volu->isKeepin()) mother=getMasterNode(buf);
  else if (volu->isModule()) { 
    mother=getMasterNode(buf);
    if (!mother) mother=getVolume(buf);
    if (volu->isActive()&&mother) mother->setActive();
  } else {
    mother=getVolume(buf);
    if (!mother) mother=getMasterNode(buf); 
  }
  volu->setMother(mother);
  if (!mother) Warning("readVolumeParams","Mother volume %s not found!",buf);
  if (nameLength>4) {
    Char_t c;
    do {
      c=fin.get();
    } while (c==' ' || c=='\n');
    Int_t i=(Int_t)c;
    fin.putback(c);
    if (!isalpha(i)) isCopy=kTRUE;
    refName=name(0,4);
    refNode=(HGeomCopyNode*)refVolumes->FindObject(refName);
    if (!refNode) {
      if (isCopy) return kFALSE;
      refVolumes->Add(new HGeomCopyNode(refName.Data(),volu));
    } else {
      HGeomNode* cn=refNode->pNode;
      volu->setCopyNode(cn);
      volu->setMedium(cn->getMedium());
      volu->setShape(cn->getShapePointer());
      Int_t n=cn->getNumPoints();
      volu->createPoints(n);
      for (Int_t i=0;i<n;i++) volu->setPoint(i,*(cn->getPoint(i)));
    }
  }
  if (!isCopy) {
    fin>>buf;
    HGeomBasicShape* sh=pShapes->selectShape(buf);
    if (sh) volu->setShape(sh);
    else return kFALSE;
    fin>>buf;
    HGeomMedium* medium=media->getMedium(buf);
    if (!medium) {
      Error("readVolumeParams","Medium %s not found in list of media",buf);
      return kFALSE;
    }
    volu->setMedium(medium);
    Int_t n=0;
    fin.getline(buf,maxbuf); // to read the end of line
    if (sh) n=sh->readPoints(&fin,volu);
    if (n<=0) return kFALSE;
  }
  readTransform(fin,volu->getTransform());
  return kTRUE;
}

Bool_t HGeomSet::readKeepIn(fstream& fin,HGeomMedia* media,TString& name) {
  // Reads the keepin volume from file
  fin.clear();
  fin.seekg(0, ios::beg);
  HGeomNode* volu=0;
  Bool_t rc=kTRUE;
  TList refVolumes;
  const Int_t maxbuf=256;
  Char_t buf[maxbuf];
  while(rc && !volu && !fin.eof()) {
    fin>>buf;
    if (buf[0]=='/') fin.getline(buf,maxbuf);
    else if (!fin.eof()) {
      if (strcmp(buf,name)==0) {
        volu=new HGeomNode;
        volu->SetName(buf);
        volu->setVolumeType(kHGeomKeepin);
        readInout(fin);
        rc=readVolumeParams(fin,media,volu,&refVolumes);
      } else {
        do {
          fin.getline(buf,maxbuf); 
        } while(!fin.eof()&&buf[0]!='/'); 
      }
    }
  }
  if (rc) volumes->Add(volu);
  else {
    delete volu;
    volu=0;
  }
  refVolumes.Delete();
  return rc;
}

Bool_t HGeomSet::readModule(fstream& fin,HGeomMedia* media,TString& modName,
                           TString& eleName,Bool_t containsActiveMod) {
  // Reads the whole geometry of a module from file
  const Int_t maxbuf=256;
  Text_t buf[maxbuf];
  fin.clear();
  fin.seekg(0,ios::beg);
  HGeomNode* volu=0;
  Bool_t rc=kTRUE;
  TList refVolumes;
  TString name;
  while(rc && !fin.eof()) {
    fin>>buf;
    if (buf[0]=='/') fin.getline(buf,maxbuf);
    else if (!fin.eof()) {
      TString name(buf);      
      if (name.BeginsWith(modName)) {
        volu=new HGeomNode;
        volu->SetName(buf);
        volu->setVolumeType(kHGeomModule);
        Int_t a=getModule(getSecNumInMod(buf),getModNumInMod(buf));
        if (a>0) volu->setActive(kTRUE);
        else volu->setActive(kFALSE);
        readInout(fin);
        rc=readVolumeParams(fin,media,volu,&refVolumes);
        if (rc) volumes->Add(volu);
        else {
          Error("readModule","Failed for module %s\n",volu->GetName());
          delete volu;
          volu=0;
        }
      } else if (!eleName.IsNull()&&name.BeginsWith(eleName)) {
        volu=new HGeomNode;
        volu->SetName(buf);
        volu->setVolumeType(kHGeomElement);
        volu->setActive(containsActiveMod);
        rc=readVolumeParams(fin,media,volu,&refVolumes);
        if (rc) volumes->Add(volu);
        else {
          Error("readModule","Failed for %s\n",volu->GetName());
          delete volu;
          volu=0;
        }
      } else {
        do {
          fin.getline(buf,maxbuf);
        } while(!fin.eof()&&buf[0]!='/'); 
      }
    }
  }
  refVolumes.Delete();
  return rc;
}

void HGeomSet::print() {
  // Prints the geometry
  if (!author.IsNull())      cout<<"//Author:      "<<author<<'\n';
  if (!description.IsNull()) cout<<"//Description: "<<description<<'\n';
  if (!description.IsNull()) 
  cout<<"//----------------------------------------------------------\n";
  cout.setf(ios::fixed,ios::floatfield);
  TListIter iter(volumes);
  HGeomNode* volu;
  while((volu=(HGeomNode*)iter.Next())) {
    volu->print();
  }
}

void HGeomSet::write(fstream& fout) {
  // Writes the volumes to file
  if (!author.IsNull())      fout<<"//Author:      "<<author<<'\n';
  if (!description.IsNull()) fout<<"//Description: "<<description<<'\n';
  fout<<"//----------------------------------------------------------\n";
  fout.setf(ios::fixed,ios::floatfield);
  TListIter iter(volumes);
  HGeomNode* volu;
  Bool_t rc=kTRUE;
  while((volu=(HGeomNode*)iter.Next())&&rc) {
    rc=volu->write(fout);
  }
}

Bool_t HGeomSet::create(HGeomBuilder* builder, const TString& paramFile, HGeomMedia* media) {
  // Creates the geometry
  if (!builder) return kFALSE;
  TListIter iter(volumes);
  HGeomNode *volu, *cn;
  Bool_t rc=kTRUE;
  while((volu=(HGeomNode*)iter.Next())&&rc) {
    if (volu->isKeepin()||volu->isModule()) {
      if (volu->isActive()) {
        rc=builder->createNode(volu);
        if (rc) {
          HGeomNode* n=getMasterNode(volu->GetName());
          if (n) {
            n->setCenterPosition(volu->getCenterPosition());
            n->setRootVolume(volu->getRootVolume());
          }
          if (volu->isSensitive()&&(cn=volu->getCopyNode())==0&&pHit) {
            pHit->setCurrentNode(volu);
            rc=builder->createHit(pHit);
            if (!rc) Error("create","Creation of hit for %s failed!",
                           volu->GetName());
          }
        } else Error("create","Creation of %s failed!",volu->GetName());
      } else {
        volu->getParameters();
        volu->getPosition();
      }
    }
  }
  iter.Reset();
  while((volu=(HGeomNode*)iter.Next())&&rc) {
    if (!volu->isKeepin()&&!volu->isModule()&&volu->isActive()) {
      rc=builder->createNode(volu);
      if (rc) {
        if (volu->isTopNode()||volu->isRefNode()) {
          HGeomNode* n=getMasterNode(volu->GetName());
          if (n) {
            n->setCenterPosition(volu->getCenterPosition());
            n->setRootVolume(volu->getRootVolume());
          }
        }
        if (volu->isSensitive()&&(cn=volu->getCopyNode())==0&&pHit) {
          pHit->setCurrentNode(volu);
          rc=builder->createHit(pHit);
          if (!rc) Error("create","Creation of hit for %s failed!",
                         volu->GetName());
        }
      } else Error("create","Creation of %s failed!",volu->GetName());
    }
  }
  if (rc && paramFile.Length() > 0) {
    rc=createAdditionalGeometry(builder, paramFile, media);
  }
  return rc;
}

void HGeomSet::compare(HGeomSet& rset) {
  // Compares the geometry parameters and print a diagnose
  if (fName.CompareTo(rset.GetName())!=0) {
    Error("compare","Sets have different type");
    return;
  }
  TListIter iter(volumes);
  HGeomNode* volu;
  Int_t n=0, nnf=0, nDiff=0;
  cout<<"name\t mother medium shape  points    pos    rot\n";
  cout<<"------------------------------------------------\n";
  while((volu=(HGeomNode*)iter.Next())) {
    n++;
    HGeomNode* rNode=rset.getVolume(volu->GetName());
    if (!rNode) nnf++;
    else if (volu->compare(*rNode)>0) nDiff++;
  }
  cout<<"Number of volumes in first list:             "<<n<<'\n';
  cout<<"Number of different volumes:                 "<<nDiff<<endl;
  cout<<"Number of volumes not found in second list:  "<<nnf<<'\n';
  TList* rlist=rset.getListOfVolumes();
  if (rlist->GetSize()!=(n-nnf)) {
    nnf=0;
    TListIter riter(rlist);
    while((volu=(HGeomNode*)riter.Next())) {
      HGeomNode* rNode=getVolume(volu->GetName());
      if (!rNode) nnf++;
    }
  } else nnf=0;
  cout<<"Number of additional volumes in second list: "<<nnf<<'\n';
}
