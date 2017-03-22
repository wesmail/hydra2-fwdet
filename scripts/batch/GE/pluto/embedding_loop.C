
#include "hseed.h"
#include "PFireball.h"
#include "PParticle.h"
#include "PChannel.h"
#include "PReaction.h"
#include "PVertexFile.h"
#include "PHGeantOutput.h"
#include "PEmbeddedParticles.h"
#include "TRandom.h"
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TSystem.h"

#include <stdio.h>
#include <iostream>

using namespace std;

int loop_emb(TString outdir="",TString outfile="",TString type="e+", Int_t nEvents=1000,TString vertex_ntuple = "")
{

    TString outDir        = outdir;
    TString outFile       = outfile;

    TString filename = Form("%s/%s",outdir.Data(),outFile.Data());
    filename.ReplaceAll("//","/");

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

    Int_t calcVertex    = 0;

    if(vertex_ntuple.CompareTo("")!=0) calcVertex=1;

    // This macro creates n pluto particle per sector (60 deg) with white disribution
    // in theta , phi and momentum. The particles will be created
    // at the vertex given my the input ntuple extracted from REAL data.


    //---------------CONIGURATION----------------------
    //
    Int_t asciiOut      = 0;    // write pluto ascci output for HGeant (==0 if we use HGeantOutput)
    Int_t rootOut       = 0;    // write pluto root file
    PChannel** channel  = 0;    // no channel needed here
    Int_t    nChannel   = 0;    // number of channels will be 0
    TString myParticle  = type; // particle type used for embedding
    Bool_t  writeSeqNumber = calcVertex==1 ? kTRUE : kFALSE;;  // write eventSeqNumber in addition
    Bool_t  writeIndex     = calcVertex==1 ? kTRUE : kFALSE;;  // write parentIndex in addition

    Float_t pmin               = 0.025;   // minimum mom  [GeV/c]
    Float_t pmax               = 2.000;   // maximum mom  [GeV/c]
    Float_t thetamin           =    10;   // minimum theta angle  [deg]
    Float_t thetamax           =    90;   // maximum theta angle  [deg]
    Float_t phimin             =     0;   // minimum phi angle    [deg]
    Float_t phimax             =    60;   // maximum phi angle    [deg]
    Float_t phistart           =     0;   // start phi angle (+ sector * 60. deg will ber rotated later)
    Int_t numParticlePerSector =     1;   // number of Particles embedded per sector

    //-------------------------------------------------




    PReaction my_reaction(channel,(Char_t*)filename.Data(),nChannel,rootOut,0,calcVertex,asciiOut);
    my_reaction.SetWriteIndex(writeIndex);


    if(calcVertex){
	//Construct the vertex container:
	PVertexFile *vertex = new PVertexFile();
	vertex->OpenFile(vertex_ntuple);

	//add to prologue action
	my_reaction.AddPrologueBulk(vertex);
    }


    PHGeantOutput* output = new PHGeantOutput();
    output->SetWriteSeqNumber(writeSeqNumber);
    output->OpenFile(Form("%s.evt",filename.Data()));
    my_reaction.AddFileOutput(output);


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
    my_reaction.AddBulk(embedded);

    if(calcVertex) {
	TFile *f = new TFile(vertex_ntuple.Data());
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

    // number of events
    my_reaction.Loop(nEvents);
    output->CloseFile();

    gSystem->Exec(Form("rm %s.root",filename.Data()));


    return 0;
}
