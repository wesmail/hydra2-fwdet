//*-- Author   : 08/06/2006 D. Gonzalez-Diaz
//*-- Modified : 12/06/2006 D. Gonzalez-Diaz
//*-- Modified : 13/12/2009 D. Gonzalez-Diaz
//*-- Modified : 24/11/2010 P. Cabanelas DigiPar container moved to
//*-- Modified :            condition style.
//*-- Modified : 22/06/2012 A.Mangiarotti General code cleanup.
//*-- Modified :            Geometry parameters used in several places
//*-- Modified :            are now stored as local variables (see D,
//*-- Modified :            geaXLoc). Reading of the digitiser
//*-- Modified :            parameters, which are constant, moved out
//*-- Modified :            of the hit processing loop (see vprop,
//*-- Modified :            sigma_el, sigma_T, t_offset, Qmean, gap)!
//*-- Modified :            The inefficiency per gap for normally
//*-- Modified :            incident particles ineff_gap_n is
//*-- Modified :            calculated once for all outside the hit
//*-- Modified :            loop. Namespace TMath used consistently
//*-- Modified :            everywhere. The procedure has been
//*-- Modified :            simplified. First the ideal left and
//*-- Modified :            right times left->gtime and right->gtime
//*-- Modified :            are calculated and after the resolutions
//*-- Modified :            are added to obtain left->time and
//*-- Modified :            right->time.
//*-- Modified : 20/09/2012 A.Mangiarotti Special treatment of
//*-- Modified :            particles produced in the gap.
//*-- Modified : 20/11/2012 A.Mangiarotti modified to include the new
//*-- Modified :            model II with parametrised dependences on
//*-- Modified :            the beta of the particles. Efficiency and
//*-- Modified :            time of flight resolution employ a
//*-- Modified :            sigmoid function. The charge distribution
//*-- Modified :            is now a Landau with parameters depending
//*-- Modified :            on beta with second degree polynomials.
//*-- Modified :            An attempt to correct on average for the
//*-- Modified :            number of active gaps is included. The
//*-- Modified :            old digitizer is now available as model
//*-- Modified :            I. If the old parameter container is
//*-- Modified :            found, model I is used automatically.
//*-- Modified : 27/11/2012 A.Mangiarotti added new cell wise mode of
//*-- Modified :            operation controllable with a flag
//*-- Modified :            (fMode=1). If the new mode is selected,
//*-- Modified :            the gaps are first reorganised into
//*-- Modified :            cells before applying the response with
//*-- Modified :            model I or model II. Two new internal
//*-- Modified :            structs celltrack and celldat added to
//*-- Modified :            handle the reorganisation of the gaps
//*-- Modified :            into cells. If model II is used in cell
//*-- Modified :            wise mode, no further corrections are
//*-- Modified :            needed for the average number of active
//*-- Modified :            gaps.
//*-- Modified : 27/02/2012 A.Mangiarotti A small bug has been
//*-- Modified :            corrected: the information about the
//*-- Modified :            local track length normalised to the gap
//*-- Modified :            was not propagated from gaps to cells.
//*-- Modified :            The access to the HGeantRpc is done with
//*-- Modified :            the new optimised getHitDigi to avoid
//*-- Modified :            retrieving information that is not used.
//*-- Modified : 03/03/2013 A.Mangiarotti Two new (private) methods
//*-- Modified :            have been introduced to streamline the
//*-- Modified :            digitiser operation and prepare for the
//*-- Modified :            different options available with the new
//*-- Modified :            HGeantRpc structure.
//*-- Modified : 10/03/2013 A.Mangiarotti Digitizer adapted to the
//*-- Modified :            new HGeantRpc structure: all the old
//*-- Modified :            options remain available for the old
//*-- Modified :            HGeantRpc to have full backward
//*-- Modified :            compatibility. With the new HGeantRpc
//*-- Modified :            only the cell wise mode is available.
//*-- Modified : 25/03/2013 J.Markert Code clean up. The particle
//*-- Modified :            mass is now consistently retrieved from
//*-- Modified :            HPhysicsConstants.
//*-- Modified : 09/07/2013 A.Mangiarotti counters nHGeantRpc and
//*-- Modified :            nDigitizedRpc introduced to flag the
//*-- Modified :            events with no generated hits and no
//*-- Modified :            digitised hits, respectively. This
//*-- Modified :            allows to avoid checking all
//*-- Modified :            cellobjects when nHGeantRpc=0 and all
//*-- Modified :            rpcobjects when nDigitizedRpc=0.
//*-- Modified :            Warning: when nHGeantRpc=0, the
//*-- Modified :            information on the HGeantRpc version
//*-- Modified :            in HGeantRpc_version is not available.
//*-- Modified : 04/12/2013 G.Kornakov   Added the embedding mode
/////////////////////////////////////////////////////////////////////
//  HRpcDigitizer digitizes HGeantRpc data, puts output values into
//  HRpcCalSim data category.
/////////////////////////////////////////////////////////////////////

#include "hrpcdigitizer.h"
#include "hades.h"
#include "hphysicsconstants.h"
#include "hruntimedb.h"
#include "hdebug.h"
#include "hspectrometer.h"
#include "hrpcdetector.h"
#include "hevent.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hlocation.h"
#include "rpcdef.h"
#include "hgeantkine.h"
#include "hrpccalsim.h"
#include "hrpccal.h"
#include "hrpcdigipar.h"
#include "hrpcgeomcellpar.h"

#include "TRandom.h"

#include <iostream>
#include <iomanip>
using namespace std;

Float_t HRpcDigitizer::fCropDistance = 1.65;

HRpcDigitizer::HRpcDigitizer(void) {
    initVars();
}

HRpcDigitizer::HRpcDigitizer(const Text_t *name, const Text_t *title) :
HReconstructor(name,title) {
    initVars();
}

HRpcDigitizer::~HRpcDigitizer(void) {

    if(iterGeantRpc) delete iterGeantRpc;
    if(iterRpcCal)   delete iterRpcCal;
    clearObjects();
    cellobjects.clear();
    rpcobjects.clear();
}
void HRpcDigitizer::initVars(){

    fGeantRpcCat = 0;
    fCalCat      = 0;
    fKineCat     = 0;
    fGeomCellPar = 0;
    fDigiPar     = 0;
    fLoc.set(3,0,0,0);
    iterGeantRpc = 0;
    iterRpcCal   = 0;
}

Bool_t HRpcDigitizer::initParContainer() {
    fGeomCellPar=(HRpcGeomCellPar *)gHades->getRuntimeDb()->getContainer("RpcGeomCellPar");
    if(!fGeomCellPar){
        Error("HRpcDigitizer::init()","No RpcGeomCellPar Parameters");
        return kFALSE;
    }
    fDigiPar    =(HRpcDigiPar *)    gHades->getRuntimeDb()->getContainer("RpcDigiPar");
    if(!fDigiPar){
        Error("HRpcDigitizer::init()","No RpcDigiPar Parameters");
        return kFALSE;
    }
    return kTRUE;
}

Bool_t HRpcDigitizer::init(void) {

    HRpcDetector* rpc = (HRpcDetector*)(gHades->getSetup()->getDetector("Rpc"));


    if(!rpc){
        Error("HRpcDigitizer::init()","No Rpc Detector found");
        return kFALSE;
    }
    //---------------------------------------------------------------
    // Working arrays
    maxCol  = rpc->getMaxColumns();
    maxCell = rpc->getMaxCells();
    rpcobjects.resize(6*maxCol*maxCell, 0 );   // Size is constant over run time
    cellobjects.resize(6*maxCol*maxCell, 0 );  // Size is constant over run time
    //---------------------------------------------------------------

    //---------------------------------------------------------------
    // Parameters
    if(!initParContainer()) return kFALSE;
    //---------------------------------------------------------------

    //---------------------------------------------------------------
    // Data
    fKineCat = gHades->getCurrentEvent()->getCategory(catGeantKine);
    if(!fKineCat) {
        Error("HRpcDigitizer::init()","HGeant kine input missing");
        return kFALSE;
    }

    fGeantRpcCat = gHades->getCurrentEvent()->getCategory(catRpcGeantRaw);
    if(!fGeantRpcCat) {
        Error("HRpcDigitizer::init()","HGeant RPC input missing");
        return kFALSE;
    }

    // Build the Calibration category

    //Adding the embedding mode!
    if(gHades->getEmbeddingMode()>0) {
	//Getting the temporary category for calibrated rpc hits.
	fCalCat = gHades->getCurrentEvent()->getCategory(catRpcCalTmp);
        if(!fCalCat) {
	    fCalCat=rpc->buildMatrixCategory("HRpcCalSim",0.5);
	    gHades->getCurrentEvent()->addCategory(catRpcCalTmp,fCalCat,"Rpc");
	}
    } else {
	fCalCat = gHades->getCurrentEvent()->getCategory(catRpcCal);
	if(!fCalCat) {
	    fCalCat=rpc->buildMatrixCategory("HRpcCalSim",0.5);
	    gHades->getCurrentEvent()->addCategory(catRpcCal,fCalCat,"Rpc");
	}
    }

    iterGeantRpc = (HIterator *)fGeantRpcCat->MakeIterator("native");
    iterRpcCal   = (HIterator *)fCalCat     ->MakeIterator("native");
    //---------------------------------------------------------------

    return kTRUE;
}

Int_t HRpcDigitizer::execute(void) {

    HGeantRpc*  geantrpc = 0;
    HRpcCalSim* cal      = 0;

    Float_t mass;
    Int_t   RefL=-1,trackNumber,gpid,mode,ngap,HGeantRpc_version=-1;
    Int_t   nHGeantRpc=0,nDigitizedRpc=0;

    // Load the relevant parameters

    vprop       = fDigiPar->getVprop();                                  // [mm/ns]
    sigma_el    = (fDigiPar->getSigmaX()) / vprop;                       // [ns]
    sigma0_T    = (fDigiPar->getSigmaT())  / 1000.;                      // [ns]
    sigma1_T    = (fDigiPar->getSigmaT1()) / 1000.;                      // [ns]
    sigma2_T    = (fDigiPar->getSigmaT2());                              // pure number
    sigma3_T    = (fDigiPar->getSigmaT3());                              // pure number
    t_offset    = (fDigiPar->getToff()) / 1000.;                         // [ns]
    Qmean0      = (fDigiPar->getQmean());                                // [pC]
    Qmean1      = (fDigiPar->getQmean1());                               // [pC]
    Qmean2      = (fDigiPar->getQmean2());                               // [pC]
    Qwid0       = (fDigiPar->getQwid());                                 // [pC]
    Qwid1       = (fDigiPar->getQwid1());                                // [pC]
    Qwid2       = (fDigiPar->getQwid2());                                // [pC]
    Eff0        = (fDigiPar->getEff());                                  // pure number
    Eff1        = (fDigiPar->getEff1());                                 // pure number
    Eff2        = (fDigiPar->getEff2());                                 // pure number
    Eff3        = (fDigiPar->getEff3());                                 // pure number
    Eff4        = (fDigiPar->getEff4());                                 // pure number
    Eff5        = (fDigiPar->getEff5());                                 // pure number
    gap         = fDigiPar->getGap();                                    // [mm]
    mode        = fDigiPar->getMode();                                   // mode flag
    //
    if((sigma1_T<=0.)||(sigma2_T<=0.)||(sigma3_T<=0.)) {
      // Digitizing model I.
      sigma_el *= TMath::Sqrt2();
    }
    //
    //if((mode==0)&&(HGeantRpc_version==5)) {
    //  Error("HRpcDigitizer::init()","HGeantRpc version 5 does not allow gap mode");
    //  return kFALSE;
    //}


    //---------------------------------------------------------------
    // Loop over the HGeantrpc objects, i.e, over the gaps/cells
    // and fill temporary working objects
    clearObjects();
    iterGeantRpc->Reset();
    nHGeantRpc=0;
    nDigitizedRpc=0;
    while ((geantrpc=(HGeantRpc *)iterGeantRpc->Next()) != 0) {
        geantrpc->setVersion(5);
        // Skip hit in virtual volume EBOX.
        if(geantrpc->getDetectorID() < 0) continue;
        // Count number of HGeantRpc objects excluding hits
	// in virtual volume EBOX.

        // Crop 1.5 mm of the closest to the cell edges along the X coordinate
        if(getDistanceToXedge(geantrpc)<fCropDistance) continue;

        nHGeantRpc++;


        // Initialise HGeantRpc version and efficiency caluclation.
        if(HGeantRpc_version==-1) {
          HGeantRpc_version = geantrpc->getVersion();
          if(HGeantRpc_version<=4) {
            // Initialise the inefficency of a gap.
            calc_eff_hit(-1);
          } else {
            // Initialise the inefficency of a cell.
            calc_eff_hit(-2);
          }
        }

        // Retrive hit.
        // Units are [mm], [ns], [MeV/c], [mm] in module ref system.
        if(HGeantRpc_version<=4) {
          // Get simulation output of a gap.
          geantrpc->getHitDigi(geaX, geaTof, geaMom, geaLocLen);
        } else {
          // Get simulation output of a cell.
          geantrpc->getCellAverageDigi(gap, geaX, geaTof, geaMom, geaLocLen);
        }
        // If the gap enetering was missed in Geant, assume the gap as local track length.
        if(geaLocLen<0.) {
          geaLocLen=gap;
          geaLocLenNorm=1.;
        } else {
          geaLocLenNorm=geaLocLen/gap;
        }

        // Determine the beta of the particle.
        trackNumber = geantrpc->getTrack();
        HGeantKine* kine = (HGeantKine*)fKineCat->getObject(trackNumber-1);
        if(!kine) {
          Warning("HRpcDigitizer::execute()",
                  "missing kine entry for track %i, assuming beta=0",trackNumber);
          beta=0.;
        } else {
          gpid =  kine->getID();
          // From the Geant particle type find the mass (only charged
          // particles need to be considered).
	  mass = HPhysicsConstants::mass(gpid);
	  if(mass <= 0 ){
	      if(mass < 0 ) Warning("HRpcDigitizer::execute()",
				    "unknown particle id %i, assuming proton mass",gpid);
	      mass = HPhysicsConstants::mass(14); // unknown + gamma
	  }

          beta=mass/geaMom;
          beta=1./TMath::Sqrt(1.+beta*beta);
        }

        fLoc[0] = geantrpc->getSector();
        fLoc[1] = geantrpc->getColumn();
        fLoc[2] = geantrpc->getCell();

        // Apply efficiency.
        if(HGeantRpc_version<=4) {
          // Calculate the efficiency of a gap.
          calc_eff_hit(1);
        } else {
          // Calculate the efficiency of a cell.
          calc_eff_hit(2);
        }
        // This may depend a bit on the digitization algorithm.
	efhits hite;
        Float_t axe = -9999;
        Float_t aye = -9999;
        Float_t aze = -9999;
        Float_t ate = -9999;
        geantrpc->getCellAverage(axe, aye, aze, ate);
	Float_t rnd = gRandom->Uniform(1);
        hite.set(fLoc[0], fLoc[1], fLoc[2], axe, aye, ate, rnd<=eff_hit);
	//if(gRandom->Uniform(1) > eff_hit) {
	Int_t efprev = -1;
	for(UInt_t e=0;e<effi_vec.size();e++) {
	    if(fLoc[0]==effi_vec[e].sector &&
	       fLoc[1]==effi_vec[e].module &&
	       fLoc[2]==effi_vec[e].cell   &&
               fabs(axe-effi_vec[e].x)<2.  &&
               fabs(aye-effi_vec[e].y)<2.  &&
	       fabs(ate-effi_vec[e].time)<2.)
                efprev = effi_vec[e].effi;
	}
	if(efprev==0) {
	    continue;
	}
	if(efprev==-1 && rnd > eff_hit) {
	    effi_vec.push_back(hite);
	    continue;
	}
        if(efprev==-1 && rnd <= eff_hit) {
	    effi_vec.push_back(hite);
	}

        if((mode==0)||(HGeantRpc_version==5)) {
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        //////////// BEGIN GAP WISE OPEREATION (MODE=0) /////////////
	//////////// WITH OLD HGeantRpc before Jan 2013 /////////////
        //////////// BEGIN CELL WISE OPEREATION         /////////////
	//////////// WITH NEW HGeantRpc after Jan 2013  /////////////
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////


          //---------------------------------------------------------
          // Collect data
          // objects are stored linear
          Int_t ind = fLoc[0] * maxCol * maxCell + fLoc[1] * maxCell + fLoc[2];

          if(rpcobjects[ind] == 0) {
            // First time we have to create the object
            rpcobjects[ind] = new rpcdat;
            //rpcobjects[ind]->linIndex= ind;
            rpcobjects[ind]->sec  = fLoc[0];
            rpcobjects[ind]->col  = fLoc[1];
            rpcobjects[ind]->cell = fLoc[2];
          }
          rpcdat* dat = rpcobjects[ind];
          //---------------------------------------------------------
          // Create new objects for left/right of the gap
          // and add them to the list of gaps for this cell
          gaptrack* left  = new gaptrack;
          gaptrack* right = new gaptrack;

          left ->reset();
          right->reset();

          dat->left .push_back(left);
          dat->right.push_back(right);
          //---------------------------------------------------------

          left ->refDgtr   = fGeantRpcCat->getIndex(geantrpc);
          left ->trackDgtr = ((HGeantRpc*)fGeantRpcCat->getObject(left->refDgtr))->getTrack();

          // Right is the same as left
          right->refDgtr   = left->refDgtr;
          right->trackDgtr = left->trackDgtr;

          RefL  = findMother(left->refDgtr); // Right is the same as left , do it only once

          if(RefL < 0){ // Mother not found. Very seldom. Keep info of daughter.
            left ->ref     = left ->refDgtr;
            left ->track   = left ->trackDgtr;
            left ->isAtBox = kFALSE;
            right->ref     = left ->refDgtr;
            right->track   = left ->trackDgtr;
            right->isAtBox = kFALSE;
            Warning("HRpcDigitizer::execute()",
                    "mother of track not found in RPC box! will use RefLDgtr=%i, TrackLDgtr=%i",
                    left->refDgtr,left->trackDgtr);
          } else {      // Mother found
            left ->track   = ((HGeantRpc*)fGeantRpcCat->getObject(RefL))->getTrack();
            left ->ref     = RefL;
            left ->isAtBox = kTRUE;

            right->track   = left->track;
            right->ref     = left->ref;
            right->isAtBox = kTRUE;
          }

          // Cell length in mm
          D = fGeomCellPar->getLength(fLoc[1],fLoc[2]);
          if(HGeantRpc_version<=4) {
            // Apply the response of a gap.
            digitize_one_hit(left,right,0);
          } else {
            // Apply the response of a cell.
            digitize_one_hit(left,right,1);
          }
          nDigitizedRpc++;

          //---------------------------------------------------------

        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        //////////// END GAP WISE OPEREATION (MODE=0)   /////////////
	//////////// WITH OLD HGeantRpc before Jan 2013 /////////////
        //////////// END CELL WISE OPEREATION           /////////////
	//////////// WITH NEW HGeantRpc after Jan 2013  /////////////
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////


        } else {


        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        //////////// BEGIN CELL WISE OPEREATION (MODE=1) ////////////
	//////////// WITH OLD HGeantRpc before Jan  2013 ////////////
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
          //---------------------------------------------------------
          // Collect data
          // objects are stored linear
          Int_t ind = fLoc[0] * maxCol * maxCell + fLoc[1] * maxCell + fLoc[2];

          if(cellobjects[ind] == 0) {
            // First time we have to create the object
            cellobjects[ind] = new celldat;
            //rpcobjects[ind]->linIndex= ind;
            cellobjects[ind]->sec  = fLoc[0];
            cellobjects[ind]->col  = fLoc[1];
            cellobjects[ind]->cell = fLoc[2];
          }
          celldat* cdat = cellobjects[ind];
          //---------------------------------------------------------
          // Create new objects for the track crossing the cell
          // and add them to the list of tracks for this cell
          celltrack* celltr;
          // CASE I: no tacks in this cell yet
          if(cdat->celltr.size()==0) {
            celltr = new celltrack;
            celltr->reset();
            cdat->celltr.push_back(celltr);
          // CASE II: this cell has already tracks associated
          // with it
          } else {
            // Look if current track is already present in the list
            for(UInt_t i=0;i<cdat->celltr.size();i++){
              celltr = cdat->celltr[i];
              // CASE II a: the current track is already associated,
              // so select it
              if(celltr->track == trackNumber) break;
            }
            // CASE II b: the current track is not associated,
            // so add it
            if(celltr->track != trackNumber) {
              celltr = new celltrack;
              celltr->reset();
              cdat->celltr.push_back(celltr);
            }
          }
          //---------------------------------------------------------

          ngap = geantrpc->getGap();
          // The condition on the time takes care of possible multiple
          // re-entries of the same track.
          if(celltr->gaptime[ngap]==0.) {
            celltr->gaptime[ngap] = geaTof;
            celltr->gappos[ngap]  = geaX;
            celltr->gapltln[ngap] = geaLocLenNorm;
            celltr->gapbeta[ngap] = beta;
            celltr->track         = trackNumber;
            celltr->geantrpcIndex = fGeantRpcCat->getIndex(geantrpc);
          } else if(celltr->gaptime[ngap]<geaTof) {
            celltr->gaptime[ngap] = geaTof;
            celltr->gappos[ngap]  = geaX;
            celltr->gapltln[ngap] = geaLocLenNorm;
            celltr->gapbeta[ngap] = beta;
            celltr->track         = trackNumber;
            celltr->geantrpcIndex = fGeantRpcCat->getIndex(geantrpc);
            Warning("HRpcDigitizer::execute()",
                    "reentry of track=%i in Sector=%i Module=%i Cell=%i",
                    trackNumber,fLoc[0],fLoc[1],fLoc[2]);
          }

        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        //////////// END CELL WISE OPEREATION (MODE=1)  /////////////
	//////////// WITH OLD HGeantRpc before Jan 2013 /////////////
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////

        }

        // End of algorithm

    } // End of HGeantRpc loop


    //---------------------------------------------------------------
    // If in cell mode and version of HGeantRpc is after Jan 2013,
    // process cells
    if((mode>0)&&(HGeantRpc_version<=4)&&(nHGeantRpc>0)) {
      for(UInt_t i = 0; i < cellobjects.size(); i ++) {
        celldat* cdat = cellobjects[i];
        if(cdat){
          fLoc[0] = cdat->sec;
          fLoc[1] = cdat->col;
          fLoc[2] = cdat->cell;

          // Change to cell coordinate system (with origin at the cell center)
          // Note that fGeomCellPar->getX(column,cell) corresponds to the left-down corner
          // Cell length in mm
          D = fGeomCellPar->getLength(fLoc[1],fLoc[2]);

          // Objects are stored linear
          Int_t ind = fLoc[0] * maxCol * maxCell + fLoc[1] * maxCell + fLoc[2];

          if(rpcobjects[ind] == 0) {
            // First time we have to create the object
            rpcobjects[ind] = new rpcdat;
            //rpcobjects[ind]->linIndex= ind;
            rpcobjects[ind]->sec  = fLoc[0];
            rpcobjects[ind]->col  = fLoc[1];
            rpcobjects[ind]->cell = fLoc[2];
          }
          rpcdat* dat = rpcobjects[ind];


          for(UInt_t j = 0; j < cdat->celltr.size(); j ++) {
            celltrack* celltr = cdat->celltr[j];
            celltr->calcMeans();

            //-------------------------------------------------------
            // Create new objects for left/right of the gap
            // and add them to the list of gaps for this cell
            gaptrack* left  = new gaptrack;
            gaptrack* right = new gaptrack;

            left ->reset();
            right->reset();

            dat->left .push_back(left);
            dat->right.push_back(right);
            //-------------------------------------------------------

            left ->refDgtr   = celltr->geantrpcIndex;
            left ->trackDgtr = ((HGeantRpc*)fGeantRpcCat->getObject(left->refDgtr))->getTrack();

            // Right is the same as left
            right->refDgtr   = left->refDgtr;
            right->trackDgtr = left->trackDgtr;

            RefL  = findMother(left->refDgtr); // Right is the same as left , do it only once

            if(RefL < 0){ // Mother not found. Very seldom. Keep info of daughter.
              left ->ref     = left ->refDgtr;
              left ->track   = left ->trackDgtr;
              left ->isAtBox = kFALSE;
              right->ref     = left ->refDgtr;
              right->track   = left ->trackDgtr;
              right->isAtBox = kFALSE;
              Warning("HRpcDigitizer::execute()",
                      "mother of track not found in RPC box! will use RefLDgtr=%i, TrackLDgtr=%i",
                      left->refDgtr,left->trackDgtr);
            } else {      // Mother found
              left ->track   = ((HGeantRpc*)fGeantRpcCat->getObject(RefL))->getTrack();
              left ->ref     = RefL;
              left ->isAtBox = kTRUE;

              right->track   = left->track;
              right->ref     = left->ref;
              right->isAtBox = kTRUE;
            }

            // Geant Position
            geaX          = celltr->pos;
            // Geant Time
            geaTof        = celltr->time;
            // Geant local track length normalised to the gap
            geaLocLenNorm = celltr->ltln;
            // Geant Beta
            beta          = celltr->beta;
            // Apply response of a cell
            digitize_one_hit(left,right,1);
            nDigitizedRpc++;


            //-------------------------------------------------------

          }  // End of celltrack loop

        }

      }  // End of cellobjects loop
    }


    //---------------------------------------------------------------
    // Fill output
    if(nDigitizedRpc>0) {

      Int_t  Track    [10];
      Int_t  TrackDgtr[10];
      Bool_t isAtBox  [10];
      vector <Int_t> tracklist;
      tracklist.reserve(20); // Minimum size

      for(UInt_t i = 0; i < rpcobjects.size(); i ++) {
        rpcdat* dat = rpcobjects[i];
        if(dat){
            fLoc[0] = dat->sec;
            fLoc[1] = dat->col;
            fLoc[2] = dat->cell;

            //-------------------------------------------------------
	    // Test if cell in use. Needed in embedding mode:
	    //THIS OPTION IS NO MORE EXISTING: its used the tmp cat in the embedding mode
            // Neither in embedding mode nor in the standard this part should not be used!
            cal = (HRpcCalSim*) fCalCat->getObject(fLoc);
            if(cal) {
                // Add track from real data for sorting procedure
                gaptrack* left  = new gaptrack;
                gaptrack* right = new gaptrack;

                left ->reset();
                right->reset();

                dat->left .push_back(left);
                dat->right.push_back(right);

                left ->time      = cal->getLeftTime();
                left ->gtime     = cal->getLeftTime();
                left ->charge    = cal->getLeftCharge();
                left ->track     = gHades->getEmbeddingRealTrackId();
                left ->trackDgtr = gHades->getEmbeddingRealTrackId();
                left ->isAtBox   = kTRUE;
                right->time      = cal->getRightTime();
                right->gtime     = cal->getRightTime();
                right->charge    = cal->getRightCharge();
                right->track     = gHades->getEmbeddingRealTrackId();
                right->trackDgtr = gHades->getEmbeddingRealTrackId();
                right->isAtBox   = kTRUE;
            } else {
                cal = (HRpcCalSim*) fCalCat->getSlot(fLoc);
                if(!cal) Error("execute()","Error: could not allocate a new slot in HRpcCalSim!");
                cal = new(cal) HRpcCalSim;
                cal->setAddress(fLoc[0], fLoc[1], fLoc[2]);
            }
            //-------------------------------------------------------
            // Sort by GEANT time and select the fastest hit
            dat->sortTime(kTRUE);  // Right
            dat->sortTime(kFALSE); // Left
            if(((mode==0)&&((sigma1_T<=0.)||(sigma2_T<=0.)||(sigma3_T<=0.)))
               ||(mode>0)||(HGeantRpc_version==5)) {
              // Digitizing model I and II (except gap-wise)
              // Take the fastest in the left and in the right.
              cal->setRightTime(dat->getSmallestTof(kTRUE )); // Right
              cal->setLeftTime (dat->getSmallestTof(kFALSE)); // Left
            } else {
              // Digitizing model II gap-wise.
              // Take the average in the left and in the right.
              cal->setRightTime(dat->getMeanTof(kTRUE ));     // Right
              cal->setLeftTime (dat->getMeanTof(kFALSE));     // Left
            }
            cal->setRightCharge(dat->getSumCharge(kTRUE ));   // Right
            cal->setLeftCharge (dat->getSumCharge(kFALSE));   // Left

            cal->setRefR    (dat->right[0]->ref);
            cal->setRefRDgtr(dat->right[0]->refDgtr);
            cal->setRefL    (dat->left [0]->ref);
            cal->setRefLDgtr(dat->left [0]->refDgtr);
            //-------------------------------------------------------


            //-------------------------------------------------------
            // Fill list of tracks
            // and remove doubles

            //-------------------------------------------------------
            // Right side
            tracklist.clear();
            Int_t ct = 0;

            for(UInt_t j = 0; j < 10; j ++) {
                Track[j] = TrackDgtr[j] = -999;
                isAtBox[j] = kFALSE;
            }

            for(UInt_t j = 0; j < dat->right.size() && ct < 10; j ++ ){
                Int_t tr = dat->right[j]->track;
                if(find(tracklist.begin(),tracklist.end(),tr ) == tracklist.end()){
                    // Track was not stored before
                    Track    [ct] = tr;
                    TrackDgtr[ct] = dat->right[j]->trackDgtr;
                    isAtBox  [ct] = dat->right[j]->isAtBox;
                    // Remember used track
                    tracklist.push_back(tr);
                    ct ++;
                }
            }

            cal->setTrackRArray    (Track);
            cal->setTrackRDgtrArray(TrackDgtr);
            cal->setRisAtBoxArray  (isAtBox);
            cal->setNTracksR(ct);
            //-------------------------------------------------------

            //-------------------------------------------------------
            // Left side
            tracklist.clear();
            ct = 0;

            for(UInt_t j = 0; j < 10; j ++) {
                Track[j] = TrackDgtr[j] = -999;
                isAtBox[j] = kFALSE;
            }

            for(UInt_t j = 0; j < dat->left.size() && ct < 10 ; j ++ ){
                Int_t tr = dat->left[j]->track;
                if(find(tracklist.begin(),tracklist.end(),tr) == tracklist.end()){
                    // Track was not stored before
                    Track    [ct] = tr;
                    TrackDgtr[ct] = dat->left[j]->trackDgtr;
                    isAtBox  [ct] = dat->left[j]->isAtBox;
                    // Remember used track
                    tracklist.push_back(tr);
                    ct ++;
                }
            }

            cal->setTrackLArray    (Track);
            cal->setTrackLDgtrArray(TrackDgtr);
            cal->setLisAtBoxArray  (isAtBox);
            cal->setNTracksL(ct);
            //-------------------------------------------------------

            //-------------------------------------------------------
        }
      }
    }
    //---------------------------------------------------------------

    return 0;
}

void  HRpcDigitizer::clearObjects(){
    // Delete objects in working array and
    // set pointer to 0. vector is still
    // not cleared.
    effi_vec.clear();
    for(UInt_t i = 0; i < rpcobjects.size(); i ++){
        if(rpcobjects[i]){
            rpcobjects[i]->reset();
            delete rpcobjects[i];
            rpcobjects[i] = 0;
        }
    }
    for(UInt_t i = 0; i < cellobjects.size(); i ++){
        if(cellobjects[i]){
            cellobjects[i]->reset();
            delete cellobjects[i];
            cellobjects[i] = 0;
        }
    }
}

Int_t HRpcDigitizer::findMother(Int_t Ref_initial) {

  // Algorithm for finding mother. When not found, -1 is returned
  // in order to tag the track. It very rarely happens and is
  // always associated to neutral particles at the box.

  Int_t detectorID =  0, track_mother = -1, Ref_final = -1;

  HGeantRpc*  geantrpc = 0;
  HGeantKine* kine     = 0;

  track_mother = ((HGeantRpc*)fGeantRpcCat->getObject(Ref_initial))->getTrack();
  kine         = (HGeantKine*)fKineCat    ->getObject(track_mother - 1);

  while(kine && detectorID >= 0){

      if(track_mother == 0)  { //End of loop. Mother not found
          Ref_final = -999;
          break;
      }

      kine = (HGeantKine*)fKineCat->getObject(track_mother - 1);

      if(kine && kine->getNRpcHits() == 0) { //Mother has no hit in RPC cells
          track_mother = kine->getParentTrack();
          continue;
      }

      Ref_final    = kine->getFirstRpcHit(); //Look for the first hit (time-wise) of the track
      geantrpc     = (HGeantRpc*)fGeantRpcCat->getObject(Ref_final);
      detectorID   = geantrpc->getDetectorID();
      track_mother = kine->getParentTrack();

  }

  return Ref_final;
}

void HRpcDigitizer::calc_eff_hit(Int_t mode) {

  // Algorithm to calculate the inefficiency of a gap. The efficiency
  // in the digitizer parameters is ALWAYS per CELL.
  // If mode=-1 the efficiency/inefficiency per hit are initialised as per gap,
  // If mode=-2 the efficiency/inefficiency per hit are initialised as per cell,
  // If mode=1 the efficiency/inefficiency per hit are calculated as per gap,
  // If mode=2 the efficiency/inefficiency per hit are calculated as per cell.

  if(mode<0) {
  //
  // Initialisation call
  //
    if((Eff1==0.)||(Eff2<=0.)||(Eff3<=0.)) {
      // Digitizing model I.
      if(mode==-1) {
        // HIT == GAP
        ineff_hit_n = TMath::Sqrt(TMath::Sqrt(1. - fDigiPar->getEff()));
      } else {
        // HIT == CELL
        ineff_hit_n = 1. - fDigiPar->getEff();
      }
    } else {
      ineff_hit_n = 0.;
    }
  } else {
  //
  // Calculation call
  //
    if((Eff1==0.)||(Eff2<=0.)||(Eff3<=0.)) {
      // Digitizing model I.
      ineff_hit = TMath::Power(ineff_hit_n,geaLocLenNorm);    // correction for angle of incidence
    } else {
      // Digitizing model II.
      eff_hit = Eff0+Eff1/(1.+TMath::Exp(-Eff2*(beta-Eff3))) + TMath::Exp(Eff4*beta+Eff5);
      if(mode==1) {
        // HIT == GAP
        ineff_hit = TMath::Sqrt(TMath::Sqrt(1. - eff_hit));   // convert to inefficency of a gap
      } else {
        // HIT == CELL
        ineff_hit = 1. - eff_hit;
      }
      ineff_hit = TMath::Power(ineff_hit,(geaLocLenNorm));    // correction for angle of incidence
    }
    // Efficiency per gap
    eff_hit = 1. - ineff_hit;
  }
}

void HRpcDigitizer::digitize_one_hit(gaptrack* left,gaptrack* right,Int_t mode) {

  // Algorithm to calculate the response of a gap.
  // If mode=0 in MODEL II a corection for the average number of active
  // gaps is applyed.

  Float_t geaXLoc = 0.;
  Float_t beta2,corr_ngap,sigma_T,sigma_el_here,X_smearing,T_smearing;
  Float_t Qmean,Qwid,Qgap;

  // Change to cell coordinate system (with origin at the cell center)
  // Note that fGeomCellPar->getX(column,cell) corresponds to the left-down corner
  geaXLoc = fGeomCellPar->getX(fLoc[1],fLoc[2]) - geaX; // Position in cell frame

  // Calculate GEANT time left/right without smearing for sorting
  left->gtime  = geaTof + geaXLoc / vprop;              // Geant Time at left-end
  right->gtime = geaTof + (D-geaXLoc) / vprop;          // Geant Time at right-end


  // Common to left and right!. Intrinsic RPC fluctuations.
  if((sigma1_T<=0.)||(sigma2_T<=0.)||(sigma3_T<=0.)) {
    // Digitizing model I.
    // Common to left and right!. Intrinsic RPC fluctuations.
    T_smearing  = gRandom->Gaus(t_offset, sigma0_T);
    left ->time = left ->gtime + T_smearing + gRandom->Gaus(0.,sigma_el);
    right->time = right->gtime + T_smearing + gRandom->Gaus(0.,sigma_el);
  } else {
    // Digitizing model II.
    sigma_T = sigma0_T+sigma1_T/(1.+TMath::Exp(-sigma2_T*(beta-sigma3_T)));
    sigma_el_here=sigma_el;

    // Attempt to approximately correct for the number of active gaps.
    if(mode==0) {
      Float_t norm=0.;
      corr_ngap=0.;
      for(Int_t i = 1; i < 5; i ++) {
        Float_t w4i=TMath::Binomial(4,i)*TMath::Power(eff_hit,i)*TMath::Power(ineff_hit,4-i);
        norm += w4i;
        corr_ngap += w4i/TMath::Sqrt((Float_t)i);
      }
      // The correction factor for the sigma of the Gaussian is the
      // inverse of corr_ngap/norm, to have a multiplicative correction
      // it is calculated here once for all.
      corr_ngap = norm/corr_ngap;
      sigma_T       *= corr_ngap;
      sigma_el_here *= corr_ngap;
    }

    T_smearing = gRandom->Gaus(t_offset,sigma_T);
    // The position and time resolutions are decoupled.
    X_smearing = gRandom->Gaus(0.,sigma_el_here);
    //X_smearing = 0.;
    left ->time = left ->gtime + T_smearing + X_smearing;
    right->time = right->gtime + T_smearing - X_smearing;
  }

  // Generated charge.
  if(Qwid0<=0.) {
    // Digitizing model I.
    // Charge distribution assumed to be flat per gap. At present slewing
    // correction is not included. Also charges in left and in right are
    // *exactly* the same by definition. A smearing in charge to account
    // for the resolution of the Q2W method can be easily included.
    Qgap = (gRandom->Uniform(1)) * (2. * Qmean0) / 4. * geaLocLenNorm; // Correction for angle of incidence
  } else {
    // Digitizing model II.
    // Use Landau distribution with a mean and width parametrised with
    // second order polynomials in beta.
    beta2 = beta*beta;
    Qmean = Qmean0+Qmean1*beta+Qmean2*beta2;
    Qwid  = Qwid0 +Qwid1*beta +Qwid2*beta2;

    // Attempt to approximately correct for the number of active gaps.
    if(mode==0) {
      Float_t norm=0.;
      corr_ngap=0.;
      for(Int_t i = 1; i < 5; i ++) {
        Float_t w4i=TMath::Binomial(4,i)*TMath::Power(eff_hit,i)*TMath::Power(ineff_hit,4-i);
        norm += w4i;
        corr_ngap += w4i*TMath::Sqrt((Float_t)i);
      }
      // The correction factor for the sigma of the Gaussian is the
      // inverse of corr_ngap/norm, to have a multiplicative correction
      // it is calculated here once for all. Note that this is very
      // approximate for a Landau distribution.
      corr_ngap = norm/corr_ngap;
      // Per gap.
      Qmean /= (4.*eff_hit);
      // Per gap.
      Qwid  *= corr_ngap;
    }

    // Correction for angle of incidence.
    Qmean *= geaLocLenNorm;
    // Correction for angle of incidence.
    Qwid  *= geaLocLenNorm;
    Qgap   = gRandom->Landau(Qmean,Qwid);
  }
  left ->charge = Qgap;
  right->charge = Qgap;
}

Float_t HRpcDigitizer::getDistanceToXedge(HGeantRpc * gea) {
    if(!fGeomCellPar || gea->getDetectorID() < 0) {
	return -1.;
    }
    
    Int_t module = gea->getColumn();
    Int_t cell   = gea->getCell();
    if(module<0 || cell<0) {
	return -1.;
    }
    Float_t axHit   = 0.;
    Float_t ayHit   = 0.;
    Float_t azHit   = 0.;
    Float_t atofHit = 0.;
    gea->getCellAverage(axHit, ayHit, azHit, atofHit);
    // In case the cell number is not the first:
    Float_t xCorner  = fGeomCellPar->getX(module,cell);
    Float_t yCorner  = fGeomCellPar->getY(module,cell);
    Float_t length   = fGeomCellPar->getLength(module,cell);
    Float_t rxCorner  = xCorner  - length;

    Int_t trackNumber = gea->getTrack();
    HGeantKine* kine = (HGeantKine*)fKineCat->getObject(trackNumber-1);
    if(!kine) return 0;

    Float_t xCornerL=-1e5;
    Float_t lengthL=-1e5;
    Float_t yCornerL=-1e5;
    Float_t rxCornerL=1e-5;
    if(cell>0) {
	xCornerL = fGeomCellPar->getX(module,cell-1);
	lengthL  = fGeomCellPar->getLength(module,cell-1);
	yCornerL = fGeomCellPar->getY(module,cell-1);
	rxCornerL = xCornerL - lengthL;
    }
    
    Float_t angle1 = 0.;
    Float_t angle2 = 0.;
    if(lengthL>0) {
	angle1 = atan2(xCorner-xCornerL  ,yCorner-yCornerL);
	angle2 = atan2(rxCorner-rxCornerL,yCorner-yCornerL);
	// Check that is not one of the cells that has the outer part flat!
	if(cell<30 && cell>27) {
	    if(xCorner == fGeomCellPar->getX(module,cell+1)) {
                angle1 = 0.;
	    }
	    if(rxCorner == (fGeomCellPar->getX(module,cell+1) - fGeomCellPar->getLength(module,cell+1)) ) {
                angle2 = 0.;
	    }
	}
    } else {
	xCornerL = fGeomCellPar->getX(module,cell+1);
        yCornerL = fGeomCellPar->getY(module,cell+1);
	lengthL  = fGeomCellPar->getLength(module,cell+1);
	rxCornerL = xCornerL + lengthL;
	angle1 = atan2(  xCornerL-xCorner,yCornerL-yCorner);
	angle2 = atan2(rxCornerL-rxCorner,yCornerL-yCorner);
    }

    Float_t distL = -axHit + (xCorner + sin(angle1) * (ayHit-yCorner));
    Float_t distR = axHit - (rxCorner + sin(angle2) * (ayHit-yCorner));

    return distL<distR ? distL : distR;
}

ClassImp(HRpcDigitizer)
