#include "hseed.h"

#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TRandom.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>
#include <cmath>



using namespace std;
    typedef struct {
	Double_t ELab;
	Double_t momLab;
	Double_t px;
	Double_t py;
	Double_t pz;
	Double_t thetaScatLab;
    } particle;

    typedef struct {

	Double_t m0  ;
	Double_t me  ;
	Double_t Pi  ;
	Double_t piFac;
	Double_t twoPi;
	Double_t RandMax1     ;
	Double_t LargeRandNorm;

	void init(){
	    m0    = 931.5; //in Mev, mass unit ion
	    me    = 0.511; //in MeV, electron
	    Pi    = 3.14159;
	    piFac = Pi/180.;
	    twoPi = 2.*Pi;
	    RandMax1      = (Double_t)RAND_MAX + 1;          // global var
	    LargeRandNorm = 1.0/Double_t(RAND_MAX*RandMax1); // global var
	}

    } constants;

    typedef struct {
         Int_t nElectronsEmitted;  // no of electrons kicked out of target = sigmaElectron/sigmaIon*targetThickness/nuclearReactionLength

	 //----------------------------------------
	 // read from input file
	 vector <Double_t> refMom;
	 vector <Double_t> probMom;
	 Double_t probMomMax;
         //----------------------------------------

	 //----------------------------------------
	 // precalculated vars used in different places
	 Double_t thetaScatMin;
	 Double_t deltaThetaScat;
	 Double_t dsdthetaMaxNorm;
	 Double_t refMomMin;
	 Double_t deltaRefMom ;
	 Double_t beta0;
	 //----------------------------------------

    } globals;

    typedef struct {
	Bool_t doFilter        ;
        Bool_t writeNtuple     ;
	Double_t FilterMom     ;    //MEV
	Int_t nIonPerEvent     ;     //  -> simulate -950 - +800 ns drifttime range

	Double_t Zion          ;    // Au
	Double_t Ebeam         ; // MeV/nucleon
	Bool_t KshellFlag      ;  //emission from K shell
	Bool_t cutoffFlag      ; // cutoff in scattering angle
	Int_t nBoundElectronsK ; // number of strongly bound electrons still attached to Au ion, less after each foil
	Int_t nBoundElectronsL ; // number of strongly bound electrons still attached to Au ion, less after each foil

	Int_t kScat ; //loop over scatter process at given momentum

    } config;

Int_t readInput(TString filenameIn, vector<Double_t>& refMom, vector<Double_t>& probMom, Double_t& probMomMax )
{
    ifstream input(filenameIn.Data());
    if (input.fail()) {
	cout<<"file "<<filenameIn<<" not found\n";
	return 1;
    }
    Char_t header[1000];
    input.getline(header,1000);
    cout<<"header: "<<header<<"\n";
    probMomMax = 0.;


    refMom.clear();
    probMom.clear();
    Double_t mom,prob;

    while(!input.eof()){
	input>>mom>>prob;
	if (!input.fail()){
	    probMom.push_back(prob);
	    refMom.push_back(mom);
	    if (prob>probMomMax) probMomMax = prob;
	}
    }
    input.close();
    return 0;
}

Int_t precalculation(config& cfg,globals& g,constants& c)
{
    Double_t refMomMax   = g.refMom[g.refMom.size()-1];
    g.refMomMin          = g.refMom[0];         // global var
    g.deltaRefMom        = refMomMax-g.refMomMin; // global var

    Double_t gamma0 = (cfg.Ebeam+c.m0)/c.m0;
    g.beta0  = sqrt(gamma0*gamma0-1.0)/gamma0;       // global var
    Double_t mom0   = g.beta0*gamma0*c.me;

    Double_t Eshell =0;;
    if(cfg.KshellFlag) {
	Eshell = (cfg.Zion-1.)*(cfg.Zion-1.)*0.0136/1000. + c.me; // appr. screening for at least 2 k-shell electrons
    } else {
	Eshell = (cfg.Zion-3.)*(cfg.Zion-3.)*0.0136/4./1000. + c.me; // appr. screening for at least 2 LI-shell electrons +2 K electrons
    }
    Double_t meanMom = sqrt(Eshell*Eshell-c.me*c.me);
    Double_t deltaMomFac = 1.-0.5*meanMom*meanMom/mom0/mom0; //meanMom is minimum momentum transfer
    if(deltaMomFac < -1.) {
	cout<<"no electon emission possible\n";
	return 1;
    }
    g.thetaScatMin = acos(deltaMomFac); // global var minimum momentum transfer for bound electron
    if (cfg.cutoffFlag) {
	if(g.thetaScatMin < 18.*c.piFac) g.thetaScatMin=18.*c.piFac; // artificial cutoff
    } else {
	if(g.thetaScatMin < 4.*c.piFac) g.thetaScatMin=4.*c.piFac; // take screening into account (very crude)
    }
    g.deltaThetaScat     = c.Pi - g.thetaScatMin;      // global var
    Double_t stheta    = sin(g.thetaScatMin/2.);
    Double_t dsdthetaMax = sin(g.thetaScatMin)/mom0/g.beta0/(stheta*stheta*stheta*stheta); //sin(theta) converts dOmega to dTheta
    g.dsdthetaMaxNorm = dsdthetaMax*c.LargeRandNorm;   // global var
    cout<<"mom="<<mom0<<" kin energy="<<(gamma0-1.)*c.me <<" mean mom="<<meanMom<<"MeV/c theta-cutoff="<<g.thetaScatMin/c.piFac << " nElectronsEmitted "<<g.nElectronsEmitted<<endl;
    return 0;
}


Int_t targ_delta_emission(Int_t maxEvents=1000,
			  TString shell='K',
			  TString inputDir   = "/misc/kempter/projects/geant_beam_ions/generator/input/",
			  TString outputDir  = "/misc/kempter/projects/geant_beam_ions/generator/evt/",
			  TString outputFile = "delta_electrons",
                          Int_t fileNumber   = 1
			 )
{
    shell.ToUpper();

    // this program generates a hgeant compatible .evt ascii file
    // with delta electrons. The electrons will keep source ID =-3
    // and can be identifed later by this geninfo in HGeantKine.
    // K (as hgeant) or L shell delta electrons can be calculated.
    // The events can be filtered by a low momenta cutoff to remove
    // unwanted electrons which will not make it into any detector.


    //----------------------------------------
    Int_t method    = 0;
    Int_t fallback  = 2;
    Int_t fixedSeed = 10;
    HSeed seedgen(method,fallback,fixedSeed);  // 0 (default) : /dev/random,
    //                                            1 : TRandom3 (local) with systime ,
    //                                            2 : TRandom3 (local) with systime + processid + iplast,
    //                                            3 : like 1 but gRandom,
    //                                            4 : like 2 but gRandom,
    //                                            5 : fixed (needs seed to be set)
    Int_t seed = seedgen.getSeed();
    gRandom->SetSeed(seed);
    srand(seed);
    cout<<"Running "<<shell<<" "<<maxEvents<<" events. Seed = "<<seed<<endl;
    //----------------------------------------

    //----------------------------------------
    constants c; // constants
    c.init();
    globals g;   // vars used in different parts
    config cfg;  // configuration vars
    //----------------------------------------


    //----------------------------------------
    cfg.doFilter         = kTRUE;
    cfg.writeNtuple      = kFALSE;
    cfg.FilterMom        = 1.5;    //MEV
    cfg.nIonPerEvent     = 360;     //  -> simulate -950 - +400 ns drifttime range

    cfg.Zion             = 79.;    // Au
    cfg.Ebeam            = 1230.0; // MeV/nucleon
    cfg.KshellFlag       = kTRUE;  //emission from K shell
    cfg.cutoffFlag       = kFALSE; // cutoff in scattering angle
    
    cfg.nBoundElectronsK = 2;  // number of strongly bound electrons still attached to Au ion, less after each foil
    cfg.nBoundElectronsL = 8;  // number of strongly bound electrons still attached to Au ion, less after each foil

    cfg.kScat = 10; //loop over scatter process at given momentum

    TString fileMomDisK   = "Au_k_shell_mom_dis_mod.dat";
    TString fileMomDisL   = "Au_LI_shell_mom_dis.dat";
    //----------------------------------------

    //----------------------------------------
    // filenames
    TString filter = "";
    if(cfg.doFilter) filter = "_filter";

    TString filenameEvents= Form("%s/%s_%s_shell_%iions_%ievents%s_%i.evt" ,outputDir.Data(),outputFile.Data(),shell.Data(),cfg.nIonPerEvent,maxEvents,filter.Data(),fileNumber);
    TString filenameTuple = Form("%s/%s_%s_shell_%iions_%ievents%s_%i.root",outputDir.Data(),outputFile.Data(),shell.Data(),cfg.nIonPerEvent,maxEvents,filter.Data(),fileNumber);
    filenameTuple .ReplaceAll("//","/");
    filenameEvents.ReplaceAll("//","/");
    //----------------------------------------



    //----------------------------------------
    // setup K or L shell
    TString fileMomDis = "";
    if (shell=='K') {
	cfg.KshellFlag = kTRUE;
	fileMomDis     = fileMomDisK;
	if (cfg.cutoffFlag) { // artificial cutoff
	    g.nElectronsEmitted = Int_t(41.*Double_t(cfg.nBoundElectronsK)); // no of electrons kicked out of target
	}else{ // = sigmaElectron/sigmaIon*targetThickness/nuclearReactionLength
	    g.nElectronsEmitted = Int_t(52.6*Double_t(cfg.nBoundElectronsK)); // no of electrons kicked out of target
	}
    } else {
	cfg.KshellFlag = kFALSE;
	fileMomDis     = fileMomDisL;
	if (cfg.cutoffFlag) { // artificial cutoff at theta-cm = 18 degree
	    g.nElectronsEmitted = Int_t(41.*Double_t(cfg.nBoundElectronsL)); // no of electrons kicked out of target
	} else {  // = sigmaElectron/sigmaIon*targetThickness/nuclearReactionLength, target thickness 1.35%
	    g.nElectronsEmitted = Int_t(240.0*Double_t(cfg.nBoundElectronsL)); // no of electrons kicked out of target
	}
    }
    TString filenameIn = inputDir + "/" + fileMomDis;
    filenameIn.ReplaceAll("//","/");
    //----------------------------------------



    //----------------------------------------
    // reading mom,pro,theta from input file
    if(readInput(filenameIn,g.refMom,g.probMom,g.probMomMax)) return 1;
    //----------------------------------------

    //----------------------------------------
    // precalculation for configured beam/target
    if(precalculation(cfg,g,c)) return 1;
    //----------------------------------------


    //----------------------------------------
    // create output files
    ofstream output(filenameEvents.Data());
    if (output.fail()) {
	cout<<"file "<<filenameEvents<<" not created\n";
	return 1;
    }
    output.precision(4);
    TFile* tupOutput =0;
    TNtuple* detuple =0;
    Float_t data[3];
    if(cfg.writeNtuple){
	tupOutput = new TFile(filenameTuple.Data(),"RECREATE");
	tupOutput->cd();
	detuple=new TNtuple("DeltaEle","DeltaEle","momLab:eLab:theta");
	
    }
    //----------------------------------------



    vector<particle> event; // holds all electrons of one beam ion interaction
    event.reserve(1000);

    Int_t nEvt=0;

    for (Int_t i = 0; i < maxEvents; i ++) {  // event loop
	if (i%100==0 ) cout<<"event "<<i<<endl; // to make waiting time less boring

	for (Int_t j = 0; j < cfg.nIonPerEvent; j ++) {  // ion loop

	    for (Int_t k = 0; k < g.nElectronsEmitted; k += cfg.kScat) { //nElectronsEmitted
                Int_t nScat = cfg.kScat;
                if(k+cfg.kScat>=g.nElectronsEmitted) nScat = g.nElectronsEmitted - k;
		//-------------------------------------------------------------
		// mom distribution
		UInt_t n=0;
                Double_t mom = 0 ;
		Double_t probMomRef = 0;
		do {
		    mom = gRandom->Rndm()*g.deltaRefMom + g.refMomMin;
		    for (n=1;n<g.refMom.size();++n) {
			if(mom<g.refMom[n]) break;
		    }
		    probMomRef=(g.probMom[n]*(mom-g.refMom[n-1]) + g.probMom[n-1]*(g.refMom[n]-mom))/(g.refMom[n]-g.refMom[n-1]);
		} while(gRandom->Rndm()*g.probMomMax > probMomRef);
		//-------------------------------------------------------------
		//    mom=0.; // test without momentum distribution

		for (Int_t l=0;l<nScat;++l)
		{
		    //-------------------------------------------------------------
		    // theta distribution
                    Double_t theta  = 0;
                    Double_t sTheta = 0;
		    do {
			theta=gRandom->Rndm()*c.Pi;  //in cm system
			sTheta=sin(theta);
		    } while(gRandom->Rndm() > sTheta);
		    //-------------------------------------------------------------

		    Double_t momYCm   = mom*sTheta; // projectile frame, projectile hit by target electrons
		    Double_t momZCm   = mom*cos(theta);
		    Double_t betaZCm  = 1./sqrt(c.me/momZCm*c.me/momZCm + 1.);
		    if(momZCm < 0.) betaZCm = -betaZCm;
		    betaZCm  = (g.beta0 + betaZCm)/(1. + g.beta0*betaZCm); //Lorentz-Transformation, recycling variables
		    Double_t gammaZCm = 1./sqrt(1. - betaZCm*betaZCm);
		    momZCm   = betaZCm*gammaZCm*c.me;
		    Double_t momCm    = sqrt(momZCm*momZCm + momYCm*momYCm);
		    Double_t betaCm   = momCm/sqrt(momCm*momCm + c.me*c.me);
		    Double_t thetaCm  = acos(momZCm/momCm);


		    //-------------------------------------------------------------
		    Double_t dsdtheta = 0;
		    Double_t thetaScat= 0;
			do {
			thetaScat = gRandom->Rndm()*g.deltaThetaScat + g.thetaScatMin;  //in cm system
			sTheta    = sin(thetaScat/2.);
			dsdtheta  = sin(thetaScat)/momCm/betaCm/(sTheta*sTheta*sTheta*sTheta); //sin(theta) converts dOmega to dTheta
		    } while ((Double_t(rand())*c.RandMax1 + Double_t(rand()))*g.dsdthetaMaxNorm > dsdtheta);
		    //-------------------------------------------------------------

		    Double_t phiScat = gRandom->Rndm()*c.twoPi;
		    Double_t px = momCm*sin(phiScat)*sin(thetaScat);
		    Double_t py = momCm*cos(phiScat)*sin(thetaScat);
		    Double_t pz = momCm*cos(thetaScat);
		    // rotate back to lab system by thetaCm and phiCm (not yet callculated)
		    // rotate back around x-axis by thetaCm (could be y-axis as well since phi is arbitrary)
		    Double_t px0 = px;
		    Double_t py0 = py*cos(-thetaCm) - pz*sin(-thetaCm);
		    Double_t pz0 = py*sin(-thetaCm) + pz*cos(-thetaCm);
		    // rotate back by phi around z-axis, recycle px, py, pz
		    Double_t phiCm = gRandom->Rndm()*c.twoPi;
		    px = px0*cos(-phiCm) - py0*sin(-phiCm);
		    py = px0*sin(-phiCm) + py0*cos(-phiCm);
		    // go from projectile to target frame via Lorentz transformation
		    betaZCm = 1./sqrt(c.me/pz0*c.me/pz0 + 1.);
		    if(pz0 > 0.) betaZCm = -betaZCm; // transformation is backwards
		    Double_t betaZLab  = (g.beta0 + betaZCm)/(1. + g.beta0*betaZCm);
		    Double_t gammaZLab = 1./sqrt(1. - betaZLab*betaZLab);
		    pz = betaZLab*gammaZLab*c.me;

		    Double_t momLab       = sqrt(px*px + py*py + pz*pz);
		    Double_t thetaScatLab = asin(sqrt(px*px+py*py)/momLab);
		    if(pz<0.) thetaScatLab = c.Pi-thetaScatLab;
		    Double_t ELab = sqrt(momLab*momLab + c.me*c.me);

		    //----------------------------------------
                    // fill particle to current event
		    if(momLab > cfg.FilterMom || !cfg.doFilter){

			particle part;
			part.ELab   = ELab/1000.;
			part.momLab = momLab;
			part.px   = px/1000.;
			part.py   = py/1000.;
			part.pz   = pz/1000.;
			part.thetaScatLab =thetaScatLab;
			event.push_back(part);
		    }
		    //----------------------------------------

		} // end scatter loop
	    } // end emitted electrons loop
	} // end ion loop

	//----------------------------------------
	// write event
	if(event.size()>0){
          Int_t flag=3;
	  //  FILE FORMATS
          //
	  //      the format of the evt input file is given
	  //      by the flag in the header
          //
	  //  Header:
          //
	  //      i           i         f     f               i
	  //      eventNumber,nParticle,ebeam [GeV],impactParameter [fm],flag
          //
	  //  Particle:  units :  Etot [GeV]
          //
	  //      flag notations:  flag = s1 + s2*10
	  //      s2 == s1 ==> userVal added,
	  //          s1 < 0 ==> indexParent added
	  //          s2 = flag/10
	  //          s1 = flag - s2*10
	  //          flag  s1 s2                f  f  f  f  f     f  f  f   i  i        i        i            f     f
	  //          1      1  0             Etot,px,py,pz,                ID,                              ,weight
	  //          11     1  1             Etot,px,py,pz,                ID,                              ,weight,userVal
	  //          2      2  0             Etot,px,py,pz,                ID,sourceID,parentID,            ,weight
	  //          22     2  2             Etot,px,py,pz,                ID,sourceID,parentID,            ,weight,userVal
	  //         -2     -2  0             Etot,px,py,pz,                ID,sourceID,parentID,indexParent ,weight
	  //         -22    -2  2             Etot,px,py,pz,                ID,sourceID,parentID,            ,weight,userVal
	  //          3      3  0             Etot,px,py,pz      ,vx,vy,vz, ID,sourceID,parentID,            ,weight
	  //          33     3  3             Etot,px,py,pz      ,vx,vy,vz, ID,sourceID,parentID,indexParent ,weight,userVal
	  //         -3     -3  0             Etot,px,py,pz      ,vx,vy,vz, ID,sourceID,parentID,indexParent ,weight
	  //         -33    -3  3             Etot,px,py,pz      ,vx,vy,vz, ID,sourceID,parentID,indexParent ,weight,userVal
	  //          4      4  0             Etot,px,py,pz, tof ,vx,vy,vz, ID,sourceID,parentID,            ,weight
	  //          44     4  4             Etot,px,py,pz, tof ,vx,vy,vz, ID,sourceID,parentID,indexParent ,weight,userVal
	  //         -4     -4  0             Etot,px,py,pz, tof ,vx,vy,vz, ID,sourceID,parentID,indexParent ,weight
	  //         -44    -4  4             Etot,px,py,pz, tof ,vx,vy,vz, ID,sourceID,parentID,indexParent ,weight,userVal
	  //------------------------------------------------------------------------------------
	    ++nEvt;
	    output<<nEvt<<" "<<event.size()<<" "<<cfg.Ebeam/1000.<<" "<<0<<" "<<flag<<"\n";
	    for(UInt_t l=0;l<event.size();l++){
		particle& part = event[l];
                //      eLab            px            py            pz            vx      vy      vz     Id       srcID  parentID weight
		output<<part.ELab<<" "<<part.px<<" "<<part.py<<" "<<part.pz<<" "<<0<<" "<<0<<" "<<0<<" "<<3<<" "<<-3<<" "<<0<<" "<<0<<"\n";

		if(cfg.writeNtuple){
		    data[0] = part.momLab; // MeV/c
		    data[1] = part.ELab; // MeV
		    data[2] = part.thetaScatLab/c.piFac;
		    detuple->Fill(data);
		}
	    }
	}
	event.clear();
	//----------------------------------------

    } // end eventloop

    output.close();
    cout<<"Events="<<nEvt<<"\n";

    if(cfg.writeNtuple){
	tupOutput->cd();
	detuple->Write();
	tupOutput->Save();
	tupOutput->Close();
    }
    return 0;
}
