//*-- AUTHOR : Ilse Koenig
//*-- modified : 28/01/2009 by Ilse Koenig


//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
// HDetParAsciiFileIo
//
// Class for parameter input/output from/into ASCII file
//
/////////////////////////////////////////////////////////////

#include "hdetparasciifileio.h"
#include "hdetgeompar.h"
#include "hgeomcompositevolume.h"
#include "hgeomshapes.h"
#include "htrb3calpar.h"
#include "TClass.h"

ClassImp(HDetParAsciiFileIo)

HDetParAsciiFileIo::HDetParAsciiFileIo(fstream* f) {
  // constructor
  pFile=f;
  sepLine=
    "##############################################################################\n";
}


Bool_t HDetParAsciiFileIo::findContainer(const Text_t* name) {
  // searches the container in the file
  const Int_t maxbuf=4000;
  Text_t buf[maxbuf];
  Text_t buf2[maxbuf];
  sprintf(buf2,"%s%s%s","[",name,"]");
  pFile->clear();
  pFile->seekg(0,ios::beg);
  while (!pFile->eof()) {
    pFile->getline(buf,maxbuf);
    if (buf[0]!='[') continue;
    if (!strncmp(buf,buf2,strlen(buf2))) break;
  }
  if (pFile->eof()) return kFALSE;
  return kTRUE;
}


Bool_t HDetParAsciiFileIo::checkAllFound(Int_t* set, Int_t setSize) {
  // checks if all modules have been initialized
  Bool_t allFound=kTRUE;
  for(Int_t i=0; i<setSize; i++) {
    if (set[i]==999) {
      set[i]=0;
      printf(" %i",i);
    }
    if (set[i]) allFound=kFALSE;
  }
  printf("\n");
  return allFound;
}


void HDetParAsciiFileIo::writeHeader(const Text_t* name, const Text_t* context,
                                     const Text_t* author, const Text_t* description) {
  // calls the function putAsciiHeader(...) of the parameter container
  // class and writes the header to the file
  pFile->write(sepLine,strlen(sepLine));
  pFile->write(fHeader.Data(),fHeader.Length());
  pFile->write(sepLine,strlen(sepLine));
  *pFile<<"["<<name<<"]\n";
  if (strlen(context)>0) *pFile<<"// Parameter Context: "<<context<<"\n";
  if (strlen(author)>0) *pFile<<"author:      "<<author<<"\n";
  TString t=description;
  if (!t.IsNull()) {
    t=t.ReplaceAll("\n"," \\\n");
    *pFile<<"description: "<<t<<"\n";
  }
  *pFile<<"//-----------------------------------------------------------"
            "-----------------\n";
}


void HDetParAsciiFileIo::readComment(const Char_t* lastBuf, HParSet* pPar) {
  // reads the author or the description
  const Int_t maxbuf=4000;
  Text_t buf[maxbuf];
  TString s=lastBuf;
  TString pName, pVal;
  Ssiz_t n=s.First(':');
  Ssiz_t m=s.Last('\\');
  pName=s(0,n);
  if (m>0) pVal=s(n+1,m-n-1);
  else pVal=s(n+1,s.Length()-n-1);
  pVal=pVal.Strip(pVal.kLeading);
  while (m>0) {
    pVal+="\n";
    pFile->getline(buf,maxbuf);
    if (buf[0]!='/') {
      s=buf;
      m=s.Last('\\');
      if (m>0) pVal+=s(0,m);
      else pVal+=s;
    }
  }
  if (pName.CompareTo("author")==0) pPar->setAuthor(pVal.Data());
  else if ( pName.CompareTo("description")==0) pPar->setDescription(pVal.Data());
}

void HDetParAsciiFileIo::writeComment(HParSet* pPar) {
  TString t=pPar->getAuthor();
  if (!t.IsNull()) *pFile<<"author:      "<<t<<"\n";
  t=pPar->getDescription();
  if (!t.IsNull()) {
    t=t.ReplaceAll("\n"," \\\n");
    *pFile<<"description: "<<t<<"\n";
  }
}

Int_t HDetParAsciiFileIo::writeFile(HDetGeomPar* pPar) {
  if (pFile) {
    Text_t buf[4000];
    const Text_t* detName=pPar->getDetectorName();
    Text_t com1[]=
        "# Parameters describing the geometry of the ";
    Text_t com2[]="\n"
        "#\n"
        "# Format:\n"
        "#   [";
    Text_t com3[]="GeomLabPositions] lab positions of the modules:\n"
        "#      name of volume   name of reference module\n"
        "#      rotation matrix\n"
        "#      translation vector\n"
        "#\n"
        "#   [";
    Text_t com4[]="GeomVolumes] parameters of the volumes:\n"
        "#      name of volume   shape   name of mother\n"
        "#      x, y and z components of points \n"
        "#      rotation matrix\n"
        "#      translation vector\n"
        "#\n";
    pFile->write(sepLine,strlen(sepLine));
    sprintf(buf,"%s%s%s%s%s%s%s",
         com1,detName,com2,detName,com3,detName,com4);
    pFile->write(buf,strlen(buf));
    pFile->write(sepLine,strlen(sepLine));
    sprintf(buf,"[%s%s]\n",detName,"GeomLabPositions");
    pFile->write(buf,strlen(buf));
    writeComment(pPar);
    const Char_t* sl="// ------------------------------------------------------\n";
    for(Int_t m=0;m<pPar->getNumModules();m++) {
      HModGeomPar* mod=pPar->getModule(m);
      if (mod) {
        pFile->write(sl,strlen(sl));
        sprintf(buf,"%s  %s\n",mod->GetName(),mod->getRefName());
        pFile->write(buf,strlen(buf));
        writeTransform(mod->getLabTransform());
      }
    }
    pFile->write(sepLine,strlen(sepLine));
    sprintf(buf,"[%s%s]\n",detName,"GeomVolumes");
    pFile->write(buf,strlen(buf));
    HGeomShapes* shapes=pPar->getShapes();
    for(Int_t m=0;m<pPar->getNumRefModules();m++) {
      HGeomCompositeVolume* mod=pPar->getRefVolume(m);
      if (mod) {
        writeVolume(mod,shapes);
        for(Int_t c=0;c<mod->getNumComponents();c++) {
          HGeomVolume* vol=mod->getComponent(c);
          if (vol&&strlen(vol->GetName())>0) writeVolume(vol,shapes);
        }
      }
    }
    pFile->write(sepLine,strlen(sepLine));
    pPar->setChanged(kFALSE);
    return 1;
  }
  cerr<<"Output is not writable"<<endl;
  return -1;
}  


Bool_t HDetParAsciiFileIo::read(HDetGeomPar* pPar,Int_t* set) {
  if (!pFile) return kFALSE;
  pFile->clear();
  pFile->seekg(0,ios::beg);
  const Char_t* detName=pPar->getDetectorName();
  TString key1(detName);
  key1+="GeomLabPositions";
  Bool_t allFound=readLabPositions(key1,pPar,set,pPar->getMaxSectors(),
                                   pPar->getMaxModules());
  if (pPar->isFirstInitialization() && allFound) {
    TString key2(detName);
    key2+="GeomVolumes";
    if (readVolumes(key2,pPar)==kFALSE) allFound=kFALSE;
  }
  if (allFound) {
    pPar->setInputVersion(1,inputNumber);
    pPar->setChanged();
    pPar->setNotFirstInit();
    printf("%s initialized from Ascii file\n",pPar->GetName());
  } else pPar->setInputVersion(-1,inputNumber);
  return allFound;  
}


Bool_t HDetParAsciiFileIo::readLabPositions(const Text_t* keyName,
            HDetGeomPar* pPar,Int_t* set, Int_t maxSec, Int_t maxMod) {
  // reads the lab positions of all modules
  if (!findContainer(keyName)) return kFALSE;
  const Int_t maxbuf=4000;
  Text_t buf[maxbuf], name[10], ref[10];
  Int_t s=-1,m,n;
  HGeomTransform transform;
  Bool_t all=kFALSE;
  while (!all && !pFile->eof()) {             
    pFile->getline(buf,maxbuf);
    if (buf[0]=='#') all=kTRUE;
    else {
      if (buf[0]!='/') {
        if (strstr(buf,":")) readComment(buf,pPar);
        else {
          sscanf(buf,"%s%s",name,ref);
          readTransform(transform);
          if (maxSec>0) s=pPar->getSecNum(name);
          m=pPar->getModNumInMod(name);
          if (maxSec>0) n=s*maxMod+m;
          else n=m;
          HModGeomPar* mod=0;
          if (set[n] && (mod=pPar->getModule(s,m))) {
            mod->getLabTransform().setTransform(transform);
            if (pPar->isFirstInitialization()) {
              mod->SetName(name);
              mod->setRefName(ref);
              Int_t mr=pPar->getModNumInMod(ref);
              HGeomCompositeVolume* refMod=pPar->getRefVolume(mr);
              if (refMod==0) {
                Int_t num=pPar->getNumComponents();
                refMod=new HGeomCompositeVolume(num);
                refMod->SetName(ref);
                pPar->addRefVolume(refMod,mr);
              }
              mod->setVolume(refMod);
            }
            set[n]=999;
          }
        }
      }
    }
  }
  Int_t setSize=0;
  if (maxSec>0) setSize=maxSec*maxMod;
  else setSize=maxMod;
  return checkAllFound(set,setSize);
}


Bool_t HDetParAsciiFileIo::readVolumes(const Text_t* keyName,HDetGeomPar* pPar) {
  // reads the geometry parameters of all volumes
  if (!findContainer(keyName)) return kFALSE;
  const Int_t maxbuf=4000;
  Text_t buf[maxbuf], name[10], mother[10], shape[10];
  HGeomVolume volume;
  HGeomShapes* shapes=pPar->getShapes();
  Bool_t all=kFALSE;
  while (!all && !pFile->eof()) {             
    pFile->getline(buf,maxbuf);
    if (buf[0]=='#') all=kTRUE;
    else {
      if (buf[0]!='/') {
        sscanf(buf,"%s%s%s",name,shape,mother);
        TString s(shape);
        s.ToUpper();
        if (s.Length()==3) s.Append(' ');
        volume.setShape(s);
        volume.setMother(mother);
        Int_t nPoints=shapes->readPoints(pFile,&volume);
        if (nPoints==0) return kFALSE;
        HGeomTransform& t=volume.getTransform();
        readTransform(t);
        Int_t m, l;
        if (strcmp(mother,"CAVE")==0) {
          m=pPar->getModNumInMod(name);
          l=-1;
        } 
        else {
          m=pPar->getModNumInComp(name);
          l=pPar->getCompNum(name);
        }
        HGeomCompositeVolume* volu=pPar->getRefVolume(m);
        if (volu) {
          if (l==-1) {
            if (volu->getNumPoints()==0) volu->setVolumePar(volume);
          } else {
            HGeomVolume* v=volu->getComponent(l);
            if (v->getNumPoints()==0) {
              v->SetName(name);
              v->setVolumePar(volume);
            } 
          }
        }
      }
    }
  }
  Bool_t allFound=kTRUE;
  HGeomCompositeVolume* v;
  for(Int_t i=0;i<pPar->getNumRefModules();i++) {
    v=pPar->getRefVolume(i);
    if (v && v->getNumPoints()==0) allFound=kFALSE;
  }
  return allFound;
}


void HDetParAsciiFileIo::readTransform(HGeomTransform& tf) {
  const Int_t maxbuf=4000;
  Text_t buf[maxbuf];
  Double_t r[9], t[3];
  pFile->getline(buf,maxbuf);
  sscanf(buf,"%lf%lf%lf%lf%lf%lf%lf%lf%lf",&r[0],&r[1],&r[2],
         &r[3],&r[4],&r[5],&r[6],&r[7],&r[8]);
  pFile->getline(buf,maxbuf);
  sscanf(buf,"%lf%lf%lf",&t[0],&t[1],&t[2]);
  tf.setRotMatrix(r);
  tf.setTransVector(t);
}


void HDetParAsciiFileIo::writeTransform(const HGeomTransform& tf) {
  Text_t buf[155];
  const HGeomRotation& r=tf.getRotMatrix();
  sprintf(buf,"%10.7f%11.7f%11.7f%11.7f%11.7f%11.7f%11.7f%11.7f%11.7f\n",
            r(0),r(1),r(2),r(3),r(4),r(5),r(6),r(7),r(8));
  pFile->write(buf,strlen(buf));
  const HGeomVector& v=tf.getTransVector();
  sprintf(buf,"%9.3f%10.3f%10.3f\n",v(0),v(1),v(2));
  pFile->write(buf,strlen(buf));
}


Bool_t HDetParAsciiFileIo::readVolume(HGeomVolume* volu,HGeomShapes* shapes,
                                    Text_t* buf) {
  Text_t name[10], mother[10], shape[10];
  sscanf(buf,"%s%s%s",name,shape,mother);
  volu->SetName(name);
  volu->setMother(mother);
  TString s(shape);
  s.ToUpper();
  if (s.Length()==3) s.Append(' ');
  volu->setShape(s);
  Int_t nPoints=shapes->readPoints(pFile,volu);
  if (nPoints==0) return kFALSE;
  HGeomTransform& t=volu->getTransform();
  readTransform(t);
  return kTRUE;
}


void HDetParAsciiFileIo::writeVolume(HGeomVolume* volu,HGeomShapes* shapes) {
  Text_t buf[120];
  sprintf(buf,"%s\n",
          "// ------------------------------------------------------");
  pFile->write(buf,strlen(buf));
  sprintf(buf,"%s  %s  %s\n",volu->GetName(),
            (const Char_t*)volu->getShape(),(const Char_t*)volu->getMother());
  pFile->write(buf,strlen(buf));
  shapes->writePoints(pFile,volu);
  writeTransform(volu->getTransform());
}


Bool_t HDetParAsciiFileIo::read(HTrb3Calpar* pPar) {
  if (!findContainer(pPar->GetName())) return kFALSE;
  pPar->clear();
  const Int_t maxbuf = 155;
  Text_t buf[maxbuf], key1[30], key2[30], key3[30], key4[30], key5[30];
  const Char_t delim[]=" ";
  Int_t address=0, subevid=0, nChan=0, nBin=0, nData=0, nEdges;
  Bool_t rc = kTRUE;
  while (!pFile->eof() && rc) {
    pFile->getline(buf, maxbuf);
    if (buf[0] == '#') break;
    if (buf[0] != '/' && buf[0] != '\0') {
      if (strstr(buf,"trbnet-address")) {
        Int_t n = sscanf(buf,"%s 0x%x %s 0x%x %s %i %s %i %s %i",key1,&address,key2,&subevid,key3,&nEdges,key4,&nChan,key5,&nBin);
        if (n != 10) {
          Error("read(HTrb3Calpar*)","Invalid data in line %s\n", buf);
          rc = kFALSE;
          break;
        }
        HTrb3CalparTdc* tdc = pPar->addTdc(address);
        nData = tdc->makeArray(subevid,nEdges,nChan,nBin);
        Float_t* arr = tdc->getBinsPar();
        Int_t l = 0;
        if (nData>0 && arr) {
          do {
            pFile->getline(buf,maxbuf);
            if (strstr(buf,"trbnet-address")) {
              rc = kFALSE;
              break;
            }
            Char_t* ss=strtok(buf,delim);
            while (ss!=0 && l<nData) {
              sscanf(ss,"%f",&arr[l]);
              l++;
              ss=strtok(NULL,delim);
            }
          } while (l<nData && buf[0]!='#' && buf[0]!='/' && !pFile->eof());
        }
        if (l<nData) {
          Error("read(HTrb3Calpar*)",
                "Not enough data for trbnet_address 0x%x\n", address );
          rc = kFALSE;
        }
      }
    }
  }
  if (rc && address > 0) {
    pPar->setInputVersion(1, inputNumber);
    pPar->setChanged();
    printf("%s initialized from Ascii file\n", pPar->GetName());
    return kTRUE;
  }
  return kFALSE;
}
