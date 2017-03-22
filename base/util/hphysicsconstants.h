// @(#)$Id: hphysicsconstants.h,v 1.7 2008-09-18 13:09:34 halo Exp $
//*-- Author : Dan Magestro
//*-- Created: 03/09/01
//*-- Copy into Pid lib and changing the class name by Marcin Jaskula
//    15/10/02
//*-- Modified     : 24/02/03 by Marcin Jaskula
//                   new ids for fakes, get rid of TObject
//*-- Modified     : 03/11/06 by Alexander Belyaev
//                   leptonCharge, baryonCharge and strangeness are added

#ifndef HPHYSICSCONSTANTS_H
#define HPHYSICSCONSTANTS_H

// -----------------------------------------------------------------------------

#include "TObject.h"
#include "TString.h"
#include <map>
#include <iostream>
#include <iomanip>
using namespace std;

// -----------------------------------------------------------------------------


class HGeantKine;

// -----------------------------------------------------------------------------
typedef struct  {
    TString  fName;
    Double_t fMass;
    Int_t    fId;
    Int_t    fCharge;
    Int_t    fLeptonCharge;
    Int_t    fBaryonCharge;
    Int_t    fStrangeness;

    Int_t    fLineColor; // eventdisplay
    Int_t    fLineStyle; // eventdisplay

    void fill(Int_t id, TString name, Double_t mass,Int_t chrg,Int_t leptchrg,Int_t barychrg,Int_t strange)
    {
	fName         = name;
	fMass         = mass;
	fId           = id;
	fCharge       = chrg;
	fLeptonCharge = leptchrg;
	fBaryonCharge = barychrg;
	fStrangeness  = strange;
	fLineColor    = 2;
        fLineStyle    = 1;

    }
    void print(){
	cout<<setw(3)<<right<<fId
	    <<" "              <<setw(15)<<left <<fName.Data()
	    <<" mass "         <<setw(10)<<left <<fMass
	    <<" chrg "         <<setw(3) <<right<<fCharge
	    <<" lepton chrg "  <<setw(3) <<right<<fLeptonCharge
	    <<" baryon chrg "  <<setw(3) <<right<<fBaryonCharge
	    <<" strangeness "  <<setw(3) <<right<<fStrangeness
	    <<" col "          <<setw(3) <<right<<fLineColor
	    <<" style "        <<setw(3) <<right<<fLineStyle
	    <<endl;
    };

} particleproperties;

class HPhysicsConstants : public TObject
{

    

    static map<Int_t,particleproperties  >   idToProp;
    static map<TString,particleproperties> nameToProp;
    static map<Int_t,TString>           mGeantProcess;

public:
    static Int_t         pid(const Char_t *pidName);
    static const Char_t* pid(Short_t pid);

    static Int_t       charge(const Int_t id);
    static Int_t       charge(const Char_t *id) { return charge(pid(id)); }

    static Int_t       leptonCharge(const Int_t id);
    static Int_t       leptonCharge(const Char_t *id) { return leptonCharge(pid(id)); }

    static Int_t       baryonCharge(const Int_t id);
    static Int_t       baryonCharge(const Char_t *id) { return baryonCharge(pid(id)); }

    static Int_t       strangeness(const Int_t id);
    static Int_t       strangeness(const Char_t *id) { return strangeness(pid(id)); }

    static Float_t     mass(const Int_t id);
    static Float_t     mass(const Char_t *id)   { return mass(pid(id));   }

    static Int_t       lineColor(const Int_t id);
    static Int_t       lineColor(const Char_t *id)   { return lineColor(pid(id));   }

    static Int_t       lineStyle(const Int_t id);
    static Int_t       lineStyle(const Char_t *id)   { return lineStyle(pid(id));   }

    static Bool_t      isFake(Int_t iId);
    static Bool_t      isArtificial(Int_t iId);

    static const Char_t* geantProcess(Int_t mech);

    static Bool_t      addParticle(Int_t id, TString name, Double_t mass,Int_t chrg,Int_t leptchrg,Int_t barychrg,Int_t strange);
    static Bool_t      addParticle(particleproperties prop);
    static Bool_t      removeParticle(Int_t id);
    static Bool_t      moveParticle(Int_t id,Int_t newid);
    static Bool_t      cpParticle  (Int_t id,Int_t newid,TString newName);
    static void        clearParticles();
    static particleproperties createParticle(Int_t id,TString name, Double_t mass,Int_t chrg,Int_t leptchrg,Int_t barychrg,Int_t strange);

    static void        setGraphic       (Short_t pid, Int_t col, Int_t style);
    static void        setDefaultGraphic(Int_t col, Int_t style);


    static map<Int_t  ,particleproperties> initParticleID();
    static map<TString,particleproperties> initParticleName();
    static map<Int_t,TString>              initGeantProcess();



    static void        loadGeantIons();
    static void        print();

    // -------------------------------------------------------------------------

    static Short_t     artificialPos(void)  { return pid("artifical+"); }
    static Short_t     artificialNeg(void)  { return pid("artifical-"); }
    static Short_t     fakePos(void)        { return pid("fake+"); }
    static Short_t     fakeNeg(void)        { return pid("fake-"); }


    // -------------------------------------------------------------------------
    ClassDef(HPhysicsConstants,0)
};

// -----------------------------------------------------------------------------

#endif
