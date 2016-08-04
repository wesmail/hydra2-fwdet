//*-- AUTHOR : Ilse Koenig
//*-- Created : 24/11/2014 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HGeomMdcWirePlanes
//
// Class for all MDC wire planes in Geant/Root
//
///////////////////////////////////////////////////////////////////////////////

#include "hgeommdcwireplanes.h"

#include <iostream>
#include <iomanip>

using namespace std;

ClassImp(HGeomMdcWirePlane)
ClassImp(HGeomMdcWirePlanes)

HGeomMdcWirePlane::HGeomMdcWirePlane(TString n, Int_t t) {
  planeName = n;
  planeType = t;
  clear();
}

void HGeomMdcWirePlane::clear() {
  numWires      = -1;
  centralWireNr = wireDist = wireOrient = wireRadius[0]= wireRadius[1] = 0.f;
  wireMedium[0] = wireMedium[1] = -1;
  //  rotMatrix.setUnitMatrix();
}

void HGeomMdcWirePlane::print() {
  cout<<"------------------------------------------------------------------"<<endl;
  cout<<"plane name :      " << planeName.Data() <<endl;
  cout<<"plane type :      " << planeType <<endl;
  cout<<"wireDist :        " << wireDist << endl;
  cout<<"wireOrient :      " << wireOrient << endl;
  if (planeType == 0) {
    cout<<"cathWireRadius :  " << wireRadius[0] << endl;
    cout<<"cathWireMedium :  " << wireMedium[0] << endl;
  } else {
    cout<<"numWires :        " << numWires <<endl;
    cout<<"centralWireNr :   " << centralWireNr <<endl;
    cout<<"fieldWireRadius : " << wireRadius[0] << endl;
    cout<<"fieldWireMedium : " << wireMedium[0] << endl;
    cout<<"sensWireRadius :  " << wireRadius[1] << endl;
    cout<<"sensWireMedium :  " << wireMedium[1] << endl;
  }
  //  cout<<"rotMatrix : ";
  //  rotMatrix.print();
}

//------------------------------------------------------------------------------------

HGeomMdcWirePlane& HGeomMdcWirePlanes::addWirePlane(TString planeName, Int_t planeType) {
  HGeomMdcWirePlane plane(planeName,planeType);
  fplanes.push_back(plane);
  return fplanes.at(fplanes.size()-1);
}

void HGeomMdcWirePlanes::printWirePlanes() {
  for(UInt_t i = 0; i < fplanes.size(); i++) {
    fplanes[i].print();
  }
}
