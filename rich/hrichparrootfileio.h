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
//  HRichParRootFileIo
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHPARROOTFILEIO_H
#define HRICHPARROOTFILEIO_H

#include "hdetparrootfileio.h"

class HRichParRootFileIo : public HDetParRootFileIo {

public:

   HRichParRootFileIo(HParRootFile* f);
   ~HRichParRootFileIo();
   Bool_t init(HParSet*, Int_t*);

   ClassDef(HRichParRootFileIo, 0) // Class for Rich parameter I/O from ROOT file
};

#endif  // HRICHPARROOTFILEIO_H

