//*-- AUTHOR : A.Rustamov
#include "align.h"
#include "htool.h"
#include "hrootsource.h"
#include "hrktrackB.h"
#include "halignmentgshower.h"
#include "hshowergeompar.h"

using namespace std;

void align()
{
  Hades* hades = new Hades();
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
  spec->addDetector(new HShowerDetector);
  spec->addDetector(new HRpcDetector);
  for (Int_t is=0; is<6; is++) 
    {
      spec->getDetector("Shower")->setModules(is,showerMods);
      spec->getDetector("Tof")   ->setModules(is,tofMods);
      spec->getDetector("Mdc")->setModules(is,mdcMods[is]);
      spec->getDetector("Rpc")   ->setModules(is,rpcMods);
    }
  
  HAlignmentGShower *align = new HAlignmentGShower();
  align -> setAlignMode(0);
  TFile f("showerFile.root","read");
  TNtuple *nt = (TNtuple*)gDirectory->Get("nt");
  align -> SetNtuple(nt);
  TFile *ff = NULL;
  
  HParAsciiFileIo* in = new HParAsciiFileIo;
  in->open("showerAlignF0Use.txt","in");
  rtdb->setFirstInput(in);
  
  HParOra2Io* ora = new HParOra2Io;
  ora->open();
  ora->setHistoryDate("now");
  rtdb->setSecondInput(ora);
  
  HParAsciiFileIo* out = new HParAsciiFileIo;
  out->open("showerAlignF0UseOut.txt","out");
  rtdb->setOutput(out);
  
  rtdb->getContainer("MdcGeomPar");
  rtdb->getContainer("SpecGeomPar");
  HShowerGeometry* showerGeom =(HShowerGeometry*) rtdb->getContainer("ShowerGeometry");
  HShowerGeomPar *pShGeomPar=(HShowerGeomPar*) rtdb ->getContainer("ShowerGeomPar");
  
  HTofGeomPar*     tofgeom    =(HTofGeomPar*)     rtdb->getContainer("TofGeomPar");
  HRpcGeomPar *rpcGeom = (HRpcGeomPar*)rtdb->getContainer("RpcGeomPar");
  rtdb -> initContainers(89655379);
  HGeomTransform transAl[6];
  HGeomTransform transOld;
  HGeomTransform transNew;
  
  for(Int_t i = 0; i<6; i++)
    {
      if( i != 0 && i!=3 ) continue;
      
      cout<<"aligning"<<endl;
      
      
      transOld      = showerGeom -> getTransform(i);
      align      -> seTrans(transOld);
      align      -> AlignShower(i);
      transNew   = align -> getTransNew();
      transAl[i] = transNew;
      
      align -> CheckAlignment(transNew, transOld,ff);	              
      
      cout<<"before alignment"<<endl;
      transOld.print();
      cout<<"after alignment"<<endl;
      transNew.print();
      
    }
  
  for(Int_t i=0; i<6; i++)
    {
      if( i != 0 && i!=3 ) continue;
      
      for(Int_t j=0; j<3; j++)
	{
	  HGeomTransform  &transInUse = pShGeomPar -> getModule(i,j) -> getLabTransform();
	  transInUse.setRotMatrix(transAl[i].getRotMatrix());
	  transInUse.setTransVector(transAl[i].getTransVector());
	}
    }	 
  
  rtdb -> writeContainers();
  rtdb -> print();
  rtdb -> saveOutput();
  
}

int main(int argc, char **argv)
{
  align();
  return 1;
}

