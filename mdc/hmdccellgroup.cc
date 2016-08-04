//*-- Author : M. Sanchez
#include "hmdccellgroup.h"

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////
// HMdcCellGroup, HMdcCellGroup6, HMdcCellGroup12
//
// 	This class and it's derived classes provide a mechanism to
// know with cells contributed to a particular hit or segment and
// how. They store therefore, a list of cells in different layers allowing 
//also to select one of the two possible signals in a given cell.
//
//       HMdcCellGroup is never instantiated directly, but HMdcCellGroup6 or
//HMdcCellGroup12 are used, where the number in the class name indicates
//how many layers are available at maximum. 
//
//The info provided by this class is retrieved with the
//following functions	
//
// * Int_t getNCells(Int_t layer)
//	Returns the number of cells in layer "layer" which contributed
//	to the hit.
//
// * Int_t getCell(Int_t layer,Int_t idx)
//      For idx=0 returns the cell number of the first fired cell
//      For idx=1 returns the cell number of the second fired cell
//	.... (maximum idx=3) 
//
// * Int_t HMdcHit::getSignId(Int_t layer,Int_t idx)
//	Returns signal id (time/distance) in cell designed by
//	idx (same criterium as before).
//
// * Int_t setSignId(Int_t layer,Int_t cell, Int_t t1,t2=0,t3=0,t4=0)
//	Sets the signal ids and base cell number for layer "layer"
//	Input:
//	  layer --> Number of layer to set
//	  cell  --> Number of the first cell fired in this layer
//	  t1 - t4 --> SignalId for cells: "cell"+0, "cell"+1 ... "cell"+4
//	              Possible values are:
//	                0 --> This cell has not fired
//	                1 --> First signal (time/distance) was used
//	                2 --> Second signal (time/distance) was used   
////////////////////////////////////////////////////////////  


UChar_t HMdcCellGroup::nCellsLookUp[256];
UChar_t HMdcCellGroup::cellPosLookUp[4][256];
Bool_t HMdcCellGroup::lookUpsInitialized=kFALSE;

Int_t HMdcCellGroup::getLayerListCells(Int_t layer,Int_t *list) {
  // return number of cells in the layer and store list of cells in array list[4]
  UShort_t lcells = pLayer[layer] & 0xFF;
  Int_t    nCells = nCellsLookUp[lcells];
  if(nCells > 0) {
    UShort_t firstCell = (pLayer[layer]>>8) & 0xFF;
    for(Int_t i=0;i<nCells;i++) list[i] = firstCell + cellPosLookUp[i][lcells];
  }
  return nCells;
}

void HMdcCellGroup::printCells(Int_t nlays) {
  for(Int_t layer=0; layer<nlays; layer++) {
    Int_t nc = getNCells(layer);
    if(nc>0) {
      printf("%2i)",layer+1);
      for(Int_t ic=0;ic<nc;ic++) printf(" %i",getCell(layer,ic)+1);
      printf("\n");
    }
  }
}

void HMdcCellGroup::initLookups(void) {
  //Initializes lookup tables
  Int_t j=0,shift;
  for (Int_t i=0;i<256;i++) {
    nCellsLookUp[i]=((i & 0x3)?1:0) + (((i>>2) & 0x3)?1:0) + (((i>>4) & 0x3)?1:0) + (((i>>6) & 0x3)?1:0);           
    shift=0; j=0;
    while (shift<4) {
      Int_t k=i>>(2*shift);
      if (k & 0x3)
	{ cellPosLookUp[j][i]=shift+0; shift++; } 
      else if (k & 0xC)
	{ cellPosLookUp[j][i]=shift+1; shift+=2;}
      else if (k & 0x30)
	{ cellPosLookUp[j][i]=shift+2; shift+=3; }
      else if (k & 0xC0)
	{ cellPosLookUp[j][i]=shift+3; shift+=4; }
      else
	{ cellPosLookUp[j][i]=0; shift+=4; }
      j++;
    }
  }
  lookUpsInitialized=kTRUE;     
}

ClassImp(HMdcCellGroup)
ClassImp(HMdcCellGroup6)
ClassImp(HMdcCellGroup12)
  
