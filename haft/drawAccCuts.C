{
gSystem->Load("libHAFT.so");
Char_t fname[]="HadesAcceptanceFilter_NOV02_v1.acc";
setfilename_(fname,sizeof(fname));

Int_t pid = 2;        // select a particle ID
Float_t p0 = 1.010;   // select cut planes sitting on the grid points
Float_t th0 = 45.0;   // (e.g. 50 p bins, 45 theta bins, 15 phi bins centered
Float_t ph0 = 31.0;   //  at 25,75,125,... & 1,3,5,7,9,... & 2,6,10,14,18,...)

Int_t m1=0, m2=-4, m3=3, m4=4;

Int_t xdim, ydim, zdim;
getdimensions_(&pid, &xdim, &ydim, &zdim);
Float_t plo, phi, dp, thlo, thhi, dth, phlo, phhi, dph;
getlimits_(&pid, &plo, &phi, &dp, &thlo, &thhi, &dth, &phlo, &phhi, &dph);

Float_t pgrid[xdim], thgrid[ydim], phgrid[zdim] ;

for (Int_t i=0; i<xdim;i++) pgrid[i] = dp/2.+dp*float(i);      // init grid
for (Int_t j=0; j<ydim;j++) thgrid[j] = dth/2.+dth*float(j);
for (Int_t k=0; k<zdim;k++) phgrid[k] = dph/2.+dph*float(k);

Float_t accgp[xdim], accgth[ydim], accgph[zdim];
for (Int_t i=0; i<xdim;i++)
   accgp[i] = gethadesacceptance_(&pid,&pgrid[i],&th0,&ph0,&m1);
for (Int_t j=0; j<ydim;j++)
   accgth[j] = gethadesacceptance_(&pid,&p0,&thgrid[j],&ph0,&m1);
for (Int_t k=0; k<zdim;k++)
   accgph[k] = gethadesacceptance_(&pid,&p0,&th0,&phgrid[k],&m1);

Int_t np = 2001;
Int_t nth = 901;
Int_t nph = 601;
Float_t p[np], th[nth], ph[nph];

for (Int_t i=0; i<np;i++) p[i] = 0.001*float(i);      // init x,y,z vectors
for (Int_t j=0; j<nth;j++) th[j] = 0.1*float(j);
for (Int_t k=0; k<nph;k++) ph[k] = 0.1*float(k);

Float_t acc1p[np], acc2p[np], acc3p[np], acc4p[np];
for (Int_t i=0; i<np;i++)
   acc1p[i] = gethadesacceptance_(&pid,&p[i],&th0,&ph0,&m1);
for (Int_t i=0; i<np;i++)
   acc2p[i] = gethadesacceptance_(&pid,&p[i],&th0,&ph0,&m2);
for (Int_t i=0; i<np;i++)
   acc3p[i] = gethadesacceptance_(&pid,&p[i],&th0,&ph0,&m3);
for (Int_t i=0; i<np;i++)
   acc4p[i] = gethadesacceptance_(&pid,&p[i],&th0,&ph0,&m4);

Float_t acc1th[nth], acc2th[nth], acc3th[nth], acc4th[nth];
for (Int_t j=0; j<nth;j++)
   acc1th[j] = gethadesacceptance_(&pid,&p0,&th[j],&ph0,&m1);
for (Int_t j=0; j<nth;j++)
   acc2th[j] = gethadesacceptance_(&pid,&p0,&th[j],&ph0,&m2);
for (Int_t j=0; j<nth;j++)
   acc3th[j] = gethadesacceptance_(&pid,&p0,&th[j],&ph0,&m3);
for (Int_t j=0; j<nth;j++)
   acc4th[j] = gethadesacceptance_(&pid,&p0,&th[j],&ph0,&m4);

Float_t acc1ph[nph], acc2ph[nph], acc3ph[nph], acc4ph[nph];
for (Int_t k=0; k<nph;k++)
   acc1ph[k] = gethadesacceptance_(&pid,&p0,&th0,&ph[k],&m1);
for (Int_t k=0; k<nph;k++)
   acc2ph[k] = gethadesacceptance_(&pid,&p0,&th0,&ph[k],&m2);
for (Int_t k=0; k<nph;k++)
   acc3ph[k] = gethadesacceptance_(&pid,&p0,&th0,&ph[k],&m3);
for (Int_t k=0; k<nph;k++)
   acc4ph[k] = gethadesacceptance_(&pid,&p0,&th0,&ph[k],&m4);

TCanvas c1("HAFT","HADES Acceptance Filter for Theorists",100,100,700,700);
c1.Divide(1,3);

TGraph p1(np,p,acc1p);           // momentum cut
p1->SetTitle("Momentum");
p1.SetLineColor(1);
p1.SetLineWidth(2);
p1->GetXaxis()->SetLabelSize(0.06);
p1->GetXaxis()->SetRangeUser(plo,phi);
p1->GetYaxis()->SetLabelSize(0.06);
TGraph p2(np,p,acc2p);
p2.SetLineColor(4);
p2.SetLineWidth(2);
TGraph p3(np,p,acc3p);
p3.SetLineColor(3);
p3.SetLineWidth(2);
TGraph p4(np,p,acc4p);
p4.SetLineColor(2);
p4.SetLineWidth(2);
c1.cd(1);
gPad->SetGrid();
p1->Draw("al");
p2->Draw("l");
p3->Draw("l");
p4->Draw("l");
TGraph pg(xdim,pgrid,accgp);
pg->Draw("*");

TGraph th1(nth,th,acc1th);     // theta cut
th1->SetTitle("Theta");
th1.SetLineColor(1);
th1.SetLineWidth(2);
th1->GetXaxis()->SetLabelSize(0.06);
th1->GetXaxis()->SetRangeUser(thlo,thhi);
th1->GetYaxis()->SetLabelSize(0.06);
TGraph th2(nth,th,acc2th);
th2.SetLineColor(4);
th2.SetLineWidth(2);
TGraph th3(nth,th,acc3th);
th3.SetLineColor(3);
th3.SetLineWidth(2);
TGraph th4(nth,th,acc4th);
th4.SetLineColor(2);
th4.SetLineWidth(2);
c1.cd(2);
gPad->SetGrid();
th1->Draw("al");
th2->Draw("l");
th3->Draw("l");
th4->Draw("l");
TGraph thg(ydim,thgrid,accgth);
thg->Draw("*");

TGraph ph1(nph,ph,acc1ph);     // phi cut
ph1->SetTitle("Phi");
ph1.SetLineColor(1);
ph1.SetLineWidth(2);
ph1->GetXaxis()->SetLabelSize(0.06);
ph1->GetXaxis()->SetRangeUser(phlo,phhi);
ph1->GetYaxis()->SetLabelSize(0.06);
TGraph ph2(nph,ph,acc2ph);
ph2.SetLineColor(4);
ph2.SetLineWidth(2);
TGraph ph3(nph,ph,acc3ph);
ph3.SetLineColor(3);
ph3.SetLineWidth(2);
TGraph ph4(nph,ph,acc4ph);
ph4.SetLineColor(2);
ph4.SetLineWidth(2);
c1.cd(3);
gPad->SetGrid();
ph1->Draw("al");
ph2->Draw("l");
ph3->Draw("l");
ph4->Draw("l");
TGraph phg(zdim,phgrid,accgph);
phg->Draw("*");
}
