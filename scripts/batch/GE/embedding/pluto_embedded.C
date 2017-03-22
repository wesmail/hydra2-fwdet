/**
 * @file   run_embedded.cc
 * @author Jochen Markert
 * @date   Mon Jun 11 22:06:01 2007
 * 
 * @brief  This file can be compiled or executed within CINT to produce
 *         a PLUTO embedded particle with ascii output in HGEANT format
 *         keeping the eventvertex read from vertex ntuple (if provided).
 *
 *         If no vertex ntuple file is provided simply the number of events
 *         will be execute. Otherwise the number of events is taken from ntuple.
 *
 * $Id$
 *
 */





#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TNtuple.h"

#include "hseed.h"

#include "PParticle.h"
#include "PFireball.h"
#include "PReaction.h"
#include "PEmbeddedParticles.h"
#include "PHGeantOutput.h"
#include "PVertexFile.h"


#include <iostream>

using namespace std;


Int_t pluto_embedded(TString outdir="",TString outfile="",TString type="pi-",
		     Int_t nEvents=1000,Int_t source = 1,
		     TString vertexntuple="")
{

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

   //-------------------------------------------------

   // This macro creates n pluto particle per sector (60 deg) with white disribution
   // in theta , phi and momentum. The particles will be created
   // at the vertex given my the input ntuple extracted from REAL data.


   //---------------CONIGURATION----------------------
   //

   Int_t  sourceType   = source; // 0 : white single tracks , 1 : thermal source
   Int_t  asciiOut     = 0;    // write pluto ascci output for HGeant (==0 if we use HGeantOutput)
   Int_t  rootOut      = 0;    // write pluto root file
   PChannel** channel  = 0;    // no channel needed here
   Int_t    nChannel   = 0;    // number of channels will be 0
   TString myParticle  = type; // particle type used for embedding
   Int_t   calcVertex     = useVertex ? 1 : 0;           // write the vertex to the ascii output for HGeant
   Bool_t  writeSeqNumber = useVertex ? kTRUE : kFALSE;  // write eventSeqNumber in addition
   Bool_t  writeIndex     = useVertex ? kTRUE : kFALSE;  // write parentIndex in addition


   //-------------------------------------------------
   PReaction* r          = 0;
   PHGeantOutput* output = 0;

   outfile = gSystem->BaseName(outfile.Data());
   TString filename = Form("%s/%s",outdir.Data(),outfile.Data());
   filename.ReplaceAll("//","/");
   filename.ReplaceAll(".root","");
   filename.ReplaceAll(".evt","");
   filename = Form("%s_pluto_%s",filename.Data(),myParticle.Data());

   if(sourceType==0)
   {   // white single tracks embedding

       Float_t pmin               = 0.025;   // minimum mom  [GeV/c]
       Float_t pmax               = 2.000;   // maximum mom  [GeV/c]
       Float_t thetamin           =    10;   // minimum theta angle  [deg]
       Float_t thetamax           =    90;   // maximum theta angle  [deg]
       Float_t phimin             =     0;   // minimum phi angle    [deg]
       Float_t phimax             =    60;   // maximum phi angle    [deg]
       Float_t phistart           =     0;   // start phi angle (+ sector * 60. deg will be rotated later)
       Int_t numParticlePerSector =     1;   // number of Particles embedded per sector

       r = new PReaction(channel,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut);
       r->SetWriteIndex(writeIndex);

       if(useVertex){
	   //Construct the vertex container:
	   PVertexFile *vertex = new PVertexFile();
	   vertex->OpenFile(vertexntuple);

	   //add to prologue action
	   r->AddPrologueBulk(vertex);
       }

       output = new PHGeantOutput();
       output->SetWriteSeqNumber(writeSeqNumber);
       output->OpenFile(Form("%s.evt",filename.Data()));
       r->AddFileOutput(output);


       //Construct the embedded container:
       PEmbeddedParticles* embedded = new PEmbeddedParticles();


       //Add an e+ which we emit at a single point:
       PParticle* particle = new PParticle((Char_t*)myParticle.Data(),1.,2.,3.);
       //Just add the particle to the container:
       embedded->AddParticle(particle);

       // added particle will be cloned with
       // n times per sector
       if (1) {
	   // nparticle / sector
	   embedded->SetSamplingSector(pmin    , pmax,                   // pmin,pmax
				       thetamin, thetamax,               // theta_min, theta_max
				       phimin  , phimax,                 // phi_min,phi_max
				       phistart, numParticlePerSector ); // phiStart, numParticle/sector
       } else {
	   // nparticle / full acceptance
	   embedded->SetSamplingSector(pmin    , pmax,                 // pmin,pmax
				       thetamin, thetamax,             // theta_min, theta_max
				       -180.    , 180.,                // phi_min,phi_max
				       phistart, numParticlePerSector, // phiStart, numParticle/sector
				       360.    , 1);                   // Delta-phi, nSec

       }


       //Add our container to the reaction:
       r->AddBulk(embedded);

   } else {  // physical sources

       makeDistributionManager()->Print("decay_models");

       //PFireball(char* particle, float AGeV, float t1, float t2, float f,
       //        float b, float a2, float a4, float w1, float w2, int sp) :
       //
       // Thermal source with temperature(s) T1 (T2), frac*f(T1) + (1-frac)*f(T2),
       // optional blast, optional polar anisotropies (A2,A4), optional flow (v1,v2).
       //
       Float_t Eb         = 1.23;   // beam energy in GeV/u
       Float_t T1         = 0.06; // temperature in GeV
       Float_t T2         = 0.;  // temperature in GeV
       Float_t fractionT1 = 1;
       Float_t blast      = 0.;   // radial expansion velocity  (0.3)
       Float_t anisoA2    = 0.;   // 1 before
       Float_t anisoA4    = 0.;
       Float_t flowV1     = 0.;
       Float_t flowV2     = 0.;

       if(type.CompareTo("lambda")   == 0){
           nChannel = 1;

	   T1 = 0.090;
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

	   r=new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
       }

       if(type.CompareTo("Xi-")   == 0){
           nChannel = 1;

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

	   r=new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
       }


       if(type.CompareTo("K0S") == 0){
           nChannel = 1;

	   T1 = 0.080;
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

	   r=new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
       }
       //
       if(type.CompareTo("K-")   == 0){
           nChannel = 1;

	   T1 = 0.070;
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

	   r=new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
       }

       if(type.CompareTo("K+")   == 0){
           nChannel = 1;

	   T1 = 0.080;
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

	   r=new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
       }

       if(type.CompareTo("phiKK") == 0)
       {
	   nChannel = 2;

	   T1 = 0.080;
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

	   r = new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut);   // 2 particle in the final state
       }

       if(type.CompareTo("p")   == 0){

	   nChannel = 1;

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

	   r=new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
       }

       if(type.CompareTo("d")   == 0){

	   nChannel = 1;

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

	   r=new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
       }

       if(type.CompareTo("pi-")   == 0){
           nChannel = 1;

	   T1 = 0.049;
	   T2 = 0.089;
	   fractionT1 = 0.89;
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

	   r=new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
       }

       if(type.CompareTo("pi+")   == 0){
           nChannel = 1;

	   T1 = 0.049;
	   T2 = 0.089;
	   fractionT1 = 0.89;
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

	   r=new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut); // 1 particle in the final state
       }

       if(type.CompareTo("w")   == 0 ||      //   vector meson -> e+ + e-
	  type.CompareTo("phi") == 0 ||
	  type.CompareTo("rho0")== 0
	 ) {
           nChannel=2;

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

	   r = new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut);  // 2 particle in the final state
       }

       if(type.CompareTo("pi0")==0 ) {          // pi0 -> g + e- + e+
           nChannel = 3;

	   T1 = 0.049;
	   T2 = 0.089;
	   fractionT1 = 0.89;
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

	   r = new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut); // 3 particle in the final state
       }

       if(type.CompareTo("eta")==0 ) {          // eta -> g + e- + e+
           nChannel = 3;
	   T1 = 0.050;
	   T2 = 0.000;
	   fractionT1 = 1.0;
	   anisoA2    = 1.0;
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

	   r = new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut);  // 3 particle in the final state
       }

       if(type.CompareTo("D+")==0 ) {    // delta -> p + e+ + e-
           nChannel = 3;

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

	   r = new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut);  // 3 particle in the final state

       }
       if(type.CompareTo("wdalitz")==0 ) {  // w -> pi0 + e- + e+
           nChannel = 3;

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

	   r = new PReaction(cc,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut);    // 3 particle in the final state
       }


       r->SetWriteIndex(writeIndex);
       r->trackedParticles();

       if(useVertex){
	   //Construct the vertex container:
	   PVertexFile *vertex = new PVertexFile();
	   vertex->OpenFile(vertexntuple);

	   //add to prologue action
	   r->AddPrologueBulk(vertex);
       } else {
        cout<<"NO VERTEX SET "<<endl;

       }


       output = new PHGeantOutput();
       output->SetWriteSeqNumber(writeSeqNumber);
       output->OpenFile(Form("%s.evt",filename.Data()));
       r->AddFileOutput(output);

   }

   r->setHGeant(0);

   if(useVertex){
       TFile *f = new TFile(vertexntuple.Data());
       if(f==NULL)
       {
	   cout << "file "<<vertexntuple.Data()<<" not found" << endl;
	   exit(1);

       }
       TNtuple *vertexnt = (TNtuple*)(f->Get("vertex"));
       if(vertexnt==NULL)
       {
	   cout << "NULL pointer to ntuple" << endl;
	   exit(1);
       }

       // number of events
       r->Loop(vertexnt->GetEntries());
   } else {
       r->Loop(nEvents);
   }

   output->CloseFile();


   return 0;
}
