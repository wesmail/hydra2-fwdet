//*-- AUTHOR : A.Zinchenko <Alexander.Zinchenko@jinr.ru>
//*-- Created : 08/08/2016
//*-- Modified: R. Lalik <Rafal.Lalik@ph.tum.de> 2016/10/10

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HFwDetStrawVectorFinder
//
//  Tracking code for FwDetStraw
//
/////////////////////////////////////////////////////////////

#include "hfwdetstrawvectorfinder.h"
#include "fwdetdef.h"
#include "hades.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hevent.h"
#include "hfwdetdetector.h"
#include "hfwdetgeompar.h"
#include "hfwdetstrawdigitizer.h"
#include "hfwdetstrawdigipar.h"
#include "hfwdetstrawcalsim.h"
#include "hfwdetstrawvector.h"
#include "hgeominterface.h"
#include "hgeomrootbuilder.h"
#include "hgeomvolume.h"
#include "hgeomcompositevolume.h"
#include "hruntimedb.h"
#include "hspectrometer.h"

#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TMatrixFLazy.h"
#include "TVectorD.h"

#include <iostream>
using namespace std;

//FILE *lun = fopen("chi2.dat","w");

// -----   Default constructor   -------------------------------------------
HFwDetStrawVectorFinder::HFwDetStrawVectorFinder() : HReconstructor()
{
    initVariables();
}
// -------------------------------------------------------------------------

// -----   Constructor   ---------------------------------------------------
HFwDetStrawVectorFinder::HFwDetStrawVectorFinder(const Text_t *name, const Text_t *title) :
    HReconstructor(name, title)
{
    initVariables();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
HFwDetStrawVectorFinder::~HFwDetStrawVectorFinder()
{
    for (Int_t i = 0; i <= fgkStat; ++i)
    {
        Int_t nVecs = fVectors[i].size();
        for (Int_t j = 0; j < nVecs; ++j) delete fVectors[i][j];
    }
    for (map<Int_t,TDecompLU*>::iterator it = fLus.begin(); it != fLus.end(); ++it)
    {
        delete it->second;
    }
}
// -------------------------------------------------------------------------

// -----   Public method initVariables  ------------------------------------
void HFwDetStrawVectorFinder::initVariables()
{
    fHits = fPoints = fTrackArray = NULL;
    fStrawDigiPar = NULL;
    fCutChi2 = 24.0; // chi2/ndf=6 for 8 hits
    fNpass = 1; // number of passes
    for (Int_t i = 0; i < fgkStat; ++i)
    {
        fIndx0[i] = 0;
    }
}
// -------------------------------------------------------------------------

// -----   Public method init (abstract in base class)  --------------------
Bool_t HFwDetStrawVectorFinder::init()
{
    // find the Forward detector in the HADES setup
    HFwDetDetector* pFwDet = (HFwDetDetector*)(gHades->getSetup()->getDetector("FwDet"));
    if (!pFwDet)
    {
        Error("HFwDetStrawVectorFinder::init","No Forward Detector found");
        return kFALSE;
    }

    // GEANT input data
    fPoints = gHades->getCurrentEvent()->getCategory(catFwDetGeantRaw);
    if (!fPoints)
    {
        Error("HFwDetStrawVectorFinder::init()","HGeant FwDet input missing");
        return kFALSE;
    }

    // hits
    fHits = gHades->getCurrentEvent()->getCategory(catFwDetStrawCal);
    if (!fHits)
    {
        Error("HFwDetStrawVectorFinder::init()","HFwDetStrawCal input missing");
        return kFALSE;
    }

    // build the Straw vector category
    fTrackArray = new HLinearCategory("HFwDetStrawVector");
    if (!fTrackArray)
    {
        Error("HFwDetStrawVectorFinder::init()","Straw vector category not created");
        return kFALSE;
    }
    else
    {
        gHades->getCurrentEvent()->addCategory(catFwDetStrawVector, fTrackArray, "FwDet");
    }

    // create the parameter container
    fStrawDigiPar = (HFwDetStrawDigiPar*) gHades->getRuntimeDb()->getContainer("FwDetStrawDigiPar");
    if (!fStrawDigiPar)
    {
        Error("HFwDetStrawVectorFinder::init()","Parameter container for vector finder not created");
        return kFALSE;
    }

    gHades->getRuntimeDb()->getContainer("FwDetGeomPar");
    fErrU = fDiam / TMath::Sqrt(12.0);

    return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method reinit  -------------------------------------------
Bool_t HFwDetStrawVectorFinder::reinit()
{
    // Get geometry parameters from database

    HRuntimeDb *rtdb = gHades->getRuntimeDb();
    HFwDetGeomPar* fwDetGeomPar = (HFwDetGeomPar*) rtdb->getContainer("FwDetGeomPar");
    for (Int_t i = 0; i < fgkStat; ++i)
    {
        HModGeomPar* modgeom = fwDetGeomPar->getModule(i);
        HGeomTransform& modLabTrans = modgeom->getLabTransform();
        //modLabTrans.print();
        HGeomCompositeVolume* mod = modgeom->getRefVolume();

        // Loop over doublets
        for (Int_t lay = 0; lay < fgkPlanes/2; ++lay)
        {
            HGeomVolume* vol = mod->getComponent(lay);
            HGeomTransform tr(vol->getTransform());
            tr.transFrom(modLabTrans);
            //tr.print();
            Double_t layZ = tr.getTransVector().getZ(); // layer position
            Double_t layDZ = TMath::Abs (vol->getPoint(0)->getZ()); // layer half-thickness
            //cout << " layZ, dz " << layZ << " " << layDZ << endl;
            HGeomRotation rot(tr.getRotMatrix());

            // Loop over tube planes
            for (Int_t iside = 0; iside < 2; ++iside)
            {
                Int_t plane = lay * 2 + iside;
                Double_t shift = layDZ - fDiam / 2;
                if (iside) shift = -shift;
                Double_t wireZ = layZ - shift;
                if (plane == 0) fZ0[i] = wireZ;
                plane += i * fgkPlanes;
                fDz[plane] = wireZ;
                fCosa[plane] = rot.getElement(0,0);
                fSina[plane] = rot.getElement(1,0);
                if (lay)
                {
                    fDtubes[i][lay-1] =
                        fRmax[i] * TMath::Tan (TMath::ASin(fSina[plane]) - TMath::ASin(fSina[plane-2]));
                    fDtubes[i][lay-1] = TMath::Abs(fDtubes[i][lay-1]) / fDiam + 10;
                }
            }
        }
    }

    for (Int_t i = fgkPlanes2 - 1; i >= 0; --i)
    {
        cout << i << " " << fDz[i] << " " << fSina[i] << endl;
        fDz[i] -= fDz[0];
        //fDz[i+fgkPlanes] = fDz[i];
    }

    computeMatrix(); // compute system matrices
    return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method execute   -----------------------------------------
Int_t HFwDetStrawVectorFinder::execute()
{
    //gErrorIgnoreLevel = kInfo; //debug level
    gErrorIgnoreLevel = kWarning; //debug level

    //cout << " Hades: " << gHades->getTree() << " " << gHades->getTree()->GetBranch("HFwDetStrawVector") << endl;
    //gHades->getTree()->GetBranch("HFwDetStrawVector")->setName("StrawVector"); // to change branch name

    for (Int_t i = 0; i <= fgkStat; ++i)
    {
        Int_t nVecs = fVectors[i].size();
        for (Int_t j = 0; j < nVecs; ++j) delete fVectors[i][j];
        fVectors[i].clear();
        if (i < fgkStat)
        {
            fVectorsHigh[i].clear();
            fIndx0[i] = 0;
        }
    }

    // Do all processing
    //Warning("execute"," Event No: %d", gHades->getEventCounter());
    cout << " Event No: " << gHades->getEventCounter() << " " << fHits->getEntries() << endl;

    for (Int_t ipass = 0; ipass < fNpass; ++ipass)
    {
        // Get hits
        getHits();

        // Build vectors
        makeVectors();

        // Remove vectors with wrong orientation
        // (using empirical cuts for omega muons at 8 GeV)
        checkParams();

        // Go to the high resolution mode processing
        Double_t err = fErrU;
        fErrU = 0.2;
        if (fErrU < 1.0) highRes();

        // Remove clones
        //RemoveClones();

        // Remove short tracks
        //RemoveShorts();

        // Store vectors
        storeVectors(0);

        // Merge vectors
        mergeVectors();

        // Select final tracks - remove ghosts
        selectTracks(ipass);

        fErrU = err;
        if (ipass == fNpass - 1) continue;
        for (Int_t i = 0; i < fgkStat; ++i) fIndx0[i] = fVectors[i].size();
    }

    // Store tracks
    storeVectors(1);

    return 0;
}
// -------------------------------------------------------------------------

// -----   Public method finalize (after each event) -----------------------
Bool_t HFwDetStrawVectorFinder::finalize()
{
    /*
    for (Int_t i = 0; i < fgkStat; ++i) {
        Int_t nVecs = fVectors[i].size();
        for (Int_t j = 0; j < nVecs; ++j) delete fVectors[i][j];
        //nVecs = fVectorsHigh[i].size();
        //for (Int_t j = 0; j < nVecs; ++j) delete fVectorsHigh[i][j];
    }
    for (map<Int_t,TDecompLU*>::iterator it = fLus.begin(); it != fLus.end(); ++it)
        delete it->second;
    */

    return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Private method ComputeMatrix   ----------------------------------
void HFwDetStrawVectorFinder::computeMatrix()
{
    // Compute system matrices for different hit plane patterns

    Double_t cos2[fgkPlanes2], sin2[fgkPlanes2], sincos[fgkPlanes2], dz2[fgkPlanes2];
    Bool_t onoff[fgkPlanes2];

    for (Int_t i = 0; i < fgkPlanes2; ++i)
    {
        cos2[i] = fCosa[i] * fCosa[i];
        sin2[i] = fSina[i] * fSina[i];
        sincos[i] = fSina[i] * fCosa[i];
        dz2[i] = fDz[i] * fDz[i];
        onoff[i] = kTRUE;
    }

    TMatrixD coef(4,4);
    Int_t  pattMax = 1 << fgkPlanes2, pattMax1 = 1 << fgkPlanes, nDouble = 0, nTot = 0;

    // Loop over doublet patterns
    for (Int_t ipat = 1; ipat < pattMax; ++ipat)
    {

        // Check if the pattern is valid:
        // either all doublets are active or 3 the first ones (for high resolution mode)
        nDouble = 0;
        if (ipat < pattMax1)
        {
            // One station
            for (Int_t j = 0; j < fgkPlanes; j += 2) if (ipat & (3 << j)) ++nDouble; else break;
        }
        else
        {
            // The other station
            Int_t ipat1 = ipat & ((pattMax1-1)<<fgkPlanes);
            for (Int_t j = 0; j < fgkPlanes; j += 2)
            {
                if (ipat1 & (3 << (j+fgkPlanes))) ++nDouble;
                else break;
            }
        }
        //if ((ipat & (3 << 6)) == 0) ++nDouble; // 3 doublets
        //if (nDouble < fgkPlanes / 2) continue;
        //if (nDouble + 1 < fgkPlanes / 2) continue;
        if (nDouble + 0 < fgkPlanes / 2) continue;

        for (Int_t j = 0; j < fgkPlanes2; ++j) onoff[j] = (ipat & (1 << j));

        coef = 0.0;
        for (Int_t i = 0; i < fgkPlanes2; ++i)
        {
            if (!onoff[i]) continue;
            coef(0,0) += cos2[i];
            coef(0,1) += sincos[i];
            Double_t dz = fDz[i];
            if ((ipat&255)==0) dz = fDz[i%fgkPlanes]; // only second station
            coef(0,2) += dz * cos2[i];
            coef(0,3) += dz * sincos[i];
        }
        for (Int_t i = 0; i < fgkPlanes2; ++i)
        {
            if (!onoff[i]) continue;
            coef(1,0) += sincos[i];
            coef(1,1) += sin2[i];
            Double_t dz = fDz[i];
            if ((ipat&255)==0) dz = fDz[i%fgkPlanes]; // only second station
            coef(1,2) += dz * sincos[i];
            coef(1,3) += dz * sin2[i];
        }
        for (Int_t i = 0; i < fgkPlanes2; ++i)
        {
            if (!onoff[i]) continue;
            Double_t dz = fDz[i], dzz = dz2[i];
            if ((ipat&255)==0) { dz = fDz[i%fgkPlanes]; dzz = dz2[i%fgkPlanes]; } // only second station
            coef(2,0) += dz * cos2[i];
            coef(2,1) += dz * sincos[i];
            coef(2,2) += dzz * cos2[i];
            coef(2,3) += dzz * sincos[i];
        }
        for (Int_t i = 0; i < fgkPlanes2; ++i)
        {
            if (!onoff[i]) continue;
            Double_t dz = fDz[i], dzz = dz2[i];
            if ((ipat&255)==0)
            {
                dz = fDz[i%fgkPlanes]; dzz = dz2[i%fgkPlanes];
            } // only second station
            coef(3,0) += dz * sincos[i];
            coef(3,1) += dz * sin2[i];
            coef(3,2) += dzz * sincos[i];
            coef(3,3) += dzz * sin2[i];
        }
        if (coef.Determinant() < 1.e-15)
            continue;

        ++nTot;

        TDecompLU *lu = new TDecompLU(4);
        lu->SetMatrix(coef);
        lu->SetTol(0.1*lu->GetTol());
        lu->Decompose();
        fLus.insert(pair<Int_t,TDecompLU*>(ipat,lu));
        TMatrixDSym cov(4);
        cov.SetMatrixArray(coef.GetMatrixArray());
        cov.Invert(); // covar. matrix
        fMatr.insert(pair<Int_t,TMatrixDSym*>(ipat,new TMatrixDSym(cov)));
    }
}
// -------------------------------------------------------------------------

// -----   Private method GetHits   ----------------------------------------
void HFwDetStrawVectorFinder::getHits()
{
    // Group hits according to their plane number

    for (Int_t ista = 0; ista < fgkStat; ++ista)
    {
        for (Int_t i = 0; i < fgkPlanes; ++i) fHitPl[ista][i].clear();
        for (Int_t i = 0; i < fgkPlanes / 2; ++i) fHit2d[ista][i].clear();
    }

    Int_t nHits = fHits->getEntries(), nSelTu[fgkStat] = {0}, sel = 0;
    Char_t st3, doub, lay;
    Int_t station3, doublet, tube;
    HFwDetStrawCalSim *hit;

    for (Int_t i = 0; i < nHits; ++i)
    {
        hit = (HFwDetStrawCalSim*) fHits->getObject(i);
        if (hit->GetUniqueID() != 0) continue; // already used
        //if (hit->getPlane() == 2) continue; //!!! test

        //!!! For debug - select hits with certain track IDs
        //sel = SelectHitId(hit, 2);
        sel = 1;
        if (!sel) continue;
        //

        hit->getAddress(st3, doub, lay, tube);
        station3 = (Int_t) st3;
        doublet = (Int_t) doub;
        Int_t layer = hit->getLayer();
        //cout << hit->get() << " " << station3 << " " << doublet << " " << layer << endl;
        Int_t plane = doublet * 2 + layer;
        //fHitPl[station3][plane].insert(pair<Int_t,Int_t>(hit->GetTube()+1000*hit->GetSegment(),i));
        fHitPl[station3][plane].insert(pair<Int_t,Int_t>(tube+1000*0,i));
        //if (fErrU < 0) fErrU = hit->GetDu();
        if (sel) ++nSelTu[station3];
    }

    // Merge neighbouring hits from 2 layers of 1 doublet.
    // If there is no neighbour, takes hit from a single layer (to account for inefficiency)
    Int_t nlay2 = fgkPlanes / 2, indx1 = 0, indx2 = 0, tube1, tube2, next1, next2;

    for (Int_t ista = 0; ista < fgkStat; ++ista)
    {
        // Loop over stations
        Int_t nSelDouble = 0;

        for (Int_t i1 = 0; i1 < nlay2; ++i1)
        {
            // Loop over doublets
            multimap<Int_t,Int_t>::iterator it1 = fHitPl[ista][i1*2].begin(), it2 = fHitPl[ista][i1*2+1].begin();
            multimap<Int_t,Int_t>::iterator it1end = fHitPl[ista][i1*2].end(), it2end = fHitPl[ista][i1*2+1].end();
            set<Int_t> tubeOk[2];
            next1 = next2 = 1;
            if (it1 == it1end) next1 = 0;
            if (it2 == it2end) next2 = 0;

            while (next1 || next2)
            {
                // Loop over tubes
                if (next1)
                {
                    indx1 = it1->second;
                    //hit1 = (CbmMuchStrawHit*) fHits->UncheckedAt(indx1);
                    tube1 = it1->first;
                }

                if (next2)
                {
                    indx2 = it2->second;
                    //hit2 = (CbmMuchStrawHit*) fHits->UncheckedAt(indx2);
                    tube2 = it2->first;
                }

                if ((it2 != it2end && tube2 < tube1) || it1 == it1end)
                {
                    // Single layer hit2 ?
                    if (tubeOk[1].find(tube2) == tubeOk[1].end())
                    {
                        sel = selDoubleId(-1, indx2);
                        nSelDouble += sel;
                        if (sel) fHit2d[ista][i1].push_back(pair<Int_t,Int_t>(-1,indx2));
                        tubeOk[1].insert(tube2);
                    }
                    ++it2;
                    next2 = 1;
                    next1 = 0;
                    if (it2 == fHitPl[ista][i1*2+1].end()) next2 = 0;
                    continue;
                }

                if ((it1 != it1end && tube2 > tube1 + 1) || it2 == it2end)
                {
                    // Single layer hit1 ?
                    if (tubeOk[0].find(tube1) == tubeOk[0].end())
                    {
                        sel = selDoubleId(indx1, -1);
                        nSelDouble += sel;
                        if (sel) fHit2d[ista][i1].push_back(pair<Int_t,Int_t>(indx1,-1));
                        tubeOk[0].insert(tube1);
                    }
                    ++it1;
                    next1 = 1;
                    next2 = 0;
                    if (it1 == fHitPl[ista][i1*2].end()) next1 = 0;
                    continue;
                }

                // Double layer hit
                sel = selDoubleId(indx1, indx2);
                nSelDouble += sel;
                if (sel) fHit2d[ista][i1].push_back(pair<Int_t,Int_t>(indx1,indx2));
                tubeOk[0].insert(tube1);
                tubeOk[1].insert(tube2);
                if (tube2 == tube1)
                {
                    ++it2;
                    next2 = 1;
                    next1 = 0;
                    if (it2 == fHitPl[ista][i1*2+1].end())
                    {
                        next2 = 0;
                        next1 = 1;
                        if (it1 == fHitPl[ista][i1*2].end()) next1 = 0;
                    }
                }
                else
                {
                    ++it1;
                    next1 = 1;
                    next2 = 0;
                    if (it1 == fHitPl[ista][i1*2].end())
                    {
                        next1 = 0;
                        next2 = 1;
                        if (it2 == fHitPl[ista][i1*2+1].end()) next2 = 0;
                    }
                }
                continue;
            } // while (next1...
        }
        cout << " Selected tubes: " << ista << " " << nSelTu[ista] << endl;
        cout << " Selected doublets: " << ista << " " << nSelDouble << endl;
    }
}
// -------------------------------------------------------------------------

// -----   Private method SelectHitId   ------------------------------------
Bool_t HFwDetStrawVectorFinder::selectHitId(HFwDetStrawCalSim *hit, Int_t idSel)
{
    // Select hits with certain track IDs (for debugging)

    //map<Double_t,Int_t>::iterator mit = hit->getDriftId().begin();
    //for ( ; mit != hit->getDriftId().end(); ++mit)
    {
        //if (mit->second == idSel)
        {
            //cout << " Plane: " << hit->getPlane() << " " << mit->second << endl;
            // Set proper drift distance
            //Double_t err = hit->getDrift() - hit->getDriftId().begin()->first;
            //Double_t coord = mit->first + err;
            //coord = TMath::Max (0.0, coord);
            //coord = TMath::Min (coord, fDiam/2);
            //hit->setDrift(coord);
            //return kTRUE;
        }
    }

    return kFALSE;
}
// -------------------------------------------------------------------------

// -----   Private method selDoubleId   ------------------------------------
Bool_t HFwDetStrawVectorFinder::selDoubleId(Int_t indx1, Int_t indx2)
{
    // Select doublets with certain track IDs (for debugging)

    return kTRUE; // no selection
}
// -------------------------------------------------------------------------

// -----   Private method MakeVectors   ------------------------------------
void HFwDetStrawVectorFinder::makeVectors()
{
    // Make vectors for stations

    for (Int_t ista = 0; ista < fgkStat; ++ista)
    {
        /*
        Int_t nvec = fVectors[ista].size();
        for (Int_t j = 0; j < nvec; ++j) delete fVectors[ista][j];
        fVectors[ista].clear();
        */
        Int_t lay2 = 0, patt = 0, flag = 0, ndoubl = fHit2d[ista][lay2].size();
        HFwDetStrawCalSim *hit = NULL;

        cout << " Doublets: " << ista << " " << ndoubl << endl;
        for (Int_t id = 0; id < ndoubl; ++id)
        {
            Int_t indx1 = fHit2d[ista][lay2][id].first;
            Int_t indx2 = fHit2d[ista][lay2][id].second;
            if (indx1 >= 0)
            {
                hit = (HFwDetStrawCalSim*) fHits->getObject(indx1);
                fUz[lay2*2][0] = hit->getX();
                //fUz[lay2*2][2] = hit->GetPhi();
                //fUzi[lay2*2][0] = hit->GetSegment();
                fUzi[lay2*2][0] = 0;
                fUzi[lay2*2][1] = indx1;
                fUzi[lay2*2][2] = hit->getTube();
            }
            if (indx2 >= 0)
            {
                hit = (HFwDetStrawCalSim*) fHits->getObject(indx2);
                fUz[lay2*2+1][0] = hit->getX();
                //fUz[lay2*2+1][2] = hit->GetPhi();
                //fUzi[lay2*2+1][0] = hit->GetSegment();
                fUzi[lay2*2+1][0] = 0;
                fUzi[lay2*2+1][1] = indx2;
                fUzi[lay2*2+1][2] = hit->getTube();
            }
            Bool_t ind1 = indx1 + 1;
            Bool_t ind2 = indx2 + 1;
            patt = ind1;
            patt |= (ind2 << 1);
            //cout << plane0 << " " << hit->GetX() << " " << hit->GetY() << " " << hit->get() << " " << hit->GetU() << " " << u << endl;
            //processDouble(ista, lay2+1, patt, flag, hit->GetTube(), hit->GetSegment());
            processDouble(ista, lay2+1, patt, flag, hit->getTube(), 0);
        }
    }
}
// -------------------------------------------------------------------------

// -----   Private method processDouble   ----------------------------------
void HFwDetStrawVectorFinder::processDouble(Int_t ista, Int_t lay2, Int_t patt, Int_t flag,
                        Int_t tube0, Int_t segment0)
{
    // Main processing engine (recursively adds doublet hits to the vector)

    // !!! Tube differences between the same views for mu from omega at 8 GeV !!!
    const Int_t dTubes2 = 30;
    Double_t pars[4] = {0.0};

    Int_t ndoubl = fHit2d[ista][lay2].size();

    for (Int_t id = 0; id < ndoubl; ++id)
    {
        Int_t indx1 = fHit2d[ista][lay2][id].first;
        Int_t indx2 = fHit2d[ista][lay2][id].second;
        HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(TMath::Max(indx1,indx2));
        //Int_t segment = hit->GetSegment();
        Int_t segment = 0;
        Int_t tube = hit->getTube();

        //if (TMath::Abs(tube - tube0) > fDtubes[ista][lay2-1]) continue; // !!! cut

        // Check the same views
        Int_t ok = 1;
        for (Int_t il = 0; il < lay2; ++il)
        {
            Int_t pl = il * 2;
            if (TMath::Abs(fSina[pl+ista*fgkPlanes]-fSina[lay2*2+ista*fgkPlanes]) < 0.01)
            {
                // The same views
                Int_t seg = fUzi[pl][0];
                if (!(patt & (1 << pl))) seg = fUzi[pl+1][0];
                if (segment != seg)
                {
                    ok = 0;
                    break;
                }

                // Check tube difference
                Double_t z = fDz[pl%fgkPlanes];
                Int_t tu = fUzi[pl][2];
                if (!(patt & (1 << pl)))
                {
                    z = fDz[pl%fgkPlanes+1];
                    tu = fUzi[pl+1][2];
                }
                z += fZ0[ista];
                //Double_t dzz = (hit->get() - z) / z;
                //Int_t hitPlane = hit->getStation() * fgkPlanes + hit->getDoublet() * 2 + hit->getLayer();
                Int_t hitPlane = hit->getLayer() * 2 + hit->getPlane();
                Double_t zHit = fDz[hitPlane] + fZ0[ista];
                Double_t dzz = (zHit - z) / z;
                if (TMath::Abs(tube - tu - dzz * tu) > dTubes2)
                {
                    // !!! cut
                    ok = 0;
                    //cout << " !!! dTube cut " << tube - tu - dzz * tu << endl;
                    break;
                }
            }
        }
        if (!ok) continue; // !!! cut

        /*
        if (lay2 > 1) {
        // Tube difference with previous to previous doublet
        Int_t pl = (lay2 - 2) * 2;
        Int_t tu = fUzi[pl][2];
        if (patt & (1 << pl+1)) tu = fUzi[pl+1][2];
        Double_t dtu = tube - tu;
        if (lay2 == 3) dtu -= slope[ista] * tu;
        //if (TMath::Abs(dtu) > dTubes2[lay2-2]) continue; // !!! cut
        }
        */

        if (indx1 >= 0)
        {
            hit = (HFwDetStrawCalSim*) fHits->getObject(indx1);
            fUz[lay2*2][0] = hit->getX();
            //fUz[lay2*2][2] = hit->GetPhi();
            //fUzi[lay2*2][0] = hit->GetSegment();
            fUzi[lay2*2][0] = 0;
            fUzi[lay2*2][1] = indx1;
            fUzi[lay2*2][2] = hit->getTube();
        }
        if (indx2 >= 0)
        {
            hit = (HFwDetStrawCalSim*) fHits->getObject(indx2);
            fUz[lay2*2+1][0] = hit->getX();
            //fUz[lay2*2+1][2] = hit->GetPhi();
            //fUzi[lay2*2+1][0] = hit->GetSegment();
            fUzi[lay2*2+1][0] = 0;
            fUzi[lay2*2+1][1] = indx2;
            fUzi[lay2*2+1][2] = hit->getTube();
        }

        Bool_t ind1 = indx1 + 1;
        Bool_t ind2 = indx2 + 1;
        // Clear bits
        patt &= ~(1 << lay2*2);
        patt &= ~(1 << (lay2*2+1));
        // Set bits
        patt |= (ind1 << lay2*2);
        patt |= (ind2 << (lay2*2+1));

        //cout << patt << " " << hit->GetX() << " " << hit->GetY() << " " << hit->get() << " " << hit->GetU() << " " << endl;

        if (lay2 + 1 < fgkPlanes / 2)
        {
            processDouble(ista, lay2+1, patt, flag, tube, segment);
        }
        else
        {
            // Straight line fit of the vector
            Int_t patt1 = patt << ista*fgkPlanes;
            findLine(patt1, pars);
            Double_t chi2 = findChi2(ista, patt1, pars);
            //cout << " *** Stat: " << ista << " " << id << " " << indx1 << " " << indx2 << " " << chi2 << " " << pars[0] << " " << pars[1] << endl;
            if (chi2 <= fCutChi2) addVector(ista, patt1, chi2, pars); // add vector to the temporary container
        }
    } // for (id = 0; id < ndoubl;
}
// -------------------------------------------------------------------------

// -----   Private method addVector   --------------------------------------
void HFwDetStrawVectorFinder::addVector(Int_t ista, Int_t patt, Double_t chi2, Double_t *pars, Bool_t lowRes)
{
    // Add vector to the temporary container (as HFwDetStrawVector)

    HFwDetStrawVector *track = new HFwDetStrawVector();
    track->setChi2(chi2);
    track->setParams(pars);
    track->setZ(fZ0[ista]);
    TMatrixDSym cov(*fMatr[patt]);
    cov *= (fErrU * fErrU);
    track->setCovar(cov);

    if (patt > (1 << fgkPlanes)) patt = patt >> fgkPlanes; // second station
    for (Int_t ipl = 0; ipl < fgkPlanes; ++ipl)
    {
        if (!(patt & (1 << ipl))) continue;
        track->addHit(fUzi[ipl][1]);
        if (lowRes) continue;

        // Store info about left-right ambig. resolving (set true if positive drift direction)
        HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(fUzi[ipl][1]);
        if (hit->getX() < fUz[ipl][0])
            track->setLRbit(track->getNofHits()-1);
    }
    Int_t ndf = (track->getNofHits() > 5) ? track->getNofHits() - 4 : 1;
    track->setNDF(ndf);
    setTrackId(track); // set track ID as its flag

    if (lowRes)
        fVectors[ista].push_back(track);
    else
        fVectorsHigh[ista].push_back(track);
}
// -------------------------------------------------------------------------

// -----   Private method SetTrackId   -------------------------------------
void HFwDetStrawVectorFinder::setTrackId(HFwDetStrawVector *vec)
{
    // Set vector ID as its flag (maximum track ID of its poins)

//     map<Int_t,Int_t> ids;
//     Int_t nhits = vec->getNofHits(), id = 0;

    // FIXME all!
//     for (Int_t ih = 0; ih < nhits; ++ih)
//     {
//         HFwDetStrawCal *hit = (HFwDetStrawCal*) pHits->getObject(vec->getHitIndex(ih));
// 
//         for (multimap<Double_t,Int_t>::iterator mit = hit->getDriftId().begin();
//         mit != hit->getDriftId().end(); ++mit)
//         {
//             id = mit->second;
//             if (ids.find(id) == ids.end()) ids.insert(pair<Int_t,Int_t>(id,1));
//             else ++ids[id];
//         }
//     }
// 
    Int_t maxim = -1, idmax = -9;
//     for (map<Int_t,Int_t>::iterator it = ids.begin(); it != ids.end(); ++it)
//     {
//         if (it->second > maxim)
//         {
//             maxim = it->second;
//             idmax = it->first;
//         }
//     }
    // Set vector ID as its flag
    vec->setFlag(idmax);
}
// -------------------------------------------------------------------------

// -----   Private method findLine   ---------------------------------------
void HFwDetStrawVectorFinder::findLine(Int_t patt, Double_t *pars)
{
    // Fit of hits to the straight line

    // Solve system of linear equations
    Bool_t ok = kFALSE, onoff;
    TVectorD b(4);
    for (Int_t i = 0; i < fgkPlanes2; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;
        Int_t i1 = i % fgkPlanes;
        b[0] += fUz[i1][0] * fCosa[i];
        b[1] += fUz[i1][0] * fSina[i];
        b[2] += fUz[i1][0] * fDz[i%fgkPlanes] * fCosa[i];
        b[3] += fUz[i1][0] * fDz[i%fgkPlanes] * fSina[i];
    }

    //lu.Print();
    TVectorD solve = fLus[patt]->Solve(b, ok);
    //TVectorD solve = lu.TransSolve(b, ok);
    //lu.Print();
    for (Int_t i = 0; i < 4; ++i) pars[i] = solve[i];
    //for (Int_t i = 0; i < 4; ++i) { cout << pars[i] << " "; if (i == 3) cout << endl; }
    //Double_t y1 = cosa / sina * (uz[1][0] * cosa - uz[0][0]) + uz[1][0] * sina;
    //Double_t y2 = -cosa / sina * (uz[2][0] * cosa - uz[0][0]) - uz[2][0] * sina;
    //cout << " OK " << ok << " " << y1 << " " << y2 << endl;
}
// -------------------------------------------------------------------------

// -----   Private method findChi2   ---------------------------------------
Double_t HFwDetStrawVectorFinder::findChi2(Int_t ista, Int_t patt, Double_t *pars)
{
    // Compute chi2 of the fit

    Double_t chi2 = 0, x = 0, y = 0, u = 0, errv = 1.;
    if (fErrU > 0.1) errv = 10;

    Bool_t onoff;
    for (Int_t i = 0; i < fgkPlanes2; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;
        x = pars[0] + pars[2] * fDz[i%fgkPlanes];
        y = pars[1] + pars[3] * fDz[i%fgkPlanes];
        u = x * fCosa[i] + y * fSina[i];
        Int_t i1 = i % fgkPlanes;
        Double_t du = (u - fUz[i1][0]) / fErrU;
        chi2 += du * du;
    }

    return chi2;
}
// -------------------------------------------------------------------------

// -----   Private method CheckParams   ------------------------------------
void HFwDetStrawVectorFinder::checkParams()
{
    // Remove vectors with wrong orientation
    // using empirical cuts for omega muons at 8 Gev

    Double_t cut[2] = {0.6, 0.7}; // !!! empirical !!!
    Double_t pars[4];

    for (Int_t ista = 0; ista < fgkStat; ++ista)
    {
        Int_t nvec = fVectors[ista].size();

        //for (Int_t iv = 0; iv < nvec; ++iv) {
        for (Int_t iv = fIndx0[ista]; iv < nvec; ++iv)
        {
            HFwDetStrawVector *vec = fVectors[ista][iv];
            vec->params(pars);
            Double_t x = pars[0], y = pars[1], tx = pars[2], ty = pars[3], z = vec->getZ();
            Double_t dTx = tx - x / z;
            Double_t dTy = ty - y / z;
            if (TMath::Abs(dTx) > cut[0] || TMath::Abs(dTy) > cut[1]) vec->setChi2(-1.0);
        }

        //for (Int_t iv = nvec-1; iv >= 0; --iv) {
        for (Int_t iv = nvec-1; iv >= fIndx0[ista]; --iv)
        {
            HFwDetStrawVector *vec = fVectors[ista][iv];
            if (vec->getChi2() < 0) {
            delete fVectors[ista][iv];
            fVectors[ista].erase(fVectors[ista].begin()+iv);
        }
        }
        cout << " Vectors after parameter check (ista, inp, out): " << ista << " " << nvec << " " << fVectors[ista].size() << endl;
    }
}
// -------------------------------------------------------------------------

// -----   Private method HighRes   ----------------------------------------
void HFwDetStrawVectorFinder::highRes()
{
    // High resolution processing (resolve ghost hits and make high resolution vectors)

    for (Int_t ista = 0; ista < fgkStat; ++ista)
    {
        Int_t nvec = fVectors[ista].size();

        for (Int_t iv = fIndx0[ista]; iv < nvec; ++iv)
        {
            HFwDetStrawVector *vec = fVectors[ista][iv];
            Int_t nhits = vec->getNofHits(), patt = 0;
            Double_t uu[fgkPlanes][2];

            for (Int_t ih = 0; ih < nhits; ++ih)
            {
                HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(vec->getHitIndex(ih));
                Int_t lay = hit->getLayer();
                Int_t side = hit->getPlane();
                Int_t plane = lay*2 + side;
                uu[plane][0] = hit->getX();
                uu[plane][1] = hit->getDrift(); // drift distance with error
                patt |= (1 << plane);
                //fUzi[plane][0] = hit->GetSegment();
                fUzi[plane][0] = 0;
                fUzi[plane][1] = vec->getHitIndex(ih);
            }

            // Number of hit combinations is 2
            // - left-right ambiguity resolution does not really work for doublets
            Int_t nCombs = (patt & 1) ? 2 : 1, flag = 1, nok = nCombs - 1;

            //cout << " Doublet ID: " << vec->getFlag() << endl;
            for (Int_t icomb = -1; icomb < nCombs; icomb += 2)
            {
                fUz[0][0] = (nCombs == 2) ? uu[0][0] + uu[0][1] * icomb : 0.0;
                processSingleHigh(ista, 1, patt, flag, nok, uu);
            }
        } // for (Int_t iv = 0;
    } // for (Int_t ista = 0;

    moveVectors(); // move vectors from one container to another, i.e. drop low resolution ones
}
// -------------------------------------------------------------------------

// -----   Private method processDoubleHigh   ------------------------------
void HFwDetStrawVectorFinder::processSingleHigh(Int_t ista, Int_t plane, Int_t patt, Int_t flag,
                        Int_t nok, Double_t uu[fgkPlanes][2])
{
    // Main processing engine for high resolution mode
    // (recursively adds singlet hits to the vector)

    Double_t pars[4] = {0.0};

    // Number of hit combinations is 2
    Int_t nCombs = (patt & (1 << plane)) ? 2 : 1;
    nok += (nCombs - 1);

    for (Int_t icomb = -1; icomb < nCombs; icomb += 2)
    {
        fUz[plane][0] = (nCombs == 2) ? uu[plane][0] + uu[plane][1] * icomb : 0.0;
        if (plane == fgkPlanes - 1 || (nok == fMinHits && flag))
        {
            // Straight line fit of the vector
            Int_t patt1 = patt & ((1 << (plane + 1)) - 1); // apply mask
            patt1 = patt1 << ista*fgkPlanes;
            findLine(patt1, pars);
            Double_t chi2 = findChi2(ista, patt1, pars);
            //cout << " *** StatHigh: " << ista << " " << plane << " " << chi2 << " " << pars[0] << " " << pars[1] << endl;
            if (icomb > -1) flag = 0;
            if (chi2 > fCutChi2) continue; // too high chi2 - do not extend line
            //cout << " *** Stat: " << ista << " " << plane << " " << chi2 << " " << pars[0] << " " << pars[1] << endl;
            //if (plane + 1 < fgkPlanes) ProcessSingleHigh(ista, plane + 1, patt, flag, nok, uu);
            if (plane + 1 < fgkPlanes)
                processSingleHigh(ista, plane + 1, patt, 0, nok, uu);
            else
                addVector(ista, patt, chi2, pars, kFALSE); // add vector to the temporary container
        }
        else
        {
            processSingleHigh(ista, plane + 1, patt, flag, nok, uu);
        }
    }
}
// -------------------------------------------------------------------------

// -----   Private method MoveVectors   ------------------------------------
void HFwDetStrawVectorFinder::moveVectors()
{
    // Drop low-resolution vectors and move high-res. ones to their container

    for (Int_t ista = 0; ista < fgkStat; ++ista)
    {
        Int_t nVecs = fVectors[ista].size();
        //for (Int_t j = 0; j < nVecs; ++j) delete fVectors[ista][j];
        //fVectors[ista].clear();
        //for (Int_t j = fIndx0[ista]; j < nVecs; ++j) {
        for (Int_t j = nVecs - 1; j >= fIndx0[ista]; --j)
        {
            delete fVectors[ista][j];
            fVectors[ista].erase(fVectors[ista].begin()+j);
        }

        nVecs = fVectorsHigh[ista].size();
        for (Int_t j = 0; j < nVecs; ++j) fVectors[ista].push_back(fVectorsHigh[ista][j]);
        fVectorsHigh[ista].clear(); //
    }
}
// -------------------------------------------------------------------------

// -----   Private method RemoveClones   -----------------------------------
void HFwDetStrawVectorFinder::removeClones()
{
    // Remove clone vectors (having at least 1 the same hit in each doublet (min. 4 the same hits))

    //Int_t nthr = 4, planes[20];
    Int_t nthr = 3, planes[20];

    for (Int_t ista = 0; ista < fgkStat; ++ista)
    {
        Int_t nvec = fVectors[ista].size();

        // Do sorting according to "quality"
        multimap<Double_t,HFwDetStrawVector*> qMap;
        multimap<Double_t,HFwDetStrawVector*>::iterator it, it1;

        for (Int_t iv = 0; iv < nvec; ++iv)
        {
            HFwDetStrawVector *vec = fVectors[ista][iv];
            Double_t qual = vec->getNofHits() + (99 - TMath::Min(Double_t(vec->getChi2()),99.0)) / 100;
            qMap.insert(pair<Double_t,HFwDetStrawVector*>(-qual,vec));
        }

        for (it = qMap.begin(); it != qMap.end(); ++it)
        {
            HFwDetStrawVector *vec = it->second;
            if (vec->getChi2() < 0) continue;
            for (Int_t j = 0; j < fgkPlanes; ++j) planes[j] = -1;

            Int_t nhits = vec->getNofHits();
            for (Int_t ih = 0; ih < nhits; ++ih)
            {
                HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(vec->getHitIndex(ih));
                Int_t lay = hit->getLayer();
                Int_t side = hit->getPlane();
                planes[lay*2+side] = vec->getHitIndex(ih);
                //cout << iv << " " << lay*2+side << " " << vec->getHitIndex(ih) << endl;
            }

            it1 = it;
            for (++it1; it1 != qMap.end(); ++it1)
            {
                HFwDetStrawVector *vec1 = it1->second;
                if (vec1->getChi2() < 0) continue;
                Int_t nsame = 0, same[fgkPlanes/2] = {0};

                Int_t nhits1 = vec1->getNofHits();
                //nthr = TMath::Min(nhits,nhits1) / 2;
                //nthr = TMath::Min(nhits,nhits1) * 0.75;
                for (Int_t ih = 0; ih < nhits1; ++ih)
                {
                    HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(vec1->getHitIndex(ih));
                    Int_t lay = hit->getLayer();
                    Int_t side = hit->getPlane();
                    if (planes[lay*2+side] >= 0)
                    {
                        if (vec1->getHitIndex(ih) == planes[lay*2+side]) same[lay] = 1;
                        //else same[lay] = 0;
                    }
                }

                for (Int_t lay = 0; lay < fgkPlanes/2; ++lay)
                {
                    nsame += same[lay];
                }

                if (nsame >= nthr)
                {
                    // Too many the same hits
                    Int_t clone = 0;
                    if (nhits > nhits1 + 0) clone = 1;
                    //else if (vec->GetChiSq() * 3.0 <= vec1->GetChiSq()) vec1->setChiSq(-1.0); // the same number of hits on 2 tracks
                    //else if (fgkPlanes > -8 && vec->getChi2() * 1 <= vec1->getChi2()) clone = 1; // the same number of hits on 2 tracks
                    else if (fgkPlanes > -8 && vec->getChi2() * 1 <= vec1->getChi2()) clone = 1; // the same number of hits on 2 tracks
                    if (clone)
                    {
                        vec1->setChi2(-1.0);
                    }
                }
            }
        }

        for (Int_t iv = nvec-1; iv >= 0; --iv)
        {
            HFwDetStrawVector *vec = fVectors[ista][iv];
            if (vec->getChi2() < 0)
            {
                   delete fVectors[ista][iv];
                fVectors[ista].erase(fVectors[ista].begin()+iv);
            }
        }
        cout << " Vectors after clones removed: " << ista << " " << nvec << " " << fVectors[ista].size() << endl;

    } // for (Int_t ista = 0; ista < fgkStat;
}
// -------------------------------------------------------------------------

// -----   Private method RemoveShorts   -----------------------------------
void HFwDetStrawVectorFinder::removeShorts()
{
    // Remove short tracks

    Int_t nshort = fgkPlanes / 2, planes[20];

    for (Int_t ista = 0; ista < fgkStat; ++ista)
    {
        Int_t nvec = fVectors[ista].size();
        Int_t remove = 0;

        for (Int_t iv = 0; iv < nvec; ++iv)
        {
            HFwDetStrawVector *vec = fVectors[ista][iv];
            if (vec->getChi2() < 0) continue;
            Int_t nhits = vec->getNofHits();
            if (nhits != nshort) continue;
            set<Int_t> overlap;
            //multiset<Int_t> overlap1;
            for (Int_t j = 0; j < fgkPlanes; ++j) planes[j] = -1;
            for (Int_t ih = 0; ih < nhits; ++ih)
            {
                HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(vec->getHitIndex(ih));
                Int_t lay = hit->getLayer();
                Int_t side = hit->getPlane();
                planes[lay*2+side] = vec->getHitIndex(ih);
            }

            for (Int_t iv1 = iv + 1; iv1 < nvec; ++iv1)
            {
                HFwDetStrawVector *vec1 = fVectors[ista][iv1];
                if (vec1->getChi2() < 0) continue;
                Int_t nhits1 = vec1->getNofHits();

                // Compare hits
                for (Int_t ih = 0; ih < nhits1; ++ih) {
                    HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(vec1->getHitIndex(ih));
                    Int_t lay = hit->getLayer();
                    Int_t side = hit->getPlane();
                    if (vec1->getHitIndex(ih) == planes[lay*2+side] && planes[lay*2+side] >= 0) overlap.insert(ih);
                }

                //if (overlap.size() == nshort) {
                if (overlap.size() > 0)
                {
                    // Hits are shared with other tracks
                    remove = 1;
                    break;
                }
            } // for (Int_t iv1 = iv + 1;
            if (remove) vec->setChi2(-1.0);
        } // for (Int_t iv = 0; iv < nvec;

        for (Int_t iv = nvec-1; iv >= 0; --iv)
        {
            HFwDetStrawVector *vec = fVectors[ista][iv];
            if (vec->getChi2() < 0)
            {
                delete fVectors[ista][iv];
                fVectors[ista].erase(fVectors[ista].begin()+iv);
            }
        }
        cout << " Vectors after shorts removed: " << ista << " " << nvec << " " << fVectors[ista].size() << endl;

    } // for (Int_t ista = 0;
}
// -------------------------------------------------------------------------

// -----   Private method StoreVectors   -----------------------------------
void HFwDetStrawVectorFinder::storeVectors(Int_t isel)
{
    // Store vectors into category

    Int_t ibeg[2] = {0, fgkStat}, iend[2] = {fgkStat-1, fgkStat};
    HLocation loc;
    loc.set(1,0);

    //for (Int_t ist = 0; ist <= fgkStat; ++ist) {
    for (Int_t ist = ibeg[isel]; ist <= iend[isel]; ++ist)
    {
        Int_t nvec = fVectors[ist].size();
        Int_t iv0 = (ist < fgkStat) ? fIndx0[ist] : 0;

        //for (Int_t iv = 0; iv < nvec; ++iv) {
        for (Int_t iv = iv0; iv < nvec; ++iv)
        {
            HFwDetStrawVector* cal = (HFwDetStrawVector*) fTrackArray->getNewSlot(loc);

            if (cal)
            {
            cal = new(cal) HFwDetStrawVector(*(fVectors[ist][iv]));
            cal->SetUniqueID(ist);
            }
        }
    }
}
// -------------------------------------------------------------------------

// -----   Private method MergeVectors   -----------------------------------
void HFwDetStrawVectorFinder::mergeVectors()
{
    // Merge vectors from 2 stations

    TMatrixD matr = TMatrixD(4,4);
    TMatrixD unit(TMatrixD::kUnit,matr);
    Double_t pars[4];
    Double_t zMed = (fZ0[0] + fZ0[1]) / 2;
    cout << fVectors[0].size() << " " << fVectors[1].size() << " " << fVectors[2].size() << endl;

    for (Int_t ista = 0; ista < 2; ++ista)
    {
        // Propagate vectors to the median plane
        Int_t nvec = fVectors[ista].size();

        //for (Int_t iv = 0; iv < nvec; ++iv) {
        for (Int_t iv = fIndx0[ista]; iv < nvec; ++iv)
        {
            HFwDetStrawVector *tr = fVectors[ista][iv];
            tr->params(pars);
            Double_t zbeg = tr->getZ();
            Double_t dz = zMed - zbeg;
            pars[0] += dz * pars[2];
            pars[1] += dz * pars[3];
            zbeg += dz;
            TMatrixDSym covd = tr->getCovarMatr();
            TMatrixD ff = unit;
            ff(2,0) = ff(3,1) = dz;
            TMatrixD cf(covd,TMatrixD::kMult,ff);
            TMatrixD fcf(ff,TMatrixD::kTransposeMult,cf);
            covd.SetMatrixArray(fcf.GetMatrixArray());
            covd.SetTol(0.1*covd.GetTol());
            covd.Invert(); // weight matrix
            tr->setParams(pars);
            tr->setZ(zbeg);
            tr->setCovar(covd);
        }
    } // for (Int_t ista = 0; ista < 2;

    // Merge vectors
    Int_t ista1 = 0, ista2 = 1, nvec1 = fVectors[ista1].size(), nvec2 = fVectors[ista2].size();
    Double_t pars1[4], pars2[4];

    for (Int_t iv1 = fIndx0[ista1]; iv1 < nvec1; ++iv1)
    {
        HFwDetStrawVector *tr1 = fVectors[ista1][iv1];
        tr1->params(pars1);
        TMatrixDSym w1 = tr1->getCovarMatr();
        TMatrixD p1(4, 1, pars1);
        TMatrixD wp1(w1, TMatrixD::kTransposeMult, p1);

        for (Int_t iv2 = fIndx0[ista2]; iv2 < nvec2; ++iv2)
        {
            HFwDetStrawVector *tr2 = fVectors[ista2][iv2];
            tr2->params(pars2);
            TMatrixDSym w2 = tr2->getCovarMatr(), w20 = w2;
            TMatrixD p2(4, 1, pars2);
            TMatrixD wp2(w2, TMatrixD::kTransposeMult, p2);
            wp2 += wp1;
            w2 += w1;
            w2.Invert();
            TMatrixD p12(w2, TMatrixD::kTransposeMult, wp2);

            // Compute Chi2
            TMatrixD p122 = p12;
            TMatrixD pMerge = p12;
            p12 -= p1;
            TMatrixD wDp1(w1, TMatrixD::kMult, p12);
            TMatrixD chi21(p12, TMatrixD::kTransposeMult, wDp1);
            p122 -= p2;
            TMatrixD wDp2(w20, TMatrixD::kMult, p122);
            TMatrixD chi22(p122, TMatrixD::kTransposeMult, wDp2);
            Double_t c2 = chi21(0,0) + chi22(0,0);
            Info("MergeVectors"," Chi2: %f %f %f %d %d",
                 chi21(0,0), chi22(0,0), c2, tr1->getFlag(), tr2->getFlag());
            if (c2 < 0 || c2 > fCutChi2 * 2) continue;
            // Merged track parameters
            pars[0] = pMerge(0,0);
            pars[1] = pMerge(1,0);
            //parOk.SetZ(par2.GetZ());
            pars[2] = pMerge(2,0);
            pars[3] = pMerge(3,0);
            //parOk.SetCovMatrix(w2);
            addTrack(ista1, tr1, tr2, iv1, iv2, pars, c2, w2); // add track
        } // for (Int_t iv2 = 0;
    } // for (Int_t iv1 = 0;
    //RemoveClonesHades(ibeg, mapMerged);
    cout << fVectors[0].size() << " " << fVectors[1].size() << " " << fVectors[2].size() << endl;
}
// -------------------------------------------------------------------------

// -----   Private method AddTrack   ---------------------------------------
void HFwDetStrawVectorFinder::addTrack(Int_t ista0, HFwDetStrawVector *tr1, HFwDetStrawVector *tr2, Int_t indx1, Int_t indx2, Double_t *parOk, Double_t c2, TMatrixDSym &w2)
{
    // Save merged vector

    HFwDetStrawVector *track = new HFwDetStrawVector();
    track->setParams(parOk);
    track->setChi2(c2+tr1->getChi2() + tr2->getChi2());
    track->setNDF(4+tr1->getNDF() + tr2->getNDF());
    track->addHit(indx1); // first vector index
    track->addHit(indx2); // second vector index
    if (tr1->getFlag() == tr2->getFlag()) track->setFlag(tr1->getFlag());
    else track->setFlag(-1);
    fVectors[fgkStat].push_back(track);

    Info("AddTrack", "trID1=%i, trID2=%i, chi2=%f, merged vectors %i",
         tr1->getFlag(),tr2->getFlag(),track->getChi2(),track->getNofHits());
}
// -------------------------------------------------------------------------

// -----   Private method SelectTracks   -----------------------------------
void HFwDetStrawVectorFinder::selectTracks(Int_t ipass)
{
    // Remove ghost tracks (having at least N the same hits (i.e. fired tubes))

    const Int_t nMax = 8, nPl = 40, nVecMin = 2;
    Int_t planes[nPl], hinds[nPl], lr[nPl], ntrs = fVectors[fgkStat].size();
    multimap<Double_t,Int_t> c2merge;

    //for (Int_t i = (ipass == 0) ? 0 : 6; i < ntrs; ++i) { // for test
    for (Int_t i = 0; i < ntrs; ++i)
    {
        HFwDetStrawVector *tr = fVectors[fgkStat][i];
        if (tr->getNofHits() != nVecMin) continue;
        Double_t qual = tr->getNofHits() +
        (499 - TMath::Min(tr->getChi2()/tr->getNDF(),499.0)) / 500;
        c2merge.insert(pair<Double_t,Int_t>(-qual,i));
    }

    Int_t nSel = 0;
    multimap<Double_t,Int_t>::iterator it, it1;
    for (it = c2merge.begin(); it != c2merge.end(); ++it)
    {
        HFwDetStrawVector *tr1 = fVectors[fgkStat][it->second];
        if (tr1 == NULL) continue;
        Int_t nvecs1 = tr1->getNofHits();
        for (Int_t j = 0; j < nPl; ++j) planes[j] = hinds[j] = lr[j] = -1;
        Int_t patt = 0;

        for (Int_t iv = 0; iv < nvecs1; ++iv)
        {
            HFwDetStrawVector *vec = fVectors[iv][tr1->getHitIndex(iv)];
            Int_t nh = vec->getNofHits();
            for (Int_t ih = 0; ih < nh; ++ih)
            {
                HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(vec->getHitIndex(ih));
                Int_t ipl = hit->getPlane();
                planes[ipl] = vec->getHitIndex(ih);
                patt |= (1 << ipl);
                hinds[ipl] = vec->getHitIndex(ih);
                if (vec->getLR().TestBitNumber(ih)) lr[ipl] = 1;
            }
        }

        it1 = it;
        for (++it1; it1 != c2merge.end(); ++it1)
        {
            HFwDetStrawVector *tr2 = fVectors[fgkStat][it1->second];
            if (tr2 == NULL) continue;
            //if (tr2->GetUniqueID() != tr1->GetUniqueID()) continue;
            Int_t nvecs2 = tr2->getNofHits(), nover = 0;
            Bool_t over = kFALSE;

            for (Int_t iv = 0; iv < nvecs2; ++iv)
            {
                HFwDetStrawVector *vec = fVectors[iv][tr2->getHitIndex(iv)];
                Int_t nh = vec->getNofHits();

                for (Int_t ih = 0; ih < nh; ++ih)
                {
                    HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(vec->getHitIndex(ih));
                    Int_t ipl = hit->getPlane();
                    if (planes[ipl] < 0) continue;
                    if (planes[ipl] == vec->getHitIndex(ih))
                    {
                        ++nover;
                        if (nover >= nMax)
                        {
                            //cout << ipl << " " << vec->getHitIndex(ih) << endl;
                            Info("Select tracks", "Track quality: qual1 %f, qual2 %f, trID1 %i, trID2 %i, overlaps: %i",
                            it->first,it1->first,tr1->getFlag(),tr2->getFlag(),nover);
                            delete fVectors[fgkStat][it1->second];
                            fVectors[fgkStat][it1->second] = NULL;
                            over = kTRUE;
                            break;
                        }
                    }
                }
                if (over) break;
            } // for (Int_t iv = 0; iv < nvecs2;
        } // for (++it1; it1 != c2merge.end();

        // Refit whole line
        Double_t pars[4], chi2;
        TMatrixDSym cov(4);
        chi2 = refit(patt, hinds, pars, &cov, lr);
        cov *= (fErrU * fErrU);
        Info("Select tracks", "Track chi2: %f",chi2);
        tr1->setParams(pars);
        tr1->setCovar(cov);
        tr1->setZ(fZ0[0]);
        ++nSel;
    } // for (++it; it != c2merge.end();

    // Apply extra slection cuts
    if (ipass < fNpass - 1 && nSel > 1)
    {
        for (Int_t i = 0; i < ntrs; ++i)
        {
            HFwDetStrawVector *tr = fVectors[fgkStat][i];
            if (tr == NULL) continue;
            if (tr->getChi2() / tr->getNDF() <= 3.0)
            {
                // Flag used hits
                Int_t nvecs1 = tr->getNofHits();
                for (Int_t iv = 0; iv < nvecs1; ++iv)
                {
                    HFwDetStrawVector *vec = fVectors[iv][tr->getHitIndex(iv)];
                    Int_t nh = vec->getNofHits();
                    for (Int_t ih = 0; ih < nh; ++ih)
                    {
                        HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(vec->getHitIndex(ih));
                        hit->SetUniqueID(999);
                    }
                }
                continue;
            }
            else
            {
                delete fVectors[fgkStat][i];
                fVectors[fgkStat][i] = NULL;
            }
        }
    }

    vector<HFwDetStrawVector*>::iterator vit = fVectors[fgkStat].begin();
    while (vit != fVectors[fgkStat].end())
    {
        if (*vit == NULL) vit = fVectors[fgkStat].erase(vit);
        else ++vit;
    }
}
// -------------------------------------------------------------------------

// -----   Private method Refit   ------------------------------------------
Double_t HFwDetStrawVectorFinder::refit(Int_t patt, Int_t *hinds, Double_t *pars, TMatrixDSym *cov, Int_t *lr)
{
    // Fit of hits to the straight line

    // Solve system of linear equations
    Bool_t ok = kFALSE, onoff;
    TVectorD b(4);

    for (Int_t i = 0; i < fgkPlanes2; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;
        HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(hinds[i]);
        Double_t uc = (hit->getX() + lr[i] * hit->getDrift()) * fCosa[i]; // resolved ambiguity !!!
        Double_t us = (hit->getX() + lr[i] * hit->getDrift()) * fSina[i]; // resolved ambiguity !!!
        b[0] += uc;
        b[1] += us;
        b[2] += uc * fDz[i];
        b[3] += us * fDz[i];
    }

    //lu.Print();
    TVectorD solve = fLus[patt]->Solve(b, ok);

    for (Int_t i = 0; i < 4; ++i)
        pars[i] = solve[i];

    cov->SetMatrixArray(fMatr[patt]->GetMatrixArray());

    // Compute chi2
    Double_t chi2 = 0;
    for (Int_t i = 0; i < fgkPlanes2; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;
        Double_t x = pars[0] + pars[2] * fDz[i];
        Double_t y = pars[1] + pars[3] * fDz[i];
        Double_t u = x * fCosa[i] + y * fSina[i];
        HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) fHits->getObject(hinds[i]);
        Double_t du = (u - hit->getX() - lr[i] * hit->getDrift()) / fErrU;
        chi2 += du * du;
    }
    return chi2;
}

// -------------------------------------------------------------------------

ClassImp(HFwDetStrawVectorFinder);
