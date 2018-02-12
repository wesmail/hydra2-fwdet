#include "hseed.h"

#include "PFireball.h"
#include "PParticle.h"
#include "PChannel.h"
#include "PReaction.h"
#include "PHGeantOutput.h"
#include "PVertexFile.h"
#include "TRandom.h"
#include "TROOT.h"
#include "TString.h"


#include <stdio.h>
#include <iostream>

using namespace std;

int loop(TString outdir="",TString outfile="",TString type="w", Int_t nEvents=1000,TString vertexntuple="")
{  // test thermal source of omegas   (R.H. 16/2/2010)

    /*
     method;        0 (default) : /dev/random,
                    1 : TRandom3 (local) with systime ,
                    2 : TRandom3 (local) with systime + processid + iplast,
                    3 : like 1 but gRandom,
                    4 : like 2 but gRandom,
                    5 : fixed (needs seed to be set)
    */
    Int_t method = 0;
    HSeed hseed(method);
    Int_t seed = hseed.getSeed();

    PUtils::SetSeed(seed);
    UInt_t seed2 = (Int_t)PUtils::sampleFlat()*kMaxUInt;
    gRandom->SetSeed(seed2); // workarround for pluto BUG (new in 5.43 fixed in 5.45)

    makeDistributionManager()->Print("decay_models");

    Bool_t useVertex = kFALSE;
    if(vertexntuple.CompareTo("no")!=0&&vertexntuple.CompareTo("")!=0) useVertex = kTRUE;


    Int_t  asciiOut     = 0;    // write pluto ascci output for HGeant (==0 if we use HGeantOutput)
    Int_t  rootOut      = 0;    // write pluto root file
    Int_t   calcVertex     = useVertex ? 1 : 0;           // write the vertex to the ascii output for HGeant
    Bool_t  writeSeqNumber = useVertex ? kTRUE : kFALSE;  // write eventSeqNumber in addition
    Bool_t  writeIndex     = useVertex ? kTRUE : kFALSE;  // write parentIndex in addition

    //PFireball(char* particle, float AGeV, float t1, float t2, float f,
    //        float b, float a2, float a4, float w1, float w2, int sp) :
    //
    // Thermal source with temperature(s) T1 (T2), frac*f(T1) + (1-frac)*f(T2),
    // optional blast, optional polar anisotropies (A2,A4), optional flow (v1,v2).
    //
    /*
    Float_t Eb    = 1.25;   // beam energy in GeV/u
    Float_t T     = 0.06;   // temperature in GeV
    Float_t blast = 0.0;    // radial expansion velocity
    */
    Float_t Eb         = 1.23;   // beam energy in GeV/u
    Float_t T1         = 0.06; // temperature in GeV
    Float_t T2         = 0.;  // temperature in GeV
    Float_t fractionT1 = 1;
    Float_t blast      = 0.;   // radial expansion velocity  (0.3)
    Float_t anisoA2    = 0.; // 1 before
    Float_t anisoA4    = 0.;
    Float_t flowV1     = 0.;
    Float_t flowV2     = 0.;

    Bool_t doFilter=kFALSE;

    PReaction *r =0 ;

    if(type.CompareTo("hypertriton")   == 0){


	//Add a bunch of dummy particles
	makeStaticData()->AddParticle          (120,"Hypertriton", 2.991);
	makeStaticData()->SetParticleTotalWidth("Hypertriton",0.005);
	makeStaticData()->SetParticleBaryon    ("Hypertriton",3);


	T1 = 0.1; // Manuel (26.9.2017)
	//T1 = 0.100; // test
	PFireball *source = new PFireball("Hypertriton",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("Hypertriton");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;


	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // two particles in the final state
    }

    if(type.CompareTo("lambda")   == 0){
        T1 = 0.090;
	//T1 = 0.100; // test
	PFireball *source = new PFireball("Lambda",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("Lambda");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;


	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // two particles in the final state
    }

    if(type.CompareTo("Xi-")   == 0){
	T1 = 0.090;
	PFireball *source = new PFireball("Xi-",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("Xi-");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;

	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // two particles in the final state
    }


    if(type.CompareTo("K0S") == 0){
	T1 = 0.080;
	//T1 = 0.100; // test
	PFireball *source = new PFireball("K0S",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("K0S");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;

	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // two particles in the final state
    }
     //
    if(type.CompareTo("K-")   == 0){
	T1 = 0.070;
	//T1 = 0.080;  // test
	PFireball *source = new PFireball("K-",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("K-");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;

	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // two particles in the final state
    }

    if(type.CompareTo("K+")   == 0){
	T1 = 0.080;
	//T1 = 0.100; // test
	PFireball *source = new PFireball("K+",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("K+");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;

	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // two particles in the final state
    }

    if(type.CompareTo("phiKK") == 0)
    {
        T1 = 0.080;
        //T1 = 0.100; // for comparison
	PFireball *source = new PFireball("phi",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();
	PParticle *mother = new PParticle("phi");

	PParticle **s = new PParticle* [2];
	s[0] = source;
	s[1] = mother;

	PChannel *c1 = new PChannel(s, 1, 1);
	PParticle *kp   = new PParticle("K+");
	PParticle *km   = new PParticle("K-");

	PParticle **decay = new PParticle* [3];
	decay[0] = mother;
	decay[1] = kp;
	decay[2] = km;

	PChannel *c2    = new PChannel(decay,2,1);

	PChannel  **cc = new PChannel* [2];
	cc[0]  = c1;
	cc[1]  = c2;

	r = new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),2,rootOut,0,calcVertex,asciiOut);
    }

    if(type.CompareTo("p")   == 0){
	T1 = 0.080;
	T2 = 0.0;
	fractionT1 = 1.;
	anisoA2    = 0.;
	anisoA4    = 0.;
	blast      = 0.3;

	PFireball *source = new PFireball("p",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("p");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;

	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
    }

    if(type.CompareTo("d")   == 0){
	T1 = 0.070;
	T2 = 0.0;
	fractionT1 = 1.;
	anisoA2    = 0.;
	anisoA4    = 0.;
	blast      = 0.36;
	PFireball *source = new PFireball("d",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("d");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;

	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
    }

    if(type.CompareTo("t")   == 0){
	T1 = 0.070;
	T2 = 0.0;
	fractionT1 = 1.;
	anisoA2    = 0.;
	anisoA4    = 0.;
	blast      = 0.3;

	PFireball *source = new PFireball("t",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("t");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;

	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
    }

    if(type.CompareTo("pi-")   == 0){
	T1 = 0.049;
	T2 = 0.089;
	fractionT1 = 0.98; // changed 4.4.2017 from 0.89
	anisoA2    = 0.;
	anisoA4    = 0.;
	PFireball *source = new PFireball("pi-",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("pi-");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;

	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
    }

    if(type.CompareTo("pi+")   == 0){

	T1 = 0.049;
	T2 = 0.089;
	fractionT1 = 0.98; // changed 4.4.2017 from 0.89
	anisoA2    = 0.;
	anisoA4    = 0.;

 	PFireball *source = new PFireball("pi+",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();

	PParticle *K0S=new PParticle("pi+");

	PParticle **K0Ss = new PParticle* [2];
	K0Ss[0] = source;
	K0Ss[1] = K0S;

	PChannel  *K0Sc=new PChannel(K0Ss,1,1);

	PChannel  **cc = new PChannel* [1];
	cc[0] = K0Sc ;

	r=new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),1,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
    }

    if(//type.CompareTo("w")   == 0 ||      //   vector meson -> e+ + e-
       //type.CompareTo("phi") == 0 ||
       type.CompareTo("rho0")== 0
      ) {

	PFireball *source = new PFireball((Char_t*)type.Data(),Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();
	PParticle *mother = new PParticle(type.Data());

	PParticle **s  = new PParticle* [2];
	s[0] = source;
	s[1] = mother;

	PChannel *c1 = new PChannel(s, 1, 1);
	PParticle *ep   = new PParticle("e+");
	PParticle *em   = new PParticle("e-");

	PParticle **decay  = new PParticle* [3];
	decay[0] = mother;
	decay[1] = ep;
	decay[2] = em;

	PChannel *c2    = new PChannel(decay,2,1);

	PChannel  **cc = new PChannel* [2];
	cc[0] = c1;
	cc[1] = c2;

	r = new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),2,rootOut,0,calcVertex,asciiOut);
    }

    if(type.CompareTo("w")   == 0 ||      //   vector meson -> e+ + e-
       type.CompareTo("phi") == 0 )
    {
	PFireball *source=new PFireball((Char_t*)type.Data(),Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);

	source->Print();
	PParticle *omeg = new PParticle(type.Data());

	PParticle **s  = new PParticle* [2];
	s[0] = source;
	s[1] = omeg;

	PChannel  *c1   = new PChannel(s,1,1);
	PParticle *ep   = new PParticle("e+");
	PParticle *em   = new PParticle("e-");

	// BW only
	TString command(Form("loop:[%s]->SetM({w_bw}->SampleTotalMass())",type.Data()));
	Double_t m = makeStaticData()->GetParticleMass(type.Data());
	Double_t w = makeStaticData()->GetParticleTotalWidth(type.Data());
	TString bwCutoff(Form("if ( ([%s]->M() > %.3f + %.3f * 4. ) || ( [%s]->M() < %.3f - %.3f * 4.) )",
			      type.Data(), m, w, type.Data(), m, w));

	command += "; " + bwCutoff + " goto loop";
	c1->Do((char*)command.Data());

	PParticle **decay  = new PParticle* [3];
	decay[0] = omeg;
	decay[1] = ep;
	decay[2] = em;

	PChannel *c2    = new PChannel(decay,2,1);
	PChannel  **cc = new PChannel* [2];
	cc[0] = c1;
	cc[1] = c2;

	r = new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),2,rootOut,0,calcVertex,asciiOut);
    }

    if(type.CompareTo("pi0")==0 ) {          // pi0 -> g + e- + e+
	T1 = 0.049;
	T2 = 0.089;
	fractionT1 = 0.98; // changed 4.4.2017 from 0.89
	anisoA2    = 0.;
	anisoA4    = 0.;

	PFireball *source = new PFireball((Char_t*)type.Data(),Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();
	PParticle *pi0 = new PParticle(type.Data());

	PParticle **s  = new PParticle* [2];
	s[0] = source;
	s[1] = pi0;

	PChannel *c1 = new PChannel(s, 1, 1);

	//----- pi0 Dalitz decay ------------
	PParticle *dielec = new PParticle("dilepton");
	PParticle *gamma  = new PParticle("g");

	PParticle *elec   = new PParticle("e-");
	PParticle *posi   = new PParticle("e+");

	PParticle **s2 = new PParticle* [3];
	s2[0] = pi0;
	s2[1] = dielec;
	s2[2] = gamma;

	PChannel  *c2 = new PChannel(s2,2,1);

	PParticle **s3 = new PParticle* [3];
	s3[0] = dielec;
	s3[1] = elec;
	s3[2] = posi;

	PChannel  *c3 = new PChannel(s3,2,1);

	PChannel  **cc = new PChannel* [3];
	cc[0] = c1;
	cc[1] = c2;
	cc[2] = c3;

	r = new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),3,rootOut,0,calcVertex,asciiOut);
    }

    if(type.CompareTo("eta")==0 ) {          // eta -> g + e- + e+
	T1 = 0.050;
	T2 = 0.000;
	fractionT1 = 1.0;
	anisoA2    = 0.0; // 1 before
	anisoA4    = 0.;

	PFireball *source = new PFireball((Char_t*)type.Data(),Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();
	PParticle *pi0 = new PParticle(type.Data());

	PParticle **s     = new PParticle* [2];
	s[0] = source;
	s[1] = pi0;

	PChannel *c1 = new PChannel(s, 1, 1);
	//----- pi0 Dalitz decay ------------
	PParticle *dielec = new PParticle("dilepton");
	PParticle *gamma  = new PParticle("g");

	PParticle *elec   = new PParticle("e-");
	PParticle *posi   = new PParticle("e+");

	PParticle **s2 = new PParticle* [3];
	s2[0] = pi0;
	s2[1] = dielec;
	s2[2] = gamma;

	PChannel  *c2 = new PChannel(s2,2,1);

	PParticle **s3 = new PParticle* [3];
	s3[0] = dielec;
	s3[1] = elec;
	s3[2] = posi;

	PChannel  *c3 = new PChannel(s3,2,1);

	PChannel  **cc = new PChannel* [3];
	cc[0] = c1;
	cc[1] = c2;
	cc[2] = c3;

	r = new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),3,rootOut,0,calcVertex,asciiOut);
    }

    if(type.CompareTo("D+")==0 ) {    // delta -> p + e+ + e-

	PFireball *source = new PFireball((Char_t*)type.Data(),Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();
	PParticle *delta = new PParticle(type.Data());

	PParticle **s  = new PParticle* [2];
	s[0] = source;
	s[1] = delta;

	PChannel *c1 = new PChannel(s, 1, 1);


	PParticle *ep = new PParticle("e+");
	PParticle *em = new PParticle("e-");
	PParticle *p  = new PParticle("p");

	PParticle *dilep = new PParticle("dilepton");

	PParticle **deltadecay = new PParticle* [3];
	deltadecay[0] = delta;
	deltadecay[1] = p;
	deltadecay[2] = dilep;

	PChannel *c2 = new PChannel(deltadecay,2,1,1);

	PParticle **dildecay = new PParticle* [3];
	dildecay[0] = dilep;
	dildecay[1] = ep;
	dildecay[2] = em;

	PChannel *c3 = new PChannel(dildecay,2,1,1);

	PChannel  **cc = new PChannel* [3];
	cc[0] = c1;
	cc[1] = c2;
	cc[2] = c3;

	r = new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),3,rootOut,0,calcVertex,asciiOut);

    }
    if(type.CompareTo("wdalitz")==0 ) {  // w -> pi0 + e- + e+

	PFireball *source = new PFireball("w",Eb,T1,T2,fractionT1,blast,anisoA2,anisoA4,flowV1,flowV2);
	source->setTrueThermal(kTRUE);
	source->Print();
	PParticle *omeg = new PParticle("w");

	PParticle **s  = new PParticle* [2];
	s[0] = source;
	s[1] = omeg;

	PChannel *c1 = new PChannel(s, 1, 1);
	//----- omega dalitz decay ------------
	PParticle *dielec = new PParticle("dilepton");
	PParticle *pi0 = new PParticle("pi0");
	PParticle *elec = new PParticle("e-");
	PParticle *posi = new PParticle("e+");

	PParticle **s2 = new PParticle* [3];
	s2[0] = omeg;
	s2[1] = dielec;
	s2[2] = pi0;

	PChannel  *c2 = new PChannel(s2,2,1);

	PParticle **s3 = new PParticle* [3];
	s3[0] = dielec;
	s3[1] = elec;
	s3[2] = posi;

	PChannel  *c3 = new PChannel(s3,2,1);

	PChannel  **cc = new PChannel* [3];
	cc[0] = c1;
	cc[1] = c2;
	cc[2] = c3;

	r = new PReaction(cc,Form("%s/%s",outdir.Data(),outfile.Data()),3,rootOut,0,calcVertex,asciiOut);
    }

    r->SetWriteIndex(writeIndex);

    if(useVertex){
	//Construct the vertex container:
	PVertexFile *vertex = new PVertexFile();
	vertex->OpenFile(vertexntuple);

	//add to prologue action
	r->AddPrologueBulk(vertex);
    }

    if(!asciiOut){
	PHGeantOutput* output;
	output = new PHGeantOutput();
	output->SetWriteSeqNumber(writeSeqNumber);
	output->OpenFile(Form("%s/%s.evt",outdir.Data(),outfile.Data()));
	r->AddFileOutput(output);
    }


    if(doFilter){
	if(type.CompareTo("lambda")   != 0 &&
	   type.CompareTo("Xi-")      != 0 &&
	   type.CompareTo("K-")       != 0 &&
           type.CompareTo("K+")       != 0 &&
           type.CompareTo("K0S")      != 0 &&
	   type.CompareTo("phiKK")    != 0 &&
	   type.CompareTo("p")        != 0 &&
	   type.CompareTo("d")        != 0 &&
	   type.CompareTo("t")        != 0 &&
	   type.CompareTo("pi-")      != 0 &&
	   type.CompareTo("pi+")      != 0
	  )
	{
	    r->Do("theta_ep = ([e+]->Theta() * 180.)/TMath::Pi()");//  	Online calculating of a variable (here: Theta of e+ in degree)
	    r->Do("theta_em = ([e-]->Theta() * 180.)/TMath::Pi()");// 	...same for e-
	    r->Do("#filter = 1; if theta_ep<5 || theta_ep>88 || theta_em<15 || theta_em>88; #filter = 0");
	} else {
	    //r->Do("theta_lambda = ([Lambda]->Theta() * 180.)/TMath::Pi()");    //  	Online calculating of a variable (here: Theta of proton in degree)
	    //r->Do("#filter = 1; if theta_lambda<15 || theta_lambda>88; #filter = 0");
	}
	//r->Print();
    }

    if(!r){
        cout<<"Unknow type "<<type<<" for reaction source !"<<endl;
	exit(1);
    }

    r->setHGeant(0);   // set to 1, if PLUTO run from HGeant prompt
    r->Preheating(10000);

    if(calcVertex) {
	TFile *f = new TFile(vertexntuple.Data());
	if(f==NULL)
	{
	    cout << "file not found" << endl;
	    exit(1);

	}
	TNtuple *vertexnt = (TNtuple*)(f->Get("vertex"));
	if(vertexnt==NULL)
	{
	    cout << "NULL pointer to ntuple" << endl;
	    exit(1);
	}
	nEvents=vertexnt->GetEntries();
    }

    r->loop(nEvents);
//    output->CloseFile();
    return 0;
}
