#ifndef  __HPARTICLECUT_H__
#define  __HPARTICLECUT_H__

#include "TString.h"
#include "TNamed.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TDirectory.h"
#include "TROOT.h"


#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;


template <class T>
class HParticleCut : public TNamed {

private:
    Int_t         fCutNumber;    // a unique cut number to handle this cut
    TString       fcondition;    // formular expression for this object to cut on
    UInt_t           fmaxCut;    // how many version can be used at max
    TDirectory*     fsaveDir;    //!  remember the actual directory before strting to work (will be restored)
    vector<ULong64_t>fctFail;    // count evaluation == kFALSE
    vector<ULong64_t>fctCall;    // count all call to the evaluation
    Bool_t      fbInverseCut;    // default kFALSE , kTRUE will invert selection
    T*                    fc;    // template class pointer
    TTree*             fTree;    // mini tree on the object to cut on
    TTreeFormula*    fselect;    // formula object

public:


    HParticleCut(TString name,Int_t num,TString cond);
    ~HParticleCut();
    void          setInverse(Bool_t inv)   { fbInverseCut = inv;}
    void          setMaxCut(UInt_t max=4);
    Bool_t        eval(T* c,UInt_t version=0);
    void          print();
    void          resetCounter();
    Float_t       getCutRate(UInt_t version=0);
    UInt_t        getNCall(UInt_t version=0);
    UInt_t        getNFail(UInt_t version=0);
    T*            getClass()     { return fc; }
    TTree*        getTree()      { return fTree; }
    TTreeFormula* getFormula()   { return fselect;}
    TString       getCondition() { return fcondition;}
    ClassDef(HParticleCut,1)
};

template <class T>
HParticleCut<T>::HParticleCut(TString name,Int_t num,TString cond)
{
    fsaveDir = gDirectory;
    gROOT->cd();
    SetName(name);
    fCutNumber = num;
    fcondition = cond;
    fbInverseCut = kFALSE;
    fc = new T();
    fTree = new TTree(Form("Tree_%s_%i",name.Data(),fCutNumber),Form("Tree_%s_%i",name.Data(),fCutNumber));
    fTree->Branch(fc->ClassName(),fc->ClassName(),&fc,99);
    fselect = new TTreeFormula(Form("Selection_%s_%i",name.Data(),fCutNumber),fcondition,fTree);
    setMaxCut(4);
    fsaveDir->cd();

}

template <class T>
HParticleCut<T>::~HParticleCut()
{
    if(fselect)delete fselect;
    if(fTree)  delete fTree;
    if(fc)     delete fc;
}


template <class T>
void   HParticleCut<T>::setMaxCut(UInt_t max)
{
    // set the max number of versions of stst counters
    fmaxCut     = max;
    if(fmaxCut == 0) fmaxCut = 1;
    resetCounter();
}


template <class T>
Float_t HParticleCut<T>::getCutRate(UInt_t version)
{
    return (version <fmaxCut && fctCall[version] > 0) ? (fctFail[version]/(Float_t)fctCall[version])*100.:0;
}

template <class T>
UInt_t HParticleCut<T>::getNCall(UInt_t version)
{
    return (version <fmaxCut ) ? fctCall[version]:0;
}

template <class T>
UInt_t HParticleCut<T>::getNFail(UInt_t version)
{
    return (version <fmaxCut ) ? fctFail[version]:0;
}

template <class T>
void HParticleCut<T>::resetCounter()
{
    fctFail.clear();
    fctCall.clear();
    fctFail.assign(fmaxCut,0);
    fctCall.assign(fmaxCut,0);
}

template <class T>
Bool_t HParticleCut<T>::eval(T* c,UInt_t version)
{
    // returns kTRUE if the object fullfills the condition
    // counters for calls and failed conditions are filled
    if(version<fmaxCut){
	fctCall[version]++;
    }

    new (fc) T(*c);

    if ( (!fbInverseCut && fselect->EvalInstance(0) == 0) ||
         ( fbInverseCut && fselect->EvalInstance(0) != 0)
       )
    {
	if(version<fmaxCut)fctFail[version]++;
	return kFALSE;

    } else   { return kTRUE; }
}

template <class T>
void HParticleCut<T>::print()
{
    Int_t p = cout.precision();
    std::ios_base::fmtflags fl =cout.flags();

    cout<<"CutNumber : "<<setw(3)<<fCutNumber
	<<" name : "<<setw(20)<<GetName()
	<<", cut [%] : "<<flush;
    for(UInt_t i = 0; i < fmaxCut ; i++){ cout<<setw(5)<<fixed<<right<<setprecision(1)<<getCutRate(i)<<" "<<flush;}
    cout<<", cut = "<<fcondition <<setprecision(p)<<endl;
    cout.flags(fl);

}

#endif
