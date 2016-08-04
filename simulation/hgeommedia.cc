//*-- AUTHOR : Ilse Koenig
//*-- Last modified : 10/11/03 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HGeomMedia
//
// Class for materials and media
//
///////////////////////////////////////////////////////////////////////////////

#include "hgeommedia.h"
#include "hgeommedium.h"
#include <iostream>
#include <iomanip>

ClassImp(HGeomMedia)

HGeomMedia::HGeomMedia() {
  // Constructor
  media=new TList();
}

HGeomMedia::~HGeomMedia() {
  // Destructor
  if (media) {
    media->Delete();
    delete media;
    media=0;
  }
}

HGeomMedium* HGeomMedia::getMedium(const Char_t* mediumName) {
  // Returns the medium with name mediumName
  return (HGeomMedium*)media->FindObject(mediumName);
}

void HGeomMedia::addMedium(HGeomMedium* m) {
  // Adds a medium to the list of media
  media->Add(m);
}

void HGeomMedia::list() {
  // Lists all media
  cout<<"********************************************************************\n";
  cout<<"List of media:\n";
  TListIter iter(media);
  HGeomMedium* medium;
  while((medium=(HGeomMedium*)iter.Next())) {
    cout<<"  "<<medium->GetName()<<"\n";
  }
}

void HGeomMedia::print() {
  // Prints the media
  if (!author.IsNull())      cout<<"//Author:      "<<author<<'\n';
  if (!description.IsNull()) cout<<"//Description: "<<description<<'\n';
  cout<<"//----------------------------------------------------------\n";
  TListIter iter(media);
  HGeomMedium* medium;
  Int_t i=0;
  while((medium=(HGeomMedium*)iter.Next())) {
    if (medium->getAutoFlag()!=0){
      medium->print();
      i++;
    }
  }
  if (i!=media->GetSize()) {
    iter.Reset();
    cout<<"//----------------------------------------------\n";
    cout<<"AUTONULL\n";
    cout<<"//----------------------------------------------\n";
    while((medium=(HGeomMedium*)iter.Next())) {
      if (medium->getAutoFlag()==0) medium->print();
    }
  }
}

void HGeomMedia::read(fstream& fin) {
  // Reads the media from file
  cout<<"Read media"<<endl;
  const Int_t maxBuf=256;
  Char_t buf[maxBuf];
  Int_t autoflag=1;
  while(!fin.eof()) {
    fin>>buf;
    if (buf[0]=='\0' || buf[0]=='/') fin.getline(buf,maxBuf);
    else if (fin.eof()) break;
    else {
      TString eleName(buf);
      if (eleName.CompareTo("AUTONULL")!=0) {
        HGeomMedium* medium=new HGeomMedium(eleName);
        medium->read(fin,autoflag);
        media->Add(medium); 
      } else autoflag=0;
    }
  }
}

void HGeomMedia::write(fstream& fout) {
  // Writes the media to file
  if (!author.IsNull())      fout<<"//Author:      "<<author<<'\n';
  if (!description.IsNull()) fout<<"//Description: "<<description<<'\n';
  fout<<"//----------------------------------------------------------\n";
  TListIter iter(media);
  HGeomMedium* medium;
  Int_t i=0;
  while((medium=(HGeomMedium*)iter.Next())) {
    if (medium->getAutoFlag()!=0){
      medium->write(fout);
      i++;
    }
  }
  if (i!=media->GetSize()) {
    iter.Reset();
    fout<<"//----------------------------------------------\n";
    fout<<"AUTONULL\n";
    fout<<"//----------------------------------------------\n";
    while((medium=(HGeomMedium*)iter.Next())) {
      if (medium->getAutoFlag()==0) medium->write(fout);
    }
  }
}
