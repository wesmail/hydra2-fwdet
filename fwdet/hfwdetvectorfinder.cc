//*-- AUTHOR : A.Zinchenko <Alexander.Zinchenko@jinr.ru>
//*-- Created : 08/08/2016
//*-- Modified: R. Lalik <Rafal.Lalik@ph.tum.de> 2016/10/10

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HFwDetVectorFinder
//
//  Tracking code for FwDetStraw
//
/////////////////////////////////////////////////////////////

#include "hades.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hevent.h"
#include "fwdetdef.h"
#include "hfwdetdetector.h"
#include "hfwdetstrawgeompar.h"
#include "hfwdetstrawcalsim.h"
#include "hfwdetstrawdigipar.h"
#include "hfwdetrpchit.h"
#include "hfwdetvectorfinder.h"
#include "hfwdetvectorfinderpar.h"
#include "hvectorcand.h"
#include "hvectorcandsim.h"
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

// #define VERBOSE_MODE 1

HFwDetVectorFinder::HFwDetVectorFinder() : HReconstructor()
{
    initVariables();
}

HFwDetVectorFinder::HFwDetVectorFinder(const Text_t *name, const Text_t *title) :
    HReconstructor(name, title)
{
    initVariables();
}

HFwDetVectorFinder::~HFwDetVectorFinder()
{
    for (Int_t i = 0; i <= FWDET_STRAW_MAX_MODULES; ++i)
    {
        Int_t nVecs = fVectors[i].size();
        for (Int_t j = 0; j < nVecs; ++j)
        {
            delete fVectors[i][j];
            fVectors[i][j] = nullptr;
        }
    }
    for (map<Int_t,TDecompLU*>::iterator it = fLus.begin(); it != fLus.end(); ++it)
    {
        delete it->second;
    }
}

void HFwDetVectorFinder::initVariables()
{
    isSimulation = kFALSE;
    pStrawHits = pRpcHits = pVectorCand = nullptr;
    pStrawVFPar = nullptr;

    fMinHits = 10; // FIXME to aparams
    nModules = 0;

    current_track = nullptr;

    for (Int_t i = 0; i < FWDET_STRAW_MAX_MODULES; ++i)
    {
        fIndx0[i] = 0;
    }
}

Bool_t HFwDetVectorFinder::init() 
{
    // find the Forward detector in the HADES setup
    HFwDetDetector* pFwDet = (HFwDetDetector*)(gHades->getSetup()->getDetector("FwDet"));
    if (!pFwDet)
    {
        Error("HFwDetVectorFinder::init","No Forward Detector found");
        return kFALSE;
    }

    // GEANT input data
    HCategory* pKine = gHades->getCurrentEvent()->getCategory(catGeantKine); 
    if (pKine) 
    {
        isSimulation = kTRUE;
    }

    // create the straw digitizer parameter container
    pStrawDigiPar = (HFwDetStrawDigiPar *)gHades->getRuntimeDb()->getContainer("FwDetStrawDigiPar");
    if (!pStrawDigiPar)
    {
        Error("HFwDetStrawDigitizer::init()","Parameter container for digitizer not created");
        return kFALSE;
    }

    // straw hits
    pStrawHits = gHades->getCurrentEvent()->getCategory(catFwDetStrawCal);
    if (!pStrawHits)
    {
        Error("HFwDetVectorFinder::init()","HFwDetStrawCal input missing");
        return kFALSE;
    }

    // straw hits
    pRpcHits = gHades->getCurrentEvent()->getCategory(catFwDetRpcHit);
    if (!pRpcHits)
    {
        Error("HFwDetVectorFinder::init()","HFwDetRpcHit input missing");
//         return kFALSE;
    }

    // build the Straw vector category
    if (isSimulation)
        pVectorCand = new HLinearCategory("HVectorCandSim");
    else
        pVectorCand = new HLinearCategory("HVectorCand");

    if (!pVectorCand)
    {
        Error("HFwDetVectorFinder::init()","Straw vector category not created");
        return kFALSE;
    }
    else
    {
        gHades->getCurrentEvent()->addCategory(catVectorCand, pVectorCand, "FwDet");
    }

    pStrawVFPar = (HFwDetVectorFinderPar *)
        gHades->getRuntimeDb()->getContainer ("FwDetVectorFinderPar");
    if (!pStrawVFPar)
    {
        Error("HFwDetVectorFinder::init()","Parameter container for vector finder parameters not created");
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

Bool_t HFwDetVectorFinder::reinit()
{
    fLRCutChi2 = pStrawVFPar->getLRCutChi2();
    fHRCutChi2 = pStrawVFPar->getHRCutChi2();

    fLRErrU = pStrawVFPar->getLRErrU() / TMath::Sqrt(12.0);
    fHRErrU = pStrawVFPar->getLRErrU();

    fTubesD = pStrawVFPar->getTubesD();

    fCutX = pStrawVFPar->getCutX();
    fCutY = pStrawVFPar->getCutY();

    fErrU = fLRErrU;

    nMaxBest = pStrawVFPar->getMaxBest();
    fNpass = pStrawVFPar->getNpass();

    dt_p[0] = pStrawDigiPar->getDriftRadiusPar(0);
    dt_p[1] = pStrawDigiPar->getDriftRadiusPar(1);
    dt_p[2] = pStrawDigiPar->getDriftRadiusPar(2);
    dt_p[3] = pStrawDigiPar->getDriftRadiusPar(3);
    dt_p[4] = pStrawDigiPar->getDriftRadiusPar(4);

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
                plane = HFwDetStrawCal::getVPlane(m, l, p);
                fDz[plane] = pStrawGeomPar->getOffsetZ(m, l, p);
                fCosa[plane] = cos(a);
                fSina[plane] = sin(a);

                // default drift time, modified if RPC hit is matched
                fDrift[plane] = 2.5;
            }
        }

        fZ0[m] = pStrawGeomPar->getOffsetZ(m, 0, 0);
    }

    for (Int_t i = FWDET_STRAW_MAX_VPLANES-1; i >= 0; --i)
    {
        fDz[i] = fDz[i] - fDz[0];
    }

    computeMatrix(); // compute system matrices

#ifdef VERBOSE_MODE
    pStrawVFPar->printParams();
#endif

    return kTRUE;
}

void HFwDetVectorFinder::clear()
{
    Int_t nVecs = 0;

    for (Int_t m = 0; m <= nModules; ++m)
    {
        nVecs = fVectors[m].size();
        for (Int_t j = 0; j < nVecs; ++j)
            delete fVectors[m][j];
        fVectors[m].clear();

        if (m >= nModules) continue;

        nVecs = fVectorsHigh[m].size();
        for (Int_t j = 0; j < nVecs; ++j)
            delete fVectorsHigh[m][j];

        fVectorsHigh[m].clear();
        fIndx0[m] = 0;
    }
}

Int_t HFwDetVectorFinder::execute()
{
    //gErrorIgnoreLevel = kInfo; //debug level
    gErrorIgnoreLevel = kWarning; //debug level

    clear();

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

        // drop low resolution vectors
        moveVectors();

        // Store vectors
//         storeVectors(0);

        // Merge vectors
        mergeVectors();

        // Select final tracks - remove ghosts
        selectTracks(ipass);

        if (ipass == fNpass - 1) continue;

        for (Int_t i = 0; i < nModules; ++i)
            fIndx0[i] = fVectors[i].size();
    }
    fErrU = fLRErrU;

    // Store tracks
    storeVectors(1);

    return 0;
}

Bool_t HFwDetVectorFinder::finalize()
{
    return kTRUE;
}

void HFwDetVectorFinder::computeMatrix()
{
    // delete existing matrices
    std::map<Int_t,TDecompLU*>::iterator it_lus;
    for (it_lus = fLus.begin(); it_lus != fLus.end(); ++it_lus)
        delete it_lus->second;

    std::map<Int_t,TMatrixDSym*>::iterator it_matr;
    for (it_matr = fMatr.begin(); it_matr != fMatr.end(); ++it_matr)
        delete it_matr->second;

    // Compute system matrices for different hit plane patterns
    Double_t cos2[FWDET_STRAW_MAX_VPLANES];
    Double_t sin2[FWDET_STRAW_MAX_VPLANES];
    Double_t sincos[FWDET_STRAW_MAX_VPLANES];
    Double_t dz2[FWDET_STRAW_MAX_VPLANES];

    Bool_t onoff[FWDET_STRAW_MAX_VPLANES];

    for (Int_t i = 0; i < FWDET_STRAW_MAX_VPLANES; ++i)
    {
        cos2[i] = fCosa[i] * fCosa[i];
        sin2[i] = fSina[i] * fSina[i];
        sincos[i] = fSina[i] * fCosa[i];
        dz2[i] = fDz[i] * fDz[i];
        onoff[i] = kTRUE;
    }

    TMatrixD coef(4,4);
    Int_t  pattMax = 1 << FWDET_STRAW_MAX_VPLANES, pattMax1 = 1 << FWDET_STRAW_MAX_VPLANES/2, nDouble = 0, nTot = 0;

    // Loop over doublet patterns
    for (Int_t ipat = 1; ipat < pattMax; ++ipat)
    {
        // Check if the pattern is valid:
        // either all doublets are active or
        // 3 the first ones (for high resolution mode)

        nDouble = 0;
        if (ipat < pattMax1)
        {
            // One station
            for (Int_t j = 0; j < (FWDET_STRAW_MAX_VPLANES/2); j += 2)
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
            Int_t ipat1 = ipat & ((pattMax1-1)<<(FWDET_STRAW_MAX_VPLANES/2));
            for (Int_t j = 0; j < (FWDET_STRAW_MAX_VPLANES/2); j += 2)
            {
                if (ipat1 & (3 << (j+(FWDET_STRAW_MAX_VPLANES/2)))) ++nDouble;
                else break;
            }
        }

        if (nDouble + 0 < (FWDET_STRAW_MAX_VPLANES/2) / 2)
            continue;

        for (Int_t j = 0; j < FWDET_STRAW_MAX_VPLANES; ++j)
            onoff[j] = (ipat & (1 << j));

        coef = 0.0;
        for (Int_t i = 0; i < FWDET_STRAW_MAX_VPLANES; ++i)
        {
            if (!onoff[i]) continue;

            Double_t dz = fDz[i];
            Double_t dzz = dz2[i];

            coef(0,0) += cos2[i];
            coef(0,1) += sincos[i];
            coef(0,2) += dz * cos2[i];
            coef(0,3) += dz * sincos[i];

            coef(1,0) += sincos[i];
            coef(1,1) += sin2[i];
            coef(1,2) += dz * sincos[i];
            coef(1,3) += dz * sin2[i];

            coef(2,0) += dz * cos2[i];
            coef(2,1) += dz * sincos[i];
            coef(2,2) += dzz * cos2[i];
            coef(2,3) += dzz * sincos[i];

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
        fMatr.insert(pair<Int_t,TMatrixDSym*>(ipat, new TMatrixDSym(cov)));
    }
}

void HFwDetVectorFinder::getHits()
{
    // Group hits according to their plane number
    for (Int_t m = 0; m < nModules; ++m)
    {
        for (Int_t l = 0; l < nLayers[m]*FWDET_STRAW_MAX_PLANES; ++l)
            fHitPl[m][l].clear();
        for (Int_t l = 0; l < nLayers[m]; ++l)
            fHit[m][l].clear();
    }

    Int_t nHits = pStrawHits->getEntries();
    Char_t addr_module, addr_layer, addr_plane, addr_udconf;
    Int_t module, layer, plane, tube;
    HFwDetStrawCal * hit = nullptr;

    for (Int_t i = 0; i < nHits; ++i)
    {
        hit = (HFwDetStrawCal*) pStrawHits->getObject(i);

        if (hit->GetUniqueID() != 0) continue; // already used

        hit->getAddress(addr_module, addr_layer, addr_plane, tube, addr_udconf);
        module = (Int_t) addr_module;
        layer = (Int_t) addr_layer;
        plane = (Int_t) addr_plane;

        Int_t vplane = hit->getVPlane(0, layer, plane);

        fHitPl[module][vplane].insert(HitPair(tube, i));

#ifdef VERBOSE_MODE
printf("(%3d)  m=%d l=%d p=%d -> vp=%d  t=%03d  p=%f    tr=%d\n", i, module, layer, plane, vplane, tube, isSimulation ? ((HFwDetStrawCalSim*) hit)->getP() : -100.0, isSimulation ? ((HFwDetStrawCalSim*) hit)->getTrack() : -1);
#endif
    }

    // Merge neighbouring hits from two planes of one layer.
    // If there is no neighbour, takes hit from a single plane
    // (to account for inefficiency)
    Int_t indx1 = 0, indx2 = 0, tube1 = 0, tube2 = 0;

    HFwDetStrawCal * hit1;
    HFwDetStrawCal * hit2;

    for (Int_t pass = 0; pass < 2; ++pass)
    {
#ifdef VERBOSE_MODE
printf("pass %d\n", pass);
#endif
        for (Int_t m = 0; m < nModules; ++m)
        {
            // Loop over stations
            for (Int_t i1 = 0; i1 < nLayers[m]; ++i1)
            {
                // Loop over doublets
                multimap<Int_t,Int_t>::iterator
                    it1 = fHitPl[m][i1*FWDET_STRAW_MAX_PLANES].begin(),
                    it2 = fHitPl[m][i1*FWDET_STRAW_MAX_PLANES+1].begin();

                multimap<Int_t,Int_t>::const_iterator
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

                    hit1 = (HFwDetStrawCal*) pStrawHits->getObject(indx1);
                    Int_t ud1 = hit1->getUDconf();

                    if (it2 == it2end)
                    {
                        if (!tube1_has_partner)
                        {
                            Bool_t badCond = ((pass == 0 and ud1 == 0x2) or
                            (pass == 1 and ud1 != 0x2));
                            if (!badCond)
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

                        hit2 = (HFwDetStrawCal*) pStrawHits->getObject(indx2);
                        Int_t ud2 = hit2->getUDconf();

                        Bool_t skipThisPair = kFALSE;

                        if (pass == 0)
                        {
                            Bool_t doBreak = kFALSE;
                            Bool_t doNext = kFALSE;

                            // we dont want lower tubes here
                            if (ud1 == 0x2)
                            {
                                ++it1;
                                doBreak = kTRUE;
                            }
                            if (ud2 == 0x2)
                            {
                                ++it2;
                                doNext = kTRUE;
                            }
                            if (doBreak)
                                break;
                            if (doNext)
                                continue;
                        }
                        else if (pass == 1)
                        {
                            Bool_t doBreak = kFALSE;
                            Bool_t doNext = kFALSE;

                            // we dont want upper tubes here
                            if (ud1 == 0x1)
                            {
                                ++it1;
                                doBreak = kTRUE;
                            }
                            if (ud2 == 0x1)
                            {
                                ++it2;
                                doNext = kTRUE;
                            }

                            if (doBreak)
                                break;
                            if (doNext)
                                continue;

                            if (ud1 == 0x3 && ud2 == 0x3)
                                skipThisPair = kTRUE;
                            if (ud1 == 0x3 && it2 == it2end)
                                skipThisPair = kTRUE;
                            if (ud2 == 0x3 && it1 == it1end)
                                skipThisPair = kTRUE;
                        }

                        // difference between tubes
                        Int_t tube_diff = tube1 - tube2;

#ifdef VERBOSE_MODE
if (skipThisPair)
    printf("  skipped!  ");

printf("Testing pair: (%d, %d, %d), (%d, %d, %d) dist=%d\n",
        tube1, indx1, ud1, tube2, indx2, ud2, tube_diff);
#endif
                        if (tube_diff > 1)      // tube1 ahead of tube 2
                        {
                            if (!tube2_has_partner)
                            {
                                if (!skipThisPair)
                                    fHit[m][i1].push_back(DoubletPair(-1, indx2));
                            }

                            ++it2;   // searching for [k, k] case
                            tube2_has_partner = kFALSE;

                            continue;
                        }

                        if (tube_diff == 1)     // we have first possible pair
                        {
                            if (!skipThisPair)
                                fHit[m][i1].push_back(DoubletPair(indx1, indx2));

                            tube1_has_partner = kTRUE;

                            ++it2;   // searching for [k, k] case
                            tube2_has_partner = kFALSE;

                            continue;
                        }

                        if (tube_diff == 0)     // we have second possible pair
                        {
                            if (!skipThisPair)
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
                                if (!skipThisPair)
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

                    hit2 = (HFwDetStrawCal*) pStrawHits->getObject(indx2);
                    Int_t ud2 = hit2->getUDconf();

                    if (!tube2_has_partner)
                    {
                        Bool_t badCond = ((pass == 0 and ud2 == 0x2) or
                            (pass == 1 and ud2 != 0x2));
                        if (!badCond)
                            fHit[m][i1].push_back(DoubletPair(-1, indx2));
                    }

                    ++it2;
                    tube2_has_partner = kFALSE;
                }
            }
        }
    }

#ifdef VERBOSE_MODE
for (Int_t m = 0; m < nModules; ++m)
{
    printf("mod=%2d\n", m);
    for (Int_t l = 0; l < nLayers[m]; ++l)
    {
        for (UInt_t d = 0; d < fHit[m][l].size(); ++d)
        printf("d=%d/%ld m=%d 2d=%d   f=%d  s=%d\n", d+1, fHit[m][l].size(),
                m, l, fHit[m][l][d].first, fHit[m][l][d].second);
    }
}
#endif
}

void HFwDetVectorFinder::makeVectors()
{
    // Make vectors for stations
    for (Int_t m = 0; m < nModules; ++m)
    {
        Int_t patt = 0, ndoubl = fHit[m][0].size();
        HFwDetStrawCal *hit = nullptr;

        for (Int_t id = 0; id < ndoubl; ++id)
        {
            Int_t indx1 = fHit[m][0][id].first;
            Int_t indx2 = fHit[m][0][id].second;
            if (indx1 >= 0)
            {
                Int_t pl = hit->getVPlane(m, 0, 0);
                hit = (HFwDetStrawCal*) pStrawHits->getObject(indx1);
                fUz[pl] = hit->getU();
                fUzHit[pl] = indx1;
                fUzTube[pl] = hit->getStraw();
            }
            if (indx2 >= 0)
            {
                Int_t pl = hit->getVPlane(m, 0, 1);
                hit = (HFwDetStrawCal*) pStrawHits->getObject(indx2);
                fUz[pl] = hit->getU();
                fUzHit[pl] = indx2;
                fUzTube[pl] = hit->getStraw();
            }
            Bool_t ind1 = indx1 + 1;
            Bool_t ind2 = indx2 + 1;
            patt = ind1;
            patt |= (ind2 << 1);

#ifdef VERBOSE_MODE
printf("makeVectors(): m=%d  l=%d  id=%d  patt=%x  idx=%d,%d\n", m, 0, id, patt, indx1, indx2);
#endif
            processDouble(m, 1, patt);
        }
    }
}

void HFwDetVectorFinder::processDouble(Int_t m, Int_t l, Int_t patt)
{
    // Main processing engine (recursively adds doublet hits to the vector)
    Double_t pars[4] = {0.0};

    Int_t ndoubl = fHit[m][l].size();

    Int_t pl = HFwDetStrawCal::getVPlane(m, l, 0);
    Int_t ipl_init = HFwDetStrawCal::getVPlane(m, 0, 0);

    for (Int_t id = 0; id < ndoubl; ++id)
    {
        Int_t indx1 = fHit[m][l][id].first;
        Int_t indx2 = fHit[m][l][id].second;

#ifdef VERBOSE_MODE
for (Int_t i = 0; i < l; ++i) printf(" ");
printf("processDouble(): m=%d  l=%d  id=%d  patt=%x  pl=%d  idx=%d,%d\n", m, l, id, patt, pl, indx1, indx2);
#endif

        HFwDetStrawCal *hit = (HFwDetStrawCal*) pStrawHits->getObject(TMath::Max(indx1,indx2));
        Int_t tube = hit->getStraw();

        // Check the same views
        Int_t ok = 1;

        for (Int_t ipl = ipl_init; ipl < pl; ++++ipl)
        {
            if (TMath::Abs(fSina[pl]-fSina[ipl]) < 0.01)
            {
                // Check tube difference
                Double_t z = fDz[ipl];
                Int_t tu = fUzTube[ipl];
                if (!(patt & (1 << pl)))
                {
                    z = fDz[ipl+1];
                    tu = fUzTube[ipl+1];
                }
                z += fZ0[m];

                Double_t zHit = fDz[pl] + fZ0[m];
                Double_t dzz = (zHit - z) / z;

                Float_t dist = TMath::Abs(tube - tu - dzz * tu);
#ifdef VERBOSE_MODE
for (Int_t i = 0; i < l; ++i) printf(" ");
printf("[%d,%d] -> [(%d,%.1f),(%d,%.1f)] dtub = %f < %f\n", ipl, pl, tube, zHit, tu, z, dist, fTubesD);
#endif
                if (dist > fTubesD)
                {
                    ok = 0;
                    break;
                }
            }
        }

        if (!ok) continue;

        if (indx1 >= 0)
        {
            hit = (HFwDetStrawCal*) pStrawHits->getObject(indx1);
            fUz[pl] = hit->getU();

            fUzHit[pl] = indx1;
            fUzTube[pl] = hit->getStraw();
        }
        if (indx2 >= 0)
        {
            hit = (HFwDetStrawCal*) pStrawHits->getObject(indx2);
            fUz[pl+1] = hit->getU();

            fUzHit[pl+1] = indx2;
            fUzTube[pl+1] = hit->getStraw();
        }

        Bool_t ind1 = indx1 + 1;
        Bool_t ind2 = indx2 + 1;

        Int_t patt_new = patt;
        patt_new |= (ind1 << l*FWDET_STRAW_MAX_PLANES);     // Set bits
        patt_new |= (ind2 << (l*FWDET_STRAW_MAX_PLANES+1));

        if (l + 1 < (FWDET_STRAW_MAX_VPLANES/2) / FWDET_STRAW_MAX_PLANES)
        {
            processDouble(m, l+1, patt_new);
        }
        else
        {
            // Straight line fit of the vector
            Int_t patt1 = patt_new << m*(FWDET_STRAW_MAX_VPLANES/2);
            findLine(patt1, pars);
            Double_t chi2 = findChi2(patt1, pars);

             // add vector to the temporary container
            if (chi2 <= fLRCutChi2)
                addVector(m, patt1, chi2, pars);
        }
    }
}

HVectorCand * HFwDetVectorFinder::addVector(Int_t m, Int_t patt, Double_t chi2, Double_t *pars, Bool_t lowRes)
{
    // Add vector to the temporary container (as HVectorCand)
#ifdef VERBOSE_MODE
// printf("      + addVector(): m=%d  patt=%x  chi2=%f  LR=%d\n", m, patt, chi2, lowRes);
// printf("        pars=%f %f %f %f\n", pars[0], pars[1], pars[2], pars[3]);
#endif

    HVectorCand *track;
    if (isSimulation)
        track = (HVectorCand*) new HVectorCandSim();
    else
        track = new HVectorCand();

    track->setChi2(chi2);
    track->setParams(pars);
    track->setZ(fZ0[m]);
    TMatrixDSym cov(*fMatr[patt]);
    cov *= (fErrU * fErrU);
    track->setCovar(cov);

    for (Int_t ipl = 0; ipl < FWDET_STRAW_MAX_VPLANES; ++ipl)
    {
        if (!(patt & (1 << ipl))) continue;
        track->addHit(fUzHit[ipl]);

        if (lowRes) continue;

        // Store info about left-right ambig. resolving
        // (set true if positive drift direction)
        HFwDetStrawCal *hit = (HFwDetStrawCal*) pStrawHits->getObject(fUzHit[ipl]);

        if (hit->getU() < fUz[ipl])
            track->setLRbit(track->getNofHits()-1);
    }
    Int_t ndf = (track->getNofHits() > 5) ? track->getNofHits() - 4 : 1;
    track->setNDF(ndf);

    if (lowRes)
        fVectors[m].push_back(track);
    else
        fVectorsHigh[m].push_back(track);

    return track;
}

void HFwDetVectorFinder::setTrackId(HVectorCand *vec)
{
    if (!isSimulation)
    {
        return;
    }

    std::map<Int_t, Int_t> ids;
    Int_t nhits = vec->getNofHits();

    // count all associated track ids
    for (Int_t ih = 0; ih < nhits; ++ih)
    {
        HFwDetStrawCalSim *hit = (HFwDetStrawCalSim*) pStrawHits->getObject(vec->getHitIndex(ih));
        ++ids[ hit->getTrack() ];
    }

    Int_t currentMax = 0;
    Int_t arg_max = 0;

    std::map< Int_t, Int_t >::iterator it;

    // find the most frequent occurance
    for(it = ids.begin(); it != ids.end(); ++it )
    {
        if (it->second > currentMax)
        {
            arg_max = it->first;
            currentMax = it->second;
        }
    }

    // to have a valid ID more than half of the tracks must have the same ID
    if (currentMax > nhits/2)
        ((HVectorCandSim*) vec)->setTrack(arg_max);
    else
        ((HVectorCandSim*) vec)->setTrack(-1);
}

void HFwDetVectorFinder::findLine(Int_t patt, Double_t *pars)
{
    // Fit hits to the straight line

    Bool_t ok = kFALSE, onoff;
    TVectorD b(4);

    for (Int_t i = 0; i < FWDET_STRAW_MAX_VPLANES; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;

        b[0] += fUz[i] * fCosa[i];
        b[1] += fUz[i] * fSina[i];
        b[2] += fUz[i] * fDz[i] * fCosa[i];
        b[3] += fUz[i] * fDz[i] * fSina[i];
    }

    // Solve system of linear equations
    TVectorD solve = fLus[patt]->Solve(b, ok);
    for (Int_t i = 0; i < 4; ++i) pars[i] = solve[i];
}

Double_t HFwDetVectorFinder::findChi2(Int_t patt, Double_t *pars)
{
    // Compute chi2 of the fit
    Double_t chi2 = 0, x = 0, y = 0, u = 0;
    Bool_t onoff;

    for (Int_t i = 0; i < FWDET_STRAW_MAX_VPLANES; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;

        x = pars[0] + pars[2] * fDz[i];
        y = pars[1] + pars[3] * fDz[i];
        u = x * fCosa[i] + y * fSina[i];

        Double_t du = (u - fUz[i]) / fErrU;
        chi2 += du * du;
    }

    return chi2;
}

void HFwDetVectorFinder::checkParams()
{
    // Remove vectors with wrong orientation
    // using empirical cuts for omega muons at 8 Gev

    Double_t pars[4];

    for (Int_t m = 0; m < nModules; ++m)
    {
        Int_t nvec = fVectors[m].size();

        //for (Int_t iv = 0; iv < nvec; ++iv) {
        for (Int_t iv = fIndx0[m]; iv < nvec; ++iv)
        {
            HVectorCand *vec = fVectors[m][iv];
            if (!vec) continue;

            vec->getParams(pars);
            Double_t x = pars[0], y = pars[1], tx = pars[2], ty = pars[3], z = vec->getZ();
            Double_t dTx = tx - x / z;
            Double_t dTy = ty - y / z;

            if (TMath::Abs(dTx) > fCutX || TMath::Abs(dTy) > fCutY)
                vec->setChi2(-1.0);
        }

        for (Int_t iv = nvec-1; iv >= fIndx0[m]; --iv)
        {
            HVectorCand *vec = fVectors[m][iv];
            if (!vec) continue;

            if (vec->getChi2() < 0)
            {
                delete fVectors[m][iv];
                fVectors[m][iv] = nullptr;
            }
        }
#ifdef VERBOSE_MODE
    printf(" Vectors after parameter check: m=%d, inp=%d, out=%lu\n",
           m, nvec, fVectors[m].size());
#endif
    }
}

void HFwDetVectorFinder::highRes()
{
    // High resolution processing (resolve ghost hits and make high resolution vectors)

    HFwDetStrawCal * hit = nullptr;

    for (Int_t m = 0; m < nModules; ++m)
    {
        Int_t nvec = fVectors[m].size();

        Int_t pl = HFwDetStrawCal::getVPlane(m, 0, 0);
        Int_t pl_limit = HFwDetStrawCal::getVPlane(m+1, 0, 0);

        for (Int_t iv = fIndx0[m]; iv < nvec; ++iv)
        {
            combos.clear();

            HVectorCand *vec = fVectors[m][iv];
            if (!vec) continue;

            Int_t nhits = vec->getNofHits(), patt = 0;
            Double_t uu[FWDET_STRAW_MAX_VPLANES][3] = { 0.0 };

            Double_t params[4];
            vec->getParams(params);

            Double_t tof_grad = 0.0;
            HFwDetRpcHit * rpchit = nullptr;
            Int_t hitno = matchRpcHit(params, fZ0[0]);
            if (hitno != -1)
            {
                rpchit = (HFwDetRpcHit*) pRpcHits->getObject(hitno);
                tof_grad = rpchit->getTof() / rpchit->getZ();
                vec->setTof(rpchit->getTof());
                vec->setDistance(rpchit->getDistance());
                if (isSimulation)
                    ((HVectorCandSim*) vec)->setRpcTrack(rpchit->getTrack());
            }

            current_track = vec;

            for (Int_t h = 0; h < nhits; ++h)
            {
                Int_t hi = vec->getHitIndex(h);
                hit = (HFwDetStrawCal*) pStrawHits->getObject(hi);

                Int_t plane = hit->getVPlane();
                Float_t time = hit->getTime();

                uu[plane][0] = hit->getU();

                if (rpchit)
                {
                    Float_t drift_time = time - tof_grad * fDz[plane];
                    fDrift[plane] =  calcDriftRadius(drift_time);
                }
                else
                {
                    fDrift[plane] = 2.5;
                }

#ifdef VERBOSE_MODE
// printf("LR dt: %f -> %f (%f), grad=%f, z=%f\n", time,  drift_time, fDrift[plane], tof_grad, fDz[plane]);
#endif
                uu[plane][1] = fDrift[plane];

                patt |= (1 << plane);
                fUzHit[plane] = hi;
            }

            // Number of hit combinations is 2
            // - left-right ambiguity resolution does not really work for doublets
            Int_t nCombs = (patt & (1 << pl)) ? 2 : 1;
            Int_t flag = 1, nok = nCombs - 1;

            //cout << " Doublet ID: " << vec->getFlag() << endl;
            for (Int_t icomb = -1; icomb < nCombs; icomb += 2)
            {
                uu[pl][2] = icomb;
                fUz[pl] = (nCombs == 2) ? uu[pl][0] + uu[pl][1] * icomb : 0.0;
                processSingleHigh(m, pl+1, pl_limit, patt, flag, nok, uu);
            }

            // from all the combinations add only the best one
            // FIXME hmm, maybe the best one is filtered later?
            std::sort(combos.begin(), combos.end());

            Int_t limit = TMath::Min(nMaxBest, (Int_t)combos.size());
            for (int i = 0; i < limit; ++i)
            {
                ComboSet cs = combos[i];
                for (Int_t i = 0; i < FWDET_STRAW_MAX_VPLANES; ++i)
                    fUz[i] = cs.z[i];

                // add vector to the temporary container
                HVectorCand * hc = addVector(m, cs.pattern, cs.chi2, cs.pars, kFALSE);
                if (hc)
                {
                    hc->setTof(current_track->getTof());
                    hc->setDistance(current_track->getDistance());
                    if (isSimulation)
                        ((HVectorCandSim*) hc)->setRpcTrack(
                            ((HVectorCandSim*) current_track)->getRpcTrack());
                }
            }
        }
    }

#ifdef VERBOSE_MODE
printf("High res vectors:\n");
for (Int_t m = 0; m < nModules; ++m)
    printf(" m=%d  vecs=%lu\n", m, fVectorsHigh[m].size());
#endif
}

void HFwDetVectorFinder::processSingleHigh(Int_t m, Int_t plane, Int_t plane_limit, Int_t patt, Int_t flag, Int_t nok, Double_t uu[FWDET_STRAW_MAX_VPLANES][3])
{
    // Main processing engine for high resolution mode
    // (recursively adds singlet hits to the vector)

    Double_t pars[4] = {0.0};

    // Number of hit combinations is 2
    Int_t nCombs = (patt & (1 << plane)) ? 2 : 1;
    nok += (nCombs - 1);

    for (Int_t icomb = -1; icomb < nCombs; icomb += 2)
    {
        uu[plane][2] = icomb;
        fUz[plane] = (nCombs == 2) ? uu[plane][0] + uu[plane][1] * icomb : 0.0;

        if (plane == (plane_limit - 1) || (nok == fMinHits && flag))
        {
            // Straight line fit of the vector
            findLine(patt, pars);
            Double_t chi2 = findChi2(patt, pars);

            if (icomb > -1) flag = 0;

            if (chi2 > fHRCutChi2) continue; // too high chi2 - do not extend line

            if (plane + 1 < plane_limit)
                processSingleHigh(m, plane + 1, plane_limit, patt, 0, nok, uu);
            else
            {
                ComboSet cs;
                cs.pattern = patt;
                cs.chi2 = chi2;
                for (Int_t i = 0; i < 4; ++i)
                    cs.pars[i] = pars[i];
                for (Int_t i = 0; i < FWDET_STRAW_MAX_VPLANES; ++i)
                {
                    cs.z[i] = fUz[i];
                }
                combos.push_back(cs);
            }
        }
        else
        {
            processSingleHigh(m, plane + 1, plane_limit, patt, flag, nok, uu);
        }
    }
}

void HFwDetVectorFinder::moveVectors()
{
    // Drop low-resolution vectors and move high-res. ones to their container

    for (Int_t m = 0; m < nModules; ++m)
    {
        Int_t nVecs = fVectors[m].size();
        for (Int_t j = nVecs - 1; j >= fIndx0[m]; --j)
        {
            delete fVectors[m][j];
            fVectors[m].erase(fVectors[m].begin()+j);
        }

        nVecs = fVectorsHigh[m].size();
        for (Int_t j = 0; j < nVecs; ++j)
            fVectors[m].push_back(fVectorsHigh[m][j]);
        fVectorsHigh[m].clear();
    }
}

void HFwDetVectorFinder::storeVectors(Int_t isel)
{
    // Store vectors into category
    Int_t ibeg[2] = {0, nModules}, iend[2] = {nModules-1, nModules};
    HLocation loc;
    loc.set(1,0);

#ifdef VERBOSE_MODE
Int_t vec_cnt = 0;
#endif

    //for (Int_t ist = 0; ist <= nModules; ++ist) {
    for (Int_t ist = ibeg[isel]; ist <= iend[isel]; ++ist)
    {
        Int_t nvec = fVectors[ist].size();
        Int_t iv0 = (ist < nModules) ? fIndx0[ist] : 0;

        for (Int_t iv = iv0; iv < nvec; ++iv)
        {
            if (!fVectors[ist][iv]) continue;

            HVectorCand* cal = (HVectorCand*) pVectorCand->getNewSlot(loc);

            if (cal)
            {
                if (isSimulation)
                    cal = (HVectorCand*) new(cal) HVectorCandSim(*(HVectorCandSim*)(fVectors[ist][iv]));
                else
                    cal = new(cal) HVectorCand(*(fVectors[ist][iv]));
                cal->SetUniqueID(ist);
                setTrackId(cal);
#ifdef VERBOSE_MODE
                if (isel)
                {
                    if (isSimulation)
                        ((HVectorCandSim*)cal)->print();
                    else
                        cal->print();
                    ++vec_cnt;
                }
#endif
            }
        }
    }
#ifdef VERBOSE_MODE
printf("Found %d vectors.\n", vec_cnt);
#endif
}

void HFwDetVectorFinder::mergeVectors()
{
    // Merge vectors from 2 stations
    TMatrixD matr = TMatrixD(4, 4);
    TMatrixD unit(TMatrixD::kUnit, matr);
    Double_t pars[4];
    Double_t zMed = 0.0;
    for (Int_t m = 0; m < FWDET_STRAW_MAX_MODULES; ++m)
        zMed += fZ0[m];
    zMed /= FWDET_STRAW_MAX_MODULES;

    for (Int_t m = 0; m < FWDET_STRAW_MAX_MODULES; ++m)
    {
        // Propagate vectors to the median plane
        Int_t nvec = fVectors[m].size();

        for (Int_t iv = fIndx0[m]; iv < nvec; ++iv)
        {
            HVectorCand *tr = fVectors[m][iv];
            if (!tr) continue;

            tr->getParams(pars);
            Double_t zbeg = tr->getZ();
            Double_t dz = zMed - zbeg;
            pars[0] += dz * pars[2];
            pars[1] += dz * pars[3];
            zbeg += dz;
            TMatrixDSym covd = tr->getCovarMatr();
            TMatrixD ff = unit;
            ff(2, 0) = ff(3, 1) = dz;
            TMatrixD cf(covd, TMatrixD::kMult, ff);
            TMatrixD fcf(ff, TMatrixD::kTransposeMult, cf);
            covd.SetMatrixArray(fcf.GetMatrixArray());
            covd.SetTol(0.1 * covd.GetTol());
            covd.Invert(); // weight matrix
            tr->setParams(pars);
            tr->setZ(zbeg);
            tr->setCovar(covd);
        }
    }

    // Merge vectors
    Int_t m1 = 0, m2 = 1, nvec1 = fVectors[m1].size(), nvec2 = fVectors[m2].size();
    Double_t pars1[4], pars2[4];

    TMatrixDSym ** w_m1 = new TMatrixDSym*[nvec1];
    TMatrixDSym ** w_m2 = new TMatrixDSym*[nvec2];

    TMatrixD ** p_m1 = new TMatrixD*[nvec1];
    TMatrixD ** p_m2 = new TMatrixD*[nvec2];

    TMatrixD ** wp_m1 = new TMatrixD*[nvec1];
    TMatrixD ** wp_m2 = new TMatrixD*[nvec2];

    for (Int_t iv = fIndx0[m1]; iv < nvec1; ++iv)
    {
        HVectorCand *tr1 = fVectors[m1][iv];
        if (!tr1) continue;

        tr1->getParams(pars1);
        w_m1[iv] = new TMatrixDSym(tr1->getCovarMatr());
        p_m1[iv] = new TMatrixD(4, 1, pars1);
        wp_m1[iv] = new TMatrixD(*w_m1[iv], TMatrixD::kTransposeMult, *p_m1[iv]);
    }

    for (Int_t iv = fIndx0[m2]; iv < nvec2; ++iv)
    {
        HVectorCand *tr2 = fVectors[m2][iv];
        if (!tr2) continue;

        tr2->getParams(pars2);
        w_m2[iv] = new TMatrixDSym(tr2->getCovarMatr());
        p_m2[iv] = new TMatrixD(4, 1, pars2);
        wp_m2[iv] = new TMatrixD(*w_m2[iv], TMatrixD::kTransposeMult, *p_m2[iv]);
    }

    for (Int_t iv1 = fIndx0[m1]; iv1 < nvec1; ++iv1)
    {
        HVectorCand *tr1 = fVectors[m1][iv1];
        if (!tr1) continue;

        for (Int_t iv2 = fIndx0[m2]; iv2 < nvec2; ++iv2)
        {
            HVectorCand *tr2 = fVectors[m2][iv2];
            if (!tr2) continue;

            TMatrixD wp = *wp_m1[iv1] + *wp_m2[iv2];
            TMatrixDSym w = *w_m1[iv1] + *w_m2[iv2];
            w.Invert();
            TMatrixD p12(w, TMatrixD::kTransposeMult, wp);

            // Compute Chi2
            TMatrixD p122 = p12;
            TMatrixD pMerge = p12;

            p12 -= *p_m1[iv1];
            TMatrixD wDp1(*w_m1[iv1], TMatrixD::kMult, p12);
            TMatrixD chi21(p12, TMatrixD::kTransposeMult, wDp1);

            p122 -= *p_m2[iv2];
            TMatrixD wDp2(*w_m2[iv2], TMatrixD::kMult, p122);
            TMatrixD chi22(p122, TMatrixD::kTransposeMult, wDp2);

            Double_t c2 = chi21(0,0) + chi22(0,0);
            Info("MergeVectors"," Chi2: %f %f %f", chi21(0,0), chi22(0,0), c2);

            if (c2 < 0 || c2 > fLRCutChi2) continue;

            // Merged track parameters
            pars[0] = pMerge(0,0);
            pars[1] = pMerge(1,0);
            pars[2] = pMerge(2,0);
            pars[3] = pMerge(3,0);

            addTrack(m1, tr1, tr2, iv1, iv2, pars, c2, w);
        }
    }

    for (Int_t iv = fIndx0[m1]; iv < nvec1; ++iv)
    {
        delete w_m1[iv];
        delete p_m1[iv];
        delete wp_m1[iv];
    }
    for (Int_t iv = fIndx0[m2]; iv < nvec2; ++iv)
    {
        delete w_m2[iv];
        delete p_m2[iv];
        delete wp_m2[iv];
    }

    delete [] w_m1;
    delete [] w_m2;

    delete [] p_m1;
    delete [] p_m2;

    delete [] wp_m1;
    delete [] wp_m2;
}

void HFwDetVectorFinder::addTrack(Int_t m0, HVectorCand *tr1, HVectorCand *tr2, Int_t indx1, Int_t indx2, Double_t *parOk, Double_t c2, TMatrixDSym &w2)
{
    // Save merged vector

    HVectorCand * track = nullptr;
    if (isSimulation)
        track = (HVectorCand*) new HVectorCandSim();
    else
        track = new HVectorCand();

    track->setParams(parOk);
    track->setChi2(c2 + tr1->getChi2() + tr2->getChi2());
    track->setNDF(4 + tr1->getNDF() + tr2->getNDF());  // FIXME why 4?

    TBits b = fVectors[0][indx1]->getLR();
    Int_t lim1 = fVectors[0][indx1]->getNofHits();
    for (Int_t i = 0; i < lim1; ++i)
    {
        track->addHit(fVectors[0][indx1]->getHitIndex(i));
        if (b[i]) track->setLRbit(i);
    }

    b = fVectors[1][indx2]->getLR();
    Int_t lim2 = fVectors[1][indx2]->getNofHits();
    for (Int_t i = 0; i < lim2; ++i)
    {
        track->addHit(fVectors[1][indx2]->getHitIndex(i));
        if (b[i]) track->setLRbit(lim1 + i);
    }

    track->setTof(fVectors[0][indx1]->getTof());
    track->setDistance(fVectors[0][indx1]->getDistance());
    if (isSimulation)
        ((HVectorCandSim*) track)->setRpcTrack(
            ((HVectorCandSim*) fVectors[0][indx1])->getRpcTrack());

    fVectors[nModules].push_back(track);

    Info("AddTrack", "chi2=%f, merged vectors %i", track->getChi2(),track->getNofHits());
}

void HFwDetVectorFinder::selectTracks(Int_t ipass)
{
    // Remove ghost tracks (having at least N the same hits (i.e. fired tubes))

    const Int_t nMax = 8, nPl = 40, nVecMin = 2;
    Int_t planes[nPl], hinds[nPl], lr[nPl], ntrs = fVectors[nModules].size();
    multimap<Double_t,Int_t> c2merge;

    for (Int_t i = 0; i < ntrs; ++i)
    {
        HVectorCand *tr = fVectors[nModules][i];
        if (!tr) continue;

//         if (tr->getNofHits() != nVecMin) continue;   // FIXME before was two tracks (m0 and m1), now we have up to 16, how to validate proper track?
        Double_t qual = tr->getNofHits() +
        (499 - TMath::Min(tr->getChi2()/tr->getNDF(),499.0)) / 500;
        c2merge.insert(pair<Double_t,Int_t>(-qual,i));
    }

    Int_t nSel = 0;
    multimap<Double_t,Int_t>::iterator it, it1;
    for (it = c2merge.begin(); it != c2merge.end(); ++it)
    {
        HVectorCand *tr = fVectors[nModules][it->second];
        if (!tr) continue;

        Int_t nvecs1 = tr->getNofHits();
        for (Int_t j = 0; j < nPl; ++j)
            planes[j] = hinds[j] = lr[j] = -1;
        Int_t patt = 0;

        for (Int_t iv = 0; iv < nvecs1; ++iv)
        {
            HFwDetStrawCal *hit = (HFwDetStrawCal*) pStrawHits->getObject(tr->getHitIndex(iv));
            Int_t ipl = hit->getVPlane();
            planes[ipl] = tr->getHitIndex(iv);
            patt |= (1 << ipl);
            hinds[ipl] = tr->getHitIndex(iv);
            if (tr->getLR().TestBitNumber(iv)) lr[ipl] = 1;
        }

        // Refit whole line
        Double_t pars[4], chi2;
        TMatrixDSym cov(4);
        chi2 = refit(patt, hinds, pars, &cov, lr, tr->getTof(), tr->getDistance());
        if (chi2 < fHRCutChi2)
        {
            cov *= (fErrU * fErrU);
            Info("Select tracks", "Track chi2: %f",chi2);
            tr->setChi2(chi2);
            tr->setParams(pars);
            tr->setCovar(cov);
            tr->setZ(fZ0[0]);

            if (isSimulation)
            {
                Int_t hidx = 0;
                HFwDetStrawCalSim * hit = nullptr;
                Float_t px, py, pz, x, y, z;

                tr->getHitIndex(0);
                hit = (HFwDetStrawCalSim*) pStrawHits->getObject(hidx);

                hit->getP(px, py, pz);
                hit->getHitPos(x, y, z);
                if (isSimulation)
                {
                    HVectorCandSim* trs = (HVectorCandSim*)tr;
                    trs->setPx1(px);
                    trs->setPy1(py);
                    trs->setPz1(pz);
                    trs->setX1(x);
                    trs->setY1(y);
                    trs->setZ1(z);
                }

                hidx = tr->getHitIndex(tr->getNofHits()-1);
                hit = (HFwDetStrawCalSim*) pStrawHits->getObject(hidx);
                hit->getP(px, py, pz);
                hit->getHitPos(x, y, z);
                if (isSimulation)
                {
                    HVectorCandSim* trs = (HVectorCandSim*)tr;
                    trs->setPx2(px);
                    trs->setPy2(py);
                    trs->setPz2(pz);
                    trs->setX2(x);
                    trs->setY2(y);
                    trs->setZ2(z);
                }
            }

            if (tr->getTof() != -1.0)
                tr->calc4vectorProperties(0.0);
            else
                tr->calc4vectorProperties(1.0, 0.0);
            ++nSel;
        }
        else
        {
            delete tr;
            tr = nullptr;
            fVectors[nModules][it->second] = nullptr;
        }
    }

    for (it = c2merge.begin(); it != c2merge.end(); ++it)
    {
        HVectorCand *tr1 = fVectors[nModules][it->second];
        if (!tr1) continue;

        Int_t nvecs1 = tr1->getNofHits();
        for (Int_t j = 0; j < nPl; ++j) planes[j] = hinds[j] = lr[j] = -1;
        Int_t patt = 0;

        for (Int_t iv = 0; iv < nvecs1; ++iv)
        {
            HFwDetStrawCal *hit = (HFwDetStrawCal*) pStrawHits->getObject(tr1->getHitIndex(iv));
            Int_t ipl = hit->getVPlane();
            planes[ipl] = tr1->getHitIndex(iv);
            patt |= (1 << ipl);
            hinds[ipl] = tr1->getHitIndex(iv);
            if (tr1->getLR().TestBitNumber(iv)) lr[ipl] = 1;
        }

        it1 = it;
        for (++it1; it1 != c2merge.end(); ++it1)
        {
            HVectorCand *tr2 = fVectors[nModules][it1->second];
            if (!tr2) continue;

            Int_t nvecs2 = tr2->getNofHits(), nover = 0;
            Bool_t over = kFALSE;

            for (Int_t iv = 0; iv < nvecs2; ++iv)
            {
                HFwDetStrawCal *hit = (HFwDetStrawCal*) pStrawHits->getObject(tr2->getHitIndex(iv));
                Int_t ipl = hit->getVPlane();
                if (planes[ipl] < 0) continue;
                if (planes[ipl] == tr2->getHitIndex(iv))
                {
                    ++nover;
                    if (nover >= nMax)
                    {
                        Info("Select tracks", "Track quality: qual1 %f, qual2 %f, overlaps: %i", it->first,it1->first,nover);

                        if (tr1->getChi2() < tr2->getChi2())
                        {
                            delete fVectors[nModules][it1->second];
                            fVectors[nModules][it1->second] = nullptr;
                        }
                        else
                        {
                            delete fVectors[nModules][it->second];
                            fVectors[nModules][it->second] = nullptr;
                        }
                        over = kTRUE;
                        break;
                    }
                }
                if (over) break;
            }
        }
    }

    // Apply extra slection cuts
    if (ipass < fNpass - 1 && nSel > 1)
    {
        for (Int_t i = 0; i < ntrs; ++i)
        {
            HVectorCand *tr = fVectors[nModules][i];
            if (!tr) continue;

            if (tr->getChi2() / tr->getNDF() <= 3.0)
            {
                // Flag used hits
                Int_t nvecs1 = tr->getNofHits();
                for (Int_t iv = 0; iv < nvecs1; ++iv)
                {
                    HVectorCand *vec = fVectors[iv][tr->getHitIndex(iv)];
                    if (!vec) continue;

                    Int_t nh = vec->getNofHits();
                    for (Int_t ih = 0; ih < nh; ++ih)
                    {
                        HFwDetStrawCal *hit = (HFwDetStrawCal*) pStrawHits->getObject(vec->getHitIndex(ih));
                        hit->SetUniqueID(999);
                    }
                }
                continue;
            }
            else
            {
                delete fVectors[nModules][i];
                fVectors[nModules][i] = nullptr;
            }
        }
    }
}

Double_t HFwDetVectorFinder::refit(Int_t patt, Int_t *hinds, Double_t *pars, TMatrixDSym *cov, Int_t *lr, Double_t tof, Double_t tofl)
{
    // Fit of hits to the straight line

    // Solve system of linear equations
    Bool_t ok = kFALSE, onoff;
    TVectorD b(4);


    Double_t tof_grad = 0.0;

    if (tof != -1.0)
    {
        tof_grad = tof / tofl;
    }

    for (Int_t i = 0; i < FWDET_STRAW_MAX_VPLANES; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;

        HFwDetStrawCal *hit = (HFwDetStrawCal*) pStrawHits->getObject(hinds[i]);

        Float_t time = hit->getTime();

        if (tof != -1.0)
        {
            Float_t drift_time = time - tof_grad * fDz[i];
            fDrift[i] =  calcDriftRadius(drift_time);
        }
        else
        {
            fDrift[i] = 2.5;
        }

#ifdef VERBOSE_MODE
// printf("HR/* dt*/: %f -> %f (%f), grad=%f, z=%f\n", time,  drift_time, fDrift[i], tof_grad, fDz[i]);
#endif

        Double_t uc = (hit->getU() + lr[i] * fDrift[i]) * fCosa[i]; // resolved ambiguity !!!
        Double_t us = (hit->getU() + lr[i] * fDrift[i]) * fSina[i]; // resolved ambiguity !!!
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

    Double_t chi2 = 0;
    for (Int_t i = 0; i < FWDET_STRAW_MAX_VPLANES; ++i)
    {
        onoff = patt & (1 << i);
        if (!onoff) continue;

        Double_t x = pars[0] + pars[2] * fDz[i];
        Double_t y = pars[1] + pars[3] * fDz[i];
        Double_t u = x * fCosa[i] + y * fSina[i];
        HFwDetStrawCal *hit = (HFwDetStrawCal*) pStrawHits->getObject(hinds[i]);
        Double_t du = (u - hit->getU() - lr[i] * fDrift[i]) / fErrU;
        chi2 += du * du;
    }
    return chi2;
}

Int_t HFwDetVectorFinder::matchRpcHit(Double_t* params, Double_t z)
{
    // Match RPC hit to the track in straws
    if (!pRpcHits)
        return -1;

    Float_t x_rpc;
    Float_t y_rpc;
    Float_t z_rpc;
    Float_t tof_rpc;

    Int_t nHits = pRpcHits->getEntries();

    if (nHits == 0)
        return -1;

    HFwDetRpcHit *hit;

    Double_t min_dist = 200.0;
    Int_t sel_hit = -1;

    for (Int_t i = 0; i < nHits; ++i)
    {
        hit = (HFwDetRpcHit*) pRpcHits->getObject(i);

        hit->getHit(x_rpc, y_rpc, z_rpc, tof_rpc);

        Double_t dd = z_rpc - z;

        Double_t x_atz_rpc = params[0] + params[2]*dd;
        Double_t y_atz_rpc = params[1] + params[3]*dd;

        TVector2 v_ext(x_atz_rpc, y_atz_rpc);
        TVector2 v_rpc(x_rpc, y_rpc);
        TVector2 v_dist = v_ext - v_rpc;
        Double_t dist = v_dist.Mod();

        if (dist < min_dist)
        {
            min_dist = dist;
            sel_hit = i;
#ifdef VERBOSE_MODE
printf(" (%02d/%02d) RPCHIT   x=%f y=%f z=%.3f tof=%f    for %f,%f (%f,%f) -> %f,%f at %f=%f+%f  with %f\n", i, nHits, x_rpc, y_rpc, z_rpc, tof_rpc, params[0], params[1], params[2], params[3], x_atz_rpc, y_atz_rpc, z_rpc, z, dd, dist);
#endif
        }
    }

    return sel_hit;
}

Float_t HFwDetVectorFinder::calcDriftRadius(Float_t r) const
{
    // Calculate drift radius using drift time
    Float_t r2 = r * r;
    Float_t r3 = r2 * r;
    Float_t r4 = r3 * r;

    Float_t dt = dt_p[0] + dt_p[1]*r + dt_p[2]*r2 + dt_p[3]*r3 + dt_p[4]*r4;
    return dt;
}

ClassImp(HFwDetVectorFinder);
