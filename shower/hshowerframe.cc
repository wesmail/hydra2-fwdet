#pragma implementation
using namespace std;
#include <stdlib.h>
#include <iostream> 
#include <iomanip>
#include "hshowerframe.h"

ClassImp(HShowerFrameCorner)
ClassImp(HShowerFrame)

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HShowerFrameCorner
// Class describe coordinates one corner in frame
//
// HShowerFrame
// Description of local coordinates of frame  
//
/////////////////////////////////////////////////////////////

HShowerFrameCorner::HShowerFrameCorner() {
  fXcor = 0.0f;
  fYcor = 0.0f;
  nFlagArea = 0;
}

/////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////

HShowerFrame::HShowerFrame() {
  m_nModuleID = -1;
  reset();
} // eof constructor


HShowerFrame::~HShowerFrame() {
  reset();
} // eof destructor

void HShowerFrame::reset()
{
//clearing data
  nCorners = 0;
  m_FrameArr.Delete();
}

HShowerFrameCorner* HShowerFrame::getCorner(Int_t n)
{
// retrieve corner information
   if ((n < 0) || (n >= nCorners))
      return NULL;

   return (HShowerFrameCorner*)m_FrameArr.At(n);
}


Int_t HShowerFrame::setCorner(HShowerFrameCorner* pCorner, Int_t n)
{
//set corner information at n position
   if ((n < 0) || (n >= nCorners))
      return 0;

   delete m_FrameArr.At(n);
   m_FrameArr.AddAt(pCorner, n);

   return 1;
}

Int_t HShowerFrame::addCorner(HShowerFrameCorner* pCorner)
{
// add new corner into table
  m_FrameArr.Add(pCorner);
  return nCorners++;
}

Int_t HShowerFrame::isOut(Float_t x, Float_t y) {
// returns 1 if point x,y is outside frame, otherwise 0
  Float_t a,b;
  if (nCorners<=2) {
    Error("isOut(Float_t,Float_t)","Frame must have at least 3 corners!");
    exit(1);
  }

  Float_t X0, X1, Y0, Y1;
  Int_t nFlag;

  for (Int_t i=0; i < nCorners - 1; i++) {
    X0 = getCorner(i)->getX();
    X1 = getCorner(i + 1)->getX();
    Y0 = getCorner(i)->getY();
    Y1 = getCorner(i + 1)->getY();
    nFlag = getCorner(i)->getFlagArea(); 
 
    if (X0 == X1) {
      if (nFlag == 0 && x > X0) return 1;
      else if (nFlag == 1 && x < X0) return 1;
    } else if (Y0 == Y1) {
      b = (X1 * Y0 - X0 * Y1)/(X1 - X0);
      if (nFlag == 0 && y > Y0) return 1;
      else if (nFlag == 1 && y < Y0) return 1;
    } else {
      a = (Y1 - Y0)/(X1 - X0);
      b = (X1*Y0 - X0*Y1)/(X1 - X0);
      if (a<0 && 
          ((nFlag == 0 && y>(a*x+b)) || 
           (nFlag == 1 && y<(a*x+b)))) return 1;
      else if (a>0 &&
               ((nFlag==0 && y<(a*x+b)) ||
                (nFlag==1 && y>(a*x+b)))) return 1;
    }
  }

  return 0;
}

void HShowerFrame::printParams() {
  cout<<"Module "<<m_nModuleID<<" Wire frame corners:"<<endl; 
  for (Int_t i=0; i < nCorners; i++) {
    cout<<"  "<<i<<setw(10)<<right<<fixed<<setprecision(3)<<getCorner(i)->getX()
        <<setw(10)<<right<<fixed<<setprecision(3)<<getCorner(i)->getY()
        <<setw(3)<<right<<getCorner(i)->getFlagArea()<<endl;
  }
  cout<<endl;
}
