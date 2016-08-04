// File: hldbase.cc
//
//*-- Author : Walter Karig <W.Karig@gsi.de>
//*-- Modified: 97/12/05 18:05:48 by Walter Karig
//*-- Modified: 25/03/98 by Manuel Sanchez Garcia
//

#include "hldbase.h"


void HldBase::swap2(UShort_t* ptr, const size_t len) const {
    for (size_t i = 0; i < len; ++i) {
        UShort_t tmp = ptr[i];
        UChar_t* t = (UChar_t*) &tmp;
        UChar_t* p = (UChar_t*) &ptr[i];
        p[0] = t[1];
        p[1] = t[0];
    }
}

void HldBase::swap4(UInt4* p, const size_t len) const {
  UInt4 tmp;
  UInt1* d;
  UInt1* const s = (UInt1*) &tmp;
  for (size_t i = 0; i < len; i++) {
    d = (UInt1*) (p + i);
    tmp = p[i];
    d[0] = s[3];
    d[1] = s[2];
    d[2] = s[1];
    d[3] = s[0];
  }
}      


