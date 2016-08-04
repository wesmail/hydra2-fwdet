#ifndef HShowerCalParCell_H
#define HShowerCalParCell_H
#pragma interface

#include "TObject.h"

class HShowerCalParCell : public TObject
{
public:
    HShowerCalParCell();
    HShowerCalParCell(Float_t fSlope, Float_t fOffset);
    HShowerCalParCell(Float_t fSlope, Float_t fOffset,
                                        Float_t fThreshold, Float_t fGain);
   ~HShowerCalParCell() {}

    Int_t   reset();

    void    setOffset(Float_t fOffset)      { m_fOffset = fOffset;  }
    Float_t getOffset()                     { return m_fOffset;     }

    void    setSlope(Float_t fSlope)        { m_fSlope = fSlope;    }
    Float_t getSlope()                      { return m_fSlope;      }

    void    setThreshold(Float_t fThreshold){ m_fThreshold  = fThreshold;}
    Float_t getThreshold()                  { return m_fThreshold;  }

    void    setGain(Float_t fGain)          { m_fGain = fGain;      }
    Float_t getGain()                       { return m_fGain;       }

    void    setParams(Float_t fSlope, Float_t fOffset);
    void    setParams(Float_t fSlope, Float_t fOffset,
                        Float_t fThreshold, Float_t fGain);

    void    print(void);

    // -------------------------------------------------------------------------

private:
    Float_t m_fOffset;      // offset parameter for calibration of the pad
    Float_t m_fSlope;       // slope parameter for  calibration of the pad
    Float_t m_fThreshold;   // threshold for the electronics
    Float_t m_fGain;

    // -------------------------------------------------------------------------

    ClassDef(HShowerCalParCell, 3) //ROOT extension
};

#endif
