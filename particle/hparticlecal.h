#ifndef __HPARTICLECAL_H__
#define __HPARTICLECAL_H__

#include "TObject.h"

#include "hparticledef.h"


class HParticleCal : public TObject
{
private:

    // Additional variables for calibration
    Int_t       fTOFModule;      // module number of hit
    Int_t       fTOFCell;        // cell number of hit
    Int_t       fMETACell;
    SmallFloat  fTOFLeftAmp;     // left amp of TOF signal
    SmallFloat  fTOFRightAmp;    // left amp of TOF signal
    SmallFloat  fMETALocX;       // x position hit
    SmallFloat  fMETALocY;       // y position hit

public:
    HParticleCal() :
	fTOFModule(-1),fTOFCell(-1),fMETACell(-1),
	fTOFLeftAmp(0),fTOFRightAmp(0),
	fMETALocX(0),fMETALocY(0)
    {
    }
	~HParticleCal() {}

	void     setTOFModule  (Int_t a)                  { fTOFModule = a;             }
	void     setTOFCell    (Int_t a)                  { fTOFCell = a;               }
	void     setMETACell   (Int_t a)                  { fMETACell = a;              }
	void     setTOFLeftAmp (Float_t a)                { fTOFLeftAmp = a;            }
	void     setTOFRightAmp(Float_t a)                { fTOFRightAmp = a;           }
	void     setMETALocX   (Float_t a)                { fMETALocX = a;              }
	void     setMETALocY   (Float_t a)                { fMETALocY = a;              }

	Int_t    getTOFModule()   const                   { return fTOFModule;          }
	Int_t    getTOFCell()     const                   { return fTOFCell;            }
	Int_t    getMETACell()    const                   { return fMETACell;           }
	Float_t  getTOFLeftAmp()  const                   { return fTOFLeftAmp;         }
	Float_t  getTOFRightAmp() const                   { return fTOFRightAmp;        }
	Float_t  getMETALocX()    const                   { return fMETALocX;           }
	Float_t  getMETALocY()    const                   { return fMETALocY;           }


	ClassDef(HParticleCal,1)  // A simple object for calibration purpose
};


#endif // __HPARTICLECAL_H__
