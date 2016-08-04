//Author: M. Sanchez && W. Koenig

class Correlator {
public:
  TH1F *multiplicity;
  TH1F *mulRich;
  TH1F *mulMdcRing; //Mdc multiplicity for Mdc Seg after matching with one ring
  TH1F *mulMdcRich;
  TH1F *mulRichMdc;
  TH1F *oangleRich;
  TH1F *oangleMdc; //theta,phi from Mdc after matching with Rich
  TH1F *oangleMdcAll;
  TH1F *thetaRM; //delta theta Rich-Mdc
  TH1F *phiRM; //delta phi Rich-Mdc
  TH1F *thetaRT;
  TH1F *phiRT;
  TH1F *tofRTT; //time of flight for Rich-Track-Tof matching
  TH1F *tofRTS; //time of flight for Rich-Track-Shower(Tofino) matching
  TH1F *momP;
  TH1F *momN;
  TH1F *momPairP;
  TH1F *momPairN;
  TH1F *imass;
  TH1F *imassPN;
  TH1F *imassPP;
  TH1F *imassNN;
  TH1F *oangle;
  TH1F *oanglePN;
  TH1F *oanglePP;
  TH1F *oangleNN;
  TH1F *conv;
  TH1F *sector;
  TH1F *pullGood; //the better pull value of like sign track pairs with one Mdc hit
  TH1F *pullBad; //the worse pull value of like sign track pairs with one Mdc hit
  TH1F *pullRich; //pull result for all matches with Rich

  Correlator(void);
  ~Correlator(void);
  void eventLoop(Int_t maxEnt=0);
  void fillMass(TObjArray &leptons);
  Bool_t matchTrack(HRichHit *hit,HKickTrack *track);
  Bool_t matchMdc(HRichHit *hit,HMdcSeg *mdc);
  void opAngRich(TObjArray &rings);
  void opAngRichMdc(TObjArray & matchedRings);
};


void Correlator::Correlator(void) {
  oangleRich = new TH1F("Rich opening","Opening angle Rich",180,1,180);
  oangleMdc = new TH1F("Mdc opening","Opening angle Mdc && Rich && close pair cut",180,1,180);
  oangleMdcAll = new TH1F("Mdc opening all","Opening angle Mdc && Rich",180,1,180);
  thetaRM=new TH1F("thetaRichMdc","Rich-Mdc polar angle difference",180,-90,90);
  phiRM=new TH1F("phiRichMdc","Rich-Mdc azimuthal angle difference",180,-60,60);
  thetaRT=new TH1F("thetaRichTrack","Rich-Track polar angle difference",180,-90,90);
  phiRT=new TH1F("phiRichTrack","Rich-Track azimuthal angle difference",180,-60,60);
  tofRTT=new TH1F("tofTofRichTrack","time of flight of TOF (Rich-Track matching)",100,0,50);
  tofRTS=new TH1F("tofShowerRichTrack","time of flight of TOFino (Rich-Track matching)",100,0,50);
  oangle = new TH1F("opening","opening angle all matched pairs",60,1,60);
  oanglePN = new TH1F("openingPN","PN opening angle",60,1,60);
  oanglePP = new TH1F("openingPP","PP opening angle",60,1,60);
  oangleNN = new TH1F("openingNN","NN opening angle",60,1,60);
  momPairP=new TH1F("momPairP","positron momentum (pairs)",100,0,1000);
  momPairN=new TH1F("momPairN","electron momentum (pairs)",100,0,1000);
  imass=new TH1F("imass","invariant mass all pairs",100,1,1000);
  imassPN=new TH1F("imassPN","invariant mass PN",100,1,1000);
  imassPP=new TH1F("imassPP","invariant mass PP",100,1,1000);
  imassNN=new TH1F("imassNN","invariant mass NN",100,1,1000);
  pullGood=new TH1F("pullGood","matching quality of better like-sign track",80,-10,10);
  pullBad=new TH1F("pullBad","matching quality of worse like-sign track",80,-10,10);
  pullRich=new TH1F("pullRich","matching quality of all tracks matched with Rich",80,-10,10);

// conversion:
//  0: like sign pair with only one ring (garbage)
//  1: one pair with only one ring
//  2: like sign pair with only one ring (recoverable fake) + another lepton
//  3: one pair with only one ring + another lepton
//  4,5: like 2,3 with 2 other leptons
//  6: spare to avoid zero suppression

  conv = new TH1F("conv","conversion",12,0,6);

}

void Correlator::~Correlator(void) {
}

void Correlator::fillMass(TObjArray &leptons) {
  HKickTrack *tr[4];
  Float_t th[4];
  Float_t ph[4];
  Int_t ch[4];

  Float_t offset;
  Double_t sec2rad = TMath::Pi() / 3.;
  Int_t nTracks=leptons.GetEntries();
  if (nTracks > 4) nTracks=4;

  Int_t i;
  for (i=0; i<nTracks; ++i) {
    tr[i]=(HKickTrack *)leptons.At(i));
    th[i] = tr[i]->getTheta();
    ph[i] = tr[i]->getPhi();
    ch[i] = tr[i]->getCharge();
    if (tr[i]->getSector() == 5) offset = -sec2rad;
    else offset = tr[i]->getSector()*sec2rad;

    ph[i] += offset;
  }

  if(nTracks>3) {
// reject conversion before calculating opening angles and mass.
// Delta theta,phi = 3 degree.
// Recover lepton from like sign pair with same Tof/Shower hit.

    if(TMath::Abs(th[3]-th[2])<0.052F && TMath::Abs(ph[3]-ph[2])<0.052F) {

// check for opposite sign, otherwise take the better one.
      if(ch[3]==ch[2]) {
        --nTracks;
        if(TMath::Abs(tr[3].pull) < TMath::Abs(tr[2].pull)) {
	  pullBad->Fill(tr[2].pull);
          if (TMath::Abs(tr[2].pull)<3.0F) conv->Fill(4.F);
	  tr[2]=tr[3];
	  th[2]=th[3];
	  ph[2]=ph[3];
	} else {
	  pullBad->Fill(tr[3].pull);
          if (TMath::Abs(tr[3].pull)<3.0F) conv->Fill(4.F);
	}
	pullGood->Fill(tr[2].pull);
      } else if (TMath::Abs(tr[3].pull)<3.0F && TMath::Abs(tr[2].pull)<3.0F) {
	conv->Fill(5.F);
        nTracks-=2;
      }
    }
  }
// reject conversion before calculating opening angles and mass
  if(nTracks>2 && TMath::Abs(th[2]-th[1])<0.052F && TMath::Abs(ph[2]-ph[1])<0.052F) {

// check for opposite sign, otherwise take the better one.
    if(ch[2]==ch[1]) {
      --nTracks;
      if(TMath::Abs(tr[2].pull) < TMath::Abs(tr[1].pull)) {
        pullBad->Fill(tr[1].pull);
        if (TMath::Abs(tr[1].pull)<3.0F) conv->Fill(2.F);
	tr[1]=tr[2];
	th[1]=th[2];
	ph[1]=ph[2];
      } else {
        pullBad->Fill(tr[2].pull);
        if (TMath::Abs(tr[2].pull)<3.0F) conv->Fill(2.F);
      }
      if(nTracks>2) {
	tr[2]=tr[3];
	th[2]=th[3];
	ph[2]=ph[3];
	ch[2]=ch[3];
      }
      pullGood->Fill(tr[1].pull);
    } else if (TMath::Abs(tr[2].pull)<3.0F && TMath::Abs(tr[1].pull)<3.0F) {
      conv->Fill(3.F);
      nTracks-=2;;
    }
  }
  if(nTracks<2) return;
  Int_t n, id;

  if(TMath::Abs(th[0]-th[1])<0.052F && TMath::Abs(ph[0]-ph[1])<0.052F) {
    if(leptons.GetEntries()>2) id=2;
    else id=0;
// check for opposite sign, otherwise take the better one.
    if(ch[0]==ch[1]) {
      --nTracks;
      if(TMath::Abs(tr[1].pull) < TMath::Abs(tr[0].pull)) {
        pullBad->Fill(tr[0].pull);
        if (TMath::Abs(tr[0].pull)<3.0F) conv->Fill(id);
	tr[0]=tr[1];
	th[0]=th[1];
	ph[0]=ph[1];
      } else {
	pullBad->Fill(tr[1].pull);
        if (TMath::Abs(tr[1].pull)<3.0F) conv->Fill(id);
      }
      pullGood->Fill(tr[0].pull);
      for(i=1; i< nTracks; ++i) {
	n=i+1;
        tr[i]=tr[n];
	th[i]=th[n];
	ph[i]=ph[n];
	ch[i]=ch[n];
      }
    } else if(TMath::Abs(tr[1].pull)<3.0F && TMath::Abs(tr[0].pull)<3.0F) {
      nTracks-=2;
      conv->Fill(id+1);
    }
  }
  if (nTracks<2) return;

  HGeomVector pp[4];
  Float_t rad2deg=180./TMath::Pi();
  Float_t mass;
  Float_t cOpening;
  Float_t openAngle;

  for (i=0; i<nTracks; ++i) {
    pp[i].setX( sin(th[i])*cos(ph[i]));
    pp[i].setY( sin(th[i])*sin(ph[i]));
    pp[i].setZ( cos(th[i]) );
  }

  for (i=0; i<nTracks-1; ++i) {
    for (n=i+1; n<nTracks; ++n) {
      cOpening = pp[i].scalarProduct(pp[n]);
      if (TMath::Abs(cOpening) <= 1) {
        openAngle=acos(cOpening);
        mass=2.0*sqrt(tr[i]->getP() * tr[n]->getP() * sin(openAngle/2));
        openAngle=openAngle * rad2deg;
        oangle->Fill(openAngle);
        imass->Fill(mass);
        if (ch[i] != ch[n]) {
          oanglePN->Fill(openAngle);
          imassPN->Fill(mass);
          if (ch[i] > 0) {
            momPairP->Fill(tr[i]->getP());
            momPairN->Fill(tr[n]->getP());
          } else {
            momPairP->Fill(tr[n]->getP());
            momPairN->Fill(tr[i]->getP());
          }
        } else {
          if(ch[i]==-1) {
            oangleNN->Fill(openAngle);
            imassNN->Fill(mass);
          } else {
            oanglePP->Fill(openAngle);
            imassPP->Fill(mass);
          }
        }
      } else cerr << "cos(openAngle) = " << cOpening << endl;
    }
  }
}

void Correlator::opAngRich(TObjArray &rings) {
  Float_t deg2rad = TMath::Pi()/180.;
  HRichHit * hit;
  Float_t th[4];
  Float_t ph[4];
  Float_t cOpening;

  Int_t nHits = rings.GetEntries();
  if (nHits > 4) nHits=4;

  Int_t i,n;
  for (i=0;i<nHits;++i) {
    hit = (HRichHit *)rings.At(i);
    th[i]=hit->getTheta()*deg2rad;
    ph[i]=hit->getPhi()*deg2rad;
  }

  for (i=0; i<nHits-1; ++i) {
    for (n=i+1; n<nHits; ++n) {
      cOpening=sin(th[i])*sin(th[n]) * (cos(ph[i])*cos(ph[n]) + sin(ph[i])*sin(ph[n]))
        + cos(th[i])*cos(th[n]);

      oangleRich->Fill(acos(cOpening)/deg2rad);
    }
  }
}

void Correlator::opAngRichMdc(TObjArray &matchedRings) {
  Float_t deg2rad = TMath::Pi()/180.;
  Float_t phiShift[6]={0.F,60.F,120.F,180.F,240.F,-60.F};

  HMdcSeg * mdcHit;
  Float_t th[4];
  Float_t ph[4];
  Float_t cOpening;

  Int_t nHits = matchedRings.GetEntries();
  if (nHits > 4) nHits=4;

  Int_t i,n;
  for (i=0;i<nHits;++i) {
    mdcHit = (HMdcSeg *)matchedRings.At(i);
    th[i]=mdcHit->getTheta();
    ph[i]=mdcHit->getPhi() + phiShift[mdcHit->getSec()]*deg2rad;
  }
  Float_t openAngle[3][3];
  Bool_t closeHit[4];
  Int_t k;

  for (i=0; i<nHits; closeHit[i++]=kFALSE);

  for (i=0; i<nHits-1; ++i) {
    for (n=i+1; n<nHits; ++n) {
      cOpening=sin(th[i])*sin(th[n]) * (cos(ph[i])*cos(ph[n]) + sin(ph[i])*sin(ph[n]))
        + cos(th[i])*cos(th[n]);

      k=n-1;
      if((openAngle[i][k]=acos(cOpening)/deg2rad)<10) {
        oangleMdc->Fill(openAngle[i][k]);
        closeHit[i]=kTRUE;
        closeHit[n]=kTRUE;
      }
      oangleMdcAll->Fill(openAngle[i][k]);
    }
  }
  for (i=0; i<nHits-1; ++i) {
    if(closeHit[i]) break;
    for (n=i+1; n<nHits; ++n) {
      if(closeHit[n]) break;
      oangleMdc->Fill(openAngle[i][n-1]);
    }
  }
}

Bool_t Correlator::matchTrack(HRichHit *hit,HKickTrack *track) {
  Double_t rad2deg = 180./TMath::Pi();
  Float_t phiShift[6]={0.0F,59.5F,120.0F,179.5F,240.0F,-60.0F};
  Float_t thDiff, phDiff;
  Bool_t r = kFALSE, tofCut;

  Int_t nSec=track->getSector();
  if(nSec == hit->getSector() && nSec < 6 && nSec >= 0) {
    phDiff=hit->getPhi()-track->getPhi()*rad2deg-phiShift[nSec];
    thDiff=hit->getTheta() - track->getTheta()*rad2deg;
    tofCut=(track->getTof() > 3.F && track->getTof() < 8.F);
    if (TMath::Abs(phDiff) < 4) {
      if (tofCut) thetaRT->Fill(thDiff);
      if (TMath::Abs(thDiff) < 3) {
	if (track.getSystem()==0) tofRTS->Fill(track->getTof());
        else tofRTT->Fill(track->getTof());
        if (tofCut) {
	  r = kTRUE;
	  pullRich->Fill(track->pull);
	}
      }
    }
    if(TMath::Abs(thDiff) < 3 && tofCut) phiRT->Fill(phDiff);
  }
  return r;
}

Bool_t Correlator::matchMdc(HRichHit *hit,HMdcSeg *mdc) {
  Double_t rad2deg = 180./TMath::Pi();
  Float_t phiShift[6]={0.0F,59.5F,120.0F,179.5F,240.0F,-60.0F};
  Float_t thDiff, phDiff;
  Bool_t r = kFALSE;

  Int_t nSec=mdc->getSec();
  if(nSec == hit->getSector() && nSec < 6 && nSec >= 0) {
    phDiff=hit->getPhi() - mdc->getPhi()*rad2deg-phiShift[nSec];
    thDiff=hit->getTheta() - mdc->getTheta()*rad2deg;
    if (TMath::Abs(phDiff) < 4) {
      thetaRM->Fill(thDiff);
      if (TMath::Abs(thDiff) < 3) r = kTRUE;
    }
    if(TMath::Abs(thDiff) < 3) phiRM->Fill(phDiff);
  }
  return r;
}

void Correlator::eventLoop(Int_t maxEnt) {
///////////////////////////////////////////////////////////
// This file has been automatically generated 
// (Thu Mar 22 07:21:07 2001 by ROOT version3.00/02) My Good, version 3!!
// from HTree T/T.2
// found on file: /mnt/scratch/xx00323033656_DST.root
///////////////////////////////////////////////////////////


//Reset ROOT and connect tree file
  TChain *T = new TChain("T");
  T->Add("/scratch/rhea/data/ExpData/dst/nov00/gen3/xx00323033656_DST.root");
    T->Add("/scratch/rhea/data/ExpData/dst/nov00/gen3/xx00323021241_DST.root");
    T->Add("/scratch/rhea/data/ExpData/dst/nov00/gen3/xx00323030254_DST.root");
    T->Add("/scratch/rhea/data/ExpData/dst/nov00/gen3/xx00323041355_DST.root");
    T->Add("/scratch/rhea/data/ExpData/dst/nov00/gen3/xx00323043128_DST.root");
    T->Add("/scratch/rhea/data/ExpData/dst/nov00/gen3/xx00323045047_DST.root");
    T->Add("/scratch/rhea/data/ExpData/dst/nov00/gen3/xx00324044137_DST.root");
    T->Add("/scratch/rhea/data/ExpData/dst/nov00/gen3/xx00324044546_DST.root");
    T->Add("/scratch/rhea/data/ExpData/dst/nov00/gen3/xx00324051724_DST.root");

  TFile *f = new TFile("/scratch/rhea/data/ExpData/dst/nov00/gen3/xx00323033656_DST.root");

   HRecEvent* fEvent = (HRecEvent*)f->Get("Event");
   if(fEvent) HEventHeader* fHead = fEvent->getHeader();
   T->SetBranchAddress("Event",&fEvent);

   f->cd("dirRich"); 
   HRichHit* ptlHRichHit = new HRichHit();
   HLinearCategory* catlHRichHit = (HLinearCategory*) gDirectory->Get("HRichHit");
   TBranch* brlHRichHit = T->GetBranch("HRichHit");
   T->SetBranchAddress("HRichHit",&catlHRichHit);

   HRichHitHeader* ptlHRichHitHeader = new HRichHitHeader();
   HLinearCategory* catlHRichHitHeader = (HLinearCategory*) gDirectory->Get("HRichHitHeader");
   TBranch* brlHRichHitHeader = T->GetBranch("HRichHitHeader");
   T->SetBranchAddress("HRichHitHeader",&catlHRichHitHeader);

   f->cd("dirMdc");
   HMdcSeg* ptmHMdcSeg = new HMdcSeg();
   HMatrixCategory* catmHMdcSeg=(HMatrixCategory*) gDirectory->Get("HMdcSeg");
   TBranch* brmHMdcSeg = T->GetBranch("HMdcSeg");
   T->SetBranchAddress("HMdcSeg",&catmHMdcSeg);

   f->cd("dirTracks"); 
   HKickTrack* ptmHKickTrack = new HKickTrack();
   HMatrixCategory* catmHKickTrack = (HMatrixCategory*) gDirectory->Get("HKickTrack");
   TBranch* brmHKickTrack = T->GetBranch("HKickTrack");
   T->SetBranchAddress("HKickTrack",&catmHKickTrack);

   f->cd();
   delete f;

   T->SetBranchStatus("*",0);
   T->SetBranchStatus("Event*",1);
   T->SetBranchStatus("HRichHit*",1);
   T->SetBranchStatus("HRichHitHeader",1);
   T->SetBranchStatus("HMdcSeg*",1);
   T->SetBranchStatus("HKickTrack*",1);
   

//     This is the loop skeleton
//     To read only selected branches, Insert statements like:
// T->SetBranchStatus("*",0);  // disable all branches
// T->SetBranchStatus("branchname",1);  // activate branchname

   Int_t nentries = maxEnt;
   if (nentries==0) nentries = T->GetEntries();
   cout << "Total entries " << T->GetEntries() << endl;


 Int_t nbytes = 0;
 HIterator *iterKick = catmHKickTrack->MakeIterator();
 HIterator *iterMdc = catmHMdcSeg->MakeIterator();
 HIterator *iterRich = catlHRichHit->MakeIterator();
 TObjArray leptons;
 TObjArray rings;
 TObjArray matchedRings;

// multiplicity for matching with Mdc:
 Int_t mulRing;
 Int_t mulMatchRich;
 
 sector = new TH1F("sector","pair sectors",7,0,6);
 multiplicity=new TH1F("m","mul",20,0,10);
 momP=new TH1F("momP","positron momentum",100,0,1000);
 momN=new TH1F("momN","electron momentum",100,0,1000);
 mulRich=new TH1F("mulRich","ring multiplicity",20,0,10);
 mulMdcRich=new TH1F("mulMdcRich","Mdc multiplicity all rings",20,0,10);
 mulRichMdc=new TH1F("mulRichMdc","Ring multiplicity matching Mdc",20,0,10);
 mulMdcRing=new TH1F("mulMdcRing","Mdc multiplicity per ring",20,0,10);

 for (Int_t i=0; i<nentries;i++) {
   if ( (i%5000) == 0 ) cout << i << endl;
   T->GetEntry(i);
   HKickTrack *track=0;
   HRichHit *hit=0;
   HMdcSeg *mdc=0;
   
   iterRich->Reset();
   if (catlHRichHit->getEntries() > 0) {
     mulMatchRich=0;
     while ( (hit=(HRichHit *)iterRich->Next()) != 0) {
//exclude noisy part of sector 2
//  if(hit->getSector() != 2 || hit->getTheta()  > 22.5) {
       rings.AddLast(hit);
       mulRing=0;
       iterMdc->Reset();
       while ( (mdc=(HMdcSeg *)iterMdc->Next()) != 0) {
	 if (matchMdc(hit,mdc)) {
           matchedRings.AddLast(mdc);
           ++mulRing;
         }
       }
       mulMdcRing->Fill(mulRing);
       if(mulRing>0) {
        ++mulMatchRich;
        iterKick->Reset();
        while ( (track=(HKickTrack *)iterKick->Next()) != 0) {
	 if (matchTrack(hit,track)) {
	   leptons->AddLast(track);
	   sector->Fill(track.getSector());
	   if(track.getCharge()<0) momN->Fill(track.getP());
	   else momP->Fill(track.getP());
	 }
        }
       }
//}
}
     mulMdcRich->Fill(matchedRings.GetEntries());
     mulRichMdc->Fill(mulMatchRich);
     mulRich->Fill(rings.GetEntries());
   
     if (rings.GetEntries() >= 2) opAngRich(rings);   
     if (matchedRings.GetEntries() >= 2) opAngRichMdc(matchedRings);   
     if (leptons.GetEntries()>0) multiplicity->Fill(leptons.GetEntries());
     if (leptons.GetEntries()>=2) fillMass(leptons);
     leptons.Clear();
     rings.Clear();
     matchedRings.Clear();
   }
   catmHKickTrack->Clear();   
   catmHMdcSeg->Clear();   
   catlHRichHit->Clear();    
 }
}











