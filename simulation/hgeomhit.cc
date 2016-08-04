//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomHit
//
// Base class for hit definition in GEANT
//
/////////////////////////////////////////////////////////////

#include "hgeomhit.h"
#include "hgeomset.h"
#include "hgeomnode.h"
#include "TString.h"
#include <ctype.h>

ClassImp(HGeomHit)

HGeomHit::HGeomHit(HGeomSet* p) {
  // Non-public constructor
  pSet=p;
  currentNode=0;
  nh=0;
  chnamh=0;
  nbitsh=0;
  orig=0;
  fact=0;
  nv=0;
  chnmsv=0;
  nbitsv=0;
  copies=0;
}

HGeomHit::~HGeomHit() {
  // Destructor
  if (chnamh) {
    delete [] chnamh;
    chnamh=0;
  }
  if (nbitsh) {
    delete [] nbitsh;
    nbitsh=0;
  }
  if (orig) {
    delete [] orig;
    orig=0;
  }
  if (fact) {
    delete [] fact;
    fact=0;
  }
  if (chnmsv) {
    delete [] chnmsv;
    chnmsv=0;
  }
  if (nbitsv) {
    delete [] nbitsv;
    nbitsv=0;
  }
  if (copies) {
    copies->Delete();
    delete copies;
    copies=0;
  }
}

const Char_t* HGeomHit::getDetectorName() {
  // Returns the name of the corresponding detector
  return pSet->GetName();
}

void HGeomHit::read(fstream& fin) {
  // Reads the hit definition from file
  const Int_t maxbuf=256;
  Char_t buf[maxbuf];
  TString tmp;
  while(!fin.eof()) {
    fin>>tmp;
    tmp=tmp.Strip(tmp.kBoth);
    if (tmp.IsNull() || tmp.BeginsWith("/")) fin.getline(buf,maxbuf);
    else {
      fName=tmp(0,4);
      fin>>nh;
      if (nh>0) {
        chnamh=new char[nh*5];
        nbitsh=new Int_t[nh];
        orig=new Float_t[nh];
        fact=new Float_t[nh];
        for(Int_t i=0;i<nh;i++) {
          fin>>tmp;
          tmp.Resize(4);
          strcpy(&chnamh[i*5],tmp.Data());
        }
        for(Int_t i=0;i<nh;i++) fin>>nbitsh[i];
        for(Int_t i=0;i<nh;i++) fin>>orig[i];
        for(Int_t i=0;i<nh;i++) fin>>fact[i];
      }
    }
  }
}


void HGeomHit::write(fstream& fout) {
  // Writes the hit definition to file
  fout<<fName<<'\n';
  fout<<nh<<'\n';  
  if (nh>0) {
      fout.precision(2);
      fout.setf(ios::fixed,ios::floatfield);
    for(Int_t i=0;i<nh;i++) fout<<setw(8)<<right<<&chnamh[i*5]<<" ";
    fout<<endl;
    for(Int_t i=0;i<nh;i++) fout<<setw(8)<<right<<nbitsh[i]<<" ";
    fout<<endl;
    for(Int_t i=0;i<nh;i++) fout<<setw(8)<<right<<orig[i]<<" ";
    fout<<endl;
    fout.setf(ios::scientific,ios::floatfield);
    fout.precision(1);
    for(Int_t i=0;i<nh;i++) fout<<setw(8)<<right<<fact[i]<<" ";
    fout<<endl;
  }
}

void HGeomHit::print() {
  // Prints the hit definition
  cout<<fName<<'\n';
  cout<<nh<<'\n';  
  if (nh>0) {
    for(Int_t i=0;i<nh;i++) cout<<setiosflags(ios::fixed)<<setprecision(2)<<setw(8)<<right<<&chnamh[i*5]<<" ";
    cout<<endl;
    for(Int_t i=0;i<nh;i++) cout<<setiosflags(ios::fixed)<<setprecision(2)<<setw(8)<<right<<nbitsh[i]<<" ";
    cout<<endl;
    for(Int_t i=0;i<nh;i++) cout<<setiosflags(ios::fixed)<<setprecision(2)<<setw(8)<<right<<orig[i]<<" ";
    cout<<endl;
    for(Int_t i=0;i<nh;i++) cout<<setiosflags(ios::scientific|ios::floatfield)<<setprecision(1)<<setw(8)<<right<<fact[i]<<" ";
    cout<<endl;
  }
  if (nv>0) {
    cout<<"Hits for sensitive volume: "<<currentNode->GetName()
        <<"   idtype: "<<getIdType()<<"   nv: "<<nv<<endl;
    for(Int_t i=0;i<nv;i++) cout<<i<<"  "<<&chnmsv[i*5]<<"  "<<nbitsv[i]<<endl;
  }
}

Bool_t HGeomHit::setCurrentNode(HGeomNode* node) {
  // Sets the current node, fills chnmsv and calculates nbitsv  
  currentNode=node;
  if (chnmsv) {
    delete [] chnmsv;
    chnmsv=0;
  }
  if (nbitsv) {
    delete [] nbitsv;
    nbitsv=0;
  }
  nv=0;
  if (!currentNode||nh<=0) return kFALSE;
  strncpy(compName,currentNode->GetName(),4);
  compName[4]='\0';
  if (!copies) {
    copies=new TList;
    TListIter iter(pSet->getListOfVolumes());
    HGeomNode* vol;
    while((vol=(HGeomNode*)iter.Next())) {
      Int_t num=vol->getCopyNo();
      if (num>1) {
        TString volName(vol->GetName(),4);
        HGeomCopyNodeNum* v=(HGeomCopyNodeNum*)(copies->FindObject(volName.Data()));
        if (v) {
          if (num>v->maxCopyNum) v->maxCopyNum=num;
        } else copies->Add(new HGeomCopyNodeNum(volName,num));
      }
    }
  }
  Char_t treeNames[20][5];
  Int_t i, j, copyNo[20], nvmax=20;
  HGeomNode* pNode=currentNode;
  while (pNode&&nv<=nvmax) {
    if (nv==nvmax) {
      Error("restoreTree()",
            "number of levels in tree too large, only %i allowed!\n",nvmax);
      return kFALSE;
    }
    TString nodeName(pNode->GetName());
    if (nodeName.Length()<=4) {
      strcpy(treeNames[nv],nodeName.Data());
      copyNo[nv]=1;
    } else {
      TString s(nodeName,4);
      HGeomCopyNodeNum* v=(HGeomCopyNodeNum*)(copies->FindObject(s));
      strcpy(treeNames[nv],s.Data());
      if (v) {
        copyNo[nv]=v->maxCopyNum;
      } else {
        copyNo[nv]=1;
      }
    }
    nv++;
    if (pNode->isModule()) break;
    pNode=pNode->getMotherNode();
  }
  chnmsv=new char[nv*5];
  nbitsv=new Int_t[nv];
  for(j=0,i=nv-1;j<nv;i--,j++) strcpy(&chnmsv[j*5],treeNames[i]);
  for(j=0,i=nv-1;j<nv;i--,j++) nbitsv[j]=int(log10((Double_t)copyNo[i])/log10(2.)+0.01)+1;
  if (nv>0) return kTRUE;
  return kFALSE;
}

Bool_t HGeomHit::calcRefPos(HGeomVector& refPos,TString& shape) {
  // Calculates the position in the module reference system
  shape=currentNode->getShape();
  return currentNode->calcRefPos(refPos);
}

void HGeomHit::setNh(Int_t n) {
  // Sets the number of components
  if (chnamh) delete [] chnamh;
  if (nbitsh) delete [] nbitsh;
  if (orig)   delete [] orig;
  if (fact)   delete [] fact;
  nh=n;
  if (n>0) {
    chnamh=new char[nh*5];
    nbitsh=new Int_t[nh];
    orig=new Float_t[nh];
    fact=new Float_t[nh];
  }
}

void HGeomHit::fill(Int_t i,const Char_t* cn,Int_t cb,Float_t co,Float_t cf) {
  // Sets the ith component 
  if (i<nh) {
    TString tmp=cn;
    tmp.Resize(4);
    strcpy(&chnamh[i*5],tmp.Data());
    nbitsh[i]=cb;
    orig[i]=co;
    fact[i]=cf;
  }
}
