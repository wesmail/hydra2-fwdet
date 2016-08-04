//*-- AUTHOR : A.Rustamov
//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////
//HMdcTrackGSpline calculates momentum of particle using spline interpolation
//Input data are 4 points  from MDC chambers, OR the additional point from the
//intersection of inner segment with kickPlane
using namespace std;
#include <iostream>
#include <fstream>
#include "hmdctrackgspline.h"
#include "hgeomtransform.h"
#include "hgeomrotation.h"
#include "TMatrixD.h"
ClassImp(HMdcTrackGSpline)
   HMdcTrackGSpline::HMdcTrackGSpline(const Char_t* name, const Char_t * title):TNamed(name,title)
{
   initPlanes(); 
   initParams(50,4);
   N=4;
   //N1=50;
   //N2=50;
   pi=acos(-1.);
isInitialized=kFALSE;
isKickIsInitialized=kFALSE;  
}

void HMdcTrackGSpline::initPlanes()
{
   A1=-0.00; B1=0.9723054821; D1=78.50169451382;
   A2=-0.00; B2=1.3761110194; D2=125.35103798876;
   A3=-0.00; B3=1.1547000400; D3=207.24486448803;
   A4=-0.00; B4=1.1547000400; D4=256.11707204248;
}
void HMdcTrackGSpline::initMiddleParamsAll()
{
   for(Int_t s=0; s<6; s++)
      for(Int_t m=0; m<4; m++)
      {
	 middleGA[s][m]=geomParams[s][m].getX();
	 middleGB[s][m]=geomParams[s][m].getY();
	 middleGD[s][m]=geomParams[s][m].getZ();
      }
 isInitialized=kTRUE;
}

void HMdcTrackGSpline::initMiddleParams(HGeomVector params)
{
   middleA=params.getX();
   middleB=params.getY();
   middleD=params.getZ();
}

void HMdcTrackGSpline::initMiddleParamsP4(HGeomVector paramsP4)
{
   middleA4=paramsP4.getX();
   middleB4=paramsP4.getY();
   middleD4=paramsP4.getZ();  
}

void HMdcTrackGSpline::initParams(Int_t N,Int_t NN)
{
   N1=N;
   N2=N-2;
   x=new Double_t [12];
   equationXY=new HGeomVector [N];
   Bx=new Double_t [N];
   By=new Double_t [N];
   Bz=new Double_t [N];
   XZ=new Double_t [NN];
   YZ=new Double_t [NN];
   XZP=new Double_t [NN-1];
   YZP=new Double_t [NN-1];
   FXZ=new Double_t [N];
   FYZ=new Double_t [N];
   FXZP=new Double_t [N-1];
   FYZP=new Double_t [N-1];
   dydz=new Double_t [N];
   dxdz=new Double_t [N];
   d2xdz=new Double_t [N];
   d2ydz=new Double_t [N];
   curv=new Double_t [N];
   x2=new Double_t [N];
   zz2=new Double_t [N];
   z2=new Double_t [N];
   y2=new Double_t [N];
   dist=new Double_t [4];
   point=new HGeomVector [4];
   fieldpoints=new HGeomVector [N];
   secDer=new HGeomVector [N];
   Tfieldpoints=new HGeomVector [N];
   field=new HGeomVector [N];
   BB=new HGeomVector [N];

}
void HMdcTrackGSpline::clearParams()
{
  if(x) {remove(x);}
  if(equationXY) {remove(equationXY);}
  if(Bx) {remove(Bx);}
  if(By) {remove(By);}
  if(Bz) {remove(Bz);}
  if(XZ) {remove(XZ);}
  if(YZ) {remove(YZ);}
  if(XZP) {remove(XZP);}
  if(YZP) {remove(YZP); }
  if(FXZ) {remove(FXZ); }
  if(FYZ) {remove(FYZ); }
  if(FXZP) {remove(FXZP);}
  if(FYZP) {remove(FYZP);}
  if(dydz) {remove(dydz);}
  if(dxdz) {remove(dxdz);}
  if(d2ydz) {remove(d2ydz);}
  if(d2xdz) {remove(d2xdz);}
  if(curv)  {remove(curv);} 
  if(x2) {remove (x2);}
  if(zz2) {remove (zz2);}
  if(z2) {remove(z2);}
  if(y2) {remove(y2);}
  if(dist) {remove(dist);}
  if(point) {remove(point);}
  if(fieldpoints) {remove(fieldpoints);}
  if(secDer) {remove(secDer);}
  if(Tfieldpoints) {remove(Tfieldpoints);}
  if(field) {remove(field);}
  if(BB) {remove(BB);} 
}

void HMdcTrackGSpline::remove(Double_t *pF)
{
  delete [] pF; pF=0;
}
void HMdcTrackGSpline::remove(HGeomVector *pGV)
{
  delete [] pGV; pGV=0;
}

HMdcTrackGSpline::~HMdcTrackGSpline() 
{
  clearParams();
  //if(fGetCont) fGetCont->deleteCont();
}


Double_t HMdcTrackGSpline::det3(Double_t a[3][3])
{
  //calculates determinant of size 3
  return (a[0][0]*a[1][1]*a[2][2]+a[1][0]*a[2][1]*a[0][2]+
	  a[0][1]*a[1][2]*a[2][0]-a[0][2]*a[1][1]*a[2][0]-
	  a[2][1]*a[1][2]*a[0][0]-a[1][0]*a[0][1]*a[2][2]);
}

void HMdcTrackGSpline::init1()
{
  for(Int_t i=0;i<N2;i++)
    {
      transSplineB(60.,0.,BB[i],BB[i]);        
    }
}

void HMdcTrackGSpline::transteta()
{
  for (Int_t i=0;i<48;i++)
    {
      secDer[i].setY((secDer[i].getY()-tan(teta))/(secDer[i].getY()*tan(teta)+1));
      transSpline(0.,tetadig,fieldpoints[i],fieldpoints[i]);
    }
  
}
void HMdcTrackGSpline::transPhi()
{
  for(Int_t i=0; i<48; i++)
    {
      if(phidig<=90)
	{
	  transSpline(phidig,0.,secDer[i],secDer[i]);
	  transSpline(phidig,0.,fieldpoints[i],fieldpoints[i]);
	  transSpline((phidig-60.),0.,BB[i],BB[i]);
	}
      else
	{
	  transSpline(phidig-90.,0.,secDer[i],secDer[i]);
	  transSpline(phidig-90.,0.,fieldpoints[i],fieldpoints[i]);
	  transSpline((phidig-90.-60.),0.,BB[i],BB[i]);
	  
	  mixPoints(fieldpoints[i],fieldpoints[i]);
	  mixPoints(BB[i],BB[i]);
	  mixPoints(secDer[i],secDer[i]);
	  
	}
	    
    }
}
void HMdcTrackGSpline::getPoint()
{
   point[0].setXYZ(x[0],x[1],x[2]);
   point[1].setXYZ(x[3],x[4],x[5]);
   point[2].setXYZ(x[6],x[7],x[8]);
   point[3].setXYZ(x[9],x[10],x[11]);
   Double_t rr=sqrt(x[0]*x[0]+x[1]*x[1]+(x[2]-target)*(x[2]-target));
   phi=atan2(x[1],x[0]);
   
   phi=atan2((x[4]-x[1]),(x[3]-x[0]));
   
   
   
   teta=acos((x[2]-target)/rr);
   Double_t r2=sqrt((x[9]-x[6])*(x[9]-x[6])+
		    (x[10]-x[7])*(x[10]-x[7])+
		    (x[11]-x[8])*(x[11]-x[8]));    
   teta2dig=acos((x[11]-x[8])/r2)*180/pi;
   phi2dig=atan((x[10]-x[7])/(x[9]-x[6]))*180/pi;
   tetadig=teta*180/pi;
   
   if (tetadig<18) tetadig=18;
   if(tetadig>84) tetadig=84;
   phidig=phi*180/pi;
   phidigg=phidig;
   phi-=pi/2;
   SegmentPoints=point[2];
}
void HMdcTrackGSpline::transFieldpoints()
{
  for (Int_t i=0;i<N2;i++)
    {
      //if(fieldpoints[i].getX()<0.)
	    // {
      //  HGeomVector newfieldpoints;
      // newfieldpoints.setXYZ(fieldpoints[i].getX()*(-1.),fieldpoints[i].getY(),fieldpoints[i].getZ());
	       // transSpline(60.,0.,newfieldpoints,Tfieldpoints[i]);
	       // }
	       // else
      transSpline(60.,0.,fieldpoints[i],Tfieldpoints[i]); 
    }
}
void HMdcTrackGSpline::spline(HGeomVector *p, Double_t *Y,Double_t *YP,
			      Double_t *X,Double_t *XP,Int_t size)
{
  Double_t P[size],R[size],Q[size],h[size];
  Double_t t1,t2;
  for (Int_t i = 0; i<size-1; i++)
    h[i] = fabs(p[i+1].getZ() - p[i].getZ());
  for(Int_t i=0;i<size-2;i++)
      R[i]=3*((p[i+2].getY()-p[i+1].getY())/(h[i+1])-(p[i+1].getY()-
						      p[i].getY())/(h[i]))/(h[i]+h[i+1]);
  P[0]=1;
   Q[0]=0;
   for (Int_t i=0;i<size-2;i++)
     {
       t1=h[i]/(2*(h[i]+h[i+1]));
       t2=h[i+1]/(2*(h[i]+h[i+1]));
       P[i+1]= -t2/(1+t1*P[i]);
       Q[i+1]=(R[i]-t1*Q[i])/(1+t1*P[i]);  
     }
   Y[size-1]=Q[size-2]/(1-P[size-2]);
  Y[size-2]=Y[size-1];
  for (Int_t i=size-3;i>0;i--)
    Y[i]=P[i]*Y[i+1]+Q[i];
  Y[0]=Y[1];
  for (Int_t i=0;i<size-1;i++)
    YP[i]=(p[i+1].getY()-p[i].getY())/(h[i])-h[i]*
      Y[i]/3-h[i]*Y[i+1]/6;
   ///////////////////////////
  for(Int_t i=0;i<size-2;i++)
    R[i]=3*((p[i+2].getX()-p[i+1].getX())/(h[i+1])-
	    (p[i+1].getX()-
	     p[i].getX())/(h[i]))/(h[i]+h[i+1]);
  P[0]=1;
  Q[0]=0;
  for (Int_t i=0;i<size-2;i++)
    {
      t1=h[i]/(2*(h[i]+h[i+1]));
      t2=h[i+1]/(2*(h[i]+h[i+1]));
      P[i+1]= -t2/(1+t1*P[i]);
      Q[i+1]=(R[i]-t1*Q[i])/(1+t1*P[i]);  
    }
  X[size-1]=Q[size-2]/(1-P[size-2]);
  X[size-2]=X[size-1];
  for (Int_t i=size-3;i>0;i--)
    X[i]=P[i]*X[i+1]+Q[i];
  X[0]=X[1];
  for (Int_t i=0;i<size-1;i++)
    XP[i]=(p[i+1].getX()-p[i].getX())/(h[i])-h[i]*
      X[i]/3-h[i]*X[i+1]/6;      
}      
void HMdcTrackGSpline::getpoints()
{
  distfield=0;
  Double_t h=(point[2].getZ()-point[1].getZ())/(N1);
  Double_t hConst=(point[2].getZ()-point[1].getZ())*6;
  dist[0]=sqrt(point[0].getX()*point[0].getX()+
	       point[0].getY()*point[0].getY()+
	       point[0].getZ()*point[0].getZ());
  dist[1]=dist[0]+sqrt((point[1].getX()-point[0].getX())*
		       (point[1].getX()-point[0].getX())+
		       (point[1].getY()-point[0].getY())*
		       (point[1].getY()-point[0].getY())+
		       (point[1].getZ()-point[0].getZ())*
		       (point[1].getZ()-point[0].getZ()));
  for(Int_t i=0;i<N1;i++)
    {
      //points along Z axis
      z2[i]=point[1].getZ()+i*h;
      //points in YZ projection
      y2[i]=point[1].getY()+i*h*YZP[1]+i*i*h*h*YZ[1]/(2)+i*i*i*h*h*h*(YZ[2]-YZ[1])/hConst;
      //points Int_t XZ projection
      x2[i]=point[1].getX()+i*h*XZP[1]+i*i*h*h*XZ[1]/(2)+i*i*i*h*h*h*(XZ[2]-XZ[1])/hConst;
      
      dydz[i]=YZP[1]+i*h*YZ[1]+3*i*i*h*h*(YZ[2]-YZ[1])/hConst;
      dxdz[i]=XZP[1]+i*h*XZ[1]+3*i*i*h*h*(XZ[2]-XZ[1])/hConst;

      d2ydz[i]=YZ[1]+6*i*h*(YZ[2]-YZ[1])/hConst;
      d2xdz[i]=XZ[1]+6*i*h*(XZ[2]-XZ[1])/hConst;
      
      fieldpoints[i].setXYZ(x2[i],y2[i],z2[i]);
      secDer[i].setXYZ(dxdz[i],dydz[i],0.);
      

    }

   checkSecDer(d2ydz,dydz);
  
  for(Int_t i=1;i<N1-1;i++)
    {
      distfield+=sqrt((x2[i]-x2[i-1])*(x2[i]-x2[i-1])+
		      (y2[i]-y2[i-1])*(y2[i]-y2[i-1])+
		      (z2[i]-z2[i-1])*(z2[i]-z2[i-1]));
    }
  distfield+=sqrt((x2[N1-1]-x2[N1-2])*(x2[N1-1]-x2[N1-2])+
		  (y2[N1-1]-y2[N1-2])*(y2[N1-1]-y2[N1-2])+
		  (z2[N1-1]-z2[N1-2])*(z2[N1-1]-z2[N1-2]));

  distfield+=sqrt( (x2[N1-1]-point[2].getX())*(x2[N1-1]-point[2].getX())+
		   (y2[N1-1]-point[2].getY())*(y2[N1-1]-point[2].getY())+
		   (z2[N1-1]-point[2].getZ())*(z2[N1-1]-point[2].getZ()) );
		  
		  
  dist[2]=distfield+dist[0]+dist[1];
  dist[3]=dist[2]+sqrt((point[3].getX()-point[2].getX())*
		       (point[3].getX()-point[2].getX())+
		       (point[3].getY()-point[2].getY())*
		       (point[3].getY()-point[2].getY())+
		       (point[3].getZ()-point[2].getZ())*
		       (point[3].getZ()-point[2].getZ())); 
}
Double_t HMdcTrackGSpline::getMetaDistance(Double_t x,Double_t y,Double_t z)
{
  metaDistance=sqrt((x-point[2].getX())*(x-point[2].getX())+
		    (y-point[2].getY())*(y-point[2].getY())+
		    (z-point[2].getZ())*(z-point[2].getZ()))+
    distfield;
  return metaDistance;
}

Double_t HMdcTrackGSpline::getFieldDistance()
{
return distfield;
}

void HMdcTrackGSpline::fieldequation()
{
  Double_t k;
  Double_t A,B;
  for (Int_t i=0;i<N2;i++)
    {
      k=2*sqrt(1+secDer[i].getY()*secDer[i].getY()+
	       secDer[i].getX()*secDer[i].getX());
      // Y istigametinde
      
       B=k*(BB[i].getX()*(1+secDer[i].getY()*secDer[i].getY()));
       
      // X istigametinde
      A=k*(BB[i].getZ()*secDer[i].getY()+
	   BB[i].getX()*secDer[i].getX()*secDer[i].getY()-
	   BB[i].getY()*(1+secDer[i].getX()*secDer[i].getX()));
      
      field[i].setXYZ(A,B,fieldpoints[i].getZ());
      
    }
}
void HMdcTrackGSpline::solveMomentum()
{ 
  Float_t xVal[2]={0.,0.};
  Float_t yVal[2]={0.,0.};
  SolveDiffEquation(field,equationXY,N2,xVal,yVal);
}         
Int_t HMdcTrackGSpline::getMomentum(Bool_t cond,Int_t nchambers)
{
  polarity=1;
  precon=SplineMinimize(equationXY,fieldpoints,N2);
  if (precon<0)
     {
	precon*=-1;
	polarity*=-1;
	
     }  
  else  if(isSecDerY)
     {
	polarity=-1;
     }
  else
     {
	polarity=1;
     }
  Double_t corr1=1.;
  if (cond){ //bashla cond
    for(Int_t k=0;k<33;k++) //teta
      for(Int_t i=0;i<15;i++) //phi
	for (Int_t j=0;j<59;j++) //momentum
	  {
	    if(phidig>90)
	      phidigg=180-phidig;
	    if(phidigg >= c->phiI[i] && phidigg < c->phiI[i+1] &&
	       precon >= c->precI[j] && precon < c->precI[j+1] && tetadig >= c->tetaI[k] && tetadig < c->tetaI[k+1])
	      {
		
		
		if(polarity==-1)
		  {
		    if(nchambers==4)
		      getcorrE(k,i,j,corr1,4);
		    else if (nchambers==3)
		      getcorrE(k,i,j,corr1,3);
		  }
		else if(polarity==1)
		  {
		    if(nchambers==4)
		      getcorrP(k,i,j,corr1,4);
		    else if(nchambers==3)
		      getcorrP(k,i,j,corr1,3);
		  }
		precon/=corr1;
		if(precon<0)precon*=-1.;
		return 1;   
	      }
	  }
  }
  return 1;
}
Double_t HMdcTrackGSpline::calcMomentum(HMdcSeg *seg[2],Bool_t condition,Double_t targ)
{
  HGeomVector segpoints[4];
  calcSegPoints(seg,segpoints);
  return calcMomentum(segpoints,condition,targ,4);
}



Double_t HMdcTrackGSpline::calcMomentum123(HMdcSeg *seg[2],Bool_t condition,Double_t targ)
{
  HGeomVector segpoints[4];
  calcSegPoints123(seg,segpoints);
  return calcMomentum(segpoints,condition,targ,3);
}

Double_t HMdcTrackGSpline::calcMomentum123P4(HMdcSeg *seg[2],Bool_t condition,Double_t targ)
{
   HGeomVector segpoints[4];
   calcSegPoints123P4(seg,segpoints);
   return calcMomentum(segpoints,condition,targ,3);
}

void HMdcTrackGSpline::calcSegPoints(HMdcSeg *SEG[2],HGeomVector *SEGPOINTS)
{
  Double_t tetaseg[2],phiseg[2],zseg[2],roseg[2];
  tetaseg[0]=SEG[0]->getTheta();
  phiseg[0]=SEG[0]->getPhi();
  zseg[0]=SEG[0]->getZ();
  roseg[0]=SEG[0]->getR();
  
  tetaseg[1]=SEG[1]->getTheta();
  phiseg[1]=SEG[1]->getPhi();
  zseg[1]=SEG[1]->getZ();
  zGlobal=zseg[0];
  roseg[1]=SEG[1]->getR();
  
  Double_t Xseg[4],Yseg[4],Zseg[4];
  
  Xseg[0]=roseg[0]*cos(phiseg[0]+pi/2);
  Yseg[0]=roseg[0]*sin(phiseg[0]+pi/2);
  Zseg[0]=zseg[0];
  Xseg[1]=Xseg[0]+cos(phiseg[0])*sin(tetaseg[0]);
  Yseg[1]=Yseg[0]+sin(phiseg[0])*sin(tetaseg[0]);
  Zseg[1]=Zseg[0]+cos(tetaseg[0]);
  
  Xseg[2]=roseg[1]*cos(phiseg[1]+pi/2);
  Yseg[2]=roseg[1]*sin(phiseg[1]+pi/2);
  Zseg[2]=zseg[1];
  Xseg[3]=Xseg[2]+cos(phiseg[1])*sin(tetaseg[1]);
  Yseg[3]=Yseg[2]+sin(phiseg[1])*sin(tetaseg[1]);
  Zseg[3]=Zseg[2]+cos(tetaseg[1]);
  
  SEGPOINTS[0].setXYZ(0.1*Xseg[0],0.1*Yseg[0],0.1*Zseg[0]);
  SEGPOINTS[1].setXYZ(0.1*Xseg[1],0.1*Yseg[1],0.1*Zseg[1]);
  SEGPOINTS[2].setXYZ(0.1*Xseg[2],0.1*Yseg[2],0.1*Zseg[2]);
  SEGPOINTS[3].setXYZ(0.1*Xseg[3],0.1*Yseg[3],0.1*Zseg[3]);   
}



void HMdcTrackGSpline::calcSegPoints(HGeomVector *SEGPOINTS,HMdcSeg *SEG)
{
  Double_t tetaseg,phiseg,zseg,roseg;
  tetaseg=SEG->getTheta();
  phiseg=SEG->getPhi();
  zseg=SEG->getZ();
  roseg=SEG->getR();
  zGlobal=zseg;
  
  Double_t Xseg[2],Yseg[2],Zseg[2];
   
  Xseg[0]=roseg*cos(phiseg+pi/2);
  Yseg[0]=roseg*sin(phiseg+pi/2);
  Zseg[0]=zseg;
  Xseg[1]=Xseg[0]+cos(phiseg)*sin(tetaseg);
  Yseg[1]=Yseg[0]+sin(phiseg)*sin(tetaseg);
  Zseg[1]=Zseg[0]+cos(tetaseg);
  
  SEGPOINTS[0].setXYZ(0.1*Xseg[0],0.1*Yseg[0],0.1*Zseg[0]);
  SEGPOINTS[1].setXYZ(0.1*Xseg[1],0.1*Yseg[1],0.1*Zseg[1]);
}



void HMdcTrackGSpline::calcErrors(Double_t *tr)
{
  Double_t dError;
  Double_t error;
  error=sqrt((cos(tr[0])*tr[6])*(cos(tr[0])*tr[6])+tr[2]*tr[2]*(sin(tr[0])*tr[4])*(sin(tr[0])*tr[4]));
  
  dError=sqrt((cos(tr[0])*sin(tr[1]))*(cos(tr[0])*sin(tr[1]))*tr[4]*tr[4]+
	      (sin(tr[0])*cos(tr[1]))*(sin(tr[0])*cos(tr[1]))*tr[5]*tr[5]);
  errorY=0.1*sqrt(error*error+dError*dError);
  
}



void HMdcTrackGSpline::calcSegPoints123(HMdcSeg *SEG[2],HGeomVector *SEGPOINTS)
{
  
  Int_t sectorG=SEG[0]->getSec();
  Int_t moduleG=2;
  Double_t tetaseg[2],phiseg[2],zseg[2],roseg[2];
  tetaseg[0]=SEG[0]->getTheta();
  phiseg[0]=SEG[0]->getPhi();
  zseg[0]=SEG[0]->getZ();
  roseg[0]=SEG[0]->getR();
  zGlobal=zseg[0];
  
  tetaseg[1]=SEG[1]->getTheta();
  phiseg[1]=SEG[1]->getPhi();
  zseg[1]=SEG[1]->getZ();
  roseg[1]=SEG[1]->getR();
  
  Double_t Xseg[4],Yseg[4],Zseg[4];
  
  Xseg[0]=roseg[0]*cos(phiseg[0]+pi/2);
  Yseg[0]=roseg[0]*sin(phiseg[0]+pi/2);
  Zseg[0]=zseg[0];
  Xseg[1]=Xseg[0]+cos(phiseg[0])*sin(tetaseg[0]);
  Yseg[1]=Yseg[0]+sin(phiseg[0])*sin(tetaseg[0]);
  Zseg[1]=Zseg[0]+cos(tetaseg[0]);
  Double_t dirX=Xseg[1]-Xseg[0];
  Double_t dirY=Yseg[1]-Yseg[0];
  Double_t dirZ=Zseg[1]-Zseg[0]; 
  //Intersection with kickplane
  HGeomVector point,dir,interkickpoint;
  point.setXYZ(Xseg[0],Yseg[0],Zseg[0]);
  dir.setXYZ(dirX,dirY,dirZ);
  kickplane->calcIntersection(point,dir,interkickpoint);
  Xseg[2]=roseg[1]*cos(phiseg[1]+pi/2);
  Yseg[2]=roseg[1]*sin(phiseg[1]+pi/2);
  Zseg[2]=zseg[1];
  Xseg[3]=Xseg[2]+cos(phiseg[1])*sin(tetaseg[1]);
  Yseg[3]=Yseg[2]+sin(phiseg[1])*sin(tetaseg[1]);
  Zseg[3]=Zseg[2]+cos(tetaseg[1]); 
  //Third point from KickPlane, 4-from the middle of 3 chamber
  //calculation of 4 point
  HGeomVector point3, point4,interpoint;//interpoint in sm
  point3.setXYZ(0.1*Xseg[2],0.1*Yseg[2],0.1*Zseg[2]);
  point4.setXYZ(0.1*Xseg[3],0.1*Yseg[3],0.1*Zseg[3]);
  calcInter(middleGA[sectorG][moduleG],middleGB[sectorG][moduleG],middleGD[sectorG][moduleG],point3,point4,interpoint);
  
   
  Xseg[2]=interkickpoint.getX();
  Yseg[2]=interkickpoint.getY();
  Zseg[2]=interkickpoint.getZ();
   
   Xseg[3]=interpoint.getX();
   Yseg[3]=interpoint.getY();
   Zseg[3]=interpoint.getZ();
   
   SEGPOINTS[0].setXYZ(0.1*Xseg[0],0.1*Yseg[0],0.1*Zseg[0]);
   SEGPOINTS[1].setXYZ(0.1*Xseg[1],0.1*Yseg[1],0.1*Zseg[1]);
   SEGPOINTS[2].setXYZ(0.1*Xseg[2],0.1*Yseg[2],0.1*Zseg[2]);
   SEGPOINTS[3].setXYZ(Xseg[3],Yseg[3],Zseg[3]);   
}


void HMdcTrackGSpline::calcSegPoints123P4(HMdcSeg *SEG[2],HGeomVector *SEGPOINTS)
{
  Int_t sectorG=SEG[0]->getSec();
  Int_t moduleG=3;

  Double_t tetaseg[2],phiseg[2],zseg[2],roseg[2];
  tetaseg[0]=SEG[0]->getTheta();
  phiseg[0]=SEG[0]->getPhi();
  zseg[0]=SEG[0]->getZ();
  roseg[0]=SEG[0]->getR();
  zGlobal=zseg[0];
  
  tetaseg[1]=SEG[1]->getTheta();
  phiseg[1]=SEG[1]->getPhi();
  zseg[1]=SEG[1]->getZ();
  roseg[1]=SEG[1]->getR();
  
  Double_t Xseg[4],Yseg[4],Zseg[4];
  
  Xseg[0]=roseg[0]*cos(phiseg[0]+pi/2);
  Yseg[0]=roseg[0]*sin(phiseg[0]+pi/2);
  Zseg[0]=zseg[0];
  Xseg[1]=Xseg[0]+cos(phiseg[0])*sin(tetaseg[0]);
  Yseg[1]=Yseg[0]+sin(phiseg[0])*sin(tetaseg[0]);
  Zseg[1]=Zseg[0]+cos(tetaseg[0]);
  Double_t dirX=Xseg[1]-Xseg[0];
  Double_t dirY=Yseg[1]-Yseg[0];
  Double_t dirZ=Zseg[1]-Zseg[0]; 
  //Intersection with kickplane
  HGeomVector point,dir,interkickpoint;
  point.setXYZ(Xseg[0],Yseg[0],Zseg[0]);
  dir.setXYZ(dirX,dirY,dirZ);
  kickplane->calcIntersection(point,dir,interkickpoint);
  Xseg[2]=roseg[1]*cos(phiseg[1]+pi/2);
  Yseg[2]=roseg[1]*sin(phiseg[1]+pi/2);
  Zseg[2]=zseg[1];
  Xseg[3]=Xseg[2]+cos(phiseg[1])*sin(tetaseg[1]);
  Yseg[3]=Yseg[2]+sin(phiseg[1])*sin(tetaseg[1]);
  Zseg[3]=Zseg[2]+cos(tetaseg[1]); 
  //Third point from KickPlane, 4-from the middle of 3 chamber
  //calculation of 4 point
  HGeomVector point3, point4,interpoint;//interpoint in sm
  point3.setXYZ(0.1*Xseg[2],0.1*Yseg[2],0.1*Zseg[2]);
  point4.setXYZ(0.1*Xseg[3],0.1*Yseg[3],0.1*Zseg[3]);
  calcInter(middleGA[sectorG][moduleG],middleGB[sectorG][moduleG],middleGD[sectorG][moduleG],point3,point4,interpoint);
  
  Xseg[2]=interkickpoint.getX();
  Yseg[2]=interkickpoint.getY();
  Zseg[2]=interkickpoint.getZ();
   
   Xseg[3]=interpoint.getX();
   Yseg[3]=interpoint.getY();
   Zseg[3]=interpoint.getZ();
   
   SEGPOINTS[0].setXYZ(0.1*Xseg[0],0.1*Yseg[0],0.1*Zseg[0]);
   SEGPOINTS[1].setXYZ(0.1*Xseg[1],0.1*Yseg[1],0.1*Zseg[1]);
   SEGPOINTS[2].setXYZ(0.1*Xseg[2],0.1*Yseg[2],0.1*Zseg[2]);
   SEGPOINTS[3].setXYZ(Xseg[3],Yseg[3],Zseg[3]);   
}


void HMdcTrackGSpline::getDistance(Double_t *distance)
{
   for(Int_t i=0; i<4; i++)
      distance[i]=10*dist[i];
}

Double_t HMdcTrackGSpline::calcMomentum(HGeomVector  *pp,Bool_t cond,Double_t targ,Int_t nchambers)
{
   target=targ*0.1; 
   HGeomVector ppp[4];
   
   //   Double_t   A1=-0.00432978, B1=  0.976708,   D1=  75.573;
   //   Double_t   A2=-0.00350257, B2=  1.373,      D2=  124.44;
   //   Double_t   A3=0.00610111,  B3=  1.17128,    D3=  209.477;
   //   Double_t   A4=0.0220935,   B4=  1.18706,    D4=  260.799;
   //Double_t   A=  0.00610111B=  1.17128D=  2125.5;
   
   calcInter(A1,B1,D1,pp[0],pp[1],ppp[0]);
   calcInter(A2,B2,D2,pp[0],pp[1],ppp[1]);
   calcInter(A3,B3,D3,pp[2],pp[3],ppp[2]);
   calcInter(A4,B4,D4,pp[2],pp[3],ppp[3]);
   
   Double_t xVector[3],fieldVector[3];
   
   x[0]=ppp[0].getX();
   x[1]=ppp[0].getY();
   x[2]=ppp[0].getZ();
   x[3]=ppp[1].getX();
   x[4]=ppp[1].getY();
   x[5]=ppp[1].getZ();
   x[6]=ppp[2].getX();
   x[7]=ppp[2].getY();
   x[8]=ppp[2].getZ();
   x[9]=ppp[3].getX();
   x[10]=ppp[3].getY();
   x[11]=ppp[3].getZ();
   getPoint();
   spline(point,YZ,YZP,XZ,XZP,4);
   getpoints();
  // transFieldpoints();
   
  for(Int_t i=0;i<N2;i++)
      {
//	 xVector[0]=Tfieldpoints[i].getX();
//	 xVector[1]=Tfieldpoints[i].getY();
//	 xVector[2]=Tfieldpoints[i].getZ();
	 	 
	 
	 xVector[0]=fieldpoints[i].getX();
	 xVector[1]=fieldpoints[i].getY();
	 xVector[2]=fieldpoints[i].getZ();
	 
	 
	 
	 b->calcField(xVector,fieldVector,0.7215);
	 Bx[i]=fieldVector[0];
	 By[i]=fieldVector[1];
	 Bz[i]=fieldVector[2];
	 BB[i].setXYZ(Bx[i],By[i],Bz[i]);
      }
  // init1();
   fieldequation();
   solveMomentum();
   getMomentum(cond,nchambers);
   return precon;
} 
void HMdcTrackGSpline::getcorrE(Int_t k,Int_t i, Int_t j, Double_t & corr,Int_t nchambers)
{

   HMdcTrackGCorrections *cc=scanCorrections();
   
  // cc=c;
  
  if(nchambers==4)
    {
      corr=cc->corrE[k][i][j]+
	(cc->corrE[k][i][j+1]-cc->corrE[k][i][j])*(precon-cc->precI[j])/
	(cc->precI[j+1]-cc->precI[j])+(cc->corrE[k][i+1][j]-cc->corrE[k][i][j])*(phidigg-cc->phiI[i])/
	(cc->phiI[i+1]-cc->phiI[i])+(cc->corrE[k+1][i][j]-cc->corrE[k][i][j])*(tetadig-cc->tetaI[k])/
	(cc->tetaI[k+1]-cc->tetaI[k]);
    }
  else if(nchambers==3)
    {
      corr=cc->corrE1[k][i][j]+
	(cc->corrE1[k][i][j+1]-cc->corrE1[k][i][j])*(precon-cc->precI[j])/
	(cc->precI[j+1]-cc->precI[j])+(cc->corrE1[k][i+1][j]-cc->corrE1[k][i][j])*(phidigg-cc->phiI[i])/
	(cc->phiI[i+1]-cc->phiI[i])+(cc->corrE1[k+1][i][j]-cc->corrE1[k][i][j])*(tetadig-cc->tetaI[k])/
	(cc->tetaI[k+1]-cc->tetaI[k]);
    }
}
void HMdcTrackGSpline::getcorrP(Int_t k,Int_t i, Int_t j, Double_t & corr,Int_t nchambers)
{  

HMdcTrackGCorrections *cc=scanCorrections();
//cc=c;
  if(nchambers==4)
    {
      corr=cc->corrP[k][i][j]+
	(cc->corrP[k][i][j+1]-cc->corrP[k][i][j])*(precon-cc->precI[j])/
	(cc->precI[j+1]-cc->precI[j])+(cc->corrP[k][i+1][j]-cc->corrP[k][i][j])*(phidigg-cc->phiI[i])/
	(cc->phiI[i+1]-cc->phiI[i])+(cc->corrP[k+1][i][j]-cc->corrP[k][i][j])*(tetadig-cc->tetaI[k])/
	(cc->tetaI[k+1]-cc->tetaI[k]);
    }
  else if (nchambers==3)
    {
      corr=cc->corrP1[k][i][j]+
	(cc->corrP1[k][i][j+1]-cc->corrP1[k][i][j])*(precon-cc->precI[j])/
	(cc->precI[j+1]-cc->precI[j])+(cc->corrP1[k][i+1][j]-cc->corrP1[k][i][j])*(phidigg-cc->phiI[i])/
	(cc->phiI[i+1]-cc->phiI[i])+(cc->corrP1[k+1][i][j]-cc->corrP1[k][i][j])*(tetadig-cc->tetaI[k])/
	(cc->tetaI[k+1]-cc->tetaI[k]);
    }
}

void HMdcTrackGSpline::setDataPointer(HMdcTrackGField* field,HMdcTrackGCorrections* corr)
{
  b=field;
  c=corr;
}

void HMdcTrackGSpline::setCorrScan(HMdcTrackGCorrections* corr[])
{
   for(Int_t i=0; i<3; i++)
      {
         corrScan[i]=corr[i];
      }
}


void HMdcTrackGSpline::setCorrPointer(HMdcTrackGCorrections* corr)
{
  c=corr;
}

void HMdcTrackGSpline::setKickPointer(HMdcKickPlane *kickpointer)
{
  kickplane=kickpointer;
  isKickIsInitialized=kTRUE;
}
void HMdcTrackGSpline::calcInter(Double_t A,Double_t B,Double_t D,HGeomVector in1,HGeomVector in2,HGeomVector &out)
{
  Double_t x1,y1,z1,x2,y2,z2,x,y,z;
  x1=in1.getX();
  y1=in1.getY();
  z1=in1.getZ();
  
  x2=in2.getX();
  y2=in2.getY();
  z2=in2.getZ();
  
  Double_t dX=x2-x1;
  Double_t dY=y2-y1;
  Double_t dZ=z2-z1;
  Double_t del=1/(A*dX+B*dY+dZ);
  
  x=(dX*(D-z1-B*y1)+x1*(B*dY+dZ))*del;
  y=(dY*(D-z1-A*x1)+y1*(A*dX+dZ))*del;
  z=D-A*x-B*y;
  out.setXYZ(x,y,z);
}
void HMdcTrackGSpline::equationofLine(Double_t* xz1,Double_t *xz2,Double_t Z,Double_t &X)
{
  Double_t A,B;
  Double_t delta,deltaA,deltaB;
  delta=xz1[1]-xz2[1];
  deltaA=xz1[0]-xz2[0];
  deltaB=xz1[1]*xz2[0]-xz1[0]*xz2[1];
  A=deltaA/delta;
  B=deltaB/delta;
  X=A*Z+B;
}

void HMdcTrackGSpline::equationofLine3D(HGeomVector point1,HGeomVector point2,Double_t Z,Double_t &X,Double_t &Y)
{
  Double_t t;
  t=(Z-point1.getZ())/(point2.getZ()-point1.getZ());
  X=point1.getX()+t*(point2.getX()-point1.getX());
  Y=point1.getY()+t*(point2.getY()-point1.getY());
}
HGeomVector HMdcTrackGSpline::calcMetaDir(HMdcSeg *segment, HGeomVector fTof)
{
  HGeomVector directionToMeta;
  Double_t distanceMeta;
  HGeomVector   kickIntersection;
  kickIntersection=calcKickIntersection(segment);  
  directionToMeta=fTof-kickIntersection;
  distanceMeta=directionToMeta.length();
  directionToMeta/=distanceMeta;
  return directionToMeta;
}
HGeomVector HMdcTrackGSpline::calcKickIntersection(HMdcSeg *segment)
{
  HGeomVector XXpoint;
  HGeomVector XXdirection;
  HGeomVector kickIntersection;
  XX=(segment->getR())*cos(segment->getPhi()+pi/2);
  YY=(segment->getR())*sin(segment->getPhi()+pi/2);
  ZZ=segment->getZ();
  XXdir=cos(segment->getPhi())*sin(segment->getTheta());
  YYdir=sin(segment->getPhi())*sin(segment->getTheta());
  ZZdir=cos(segment->getTheta());
  XXpoint.setXYZ(XX,YY,ZZ);
  XXdirection.setXYZ(XXdir,YYdir,ZZdir);
  kickplane->calcIntersection(XXpoint,XXdirection,kickIntersection);
  return kickIntersection;
} 

void HMdcTrackGSpline::calcKickIntersectionErr(HMdcSeg *segment,HGeomVector &kickIntersection,HGeomVector &kickErr)
{
  HGeomVector XXpoint;
  HGeomVector XXdirection;
  HGeomVector intError;
  HGeomVector kickIntersectionDev[6];
  HGeomVector XXpointDev[3];
  HGeomVector XXdirectionDev[3]; 
  
  Float_t cosT=cos(segment->getTheta());
  Float_t sinT=sin(segment->getTheta());
  Float_t cosPhi=cos(segment->getPhi());
  Float_t sinPhi=sin(segment->getPhi());
  Float_t zz=segment->getZ();
  Float_t errRho=segment->getErrR();
  Float_t errPhi=segment->getErrPhi();
  Float_t errTheta=segment->getErrTheta();
  
  XX=(segment->getR())*cos(segment->getPhi()+pi/2);
  YY=(segment->getR())*sin(segment->getPhi()+pi/2);
 
  ZZ=zz;
  XXdir=sinT*cosPhi;
  YYdir=sinT*sinPhi;
  ZZdir=cosT;

  XXpoint.setXYZ(XX,YY,ZZ);
  XXdirection.setXYZ(XXdir,YYdir,ZZdir);

  Float_t devXX=sqrt(TMath::Power(cos(segment->getPhi()+pi/2)*errRho,2)+TMath::Power(YY*errPhi,2));
  Float_t devYY=sqrt(TMath::Power(sin(segment->getPhi()+pi/2)*errRho,2)+TMath::Power(XX*errPhi,2));
  Float_t devZZ=segment->getErrZ();
  Float_t devXXdir=sqrt(TMath::Power(cosT*cosPhi*errTheta,2)+TMath::Power(sinT*sinPhi*errPhi,2));
  Float_t devYYdir=sqrt(TMath::Power(cosT*sinPhi*errTheta,2)+TMath::Power(sinT*cosPhi*errPhi,2));
  Float_t devZZdir=sqrt(TMath::Power(sinT*errTheta,2));
  

  XXpointDev[0].setXYZ(XX+devXX,YY,ZZ);
  XXpointDev[1].setXYZ(XX,YY+devYY,ZZ);
  XXpointDev[2].setXYZ(XX,YY,ZZ+devZZ);
  XXdirectionDev[0].setXYZ(XXdir+devXXdir,YYdir,ZZdir);
  XXdirectionDev[1].setXYZ(XXdir,YYdir+devYYdir,ZZdir);
  XXdirectionDev[2].setXYZ(XXdir,YYdir,ZZdir+devZZdir);

  kickplane->calcIntersection(XXpoint,XXdirection,kickIntersection);

  kickplane->calcIntersection(XXpointDev[0],XXdirection,kickIntersectionDev[0]); //0

  kickplane->calcIntersection(XXpoint,XXdirectionDev[0],kickIntersectionDev[1]); //1

  kickplane->calcIntersection(XXpointDev[1],XXdirection,kickIntersectionDev[2]); //2

  kickplane->calcIntersection(XXpoint,XXdirectionDev[1],kickIntersectionDev[3]); //3

  kickplane->calcIntersection(XXpointDev[2],XXdirection,kickIntersectionDev[4]); //4

  kickplane->calcIntersection(XXpoint,XXdirectionDev[2],kickIntersectionDev[5]); //5

  Float_t sigmaX=0; 
  Float_t sigmaY=0;   
  Float_t sigmaZ=0;
  for(Int_t i=0; i<6; i++)
    {
      sigmaX+=TMath::Power(kickIntersection.getX()-kickIntersectionDev[i].getX(),2);
      sigmaY+=TMath::Power(kickIntersection.getY()-kickIntersectionDev[i].getY(),2);
      sigmaZ+=TMath::Power(kickIntersection.getZ()-kickIntersectionDev[i].getZ(),2);
    }

  kickErr.setXYZ(sqrt(sigmaX),sqrt(sigmaY),sqrt(sigmaZ));
} 





void  HMdcTrackGSpline::mixPoints(HGeomVector &in,HGeomVector &out)
{
  Double_t inXtemp=in.getX();
  Double_t inYtemp=in.getY();
  Double_t inZtemp=in.getZ();
  out.setXYZ(inYtemp,-1*inXtemp,inZtemp);
}


void  HMdcTrackGSpline::transSpline(Double_t phi,Double_t teta, HGeomVector &in,HGeomVector & out)
{
  
  if(teta==0.)
    {
      phi*=acos(-1.)/180;
      
      Double_t inXtemp;
      Double_t inYtemp;
      Double_t inZtemp;
      
      
      inXtemp=in.getX()*cos(phi)+in.getY()*sin(phi);
      inYtemp=-in.getX()*sin(phi)+in.getY()*cos(phi);
      inZtemp=in.getZ();
      
      out.setXYZ(inXtemp,inYtemp,inZtemp);
      
      
    }
  else if(phi==0.)
    {
      teta*=acos(-1.)/180;
      
      Double_t inXtemp;
      Double_t inYtemp;
      Double_t inZtemp;
      inXtemp=in.getX();
      inYtemp=in.getY()*cos(teta)+in.getZ()*sin(teta);
      inZtemp=-in.getY()*sin(teta)+in.getZ()*cos(teta);
      out.setXYZ(inXtemp,inYtemp,inZtemp);
    }
}

void  HMdcTrackGSpline::transSplineB(Double_t phi,Double_t teta, HGeomVector &in,HGeomVector & out)
{
  
  if(teta==0.)
    {
      phi*=acos(-1.)/180;
      
      Double_t inXtemp;
      Double_t inYtemp;
      Double_t inZtemp;
      
      
      inXtemp=in.getX()*cos(phi)-in.getY()*sin(phi);
      inYtemp=in.getX()*sin(phi)+in.getY()*cos(phi);
      inZtemp=in.getZ();
      
      out.setXYZ(inXtemp,inYtemp,inZtemp);
      
      
    }
  else if(phi==0.)
    {
      teta*=acos(-1.)/180;
      
      Double_t inXtemp;
      Double_t inYtemp;
      Double_t inZtemp;
      inXtemp=in.getX();
      inYtemp=in.getY()*cos(teta)+in.getZ()*sin(teta);
      inZtemp=-in.getY()*sin(teta)+in.getZ()*cos(teta);
      out.setXYZ(inXtemp,inYtemp,inZtemp);
    }
}


Double_t  HMdcTrackGSpline::SplineMinimize(HGeomVector *eq,HGeomVector *fp,Int_t size)
{
  Double_t sumz=0.,sumz2=0.;
  Double_t sumY=0.,sumy=0.,sumYy=0.,sumyz=0.,sumYz=0.,sumY2=0.,sumy2=0.;
  Double_t sumX=0.,sumx=0.,sumXx=0.,sumxz=0.,sumXz=0.,sumX2=0.,sumx2=0.;
  for (Int_t i=1;i<size;i++)
    {
      sumz+=eq[i].getZ()-eq[0].getZ();
      sumY+=eq[i].getY();
      sumX+=eq[i].getX(); 
      
      sumYz+=eq[i].getY()*(eq[i].getZ()-eq[0].getZ());
      sumXz+=eq[i].getX()*(eq[i].getZ()-eq[0].getZ());
      
      sumyz+=fp[i].getY()*(eq[i].getZ()-eq[0].getZ());
      sumxz+=fp[i].getX()*(eq[i].getZ()-eq[0].getZ());
      
      sumy+=fp[i].getY();
      sumx+=fp[i].getX();
      
      sumYy+=eq[i].getY()*fp[i].getY();
      sumXx+=eq[i].getX()*fp[i].getX();
      
      sumz2+=(eq[i].getZ()-eq[0].getZ())*(eq[i].getZ()-eq[0].getZ());
      
      sumY2+=eq[i].getY()*eq[i].getY();
      sumX2+=eq[i].getX()*eq[i].getX();
      
      sumy2+=fp[i].getY()*fp[i].getY();
      sumx2+=fp[i].getX()*fp[i].getX();
    }
 
  Double_t delta[3][3]={{(Double_t)(N2-1),sumz,sumY},
			{sumz,sumz2,sumYz},
			{sumY,sumYz,sumY2}};
  
  Double_t delta1[3][3]={{(Double_t)(N2-1),sumz,sumy},
			 {sumz,sumz2,sumyz},
			 {sumY,sumYz,sumYy}};
  
  Double_t deltaConst1[3][3]={{sumy,sumz,sumY},
			      {sumyz,sumz2,sumYz},
			      {sumYy,sumYz,sumY2}};

  Double_t deltaConst2[3][3]={{(Double_t)(N2-1),sumy,sumY},
			      {sumz,sumyz,sumYz},
			      {sumY,sumYy,sumY2}};
  
  Double_t det3Delta=det3(delta);
  Double_t det3Delta1=det3(delta1);
  Double_t det3DeltaConst1=det3(deltaConst1);
  Double_t det3DeltaConst2=det3(deltaConst2);
  
  precon=det3Delta/det3Delta1;
  qSpline=0.;
  for(Int_t i=1; i<N2; i++)
    {
      qSpline+=(det3DeltaConst1/det3Delta+det3DeltaConst2/det3Delta*(eq[i].getZ()-eq[0].getZ())+1/precon*eq[i].getY()-fp[i].getY())*
	       (det3DeltaConst1/det3Delta+det3DeltaConst2/det3Delta*(eq[i].getZ()-eq[0].getZ())+1/precon*eq[i].getY()-fp[i].getY());
    }
  qSpline/=45.;
  qSpline*=10000.;
  return precon;
}

HGeomVector HMdcTrackGSpline::takeMiddleParams(HMdcGetContainers *fGetCont,Int_t s,Int_t m)
{
  //Takes the parameters of middle plane
  HGeomTransform labTrMod;
  HGeomVector params;
  if(!(fGetCont->getLabTransMod(labTrMod,s,m)))
     {
	params.setXYZ(0.,0.,0);
	return params;
     }
  const HGeomVector& vc=labTrMod.getTransVector();
  const HGeomRotation& rot=labTrMod.getRotMatrix();
  Double_t parC=rot(0)*rot(4)-rot(3)*rot(1);
  Double_t parA=(rot(3)*rot(7)-rot(6)*rot(4))/parC;
  Double_t parB=(rot(6)*rot(1)-rot(0)*rot(7))/parC;
  Double_t parD=parA*vc(0)+parB*vc(1)+vc(2);
  params.setXYZ(parA,parB,parD*0.1);
  return params;
}


Int_t HMdcTrackGSpline::takeMiddleParams(const HGeomTransform* labTrMod, Int_t s,Int_t m)
{
  //Takes the parameters of middle plane
  if(labTrMod == NULL){
      geomParams[s][m].setXYZ(0.,0.,0.);
      return 0;
  }

  const HGeomVector& vc=labTrMod->getTransVector();
  const HGeomRotation& rot=labTrMod->getRotMatrix();
  Double_t parC=rot(0)*rot(4)-rot(3)*rot(1);
  Double_t parA=(rot(3)*rot(7)-rot(6)*rot(4))/parC;
  Double_t parB=(rot(6)*rot(1)-rot(0)*rot(7))/parC;
  Double_t parD=parA*vc(0)+parB*vc(1)+vc(2);
  geomParams[s][m].setXYZ(parA,parB,parD*0.1);
  return 1;
}







Float_t HMdcTrackGSpline::calcTarDist(HGeomVector &p1,HGeomVector &p2,HGeomVector &p3)
{
   Float_t dirX21=p2.getX()-p1.getX();
   Float_t dirY21=p2.getY()-p1.getY();
   Float_t dirZ21=p2.getZ()-p1.getZ();
   Float_t dirX31=p3.getX()-p1.getX();
   Float_t dirY31=p3.getY()-p1.getY();
   Float_t dirZ31=p3.getZ()-p1.getZ();
   
   Float_t delta=dirX21*dirX21+dirY21*dirY21+dirZ21*dirZ21;
   
   Float_t t=(dirX21*dirX31+dirY21*dirY31+dirZ21*dirZ31)/delta;

   Float_t xx=dirX21*t+p1.getX();
   Float_t yy=dirY21*t+p1.getY();
   Float_t zz=dirZ21*t+p1.getZ();

   Float_t dist=(p3.getX()-xx)*(p3.getX()-xx)+(p3.getY()-yy)*(p3.getY()-yy)+(p3.getZ()-zz)*(p3.getZ()-zz);

   return sqrt(dist);
}

Float_t HMdcTrackGSpline::calcTarDist(HVertex &vertex,HMdcSeg *SEG,HGeomTransform *secLabTrans)
{
   HGeomVector segpoints[2];
   HGeomVector vertPoint;
   if(vertex.getZ()==-1000) return -1000.;
   vertPoint.setXYZ(vertex.getX(),vertex.getY(),vertex.getZ());
   vertPoint=secLabTrans->transTo(vertPoint); //in sector;
   calcSegPoints(segpoints,SEG);
 
   segpoints[0]*=10.;
   segpoints[1]*=10.;
   
   return (calcTarDist(segpoints[0],segpoints[1],vertPoint));
}

void HMdcTrackGSpline::SolveDiffEquation(HGeomVector *in,HGeomVector *out,Int_t size,Float_t *inXval,Float_t *inYval)
{
  Double_t xDir[size],yDir[size];
  Double_t xDirP[size-1],yDirP[size-1];
  Float_t Yequation[size],Xequation[size];
  Float_t ypr[size]; 
  Float_t xpr[size];
  Float_t h[size];
  
  ypr[0]=inYval[0];
  xpr[0]=inXval[0];
  Yequation[0]=inYval[1];
  Xequation[0]=inXval[1];
  
  //for(Int_t i=0; i<size; i++)
  //{
  //  out[i].setZ(in[i].getZ());
  //}
  spline(in,yDir,yDirP,xDir,xDirP,size);
  
  for (Int_t i = 1; i<N2-1; i++)
      h[i] = in[i+1].getZ() - in[i].getZ();
      h[0]=0;  
  for(Int_t i = 0; i<size-1; i++)
    {
//      h[i] = in[i+1].getZ() - in[i].getZ();
      
      xpr[i+1]=xpr[i]+h[i]*(in[i+1].getX()+in[i].getX())/(2)-
	h[i]*h[i]*h[i]*(xDir[i]+xDir[i+1])/24;
      
      ypr[i+1]=ypr[i]+h[i]*(in[i+1].getY()+in[i].getY())/(2)-
	h[i]*h[i]*h[i]*(yDir[i]+yDir[i+1])/24;
      
    }
  
  
  for(Int_t i=0;i<size-1;i++)
    {
      Xequation[i+1]=Xequation[i]+
	xpr[i]*h[i]+h[i]*h[i]*(2*in[i].getX()+in[i+1].getX())/(6)-
	h[i]*h[i]*h[i]*h[i]*(0.8*xDir[i]+0.7*xDir[i+1])/(36);
      
      Yequation[i+1]=Yequation[i]+
	ypr[i]*h[i]+h[i]*h[i]*(2*in[i].getY()+in[i+1].getY())/(6)-
	h[i]*h[i]*h[i]*h[i]*(0.8*yDir[i]+0.7*yDir[i+1])/(36);
      
    }
  for(Int_t i=0; i<size;i++)
    {
      out[i].setXYZ(Xequation[i],Yequation[i],in[i].getZ());
      //out[i].setY(Yequation[i]);
    }
  
}

  void HMdcTrackGSpline::getXYpoint(Float_t *x,Float_t *y,Float_t *z)
  {
 
  x[0]=point[0].getX();
  y[0]=point[0].getY();
  z[0]=point[0].getZ();
  x[1]=point[1].getX();
  y[1]=point[1].getY();
  z[1]=point[1].getZ();
  for(Int_t i=2; i<N2+2; i++)
  {
  x[i]=fieldpoints[i-2].getX();
  y[i]=fieldpoints[i-2].getY();
  z[i]=fieldpoints[i-2].getZ();
  }
  x[N2+2]=point[2].getX();
  y[N2+2]=point[2].getY();
  z[N2+2]=point[2].getZ();
  x[N2+3]=point[3].getX();
  y[N2+3]=point[3].getY();
  z[N2+3]=point[3].getZ();
    }
    
void HMdcTrackGSpline::getField(Float_t *x,Float_t *y,Float_t *z)
{
for(Int_t i=0; i<N2; i++)
  {
  x[i]=BB[i].getX();
  y[i]=BB[i].getY();
  z[i]=BB[i].getZ();
  }
}

void  HMdcTrackGSpline::transSplineLocal(Double_t phi,Double_t teta, HGeomVector &in,HGeomVector & out)
{
  Double_t inXtemp;
  Double_t inYtemp;
  Double_t inZtemp;
  inXtemp=in.getX()*cos(phi)*cos(teta)+in.getY()*sin(phi)*cos(teta)-in.getZ()*sin(teta);
  inYtemp=-in.getX()*sin(phi)+in.getY()*cos(phi);
  inZtemp=in.getX()*cos(phi)*sin(teta)+in.getY()*sin(phi)*sin(teta)+in.getZ();
  out.setXYZ(inXtemp,inYtemp,inZtemp);
}

HMdcTrackGCorrections *HMdcTrackGSpline::scanCorrections()

{
   
//   zGlobal=0.;

   if(zGlobal>(-15))
     {
         return corrScan[0];
     }
   else
      if(zGlobal>(-35))
    {
            return corrScan[1];
    }
      else
    {
            return corrScan[2];
    }
   
}


void HMdcTrackGSpline::checkSecDer(Double_t *der2, Double_t *der1)
{
   isSecDerY=kFALSE;
   
   for(Int_t i=1; i<N2; i++)
      {
	 
	 curv[i]=fabs(der2[i])/(TMath::Power((1+der1[i]*der1[i]),3/2));
	 if(curv[i]>.5)
	    {
	       isSecDerY=kTRUE;
	       break;
	    }
	 
      }
   
   
}

Float_t HMdcTrackGSpline::calcIOMatching(HMdcSeg *segments[2])
{
  HGeomVector innerInt;
  HGeomVector innerErr;
  HGeomVector outerInt;
  HGeomVector outerErr;
  
  calcKickIntersectionErr(segments[0],innerInt,innerErr);
  calcKickIntersectionErr(segments[1],outerInt,outerErr);
  
  Float_t devvX=sqrt(TMath::Power(innerErr.getX(),2)+TMath::Power(outerErr.getX(),2));
  Float_t devvY=sqrt(TMath::Power(innerErr.getY(),2)+TMath::Power(outerErr.getY(),2));
  
  
  Float_t qIOMatching=TMath::Power((innerInt.getX()-outerInt.getX())/devvX,2)+
    TMath::Power((innerInt.getY()-outerInt.getY())/devvY,2); 
  
  return sqrt(qIOMatching);
}
