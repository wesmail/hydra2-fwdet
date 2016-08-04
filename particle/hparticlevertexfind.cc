//*-- Author : M.Sanchez (14.06.2000)
#include "hparticlevertexfind.h"
#include "hcategory.h"
#include "hmdcgeompar.h"
#include "hades.h"
#include "htool.h"
#include "hevent.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hgeomvector.h"
#include "hparticlecand.h"
#include "hgeomvolume.h"
#include "hrecevent.h"
#include "hpartialevent.h"

#include "hcategorymanager.h"
#include "hparticletool.h"
#include "hparticledef.h"

#include "TMath.h"


#include <vector>

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////
// HParticleVertexFind
//
//   Vertex finder using weighted LSM with optional Tukey weights
//
// To use it in a macro just add
//
// taskset->add(new HParticleVertexFind("name","title",tukey)
//
// where  input data is read from HParticleCand (r,z,phi,theta from RK)
// Only candidate flagged kIsUsed are taken into account . Inner segment
// is require to be fitted and RK chi2 < 1000. Candidates markes as fakes
// are skipped by default.
//
// As for 'tukey' it can be kTRUE or kFALSE depending on if Tukey weights
// should be used in the minimization. For low multiplicity environments
// like C+C I would recomend kFALSE; at least until I have found better
// parameters for the Tukey weigths minimization.
//
/////////////////////////////////////////

Bool_t  HParticleVertexFind::doSkipFakes = kTRUE;

HParticleVertexFind::HParticleVertexFind(const Text_t name[],const Text_t title[], Bool_t tukey) :
    HReconstructor(name,title),fPos("HGeomVector",300),
    fAlpha("HGeomVector",300)
{
    initVars();
    useTukeyWeights(tukey);
}

HParticleVertexFind::~HParticleVertexFind(void)
{
    initVars();
}

void HParticleVertexFind::initVars(void)
{
    fInput         = 0;
    fGeometry      = 0;
    fMaxIterations = 100;
    fTukeyConst    = 6.0;
    fEpsilon       = .3;
    fUsingTukey    = kTRUE;

    fmomChi2Cut  = 100;
    fseg0Chi2Cut = 6;  // cut 10%
    fcallExecuteManual = kFALSE;
    setCut(-120,20,15);
    setMinReqTracks(10);
    setMinWindow(2);
    fProgressiveTukey = kFALSE;
}

Bool_t HParticleVertexFind::init(void)
{
    HRuntimeDb *rtdb = gHades->getRuntimeDb();
    HRecEvent *ev    = dynamic_cast<HRecEvent *>(gHades->getCurrentEvent());
    if(!ev) {return kFALSE;}

    fInput = gHades->getCurrentEvent()->getCategory(catParticleCand);
    if(!fInput) { Warning("init()","No catParticleCand category in input!");}

    if(fInput){
	fGeometry     = (HMdcGeomPar *) rtdb->getContainer("MdcGeomPar");
	fSpecGeometry = (HSpecGeomPar *)rtdb->getContainer("SpecGeomPar");
    }
    return kTRUE;
}

Bool_t HParticleVertexFind::finalize(void)
{
    return kTRUE;
}

Bool_t HParticleVertexFind::pointsToTarget(const HGeomVector &r,HGeomVector &alpha,
					   Int_t sector,Int_t module, Double_t locminZ,Double_t locmaxZ)
{
    Double_t bx = alpha.getX() / alpha.getZ();
    Double_t by = alpha.getY() / alpha.getZ();
    Bool_t res  = kTRUE;

    HGeomVector rmin;

    //Evaluate maximum approach to z axis
    rmin.setZ(r.getZ() - (r.getX() * bx + r.getY() * by) / (bx * bx + by * by));
    rmin.setX(r.getX() + bx * (rmin.getZ() - r.getZ()));
    rmin.setY(r.getY() + by * (rmin.getZ() - r.getZ()));
    Double_t rhomin = TMath::Sqrt(rmin.getX() * rmin.getX() +
				  rmin.getY() * rmin.getY());


    if(rmin.getZ() < locmaxZ && rmin.getZ() > locminZ && rhomin < fmaxR) {res = kTRUE;}
    else                                                                 {res = kFALSE;}

    return res;
}

void HParticleVertexFind::transform(HParticleCand *hit,
				    HGeomVector &r,HGeomVector &alpha)
{

    //Calculates position and direction vector for a Segment (theta,phi.r,alpha)

    Float_t phi   =  HParticleTool::phiLabToPhiSecDeg(hit->getPhi())*TMath::DegToRad();
    Float_t theta =  hit->getTheta() * TMath::DegToRad();

    Double_t pi2 = TMath::Pi() / 2.;

    r.setZ(hit->getZ());
    r.setX(hit->getR() * TMath::Cos(phi + pi2));
    r.setY(hit->getR() * TMath::Sin(phi + pi2));
    alpha.setZ(TMath::Cos(theta));
    alpha.setY(TMath::Sin(theta) * TMath::Sin(phi));
    alpha.setX(TMath::Sin(theta) * TMath::Cos(phi));
}

Bool_t HParticleVertexFind::readInput(HGeomVector &estimate)
{
    HGeomVector *r,*alpha,rLocal,alphaLocal;
    HParticleCand *cand = 0;
    Int_t count   = 0;
    fCands  .clear();
    fweights.clear();
    if(fInput)
    {
	//First vertex estimation and filling of fPos, fAlpha.


        //---------------------------------------------
        // get x,y shift of beam
	HVertex& vertexclust = gHades->getCurrentEvent()->getHeader()->getVertexCluster();
        Double_t locmaxZ = fmaxZ;
	Double_t locminZ = fminZ;
	Double_t xClust  = 0.;
	Double_t yClust  = 0.;

	if(vertexclust.getZ() != -1000){ // was filled
             xClust  = vertexclust.getX();
             yClust  = vertexclust.getY();
	}
        //---------------------------------------------

	fFitter.reset();

	Int_t n = fInput->getEntries();
        vector <HParticleCand*> usedCand;
        vector <HParticleCand*> usedCandFinal;

        //---------------------------------------------
	// fill starting list
	for(Int_t i = 0; i < n; i ++)
	{
	    cand = HCategoryManager::getObject(cand,fInput,i);
	    if( cand->isFakeRejected())   continue;
	    if(!cand->isFlagBit(kIsUsed)) continue;

	    if(cand->getZ() < locmaxZ && cand->getZ() > locminZ && cand->getR() < fmaxR) {
		usedCand.push_back(cand);
	    }
	}
        //---------------------------------------------

	if (usedCand.size() > 10) {  // we have enough tracks

	    vector <HParticleCand*> usedCandTmp;
	    vector <HParticleCand*> usedCandTmp2;
            vector<Double_t> values;
	    Double_t mean = 0;

	    for(UInt_t i = 0; i < usedCand.size() ; i ++){
                cand = usedCand[i];
		if(cand->getChi2()             < getMomChi2Cut()  &&
		   cand->getInnerSegmentChi2() < getSeg0Chi2Cut() &&
                   cand->getBeta() > 0 && cand->getBeta() < 1.2
		  ) {
		    usedCandFinal.push_back(cand);
		    usedCandTmp2.push_back(cand);
		    values.push_back(cand->getZprime(xClust,yClust));
		}
	    }
	    mean = TMath::Mean(values.size(),values.data());

	    //------------------------------------------------------
            // filter more
	    if(usedCandFinal.size() > 0)
	    {
                
		Double_t accepted    = usedCandFinal.size();
                Double_t window      = 40;
                Int_t maxIteration   = 20;
		Double_t fac         = 0.8;
		Int_t it = 0;

		while (accepted >= fminReqTrack && window >= fminWindow && it < maxIteration){
                    it++;
		    usedCandTmp.clear();
		    accepted = 0;
                    values.clear();
		    //------------------------------------------------------
                    // calculate new mean + accepted tracks
		    for(UInt_t i = 0; i < usedCandFinal.size() ; i ++){
			cand = usedCandFinal[i];
			Double_t zprime = cand->getZprime(xClust,yClust) ;
			if( TMath::Abs(zprime - mean) < window){
                            values.push_back(zprime);
			    usedCandTmp.push_back(cand);
			}
		    }
		    accepted = values.size();
		    //------------------------------------------------------

		    //------------------------------------------------------
		    // worked out : copy pointers
		    if(accepted >= fminReqTrack) {
			usedCandTmp2.clear();
			for(UInt_t i = 0; i < usedCandTmp.size() ; i ++){
			    usedCandTmp2.push_back(usedCandTmp[i]);
			}
			mean = TMath::Mean(values.size(),values.data());
			window *= fac; // shrink the window
		    }
		    //------------------------------------------------------

		} // end while
	    }
            //------------------------------------------------------

	    //------------------------------------------------------
            // copy to output
            usedCandFinal.clear();
	    for(UInt_t i = 0; i < usedCandTmp2.size() ; i ++){
		usedCandFinal.push_back(usedCandTmp2[i]);
	    }
            //------------------------------------------------------



	} else {  // do not cut sharp at low mult
	    for(UInt_t i = 0; i < usedCand.size() ; i ++){
		usedCandFinal.push_back(usedCand[i]);
	    }

	}

	for(UInt_t i = 0; i < usedCandFinal.size(); i ++)
	{
            cand = usedCandFinal[i] ;

	    Int_t s = cand->getSector();

	    HGeomTransform &trans = fSpecGeometry->getSector(s)->getTransform();

	    transform(cand,rLocal,alphaLocal);

	    r    = new(fPos  [count]) HGeomVector(trans.transFrom(rLocal));
	    alpha= new(fAlpha[count]) HGeomVector(trans.getRotMatrix() * alphaLocal);

	    //Feed to LSM fitter.
	    if(pointsToTarget(*r,*alpha,s,0,locminZ,locmaxZ))
	    {
		fFitter.addLine(*r,*alpha); //Default weigth =1.0
		count ++;
                fCands.push_back(cand);
                fweights.push_back(1.);
	    }
	}
    }

    if(count > 1){
	fFitter.getVertex(estimate);
    } else {
	estimate.setXYZ(-1000,-1000,-1000);
	return kFALSE;
    }
    return kTRUE;
}

Int_t HParticleVertexFind::execute(void)
{
      if (!fcallExecuteManual) return doFit();
      return 0;

}

Int_t HParticleVertexFind::doFit()
{
    HGeomVector *r,*alpha;
    HGeomVector oldVertex;
    HVertex     &event_vertex = gHades->getCurrentEvent()->getHeader()->getVertexReco();
    HGeomVector &vertex       = event_vertex.getPos();



    Int_t count = 0,iteration = 0;
    Double_t weight,residual2,temp; //Weight and residual^2

    readInput(vertex);


    if(fUsingTukey)
    {
	count = fPos.GetEntriesFast();

	Double_t locTukeyConst = fTukeyConst;

	if(fProgressiveTukey){
	    //--------------------------------------------------------
	    // progressive tukey constants
	    Double_t minTuk = 1.0;
	    Double_t maxTuk = fTukeyConst;
	    Int_t   minMult = 10;
	    Int_t   maxMult = 30;

	    if(count >= minMult && count < maxMult ){
		locTukeyConst = maxTuk - ((maxTuk-minTuk)/(maxMult-minMult)) * (count-maxMult);

	    } else if (count < minMult) {

		locTukeyConst = maxTuk;

	    } else if (count >= maxMult) {

		locTukeyConst = minTuk;
	    }
	    //--------------------------------------------------------
	}

	if(count > 1 && (vertex.Z()!=-1000. && vertex.X()!=-1000. && vertex.Y()!=-1000.))
	{
	    //Iteration on weights
	    Float_t sumOfResiduals = 0;
	    Float_t sumOfWeights   = 0;
	    for (Int_t i = 0; i < count; i ++) {
		r     = (HGeomVector *)fPos.UncheckedAt(i);
		alpha = (HGeomVector *)fAlpha.UncheckedAt(i);
		temp  = ((r->getY() - vertex.getY()) * alpha->getZ() -
			 (r->getZ() - vertex.getZ()) * alpha->getY());
		residual2 = temp * temp;
		temp = ((r->getZ() - vertex.getZ()) * alpha->getX() -
			(r->getX() - vertex.getX()) * alpha->getZ());
		residual2 += temp * temp;
		temp = ((r->getX() - vertex.getX()) * alpha->getY() -
			(r->getY() - vertex.getY()) * alpha->getX());
		residual2 += temp * temp;
		sumOfResiduals += residual2;
	    }

	    Float_t width = locTukeyConst * sqrt(sumOfResiduals);


	    iteration = 0;
	    do {
		sumOfResiduals = 0;
		sumOfWeights   = 0;
		oldVertex      = vertex;
		iteration ++;
		fFitter.reset();
		for (Int_t i = 0; i < count; i ++)
		{
		    r     = (HGeomVector *)fPos.UncheckedAt(i);
		    alpha = (HGeomVector *)fAlpha.UncheckedAt(i);
		    temp  = ((r->getY() - oldVertex.getY()) * alpha->getZ() -
			     (r->getZ() - oldVertex.getZ()) * alpha->getY());
		    residual2 = temp * temp;
		    temp = ((r->getZ() - oldVertex.getZ()) * alpha->getX() -
			    (r->getX() - oldVertex.getX()) * alpha->getZ());
		    residual2 += temp * temp;
		    temp = ((r->getX() - oldVertex.getX()) * alpha->getY() -
			    (r->getY() - oldVertex.getY()) * alpha->getX());
		    residual2 += temp * temp;



		    temp   = sqrt(residual2);
		    weight = (temp < width) ? (1. - temp / width * temp / width)  * (1. - temp/width * temp/width) : 0.0;
                    fweights[i]=weight;

		    sumOfResiduals += weight*residual2;
		    sumOfWeights   += weight;
		    fFitter.addLine(*r,*alpha,weight);
                }

                width = locTukeyConst * sqrt(sumOfResiduals / sumOfWeights);

                fFitter.getVertex(vertex);

            } while(!hasConverged(vertex,oldVertex) && (iteration < fMaxIterations));


          if(iteration < fMaxIterations){
	      event_vertex.setIterations(iteration);
	      event_vertex.setChi2(sumOfResiduals / sumOfWeights);
	      event_vertex.setSumOfWeights(sumOfWeights);
	  } else {  // iteration >= fMaxIterations
	      event_vertex.setIterations(-2);
	      event_vertex.setChi2(-1);
	      event_vertex.setSumOfWeights(-1);
	  }
       } else {  // count <= 1  or vertex fit failed
          event_vertex.setIterations(-1);
	  event_vertex.setChi2(-1);
	  event_vertex.setSumOfWeights(-1);
       }
     } else {  // !fUsingTukey
       event_vertex.setIterations(1);
       event_vertex.setChi2(-1);
       event_vertex.setSumOfWeights(-1);
     }

     for (UInt_t i = 0; i < fCands.size(); i ++){
       if(fweights[i]>0) fCands[i]->setUsedVertex();
     }

     fPos.Clear();
     fAlpha.Clear();

     return 0;
}

Bool_t HParticleVertexFind::hasConverged(HGeomVector &v,HGeomVector &oldV) {
    Bool_t r =((v - oldV).length() < fEpsilon) ? kTRUE:kFALSE;
    return r;
}

ClassImp(HParticleVertexFind)
