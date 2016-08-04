//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HGeomMdc
//
// Class for geometry of MDC
//
// Creation of wires:
//
// The parameters (media, radia, wire distances, ...) to create wires
// (see class HGeomMdcWirePlanes) are read from a parameter ROOT file.
// For each wire the length and position is then calculated and an unique name
// generated.
// All wire planes (sensitive mother volumes) are implemented as TRD1, all
// wires as TUBE filled with a not-sentive medium. The GEANT/ROOT coordinate
// system of both shapes is indentical, but different from the HADES coordinate
// system.
//
//     HADES coordinate system                    TRD1 coordinate system
//                                                
//            Line 1                              xHades = xTrd1
//          ----------              ^ (y)         yHades = zTrd1
//          \ Layer  /              |             zHades = -yTrd1
// Line 0 -> \ MDC  /  <- Line 2    |       
//            \____/                |             
//            Line 3                |      
//  (x) <--------|-------------------      
//              0,0
//
///////////////////////////////////////////////////////////////////////////////

#include "hgeommdc.h"
#include "hgeommdchit.h"
#include "hgeombuilder.h"
#include "hgeommedia.h"
#include "hgeommedium.h"
#include "hgeomnode.h"
#include "hgeommdcwireplanes.h"
#include "hgeommdcwire.h"

#include "TFile.h"

#include <iostream>
#include <iomanip>

using namespace std;

ClassImp(HGeomMdc)

HGeomMdc::HGeomMdc() {
  // Constructor
  fName="mdc";
  maxSectors=6;
  maxModules=4;
  pHit=new HGeomMdcHit(this);
  wn0=33; wn1=0; wn2=0; wn3=-1; // first name will be X000
  memcpy(wnbuf,"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ",36);
}

const Char_t* HGeomMdc::getModuleName(Int_t m) {
  // Return the module name in plane m
  sprintf(modName,"DR%iM",m+1);
  return modName;
}

const Char_t* HGeomMdc::getEleName(Int_t m) {
  // Return the element name in plane m
  sprintf(eleName,"D%i",m+1);
  return eleName;
}

Bool_t HGeomMdc::createAdditionalGeometry(HGeomBuilder* builder, const TString& paramFile, HGeomMedia* media) {
  // reads wire plane parameters from file and creates the wires
  Bool_t rc = kTRUE;
  if (builder && paramFile.Length() > 0) {
    TFile* file = new TFile(paramFile.Data(),"READ");
    if (file==NULL || file->IsOpen()== kFALSE) {
      Error("createAdditionalGeometry", "Parameter file %s not found", paramFile.Data());
      return kFALSE;
    }
    file->cd();
    HGeomMdcWirePlanes *mdcWirePlanes = (HGeomMdcWirePlanes*)file->Get("HGeomMdcWirePlanes");
    if (mdcWirePlanes) {
      //mdcWirePlanes->printWirePlanes();
      vector<HGeomMdcWirePlane>& pWirePlanes = mdcWirePlanes->getWirePlanes();
      Int_t totNumWires=0;
      for (UInt_t np=0; np<pWirePlanes.size(); np++) {
        HGeomMdcWirePlane& plane = pWirePlanes[np];
        HGeomNode* pMother=getVolume(plane.planeName.Data());
        if (pMother==NULL || pMother->isActive()==kFALSE) continue;
        Double_t planeX[4], planeY[4];
        for (Int_t i=0; i<4; i++) {
          HGeomVector* point = pMother->getPoint(i);
          planeX[i] = point->getX();
          planeY[i] = point->getY();
        }
        Int_t medId[2] = {-1,-1};
        for (Int_t i = 0; i <= plane.planeType && rc; i++) {
          HGeomMedium* medium = media->getMedium(plane.wireMedium[i]);
          if (medium) {
            medId[i] = medium->getMediumIndex();
            if (medId[i] <= 0) medId[i] = builder->createMedium(medium);
            if (medId[i] <= 0) {
              Error("createAdditionalGeometry",
                    "Medium %s not created",plane.wireMedium[i].Data());
              rc = kFALSE;
            }
          } else {
            Error("createAdditionalGeometry",
                  "Medium %s not found in list of media",plane.wireMedium[i].Data());
            rc = kFALSE;
          }
        }
        // --------------------------------------------------------------------
        // create cathode wires
        if (rc && plane.planeType == 0) {

          Float_t wireDist = plane.wireDist;
          Float_t halfDist = wireDist * 0.5;
          Float_t radius = plane.wireRadius[0];
          Double_t posX = 0., posY = 0., posZ = 0.; // position relative to center of plane
          Double_t at = (planeY[1] - planeY[0]) / (planeX[1] - planeX[0]);
	  Double_t bt = planeY[0] - at * planeX[0];
          Double_t halfLengthCenter = (planeY[1] - planeY[0]) * 0.5;  // central wire at x = 0.
          TString wireName;
          generateWireName(wireName);
          Int_t wireNum = 0;
          Int_t copyNum = 1;
          //
          // central part of cathode plane, wires are copies of central wire
          HGeomMdcWire* pWire = NULL;
          Int_t arrIndex = -1;
          do {
            copyNum = wireNum + 1;
            arrIndex = addWireObject(wireNum, wireName, copyNum, 0, radius, halfLengthCenter, posX, posY, posZ);
            if (wireNum == 0) {
              pWire = wireObjects[arrIndex];
            } else {
              wireObjects[arrIndex]->setCopyNode(pWire); // needed to create copy nodes in ROOT
            }
	    wireNum++;
            posX += wireDist;
          } while ((posX+radius) <= planeX[0]);
          Int_t maxCopyNumLeft = copyNum;
          //
          // left part of cathode plane (all with copyNum 1)
          copyNum = 1;
          do {
            generateWireName(wireName);
            // calc half-length and position
            Double_t y = at * (posX + radius) + bt;
            Double_t halfLength = (planeY[1] - y) / 2.;
            posZ = halfLengthCenter - halfLength;
            addWireObject(wireNum, wireName, copyNum, 0, radius, halfLength, posX, posY, posZ);
	    wireNum++;
            posX += wireDist;
          } while ((posX+halfDist) < planeX[1]);
          Int_t maxInd = wireObjects.size();
          //
          // create all left side wires
          Int_t rotInd = 0;  // no rotation needed for cathode wires
          for (Int_t i=0; i<maxInd && rc; i++) {
            pWire = wireObjects[i];
            copyNum = pWire->getCopyNumber();
            if (copyNum == 1) {
              rc = builder->createVolume(pWire,medId[0]);
              if (!rc) {
                Error("createAdditionalGeometry",
		      "Plane %s wire %i not created",plane.planeName.Data(),i);
                break;
              }
            }
            rc = builder->positionNode(pWire,pMother,rotInd);
            if (!rc) {
              Error("createAdditionalGeometry",
		    "Plane %s wire %i not positioned",plane.planeName.Data(),i);
            }
          }
          //
          // create all right side wires starting from the middle (without central wire)
          for (Int_t i=1; i<maxInd && rc; i++) {
            pWire = wireObjects[i];
            copyNum = pWire->getCopyNumber();
            if (copyNum > 1) {
              copyNum += maxCopyNumLeft -1;
            } else {
              copyNum += 1;
            }
            // change a left side wire to a right side wire
            Float_t* wirePos = pWire->getPosition();
            wirePos[0] *= -1.;
            pWire->setWireNumber(wireNum);
            pWire->setCopyNumber(copyNum);
            rc = builder->positionNode(pWire,pMother,rotInd);
	    wireNum++;
          }
          //cout<<"***** Plane: "<<plane.planeName<<"   Wires created: "<<wireNum<<endl;
          totNumWires += wireNum;
          clearWireObjects();
        } // cathode plane
        //
        // --------------------------------------------------------------------
        // create field (even wire number) and sens wires (odd wire number)
        if (rc && plane.planeType == 1) {
          //
          // calculate and create GEANT rotation matrix for wires in the TRD1 coordinate system
          Double_t wireOrient = -plane.wireOrient;  // looking in reverse z-direction
          Double_t sinWireOr = TMath::Sin(wireOrient*TMath::DegToRad());
          Double_t cosWireOr = TMath::Cos(wireOrient*TMath::DegToRad());
          Double_t arr[] = {sinWireOr, 0., cosWireOr, 0., 1., 0., -cosWireOr, 0., sinWireOr};
          HGeomRotation rotMatrix;
          rotMatrix.setMatrix(arr);
          Int_t rotInd = builder->createRotation(&rotMatrix);
          //
          // calculate parameters relative to plane center
          Double_t planeCenter[2];
          planeCenter[0] = (planeX[0] + planeX[1] + planeX[2] + planeX[3]) * .25;
          planeCenter[1] = (planeY[0] + planeY[1] + planeY[2] + planeY[3]) * .25;
          for (Int_t i=0; i<4; i++) {
            planeX[i] -= planeCenter[0];
            planeY[i] -= planeCenter[1];
          }
          Double_t wireOffset = (plane.centralWireNr - 1) * plane.wireDist;
          //
          // reduce plane size for calculation to avoid extrusions of wire edges
          Double_t at[4], bt[4]; // border lines of plane
          Double_t radius = plane.wireRadius[0];
          at[0] = (planeY[1] - planeY[0]) / (planeX[1] - planeX[0]);
          Double_t dy = radius * cosWireOr;
          Double_t dx1 = dy / at[0];
          Double_t dx2 = 0;
          if (wireOrient > 0.) {
            dx2 = radius * (sinWireOr + cosWireOr / at[0]);  // larger extrusion on left side
          } else {
            dx2 = -radius * (sinWireOr - cosWireOr / at[0]); // larger extrusion on right side
          }
          planeX[0] = planeX[0] + dx1 - dx2;
          planeY[0] = planeY[0] + dy;
          planeX[1] = planeX[1] - dx1 - dx2;
          planeY[1] = planeY[1] - dy;
          planeX[2] = -planeX[1];
          planeY[2] = planeY[1];
          planeX[3] = -planeX[0];
          planeY[3] = planeY[0];
          //
          // calculate lines
          for (Int_t i1=0; i1<4; i1++) {
            Int_t i2 = i1 + 1;
            if (i2==4) i2=0;
            at[i1] = (planeY[i2] - planeY[i1]) / (planeX[i2] - planeX[i1]);
            bt[i1] = planeY[i1] - at[i1] * planeX[i1];
            //cout<<"line "<<i1<<"   at: "<<at[i1]<<"  bt: "<<bt[i1]<<endl;
          }
          //
          // loop on wires
	  Double_t a = TMath::Tan(wireOrient*TMath::DegToRad());
          Int_t numWiresPlane=0;
          for(Int_t wireNum=0; wireNum<plane.numWires; wireNum++) {
            Int_t wireType =  wireNum%2;
            Float_t radius = plane.wireRadius[wireType];
            //
            // calculation of wire length
            Double_t yWire = wireNum * plane.wireDist - wireOffset;
            Double_t b     = yWire/cosWireOr - planeCenter[1];
            Double_t x1  = (bt[0]-b)/(a-at[0]);       // Xb    line 0
            Double_t x2  = (bt[2]-b)/(a-at[2]);       // Xe    line 2
            Double_t y1  = a*x1+b;                    // Yb
            Double_t y2  = a*x2+b;                    // Ye
            Int_t nLine1 = (x1>=planeX[0] && x1<=planeX[1] && y1>=planeY[0] && y1<=planeY[1]) ? 0:-1;
            Int_t nLine2 = (x2<=planeX[3] && x2>=planeX[2] && y2>=planeY[3] && y2<=planeY[2]) ? 2:-1;
            if(nLine1<0 && nLine2 <0) {
            //  Warning("fillCont","%s: wire %i is out of layer.",vname.Data() , wireNum);
              continue;
            }
            if(nLine1<0) {
              x1 = (bt[1]-b)/(a-at[1]);                    // Xb    line 1
              if(x1<planeX[2]) x1 = (bt[3]-b)/(a-at[3]);   // Xb    line 3
              y1 = a*x1+b;                                 // Yb
            } else if(nLine2<0) {
              x2 = (bt[1]-b)/(a-at[1]);                    // Xe    line 1
              if(x2>planeX[1]) x2 = (bt[3]-b)/(a-at[3]);   // Xe    line 3
              y2 = a*x2+b;                                 // Ye
            }
            Double_t wireLength = TMath::Sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
            Double_t posX = (x2 + x1) * .5;  // position in TRD1 coordinate system
            Double_t posY = 0.;
            Double_t posZ = (y2 + y1) * .5;
            //cout<<wireNum<<"  "<<yWire<<"  "<<wireLength<<"  "<<x1<<"  "<<y1<<"  "<<x2<<"  "<<y2<<"  "<<posX<<"  "<<posZ<<endl;
            //
            // create and position wire
            TString wireName;
            generateWireName(wireName);
            HGeomMdcWire wire(wireNum, wireName, 1, wireType, radius, wireLength * 0.5, posX, posY, posZ);
            rc = builder->createVolume(&wire, medId[wireType]);
            if (!rc) {
              Error("createAdditionalGeometry",
                    "Plane %s wire %i not created",plane.planeName.Data(),wireNum);
              break;
            }
            rc = builder->positionNode(&wire,pMother,rotInd);
            if (!rc) {
              Error("createAdditionalGeometry",
                    "Plane %s wire %i not positioned",plane.planeName.Data(),wireNum);
            }
	    numWiresPlane++;
          } //loop on wires
          totNumWires += numWiresPlane;
          //cout<<"***** Plane: "<<plane.planeName<<"   Wires created: "<<numWiresPlane<<endl;
        } // senswire plane
        if (rc) builder->fillMdcCommonBlock(pMother);
      } // loop on planes
      cout<<"  Wires created: "<<totNumWires<<endl;
      delete mdcWirePlanes;
      mdcWirePlanes = NULL;
    } else {
      Warning("createAdditionalGeometry", "HGeomMdcWires not found in parameter file %s", paramFile.Data());
    }
    file->Close();
  }
  return rc;
}

void HGeomMdc::generateWireName(TString& vName) {
  // generates the wire name
  if (wn3<35) wn3++;
  else {
    wn3 = 0;
    if (wn2<35) wn2++;
      else {
	wn2 = 0;
	if (wn1<35) wn1++;
        else {
	  wn1 = 0;
	  wn0++;
        }
     }
  }
  vName.Form("%c%c%c%c",wnbuf[wn0],wnbuf[wn1],wnbuf[wn2],wnbuf[wn3]);
}

void HGeomMdc::clearWireObjects(){
  // deletes objects in working array and sets pointer to 0.
  // cleares the vector.
  for(UInt_t i = 0; i < wireObjects.size(); i ++) {
    if (wireObjects[i]) {
      delete wireObjects[i];
      wireObjects[i] = 0;
    }
  }
  wireObjects.clear();
}

Int_t HGeomMdc::addWireObject(Int_t wn, TString& wname, Int_t cn, Int_t wtype, Float_t radius,
                              Double_t hlen, Double_t xpos, Double_t ypos,  Double_t zpos) {
  // creates a wire objects and adds it in the working array
  // returns the array index
  HGeomMdcWire* wire = new HGeomMdcWire(wn, wname, cn, wtype, radius, hlen, xpos, ypos, zpos);
  wireObjects.push_back(wire);
  return (wireObjects.size()-1);
}
