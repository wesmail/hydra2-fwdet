#include "hhistconverter.h"


#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include <iostream>
#include <iomanip>
using namespace std;

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////
// HHistConverter
//
// Stores/reads Histograms in linear TArrayD. Supports
// TH1D, TH2D, TH3D and non constant binning.
// Helper class for using histograms in HParCond
// containers. Only array data will be stored to ORACLE.
// The Data can therefore be displayed in ORACLE or ASCII
// file format.
// To fill the Array linData one can fill first Histograms
// and convert them using the function
//
// static void  HHistConverter::fillArray(const TH1* h,TArrayD& linData,TString name,Int_t nvals,Int_t width, Bool_t print)
//
// where name (lable), nvals (number of displayed values /row),
// width (formating digits for the fields in row and print
// are options to display the result of the convertion.
//
// From an existing array a Histogram cand be constructed with
//
// static TH1* HHistConverter::createHist(TString name,const TArrayD& linData)
//
// The created histogram with name "name" is dispatched from the
// directory (hist->SetDirectory(0)).
// The Array has the following structure:
//
// array[0]                    = dimension of Histogram (1-3)
// array[1]                    = n Bins in x
// array[2]                    = n Bins in y (0 if 1 dim)
// array[3]                    = n Bins in z (0 if 1 or 2 dim)
// array[4 - 4 + nbinsx + 1]   = x axis  (nBinx+1 values)
// array[....]                 = y axis  (if dim > 1, nBiny+1 values)
// array[....]                 = z axis  (if dim > 2, nBinz+1 values)
// array[....]                 = data of histcontent linearized form
//
// linearization (bin starting from 0):
//                  1 dim     ind = xbin
//                  2 dim     ind = xbin * nBiny + ybin
//                  3 dim     ind = xbin * nBiny * nBinz + ybin * nBinz + zbin
//
//
///////////////////////////////////////////////////////////

ClassImp(HHistConverter)


HHistConverter::HHistConverter()
{
}

HHistConverter::~HHistConverter()
{

}

void HHistConverter::printArray(const TArray& dat,Int_t nvals,Int_t width,Int_t start,Int_t end)
{
    // prints content of array with n values /row, each data field
    // with width digits from index start to index end (both included)

    Int_t size = dat.GetSize();
    if(start < 0 || start+1 >= size || start >= end   ) start = 0;
    if(end   < 0 || end+1   >= size || end   <= start ) end   = size;
    cout<<"        "<<flush;
    Int_t ct = 1;
    Int_t n  = end - start;
    for(Int_t i = start+1; i < end+1; i ++){
	if (ct%nvals == 0 && ct > 1 && ct != n )  { // break to new line
	    cout<<setw(width)<<dat.GetAt(i-1)<<" \\ "<<endl;
	    cout<<"        "<<flush;
	}
	else if  (ct == n )  cout<<setw(width)<<dat.GetAt(i-1)<<flush;         // end reached
	else                 cout<<setw(width)<<dat.GetAt(i-1)<<" "<<flush;   // inside line

	ct++;
    }
    cout<<endl;
}

void HHistConverter::writeArray(ostream& out,TString name,const TArray& dat,Int_t nvals)
{
    // write content of array with n values /row to stream out.
    // A lable name and and type specifier is added. Format is
    // compatible to HParCond ascii file format.

    Int_t maxbin = dat.GetSize();
    out<<name.Data()<<": Double_t \\"<<endl;
    out<<"        "<<flush;
    for(Int_t i = 1; i < maxbin+1; i ++){
	if ( (i%(nvals) == 0) && i > 1 && i != maxbin )  {   // break to new line
	    out<<dat.GetAt(i-1)<<" \\ "<<endl;
	    out<<"        "<<flush;
	}
	else if                          (i == maxbin )  {out<<dat.GetAt(i-1)<<flush; }        // end reached
	else                                             {out<<dat.GetAt(i-1)<<" "<<flush; }  // inside line

    }
    out<<endl;
}

void HHistConverter::printArrayInfo(const TArrayD& linData,TString name,Int_t nvals,Int_t width)
{
    // print the information of a TArrayD containing
    // linearized histogram data. Axis and data are
    // displayed with n values nvalues per/row, each
    // data field with width digits.

    Int_t dim  = (Int_t)linData[0];    // dimension of hist
    Int_t xbin = (Int_t)linData[1];    // n bins x
    Int_t ybin = (Int_t)linData[2];    // n bins y
    Int_t zbin = (Int_t)linData[3];    // n bins z

    Int_t offsetData = 0;
    Int_t offsetx    = 4;
    Int_t offsety    = offsetx + (xbin + 1);
    Int_t offsetz    = offsety + (ybin + 1);

    Int_t maxbin = 0;
    if(dim == 1) { maxbin = xbin;               offsetData = offsetx + (xbin + 1);}
    if(dim == 2) { maxbin = xbin * ybin;        offsetData = offsety + (ybin + 1);}
    if(dim == 3) { maxbin = xbin * ybin * zbin; offsetData = offsetz + (zbin + 1);}

    cout<<"---------------------------------------------------------------"<<endl;
    if(dim == 1) cout<<"info : "<<name.Data()<<", nBin = "<<xbin  <<endl;
    if(dim == 2) cout<<"info : "<<name.Data()
	<<", nBin = "<<maxbin
	    <<", nx = "  <<xbin
	    <<", ny = "  <<ybin<<", stored linear : ind = x * ybin + y"<<endl;
    if(dim == 3) cout<<"info : "<<name.Data()
	<<", nBin = "<<maxbin
	    <<", nx = "  <<xbin
	    <<", ny = "  <<ybin
	    <<", nz = "  <<zbin<<", stored linear : ind = x * (ybin*zbin) + y*zbin + z"<<endl;

    if(dim > 0){
	cout<<"x axis : "<<endl;
	printArray(linData,nvals,width,offsetx,offsetx + xbin + 1);
    }
    if(dim > 1) {
	cout<<"y axis : "<<endl;
	printArray(linData,nvals,width,offsety,offsety + ybin + 1);
    }
    if(dim == 3) {
	cout<<"z axis : "<<endl;
	printArray(linData,nvals,width,offsetz,offsetz + zbin + 1);
    }
    cout<<"data : "<<endl;
    printArray(linData,nvals,width,offsetData,offsetData + maxbin);
}


TH1* HHistConverter::createHist(TString name,const TArrayD& linData)
{
    // creates a histogram withname name from an TArrayD.
    // supports 1,2 and 3 dim Histograms of type Double_t.

    TH1* h = 0;
    TArrayD& linDat = (TArrayD&)linData;
    Int_t dim  = (Int_t)linData[0];    // dimension of hist
    Int_t xbin = (Int_t)linData[1];    // n bins x
    Int_t ybin = (Int_t)linData[2];    // n bins y
    Int_t zbin = (Int_t)linData[3];    // n bins z

    Int_t offsetData = 0;
    Int_t offsetx    = 4;
    Int_t offsety    = offsetx + (xbin + 1);
    Int_t offsetz    = offsety + (ybin + 1);

    if(dim == 1) { offsetData = offsetx + (xbin + 1);}
    if(dim == 2) { offsetData = offsety + (ybin + 1);}
    if(dim == 3) { offsetData = offsetz + (zbin + 1);}

    if     (dim == 1){

	h = (TH1*) new TH1D(name.Data(),name.Data(),xbin,&linDat[offsetx]);

	for(Int_t i = 0; i < xbin; i ++){
	    h->SetBinContent(i+1,linData.At(offsetData + i));
	}
    } else if(dim == 2) {
	h = (TH1*) new TH2D(name.Data(),name.Data()
			    ,xbin,&linDat[offsetx]
			    ,ybin,&linDat[offsety]
			   );
	Int_t ind;
	for(Int_t i = 0; i < xbin; i ++){
	    for(Int_t j = 0; j < ybin; j ++){
		ind = i * ybin + j;
		h->SetBinContent(i+1,j+1,linData.At(offsetData + ind) );
	    }
	}
    } else if(dim == 3) {
	Int_t ind;
	h = (TH1*) new TH3D(name.Data(),name.Data()
			    ,xbin,&linDat[offsetx]
			    ,ybin,&linDat[offsety]
			    ,zbin,&linDat[offsetz]
			   );

	for(Int_t i = 0; i < xbin; i ++){
	    for(Int_t j = 0; j < ybin; j ++){
		for(Int_t k = 0; k < zbin; k ++){
		    ind = i * ybin*zbin + j*ybin + j;
		    h->SetBinContent(i+1,j+1,k+1,linData.At(offsetData + ind) );
		}
	    }
	}
    }
    h->SetDirectory(0);

    return h;
}
void    HHistConverter::fillArray(const TH1* h,TArrayD& linData,TString name,Int_t nvals,Int_t width, Bool_t print)
{
    // To fill the Array linData from histogram h
    // where name (lable), nvals (number of displayed values /row),
    // width (formating digits for the fields in row and print
    // are options to display the result of the convertion.

    Int_t dim  = h->GetDimension();
    Int_t xbin = h->GetNbinsX();
    Int_t ybin = h->GetNbinsY();
    Int_t zbin = h->GetNbinsZ();

    Int_t offsetData = 0;
    Int_t offsetx    = 4;
    Int_t offsety    = offsetx + (xbin + 1);
    Int_t offsetz    = offsety + (ybin + 1);

    Int_t maxbin = 0;
    if(dim == 1) { maxbin = xbin;               offsetData = offsetx + (xbin + 1); ybin = zbin = 0;}
    if(dim == 2) { maxbin = xbin * ybin;        offsetData = offsety + (ybin + 1); zbin = 0;}
    if(dim == 3) { maxbin = xbin * ybin * zbin; offsetData = offsetz + (zbin + 1);}

    linData.Set(offsetData + maxbin);

    linData[0] = dim;
    linData[1] = xbin;
    linData[2] = ybin;
    linData[3] = zbin;

    //-----------------------------------------------------------------------------
    // Filling axis
    for(Int_t i = 0; i < xbin; i ++){
	linData.SetAt(h->GetXaxis()->GetBinLowEdge(i + 1),offsetx + i);
    }
    linData.SetAt(h->GetXaxis()->GetBinUpEdge(xbin),offsetx + xbin);


    if(dim >= 2){
	for(Int_t i = 0; i < ybin; i ++){
	    linData.SetAt(h->GetYaxis()->GetBinLowEdge(i + 1),offsety + i);
	}
	linData.SetAt(h->GetYaxis()->GetBinUpEdge(ybin),offsety + ybin);
    }
    if(dim == 3){
	for(Int_t i = 0; i < zbin; i ++){
	    linData.SetAt(h->GetZaxis()->GetBinLowEdge(i + 1),offsetz + i);
	}
	linData.SetAt(h->GetZaxis()->GetBinUpEdge(zbin),offsetz + zbin);
    }

    //-----------------------------------------------------------------------------
    // Filling data
    if(dim == 1){
	for(Int_t i = 0; i < xbin; i ++){
	    linData.SetAt(h->GetBinContent(i + 1),offsetData + i);
	}
    } else if (dim == 2){
	Int_t ind;

	for(Int_t i = 0; i < xbin; i ++){
	    for(Int_t j = 0; j < ybin; j ++){
		ind = i * ybin + j;
		linData.SetAt(h->GetBinContent(i + 1,j + 1),offsetData + ind);
	    }
	}
    } else if (dim == 3){
	Int_t ind;

	for(Int_t i = 0; i < xbin; i ++){
	    for(Int_t j = 0; j < ybin; j ++){
		for(Int_t k = 0; k < zbin; k ++){
		    ind = i * ybin * zbin + j * ybin + k;
		    linData.SetAt(h->GetBinContent(i + 1,j + 1,k + 1),offsetData + ind);
		}
	    }
	}
    }
    if(print){
	printArrayInfo(linData,name,nvals,width);
    }

}
