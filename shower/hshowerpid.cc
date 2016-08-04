//###################################################
//#
//#       
//#                      FRAMEWORK
//#
//#       Authors:                           W.Koenig
//#       adoption to framework & ROOT       W.Schoen
//#    
//#       last mod. W.Schoen 19.1.98
// ###################################################

#pragma implementation
#include "hshowerpid.h"
ClassImp(HShowerPID)

////////// **********ShowerPID defintion ****************/////////////77

Int_t HShowerPID::clear()
{
   m_fCharge=0.0F;
   m_fX = 0.0;
   m_fY = 0.0;

   m_fShower = 0.0; //shower - diff m_fSum2 - m_fSum1 if > 0

   m_nSector = -1;
   m_nModule = -1;
   m_nRow = -1;
   m_nCol = -1;
   m_nAddress = -1;
 
   return 1;
}

Int_t HShowerPID::calcAddress() {
  Char_t s = (m_nSector) ? m_nSector : 6;
  m_nAddress = 100000 * s;
  m_nAddress += 10000 * m_nModule;
  m_nAddress += 100 * m_nRow;
  m_nAddress += m_nCol;

  return m_nAddress;
}

