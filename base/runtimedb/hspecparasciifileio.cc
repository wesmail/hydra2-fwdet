//*-- AUTHOR : Ilse Koenig
//*-- modified : 06/12/2009 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
// HSpecParAsciiFileIo
//
// Class for parameter input/output from/into Ascii file for
// the spectrometer (not related to a special detector)
//
/////////////////////////////////////////////////////////////

#include "hspecparasciifileio.h"
#include "hgeomvolume.h"
#include "hgeomshapes.h"
#include "hspecgeompar.h"
#include "htrblookup.h"
#include "htrbnetaddressmapping.h"
#include "htrb2correction.h"
#include "hslowpar.h"

ClassImp(HSpecParAsciiFileIo)

HSpecParAsciiFileIo::HSpecParAsciiFileIo(fstream* f) : HDetParAsciiFileIo(f) {
  // constructor calls the base class constructor
  fName="HSpecParIo";
}


Bool_t HSpecParAsciiFileIo::init(HParSet* pPar,Int_t* set) {
  // calls the appropriate read function of the container
  const  Text_t* name=pPar->GetName();
  if (pFile) {
    if (strcmp(name,"SpecGeomPar")==0)
      return read((HSpecGeomPar*)pPar,set);
    if (strcmp(name,"TrbLookup")==0)
      return readFile((HTrbLookup*)pPar);
    if (strcmp(name,"TrbnetAddressMapping")==0)
      return read((HTrbnetAddressMapping*)pPar);
    if (strcmp(name,"SlowPar")==0)
      return readFile((HSlowPar*)pPar);
  }
  cerr<<"initialization of "<<name<<" not possible from file!"<<endl;
  return kFALSE;
}

Int_t HSpecParAsciiFileIo::write(HParSet* pPar) {
  // calls the appropriate write function of the container
  const  Text_t* name=pPar->GetName();
  if (pFile) {
    if (strcmp(name,"SpecGeomPar")==0)
      return writeGeom((HSpecGeomPar*)pPar);
    if (strcmp(name,"TrbLookup")==0)
      return writeFile((HTrbLookup*)pPar);
    if (strcmp(name,"TrbnetAddressMapping")==0)
      return writeFile((HTrbnetAddressMapping*)pPar);
    if (strcmp(name,"SlowPar")==0)
      return writeFile((HSlowPar*)pPar);
  }
  cerr<<name<<" could not be written to Ascii file"<<endl;
  return kFALSE;
}


Bool_t HSpecParAsciiFileIo::read(HSpecGeomPar* pPar,Int_t* set) {
  // Reads the geometry parameter container SpecGeomPar from ASCII file
  if (!pFile) return kFALSE;
  TString key[3]={"GeomParCave","GeomParSectors","GeomParTargets"};
  Int_t n1=0;
  if (!pPar->isFirstInitialization()) n1=2;
  Bool_t allFound=kTRUE;
  const Int_t maxbuf=1550;
  Text_t buf[maxbuf];
  HGeomVolume volu;
  HGeomTransform transform;
  HGeomShapes* shapes=pPar->getShapes();
  for(Int_t i=n1;i<3;i++) {
    pFile->clear();
    pFile->seekg(0,ios::beg);
    Int_t k=0;
    Bool_t all=kFALSE;
    while (!all && !pFile->eof()) {
      pFile->getline(buf,maxbuf);
      if( (buf[0]=='[')) {
        if (strstr(buf,(const Char_t*)key[i])!=0) {
          while (!all && !pFile->eof()) {
            pFile->getline(buf,maxbuf);
            if (buf[0]=='#') all=kTRUE;   
            else {
              if (buf[0]=='/') continue;
              if (strstr(buf,":")) readComment(buf,pPar);
              else {
                if (readVolume(&volu,shapes,buf)==kFALSE) return kFALSE;
                if (set[i]) {
                  switch(i) {
	            case 0: { pPar->addCave(&volu); break; }
	            case 1: { pPar->addSector(&volu); break; }
	            case 2: { pPar->addTarget(&volu); break; }
                  }
                  k++;
                } 
              }
	    }
          }
        }
      }
    }
    if (k) set[i]=0;
    else allFound=kFALSE;
  }
  if (allFound) {
    pPar->setInputVersion(1,inputNumber);
    pPar->setChanged();
    pPar->setNotFirstInit();
    printf("%s initialized from Ascii file\n",pPar->GetName());
  }
  return allFound;  
}


Int_t HSpecParAsciiFileIo::writeGeom(HSpecGeomPar* pPar) {
  // Writes the geometry parameter container SpecGeomPar to the ASCII file
  if (pFile) {
    Text_t buf[1000];
    Text_t com1[]=
        "#########################################################\n"
        "# Parameters describing the geometry of the cave,\n"
        "# the sector(s) and the target(s)\n"
        "#\n"
        "#   [GeomParCave] parameters of the cave:\n"
        "#      name of volume   shape\n";
    Text_t com2[]=
        "#   [GeomParSectors] parameters of the sectors:\n"
        "#      name of volume   shape   name of mother\n";
    Text_t com3[]=
        "#   [GeomParTargets] parameters of the targets:\n"
        "#      name of volume   shape   name of mother\n";
    Text_t com4[]=
        "#      x, y and z components of points \n"
        "#      rotation matrix\n"
        "#      translation vector\n"
        "#\n";
    Text_t com5[]=
        "#########################################################\n";
    sprintf(buf,"%s%s%s",
         com1,com4,com5);
    pFile->write(buf,strlen(buf));
    sprintf(buf,"[GeomParCave]\n");
    pFile->write(buf,strlen(buf));
    writeComment(pPar);
    HGeomShapes* shapes=pPar->getShapes();
    HGeomVolume* volu=pPar->getCave();
    if (volu) writeVolume(volu,shapes);
    sprintf(buf,"%s%s%s%s[GeomParSectors]\n",com5,com2,com4,com5);
    pFile->write(buf,strlen(buf));
    for(Int_t i=0;i<pPar->getNumSectors();i++) {
      volu=pPar->getSector(i);
      if (volu) writeVolume(volu,shapes);
    }
    sprintf(buf,"%s%s%s%s[GeomParTargets]\n",com5,com3,com4,com5);
    pFile->write(buf,strlen(buf));
    for(Int_t i=0;i<pPar->getNumTargets();i++) {
      volu=pPar->getTarget(i);
      if (volu) writeVolume(volu,shapes);
    }
    pFile->write(com5,strlen(com5));
    pPar->setChanged(kFALSE);
    return 1;
  }
  cerr<<"Output is not writable"<<endl;
  return -1;
}

Bool_t HSpecParAsciiFileIo::read(HTrbnetAddressMapping* pPar) {
  if (!findContainer(pPar->GetName())) return kFALSE;
  pPar->clear();
  const Int_t maxbuf=155;
  Text_t buf[maxbuf], key[80], tempSensor[80];
  Bool_t rc=kTRUE;
  const Char_t delim[]=" ";
  HTrb2Correction* pTrb=0;
  Int_t nData=0, nChan=0, nChannels=0;
  Float_t* corrData=0;
  while (!pFile->eof()&&rc) {
    pFile->getline(buf, maxbuf);
    if (buf[0]=='#') break;
    if (buf[0]!='/' && buf[0]!='\0') {
      rc=pPar->readline(buf);
    }
  }
  if (rc&&findContainer("Trb2Corrections")) {
    while (!pFile->eof()&&rc) {
      pFile->getline(buf, maxbuf);
      if (buf[0]=='#') break;
      if (buf[0]!='/' && buf[0]!='\0') {
        if (strstr(buf,"temperatureSensor")) {
          sscanf(buf,"%s%s",key,tempSensor);
          pTrb=pPar->getBoard(tempSensor);
          if (pTrb) {
            corrData=pTrb->makeArray();
            if (corrData) {
              nData=pTrb->getSize();
              nChannels=pTrb->getNChannels();
              nChan=0;
              Int_t l=0;
              do {
                pFile->getline(buf,maxbuf);
                if (strstr(buf,"temperatureSensor")) break;
		if (strstr(buf,"channel")) {
                  nChan++;
                } else {
                  Char_t* ss=strtok(buf,delim);
                  while (ss!=0 && l<nData) {
                    sscanf(ss,"%f",&corrData[l]);
                    l++;
                    ss=strtok(NULL,delim);
                  }
                }
	      } while (l<nData && buf[0]!='#' && buf[0]!='/' && !pFile->eof());
              if (l<nData||nChan<nChannels) {
                Warning("read(HTrbnetAddressMapping*)",
                      "Only %i data for temperatureSensor %s",l,tempSensor);
              }
            } else {
              rc=kFALSE;
            }
          } else {
            Warning("read(HTrbnetAddressMapping*)",
                  "Board for temperatureSensor %s not found",tempSensor);
          }
        }
      }
    }
  }
  if (rc) {
    pPar->setInputVersion(1,inputNumber);
    pPar->setChanged();
    printf("%s initialized from Ascii file\n",pPar->GetName());
  }
  return rc;
}


template<class T> Bool_t HSpecParAsciiFileIo::readFile(T* pPar) {
  // template function for parameter containers
  // searches the container in the file, reads the data line by line and
  // called the member function readline(...) of the container class
  if (!findContainer(pPar->GetName())) return kFALSE;
  pPar->clear();
  const Int_t maxbuf=400;
  Text_t buf[maxbuf];
  Bool_t rc=kTRUE;
  while (!pFile->eof()&&rc) {
    pFile->getline(buf, maxbuf);
    if (buf[0]=='#') break;
    if (buf[0]!='/' && buf[0]!='\0') rc=pPar->readline(buf);
  }
  if (rc) {
    pPar->setInputVersion(1,inputNumber);
    pPar->setChanged();
    printf("%s initialized from Ascii file\n",pPar->GetName());
  }
  return rc;
}

template<class T> Int_t HSpecParAsciiFileIo::writeFile(T* pPar) {
  // template function for parameter containers 
  // calls the function putAsciiHeader(TString&) of the parameter container,
  // writes the header and calls the function write(fstream&) of the class
  if (pFile) {
    pPar->putAsciiHeader(fHeader);
    writeHeader(pPar->GetName(),pPar->getParamContext());
    pPar->write(*pFile);
    pFile->write(sepLine,strlen(sepLine));
    pPar->setChanged(kFALSE);
    return 1;
  }
  Error("writeFile(T*)","Output is not writable");
  return -1;
}
