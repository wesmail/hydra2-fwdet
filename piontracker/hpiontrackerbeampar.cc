#include "hpiontrackerbeampar.h"

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
// HPionTrackerBeamPar
//
// Container class for PionTracker beam parameters
//
/////////////////////////////////////////////////////////////

#include "hparamlist.h"

#include "TObjString.h"
#include "TObjArray.h"


#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
using namespace std;

ClassImp (HPionTrackerBeamPar)

HPionTrackerBeamPar::HPionTrackerBeamPar(const Char_t * name, const Char_t * title, const Char_t * context)
	: HParCond (name, title, context)
{
	clear();
}

void HPionTrackerBeamPar::clear()
{
	// clears the container
}

void HPionTrackerBeamPar::putParams(HParamList * l)
{

	// add the parameters to the list for writing
	if (!l) return;
	vector<HBeamElement>& elements = beam.getElements();
        TString all_elements="";

	for(UInt_t i=0;i<elements.size();i++){
	    all_elements+=elements[i].fName;
	    if(i<elements.size()-1)all_elements+="," ;
	}
	l->add ("elements", (Text_t*)all_elements.Data());
	l->add ("targetElementNum", ftargetElementNum);

	for(UInt_t i=0;i<elements.size();i++){
	    TArrayD all;
	    elements[i].toLinearArray(all);
	    l->add (Form("%s",elements[i].fName.Data()), all);
	}

}

Bool_t HPionTrackerBeamPar::getParams(HParamList * l)
{
	// gets the parameters from the list (read from input)
        if (!l) return kFALSE;
        Char_t line[1000];
        if (!l->fill ("elements"        , (Text_t*)line,1000))      return kFALSE;
        TString all_elements=line;
	if (!l->fill ("targetElementNum", &ftargetElementNum)) return kFALSE;
	vector<HBeamElement>& elements = beam.getElements();
	elements.clear();

	beam.setTargetElementOnly(ftargetElementNum);

	TObjArray* ar = all_elements.Tokenize(",");
	if(ar) {
	    for(Int_t i=0; i<ar->GetEntries();i++){
               TString name = ((TObjString*)ar->At(i))->GetString();
               TArrayD all;
	       if (!l->fill (name.Data(), &all)) return kFALSE;
	       HBeamElement e;
               e.fromLinearArray(all);
               e.fName = name;
	       elements.push_back(e);
	    }
	}

	if(ar) {
	    ar->Delete() ;
	    delete ar;
	}

	return kTRUE;
}

Bool_t  HPionTrackerBeamPar::initBeamLine(TString filename,Int_t targetElementNum,Bool_t debug)
{
    ftargetElementNum = targetElementNum;
    return beam.initBeamLine(filename,targetElementNum,debug);
}

void  HPionTrackerBeamPar::createAsciiFile(TString filename)
{

    ofstream fout;
    fout.open(filename.Data());

    fout<<"##############################################################################"<<endl;
    fout<<"# Class:   HPionTrackerBeamPar "<<endl;
    fout<<"# Context: PionTrackerBeamParProduction "<<endl;
    fout<<"#"<<endl;
    fout<<"##############################################################################"<<endl;
    fout<<"[PionTrackerBeamPar]"<<endl;
    fout<<"//----------------------------------------------------------------------------"<<endl;

    vector<HBeamElement>& elements = beam.getElements();
    TString all_elements="";

    for(UInt_t i=0;i<elements.size();i++){
	all_elements+=elements[i].fName;
	if(i<elements.size()-1)all_elements+="," ;
    }
    fout<<"elements: Text_t "<<all_elements.Data()<<endl;
    fout<<"targetElementNum: Int_t "<<ftargetElementNum<<endl;

    for(UInt_t i=0;i<elements.size();i++){
	TArrayD all;
	elements[i].toLinearArray(all);
	fout<<Form("%s: Double_t \\",elements[i].fName.Data())<<endl;
        Int_t ct=0;
	for(Int_t j=0;j<all.GetSize();j++){
	    if(ct==10){
		fout<<" \\"<<endl;
		ct=0;
	    }
            ct++;
            fout<<" "<<setw(10)<<all[j];
	}
        fout<<endl;
    }
    fout<<"##############################################################################"<<endl;
    fout.close();

}
void  HPionTrackerBeamPar::printParam()
{
    cout<<"##############################################################################"<<endl;
    cout<<"# Class:   HPionTrackerBeamPar "<<endl;
    cout<<"#"<<endl;
    cout<<"##############################################################################"<<endl;
    beam.printBeamLine(kTRUE);
}
