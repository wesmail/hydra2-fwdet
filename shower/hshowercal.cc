#pragma implementation
#include "hshowercal.h"


ClassImp(HShowerCal)

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
//HShowerCal
//
//HShowerCal contains calibrated data of the pre-SHOWER
//In the case of simulaton data, HShowerCal contains these data
//from HShowerRawMatr which charge is greater as 1 
//
//////////////////////////////////////////////////////////////////////


Int_t HShowerCal::clear()
{
//clearing parameters
   m_fCharge=0.0F;
   m_nSector = -1;
   m_nModule = -1;
   m_nRow = -1;
   m_nCol = -1;
//   m_nAddress = -1;

   resetLocalMax(); 
   return 1;
}

Int_t HShowerCal::calcAddress() {
//calculation of address of the pad to SMRRCC format
//S - sector; 6 for sector==0
//M - module 
//RR - row
//CC - col

  Int_t nAddress;
  Char_t s = (m_nSector) ? m_nSector : 6;
  nAddress = 100000 * s;
  nAddress += 10000 * m_nModule;
  nAddress += 100 * m_nRow;
  nAddress += m_nCol;

  return nAddress;
}

Int_t HShowerCal::getNLocationIndex(void) {
  return 4;
}

Int_t HShowerCal::getLocationIndex(Int_t i) {
  switch (i) {
  case 0 : return getSector(); break;
  case 1 : return getModule(); break;
  case 2 : return getRow(); break;
  case 3 : return getCol(); break;
  }
  return -1;
}

