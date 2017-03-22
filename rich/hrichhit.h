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
//  HRichHit
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHHIT_H
#define HRICHHIT_H

#include "TArrayI.h"
#include "TObject.h"

class HRichHit: public TObject {

public:
   //--------------------------------------
   // RICH700 vars
   Double_t fRich700CircleCenterX;
   Double_t fRich700CircleCenterY;
   Double_t fRich700CircleRadius;
   Double_t fRich700CircleChi2;
   Int_t    fRich700NofRichCals;
   //--------------------------------------


   Int_t iRingX;                // x coordinate of a ring in pad units , RICH700 : PMT index x
   Int_t iRingY;                // y coordinate of a ring in pad units , RICH700 : PMT index y
   Int_t iRingQuality;          //! this is iRingPatMat or iRingHouTra
   Int_t iRingPatMat;           // maximum amplitude of Pattern Matrix (PM)
   Int_t iRingHouTra;           // maximum amplitude of Hough Transform (HT)
   Int_t iRingPadNr;            // number of fired pads in a ring
   Int_t iRingAmplitude;        // integrated charge (charge of all pads) in a ring
   Int_t iRingLocalMax4;        // number of 4-connected local maxima in a ring
   Int_t iRingLocalMax8;        // number of 8-connected local maxima in a ring
   Int_t iRingClusterNr;        // number of clusters in a ring
   Float_t fRingClusterSize;    // mean custer size in a ring
   Float_t fRingClusterLMax4;   // mean cluster size with 4-connected local maximum
   Float_t fRingClusterLMax8;   // mean cluster size with 8-connected local maximum
   Int_t iRingAlgorithmIndex;   // index describing algorithm, odd - PM, even - HT

   static Int_t iRingImageSize; //! this must be == iRingMaskSize
   Int_t iRingImage[256];       //! used internally to calculate clusters
   Int_t iRingFreeParam;        //! pad label number
   Int_t iRingMaxLabel;         //! local maximum label number

   Int_t fMaxClusterSize;    // local maximum cluster size
   Int_t fMaxClusterSum;     // integrated amplitude of a local maximum cluster
   Int_t fMaxThrClusterSize; // local maximum cluster size above threshold


   Float_t fLabX; // x ring position in LAB
   Float_t fLabY; // y ring position in LAB
   Float_t fLabZ; // z ring position in LAB


   Float_t fX;         // x coordinate of a ring in cm (weighted mean of local max cluster) on padplane
   Float_t fY;         // y coordinate of a ring in cm (weighted mean of local max cluster) on padplane
   Float_t fPadX;      // x coordinate of a ring in pad units (with pad fraction, mean)
   Float_t fPadY;      // y coordinate of a ring in pad units (with pad fraction, mean)
   Float_t fMeanTheta; // theta (weighted mean of local max cluster)
   Float_t fMeanPhi;   // phi (weighted mean of local max cluster)
   Float_t fTheta;     // theta angle of presumable lepton emitted from a target (in deg)
   Float_t fPhi;       // phi angle of presumable lepton emitted from a target (in deg)

   Int_t nSector;      // sector number, 0-5, 0 is the uppermost
   Int_t lEventNr;     // event number
   Int_t fAddress;     // address of a pad - centre of a ring


   Bool_t fTestDens;   // density test
   Bool_t fTestBord;   // border amplitude decrease test
   Bool_t fTestDyna;   // dynamical amplitude test
   Bool_t fTestRati;   // ratio of fired pads in and outside a ring test
   Bool_t fTestAsym;   // asymmetry test, only for rings inside the frame
   Bool_t fTestCharge; // ring charge test.
   Bool_t fRejFake;    // test of the ring quality of those candidates that are near a candidate with very high quality. Those candidates could be fakes, the flag is 0 if the ring is rejected.

   Int_t fTests;           // fTestDens+10*fTestBord+100*fTestDyna+1000*fTestRati+10000*fTestAsym + fTestCharge*100000 + fRejFake*1000000;
   Float_t fRingCentroid;  // ring centroid calculated during assymetry test
   Float_t fRingRadius;    // ring radius (harmonical mean)
   Float_t fBorderFactor;  // 1 if ring inside, less than 1 if partially outside

public:

   HRichHit();
   HRichHit(Int_t x, Int_t y, Int_t q, Int_t fp, Int_t lab);
   virtual ~HRichHit();

   HRichHit(const HRichHit& source);
   HRichHit& operator=(const HRichHit& source);

   //----------COMMON-------------------------------
   void    Reset();
   Int_t   calcAddress();
   Int_t   getSector(void)                                  { return nSector;  }
   Int_t   getAddress()                                     { return (fAddress > 0) ? fAddress : calcAddress(); }
   Int_t   getEventNr()                                     { return lEventNr;   }
   void    getXY(Float_t *px, Float_t *py) { *px = fX; *py = fY; }
   void    getLabXYZ(Float_t *px, Float_t *py, Float_t *pz) { *px = fLabX;  *py = fLabY; *pz = fLabZ; }
   Float_t getTheta()                                       { return fTheta;  }
   Float_t getPhi()                                         { return fPhi;    }
   Int_t   getRingCenterX()            { return iRingX;  };   // RICH700 : PMT index x
   Int_t   getRingCenterY()            { return iRingY;  };   // RICH700 : PMT index y

   //----------NEW RICH700---------------------------
   Double_t getRich700CircleCenterX()  { return fRich700CircleCenterX; }
   Double_t getRich700CircleCenterY()  { return fRich700CircleCenterY; }
   Double_t getRich700CircleRadius()   { return fRich700CircleRadius;  }
   Double_t getRich700CircleChi2()     { return fRich700CircleChi2;    }
   Int_t    getRich700NofRichCals()    { return fRich700NofRichCals;   }





   //-----------OLD RICH-----------------------------
   void    dumpToStdout();
   Float_t getPadY()                   { return fPadY; }
   Float_t getPadX()                   { return fPadX; }
   Bool_t  getTestRati()               { return fTestRati;  }
   Bool_t  getTestAsym()               { return fTestAsym;  }
   Bool_t  getTestCharge()             { return fTestCharge;}
   Float_t getMeanTheta()              { return fMeanTheta; }
   Float_t getMeanPhi()                { return fMeanPhi;   }
   Bool_t  getTestDens()               { return fTestDens;  }
   Bool_t  getTestBord()               { return fTestBord;  }
   Bool_t  getTestDyna()               { return fTestDyna;  }
   Bool_t  getRejFake()                { return fRejFake;   }
   Int_t   getRingQuality()            { return iRingQuality; };
   Int_t   getRingPatMat()             { return iRingPatMat;  };
   Int_t   getRingHouTra()             { return iRingHouTra;  };
   Int_t   getRingAmplitude()          { return (Int_t) iRingAmplitude; };
   Int_t   getRingPadNr()              { return (Int_t) iRingPadNr;   };
   Int_t   getRingLocalMax4()          { return (Int_t) iRingLocalMax4; };
   Int_t   getRingLocalMax8()          { return (Int_t) iRingLocalMax8; };
   Int_t   getRingClusterNr()          { return (Int_t) iRingClusterNr; };
   Int_t   getRingAlgorithmIndex()     { return (Int_t) iRingAlgorithmIndex; };
   Float_t getRingClusterSize()        { return fRingClusterSize; };
   Float_t getRingClusterLMax4()       { return fRingClusterLMax4;   };
   Float_t getRingClusterLMax8()       { return fRingClusterLMax8;   };
   Int_t   getRingMaxClusterSize()     { return fMaxClusterSize;   };
   Int_t   getRingMaxClusterSum()      { return fMaxClusterSum;   };
   Int_t   getRingMaxThrClusterSize()  { return fMaxThrClusterSize; };
   Float_t getCentroid()               { return fRingCentroid;  };
   Float_t getRadius()                 { return fRingRadius;   };


   //----------COMMON-------------------------------
   void setSector  (Int_t s)           { nSector  = s;   }
   void setEventNr (Int_t lNr)         { lEventNr = lNr; }
   void setTheta   (const Float_t th)  { fTheta   = th;  }
   void setPhi     (const Float_t phi) { fPhi     = phi; }
   void setXY(Float_t px, Float_t py)                   { fX = px;  fY = py; }
   void setLabXYZ(Float_t px, Float_t py, Float_t pz)   { fLabX = px;  fLabY = py; fLabZ = pz;   }
   void setRingCenterX          (Int_t x)               {  iRingX             = x;               };
   void setRingCenterY          (Int_t y)               {  iRingY             = y;               };

   //----------NEW RICH700---------------------------
   void setRich700CircleCenterX(Double_t val)  { fRich700CircleCenterX = val; }
   void setRich700CircleCenterY(Double_t val)  { fRich700CircleCenterY = val; }
   void setRich700CircleRadius (Double_t val)  { fRich700CircleRadius  = val; }
   void setRich700CircleChi2   (Double_t val)  { fRich700CircleChi2    = val; }
   void setRich700NofRichCals  (Int_t val)     { fRich700NofRichCals   = val; }

   //-----------OLD RICH-----------------------------

   void setPadY                 (const Float_t pady)    { fPadY               = pady;            }
   void setPadX                 (const Float_t padx)    { fPadX               = padx;            }
   void setMeanTheta            (const Float_t th)      { fMeanTheta          = th;              }
   void setMeanPhi              (const Float_t phi)     { fMeanPhi            = phi;             }
   void setTestDens             (Bool_t val)            { fTestDens           = val;             }
   void setTestBord             (Bool_t val)            { fTestBord           = val;             }
   void setTestDyna             (Bool_t val)            { fTestDyna           = val;             }
   void setTestRati             (Bool_t val)            { fTestRati           = val;             }
   void setTestAsym             (Bool_t val)            { fTestAsym           = val;             }
   void setTestCharge           (Bool_t val)            { fTestCharge         = val;             }
   void setRejFake              (Bool_t val)            { fRejFake            = val;             }
   void setRingQuality          (Int_t qual)            { iRingQuality        = qual;            };
   void setRingPatMat           (Int_t patmat)          { iRingPatMat         = patmat;          };
   void setRingHouTra           (Int_t houtra)          { iRingHouTra         = houtra;          };
   void setRingAmplitude        (Short_t ampl)          { iRingAmplitude      = ampl;            };
   void setRingPadNr            (Short_t padnr)         { iRingPadNr          = padnr;           };
   void setRingLocalMax4        (Short_t locmax4)       { iRingLocalMax4      = locmax4;         };
   void setRingLocalMax8        (Short_t locmax8)       { iRingLocalMax8      = locmax8;         };
   void setRingClusterNr        (Short_t clusternr)     { iRingClusterNr      = clusternr;       };
   void setRingAlgorithmIndex   (Short_t algoindex)     { iRingAlgorithmIndex = algoindex;       };
   void setRingClusterSize      (Float_t clustersize)   { fRingClusterSize    = clustersize;     };
   void setRingClusterLMax4     (Float_t clusterLMax4)  { fRingClusterLMax4   = clusterLMax4;    };
   void setRingClusterLMax8     (Float_t clusterLMax8)  { fRingClusterLMax8   = clusterLMax8;    };
   void setRingMaxClusterSize   (Int_t fmaxclussize)    { fMaxClusterSize     = fmaxclussize;    };
   void setRingMaxClusterSum    (Int_t fmaxclussum)     { fMaxClusterSum      = fmaxclussum;     };
   void setRingMaxThrClusterSize(Int_t fmaxthrclussize) { fMaxThrClusterSize  = fmaxthrclussize; };
   void setCentroid             (Float_t centro)        { fRingCentroid       = centro;          };
   void setRadius               (Float_t rad)           { fRingRadius         = rad;             };

   Bool_t isNewRich() { return fRich700NofRichCals == 0 ? kFALSE: kTRUE; }
   ClassDef(HRichHit, 2)
};



#endif // HRICHHIT_H

