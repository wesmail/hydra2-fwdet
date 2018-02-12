//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700HistManager
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700histmanager.h"
#include "hrich700utils.h"

#include "TH1.h"
#include "TH2.h"
#include "TNamed.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TClass.h"

#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <climits>

using std::string;
using std::map;
using std::cout;
using std::endl;
using std::vector;
using std::exception;
using std::sort;

class CompareTNamedMore:
public std::binary_function<
    const TNamed*,
    const TNamed*,
    Bool_t>
{
public:
    Bool_t operator()(const TNamed* object1, const TNamed* object2) const {
	return string(object1->GetName()) > string(object2->GetName());
    }
};

HRich700HistManager::HRich700HistManager():
    fMap()
{

}

HRich700HistManager::~HRich700HistManager()
{

}


void HRich700HistManager::Add(const string& name,TNamed* object)
{
    // Add new named object to manager.
    // name Name of the object.
    // object Pointer to object.

    map<string, TNamed*>::iterator it = fMap.find(name);
    if (it != fMap.end()){
        cout << "HRich700HistManager::Add Object with name:" << name << " was already added. Set new object." << endl;
    }

    std::pair<string, TNamed*> newpair = std::make_pair(name, object);
    fMap.insert(newpair);
}
/*
template<class T> void HRich700HistManager::Create1(const string& name,
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
}
*/
/*
template<class T> void HRich700HistManager::Create2(const string& name,
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
}
*/
/*
template<class T> void HRich700HistManager::Create3(const string& name,
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
*/
TH1* HRich700HistManager::H1(const string& name) const {

    // Return pointer to TH1 histogram.
    // name Name of histogram.
    // pointer to TH1 histogram.

    if (fMap.count(name) == 0) { // Temporarily used for debugging
	std::cout << "Error: RichHistManager::H1(name): name=" << name << std::endl;
    }
    assert(fMap.count(name) != 0);
    return dynamic_cast<TH1*>(fMap.find(name)->second);
}

TH2* HRich700HistManager::H2(const string& name) const {
    // Return pointer to TH2 histogram.
    // name Name of histogram.
    // pointer to TH2 histogram.

    if (fMap.count(name) == 0) { // Temporarily used for debugging
	std::cout << "Error: RichHistManager::H2(name): name=" << name << std::endl;
    }
    assert(fMap.count(name) != 0);
    return dynamic_cast<TH2*>(fMap.find(name)->second);
}

TH3* HRich700HistManager::H3(const string& name) const {
    // Return pointer to TH3 histogram.
    // name Name of histogram.
    //pointer to TH3 histogram.

    if (fMap.count(name) == 0) { // Temporarily used for debugging
	std::cout << "Error: RichHistManager::H3(name): name=" << name << std::endl;
      }
    assert(fMap.count(name) != 0);
    return dynamic_cast<TH3*>(fMap.find(name)->second);
}

TGraph* HRich700HistManager::G1(const string& name) const {
    // Return pointer to TGraph.
    // name Name of graph.
    // pointer to TGraph.

    if (fMap.count(name) == 0) { // Temporarily used for debugging
	std::cout << "Error: RichHistManager::G1(name): name=" << name << std::endl;
    }
    assert(fMap.count(name) != 0);
    return dynamic_cast<TGraph*>(fMap.find(name)->second);
}

TGraph2D* HRich700HistManager::G2(const string& name) const {
    // Return pointer to TGraph2D.
    // name Name of graph.
    // pointer to TGraph.

    if (fMap.count(name) == 0) { // Temporarily used for debugging
	std::cout << "Error: RichHistManager::G2(name): name=" << name << std::endl;
    }
    assert(fMap.count(name) != 0);
    return dynamic_cast<TGraph2D*>(fMap.find(name)->second);
}

TProfile* HRich700HistManager::P1(const string& name) const {
    // Return pointer to TProfile.
    // name Name of profile.
    // pointer to TProfile.

    if (fMap.count(name) == 0) { // Temporarily used for debugging
	std::cout << "Error: RichHistManager::P1(name): name=" << name << std::endl;
    }
    assert(fMap.count(name) != 0);
    return dynamic_cast<TProfile*>(fMap.find(name)->second);
}

TProfile2D* HRich700HistManager::P2(const string& name) const {
    // Return pointer to TH2 histogram.
    // name Name of histogram.
    // pointer to TH1 histogram.

    if (fMap.count(name) == 0) { // Temporarily used for debugging
	std::cout << "Error: RichHistManager::P2(name): name=" << name << std::endl;
    }
    assert(fMap.count(name) != 0);
    return dynamic_cast<TProfile2D*>(fMap.find(name)->second);
}

Bool_t HRich700HistManager::Exists(const string& name) const {
    // Check existence of histogram in manager.
    // name Name of histogram.
    // return True if histogram exists in manager.

    return (fMap.count(name) == 0) ? kFALSE : kTRUE;
}

void HRich700HistManager::WriteToFile()
{
    //Write all histograms to current opened file.

    map<string, TNamed*>::iterator it;
    for (it = fMap.begin(); it != fMap.end(); it++){
	it->second->Write();
    }
}

void HRich700HistManager::ReadFromFile(
				       TFile* file)
{
    // Read histograms from file.
    // file Pointer to file with histograms.

    assert(file != NULL);
    cout << "-I- HRich700HistManager::ReadFromFile" << endl;
    TDirectory* dir = gDirectory;
    TIter nextkey(dir->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*) nextkey())) {
    	TObject* obj = key->ReadObj();
        AddTNamedObject(obj);
        AddTDirectoryObject(obj);
    }
}

void HRich700HistManager::AddTNamedObject(
				       TObject* obj)
{
    if (obj->IsA()->InheritsFrom (TH1::Class()) || obj->IsA()->InheritsFrom (TGraph::Class()) || obj->IsA()->InheritsFrom (TGraph2D::Class())) {
	    TNamed* h = (TNamed*) obj;
	    //TNamed* h1 = (TNamed*)file->Get(h->GetName());
	    Add(string(h->GetName()), h);
	}
}

void HRich700HistManager::AddTDirectoryObject(
				       TObject* obj)
{
    if (obj->IsA()->InheritsFrom (TDirectoryFile::Class())) {
        TDirectoryFile* fileDir = (TDirectoryFile*)obj;
        TIter nextkey(fileDir->GetListOfKeys());
        TKey *key2;
        while ((key2 = (TKey*) nextkey())) {
            TObject* obj2 = key2->ReadObj();
            AddTNamedObject(obj2);
            AddTDirectoryObject(obj2);
        }
    }
}

void HRich700HistManager::Clear(Option_t*)
{
    //Clear memory. Remove all histograms.
    map<string, TNamed*>::iterator it;
    for (it = fMap.begin(); it != fMap.end(); it++) {
	delete (*it).second;
    }
    fMap.clear();
}

void HRich700HistManager::ShrinkEmptyBinsH1(
					    const string& histName)
{
    // Shrink empty bins in H1.
    // histName Name of histogram.

    TH1* hist = H1(histName);
    Int_t nofBins = hist->GetNbinsX();
    Int_t minShrinkBin = INT_MIN;//std::numeric_limits<Int_t>::max();
    Int_t maxShrinkBin = INT_MAX;//std::numeric_limits<Int_t>::min();
    Bool_t isSet = kFALSE;
    for (Int_t iBin = 1; iBin <= nofBins; iBin++) {
	Double_t content = hist->GetBinContent(iBin);
	if (content != 0.) {
	    minShrinkBin = std::min(iBin, minShrinkBin);
	    maxShrinkBin = std::max(iBin, maxShrinkBin);
	    isSet = kTRUE;
	}
    }
    if (isSet) {
	hist->GetXaxis()->SetRange(minShrinkBin, maxShrinkBin);
    }
}

void HRich700HistManager::ShrinkEmptyBinsH2(
					    const string& histName)
{
    // Shrink empty bins in H2.
    // histName Name of histogram.

    TH1* hist = H2(histName);
    Int_t nofBinsX = hist->GetNbinsX();
    Int_t nofBinsY = hist->GetNbinsY();
    Int_t minShrinkBinX = INT_MAX;//std::numeric_limits<Int_t>::max();
    Int_t maxShrinkBinX = INT_MIN;//std::numeric_limits<Int_t>::min();
    Int_t minShrinkBinY = INT_MAX;//std::numeric_limits<Int_t>::max();
    Int_t maxShrinkBinY = INT_MIN;//std::numeric_limits<Int_t>::min();
    Bool_t isSet = kFALSE;
    for (Int_t iBinX = 1; iBinX <= nofBinsX; iBinX++) {
	for (Int_t iBinY = 1; iBinY <= nofBinsY; iBinY++) {
	    Double_t content = hist->GetBinContent(iBinX, iBinY);
	    if (content != 0.) {
		minShrinkBinX = std::min(iBinX, minShrinkBinX);
		maxShrinkBinX = std::max(iBinX, maxShrinkBinX);
		minShrinkBinY = std::min(iBinY, minShrinkBinY);
		maxShrinkBinY = std::max(iBinY, maxShrinkBinY);
		isSet = kTRUE;
	    }
	}
    }
    if (isSet) {
	hist->GetXaxis()->SetRange(minShrinkBinX, maxShrinkBinX);
	hist->GetYaxis()->SetRange(minShrinkBinY, maxShrinkBinY);
    }
}

void HRich700HistManager::Scale(
				const string& histName,
				Double_t scale)
{
    // Scale histogram.
    // histName Name of histogram.
    // scale Scaling factor.

    H1(histName)->Scale(scale);
}

void HRich700HistManager::NormalizeToIntegral(
					      const string& histName)
{
    // Normalize histogram to integral.
    // histName Name of histogram.

    TH1* hist = H1(histName);
    hist->Scale(1. / hist->Integral());
}


void HRich700HistManager::Rebin(
				const string& histName,
				Int_t ngroup)
{
    // Rebin histogram.
    // histName Name of histogram.
    // ngroup Rebining factor.

    TH1* hist = H1(histName);
    if (ngroup > 1) {
	hist->Rebin(ngroup);
	hist->Scale(1. / (Double_t)ngroup);
    }
}


string HRich700HistManager::ToString() const
{
    // Return string representation of class.
    // string representation of class.

    string str = "HRich700HistManager list of histograms:\n";
    map<string, TNamed*>::const_iterator it;
    for (it = fMap.begin(); it != fMap.end(); it++){
	str += it->first + "\n";
    }
    return str;
}

TCanvas* HRich700HistManager::CreateCanvas(
		const string& name,
		const string& title,
		Int_t width,
		Int_t height)
{
	TCanvas* c = new TCanvas(name.c_str(), title.c_str(), width, height);
	fCanvases.push_back(c);
	return c;
}

void HRich700HistManager::SaveCanvasToImage(
		const string& outputDir,
		const string& options)
{
	for (unsigned int i = 0; i < fCanvases.size(); i++) {
		RichUtils::SaveCanvasAsImage(fCanvases[i], outputDir, options);
	}
}

ClassImp(HRich700HistManager)
