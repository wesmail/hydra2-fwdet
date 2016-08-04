#include "TMath.h"
#include "TFile.h"
#include "TH3F.h"
#include <cstring>
#include <fstream>
#include <iostream>
//
// Convert a 3d HADES acceptance matrix (i.e. TH3F object) into
// FORTRAN-readable format used by the HADES Theory Acceptance Filter (HAFT)
//
// Optionally the matrix can be smoothed with a Savitzky-Golay filter.
//
// Usage:
// -----
//
// root[0] .L writeHAFT.C+
// root[1] writeHAFT("inputFilename","matrixName1",particleId1,"comment");
// root[2] writeHAFT("inputFilename","matrixName2",particleId2);
// root[3] writeHAFT("inputFilename","matrixName3",particleId3);
//   .
//   .
//   .
// If file HadesAcceptanceFilter.acc exists already, the new matrix is appended
// with its dimensions and limits.
//
// The created matrix file can be scanned for its content with:
//
// root[0] scanHAFT("filename");
//
//
// Created :   06/04/05 by R. Holzmann
// Modified :  20/04/05 by R. Holzmann  added Savitzky-Golay filtering
// Modified :  28/04/05 by R. Holzmann  added support for resolution parameters
// Modified :  03/05/05 by R. Holzmann  added multiple scattering
// Modified :  27/01/06 by R. Holzmann  added support for diff. dims and lims
//

Int_t writeFortranStyle(Int_t bytecount, void* array, fstream* out, Int_t mode);
Int_t readFortranStyle(Int_t bytecount, void* array, fstream* in, Int_t mode);
void doSavitzkyGolayFiltering(Float_t *array, Int_t xchan, Int_t ychan, Int_t zchan);


Float_t unit=1000.;                                 // 1 GeV/c**2 = 1000 MeV/c**2 

void writeHAFT(Text_t *inName, Text_t *matName, Int_t parId, Text_t *comment="") {

  Bool_t filter = kTRUE;                           // no Savitzky-Golay filtering
  Text_t outName[] = "HadesAcceptanceFilter.acc";   // output file name

  Float_t sigPa[3] = {0.0, 1.0, 0.6};  // Schicker parametrization of momentum resolution [ NIM A380 (1996) 586 ]
  Float_t sigPb[3] = {15.0, 3.6, 1.1}; // sigma(p) = 0.01*p*(sigPa[mode] + sigPb[mode]*p)  p in GeV/c
  Float_t sigTheta = 0.0031;           // sigma(Theta) = sigma(Phi)*sin(Theta) = 0.18 degree = 0.0031 rad
  Float_t sigPhi = 0.0031;             // from Hades technical proposal (Figs. 4-20 and 4-21) and J. Markert thesis
  Float_t XX0 = 0.02;                  // X/X0 of central Hades region (target + beam pipe + Rich) 
  Float_t sigAngles[3] = {sigTheta, sigPhi, XX0};

  Text_t commentString[] =
    "                                                                                ";
  // 1        10        20        30        40        50        60        70        80

  TFile *input = new TFile(inName,"read");
  if (input->IsOpen()==kFALSE) {
    cout << "Input file " << inName << " could not be opened!" << endl;
    return;
  } 

  TH3F *matrix = (TH3F*)(input->Get(matName));
  if (matrix==NULL) {
    cout << "Acceptance matrix " << matName << " not found in file!" << endl;
    return;
  } 

  strncpy(commentString, comment, strlen(comment));

  Int_t dims[3];
  Int_t xchan,ychan,zchan;
  xchan=dims[0]=matrix->GetNbinsX();           // get matrix dimensions
  ychan=dims[1]=matrix->GetNbinsY();
  zchan=dims[2]=matrix->GetNbinsZ();

  Float_t lims[6];
  Float_t xmin,xmax,ymin,ymax,zmin,zmax;
  xmin=lims[0]=(matrix->GetXaxis()->GetXmin())/unit;  // get matrix limits
  xmax=lims[1]=(matrix->GetXaxis()->GetXmax())/unit;
  ymin=lims[2]=(matrix->GetYaxis()->GetXmin());
  ymax=lims[3]=(matrix->GetYaxis()->GetXmax());
  zmin=lims[4]=(matrix->GetZaxis()->GetXmin());
  zmax=lims[5]=(matrix->GetZaxis()->GetXmax());

  cout << endl << "Convert matrix with dimensions ("
       << xchan << "," << ychan << "," << zchan << ")" << endl;
  cout << "and ranges [" << xmin << "-" << xmax << "]["
       << ymin << "-" << ymax << "][" << zmin << "-" << zmax << "]" << endl;

  Float_t arrayC[xchan][ychan][zchan]; // allocate array with C++ memory layout

  for(Int_t ix=1;ix<=xchan;ix++) {     // copy to array in C-style
    for(Int_t iy=1;iy<=ychan;iy++) {   // row-by-row mode
      for(Int_t iz=1;iz<=zchan;iz++) { // (3rd index varies fastest!)
         arrayC[ix-1][iy-1][iz-1] = matrix->GetBinContent(ix,iy,iz);
      }
    }
  }

  if (filter) doSavitzkyGolayFiltering(&arrayC[0][0][0],xchan,ychan,zchan);

  Float_t arrayF[zchan][ychan][xchan]; // allocate array with FORTRAN memory layout

  for(Int_t ix=1;ix<=xchan;ix++) {     // copy to array in FORTRAN-style
    for(Int_t iy=1;iy<=ychan;iy++) {   // column-by-column mode
      for(Int_t iz=1;iz<=zchan;iz++) { // (1st index varies fastest!)
         arrayF[iz-1][iy-1][ix-1] = arrayC[ix-1][iy-1][iz-1];
      }
    }
  }

  input->Close();
  delete input;

  Bool_t newfile;
  fstream in;
  in.open(outName,ios::in);
  if (in.is_open()) newfile = kFALSE;  // check if file exists already
  else newfile = kTRUE;
  in.close();

  fstream out;
  if (newfile) out.open(outName,ios::out|ios::trunc);  // (re)open file for unformatted output
  else out.open(outName,ios::out|ios::app);
  Int_t nbytes=0;
//
// FORTRAN unformatted direct access I/O needs a bytecount.
//
// The following output **MUST** be read via the following FORTRAN statements:
//
//       read(unit) comment                        ! comment string into character*80
//       read(unit) sigPa(3)                       ! res params into 3 x real*4
//       read(unit) sigPb(3)                       ! res params into 3 x real*4
//       read(unit) sigTh, sigPh, XX0              ! res params into 3 x real*4
//    |- read(unit) parId                          ! particle id into integer*4  (Geant3 convention)
//    |- read(unit) ix,iy,iz                       ! array dimensions into 3 x integer*4
//    |- read(unit) xmin,xmax,ymin,ymax,zmin,zmax  ! axis limits into 6 x real*4
//    |- read(unit) arrayN                         ! data into real*4 array declared with proper size
//    |
//    '--> repeat last 4 reads until EOF
//
  if (newfile) {
    cout << endl << "|" << commentString << "|" << endl << endl;
    nbytes += writeFortranStyle(sizeof(commentString)-1,&commentString,&out,0);
    nbytes += writeFortranStyle(sizeof(sigPa),&sigPa,&out,0);
    nbytes += writeFortranStyle(sizeof(sigPb),&sigPb,&out,0);
    nbytes += writeFortranStyle(sizeof(sigAngles),&sigAngles,&out,0);
  }
  nbytes += writeFortranStyle(sizeof(parId),&parId,&out,0);
  nbytes += writeFortranStyle(sizeof(dims),&dims,&out,0);
  nbytes += writeFortranStyle(sizeof(lims),&lims,&out,0);
  nbytes += writeFortranStyle(sizeof(arrayF),&arrayF,&out,0);

  out.close();
  if (newfile) cout << endl << "Conversion done: " << nbytes
       << " bytes written to file " << outName << endl << endl;
  else cout << endl << "Conversion done: " << nbytes
       << " additional bytes written to file " << outName << endl << endl;
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

void scanHAFT(Text_t *inName) {
//
// Scan a matrix file and list its content
//
  fstream in;
  in.open(inName,ios::in);
  if (!in.is_open()) {
    cout << endl << "File does not exist" << endl;  // check if file exists already
    return;
  }

  cout << endl << "Scan file:" << endl << endl;

  Int_t nbytes = 0;

  Text_t comment[81];
  Float_t sigPa[3], sigPb[3], sigTheta, sigPhi, XX0;
  Int_t dims[3];
  Int_t xchan,ychan,zchan;
  Float_t lims[6];
  Float_t xmin,xmax,ymin,ymax,zmin,zmax;

  nbytes += readFortranStyle(sizeof(comment)-1,&comment,&in,0);
  comment[80] = '\0';
  nbytes += readFortranStyle(sizeof(sigPa),&sigPa,&in,0);
  nbytes += readFortranStyle(sizeof(sigPb),&sigPb,&in,0);
  nbytes += readFortranStyle(sizeof(Float_t),&sigTheta,&in,0);
  nbytes += readFortranStyle(sizeof(Float_t),&sigPhi,&in,0);
  nbytes += readFortranStyle(sizeof(Float_t),&XX0,&in,0);

  cout << "Comment : " << comment << endl << endl;
  cout << "Resolution : " << sigPa[0] << ", " << sigPa[1] << ", " << sigPa[2] << endl;
  cout << "             " << sigPb[0] << ", " << sigPb[1] << ", " << sigPb[2] << endl;
  cout << "sigTh,sigPh :" << sigTheta << ", " << sigPhi << endl;
  cout << "XX0 :        " << XX0 << endl << endl;

  Int_t parId;
  Float_t array[250000];  // allocate array

  while (1) {
    Int_t nread=0;
    if ( (nread=readFortranStyle(sizeof(parId),&parId,&in,0))==0) break;
    nbytes += nread;
    nbytes += readFortranStyle(sizeof(dims),&dims,&in,0);
    xchan = dims[0]; ychan = dims[1]; zchan = dims[2];
    nbytes += readFortranStyle(sizeof(lims),&lims,&in,0);
    xmin = lims[0]; xmax = lims[1];
    ymin = lims[2]; ymax = lims[3];
    zmin = lims[4]; zmax = lims[5];
    nbytes += readFortranStyle(sizeof(array),&array,&in,0);
    cout << "Matrix read for id " << parId << endl;
    cout << "Dimensions Acc(" << xchan << "," << ychan << "," << zchan << ") ";
    cout << "and ranges [" << xmin << "-" << xmax << "][" << ymin
         << "-" << ymax << "][" << zmin << "-" << zmax << "]" << endl << endl;
  }

  cout << endl << "Scan done: " << nbytes
       << " bytes read from file " << inName << endl << endl;

  in.close();
  return;
}

Int_t readFortranStyle(Int_t bytecount, void* array, fstream* in, Int_t mode) {
//
// Read data in FORTRAN unformatted direct (mode=0) or sequential (mode=1) 
// style from input file
//
  Int_t nread=0;
  if (mode==1) {
    in->read((char*)&bytecount,4);
    nread = in->gcount();
  }
  in->read((char*)array,bytecount);
  nread += in->gcount();
  if (mode==1) {
    in->read((char*)&bytecount,4);
    nread += in->gcount();
    if (nread==bytecount+8) return nread;
    else return 0;
  }
  if (nread==bytecount) return nread;
  else return 0;
}


void testChannels(Text_t *inName, Text_t *matName) {
//
// Print out (ix,iy,iz)-indexed channels for testing purposes
//
  TFile *input = new TFile(inName,"read");
  if (input->IsOpen()==kFALSE) {
    cout << "Input file " << inName << " could not be opened!" << endl;
    return;
  } 

  TH3F *matrix = (TH3F*)(input->Get(matName));
  if (matrix==NULL) {
    cout << "Acceptance matrix " << matName << " not found in file!" << endl;
    return;
  } 

  Int_t xchan=matrix->GetNbinsX();  // get matrix dimensions
  Int_t ychan=matrix->GetNbinsY();
  Int_t zchan=matrix->GetNbinsZ();

  cout << "Test matrix with dimensions ("
       << xchan << "," << ychan << "," << zchan << ")" << endl;

  Float_t val=0.;
  Int_t ix,iy,iz;
  while (1) {
    cout << "Enter ix,iy,iz : ";
    cin >> ix >> iy >> iz;
    if (ix<=0) return;
    val=matrix->GetBinContent(ix,iy,iz);
    cout << "has value : " << val << endl;
  }
}

void testBins(Text_t *inName, Text_t *matName) {
//
// Print out (p,theta,phi)-bins for testing purposes
//
  TFile *input = new TFile(inName,"read");
  if (input->IsOpen()==kFALSE) {
    cout << "Input file " << inName << " could not be opened!" << endl;
    return;
  } 

  TH3F *matrix = (TH3F*)(input->Get(matName));
  if (matrix==NULL) {
    cout << "Acceptance matrix " << matName << " not found in file!" << endl;
    return;
  } 

  Float_t xmin=(matrix->GetXaxis()->GetXmin())/unit;  // get matrix limits
  Float_t xmax=(matrix->GetXaxis()->GetXmax())/unit;
  Float_t ymin=(matrix->GetYaxis()->GetXmin());
  Float_t ymax=(matrix->GetYaxis()->GetXmax());
  Float_t zmin=(matrix->GetZaxis()->GetXmin());
  Float_t zmax=(matrix->GetZaxis()->GetXmax());

  cout << "Test matrix with ranges [" << xmin << "-" << xmax << "]["
       << ymin << "-" << ymax << "][" << zmin << "-" << zmax << "]" << endl;

  Float_t val=0.;
  Float_t p,th,ph;
  Int_t ix,iy,iz;
  while (1) {
    cout << "Enter p,theta,phi : ";
    cin >> p >> th >> ph;
    if (p<xmin) return;
    ix=matrix->GetXaxis()->FindBin(p);
    iy=matrix->GetYaxis()->FindBin(th);
    iz=matrix->GetZaxis()->FindBin(ph);
    val=matrix->GetBinContent(ix,iy,iz);
    cout << "has value : " << val << endl;
    Int_t binnr = matrix->GetBin(ix,iy,iz);
    cout << ix << " " << iy << " " << iz << " : " << binnr << endl;
    val=matrix->GetBinContent(binnr);
    cout << "has value : " << val << " " << endl;
  }
}
