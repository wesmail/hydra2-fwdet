//*-- Author : Dan Magestro (magestro@gsi.de)
//*-- Modified : 24/01/2002
//*-- Modified : 20/01/2004 Jacek Otwinowski
//*-- Modified : 04/01/2005 Jacek Otwinowski
//*-- Modified : 08/02/2005 A.Sadovski
//*-- Modified : 26/11/2007 A.Sadovsky
#ifndef HQAMAKER_H
#define HQAMAKER_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "TRandom.h"
#include "hcategory.h"
#include "TTree.h"
#include "TFile.h"
#include "hqatreemaker.h"
#include "hslowpar.h"

class HQAHistograms;
class HQAVariations;
class HQAScalers;
class HEvent;
//class HCategory;      //asd
class HIterator;
//class HMdcLookupGeom; //asd
//class HMdcLookupRaw;  //asd
class HMdcTimeCut;      //asd
class HQAOutputPS;
class HMdcCutStat;      //asd



class HQAMaker : public HReconstructor {

public:
   HQAMaker(void);
   HQAMaker(const Text_t* name, const Text_t* title);
   ~HQAMaker(void);

   void setSamplingRate(Int_t rate) {
      samplingRate = rate;
   }
   void setIntervalSize(Int_t size) {
      intervalSize = size;
   }
   void setPSFileName(const Char_t *fname) {
      psFile = fname;
   }
   void setOutputDir(const Char_t *fdir) {
      fDir = fdir;
   }

   void setUseSlowPar(Bool_t use) {fUseSlowPar=use;}

   void generatePDF(Bool_t kpdf = kTRUE) {
      kPDF = kpdf;
   }

   Bool_t getIteratorPointers();
   void initIteratorPointers();
   void setParContainers(); //asd

   void Print(const Option_t *option = "");
   void writeToFile(Bool_t kwtf = kTRUE) {
      kWriteToFile = kwtf;
   }

   void setBeamTime(TString beam){fbeamtime=beam;}

   Bool_t init(void);
   Bool_t finalize(void);
   Int_t  execute(void);

private:
   void buildPSFilename();

   void fillHistStart();
   void fillHistRich();
   void fillHistMdc();
   void fillHistTof();
   void fillHistRpc();
   void fillHistShower();
   void fillHistWall();
   void fillHistSpline();
   void fillHistRungeKutta();
   void fillHistRichMDC();
   void fillHistMatching();
   void fillHistPid();
   void fillScalers();
   void fillHistDaqScaler();
   void fillHistShowerRpc();

   void fillGlobalVertex();

   //Johannes Test

   void fillMassSpectrum();

   void finalizeHistStart();
   void finalizeHistRich();
   void finalizeHistMdc();
   void finalizeHistTof();
   void finalizeHistRpc();
   void finalizeHistShower();
   void finalizeHistSpline() {}
   void finalizeHistRichMDC();
   void finalizeHistDaqScaler();
   void finalizeHistPid();
   void finalizeHistShowerRpc();
   void finalizeMassSpectrum();

   HIterator *iterStCal;         //! HStartCal iterator
   HIterator *iterStHit;         //! HStartHit iterator
   HIterator *iterRichCal;       //! HRichCal iterator
   HIterator *iterRichHit;       //! HRichHit iterator
   HIterator *iterMdcRaw;        //! HMdcRaw iterator
   HIterator *iterMdcCal1;       //! HMdcCal1 iterator
   HIterator *iterMdcHit;        //! HMdcHit iterator
   HIterator *iterMdcSeg;        //! HMdcSeg iterator
   HIterator *iterTofHit;        //! HTofHit iterator
   HIterator *iterTofCluster;    //! HTofCluster iterator
   HIterator *iterRpcHit;        //! HRpcHit iterator
   HIterator *iterRpcCluster;    //! HRpcCluster iterator
   HIterator *iterShoHit;        //! HShowerHit iterator
   HIterator *iterFwRaw;         //! HWallRaw iterator
   HIterator *iterFwHit;         //! HWallHit iterator
   HIterator *iterSplineTrk;     //! HSplineTrack iterator
   HIterator *iterRungeKuttaTrk; //! HSplineTrack iterator
   HIterator *iterMetaMatch;     //! HMetaMatch iterator
   HIterator *iterParticleCand;  //! HPidTrackCand iterator
   HIterator *iterHTBoxChan;     //!



   HCategory *catSplineTrk;
   HCategory *catRungeKuttaTrk;
   HCategory *catShoHit;
   HCategory *catTfHit;
   HCategory *fCatRpcHit;
   HCategory *fCatRpcCluster;
   HCategory *catTfClust;
   HCategory *fCatParticleCand;

   HSlowPar  *SlowPar;


   TString fbeamtime;          //beam time needed for oraslowmanager


   HQAFileInfoTree TFileInfo;  //Object for File characteristics, which is filled to tree
   HQAStartTree TStart;        //Object for Start detector, which is filled to Tree
   HQARichTree TRich;          //Object for Rich detector, which is filled to Tree
   HQAMdcTree TMdc;            //Object for Mdc detector, which is filled to Tree
   HQATofTree TTof;            //Object for Tof detector, which is filled to Tree
   HQARpcTree TRpc;            //Object for Tof detector, which is filled to Tree
   HQAShowerTree TShw;         //Object for Shower detector, which is filled to Tree
   HQAPhysicsTree TPhy;         //Object with Physics Information which is written to Tree

   HLocation   lTrack;         // location used in the execute for HKickTrack
   HLocation   lMdc;           // location used in the execute for HMdcSeg

   Bool_t kFIRST;
   Bool_t kPDF;
   Bool_t kWriteToFile;   //! boolean for writing hists to gHades output file
   Bool_t isSim;          //! boolean for sim/real
   Bool_t fUseSlowPar;    //! kTRUE(defualt) to use HSlowPar container
   Bool_t makePS();

   Int_t nProcessed;      //! number of events QA-analyzed
   Int_t nEvent;          //! number of events analyzed
   Int_t nCountCalEvents; //! number of calibration events analyzed
   Int_t nInterval;       //! interval number (for variation hists)

   Int_t samplingRate;    //! rate of event sampling
   Int_t intervalSize;    //! number of events per interval (for variation hists)

   TString psFile;        //! PostScript output filename
   TString dstFile;       //! DST filename
   TString fDir;            //! QA output directory

   HQAVariations *varHists; //! Run variation histograms
   HQAHistograms *hists;    //! General QA histograms
   HQAScalers *scalers;     //! Run QA scalers

   Float_t betaMin;         //! beta cut

   TTree  *outputTree;      // This is the tree where important numbers of the different histograms are stored
   TFile *fileTree;         // This is the output file for the outputTree

   //Variables for the ouputTree


   


protected: //asd
   // pointer to used parameter containers
   //HMdcLookupGeom* lookup;   //! lookup table for mapping
   //HMdcLookupRaw*  lookupRaw;//! lookup table for mapping
   //HMdcTimeCut*    timecut;    //! container for time cuts //Not necessary
   HMdcCutStat*    cutStat;    //! container for statistics on cuts //asd

   static Int_t cutResults[4];         // contains 0/1 for cuts in t1,t2 and t2-1 after testTimeCuts()
public:

   ClassDef(HQAMaker, 1) // Hydra task for filling QA histograms and scalers
};



#endif
