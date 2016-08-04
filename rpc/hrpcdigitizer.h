/////////////////////////////////////////////////////////////////////
//  HRpcDigitizer digitizes HGeantRpc data, puts output values into
//  HRpcCalSim data category.
/////////////////////////////////////////////////////////////////////

#ifndef HRPCDIGITIZER_H
#define HRPCDIGITIZER_H

#include "hreconstructor.h"
#include "hlocation.h"
#include <vector>
#include <algorithm>
#include "hgeantrpc.h"


using namespace std;

class HIterator;
class HCategory;
class HRpcGeomCellPar;
class HRpcDigiPar;

class HRpcDigitizer : public HReconstructor {

private:

    //---------------------------------------------------------------
    // define some auxiliary strutures
    typedef struct gaptrack {  // stores all needed infos per gap
        Float_t  gtime;        // GEANT time (used for sorting)
        Float_t  time ;        // smeared time
        Float_t  charge;       // charge
        Int_t    track;        // GEANT track number at the box
        Int_t    trackDgtr;    // GEANT track number at the gap
        Int_t    ref;          // index in category for hit at box
        Int_t    refDgtr;      // index in category for hit at gap
        Bool_t   isAtBox;      // kTRUE if track is at box

        // Reset initial values
        void reset(){
            charge    =  0.;
            gtime     =  10000.;
            time      =  10000.;
            track     = -1;
            trackDgtr = -1;
            ref       = -1;
            refDgtr   = -1;
            isAtBox   = kFALSE;
        }
        Bool_t static cmpTime(gaptrack* a, gaptrack* b) {
            // sort by GEANT time in increasing order
            return a->gtime < b->gtime;
        }
    } gaptrack;


    typedef struct  {           // collection of all gaptracks per cell
      //Short_t linIndex;       // linear index in working array
                                // (= sector * maxCol * maxCell + column * maxCell + cell )
        Short_t sec;            // sector
        Short_t col;            // column
        Short_t cell;           // cell

        vector<gaptrack*> right;// gaptracks stored according to right side of the cell
        vector<gaptrack*> left; // gaptracks stored according to left  side of the cell

        void reset(){           // Reset initial values. deletes right/left vector
            //linIndex= -1;
            sec  = -1;
            col  = -1;
            cell = -1;

            for(UInt_t i=0;i<right.size();i++){ delete right[i]; }
            for(UInt_t i=0;i<left .size();i++){ delete left [i]; }
            right.clear();
            left .clear();
        }

        // Sort by increasing GEANT time (isRight=kTRUE -> right side)
        void sortTime(Bool_t isRight) {
            if(isRight) std::sort(right.begin(),right.end(),gaptrack::cmpTime);
            else        std::sort(left .begin(),left .end(),gaptrack::cmpTime);
        }

        // Get the sum of charge in all gaps (isRight=kTRUE -> right side)
        Float_t getSumCharge(Bool_t isRight){
            Float_t chrg = 0.;
            if(isRight) {for(UInt_t i=0;i<right.size();i++){ chrg += right[i]->charge; } return chrg;}
            else        {for(UInt_t i=0;i<left .size();i++){ chrg += left [i]->charge; } return chrg;}
        }

        // Get the mean of TOF in all gaps (isRight=kTRUE -> right side)
        Float_t getMeanTof(Bool_t isRight){
            Float_t time = 0.;
            if(isRight) {
              for(UInt_t i=0;i<right.size();i++){ time += right[i]->time; }
              return time/((Float_t)right.size());
            } else      {
              for(UInt_t i=0;i<left .size();i++){ time += left [i]->time; }
              return time/((Float_t)left .size());
            }
        }

        // Get the smallest digitizied TOF of all gaps (isRight=kTRUE -> right side)
        Float_t getSmallestTof(Bool_t isRight){
            Float_t time = 10000.;
            if(isRight) {
              for(UInt_t i=0;i<right.size();i++){ if(right[i]->time < time) time = right[i]->time; } return time;
            } else {
              for(UInt_t i=0;i<left .size();i++){ if(left [i]->time < time) time = left [i]->time; } return time;
            }
        }
    } rpcdat;


    typedef struct celltrack {  // stores all needed infos per gap
        Float_t  gaptime[4];    // GEANT time
        Float_t  gappos[4];     // GEANT x position
        Float_t  gapltln[4];    // GEANT local track length normalised to the gap
        Float_t  gapbeta[4];    // GEANT beta
        Float_t  time;          // GEANT average time
        Float_t  pos;           // GEANT average x position
        Float_t  beta;          // GEANT average beta
        Float_t  ltln;          // GEANT average local track length normalised to the gap
        Int_t    geantrpcIndex; // index in category
        Int_t    track;         // GEANT track number at the box

        // Reset initial values.
        void reset(){
            for(UInt_t i=0;i<4;i++){
              gaptime[i] = 0.;
              gappos[i]  = 0.;
              gapltln[i] = 0.;
              gapbeta[i] = 0.;
            }
            track         = -1;
            time          = 0.;
            pos           = 0.;
            ltln          = 0.;
            beta          = 0.;
            geantrpcIndex = -1;
        }

        // Calculate the mean of all crossed gaps
        void calcMeans(){
            UInt_t ii=0;
            for(UInt_t i=0;i<4;i++){
              if(gaptime[i]>0.) {
                time += gaptime[i];
                pos  += gappos[i];
                ltln += gapltln[i];
                beta += gapbeta[i];
                ii++;
              }
            }
            if(ii>0) {
              Float_t aii=(Float_t)ii;
              time /= aii;
              pos  /= aii;
              ltln /= aii;
              beta /= aii;
            }
        }

        // Get the beta at the first crossed gap
        Float_t getFirstGapBeta(){
            UInt_t i;
            for(i=0;i<4;i++){ if(gapbeta[i]>0.) break; }
            return gapbeta[i];
        }
    } celltrack;


    typedef struct  {             // collection of all celltracks per cell
      //Short_t linIndex;         // linear index in working array
                                  // (= sector * maxCol * maxCell + column * maxCell + cell )
        Short_t sec;              // sector
        Short_t col;              // column
        Short_t cell;             // cell

        vector<celltrack*> celltr;// gaptracks stored according to left  side of the cell

        void reset(){             // Reset initial values. deletes right/left vector
            //linIndex= -1;
            sec  = -1;
            col  = -1;
            cell = -1;

            for(UInt_t i=0;i<celltr.size();i++){ delete celltr[i]; }
            celltr.clear();
        }
    } celldat;

    typedef struct {
        Float_t x;
        Float_t y;
        Float_t time;
        Int_t sector;
        Int_t module;
	Int_t cell;
        Bool_t effi;
	void reset( ){
	    x = -999;
	    y = -999;
	    time = -999;
	    sector = -1;
	    module = -1;
	    cell   = -1;
            effi = kFALSE;
	}
	void set(Int_t se,Int_t co,Int_t ce,Float_t ax, Float_t ay, Float_t at, Bool_t eff){
	    sector = se;
	    module = co;
	    cell   = ce;
	    x      = ax;
	    y      = ay;
	    time   = at;
            effi   = eff;
	}
    } efhits;
    //---------------------------------------------------------------


    HLocation        fLoc;                //! Location for new object
    HCategory*       fGeantRpcCat;        //! Pointer to Rpc Geant data category
    HCategory*       fCalCat;             //! Pointer to Cal  data category
    HCategory*       fKineCat;            //! Pointer to Kine data category
    HRpcGeomCellPar* fGeomCellPar;        //! Geometrical parameters
    HRpcDigiPar*     fDigiPar;            //! Digitization parameters
    HIterator*       iterGeantRpc;        //! Iterator over catRpcGeantRaw category
    HIterator*       iterRpcCal;          //! Iterator over HRpcCalSim category

    Int_t maxCol  ;                       //!
    Int_t maxCell ;                       //!
    vector <celldat* > cellobjects;       //! temporary working array for regrouping gaps
                                          //! into cells
    vector <rpcdat* > rpcobjects;         //! temporary working array for digitized data
    vector <efhits>   effi_vec;           //! vector of hits for efficiency calculation
    void    initVars();
    Bool_t  initParContainer();
    void    clearObjects();
    Int_t   findMother(Int_t Ref_initial);
    void    calc_eff_hit(Int_t mode);
    void    digitize_one_hit(gaptrack* left, gaptrack* right, Int_t mode);

    Float_t Eff0,Eff1,Eff2,Eff3,Eff4,Eff5,ineff_hit_n,ineff_hit;
    Float_t sigma0_T,sigma1_T,sigma2_T,sigma3_T,sigma_el,vprop,t_offset;
    Float_t Qmean0,Qmean1,Qmean2,Qwid0,Qwid1,Qwid2;
    Float_t geaX,geaTof,geaMom,geaLocLen,geaLocLenNorm;
    Float_t D, gap,beta,eff_hit;

    static Float_t fCropDistance;


public:
    HRpcDigitizer(void);
    HRpcDigitizer(const Text_t* name,const Text_t* title);
    ~HRpcDigitizer(void);
    Bool_t init    (void);
    Bool_t finalize(void) {return kTRUE;}
    Int_t  execute (void);
    Float_t getDistanceToXedge(HGeantRpc * gea);
    static void setCropDistance(Float_t val) {fCropDistance = val;}
    ClassDef(HRpcDigitizer,0) // Digitizer of RPC data.
};

#endif
