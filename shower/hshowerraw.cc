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
#include "hshowerraw.h"
ClassImp(HShowerRaw)
ClassImp(HShowerRawMatr)

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
//
//HShowerRaw contains unpacked raw data of the pre-SHOWER
//HShowerRawMatr is the same as HShowerRaw; It is used
//to store data after digitalisation.
//HShowerRawMatr could contain additional data for simulations
//
//////////////////////////////////////////////////////////////////////

Int_t HShowerRaw::clear()
{
   m_fCharge = 0.0F;

   m_nSector = -1;
   m_nModule = -1;
   m_nRow = -1;
   m_nCol = -1;
//   m_nAddress = -1;
 
   return 1;
}

Float_t HShowerRaw::addCharge(Float_t q) {
//add charge q to cumulated charge
//it's used in during digitalistion of charge of the pad
  m_fCharge += q;
//  if (m_fCharge > 255.0)	// MJ Mon Jul 30 16:37:35 CEST 2001
//      m_fCharge = 255.0;

  return m_fCharge;
}

Int_t HShowerRaw::calcAddress() {
//calculation of address
  Int_t nAddress;
  Char_t s = (m_nSector) ? m_nSector : 6;
  nAddress = 100000 * s;
  nAddress += 10000 * m_nModule;
  nAddress += 100 * m_nRow;
  nAddress += m_nCol;

  return nAddress;
}


Int_t HShowerRaw::getNLocationIndex(void) {
  return 4;
}

Int_t HShowerRaw::getLocationIndex(Int_t i) {
  switch (i) {
  case 0 : return getSector(); break;
  case 1 : return getModule(); break;
  case 2 : return getRow(); break;
  case 3 : return getCol(); break;
  }
  return -1;
}

void HShowerRaw::getLocation(HLocation& loc) {
  loc.setNIndex(getNLocationIndex());
  for(Int_t i = 0; i < getNLocationIndex(); i++)
    loc.setIndex(i, getLocationIndex(i));  
}

