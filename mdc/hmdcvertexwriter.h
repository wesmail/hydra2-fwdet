#ifndef HMDCVERTEXWRITER_H
#define HMDCVERTEXWRITER_H

#include "hreconstructor.h"
#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TSystem.h"
#include "TObjArray.h"

#include <iostream>
using namespace std;

class HMdcVertexWriter : public HReconstructor {
protected:
    TString outputdir;                // outputdirectory (optional)
    TFile*         outfile;           // pointer to root file
    TNtuple*       vertex;            // pointer to vertex ntuple
    TString        filename;          //
    Bool_t         fileNameFromInput; // flag to remember if filename was created from input or outputfile
    Bool_t         fileNameManual;    // outputfile set by hand
    Bool_t         isEmbedding;       // flag to check if embedding mode is on
    static Bool_t  doSkipNoVertex;    // == kTRUE skip no vertex even if it is not calculated (default: KFALSE)
    static Int_t   vertextype;        // kVertexCluster= 1, kVertexSegment = 2 (segments), kVertexParticle = 3 , kVertexClustMeanXY = 4 , kVertexGeant = 5
    static Double_t fMeanX;           // mean position of Vertex in x direction (used in vertextype = 4)
    static Double_t fMeanY;           // mean position of Vertex in y direction (used in vertextype = 4)
    Bool_t (*pUserSelectEvent)(TObjArray* ); //! user provided function pointer to lepton selection
    TObjArray*     pUserParams;        // user params for event selection
public:
  HMdcVertexWriter(void);
  HMdcVertexWriter(const Text_t* name,const Text_t* title);
  ~HMdcVertexWriter(void);
  Bool_t init(void);
  Bool_t finalize(void);
  Int_t  execute(void);
  static void setSkipNoVertex(Bool_t skip = kTRUE){ doSkipNoVertex = skip; }
  static void setVertexType(Int_t type , Double_t meanx=-1000.,Double_t meany=-1000.)
  {
      if(type>0&&type<=5) vertextype = type;
      else cout<<"Error : setVertexType() , Unknown vertextype "<<type<<"! Will be ignored."<<endl;
      fMeanX=meanx;
      fMeanY=meany;

  }
  void setOutputDir(TString dir = "")      { outputdir = dir;       }
  void setOutputFile(TString file = "")    { filename = gSystem->BaseName(file.Data()); outputdir = gSystem->DirName(file.Data());  fileNameManual = kTRUE ;}
  void setUserEventSelection(Bool_t (*function)(TObjArray* ),TObjArray* params=0){ pUserSelectEvent = function; pUserParams=params;}
  ClassDef(HMdcVertexWriter,0) // Writes the event vertex pointer to ascii file
};

#endif /* !HMDCVERTEXWRITER_H */

