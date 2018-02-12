#ifndef HRICH700HISTMANAGER_H_
#define HRICH700HISTMANAGER_H_

#include "TObject.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <functional>

class TFile;
class TNamed;

using std::map;
using std::make_pair;
using std::string;
using std::vector;

class HRich700HistManager : public TObject
{
public:

   HRich700HistManager();

   virtual ~HRich700HistManager();

   void Add( const string& name,TNamed* object);

   template<class T> void Create1(const string& name,
				  const string& title,
				  Int_t nofBins,
				  Double_t minBin,
				  Double_t maxBin)
   {
       // Helper function for creation of 1-dimensional histograms and profiles.
       // Template argument is a real object type that has to be created, for example,
       //  Create1<TH1F>("name", "title", 100, 0, 100);
       // name Object name.
       // title Object title.
       // nofBins Number of bins.
       // minBin Low axis limit.
       //maxBin Upper axis limit.

       T* h = new T(name.c_str(), title.c_str(), nofBins, minBin, maxBin);
       Add(name, h);
   };

   template<class T> void Create2(const string& name,
				  const string& title,
				  Int_t nofBinsX,
				  Double_t minBinX,
				  Double_t maxBinX,
				  Int_t nofBinsY,
				  Double_t minBinY,
				  Double_t maxBinY)
   {
       // Helper function for creation of 2-dimensional histograms and profiles.
       // Template argument is a real object type that has to be created, for example,
       // Create2<TH2F>("name", "title", 100, 0, 100, 200, 0, 200);
       // name Object name.
       //  title Object title.
       // nofBinsX Number of bins for X axis.
       // minBinX Low X axis limit.
       // maxBinX Upper X axis limit.
       // nofBinsY Number of bins for Y axis.
       // minBinY Low Y axis limit.
       //maxBinY Upper Y axis limit.

       T* h = new T(name.c_str(), title.c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
       Add(name, h);
   };

   template<class T> void Create3(const string& name,
				  const string& title,
				  Int_t nofBinsX,
				  Double_t minBinX,
				  Double_t maxBinX,
				  Int_t nofBinsY,
				  Double_t minBinY,
				  Double_t maxBinY,
				  Int_t nofBinsZ,
				  Double_t minBinZ,
				  Double_t maxBinZ)
   {
       // Helper function for creation of 3-dimensional histograms and profiles.
       // Template argument is a real object type that has to be created, for example,
       // Create3<TH3F>("name", "title", 100, 0, 100, 200, 0, 200, 300, 0, 300);
       // name Object name.
       // title Object title.
       // nofBinsX Number of bins for X axis.
       // minBinX Low X axis limit.
       // maxBinX Upper X axis limit.
       // nofBinsY Number of bins for Y axis.
       // minBinY Low Y axis limit.
       // maxBinY Upper Y axis limit.
       // nofBinsZ Number of bins for Z axis.
       // minBinZ Low Z axis limit.
       // maxBinZ Upper Z axis limit.

       T* h = new T(name.c_str(), title.c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY, nofBinsZ, minBinZ, maxBinZ);
       Add(name, h);
}

   TH1* H1( const string& name) const;
   TH2* H2( const string& name) const;
   TH3* H3( const string& name) const;
   TGraph*     G1( const string& name) const;
   TGraph2D*   G2( const string& name) const;
   TProfile*   P1( const string& name) const;
   TProfile2D* P2( const string& name) const ;

   Bool_t Exists( const string& name) const;

   void WriteToFile();
   void ReadFromFile(TFile* file);
   void AddTNamedObject(TObject* obj);
   void AddTDirectoryObject(TObject* obj);
   void Clear(Option_t*);
   void ShrinkEmptyBinsH1(const string& histName);
   void ShrinkEmptyBinsH2(const string& histName);
   void Scale( const string& histName, Double_t scale);
   void NormalizeToIntegral(const string& histName);
   void Rebin( const string& histName, Int_t ngroup);
   string ToString() const;

    TCanvas* CreateCanvas(
         const string& name,
         const string& title,
         Int_t width,
         Int_t height);

    void SaveCanvasToImage(
         const string& outputDir,
         const string& options = "png");

    // Operator << for convenient output to std::ostream.
    // return Insertion stream in order to be able to call a succession of insertion operations.

   friend std::ostream& operator<<(std::ostream& strm, const HRich700HistManager& histManager) {
      strm << histManager.ToString();
      return strm;
   }

private:

   // Map of histogram (graph) name to its pointer
   map<string, TNamed*> fMap;

   vector<TCanvas*> fCanvases; // Pointers to all created canvases

   ClassDef(HRich700HistManager, 1)
};

#endif /* HRICH700HISTMANAGER_H_ */
