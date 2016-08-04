#include <TMath.h>
#include <TFile.h>
#include <TH3F.h>

//
// Smooth a 3d HADES acceptance or efficiency matrix (i.e. TH3F object)
// with a Savitzky-Golay filter.
//
// Clipping is done by default, meaning that bins will stay within [0,1].
//
// Usage:
// -----
//
// root[0] .L smoothMatrix.C+
// root[1] smoothMatrix("inputFilename","matrixName1");
// root[2] smoothMatrix("inputFilename","matrixName2");
//   .
//   .
//   .
// Created :   31/05/05 by R. Holzmann
// Modified :  01/06/05 by R. Holzmann
//

void do2dSavitzkyGolayFiltering(Float_t *, Int_t, Int_t, Bool_t);
void do3dSavitzkyGolayFiltering(Float_t *, Int_t, Int_t, Int_t, Bool_t);

void smoothMatrix(Text_t *inName, Text_t *matName, Bool_t clip=kTRUE) {

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

  Int_t xchan,ychan,zchan;
  xchan = matrix->GetNbinsX();           // get matrix dimensions
  ychan = matrix->GetNbinsY();
  zchan = matrix->GetNbinsZ();

  Float_t xmin,xmax,ymin,ymax,zmin,zmax;
  xmin = (matrix->GetXaxis()->GetXmin());  // get matrix limits
  xmax = (matrix->GetXaxis()->GetXmax());
  ymin = (matrix->GetYaxis()->GetXmin());
  ymax = (matrix->GetYaxis()->GetXmax());
  if (zchan>1) {
    zmin = (matrix->GetZaxis()->GetXmin());
    zmax = (matrix->GetZaxis()->GetXmax());
  } else zmin= zmax =0.;

  cout << endl << "Smooth matrix with dimensions ("
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

  if (zchan>1) do3dSavitzkyGolayFiltering(&arrayC[0][0][0],xchan,ychan,zchan,clip);
  else do2dSavitzkyGolayFiltering(&arrayC[0][0][0],xchan,ychan,clip);

  for(Int_t ix=1;ix<=xchan;ix++) {     // copy back to TH3F object
    for(Int_t iy=1;iy<=ychan;iy++) {   // (under- and overflows are not changed) 
      for(Int_t iz=1;iz<=zchan;iz++) {
         matrix->SetBinContent(ix,iy,iz,arrayC[ix-1][iy-1][iz-1]);
      }
    }
  }

  input->Close();
  delete input;

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


void do3dSavitzkyGolayFiltering(Float_t *data, Int_t xchan, Int_t ychan, Int_t zchan, Bool_t clip) {
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
        if (clip) data[i*yzchan+j*zchan+k] = TMath::Max(double(TMath::Min(double(work[i][j][k]),1.0)),0.);
        else data[i*yzchan+j*zchan+k] = work[i][j][k];
      }
    }
  }

  cout << " ... as should be" << endl;
  return;
}


void do2dSavitzkyGolayFiltering(Float_t *data, Int_t xchan, Int_t ychan, Bool_t clip) {
//
// Do a 2dim symmetric second-order Savitzky-Golay noise filtering on data
//
//   - 2dim, because data=data(i,j)
//   - symmetric, because nR = nL = n
//   - second-order, because m=2
//
  Int_t n=2;                          // fix filter window to [-2,2]
  Float_t work[xchan][ychan];         // temporary work matrix
  Float_t c[2*n+1][2*n+1];            // coefficient matrix

  Float_t sum = 0.;
  for(Int_t i=-n; i<=n; i++) {        // tabulate filter coefficients
    for(Int_t j=-n; j<=n; j++) {
      sum += c[i+n][j+n] = coefSavGol2(i,j,0,n,n,0);
    }
  }
  cout << endl << "Sum of Savitzky-Golay coefficients : " << sum;

  Float_t val;
  Int_t ic, jc;
  for(Int_t i=n; i<xchan-n; i++) {     // loop over data
    for(Int_t j=n; j<ychan-n; j++) {   // and filter
      val = 0;
      ic = -n;
      for(Int_t ip=i-n; ip<=i+n; ip++, ic++) {
        jc = -n;
        for(Int_t jp=j-n; jp<=j+n; jp++, jc++) {
	  val += c[ic+n][jc+n]*data[ip*ychan+jp];
        }
      }
      work[i][j] = val;
    }
  }

  for(Int_t i=n; i<xchan-n; i++) {    // clip and copy back filtered data
    for(Int_t j=n; j<ychan-n; j++) {
      if (clip) data[i*ychan+j] = TMath::Max(double(TMath::Min(double(work[i][j]),1.0)),0.);
      else data[i*ychan+j] = work[i][j];
    }
  }

  cout << " ... as should be" << endl;
  return;
}
