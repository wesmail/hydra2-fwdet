#include <TROOT.h>
#include <TSystem.h>
#include <TMath.h>
#include <TFile.h>
#include <TH3F.h>
#include <TString.h>
#include "/misc/holzmann/pluto/src/PParticle.h"
#include "/misc/holzmann/pluto/src/PChannel.h"
#include <string.h>
#include <fstream.h>
#include <iostream.h>

//
// Reduce 2 singles 3d HADES acceptance matrices (i.e. for e+  and e-)
// to a pair acceptance matrix, averaging over phi and the 2 internal degrees of freedom
// (phi, theta_decay, phi_decay).  Take singles matrices from a HAFT file.
// 
// The resulting TH3F object is written to a Root file and in binary format to a
// FORTRAN-readable file as well. 
//
// Usage:
// -----
//
// root[0] .L libPluto.so
// root[1] .L libHAFT.so
// root[2] .L makePairMatrix.C+
// root[3] makePairMatrix("HAFTFilename","outFilename",
//                         mchan,mlow,mup,ptchan,ptlow,ptup,ychan,ylow,yup,"comment");
//   .
// (mass in GeV/c**2 and pt in GeV/c)
//   .
// Created :        21/06/05 by R. Holzmann
// Last modified :  13/02/05 by R. Holzmann
//

extern "C" void setfilename_(Text_t*, Int_t);
extern "C" Float_t gethadesacceptance_(Int_t*,Float_t*,Float_t*,Float_t*,Int_t*);

Int_t writeFortranStyle(Int_t bytecount, void* array, fstream* out, Int_t mode);
void doSavitzkyGolayFiltering(Float_t *array, Int_t xchan, Int_t ychan, Int_t zchan);

void makePairMatrix(Text_t *inName, Text_t *outName,
                    Int_t mchan, Float_t ml, Float_t mu,
                    Int_t ptchan, Float_t ptl, Float_t ptu,
                    Int_t ychan, Float_t yl, Float_t yu, Text_t *comment="Pair acceptance matrix") {

  setfilename_(inName,strlen(inName));

  TH3F *pairAcceptance = new TH3F("pairAcc",comment,mchan,ml,mu,ptchan,ptl,ptu,ychan,yl,yu);

  Int_t pidposi = 2;
  Int_t pidelec = 3;

  Text_t commentString[] =
    "                                                                                ";
  // 1        10        20        30        40        50        60        70        80
  strncpy(commentString, comment, strlen(comment));

  Bool_t filter = kFALSE;
  Int_t mode = -2;             // interpolation mode
  const Int_t phibins = 60;    // nb. of phi bins
  const Int_t samples = 250;   // nb. of decay samples
  const Float_t openCut = 9.0; // pair opening angle cut
//const Float_t momcut = 0.1;  // momentum cut at 0.1 GeV/c
  const Float_t momcut = 0.0;  // no momentum cut

  Float_t norm = phibins*samples;

  const Float_t r2d = 57.29577951;

  PParticle *positron = new PParticle("e+");
  PParticle *electron = new PParticle("e-");
  PParticle *dilepton = new PParticle("dilepton");
  PParticle *particles[] = {dilepton, positron, electron};
  PChannel *engine = new PChannel(particles,2,0);
  //  engine->setAngleFunction(new TF2("thetaInternal",
  //           "(1+(1-sqrt(y)/1.5)*x*x)/2", -1.,1., 0.,2.));  // set internal angular distribution

  for (Int_t ix=1;ix<=mchan;ix++) { // loop over pair matrix (in mass, pt and y)
    Float_t mass = pairAcceptance->GetXaxis()->GetBinCenter(ix);
    for (Int_t iy=1;iy<=ptchan;iy++) {
      Float_t pt = pairAcceptance->GetYaxis()->GetBinCenter(iy);
      Double_t mt = sqrt(pt*pt+mass*mass);
      for (Int_t iz=1;iz<=ychan;iz++) {
        Float_t y = pairAcceptance->GetZaxis()->GetBinCenter(iz); 
        Double_t E = mt*cosh(y);
        Double_t pz = mt*sinh(y);
        Float_t val = 0.;
        for (Int_t jx=0;jx<phibins;jx++) { // average over dilepton azimuthal angle
          Double_t phi = jx*6.28319/phibins;
          Double_t px = pt*cos(phi);
          Double_t py = pt*sin(phi);
	  dilepton->SetPxPyPzE(px,py,pz,E);
          for (Int_t jy=0;jy<samples;jy++) { // average over dilepton internal d.o.f.
            engine->decay();
            Float_t p1 = positron->P();
            Float_t theta1 = r2d*positron->Theta();
            Float_t phi1 = r2d*positron->Phi();
            if (phi1<0.) phi1 += 360.;       // pecularity of TLorentzVector!
            Float_t p2 = electron->P();
            Float_t theta2 = r2d*electron->Theta();
            Float_t phi2 = r2d*electron->Phi();
            if (phi2<0.) phi2 += 360.;
            Float_t open = r2d*positron->Angle(electron->Vect());
            if (open>openCut && p1>momcut && p2>momcut) {
              Float_t acc = gethadesacceptance_(&pidposi,&p1,&theta1,&phi1,&mode) *
                            gethadesacceptance_(&pidelec,&p2,&theta2,&phi2,&mode);
              val += acc;
            }
          }
        }
        val /= norm;
        printf("m=%f pt=%f y=%f | <acc>=%f\n",mass,pt,y,val);
        pairAcceptance->SetBinContent(ix,iy,iz,val);
      }
    }
  }

  Float_t arrayC[mchan][ptchan][ychan]; // allocate array with C++ memory layout

  for(Int_t ix=1;ix<=mchan;ix++) {      // copy to array in C-style
    for(Int_t iy=1;iy<=ptchan;iy++) {   // row-by-row mode
      for(Int_t iz=1;iz<=ychan;iz++) {  // (3rd index varies fastest!)
         arrayC[ix-1][iy-1][iz-1] = pairAcceptance->GetBinContent(ix,iy,iz);
      }
    }
  }

  TString fname(outName);
  fname += ".root";
  TFile out(fname.Data(),"new");  // save 3d matrix to Root file
  pairAcceptance->Write();
  out.Close();

  if (filter) doSavitzkyGolayFiltering(&arrayC[0][0][0],mchan,ptchan,ychan);

  Float_t arrayF[ychan][ptchan][mchan]; // allocate array with FORTRAN memory layout

  for(Int_t ix=1;ix<=mchan;ix++) {      // copy to array in FORTRAN-style
    for(Int_t iy=1;iy<=ptchan;iy++) {   // column-by-column mode
      for(Int_t iz=1;iz<=ychan;iz++) {  // (1st index varies fastest!)
         arrayF[iz-1][iy-1][ix-1] = arrayC[ix-1][iy-1][iz-1];
      }
    }
  }

  fstream out2;
  TString fname2(outName);
  fname2 += ".acc";
  out2.open(fname2.Data(),ios::out|ios::trunc);
  Int_t nbytes=0;
//
// FORTRAN unformatted sequential I/O needs a bytecount, i.e. each I/O
// operation works on a new record, prefixed and appended with the bytecount.
//
// The following output **MUST** be read via the following FORTRAN statements:
//
//       read(unit) comment                        ! comment string into character*80
//       read(unit) ix,iy,iz                       ! array dimensions into 3 x integer*4
//       read(unit) xmin,xmax,ymin,ymax,zmin,zmax  ! axis limits into 6 x real*4
//       read(unit) parId                          ! particle id into integer*4  (Geant3 convention)
//       read(unit) arrayN                         ! data into real*4 array declared with proper size
//
  Int_t dims[3];
  dims[0]=pairAcceptance->GetNbinsX();            // get matrix dimensions
  dims[1]=pairAcceptance->GetNbinsY();
  dims[2]=pairAcceptance->GetNbinsZ();
  Float_t lims[6];
  lims[0]=pairAcceptance->GetXaxis()->GetXmin();  // get matrix limits
  lims[1]=pairAcceptance->GetXaxis()->GetXmax();
  lims[2]=pairAcceptance->GetYaxis()->GetXmin();
  lims[3]=pairAcceptance->GetYaxis()->GetXmax();
  lims[4]=pairAcceptance->GetZaxis()->GetXmin();
  lims[5]=pairAcceptance->GetZaxis()->GetXmax();

  nbytes += writeFortranStyle(sizeof(commentString)-1,&commentString,&out2,0);
  nbytes += writeFortranStyle(sizeof(dims),&dims,&out2,0);
  nbytes += writeFortranStyle(sizeof(lims),&lims,&out2,0);
  nbytes += writeFortranStyle(sizeof(arrayF),&arrayF,&out2,0);

  out2.close();
  cout << endl << "Reduction done: " << nbytes
       << " bytes written to file " << fname2.Data() << endl << endl;

  return;

}

Float_t coefSavGol2(Int_t i, Int_t j, Int_t k, Int_t ni, Int_t nj, Int_t nk) {
//
// Compute 2nd order Savitzky-Golay polynomial coefficient c=c(i,j,k,ni,nj,nk)
//
  if (abs(i)>ni || abs(j)>nj || abs(k)>nk) return 0.;
  Float_t Nijk = (2*ni+1)*(2*nj+1)*(2*nk+1);
  Float_t gi = (ni*(ni+1))/3.;
  Float_t gj = (nj*(nj+1))/3.;
  Float_t gk = (nk*(nk+1))/3.;
  Float_t termi = 5.*((Float_t)(i*i)-gi)/(4.*gi-1.);
  Float_t termj = 5.*((Float_t)(j*j)-gj)/(4.*gj-1.);
  Float_t termk = 5.*((Float_t)(k*k)-gk)/(4.*gk-1.);
  return (1.-termi-termj-termk)/Nijk;
}

void doSavitzkyGolayFiltering(Float_t *data, Int_t xchan, Int_t ychan, Int_t zchan) {
//
// Do a 3dim symmetric second-order Savitzky-Golay noise filtering on data
//
//   - 3dim, because data=data(i,j,k)
//   - symmetric, because nR = nL = n
//   - second-order, because m=2
//
  Int_t n=2;                          // fix filter window to [-2,2]
  Float_t work[xchan][ychan][zchan];  // temporary work matrix
  Int_t yzchan = ychan*zchan;
  Float_t c[2*n+1][2*n+1][2*n+1];     // coefficient matrix

  Float_t sum = 0.;
  for(Int_t i=-n; i<=n; i++) {        // tabulate filter coefficients
    for(Int_t j=-n; j<=n; j++) {
      for(Int_t k=-n; k<=n; k++) {
        sum += c[i+n][j+n][k+n] = coefSavGol2(i,j,k,n,n,n);
      }
    }
  }
  cout << endl << "Sum of Savitzky-Golay coefficients : " << sum;

  Float_t val;
  Int_t ic, jc, kc;
  for(Int_t i=n; i<xchan-n; i++) {     // loop over data
    for(Int_t j=n; j<ychan-n; j++) {   // and filter
      for(Int_t k=n; k<zchan-n; k++) {
        val = 0;
        ic = -n;
        for(Int_t ip=i-n; ip<=i+n; ip++, ic++) {
          jc = -n;
          for(Int_t jp=j-n; jp<=j+n; jp++, jc++) {
            kc = -n;
            for(Int_t kp=k-n; kp<=k+n; kp++, kc++) {
              val += c[ic+n][jc+n][kc+n]*data[ip*yzchan+jp*zchan+kp];
            }
          }
        }
        work[i][j][k] = val;
      }
    }
  }
  for(Int_t i=n; i<xchan-n; i++) {    // clip and copy back filtered data
    for(Int_t j=n; j<ychan-n; j++) {
      for(Int_t k=n; k<zchan-n; k++) {
        data[i*yzchan+j*zchan+k] = TMath::Max(double(TMath::Min(double(work[i][j][k]),1.0)),0.);
      }
    }
  }

  cout << " ... as should be" << endl;
  return;
}

Int_t writeFortranStyle(Int_t bytecount, void* array, fstream* out, Int_t mode) {
//
// Write data in FORTRAN unformatted direct (mode=0) or sequential (mode=1)
// style to output file
//
  if (mode==1) out->write((char*)&bytecount,4);
  out->write((char*)array,bytecount);
  if (mode==1) out->write((char*)&bytecount,4);
  if (mode==1) return bytecount+8;
  else return bytecount;
}
