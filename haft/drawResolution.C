{
// exercise HAFT resolution functions
//
gSystem->Load("libHAFT.so");
Char_t fname[]="HadesAcceptanceFilter_NOV02_v1.acc";
setfilename_(fname,sizeof(fname));

Int_t pid = 3;        // select a particle ID
Float_t p0 = 1.010;   // select cut planes sitting on the grid points
Float_t th0 = 45.0;   // (e.g. 50 p bins, 45 theta bins, 15 phi bins centered
Float_t ph0 = 31.0;   //  at 25,75,125,... & 1,3,5,7,9,... & 2,6,10,14,18,...)

Int_t m1=0, m2=-4, m3=3, m4=4;
Int_t mode;

Int_t xdim, ydim, zdim;
getdimensions_(&pid, &xdim, &ydim, &zdim);
Float_t plo, phi, dp, thlo, thhi, dth, phlo, phhi, dph;
getlimits_(&pid, &plo, &phi, &dp, &thlo, &thhi, &dth, &phlo, &phhi, &dph);

Float_t pgrid[xdim], thgrid[ydim], phgrid[zdim] ;

for (Int_t i=0; i<xdim;i++) pgrid[i] = dp/2.+dp*float(i);     // init grid
for (Int_t j=0; j<ydim;j++) thgrid[j] = dth/2.+dth*float(j);
for (Int_t k=0; k<zdim;k++) phgrid[k] = dph/2.+dph*float(k);

Float_t resplo[xdim], respme[xdim], resphi[xdim];
Float_t resth[ydim], resph[ydim]; // we want to test phi resol. vs. theta

Float_t mass = 0.000511;  // electron mass
Float_t mom[4] = {0.,0.,0.,mass};
Double_t ptot, ptot2, pt2, pt, sum, sum2;

Int_t n=10000;

mode = 1;
for (Int_t i=0; i<xdim;i++) {
  sum = sum2 = resplo[i] = 0.;
  for (Int_t ii=0;ii<n;ii++) {
    mom[0] = mom[1] = 0.;
    mom[2] = pgrid[i];
    mom[3] = sqrt(mom[2]*mom[2]+mass*mass);
    smearhades4momentum_(mom,&mode,&pid);
    ptot2 = mom[0]*mom[0] + mom[1]*mom[1] + mom[2]*mom[2];
    sum2 += ptot2/float(n);
    sum += sqrt(ptot2)/float(n);
  }
  resplo[i] = sqrt(sum2-sum*sum)/pgrid[i];
}
mode = 2;
for (Int_t i=0; i<xdim;i++) {
  sum = sum2 = respme[i] = 0.;
  for (Int_t ii=0;ii<n;ii++) {
    mom[0] = mom[1] = 0.;
    mom[2] = pgrid[i];
    mom[3] = sqrt(mom[2]*mom[2]+mass*mass);
    smearhades4momentum_(mom,&mode,&pid);
    ptot2 = mom[0]*mom[0] + mom[1]*mom[1] + mom[2]*mom[2];
    sum2 += ptot2/float(n);
    sum += sqrt(ptot2)/float(n);
  }
  respme[i] = sqrt(sum2-sum*sum)/pgrid[i];
}
mode = 3;
for (Int_t i=0; i<xdim;i++) {
  sum = sum2 = resphi[i] = 0.;
  for (Int_t ii=0;ii<n;ii++) {
    mom[0] = mom[1] = 0.;
    mom[2] = pgrid[i];
    mom[3] = sqrt(mom[2]*mom[2]+mass*mass);
    smearhades4momentum_(mom,&mode,&pid);
    ptot2 = mom[0]*mom[0] + mom[1]*mom[1] + mom[2]*mom[2];
    sum2 += ptot2/float(n);
    sum += sqrt(ptot2)/float(n);
  }
  resphi[i] = sqrt(sum2-sum*sum)/pgrid[i];
}

TCanvas c1("HAFT","HADES Acceptance Filter for Theorists",100,100,700,700);
c1.Divide(1,3);

TGraph p1(xdim,pgrid,resplo);         // momentum resolution (low mode)
p1->SetTitle("Momentum");
p1.SetLineColor(1);
p1.SetLineWidth(2);
p1->GetXaxis()->SetLabelSize(0.06);
p1->GetXaxis()->SetRangeUser(plo,phi);
p1->GetYaxis()->SetLabelSize(0.06);
TGraph p2(xdim,pgrid,respme);         // momentum resolution (medium mode)
p2.SetLineColor(4);
p2.SetLineWidth(2);
TGraph p3(xdim,pgrid,resphi);         // momentum resolution (high mode)
p3.SetLineColor(3);
p3.SetLineWidth(2);
c1.cd(1);
gPad->SetGrid();
p1->Draw("al");
p2->Draw("l");
p3->Draw("l");

Double_t theta, phi1, phi0;
const Double_t d2r = 0.017453293;

n=10000;
mode = 3;
for (Int_t j=0; j<ydim;j++) {
  sum2 = sum = resth[j] = 0.;
  ptot=1.;
  phi0=60.;
  for (Int_t jj=0;jj<n;jj++) {
    mom[0] = ptot*TMath::Sin(d2r*double(thgrid[j]))*TMath::Cos(d2r*phi0);
    mom[1] = ptot*TMath::Sin(d2r*double(thgrid[j]))*TMath::Sin(d2r*phi0);
    mom[2] = ptot*TMath::Cos(d2r*double(thgrid[j]));
    mom[3] = sqrt(ptot*ptot+mass*mass);
    smearhades4momentum_(mom,&mode,&pid);
    ptot2 = mom[0]*mom[0] + mom[1]*mom[1] + mom[2]*mom[2];
    theta = TMath::ACos(mom[2]/sqrt(ptot2))/d2r;
    sum2 += theta*theta/double(n);
    sum += theta/double(n);
  }
  resth[j] = sqrt(sum2-sum*sum);
  //  cout << thgrid[j] << " " << sum2 << " " << sum*sum << endl;
}
resth[0]=0.;

TGraph th1(ydim,thgrid,resth);         // plot theta resolution
th1->SetTitle("Theta");
th1.SetLineColor(1);
th1.SetLineWidth(2);
th1->GetXaxis()->SetLabelSize(0.06);
th1->GetXaxis()->SetRangeUser(10.,90.);
th1->GetYaxis()->SetLabelSize(0.06);

c1.cd(2);
gPad->SetGrid();
th1->Draw("al");

n=10000;
mode = 3;
for (Int_t k=0; k<ydim;k++) {
  sum2 = sum = resph[k] = 0.;
  ptot=1.;
  phi0=60.;
  for (Int_t kk=0;kk<n;kk++) {
    mom[0] = ptot*TMath::Sin(d2r*thgrid[k])*TMath::Cos(d2r*phi0);
    mom[1] = ptot*TMath::Sin(d2r*thgrid[k])*TMath::Sin(d2r*phi0);
    mom[2] = ptot*TMath::Cos(d2r*thgrid[k]);
    mom[3] = sqrt(ptot*ptot+mass*mass);
    smearhades4momentum_(mom,&mode,&pid);
    pt = sqrt(mom[0]*mom[0] + mom[1]*mom[1]);
    phi1 = TMath::ACos(mom[0]/pt)/d2r;
    sum2 += phi1*phi1/double(n);
    sum += phi1/double(n);
  }
  resph[k] = sqrt(sum2-sum*sum);
}
resph[0]=0.;


TGraph ph1(ydim,thgrid,resph);         // plot phi resolution
ph1->SetTitle("Phi");
ph1.SetLineColor(1);
ph1.SetLineWidth(2);
ph1->GetXaxis()->SetLabelSize(0.06);
ph1->GetXaxis()->SetRangeUser(10.,90.);
ph1->GetYaxis()->SetRangeUser(0.,2.);
ph1->GetYaxis()->SetLabelSize(0.06);

c1.cd(3);
gPad->SetGrid();
ph1->Draw("al");

}
