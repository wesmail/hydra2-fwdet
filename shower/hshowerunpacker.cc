// File: showerunpacker.cc
//
// Author: Leszek Kidon
// Last update: 25/5/2000
// Last update: 6/7/2010
//
using namespace std;
#include "hades.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hcategory.h"
#include "hldsubevt.h"
#include "hdebug.h"
#include "heventheader.h"
#include "hshowerunpacker.h"
#include "hshowerrawhist.h"
#include "hshowerraw.h"

#include <iostream>
#include <iomanip>
#include <stdio.h>

HShowerUnpacker::HShowerUnpacker(Int_t nSubId) {
  m_nSubId = nSubId;
  pRawCat = NULL;
  fEventNr = 0;
  m_loc.set(4,0,0,0,0);
  m_zeroLoc.set(0);
}

HShowerUnpacker::~HShowerUnpacker(void) {
}

Bool_t HShowerUnpacker::init(void) {
  pRawCat=gHades->getCurrentEvent()->getCategory(catShowerRaw);
  if (!pRawCat) {         
    pRawCat=gHades->getSetup()->getDetector("Shower")
                             ->buildCategory(catShowerRaw);
    if (!pRawCat) {   
      return kFALSE;  
    } else {
      gHades->getCurrentEvent()->addCategory(catShowerRaw,pRawCat,"Shower");
    }         
  }

  return kTRUE;
}

Bool_t HShowerUnpacker::reinit(void) {
  return kTRUE;
}

Int_t HShowerUnpacker::fillData(class HLocation &loc, Int_t nCharge, Int_t nEvtId) {
  static long i = 0;
  HShowerRaw *pCell = NULL;
  // check the location of the data, should not be out of the detector !!!
  Long_t nEvt=gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
  if(loc[0]<0 || loc[0]>5 || loc[1]<0 || loc[1]>2 || loc[2]<0 || loc[2]>31 || loc[3]<0 || loc[3]>31){
      Error("fillData"," EventSeqNumber: %ld  ShowerUnpack: data out of range: sec: %d  mod: %d  row: %d  col: %d  data: %d  error nr: %ld\n",
	    nEvt, loc[0], loc[1], loc[2], loc[3], nCharge, i++);
    return 0;
  }

//cout << "ShowerUnpacker fill data ******************* " << endl;

  pCell = (HShowerRaw*) pRawCat->getNewSlot(m_zeroLoc);
  if (pCell != NULL) {
      pCell = new(pCell) HShowerRaw;

      pCell->setCharge(nCharge);
      pCell->setSector(loc[0]);
      pCell->setModule(loc[1]);
      pCell->setRow(loc[2]);
      pCell->setCol(loc[3]);
      pCell->setEventType(nEvtId&0xFF);
      pCell->setEventNr(nEvt);

  }
  else
  {
      Error("HShowerUnpacker::execute","Not enough memory");
      return 0;
  }
  return 1;
}

Int_t HShowerUnpacker::execute() {
  SHeader dHeader;
  Int_t nEvtId;
  nEvtId = gHades->getCurrentEvent()->getHeader()->getId();
  fEventNr = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

//cout << "ShowerUnpacker execute ******************* "<< fEventNr << " ( " << nEvtId << " ) " << endl;

  if (pSubEvt) {

//cout << "ShowerUnpacker execute pSubEvt "<< pSubEvt << endl;
    UInt_t dataLen = pSubEvt->getDataLen();
    UInt_t sumHeaderSize = 0;
    //fprintf(stderr, "datalen: %d\n", datalen);
    for (UInt_t i = 0; i < dataLen; i++) {

//cout << "ShowerUnpacker execute pSubEvt "<< pSubEvt->getDataLen() << endl;

       // decode subsub_event and its header
       // **************** DUMP ************************
       //dHeader.dump();
       // **************** DUMP ************************

       dHeader.getEvent(pSubEvt->getData()[i]);
       //fprintf(stderr, " %d:header:%x\n", i, pSubEvt->getData()[i]);
       if (0 == dHeader.getSize()) {
          // no data im subsubevent
          continue;
       }

       //cout << " ------------------- SUBEVt HEADER: SIZE " << hex << dHeader.getSize() << " SOURCE: " << hex << dHeader.getSource() << endl;

       // After decoding and checking the header jump to the next data word
       i++;
       fDataWord.sector = 0;

       fDataWord.sector = ( dHeader.getSource() & 0x00F0 ) >> 4;

         // Read the sub-sub event
	 UInt_t headerSize = dHeader.getSize();
	 sumHeaderSize += headerSize;
	 UInt_t sumAdcSize = 0;
         for (UInt_t j = 0; j < headerSize && i < dataLen; j++, i++) {
            // Check for debug word (skip)

            // check the source (shower - 32) and the endpoint number (1 or 2) - if not the case - skip
            if ( !(  ((dHeader.getSource() & 0xFF0F) == 0x3201) || ((dHeader.getSource() & 0xFF0F) == 0x3202) ||  ((dHeader.getSource() & 0xFFFF) == 0x5555) )  ) continue;

      //      if ( (i%4) == 0 ) cout << endl; 
      //    printf("0x%04x:0x%04x\t", pSubEvt->getData() + i, pSubEvt->getData()[i]);

            if ((dHeader.getSource() & 0xFFFF) != 0x5555)
	    {
	      if ((pSubEvt->getData()[i] & 0xFFFF) != 0x5555)
	      {
	               m_loc[0]=m_loc[1]=m_loc[2]=m_loc[3]=0;
		       fDataWord.module  = 0;
		       fDataWord.row = 0;
		       fDataWord.column     = 0;
		       fDataWord.charge     = 0;




            // Fill the data structure
                       fDataWord.module  = ((pSubEvt->getData()[i] & 0x30000000)  >> 28);   //0xF0000000 -> 0x30000000 - module masking
                       fDataWord.row = ((pSubEvt->getData()[i] & 0x01F00000) >> 20);        //0x0FF00000 -> 0x01F00000 - row masking
                       fDataWord.column     = ((pSubEvt->getData()[i] & 0x0001F000) >> 12); //0x000FF000 -> 0x0001F000 - col masking
                       fDataWord.charge     = (pSubEvt->getData()[i] & 0x000003FF);         //0x00000FFF -> 0x000003FF - charge masking


                        if ((pSubEvt->getData()[i] & 0xFFF) == 0xBAD) 
			{
			    /*fprintf(stderr, " %x:data:%x", k, pSubEvt->getData()[i]);
			    k = 0;*/
			    UInt_t adcSize = ((pSubEvt->getData()[i] & 0x7FF000) >> 12);
			    sumAdcSize += adcSize;

			}
			else
			{
                            m_loc[0]=fDataWord.sector;
                            m_loc[1]=fDataWord.module;
              	            m_loc[2]=fDataWord.row;
              	            m_loc[3]=fDataWord.column;
               	            fillData(m_loc, fDataWord.charge, nEvtId);
			}

	       }
	       else
	       {
//			i = i + ((pSubEvt->getData()[i] & 0xFFFF0000) >> 16);
	       }
	    }

         }
	 if (headerSize != sumAdcSize && sumAdcSize)
	 {
	     Error("execute", " EventSeqNumber: %d  Size from subevent: header = %d; size from adc = %d\n", fEventNr, headerSize, sumAdcSize);
	 }
	 // end of unpacking subsub event
	 //cout << endl;
         // rewind by one data word
         i--;

    } // end of unpacking sub event
    /*if (dataLen - sumHeaderSize != 11)
    {
        fprintf(stderr, "Data length = %d; Size from subevent headers = %d\n", dataLen, sumHeaderSize);
    }*/
  }
// printf("ii unpack = %d\n", ii);
//  dump();

  return 1;
}


Bool_t HShowerUnpacker::finalize() {
  return kTRUE;
}

Int_t HShowerUnpacker::dump() {
  if (pSubEvt) {
    UInt_t i = 0;
    cout << endl;
    while (i < pSubEvt->getDataLen()) {
       cout << "Dump must be implemented!" << endl;
       i++;
    }
    cout << endl;
  }

  return 1;
}





ClassImp(HShowerUnpacker)
