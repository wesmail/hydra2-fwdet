#ifndef HGEOMMEDIA_H
#define HGEOMMEDIA_H

using namespace std;
#include "TNamed.h"
#include "TList.h"

class HGeomMedium;

class HGeomMedia : public TNamed {
private:
  TList* media;           // list of media
  TString inputFile;      // name of input file or Oracle
  TString author;         // author of the media version
  TString description;    // description of the version
public:
  HGeomMedia();
  ~HGeomMedia();
  void setInputFile(const Char_t* file) { inputFile=file; }
  const Char_t* getInputFile() { return inputFile; }
  void addMedium(HGeomMedium* m);
  HGeomMedium* getMedium(const Char_t*);
  TList* getListOfMedia() {return media;}
  void read(fstream&);
  void print();
  void list();
  void write(fstream&);
  void setAuthor(TString& s) {author=s;}
  void setDescription(TString& s) {description=s;}
  TString& getAuthor() {return author;}
  TString& getDescription() {return description;}
private:
  ClassDef(HGeomMedia,0) // Class for materials and media
};

#endif /* !HGEOMMEDIA_H */
