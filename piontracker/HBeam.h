#ifndef _HBEAM_H_
#define _HBEAM_H_

//#define isPluto


/////////////////////////////////////////////////////////////////////
//
// Beam line transport
//
// Author:  T. Hennino (core), J. Biernat , J.Markert
//          Pluto implementation by IF
// Written: 14.11.2013
//
/////////////////////////////////////////////////////////////////////



//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// HBeam
//
//    This class simulates a beam line. Beam particles (HBeamParticle) will be produced using
//    a certain beam profile and momentum smearing. The particle will be tranported through
//    the beam line using matrices for the single beam elements (HBeamElement). The particles
//    cand be registered in detectors along the beam line.
//
//    HBeam pionbeam;
//    pionbeam.setBeam           (HPhysicsConstants::pid("pi-"),1.3,60,60,0.0,0.0); // id, total mom [GeV], beamtube x and y size, xoff,yoff
//    pionbeam.SetBeamResolution (0.01,0.05,0.06);                                  // dpx [rad],dpy [rad],dpz [frac]  [+-] white random
//    pionbeam.setBeamProfile    (.05,0.0);                                         // sigma [mm], flatradius [mm]
//                                                                                  // sigma>0 will select the beam profile (gausian + flat top)
//                                                                                  // sigma==0, flatradius>0 will give an extended beam spot without gaussian borders
//                                                                                  // sigma==0,flatradius==0 spot like beam
//    if(!pionbeam.initBeamLine  ("pibeam_set6_mod.data",32)) return;               // transform input file and target element number
//    pionbeam.addDetector("det1", -17092.6,2,50.,50.);                             // [mm] relative to HADES 0,0,0    cutype (0,1,2), xcut[mm],ycut[mm]
//    pionbeam.addDetector("det2",  -5400.0,2,50.,50.);                             // [mm] relative to HADES 0,0,0    cutype (0,1,2), xcut[mm],ycut[mm]
//    pionbeam.addDetector("plane", -1300.0,1,60.,60.);                             // [mm] relative to HADES 0,0,0    cutype (0,1,2), xcut[mm],ycut[mm]
//
//    pionbeam.printBeamLine(kTRUE);          // kTRUE : print transform matrices in addition to name and distance
//    pionbeam.printDetectors();
//    pionbeam.printBeamProfile();
//
//
//    //-----------------------------------------------------------
//    // create particles
//    vector<HBeamParticle>& vhistory = pionbeam.newParticle();  // beam particle at : beam,det1,det2,plane.target
//    // check if particle was in acceptance (using the particle at the end of transport)
//    if(vhistory[vhistory.size()-1].fAccepted) {   ......   }
//
//    // get access to all beam elements and detectors (positions,accptance,statistics ...)
//    vector<HBeamElement>& elements  = pionbeam.getElements();
//    vector<HBeamElement>& detectors = pionbeam.getDetectors();
//
///////////////////////////////////////////////////////////////////////////////

#include "TString.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TF1.h"
#include "TMath.h"
#include "TArrayD.h"
#include "TObject.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;


class HBeamElement : public TObject {

public:

    TString  fName;          // name of the lement : default  = Element[i]
    Double_t fDistance;      // position along beam line
    Int_t    fId;            // element number
    Double_t Tij [5][5];     // first order transform
    Double_t Tijk[5][5][5];  // second order transform
    Double_t fout[5];        // particle state at element [x,tx,y,ty,dp]
    Double_t fxCut;          // cut ond x for apperture
    Double_t fyCut;          // cut ond y for apperture
    Int_t    fCutType;       // 0: no cut ,1 : radial cut (default), 2: square cut
    // square cut uses x,y , radial cut only x!
    Long64_t fCtAll ;        // count all beam particle transport
    Long64_t fCtFail;        // count if particle is out of apperture
    Bool_t   fAccepted;      // remembers the last result of check()

    HBeamElement(TString n="not_set",Double_t dist = 0, Int_t id=-1)
    {
	fName     = n;
	fDistance = dist;
	fId       = id;
	fCutType  = 1;
	fxCut     = 60;
	fyCut     = 60;
	fCtAll    = 0;
	fCtFail   = 0;
	fAccepted = kTRUE;
	clear();
    }

    void setElement(TString name,Int_t cuttype,Double_t xcut,Double_t ycut){
	if(name.CompareTo("")!=0) fName = name;
	fCutType = cuttype;
	fxCut = xcut;
        fyCut = ycut;
    }

    void clear()
    {
	for(Int_t i = 0; i < 5; i ++) {
	    fout[i] = 0;
	    for(Int_t j = 0; j < 5; j ++){
		Tij[i][j] =0;
		for(Int_t k = 0; k < 5; k ++){
		    Tijk[i][j][k] = 0;
		}
	    }
	}
    }

    void printTij()
    {
	for(Int_t i = 0; i < 5; i ++) {     // row
	    for(Int_t j = 0; j < 5; j ++){  // column
		cout<<setw(12)<<Tij[i][j]<<" "<<flush;

	    }
	    cout<<endl;
	}
    }

    void printTijk()
    {
	for(Int_t i = 0; i < 5; i ++) { // block
	    for(Int_t k = 0; k < 5; k ++){ // line
		for(Int_t j = 0; j <= k; j ++){    // values
		    cout<<setw(12)<<Tijk[i][j][k]<<" "<<flush;
		}
		cout<<endl;
	    }
	    cout<<endl;
	}
    }


    void print(Bool_t printAll = kFALSE)
    {
	// printAll = kTRUE will print transforms in addition
	cout<<"---------------------------------"<<endl;
	cout<<"ID  : "<<setw(3)<<fId<<" , "<<fName.Data()<<endl;
	cout<<"distance   : "<<setw(12)<<fDistance<<endl;
	cout<<"CutType    : "<<setw(3)<<fCutType<<" , x : "<<setw(12)<<fxCut<<", y : "<<setw(12)<<fyCut<<endl;
	cout<<"Acceptance : "<<setw(8)<<fCtAll<<" , Failed : "<<setw(8)<<fCtFail<<" ,  "<<setw(8)<<(fCtAll > 0?  (fCtFail/(Double_t)fCtAll)*100: 100)<<" %"<<endl;
        if(printAll){
	    cout<<"first  order transform:"<<endl;
	    printTij();
	    cout<<"second order transform:"<<endl;
	    printTijk();
	}
    }

    Bool_t isInAcceptance()
    {
	// checks if the particle is inside acceptance
	// for this element. to be called after beam transport (for elements)
	// and after createDetectorHits() for detectors
	if(fCutType == 0 || fCutType > 2) return kTRUE;
	if(fCutType == 1 ){
	    Double_t radius = TMath::Sqrt(fout[0] * fout[0] + fout[2] * fout[2]) ;
	    if(radius <  fxCut )   return kTRUE;
	    else                   return kFALSE;
	}
	if(fCutType == 2 ){
	    if(TMath::Abs(fout[0]) < fxCut && TMath::Abs(fout[2]) < fyCut ) return kTRUE;
	    else                                                            return kFALSE;
	}
	return kTRUE;
    }

    void check()
    {
	// checks the acceptance and fill the stats counters
	// and the fAccepted flag
	if(!isInAcceptance()) {
	    fCtFail++;
	    fAccepted = kFALSE;
	} else  fAccepted = kTRUE;

	fCtAll++;
    }

    void  toLinearArray(TArrayD& all)
    {
	// store in linearized fashion  i*5+j  and  i*25+j*5+k fID,fDistance,fxCut,fyCut,fCuttype
        // last element is the element id

	all.Set(5*5 + 5*5*5 + 5);  // 1st,2nd, fID,fDistance,fxCut,fyCut,fCuttype

	for(Int_t i = 0; i < 5; i ++) {
	    for(Int_t j = 0; j < 5; j ++){
		all[i*5+j]=Tij[i][j];
	    }
	}
	for(Int_t i = 0; i < 5; i ++) {
	    for(Int_t j = 0; j < 5; j ++){
		for(Int_t k = 0; k < 5; k ++){
		    all[25+i*25+j*5+k]=Tijk[i][j][k];
		}
	    }
	}
        all[150+0] = fId;
        all[150+1] = fDistance;
        all[150+2] = fxCut;
        all[150+3] = fyCut;
        all[150+4] = fCutType;
    }

    void  fromLinearArray(TArrayD& all)
    {
	// store in linearized fashion  i*5+j  and  i*25+j*5+k   fID,fDistance,fxCut,fyCut,fCuttype
        // last element is the element id

	for(Int_t i = 0; i < 5; i ++) {
	    for(Int_t j = 0; j < 5; j ++){
                Tij[i][j]=all[i*5+j];
	    }
	}
	for(Int_t i = 0; i < 5; i ++) {
	    for(Int_t j = 0; j < 5; j ++){
		for(Int_t k = 0; k < 5; k ++){
		    Tijk[i][j][k]=all[25+i*25+j*5+k];
		}
	    }
	}
        fId       = all[150+0];
        fDistance = all[150+1];
        fxCut     = all[150+2];
        fyCut     = all[150+3];
        fCutType  = all[150+4];
    }
#ifndef isPluto
     ClassDef(HBeamElement,1)
#endif
};


class HBeamParticle : public TObject {

public:
    TVector3 fP;                 // momentum vector
    TVector3 fPos;               // position vector
    TVector3 fOffset;            // beam could be displaced : profile function makes use of it
    TVector3 fMomRes;            // momentum resolution

    Double_t fBeamMom;           // nominal beam momentum
    Double_t fBeamMomSmeared;    // smeared beam momentum
    Int_t    fPid;               // type of beam particle
    Int_t    fDetnum;            // detector number where the transported particle was registered
    TString  fName;              // name of detecor where particle was registered
    Double_t fbeamtube_size_x;   // x general acceptance of the beam tube
    Double_t fbeamtube_size_y;   // y general acceptance of the beam tube
    Bool_t   fAccepted;          // whether the particle was reaching the detector without violation acceptance

    HBeamParticle(){
	clear();
    }

    void clear (){
	fP     .SetXYZ(0.,0.,0.);
	fPos   .SetXYZ(0.,0.,0.);
	fOffset.SetXYZ(0.,0.,0.);
	fMomRes.SetXYZ(0.,0.,0.);
	fBeamMom         = -1.;
	fBeamMomSmeared  = -1.;
	fPid             = -10;
	fDetnum          = -1;
	fbeamtube_size_x = -1.;
	fbeamtube_size_y = -1.;
	fAccepted        = kTRUE;
    }


    void print(Bool_t printAll = kFALSE){
	// printAll=kTRUE print in addition momentum vector
	cout<<"---------------------------------"<<endl;
	cout<<setw(12)<<fName.Data()<<" acc : "<<fAccepted<<" , ID "<<fPid<<" , Beam Mom : "<<setw(12)<<fBeamMom<<endl;
	cout<<"Beam tube [x,y]       : "<<setw(12)<<fbeamtube_size_x <<" , "<<setw(12)<<fbeamtube_size_y <<endl;
	cout<<"P smear   [x,y,z]     : "<<setw(12)<<fMomRes.X()<<" , "<<setw(12)<<fMomRes .Y()<<" , "<<setw(12)<<fMomRes.Z()<<endl;
	cout<<"Position  [x,y,z]     : "<<setw(12)<<fPos   .X()<<" , "<<setw(12)<<fPos    .Y()<<" , "<<setw(12)<<fPos   .Z()<<endl;
	cout<<"Offset    [x,y,z]     : "<<setw(12)<<fOffset.X()<<" , "<<setw(12)<<fOffset .Y()<<" , "<<setw(12)<<fOffset.Z()<<endl;

	if(printAll){
	    cout<<"P         [x,y,z,tot] : "<<setw(12)<<fP     .X()<<" , "<<setw(12)<<fP      .Y()<<" , "<<setw(12)<<fP     .Z()<<" , "<<setw(12)<<fP  .Mag()<<endl;
	    cout<<"Detector ID           : "<<setw(12)<<fDetnum    <<endl;
	}


    }
#ifndef isPluto
     ClassDef(HBeamParticle,1)
#endif

};




class HBeam : public TObject {



private:

    static Double_t profile(Double_t *x, Double_t *par)
    {
	Double_t xx    = x[0];
	Double_t c     = 1;
	Double_t mean  = 0;
	Double_t sigma = par[0];
	Double_t flat  = par[1];
	Double_t result = 0;

	if(xx < mean - flat) {         // left gaus
	    result = c * TMath::Gaus(xx,mean - flat,sigma);
	} else if(xx > mean + flat) {  // right gaus
	    result = c * TMath::Gaus(xx,mean + flat,sigma);
	} else {                       // flat top
	    result = c;
	}

	return result;
    }



    HBeamParticle fBeam;                 // beam particle
    TF1*          fProfile;              //-> beam profile

    vector <HBeamElement> fdetectors;    // vector of all detectors
    vector <HBeamElement>  felements;    // vector of all beam elements
    vector <HBeamParticle>  fhistory;    //! vector of beam particle at all detectors
    Int_t                fnum_target;    // index of elemnt target
    Double_t            ftoPluto  [5];   // units -> transport ->Pluto
    Double_t            ffromPluto[5];   // units -> Pluto     ->transport
    Bool_t setTargetElement(UInt_t n)
    {
	if (n < 1 || n >= felements.size()){
	    cout<<"HBeam::setTargetElement() : specifified target number outside range!"<<endl;
	    return kFALSE;
	}
	fnum_target = n;
	Double_t shift = felements[fnum_target].fDistance;
	for (UInt_t i = 0; i < felements.size(); i ++){
	    felements[i].fDistance -= shift;
	}
	return kTRUE;
    }

public:

    HBeam()
    {
	fnum_target = -1;
	fProfile = new TF1("fprofile",profile,-20,20,2);
	fProfile->SetParNames("sigma","flatradius");
	setBeamProfile(0.05,0);
	setBeamResolution(0.01,0.05,0.06); //
	ftoPluto[0] = 10.;      // x  cm   -> mm
	ftoPluto[1] =  0.001;   // tx mrad -> rad
	ftoPluto[2] = 10.;      // y  cm   -> mm
	ftoPluto[3] =  0.001;   // ty mrad -> rad
	ftoPluto[4] =  0.01;    // dp %    -> frac

	for(Int_t i = 0; i < 5; i ++) ffromPluto[i] = 1./ftoPluto[i];
    }

    ~HBeam()
    {
	if(fProfile) delete fProfile;
    }
    vector<HBeamElement>&    getDetectors()     { return fdetectors ;  }
    vector<HBeamElement>&    getElements()      { return felements ;   }
    vector<HBeamParticle>&   getHistory()       { return fhistory ;    }

    void                     setTargetElementOnly(Int_t id)        { fnum_target=id;      }

    Int_t addDetector(TString name, Double_t distance,Int_t cutType,Double_t xcut=50,Double_t ycut=50)
    {
	// adds a detector "name" at place "distance" [in mm]
	// (distance must be negative if placed before the target)
	// cutType (0,1,2)(no,radial,square) xcut/ycut [mm]

	HBeamElement det(name,distance,fdetectors.size()+1);
	det.fxCut    = TMath::Abs(xcut);
	det.fyCut    = TMath::Abs(ycut);
	det.fCutType = cutType;

	fdetectors.push_back(det);
	return fdetectors.size();
    }

    void setBeam(Int_t id,
		 Double_t p,
		 Double_t dbeamtube_size_x,Double_t dbeamtube_size_y,
		 Double_t xoff = 0 ,Double_t yoff = 0)
    {
	fBeam.fBeamMom = TMath::Abs(p);
	fBeam.fPid     = id;
	fBeam.fOffset.SetXYZ(xoff,yoff,0.);
	fBeam.fbeamtube_size_x = TMath::Abs(dbeamtube_size_x);
	fBeam.fbeamtube_size_y = TMath::Abs(dbeamtube_size_y);
    }

    void setBeamResolution(Double_t dpx = 0.001,Double_t dpy = 0.005,Double_t dpz = 0.06)
    {
	fBeam.fMomRes.SetXYZ(TMath::Abs(dpx),TMath::Abs(dpy),TMath::Abs(dpz));
    }

    void setBeamProfile(Double_t sigma_beam = 0.05,Double_t flat_radius = 0)
    {
	fProfile->SetParameter(0,TMath::Abs(sigma_beam));
	fProfile->SetParameter(1,TMath::Abs(flat_radius));
    }


    void createBeam(HBeamParticle& part)
    {
	//----------------------------------------
	// beam profile
	// x-y symmetric profile
	Double_t x = 0;
	if      ( fProfile->GetParameter(0) != 0)                                  x  = fProfile->GetRandom();
	else if ( fProfile->GetParameter(0) == 0  && fProfile->GetParameter(1)!=0) x  = 2 * (gRandom->Rndm() - 0.5) * fProfile->GetParameter(3);
	else                                                                       x  = 0;
	Double_t phi = gRandom->Rndm()*360.;
	TVector3 p1(x,0.,felements[fnum_target].fDistance);
	p1.RotateZ(phi*TMath::DegToRad());
	p1 += fBeam.fOffset;  // beam displacement
	part.fPos += p1;
	// beam divergence
	Double_t p  = fBeam.fBeamMom + fBeam.fBeamMom * (gRandom->Rndm() - 0.5) * 2*fBeam.fMomRes.Z();
	Double_t px = p * (gRandom->Rndm() - 0.5) * 2*fBeam.fMomRes.X();
	Double_t py = p * (gRandom->Rndm() - 0.5) * 2*fBeam.fMomRes.Y();
	Double_t pz = TMath::Sqrt(p*p - px*px - py*py);

	part.fP.SetXYZ(px,py,pz);
	part.fBeamMomSmeared = p;
	part.fName   = "beam";
	part.fDetnum = -1;
	//----------------------------------------
    }

    Bool_t transport(HBeamParticle& part)
    {

	//----------------------------------------
	// coordinates  x [cm], px/pz [mrad],y [mm], py/pz [mrad], dp to cental p [%]
	//
	//
	//   x_i =  sum (j=0,5) T_ij*x_j + sum (j=0,5)(k=j,5)T_ijk*x_j*x_k
	//
	//
	//

	Double_t state   [5] = { 0,  0, 0,  0, 0};
	Double_t stateold[5] = {
	    part.fPos.X()                        *ffromPluto[0],
	    TMath::ATan2(part.fP.X(),part.fP.Z())*ffromPluto[1],
	    part.fPos.Y()                        *ffromPluto[2],
	    TMath::ATan2(part.fP.Y(),part.fP.Z())*ffromPluto[3],
	    ((part.fBeamMomSmeared-fBeam.fBeamMom)/fBeam.fBeamMom)*ffromPluto[4]
	};

	for(UInt_t el = 0; el < felements.size(); el++){
	    for(Int_t i = 0; i < 5; i ++){ // state vars
		// first order Term
		for(Int_t j = 0; j < 5; j ++){ //
		    state[i] += felements[el].Tij[i][j] * stateold[j];
		}
		// second order Term
		for(Int_t j = 0; j < 5; j ++){ //
		    for(Int_t k = j; k < 5; k ++){ //
			state[i] += felements[el].Tijk[i][j][k] * stateold[j] * stateold[k];
		    }
		}
	    }

	    // propagate and reset
	    for(Int_t i = 0; i < 5; i ++) {
		felements[el].fout[i] = state[i] * ftoPluto[i];
		state[i] = 0.;
	    }
	    felements[el].fout[4] = ((part.fBeamMomSmeared-fBeam.fBeamMom)/fBeam.fBeamMom) ; // keep dp
	    felements[el].check(); // stats and accepted flag
	}

	return kTRUE;
    }

    Bool_t createDetectorHits(HBeamParticle& part)
    {
	//calc particles at detector places
	Double_t state   [5] = { 0,  0, 0,  0, 0};
	Double_t p,px,py,pz;
	Bool_t   accepted    = kTRUE;
	Bool_t   acceptedDet = kTRUE;

	for (UInt_t i = 0; i < fdetectors.size(); i ++) {
	    accepted = kTRUE;
	    for (UInt_t k = 0; k < felements.size() - 1; k ++) {
		if(!felements[k].fAccepted) {
		    accepted = kFALSE;
		}
		if (felements[k].fDistance <= fdetectors[i].fDistance &&
		    fdetectors[i].fDistance < felements[k+1].fDistance) {

		    Double_t frac = (fdetectors[i].fDistance - felements[k].fDistance) / (felements[k+1].fDistance - felements[k].fDistance);

		    for(Int_t j = 0; j < 5; j ++){
			state[j] = felements[k].fout[j] + frac*(felements[k+1].fout[j] - felements[k].fout[j]);
		    }

		    p  = fBeam.fBeamMom * (1. + state[4]);
		    pz = p / sqrt(1.+ tan(state[1])*tan(state[1]) + tan(state[3])*tan(state[3]));
		    px = pz * tan(state[1]);
		    py = pz * tan(state[3]);

		    part.fP  .SetXYZ(px,py,pz);
		    part.fPos.SetXYZ(state[0],state[2],fdetectors[i].fDistance);
		    part.fDetnum   = fdetectors[i].fId;
		    part.fName     = fdetectors[i].fName;
		    part.fAccepted = accepted;

		    fdetectors[i].fout[0] = state[0] ;
		    fdetectors[i].fout[2] = state[2] ;
		    fdetectors[i].check();

		    if(!fdetectors[i].fAccepted) {
			part.fAccepted = kFALSE;
			acceptedDet    = kFALSE;
		    }
		    fhistory.push_back(part);
		}
	    }
	}
	if(!felements[felements.size()-1].fAccepted) accepted = kFALSE;

	return  accepted&acceptedDet;
    }

    vector <HBeamParticle>& newParticle()
    {
	HBeamParticle part(fBeam);
	fhistory.clear();

	createBeam(part);         // beam profile + smearing

	fhistory.push_back(part);

	transport(part);          // transport particle through beamline

	Bool_t accepted = createDetectorHits(part); // fill detectors

	// calc particle at 0,0,0 ideal target
	Double_t* out = &felements[fnum_target].fout[0];
	Double_t p,px,py,pz;
	p  = fBeam.fBeamMom * (1. + out[4]);
	pz = p / sqrt(1.+ tan(out[1])*tan(out[1]) + tan(out[3])*tan(out[3]));
	px = pz * tan(out[1]);
	py = pz * tan(out[3]);

	part.fP  .SetXYZ(px,py,pz);
	part.fPos.SetXYZ(out[0],out[2],felements[fnum_target].fDistance);
	part.fDetnum = -1;
	part.fName   = felements[fnum_target].fName;
	part.fAccepted = accepted;
	fhistory.push_back(part);

	return fhistory;
    }


    void printBeamLine(Bool_t trans=kFALSE)
    {
	cout<<"############################################################################"<<endl;
	cout<<"BEAMLINE ELEMENTS :"<<endl;
	for(UInt_t i = 0; i < felements.size(); i ++){
	    felements[i].print(trans);
	}
	cout<<"############################################################################"<<endl;
    }
    void printBeamProfile()
    {
	cout<<"############################################################################"<<endl;
	cout<<"BEAM PROFILE :"<<endl;
	for(Int_t i = 0 ; i < fProfile->GetNpar();i ++){
	    cout<<setw(20)<<fProfile->GetParName(i)<<" : "<<setw(12)<<fProfile->GetParameter(i)<<endl;
	}
	fBeam.print(kFALSE);
	cout<<"############################################################################"<<endl;
    }
    void printDetectors()
    {
	cout<<"############################################################################"<<endl;
	cout<<"DETECTORS :"<<endl;
	for(UInt_t i = 0; i < fdetectors.size(); i ++){
	    fdetectors[i].print(kFALSE);
	}
	cout<<"############################################################################"<<endl;
    }

    Bool_t initBeamLine(TString filename,Int_t targetElementNum,Bool_t debug=kFALSE)
    {
	//opens the ASCII description of the beam line


	cout<<"HBeam:: reading input!"<<endl;
	Int_t nelem = 0;



	std::ifstream input;
	input.open(filename.Data());

	if(input.fail()){
	    cout<<"HBeam:: could not open input!"<<endl;
	    return kFALSE;
	}


	Char_t str[1000];
	Int_t  nread = 1000;
	Bool_t readfile = kTRUE;
	//----------------------------
	// local dummy vars
	Double_t aa;
	Int_t ind1;
	Int_t ind23;
	Double_t distance;
	//----------------------------


	while (readfile && !input.fail()) {

	    input>>distance;
	    if(input.peek()!='\n') input.ignore(nread,'\n');
	    if(input.fail() && nelem > 0 ) continue;
	    if(input.fail() ){ cout<<"HBeam:: "<<nelem<<" failed reading distance"<<endl; return kFALSE;}
	    distance *= 1000;  // [m] -> [mm]
	    if(debug) cout<<nelem<<" ------------------------"<<distance<<endl;
	    for(Int_t i = 0; i < 5; i++) {
		input.ignore(nread,'\n');
	    }
	    if(input.fail() ) {
		cout<<"HBeam:: "<<nelem<<" failed after first block "<<endl;
		return kFALSE;
	    }
	    input.getline(str,nread,'\n');  // *TRANSFORM 1*
	    if(debug) cout<<"HBeam:: "<<"1st "<<str<<endl;

	    HBeamElement e(Form("Element[%i]",nelem),distance,nelem);
	    // first order transform
	    Int_t ind = 0;
	    for(Int_t i = 0; i < 6; i++){ // lines
		if(i == 4) {
		    input>>aa>>aa>>aa>>aa>>aa>>aa;
		    continue;
		}
		if(i > 4) ind = i-1;
		else      ind = i;
		input>>e.Tij[ind][0]>>e.Tij[ind][1]>>e.Tij[ind][2]>>e.Tij[ind][3]>>aa>>e.Tij[ind][4];
	    }
	    if(debug) e.printTij();

	    if(input.fail() ) {
		cout<<"HBeam:: "<<nelem<<" failed after first order "<<endl;
		return kFALSE;
	    }
	    if(input.peek() =='\n') input.ignore(nread,'\n');
	    input.getline(str,nread,'\n');   // *2ND ORDER TRANSFORM*

	    if(debug) cout<<"HBeam:: "<<"2nd : "<<str<<endl;
	    //C
	    //C     second order transform
	    //C

	    for(Int_t i = 0; i < 5; i++){ // blocks i
		input>>ind1>>ind23>>e.Tijk[i][0][0];
		input>>ind1>>ind23>>e.Tijk[i][0][1]>>ind1>>ind23>>e.Tijk[i][1][1];
		input>>ind1>>ind23>>e.Tijk[i][0][2]>>ind1>>ind23>>e.Tijk[i][1][2]>>ind1>>ind23>>e.Tijk[i][2][2];
		input>>ind1>>ind23>>e.Tijk[i][0][3]>>ind1>>ind23>>e.Tijk[i][1][3]>>ind1>>ind23>>e.Tijk[i][2][3]>>ind1>>ind23>>e.Tijk[i][3][3];
		input>>ind1>>ind23>>aa             >>ind1>>ind23>>aa             >>ind1>>ind23>>aa             >>ind1>>ind23>>aa             >>ind1>>ind23>>aa;
		input>>ind1>>ind23>>e.Tijk[i][0][4]>>ind1>>ind23>>e.Tijk[i][1][4]>>ind1>>ind23>>e.Tijk[i][2][4]>>ind1>>ind23>>e.Tijk[i][3][4]>>ind1>>ind23>>aa>>ind1>>ind23>>e.Tijk[i][4][4];
	    }

	    if(debug) e.printTijk();

	    if(input.fail() ) {
		cout<<"HBeam:: "<<nelem<<" failed after second order "<<endl;
		return kFALSE;
	    }

	    felements.push_back(e);
	    input.ignore(nread,'\n');

	    if(input.fail() ) return kFALSE;

	    if (!input.eof())
		nelem++;
	    else
		readfile = kFALSE;

	} //end nelem

	input.close();
	cout<<"HBeam:: finished reading input!"<<endl;
	return setTargetElement(targetElementNum);

    }



#ifndef isPluto
    ClassDef(HBeam,1)
#endif

};


#endif


