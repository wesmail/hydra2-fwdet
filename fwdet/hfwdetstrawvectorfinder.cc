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
#include "hfwdetstrawvectorfinderpar.h"
#include "fwdetdef.h"
#include "hades.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hevent.h"
#include "hfwdetdetector.h"
#include "hfwdetgeompar.h"
#include "hfwdetstrawgeompar.h"
#include "hfwdetstrawdigitizer.h"
#include "hfwdetstrawdigipar.h"
#include "hfwdetstrawcal.h"
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

// #define VERBOSE_MODE 1

// -----   Default constructor   -------------------------------------------
HFwDetStrawVectorFinder::HFwDetStrawVectorFinder() : HReconstructor()
{
    initVariables();
}

// -----   Constructor   ---------------------------------------------------
HFwDetStrawVectorFinder::HFwDetStrawVectorFinder(const Text_t *name, const Text_t *title) :
    HReconstructor(name, title)
{
    initVariables();
}

// -----   Destructor   ----------------------------------------------------
HFwDetStrawVectorFinder::~HFwDetStrawVectorFinder()
{
    for (Int_t i = 0; i <= FWDET_STRAW_MAX_MODULES; ++i)
    {
        Int_t nVecs = fVectors[i].size();
        for (Int_t j = 0; j < nVecs; ++j) delete fVectors[i][j];
    }
    for (map<Int_t,TDecompLU*>::iterator it = fLus.begin(); it != fLus.end(); ++it)
    {
        delete it->second;
    }
}

// -----   Public method initVariables  ------------------------------------
void HFwDetStrawVectorFinder::initVariables()
{
    isSimulation = kFALSE;
    pKine = pHits = pTrackArray = NULL;
    pStrawVFPar = NULL;

    fMinHits = 10;// FIXME to aparams
    nModules = 0;
    nVplanes = 0;
    nVplanesH = 0;

    for (Int_t i = 0; i < FWDET_STRAW_MAX_MODULES; ++i)
    {
        fIndx0[i] = 0;
    }
}

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
    pKine = gHades->getCurrentEvent()->getCategory(catGeantKine); 
    if (!pKine) 
    { 
        isSimulation = kTRUE;
    }

    // hits
    pHits = gHades->getCurrentEvent()->getCategory(catFwDetStrawCal);
    if (!pHits)
    {
        Error("HFwDetStrawVectorFinder::init()","HFwDetStrawCal input missing");
        return kFALSE;
    }

    // build the Straw vector category
    pTrackArray = new HLinearCategory("HFwDetStrawVector");
    if (!pTrackArray)
    {
        Error("HFwDetStrawVectorFinder::init()","Straw vector category not created");
        return kFALSE;
    }
    else
    {
        gHades->getCurrentEvent()->addCategory(catFwDetStrawVector, pTrackArray, "FwDet");
    }

    pStrawVFPar = (HFwDetStrawVectorFinderPar *)
        gHades->getRuntimeDb()->getContainer ("FwDetStrawVectorFinderPar");
    if (!pStrawVFPar)
    {
        Error("HFwDetStrawVectorFinder::init()","Parameter container for vector finder parameters not created");
        return kFALSE;
    }

    // create the parameter container
    pStrawGeomPar = (HFwDetStrawGeomPar *)gHades->getRuntimeDb()->getContainer("FwDetStrawGeomPar");
    if (!pStrawGeomPar)
    {
        Error("HFwDetStrawDigitizer::init()","Parameter container for geometry not created");
        return kFALSE;
    }

    return kTRUE;
}

// -----   Public method reinit  -------------------------------------------
Bool_t HFwDetStrawVectorFinder::reinit()
{
    // Get geometry parameters from database

    fLRCutChi2 = pStrawVFPar->getLRCutChi2();
    fHRCutChi2 = pStrawVFPar->getHRCutChi2();

    fLRErrU = pStrawVFPar->getLRErrU() / TMath::Sqrt(12.0);
    fHRErrU = pStrawVFPar->getLRErrU();

    fErrU = fLRErrU;

    fNpass = pStrawVFPar->getNpass();

    Int_t plane = 0;
    nModules = pStrawGeomPar->getModules();

    for (Int_t m = 0; m < nModules; ++m)
    {
        nLayers[m] = pStrawGeomPar->getLayers(m);
        // Loop over layers
        for (Int_t l = 0; l < nLayers[m]; ++l)
        {
            Float_t a = pStrawGeomPar->getLayerRotation(m, l) * TMath::DegToRad();

            for (Int_t p = 0; p < FWDET_STRAW_MAX_PLANES; ++p)
            {
                fDz[plane] = pStrawGeomPar->getOffsetZ(m, l, p);
                fCosa[plane] = cos(a);
                fSina[plane] = sin(a);
                ++plane;
            }
            ++nVplanesH;

            if (l == 0)
                fZ0[m] = pStrawGeomPar->getOffsetZ(m, l, 0);
        }
    }

    nVplanes = plane;

    for (Int_t i = nVplanes - 1; i >= 0; --i)
    {
        fDz[i] -= fDz[0];
    }

    computeMatrix(); // compute system matrices
    return kTRUE;
}

// -----   Public method execute   -----------------------------------------
Int_t HFwDetStrawVectorFinder::execute()
{
    //gErrorIgnoreLevel = kInfo; //debug level
    gErrorIgnoreLevel = kWarning; //debug level

    for (Int_t i = 0; i <= nModules; ++i)
    {
        Int_t nVecs = fVectors[i].size();

        for (Int_t j = 0; j < nVecs; ++j)
            delete fVectors[i][j];

        fVectors[i].clear();

        if (i < nModules)
        {
            fVectorsHigh[i].clear();
            fIndx0[i] = 0;
        }
    }

    for (Int_t ipass = 0; ipass < fNpass; ++ipass)
    {
        fErrU = fLRErrU;

        getHits();
        makeVectors();

        // Remove vectors with wrong orientation
        // (using empirical cuts for omega muons at 8 GeV)
        checkParams();

        // Go to the high resolution mode processing
        fErrU = fHRErrU;
        if (fHRErrU > 0.0) highRes();

        // Store vectors
        storeVectors(0);

        // Merge vectors
        mergeVectors();

        // Select final tracks - remove ghosts
        selectTracks(ipass);

        if (ipass == fNpass - 1) continue;
        for (Int_t i = 0; i < nModules; ++i) fIndx0[i] = fVectors[i].size();
    }
    fErrU = fLRErrU;

    // Store tracks
    storeVectors(1);

    return 0;
}

// -----   Public method finalize (after each event) -----------------------
Bool_t HFwDetStrawVectorFinder::finalize()
{
    return kTRUE;
}

// -----   Private method ComputeMatrix   ----------------------------------
void HFwDetStrawVectorFinder::computeMatrix()
{
    // Compute system matrices for different hit plane patterns

    Double_t cos2[FWDET_STRAW_MAX_VPLANES];
    Double_t sin2[FWDET_STRAW_MAX_VPLANES];
    Double_t sincos[FWDET_STRAW_MAX_VPLANES];
    Double_t dz2[FWDET_STRAW_MAX_VPLANES];

    Bool_t onoff[nVplanes];

    for (Int_t i = 0; i < nVplanes; ++i)
    {
        cos2[i] = fCosa[i] * fCosa[i];
        sin2[i] = fSina[i] * fSina[i];
        sincos[i] = fSina[i] * fCosa[i];
        dz2[i] = fDz[i] * fDz[i];
        onoff[i] = kTRUE;
    }

    TMatrixD coef(4,4);
    Int_t  pattMax = 1 << nVplanes, pattMax1 = 1 << nVplanes/2, nDouble = 0, nTot = 0;

    // Loop over doublet patterns
    for (Int_t ipat = 1; ipat < pattMax; ++ipat)
    {

        // Check if the pattern is valid:
        // either all doublets are active or 3 the first ones (for high resolution mode)
        nDouble = 0;
        if (ipat < pattMax1)
        {
            // One station
            for (Int_t j = 0; j < nVplanesH; j += 2)
            {
                if (ipat & (3 << j))
                    ++nDouble;
                else
                    break;
            }
        }
        else
        {
            // The other station
            Int_t ipat1 = ipat & ((pattMax1-1)<<nVplanesH);
            for (Int_t j = 0; j < nVplanesH; j += 2)
            {
                if (ipat1 & (3 << (j+nVplanesH))) ++nDouble;
                else break;
            }
        }

        if (nDouble + 0 < nVplanesH / 2) continue;

        for (Int_t j = 0; j < nVplanes; ++j) onoff[j] = (ipat & (1 << j));

        coef = 0.0;
        for (Int_t i = 0; i < nVplanes; ++i)
        {
            if (!onoff[i]) continue;
            coef(0,0) += cos2[i];
            coef(0,1) += sincos[i];
            Double_t dz = fDz[i];
            if ((ipat&255)==0) dz = fDz[i%nVplanesH]; // only second station
            coef(0,2) += dz * cos2[i];
            coef(0,3) += dz * sincos[i];
        }
        for (Int_t i = 0; i < nVplanes; ++i)
        {
            if (!onoff[i]) continue;
            coef(1,0) += sincos[i];
            coef(1,1) += sin2[i];
            Double_t dz = fDz[i];
            if ((ipat&255)==0) dz = fDz[i%nVplanesH]; // only second station
            coef(1,2) += dz * sincos[i];
            coef(1,3) += dz * sin2[i];
        }
        for (Int_t i = 0; i < nVplanes; ++i)
        {
            if (!onoff[i]) continue;
            Double_t dz = fDz[i], dzz = dz2[i];
            if ((ipat&255)==0) { dz = fDz[i%nVplanesH]; dzz = dz2[i%nVplanesH]; } // only second station
            coef(2,0) += dz * cos2[i];
            coef(2,1) += dz * sincos[i];
            coef(2,2) += dzz * cos2[i];
            coef(2,3) += dzz * sincos[i];
        }
        for (Int_t i = 0; i < nVplanes; ++i)
        {
            if (!onoff[i]) continue;
            Double_t dz = fDz[i], dzz = dz2[i];
            if ((ipat&255)==0)
            {
                dz = fDz[i%nVplanesH]; dzz = dz2[i%nVplanesH];
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

// -----   Private method GetHits   ----------------------------------------
void HFwDetStrawVectorFinder::getHits()
{
    // Group hits according to their plane number
    for (Int_t m = 0; m < nModules; ++m)
    {
        for (Int_t l = 0; l < nLayers[m]*2; ++l) fHitPl[m][l].clear();
        for (Int_t l = 0; l < nLayers[m]; ++l) fHit[m][l].clear();
    }

    Int_t nHits = pHits->getEntries();
    Char_t addr_module, addr_layer, addr_plane;
    Int_t module, layer, tube;
    HFwDetStrawCal *hit;

    for (Int_t i = 0; i < nHits; ++i)
    {
        hit = (HFwDetStrawCal*) pHits->getObject(i);

        if (hit->GetUniqueID() != 0) continue; // already used

        hit->getAddress(addr_module, addr_layer, addr_plane, tube);
        module = (Int_t) addr_module;
        layer = (Int_t) addr_layer;

        Int_t vplane = layer * FWDET_STRAW_MAX_PLANES + addr_plane;

        fHitPl[module][vplane].insert(HitPair(tube, i));
    }

    // Merge neighbouring hits from two planes of one layer.
    // If there is no neighbour, takes hit from a single plane (to account for inefficiency)
    Int_t indx1 = 0, indx2 = 0, tube1 = 0, tube2 = 0;

    for (Int_t m = 0; m < nModules; ++m)
    {
        // Loop over stations
        for (Int_t i1 = 0; i1 < nLayers[m]; ++i1)
        {
            // Loop over doublets
            multimap<Int_t,Int_t>::iterator
                it1 = fHitPl[m][i1*FWDET_STRAW_MAX_PLANES].begin(),
                it2 = fHitPl[m][i1*FWDET_STRAW_MAX_PLANES+1].begin();

            multimap<Int_t,Int_t>::iterator
                it1end = fHitPl[m][i1*FWDET_STRAW_MAX_PLANES].end(),
                it2end = fHitPl[m][i1*FWDET_STRAW_MAX_PLANES+1].end();

            Bool_t tube1_has_partner = kFALSE;  // if tube1 was matched
            Bool_t tube2_has_partner = kFALSE;  // if tube2 was matched

            // straws in two planes are as following
            //
            //        \   |    / <-- tracks
            // p=1    (1) (2) (3)... (n)  <-- tube number
            // p=0  (1) (2) (3)... (n)
            //           \| /
            //
            // possible pairs for [p=0,p=1] and for 1 < k < n
            // are [k, k] and [k+1, k]
            //
            // Algorithm is as follow:
            // 1. Iterate over p=0
            // 2. check for partners in p=1
            // 3. when all straws from p=0 are done
            // 4. fill remaining straws from p=1

            while(it1 != it1end)
            {
                indx1 = it1->second;
                tube1 = it1->first;

                if (it2 == it2end)
                {
                    if (!tube1_has_partner)
                    {
                        fHit[m][i1].push_back(DoubletPair(indx1, -1));
                    }
                    ++it1;
                    tube1_has_partner = kFALSE;

                    continue;
                }

                while (it2 != it2end)
                {
                    indx2 = it2->second;
                    tube2 = it2->first;

                    // difference between tubes
                    Int_t tube_diff = tube1 - tube2;

                    if (tube_diff > 1)      // tube1 ahead of tube 2
                    {
                        if (!tube2_has_partner)
                        {
                            fHit[m][i1].push_back(DoubletPair(-1, indx2));
                        }

                        ++it2;   // searching for [k, k] case
                        tube2_has_partner = kFALSE;

                        continue;
                    }

                    if (tube_diff == 1)     // we have first possible pair
                    {
                        fHit[m][i1].push_back(DoubletPair(indx1, indx2));

                        tube1_has_partner = kTRUE;

                        ++it2;   // searching for [k, k] case
                        tube2_has_partner = kFALSE;

                        continue;
                    }

                    if (tube_diff == 0)     // we have second possible pair
                    {
                        fHit[m][i1].push_back(DoubletPair(indx1, indx2));

                        tube2_has_partner = kTRUE;

                        ++it1;   // searching for [k+1, k] case
                        tube1_has_partner = kFALSE;

                        break;
                    }

                    if (tube_diff < 0)      // tube2 ahead of tube1
                    {
                        if (!tube1_has_partner)
                        {
                            fHit[m][i1].push_back(DoubletPair(indx1, -1));
                        }
                        tube1_has_partner = kFALSE;
                        ++it1;  // searching for [k, k] case
                        break;
                    }
                }
            }

            while (it2 != it2end)
            {
                indx2 = it2->second;
                tube2 = it2->first;

                if (!tube2_has_partner)
                {
                    fHit[m][i1].push_back(DoubletPair(-1, indx2));
                }

                ++it2;
                tube2_has_partner = kFALSE;
            }
        }
    }
}

// -----   Private method MakeVectors   ------------------------------------
void HFwDetStrawVectorFinder::makeVectors()
{
    // Make vectors for stations
    for (Int_t m = 0; m < nModules; ++m)
    {
        Int_t l = 0;    // layer
        Int_t patt = 0, ndoubl = fHit[m][l].size();
        HFwDetStrawCal *hit = NULL;

        for (Int_t id = 0; id < ndoubl; ++id)
        {
            Int_t indx1 = fHit[m][l][id].first;
            Int_t indx2 = fHit[m][l][id].second;
            if (indx1 >= 0)
            {
                hit = (HFwDetStrawCal*) pHits->getObject(indx1);
                fUz[l*FWDET_STRAW_MAX_PLANES] = hit->getX();

                fUzi[l*FWDET_STRAW_MAX_PLANES][HITNR] = indx1;
                fUzi[l*FWDET_STRAW_MAX_PLANES][TUBENR] = hit->getTube();
            }
            if (indx2 >= 0)
            {
                hit = (HFwDetStrawCal*) pHits->getObject(indx2);
                fUz[l*FWDET_STRAW_MAX_PLANES+1] = hit->getX();

                fUzi[l*FWDET_STRAW_MAX_PLANES+1][HITNR] = indx2;
                fUzi[l*FWDET_STRAW_MAX_PLANES+1][TUBENR] = hit->getTube();
            }
            Bool_t ind1 = indx1 + 1;
            Bool_t ind2 = indx2 + 1;
            patt = ind1;
            patt |= (ind2 << 1);

            processDouble(m, l+1, patt);
        }
    }
}

// -----   Private method processDouble   ----------------------------------
void HFwDetStrawVectorFinder::processDouble(Int_t m, Int_t l, Int_t patt)
{
    // Main processing engine (recursively adds doublet hits to the vector)

    // Tube differences between the same views for mu from omega at 8 GeV
    const Int_t dTubes2 = 30;
    Double_t pars[4] = {0.0};

    Int_t ndoubl = fHit[m][l].size();

    for (Int_t id = 0; id < ndoubl; ++id)
    {
        Int_t indx1 = fHit[m][l][id].first;
        Int_t indx2 = fHit[m][l][id].second;
        HFwDetStrawCal *hit = (HFwDetStrawCal*) pHits->getObject(TMath::Max(indx1,indx2));
        Int_t tube = hit->getTube();

        // Check the same views
        Int_t ok = 1;
        for (Int_t il = 0; il < l; ++il)
        {
            Int_t pl = il * 2;
            if (TMath::Abs(fSina[pl+m*nVplanesH]-fSina[l*2+m*nVplanesH]) < 0.01)
            {
                // Check tube difference
                Double_t z = fDz[pl%nVplanesH];
                Int_t tu = fUzi[pl][TUBENR];
                if (!(patt & (1 << pl)))
                {
                    z = fDz[pl%nVplanesH+1];
                    tu = fUzi[pl+1][TUBENR];
                }
                z += fZ0[m];

                Int_t hitPlane = hit->getLayer() * 2 + hit->getPlane();
                Double_t zHit = fDz[hitPlane] + fZ0[m];
                Double_t dzz = (zHit - z) / z;

                if (TMath::Abs(tube - tu - dzz * tu) > dTubes2)
                {
                    // !!! cut
                    ok = 0;
                    break;
                }
            }
        }

        if (!ok) continue; // !!! cut

        if (indx1 >= 0)
        {
            hit = (HFwDetStrawCal*) pHits->getObject(indx1);
            fUz[l*2] = hit->getX();

            fUzi[l*2][HITNR] = indx1;
            fUzi[l*2][TUBENR] = hit->getTube();
        }
        if (indx2 >= 0)
        {
            hit = (HFwDetStrawCal*) pHits->getObject(indx2);
            fUz[l*2+1] = hit->getX();

            fUzi[l*2+1][HITNR] = indx2;
            fUzi[l*2+1][TUBENR] = hit->getTube();
        }

        Bool_t ind1 = indx1 + 1;
        Bool_t ind2 = indx2 + 1;
        // Clear bits
        patt &= ~(1 << l*FWDET_STRAW_MAX_PLANES);
        patt &= ~(1 << (l*FWDET_STRAW_MAX_PLANES+1));
        // Set bits
        patt |= (ind1 << l*FWDET_STRAW_MAX_PLANES);
        patt |= (ind2 << (l*FWDET_STRAW_MAX_PLANES+1));

        if (l + 1 < nVplanesH / FWDET_STRAW_MAX_PLANES)
        {
            processDouble(m, l+1, patt);
        }
        else
        {
            // Straight line fit of the vector
            Int_t patt1 = patt << m*nVplanesH;
            findLine(patt1, pars);
            Double_t chi2 = findChi2(m, patt1, pars);

             // add vector to the temporary container
            if (chi2 <= fLRCutChi2)
                addVector(m, patt1, chi2, pars);
        }
    }
}

// -----   Private method addVector   --------------------------------------
void HFwDetStrawVectorFinder::addVector(Int_t m, Int_t patt, Double_t chi2, Double_t *pars, Bool_t lowRes)
{
    // Add vector to the temporary container (as HFwDetStrawVector)

    HFwDetStrawVector *track = new HFwDetStrawVector();
    track->setChi2(chi2);
    track->setParams(pars);
    track->setZ(fZ0[m]);
    TMatrixDSym cov(*fMatr[patt]);
    cov *= (fErrU * fErrU);
    track->setCovar(cov);

    if (patt > (1 << nVplanesH)) patt = patt >> nVplanesH; // second station
    for (Int_t ipl = 0; ipl < nVplanesH; ++ipl)
    {
        if (!(patt & (1 << ipl))) continue;
        track->addHit(fUzi[ipl][HITNR]);
        if (lowRes) continue;

        // Store info about left-right ambig. resolving (set true if positive drift direction)
        HFwDetStrawCal *hit = (HFwDetStrawCal*) pHits->getObject(fUzi[ipl][HITNR]);
        if (hit->getX() < fUz[ipl])
            track->setLRbit(track->getNofHits()-1);
    }
    Int_t ndf = (track->getNofHits() > 5) ? track->getNofHits() - 4 : 1;
    track->setNDF(ndf);
    setTrackId(track); // set track ID as its flag

    if (lowRes)
        fVectors[m].push_back(track);
    else
        fVectorsHigh[m].push_back(track);
}

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

// -----   Private method findLine   ---------------------------------------
void HFwDetStrawVectorFinder::findLine(Int_t patt, Double_t *pars)
{
    // Fit of hits to the straight line

    // Solve system of linear equations
    Bool_t ok = kFALSE, onoff;
    TVectorD b(4);
    for (Int_t i = 0; i < nVplanes; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;
        Int_t i1 = i % nVplanesH;
        b[0] += fUz[i1] * fCosa[i];
        b[1] += fUz[i1] * fSina[i];
        b[2] += fUz[i1] * fDz[i%nVplanesH] * fCosa[i];
        b[3] += fUz[i1] * fDz[i%nVplanesH] * fSina[i];
    }

    TVectorD solve = fLus[patt]->Solve(b, ok);
    for (Int_t i = 0; i < 4; ++i) pars[i] = solve[i];
}

// -----   Private method findChi2   ---------------------------------------
Double_t HFwDetStrawVectorFinder::findChi2(Int_t m, Int_t patt, Double_t *pars)
{
    // Compute chi2 of the fit

    Double_t chi2 = 0, x = 0, y = 0, u = 0;

    Bool_t onoff;
    for (Int_t i = 0; i < nVplanes; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;
        x = pars[0] + pars[2] * fDz[i%nVplanesH];
        y = pars[1] + pars[3] * fDz[i%nVplanesH];
        u = x * fCosa[i] + y * fSina[i];
        Int_t i1 = i % nVplanesH;
        Double_t du = (u - fUz[i1]) / fErrU;
        chi2 += du * du;
    }

    return chi2;
}

// -----   Private method CheckParams   ------------------------------------
void HFwDetStrawVectorFinder::checkParams()
{
    // Remove vectors with wrong orientation
    // using empirical cuts for omega muons at 8 Gev

    Double_t cut[2] = {
        pStrawVFPar->getCutX(),
        pStrawVFPar->getCutY() }; // !!! empirical !!!

    Double_t pars[4];

    for (Int_t m = 0; m < nModules; ++m)
    {
        Int_t nvec = fVectors[m].size();

        //for (Int_t iv = 0; iv < nvec; ++iv) {
        for (Int_t iv = fIndx0[m]; iv < nvec; ++iv)
        {
            HFwDetStrawVector *vec = fVectors[m][iv];
            vec->params(pars);
            Double_t x = pars[0], y = pars[1], tx = pars[2], ty = pars[3], z = vec->getZ();
            Double_t dTx = tx - x / z;
            Double_t dTy = ty - y / z;
            if (TMath::Abs(dTx) > cut[0] || TMath::Abs(dTy) > cut[1])
                vec->setChi2(-1.0);
        }

        //for (Int_t iv = nvec-1; iv >= 0; --iv) {
        for (Int_t iv = nvec-1; iv >= fIndx0[m]; --iv)
        {
            HFwDetStrawVector *vec = fVectors[m][iv];
            if (vec->getChi2() < 0)
            {
                delete fVectors[m][iv];
                fVectors[m].erase(fVectors[m].begin()+iv);
            }
        }
//         cout << " Vectors after parameter check (m, inp, out): " << m << " " << nvec << " " << fVectors[m].size() << endl;
    }
}

// -----   Private method HighRes   ----------------------------------------
void HFwDetStrawVectorFinder::highRes()
{
    // High resolution processing (resolve ghost hits and make high resolution vectors)

    HFwDetStrawCal * hit = 0;

    for (Int_t m = 0; m < nModules; ++m)
    {
        Int_t nvec = fVectors[m].size();

        for (Int_t iv = fIndx0[m]; iv < nvec; ++iv)
        {
            HFwDetStrawVector *vec = fVectors[m][iv];
            Int_t nhits = vec->getNofHits(), patt = 0;
            Double_t uu[nVplanesH][2];

            for (Int_t h = 0; h < nhits; ++h)
            {
                hit = (HFwDetStrawCal*) pHits->getObject(vec->getHitIndex(h));

                Int_t lay = hit->getLayer();
                Int_t side = hit->getPlane();
                Int_t plane = lay*2 + side;
                uu[plane][0] = hit->getX();
                uu[plane][1] = 2.5; /*hit->getDrift() FIXME: no drift radius anymore*/; // drift distance with error
                patt |= (1 << plane);
                //fUzi[plane][0] = hit->GetSegment();
                fUzi[plane][HITNR] = vec->getHitIndex(h);
            }

            // Number of hit combinations is 2
            // - left-right ambiguity resolution does not really work for doublets
            Int_t nCombs = (patt & 1) ? 2 : 1;
            Int_t flag = 1, nok = nCombs - 1;

            //cout << " Doublet ID: " << vec->getFlag() << endl;
            for (Int_t icomb = -1; icomb < nCombs; icomb += 2)
            {
                fUz[0] = (nCombs == 2) ? uu[0][0] + uu[0][1] * icomb : 0.0;
                processSingleHigh(m, 1, patt, flag, nok, uu);
            }
        }
    }

    moveVectors(); // move vectors from one container to another, i.e. drop low resolution ones
}

// -----   Private method processDoubleHigh   ------------------------------
void HFwDetStrawVectorFinder::processSingleHigh(Int_t m, Int_t plane, Int_t patt, Int_t flag, Int_t nok, Double_t uu[FWDET_STRAW_MAX_VPLANES/2][FWDET_STRAW_MAX_PLANES])
{
    // Main processing engine for high resolution mode
    // (recursively adds singlet hits to the vector)

    Double_t pars[4] = {0.0};

    // Number of hit combinations is 2
    Int_t nCombs = (patt & (1 << plane)) ? 2 : 1;
    nok += (nCombs - 1);

    for (Int_t icomb = -1; icomb < nCombs; icomb += 2)
    {
        fUz[plane] = (nCombs == 2) ? uu[plane][0] + uu[plane][1] * icomb : 0.0;

        if (plane == nVplanesH - 1 || (nok == fMinHits && flag))
        {
            // Straight line fit of the vector
            Int_t patt1 = patt & ((1 << (plane + 1)) - 1); // apply mask
            patt1 = patt1 << m*nVplanesH;
            findLine(patt1, pars);
            Double_t chi2 = findChi2(m, patt1, pars);

            if (icomb > -1) flag = 0;

            if (chi2 > fHRCutChi2) continue; // too high chi2 - do not extend line

            if (plane + 1 < nVplanesH)
                processSingleHigh(m, plane + 1, patt, 0, nok, uu);
            else
                addVector(m, patt, chi2, pars, kFALSE); // add vector to the temporary container
        }
        else
        {
            processSingleHigh(m, plane + 1, patt, flag, nok, uu);
        }
    }
}

// -----   Private method MoveVectors   ------------------------------------
void HFwDetStrawVectorFinder::moveVectors()
{
    // Drop low-resolution vectors and move high-res. ones to their container

    for (Int_t m = 0; m < nModules; ++m)
    {
        Int_t nVecs = fVectors[m].size();
        //for (Int_t j = 0; j < nVecs; ++j) delete fVectors[m][j];
        //fVectors[m].clear();
        //for (Int_t j = fIndx0[m]; j < nVecs; ++j) {
        for (Int_t j = nVecs - 1; j >= fIndx0[m]; --j)
        {
            delete fVectors[m][j];
            fVectors[m].erase(fVectors[m].begin()+j);
        }

        nVecs = fVectorsHigh[m].size();
        for (Int_t j = 0; j < nVecs; ++j) fVectors[m].push_back(fVectorsHigh[m][j]);
        fVectorsHigh[m].clear(); //
    }
}

// -----   Private method StoreVectors   -----------------------------------
void HFwDetStrawVectorFinder::storeVectors(Int_t isel)
{
    // Store vectors into category

    Int_t ibeg[2] = {0, nModules}, iend[2] = {nModules-1, nModules};
    HLocation loc;
    loc.set(1,0);

    //for (Int_t ist = 0; ist <= nModules; ++ist) {
    for (Int_t ist = ibeg[isel]; ist <= iend[isel]; ++ist)
    {
        Int_t nvec = fVectors[ist].size();
        Int_t iv0 = (ist < nModules) ? fIndx0[ist] : 0;

        //for (Int_t iv = 0; iv < nvec; ++iv) {
        for (Int_t iv = iv0; iv < nvec; ++iv)
        {
            HFwDetStrawVector* cal = (HFwDetStrawVector*) pTrackArray->getNewSlot(loc);

            if (cal)
            {
                cal = new(cal) HFwDetStrawVector(*(fVectors[ist][iv]));
                cal->SetUniqueID(ist);
            }
        }
    }
}

// -----   Private method MergeVectors   -----------------------------------
void HFwDetStrawVectorFinder::mergeVectors()
{
    // Merge vectors from 2 stations

    TMatrixD matr = TMatrixD(4,4);
    TMatrixD unit(TMatrixD::kUnit,matr);
    Double_t pars[4];
    Double_t zMed = (fZ0[0] + fZ0[1]) / 2;

    for (Int_t m = 0; m < 2; ++m)
    {
        // Propagate vectors to the median plane
        Int_t nvec = fVectors[m].size();

        //for (Int_t iv = 0; iv < nvec; ++iv) {
        for (Int_t iv = fIndx0[m]; iv < nvec; ++iv)
        {
            HFwDetStrawVector *tr = fVectors[m][iv];
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
    }

    // Merge vectors
    Int_t m1 = 0, m2 = 1, nvec1 = fVectors[m1].size(), nvec2 = fVectors[m2].size();
    Double_t pars1[4], pars2[4];

    for (Int_t iv1 = fIndx0[m1]; iv1 < nvec1; ++iv1)
    {
        HFwDetStrawVector *tr1 = fVectors[m1][iv1];
        tr1->params(pars1);
        TMatrixDSym w1 = tr1->getCovarMatr();
        TMatrixD p1(4, 1, pars1);
        TMatrixD wp1(w1, TMatrixD::kTransposeMult, p1);

        for (Int_t iv2 = fIndx0[m2]; iv2 < nvec2; ++iv2)
        {
            HFwDetStrawVector *tr2 = fVectors[m2][iv2];
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
            if (c2 < 0 || c2 > fLRCutChi2 * 2) continue;

            // Merged track parameters
            pars[0] = pMerge(0,0);
            pars[1] = pMerge(1,0);
            pars[2] = pMerge(2,0);
            pars[3] = pMerge(3,0);

            addTrack(m1, tr1, tr2, iv1, iv2, pars, c2, w2);
        }
    }
}

// -----   Private method AddTrack   ---------------------------------------
void HFwDetStrawVectorFinder::addTrack(Int_t m0, HFwDetStrawVector *tr1, HFwDetStrawVector *tr2, Int_t indx1, Int_t indx2, Double_t *parOk, Double_t c2, TMatrixDSym &w2)
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
    fVectors[nModules].push_back(track);

    Info("AddTrack", "trID1=%i, trID2=%i, chi2=%f, merged vectors %i",
         tr1->getFlag(),tr2->getFlag(),track->getChi2(),track->getNofHits());
}

// -----   Private method SelectTracks   -----------------------------------
void HFwDetStrawVectorFinder::selectTracks(Int_t ipass)
{
    // Remove ghost tracks (having at least N the same hits (i.e. fired tubes))

    const Int_t nMax = 8, nPl = 40, nVecMin = 2;
    Int_t planes[nPl], hinds[nPl], lr[nPl], ntrs = fVectors[nModules].size();
    multimap<Double_t,Int_t> c2merge;

    for (Int_t i = 0; i < ntrs; ++i)
    {
        HFwDetStrawVector *tr = fVectors[nModules][i];
        if (tr->getNofHits() != nVecMin) continue;
        Double_t qual = tr->getNofHits() +
        (499 - TMath::Min(tr->getChi2()/tr->getNDF(),499.0)) / 500;
        c2merge.insert(pair<Double_t,Int_t>(-qual,i));
    }

    Int_t nSel = 0;
    multimap<Double_t,Int_t>::iterator it, it1;
    for (it = c2merge.begin(); it != c2merge.end(); ++it)
    {
        HFwDetStrawVector *tr1 = fVectors[nModules][it->second];
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
                HFwDetStrawCal *hit = (HFwDetStrawCal*) pHits->getObject(vec->getHitIndex(ih));
                Int_t ipl = hit->getVPlane();
                planes[ipl] = vec->getHitIndex(ih);
                patt |= (1 << ipl);
                hinds[ipl] = vec->getHitIndex(ih);
                if (vec->getLR().TestBitNumber(ih)) lr[ipl] = 1;
            }
        }

        it1 = it;
        for (++it1; it1 != c2merge.end(); ++it1)
        {
            HFwDetStrawVector *tr2 = fVectors[nModules][it1->second];
            if (tr2 == NULL) continue;

            Int_t nvecs2 = tr2->getNofHits(), nover = 0;
            Bool_t over = kFALSE;

            for (Int_t iv = 0; iv < nvecs2; ++iv)
            {
                HFwDetStrawVector *vec = fVectors[iv][tr2->getHitIndex(iv)];
                Int_t nh = vec->getNofHits();

                for (Int_t ih = 0; ih < nh; ++ih)
                {
                    HFwDetStrawCal *hit = (HFwDetStrawCal*) pHits->getObject(vec->getHitIndex(ih));
                    Int_t ipl = hit->getVPlane();
                    if (planes[ipl] < 0) continue;
                    if (planes[ipl] == vec->getHitIndex(ih))
                    {
                        ++nover;
                        if (nover >= nMax)
                        {
                            Info("Select tracks", "Track quality: qual1 %f, qual2 %f, trID1 %i, trID2 %i, overlaps: %i",
                            it->first,it1->first,tr1->getFlag(),tr2->getFlag(),nover);
                            delete fVectors[nModules][it1->second];
                            fVectors[nModules][it1->second] = NULL;
                            over = kTRUE;
                            break;
                        }
                    }
                }
                if (over) break;
            }
        }

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
    }

    // Apply extra slection cuts
    if (ipass < fNpass - 1 && nSel > 1)
    {
        for (Int_t i = 0; i < ntrs; ++i)
        {
            HFwDetStrawVector *tr = fVectors[nModules][i];
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
                        HFwDetStrawCal *hit = (HFwDetStrawCal*) pHits->getObject(vec->getHitIndex(ih));
                        hit->SetUniqueID(999);
                    }
                }
                continue;
            }
            else
            {
                delete fVectors[nModules][i];
                fVectors[nModules][i] = NULL;
            }
        }
    }

    vector<HFwDetStrawVector*>::iterator vit = fVectors[nModules].begin();
    while (vit != fVectors[nModules].end())
    {
        if (*vit == NULL) vit = fVectors[nModules].erase(vit);
        else ++vit;
    }
}

// -----   Private method Refit   ------------------------------------------
Double_t HFwDetStrawVectorFinder::refit(Int_t patt, Int_t *hinds, Double_t *pars, TMatrixDSym *cov, Int_t *lr)
{
    // Fit of hits to the straight line

    // Solve system of linear equations
    Bool_t ok = kFALSE, onoff;
    TVectorD b(4);

    for (Int_t i = 0; i < nVplanes; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;
        HFwDetStrawCal *hit = (HFwDetStrawCal*) pHits->getObject(hinds[i]);
        Double_t uc = 0.0;// FIXME drift: (hit->getX() + lr[i] * hit->getDrift()) * fCosa[i]; // resolved ambiguity !!!
        Double_t us = 0.0;// FIXME drift: (hit->getX() + lr[i] * hit->getDrift()) * fSina[i]; // resolved ambiguity !!!
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
    for (Int_t i = 0; i < nVplanes; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;
        Double_t x = pars[0] + pars[2] * fDz[i];
        Double_t y = pars[1] + pars[3] * fDz[i];
        Double_t u = x * fCosa[i] + y * fSina[i];
        HFwDetStrawCal *hit = (HFwDetStrawCal*) pHits->getObject(hinds[i]);
        Double_t du = (u - hit->getX() - lr[i] * 5.05); // FIXME drift:  hit->getDrift()) / fErrU;
        chi2 += du * du;
    }
    return chi2;
}

ClassImp(HFwDetStrawVectorFinder);
