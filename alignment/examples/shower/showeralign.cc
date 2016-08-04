//*-- AUTHOR : A.Rustamov

#include "showeralign.h"
#include "htool.h"
#include "hrootsource.h"
#include "hrktrackB.h"
#include  "halignmentgbase.h"
#include  "halignmentgdata.h"
#include  "halignmentgnomag.h"
#include  "halignmentggeom.h"
#include  "halignmentgrotations.h"

using namespace std;

void AlignASCIIout_V4(Text_t *mytext="2010", Int_t nevts = 50000000)
{
  Int_t nEvents = nevts;
  TString inputDir  ="./"; //put some input directory
  TString outputDir ="";  //put some output directory

  HEvent* event;
  HEventHeader* header;
  Hades* hades = new Hades();
  gHades->setQuietMode(2);
  Bool_t makeTree  = kFALSE;
  const Int_t numoffiles = 4;
  Int_t size = numoffiles;
  Char_t* files[numoffiles] =
    {
      "be1031714361910_NOV10_dst_v0_.root",
      "be1031714361910_NOV10_dst_v0__1.root",
      "be1031714361909_NOV10_dst_v0_.root",
      "be1031714361909_NOV10_dst_v0__1.root"
      
    };
  //--------------------setting root source--------------------------------
  HRuntimeDb* rtdb=gHades->getRuntimeDb();
  Int_t tofMods   [22]   = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Int_t showerMods[3]    = {1,2,3};
  Int_t rpcMods   [1]    = {1};

  Int_t mdcMods[6][4] = { {1,1,1,1},
                          {1,1,1,1},
                          {1,1,1,1},
                          {1,1,1,1},
                          {1,1,1,1},
                          {1,1,1,1} };

  HSpectrometer* spec = gHades->getSetup();
  spec->addDetector(new HMdcDetector);
  spec->addDetector(new HTofDetector);
  spec->addDetector(new HRpcDetector);
  spec->addDetector(new HShowerDetector);

  for (Int_t is=0; is<6; is++) {
    spec->getDetector("Mdc")   ->setModules(is,mdcMods[is]);
    spec->getDetector("Shower")->setModules(is,showerMods);
    spec->getDetector("Rpc")   ->setModules(is,rpcMods);
    spec->getDetector("Tof")   ->setModules(is,tofMods);
  }



   HParAsciiFileIo* in = new HParAsciiFileIo;
   in->open("rpc_shower_use.txt","in");
   rtdb->setFirstInput(in);

  HParOra2Io* ora = new HParOra2Io;
  ora->open();
  ora->setHistoryDate("now");
  rtdb->setSecondInput(ora);
//  rtdb->getContainer("MdcGeomPar");
//  rtdb->getContainer("SpecGeomPar");
  HAlignmentGNoMag *noMag   = new HAlignmentGNoMag();
  HShowerGeometry* showerGeom =(HShowerGeometry*) rtdb->getContainer("ShowerGeometry");
  
  HRootSource *source = new HRootSource;
  source->setDirectory((char*)inputDir.Data());

  for(Int_t ifile=0; ifile < numoffiles; ifile++)
    {
      cout << "Selected file = "<< files[ifile] << endl;
      source->addFile(files[ifile]);
    }
  gHades->setDataSource(source);

  if(!gHades->init())
    {
      cout << "gHades->init() ERROR " << endl;
      printf("\nError in Hades::init() \n");
      exit(2);
    }

  HGeomTransform trans[6][4];
  for(Int_t i=0; i<6; i++)
    for(Int_t j=0; j<4; j++)
      {
        trans[i][j] = *(noMag->GetAlignRot()->GetTransMdc(i,j));
      }
  
  HShowerHit* pShowerHit;
  HCategory*  pShowerCat =  (HCategory*)gHades->getCurrentEvent()->getCategory(catShowerHit);
  TIterator*  iterShowerCat =  pShowerCat -> MakeIterator();
  HCategory*   pMdcHitCat;
  pMdcHitCat = (HCategory*)gHades -> getCurrentEvent()->getCategory(catMdcHit);
  TIterator*   iterMdcHitCat = pMdcHitCat->MakeIterator();
  Int_t   counterShower[6] = {0,0,0,0,0,0};
  Int_t   counterMdc[6][4];
  HGeomVector pointLab1, pointLab2, pointLab4, pointLabShower;
  HGeomVector pointSec1, pointSec2;
  HGeomVector pointsMdc[6][4][50];
  Float_t xmdc[6][4][50];
  Float_t ymdc[6][4][50];
  Float_t xSh, ySh;
  HGeomVector pointsShower[6][50];
  HGeomTransform transShower;
  Float_t diffZ, diffX, diffY, xCr, yCr;
  TFile fsh(outputDir+"showerFile.root","recreate");
  TNtuple *nt = new TNtuple("nt","nt","x1:y1:z1:x2:y2:z2:xsh:ysh:zsh:xshl:yshl:zshl:xcr:ycr:s:x1s:y1s:z1s:x2s:y2s"
			    ":z2s:xv:yv:zv");
  for(Int_t eventcounter = 0; eventcounter < nevts; eventcounter++)
    {
      
      if(eventcounter%1000 == 0 ) cout<<"event == "<<eventcounter<<endl;
      
      Int_t retvalue=gHades->eventLoop(1);  // source event counter
      if(retvalue == 0)
	{
	  cout << "EventLoop return value is 0 at event " << eventcounter << endl;
	  break;
	}
      
      
      iterMdcHitCat->Reset();
      
      for(Int_t s = 0; s < 6; s++) for(Int_t m = 0; m < 4; m++ ) counterMdc[s][m] = 0;
      
      HMdcHit* pMdchit;
      while ( (pMdchit=(HMdcHit *)iterMdcHitCat->Next()) != 0 )
	{
	  if(pMdchit->getChi2() <= 0) continue;
	  
	  Int_t   sec = pMdchit->getSector();
	  Int_t   mod = pMdchit->getModule();
	  Float_t x   = pMdchit->getX();
	  Float_t y   = pMdchit->getY();
	  
	  if( fabs(x) <= 0.0001 || fabs(y) <=  0.0001 ) continue;
	  if( fabs(x) > 10000. || fabs(y) > 10000 )     continue;
	  
	  Int_t &counter = counterMdc[sec][mod];
	  if(counter >=50 ) {
	    cout<<"Number of MDC hits >50 !!!"<<endl;
	    break;
	  }
	  
	  pointsMdc[sec][mod][counter].setXYZ(x,y,0.);
	  counter++;
	}
      
      for(Int_t s = 0; s < 6; s++) counterShower[s] = 0;
      iterShowerCat->Reset();
      
      while ( (pShowerHit=(HShowerHit *)iterShowerCat->Next()) != 0 )
	{
	  if(pShowerHit -> getModule() != 0 ) continue;
	  Int_t sec = pShowerHit->getSector();
	  
	  Int_t &counter = counterShower[sec];
	  if( counter>= 5 ) {
	    break;
	  }
	  
	  
	  pShowerHit -> getXY(&xSh,&ySh);
	  pointsShower[sec][counter].setXYZ(xSh,ySh,pShowerHit->getZ());
	  counter++;
	}
      pShowerCat->Clear();
      
      
      for(Int_t s = 0; s < 6; s++) {
	if( counterMdc[s][0] ==0 || counterMdc[s][1] ==0 && counterMdc[s][3] == 0 ) continue;
	if( counterShower[s] ==0) continue;
	
	transShower = showerGeom -> getTransform(s);
	
	for(Int_t m1 = 0; m1 < counterMdc[s][0]; m1++) 
	  for(Int_t m2 = 0; m2 < counterMdc[s][1]; m2++)
	    for(Int_t m4 = 0; m4 < counterMdc[s][3]; m4++)
	      {
		HGeomVector &point1 = pointsMdc[s][0][m1];
		HGeomVector &point2 = pointsMdc[s][1][m2];
		HGeomVector &point4 = pointsMdc[s][3][m4];
		
		pointLab1  =  noMag -> GetAlignRot() -> TransMdc(point1,"FromModToLab",s,0);
		pointLab2  =  noMag -> GetAlignRot() -> TransMdc(point2,"FromModToLab",s,1);
		pointLab4  =  noMag -> GetAlignRot() -> TransMdc(point4,"FromModToLab",s,3);
		
		pointSec1  =  noMag -> GetAlignRot()-> TransMdc(point1,"FromModToSec",s,0); 
		pointSec2  =  noMag -> GetAlignRot()-> TransMdc(point2,"FromModToSec",s,1);
		
		Float_t dist = noMag -> GetAlignGeom() -> CalcDistanceToLine(pointLab2,pointLab1,pointLab4);
		if(dist >= .5) continue;
		
		
		for(Int_t iSh = 0; iSh < counterShower[s]; iSh++)
		  {
		    HGeomVector testCr = noMag -> GetAlignGeom() -> CalcIntersection(pointLab1,pointLab2,transShower);
		    testCr = transShower.transTo(testCr);
		    
		    HGeomVector NullPoint(0.,0.,0);
		    HGeomVector NullPointDir(0.,0.,1.);
		    
		    HGeomVector zCoord = noMag -> GetAlignGeom() -> 
		      CalcVertex(pointLab1,pointLab2-pointLab1,NullPoint,NullPointDir);
		    
		    HGeomVector &pointShower = pointsShower[s][iSh];
		    HGeomVector cutdist = pointsShower[s][iSh];
		    cutdist -= testCr;
		    if(cutdist.length() > 50.) continue;
		    
		    pointLabShower = transShower.transFrom(pointShower);
		    
		    Float_t ddata[] = {pointLab1.getX(), pointLab1.getY(),pointLab1.getZ(),
				       pointLab2.getX(), pointLab2.getY(),pointLab2.getZ(),
				       pointLabShower.getX(), pointLabShower.getY(),pointLabShower.getZ(),
				       pointShower.getX(), pointShower.getY(), pointShower.getZ(),
				       testCr.getX(), testCr.getY(),s,pointSec1.getX(),
				       pointSec1.getY(),pointSec1.getZ(),pointSec2.getX(),pointSec2.getY(),
				       pointSec2.getZ(), 
				       zCoord.getX(), zCoord.getY(),zCoord.getZ()};
		    
		    
		    nt -> Fill(ddata);
		    
		  }
	      }
      }
    }
  fsh.cd();
  nt->Write();	  
}

int main(int argc, char **argv)
{
  AlignASCIIout_V4();
  return 1;
}

