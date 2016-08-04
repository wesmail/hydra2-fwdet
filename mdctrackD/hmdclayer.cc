#include "hmdclayer.h"
#include "hmdcsizescells.h"
#include "hgeomvolume.h"

#include <iomanip>
#include <iostream>
#include <cmath>

using namespace std;
ClassImp(HMdcLayer)


HMdcLayer::HMdcLayer(){


    //MDCI
    xcrop[0][0] = 10;   // low
    xcrop[0][1] = 10;   //  up
    ycrop[0][0] =  0;
    ycrop[0][1] = 30;

    //MDCII
    xcrop[1][0] = 20;
    xcrop[1][1] = 20;
    ycrop[1][0] =  0;
    ycrop[1][1] =  0;

    //MDCIII
    xcrop[2][0] = 20;
    xcrop[2][1] = 40;
    ycrop[2][0] = 85;
    ycrop[2][1] = 77;

    //MDCIV
    xcrop[3][0] = 20;
    xcrop[3][1] = 40;
    ycrop[3][0] = 100;
    ycrop[3][1] =  0;

}

void   HMdcLayer::setMDCCrop(Int_t m,Double_t xlowcrop,Double_t xupcrop,Double_t ylowcrop,Double_t yupcrop)
{
    xcrop[m][0] = xlowcrop;
    xcrop[m][1] = xupcrop;
    ycrop[m][0] = ylowcrop;
    ycrop[m][1] = yupcrop;
}

void   HMdcLayer::getLayerPoints     (Int_t s,Int_t m,Int_t l, TVector2& p0,TVector2& p1,TVector2& p2,TVector2& p3)
{
    p0 = layerP[s][m][l][0];
    p1 = layerP[s][m][l][1];
    p2 = layerP[s][m][l][2];
    p3 = layerP[s][m][l][3];
}
void   HMdcLayer::getCropedLayerPoints(Int_t s,Int_t m,Int_t l, TVector2& p0,TVector2& p1,TVector2& p2,TVector2& p3)
{
    p0 = layerPcrop[s][m][l][0];
    p1 = layerPcrop[s][m][l][1];
    p2 = layerPcrop[s][m][l][2];
    p3 = layerPcrop[s][m][l][3];
}

Bool_t HMdcLayer::getLayersEdge(TVector2& p,
		     Int_t s,Int_t m,Int_t l,
		     Double_t& xmin ,Double_t& xmax,Double_t& ymin,Double_t& ymax,
		     Double_t& xmincrop ,Double_t& xmaxcrop,Double_t& ymincrop,Double_t& ymaxcrop
		    )
{
    // calculates min and max values of x and y  for a given point p
    // on the MDC layer. Both for full layer and croped layer
    //
    //                  y|
    //                   |
    //        p3         L2           p2
    //          --------------------
    //          \        |         /
    //           \       |        /
    //            \      |0,0    /    x
    //         ----\-----|------/----
    //           L3 \    |     /  L1
    //               \   |    /
    //                \  |   /
    //                 \ |  /
    //                  ----
    //                p4 |  p1
    //                   | L4
    //                   |
    //

    const TVector2& p1 = layerP[s][m][l][0];
    const TVector2& p2 = layerP[s][m][l][1];
    const TVector2& p3 = layerP[s][m][l][2];
    const TVector2& p4 = layerP[s][m][l][3];

    ymax = p2.Y();
    ymin = p1.Y();

    TVector2 dir1 = (p2-p1)/fabs(p2.Y()-p1.Y());
    TVector2 dir3 = (p3-p4)/fabs(p4.Y()-p3.Y());

    TVector2 pmax  = p1 + dir1 *  fabs(p.Y()-p1.Y());  //L1
    xmax =  pmax.X();
    pmax = p4 + dir3 *  fabs(p.Y()-p4.Y());            //L4
    xmin = pmax.X();

    if(p.Y()>ymax||p.Y()<ymin||p.X()>xmax||p.X()<xmin) {
	Error("getLayersEdge()","Point outside layer %i : %f,%f edge : x %f,%f , y %f,%f !",l,p.X(),p.Y(),xmin,xmax,ymin,ymax);
	return kFALSE;
    }


    const TVector2& p1c = layerPcrop[s][m][l][0];
    const TVector2& p2c = layerPcrop[s][m][l][1];
    const TVector2& p3c = layerPcrop[s][m][l][2];
    const TVector2& p4c = layerPcrop[s][m][l][3];

    ymaxcrop = p2c.Y();
    ymincrop = p1c.Y();

    dir1 = (p2c-p1c)/fabs(p2c.Y()-p1c.Y());
    dir3 = (p3c-p4c)/fabs(p4c.Y()-p3c.Y());

    pmax  = p1c + dir1 *  fabs(p.Y()-p1c.Y());  //L1
    xmaxcrop =  pmax.X();
    pmax = p4c + dir3 *  fabs(p.Y()-p4c.Y());   //L4
    xmincrop = pmax.X();

    return kTRUE;

}

Bool_t HMdcLayer::getLayersEdge(TVector2& p,
		     Int_t s,Int_t m,Int_t l,
		     Double_t& xmin ,Double_t& xmax,Double_t& ymin,Double_t& ymax
		    )
{
    // calculates min and max values of x and y  for a given point p
    // on the MDC layer. Both for full layer and croped layer
    //
    //                  y|
    //                   |
    //        p3         L2           p2
    //          --------------------
    //          \        |         /
    //           \       |        /
    //            \      |0,0    /    x
    //         ----\-----|------/----
    //           L3 \    |     /  L1
    //               \   |    /
    //                \  |   /
    //                 \ |  /
    //                  ----
    //                p4 |  p1
    //                   | L4
    //                   |
    //

    const TVector2& p1 = layerP[s][m][l][0];
    const TVector2& p2 = layerP[s][m][l][1];
    const TVector2& p3 = layerP[s][m][l][2];
    const TVector2& p4 = layerP[s][m][l][3];

    ymax = p2.Y();
    ymin = p1.Y();

    TVector2 dir1 = (p2-p1)/fabs(p2.Y()-p1.Y());
    TVector2 dir3 = (p3-p4)/fabs(p4.Y()-p3.Y());

    TVector2 pmax  = p1 + dir1 *  fabs(p.Y()-p1.Y());  //L1
    xmax =  pmax.X();
    pmax = p4 + dir3 *  fabs(p.Y()-p4.Y());            //L4
    xmin = pmax.X();

    if(p.Y()>ymax||p.Y()<ymin||p.X()>xmax||p.X()<xmin) {
	Error("getLayersEdge()","Point outside layer %i : %f,%f edge : x %f,%f , y %f,%f !",l,p.X(),p.Y(),xmin,xmax,ymin,ymax);
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HMdcLayer::getCropedLayersEdge(TVector2& p,
			   Int_t s,Int_t m,Int_t l,
			   Double_t& xmin ,Double_t& xmax,Double_t& ymin,Double_t& ymax
			  )
{
    // calculates min and max values of x and y  for a given point p
    // on the MDC layer. Both for full layer and croped layer
    //
    //                  y|
    //                   |
    //        p3         L2           p2
    //          --------------------
    //          \        |         /
    //           \       |        /
    //            \      |0,0    /    x
    //         ----\-----|------/----
    //           L3 \    |     /  L1
    //               \   |    /
    //                \  |   /
    //                 \ |  /
    //                  ----
    //                p4 |  p1
    //                   | L4
    //                   |
    //

    const TVector2& p1 = layerPcrop[s][m][l][0];
    const TVector2& p2 = layerPcrop[s][m][l][1];
    const TVector2& p3 = layerPcrop[s][m][l][2];
    const TVector2& p4 = layerPcrop[s][m][l][3];

    ymax = p2.Y();
    ymin = p1.Y();

    TVector2 dir1 = (p2-p1)/fabs(p2.Y()-p1.Y());
    TVector2 dir3 = (p3-p4)/fabs(p4.Y()-p3.Y());

    TVector2 pmax  = p1 + dir1 *  fabs(p.Y()-p1.Y());  //L1
    xmax =  pmax.X();
    pmax = p4 + dir3 *  fabs(p.Y()-p4.Y());            //L4
    xmin = pmax.X();

    if(p.Y()>ymax||p.Y()<ymin||p.X()>xmax||p.X()<xmin) {
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HMdcLayer::isCroped(TVector2& p,Int_t s,Int_t m,Int_t l)
{
    // calculates min and max values of x and y  for a given point p
    // on the MDC crop layer. return kTRUE i p is outside
    //
    //                  y|
    //                   |
    //        p3         L2           p2
    //          --------------------
    //          \        |         /
    //           \       |        /
    //            \      |0,0    /    x
    //         ----\-----|------/----
    //           L3 \    |     /  L1
    //               \   |    /
    //                \  |   /
    //                 \ |  /
    //                  ----
    //                p4 |  p1
    //                   | L4
    //                   |
    //

    const TVector2& p1 = layerPcrop[s][m][l][0];
    const TVector2& p2 = layerPcrop[s][m][l][1];
    const TVector2& p3 = layerPcrop[s][m][l][2];
    const TVector2& p4 = layerPcrop[s][m][l][3];
    Double_t xmin,xmax,ymin,ymax;

    ymax = p2.Y();
    ymin = p1.Y();

    TVector2 dir1 = (p2-p1)/fabs(p2.Y()-p1.Y());
    TVector2 dir3 = (p3-p4)/fabs(p4.Y()-p3.Y());

    TVector2 pmax  = p1 + dir1 *  fabs(p.Y()-p1.Y());  //L1
    xmax =  pmax.X();
    pmax = p4 + dir3 *  fabs(p.Y()-p4.Y());            //L4
    xmin = pmax.X();

    if(p.Y()>ymax||p.Y()<ymin||p.X()>xmax||p.X()<xmin) {
	return kTRUE;
    }
    return kFALSE;
}

void HMdcLayer::fillRKPlane(HMdcSizesCells* fSizesCells){
    //---------------------------------------------------
    // path length correction for vertex
    // Labsys
    Double_t rot[9] = {1.,0,0, 0.,1.,0., 0,0,1.};
    Double_t tr[3]  = {0.,0.,-500.};
    HGeomTransform trPlane;
    for(Int_t s=0;s<6;s++) {
	if(!fSizesCells->modStatus(s,0)) continue;
	HMdcSizesCellsSec& scsec = (*fSizesCells)[s];

	const HGeomTransform *trSecLab = scsec.getLabTrans();
	HMdcSizesCellsMod &sm    = scsec[0];
	const HGeomTransform* trModSeg = sm.getSecTrans();
	trPlane.setRotMatrix(rot);
	trPlane.setTransVector(tr);
	trPlane.transFrom(*trModSeg);
	trPlane.transFrom(*trSecLab);
	plMdc[s].setPlanePar(trPlane);
    }

    targetMidPoint = fSizesCells->getTargetMiddlePoint();
}

void HMdcLayer::fillLayerPoints(HMdcSizesCells* fSizesCells, Bool_t secsys)
{
    // set layers in layer coord system (using HMdcSizesCells)
    // calulates the croped layer realtive to the full layer
    // uses xcrop and ycrop values (default values are set in constructor).
    // if secsys = kTRUE the layer points are transformed to
    // sector system (default kFALSE).


    Double_t d = 0.1; //savety
    for(Int_t s=0;s<6;s++) {
	for(Int_t m=0;m<4;m++) {

	    if(!fSizesCells->modStatus(s,m)) continue;
	    HMdcSizesCellsMod& mod=(*fSizesCells)[s][m];
	    for(Int_t l=0;l<6;l++) {
		HMdcSizesCellsLayer& lay = (mod)[l];

		HGeomVolume* v = lay.getGeomVolume();
		for(Int_t nPoint=0; nPoint<4;nPoint++) {
		    HGeomVector& p = *v->getPoint(nPoint);

		    if     (nPoint==0) layerP [s][m][l][nPoint].Set(p.getX() + d, p.getY() - d);   // lower left
		    else if(nPoint==1) layerP [s][m][l][nPoint].Set(p.getX() + d, p.getY() + d);   // upper left
		    else if(nPoint==2) layerP [s][m][l][nPoint].Set(p.getX() - d, p.getY() + d);   // upper right
		    else if(nPoint==3) layerP [s][m][l][nPoint].Set(p.getX() - d, p.getY() - d);   // lower right
		}

                calcCropedLayer(s,m,l) ;

		if(secsys){
		    for(Int_t nPoint=0; nPoint<4;nPoint++) {
			Double_t x,y,z;
                        x=layerP [s][m][l][nPoint].X();
                        y=layerP [s][m][l][nPoint].Y();
			z=0;
			lay.transFrom(x,y,z);
			layerP [s][m][l][nPoint].Set(x,y);
                        x=layerPcrop [s][m][l][nPoint].X();
                        y=layerPcrop [s][m][l][nPoint].Y();
			z=0;
			lay.transFrom(x,y,z);
			layerPcrop [s][m][l][nPoint].Set(x,y);

		    }
		}
	    }
	}
    }

}


void HMdcLayer::calcCropedLayer (Int_t s,Int_t m,Int_t l)
{

    // calulates the croped layer realtive to the full layer
    // in layer coordinate system. uses xcrop and ycrop values.
    // the layer points and xcrop and ycrop have to be filled before.

    Double_t xmin,xmax,ymin,ymax;
    TVector2 ptmp;

    ptmp.Set(0,layerP[s][m][l][0].Y()+ycrop[m][0]);
    getLayersEdge(ptmp,s,m,l,xmin,xmax,ymin,ymax);
    layerPcrop[s][m][l][0].Set(xmax-xcrop[m][0],layerP[s][m][l][0].Y()+ycrop[m][0]);

    ptmp.Set(0,layerP[s][m][l][1].Y()-ycrop[m][1]);
    getLayersEdge(ptmp,s,m,l,xmin,xmax,ymin,ymax);
    layerPcrop[s][m][l][1].Set(xmax-xcrop[m][1],layerP[s][m][l][1].Y()-ycrop[m][1]);

    ptmp.Set(0,layerP[s][m][l][2].Y()-ycrop[m][1]);
    getLayersEdge(ptmp,s,m,l,xmin,xmax,ymin,ymax);
    layerPcrop[s][m][l][2].Set(xmin+xcrop[m][1],layerP[s][m][l][2].Y()-ycrop[m][1]);

    ptmp.Set(0,layerP[s][m][l][3].Y()+ycrop[m][0]);
    getLayersEdge(ptmp,s,m,l,xmin,xmax,ymin,ymax);
    layerPcrop[s][m][l][3].Set(xmin+xcrop[m][0],layerP[s][m][l][3].Y()+ycrop[m][0]);


}

void  HMdcLayer::printLayer (Int_t s,Int_t m,Int_t l)
{

    cout<<"s "<<s<<" m "<<m<<" l "<<l
	<<" p0("<<setw(10)<<layerP[s][m][l][0].X()<<","<<setw(10)<<layerP[s][m][l][0].Y()<<"),"
	<<" p1("<<setw(10)<<layerP[s][m][l][1].X()<<","<<setw(10)<<layerP[s][m][l][1].Y()<<"),"
	<<" p2("<<setw(10)<<layerP[s][m][l][2].X()<<","<<setw(10)<<layerP[s][m][l][2].Y()<<"),"
	<<" p3("<<setw(10)<<layerP[s][m][l][3].X()<<","<<setw(10)<<layerP[s][m][l][3].Y()<<")"<<endl;

}
void  HMdcLayer::printCropedLayer (Int_t s,Int_t m,Int_t l)
{

    cout<<"s "<<s<<" m "<<m<<" l "<<l
	<<" p0("<<setw(10)<<layerPcrop[s][m][l][0].X()<<","<<setw(10)<<layerPcrop[s][m][l][0].Y()<<"),"
	<<" p1("<<setw(10)<<layerPcrop[s][m][l][1].X()<<","<<setw(10)<<layerPcrop[s][m][l][1].Y()<<"),"
	<<" p2("<<setw(10)<<layerPcrop[s][m][l][2].X()<<","<<setw(10)<<layerPcrop[s][m][l][2].Y()<<"),"
	<<" p3("<<setw(10)<<layerPcrop[s][m][l][3].X()<<","<<setw(10)<<layerPcrop[s][m][l][3].Y()<<")"<<endl;

}
