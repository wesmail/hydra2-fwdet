//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichParAsciiFileIo
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHPARASCIIFILEIO_H
#define HRICHPARASCIIFILEIO_H

#include "hdetparasciifileio.h"

class HRichParAsciiFileIo : public HDetParAsciiFileIo {

public:

   HRichParAsciiFileIo(fstream* f);
   ~HRichParAsciiFileIo() {}

   // calls special read-function for each container type
   Bool_t init(HParSet*, Int_t*);

   // calls special update-function for each container type
   Int_t write(HParSet*);


   ClassDef(HRichParAsciiFileIo, 0) // Class for Rich parameter I/O from ASCII file
};

//============================================================================


#endif  // HRICHPARASCIIFILEIO_H 
