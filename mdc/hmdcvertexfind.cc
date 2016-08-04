//*-- Author : M.Sanchez (14.06.2000)
#include "hmdcvertexfind.h"
#include "hcategory.h"
#include "hmdcgeompar.h"
#include "hades.h"
#include "hevent.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hgeomvector.h"
#include "hmdcdef.h"
#include "hmdcseg.h"
#include "hmdcsegsim.h"
#include "hmdchit.h"
#include "hmdchitsim.h"
#include "hgeomvolume.h"
#include "hrecevent.h"
#include "hpartialevent.h"
#include "hgeantdef.h"
#include "hgeantkine.h"
#include "TNtuple.h"

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////
// HMdcVertexFind
//
//   Vertex finder using weighted LSM with optional Tukey weights
//
// To use it in a macro just add
//
// taskset->add(new HMdcVertexFind("name","title",input_mode,tukey)
//
// where
//
// input_mode is one of HMdcVertexFind::kSegments or HMdcVertexFind::kHits.
// In the first case HMdcSeg objecst will be used as input (recomended for
// now) while in the secon HMdcHit objects will be used
//
// As for 'tukey' it can be kTRUE or kFALSE depending on if Tukey weights
// should be used in the minimization. For low multiplicity environments
// like C+C I would recomend kFALSE; at least until I have found better
// parameters for the Tukey weigths minimization.
//
//  static void setRejectEmbeddedTracks(Bool_t reject = kTRUE){rejectEmbeddedTracks = reject;}
//  static void setUseEventSeqNumber   (Bool_t use    = kTRUE){useEventSeqNumber    = use;   }
//  static void setSkipNoVertex        (Bool_t skip   = kTRUE){doSkipNoVertex       = skip;  }
//
// For the track embedding the are several configurations possible using
// the above static member functions.
// MODE:
//       1. THIS IS CLEARLY THE PREFERED METHOD IF ON NEEDS TO GET SIMULATED PARTICLES
//          AT EXACTLY THE SAME VERTEX AS THE REAL DATA!
//          READING vertex from HGeantKine objects (sim done with real vertices)
//          In this mode (useEventSeqNumber == kTRUE (default)) the event sequence number
//          of the real events which have been used to create the sim tracks with
//          PLUTO will be stored in the HGeantKine::userVal of the particles. If the
//          current event does not match the one from GEANT the event will be skipped as
//          Long_t as the numbers match again.
//       2. useEventSeqNumber == kFALSE. In this mode rejectEmbeddedTracks == kTRUE (default)
//          segments which keep GEANT track numbers and more than one contributing track number
//          will be skipped (minimize the influence of the embedded tracks to the vertex calculation).
//          Anyway the vertex will never be exactly the same as the one from the real dsts. Eventually
//          everything will go out of sync with the vertex of the sim input (if used real vertex with PLUTO)
//          because an additional event will skipped.
//          With rejectEmbeddedTracks == kFALSE all segments will be taken into account.
//       3. With doSkipNoVertex == kTRUE (default : kFALSE) events without calculated vertex will be skipped.
//          This mode has only influence if MODE 1 is not used, because here the events will be skipped.
//
/////////////////////////////////////////

Bool_t HMdcVertexFind::rejectEmbeddedTracks = kTRUE;
Bool_t HMdcVertexFind::useEventSeqNumber    = kTRUE;
Bool_t HMdcVertexFind::doSkipNoVertex       = kFALSE;
Bool_t HMdcVertexFind::doSkipFakeSegments   = kTRUE;
HMdcVertexFind::HMdcVertexFind(const Text_t name[],const Text_t title[],EInputMode m,
			       Bool_t tukey) :
    HReconstructor(name,title),fPos("HGeomVector",200),
    fAlpha("HGeomVector",200)
{
    initVars();
    useTukeyWeights(tukey);
    fInputMode = m;
}

HMdcVertexFind::~HMdcVertexFind(void)
{
    if(geantKineIter) {delete geantKineIter;}
    initVars();
}

void HMdcVertexFind::initVars(void)
{
    fInput         = 0;
    fGeometry      = 0;
    fIter          = 0;
    fMaxIterations = 100;
    fTukeyConst    = 6.0;
    fEpsilon       = .3;
    fUsingTukey    = kTRUE;
    fDebugMode     = kFALSE;
    fInputMode     = HMdcVertexFind::kSegments;
    isEmbedding    = kFALSE;
    geantKineIter  = 0;

    fseg0Chi2Cut       = 6;  // cut 10%
    fcallExecuteManual = kFALSE;
    setCut(-120,20,15);
    setMinReqTracks(10);
    setMinWindow(2);
    fProgressiveTukey = kFALSE;

}

Bool_t HMdcVertexFind::init(void)
{
    HRuntimeDb *rtdb = gHades->getRuntimeDb();
    HRecEvent *ev    = dynamic_cast<HRecEvent *>(gHades->getCurrentEvent());
    if(!ev) {return kFALSE;}

    switch(fInputMode)
    {
    case kSegments:
	fInput = gHades->getCurrentEvent()->getCategory(catMdcSeg);
	break;
    case kHits:
	fInput = gHades->getCurrentEvent()->getCategory(catMdcHit);
	break;
    default:
	fInput = 0;
    }
    if(!fInput) {return kFALSE;}
    //  else fIter=(HIterator *)fInput->MakeIterator();
    fGeometry     = (HMdcGeomPar *) rtdb->getContainer("MdcGeomPar");
    fSpecGeometry = (HSpecGeomPar *)rtdb->getContainer("SpecGeomPar");

    if(gHades->getOutputFile())
    {
	gHades->getOutputFile()->cd();
	//fData----
	// x,y,z,n : posicion del vertice en la iteracion n
	// con : converged??
	//fData=new TNtuple("Data","Data","x:y:z:n:con");
	if(fDebugMode) {fControl = new TNtuple("con","control","rho:dx:dy:dz:sector:module");}
    }

    //-----------------------------------------------
    // find out if we are running track embedding mode
    // This flag is later used to reject the embedded
    // simulated tracks from the vertex calculation
    isEmbedding = kFALSE;
    HCategory* geantKineCat = (HCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);
    if(gHades->getEmbeddingMode() > 0 && geantKineCat)
    {
	isEmbedding   = kTRUE;
	geantKineIter = geantKineCat->MakeIterator();
    }
    //-----------------------------------------------

    return kTRUE;
}

Bool_t HMdcVertexFind::finalize(void)
{
    return kTRUE;
}

Bool_t HMdcVertexFind::pointsToTarget(const HGeomVector &r,HGeomVector &alpha,
				      Int_t sector,Int_t module, Double_t locminZ,Double_t locmaxZ)
{
    //Gilipollas el HMdcSeg ya tiene esta informacion.....
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

    //Impose condition
    if(fDebugMode){
	fControl->Fill(rhomin,rmin.getX(),rmin.getY(),rmin.getZ(),sector,module);
    }

    if(rmin.getZ() < locmaxZ && rmin.getZ() > locminZ && rhomin < fmaxR) {res = kTRUE;}
    else                                                                 {res = kFALSE;}

    return res;
}

void HMdcVertexFind::transform(HMdcSeg *hit,
			       HGeomVector &r,HGeomVector &alpha)
{
    //Calculates position and direction vector for a HMdcHit
    Float_t theta,phi;
    Double_t pi2 = TMath::Pi() / 2.;

    theta = hit->getTheta();
    phi   = hit->getPhi();
    r.setZ(hit->getZ());
    r.setX(hit->getR() * TMath::Cos(phi + pi2));
    r.setY(hit->getR() * TMath::Sin(phi + pi2));
    alpha.setZ(TMath::Cos(theta));
    alpha.setY(TMath::Sin(theta) * TMath::Sin(phi));
    alpha.setX(TMath::Sin(theta) * TMath::Cos(phi));
}

Bool_t HMdcVertexFind::readSegments(HGeomVector &estimate)
{
    HGeomVector *r,*alpha,rLocal,alphaLocal;
    HMdcSeg *data = 0;
    Int_t count   = 0;
    fInput        = gHades->getCurrentEvent()->getCategory(catMdcSeg);



    if(fInput)
    {
	//First vertex estimation and filling of fPos, fAlpha.

	Double_t locmaxZ = fmaxZ;
	Double_t locminZ = fminZ;

	fIter = (HIterator *)fInput->MakeIterator();
	fIter->Reset();
	fFitter.reset();

	vector <HMdcSeg*> usedCand;
        vector <HMdcSeg*> usedCandFinal;

	while( (data = (HMdcSeg *)fIter->Next()) != 0)
	{
	    // Transform from MDC system to LAB system
	    if(data->getIOSeg() == 0 && data->getChi2() >= 0)
	    {
                if(doSkipFakeSegments && data->isFake()) continue;

		if(isEmbedding && rejectEmbeddedTracks)
		{
		    // in embedding mode we have to reject the
		    // embedded tracks from vertex calculations

		    HMdcSegSim* datasim = (HMdcSegSim*) data;
		    if( (datasim->getTrack(0) > 0 && datasim->getNTracks() == 1) ||
		         datasim->getNTracks() > 1
		      )
		    {
			continue;
		    }
		}


		//---------------------------------------------
		// fill starting list
		if(data->getZ() < locmaxZ && data->getZ() > locminZ && data->getR() < fmaxR) {
		    usedCand.push_back(data);
		}
		//---------------------------------------------
	    }
	}
	delete fIter;


	if (usedCand.size() > 10) {  // we have enough tracks

	    vector <HMdcSeg*> usedCandTmp;
	    vector <HMdcSeg*> usedCandTmp2;
            vector<Double_t> values;
	    Double_t mean = 0;

	    for(UInt_t i = 0; i < usedCand.size() ; i ++){
                data = usedCand[i];
		if(data->getChi2() < getSeg0Chi2Cut()
		  ) {
		    usedCandFinal.push_back(data);
		    usedCandTmp2.push_back(data);
		    values.push_back(data->getZprime());
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
			data = usedCandFinal[i];
			Double_t zprime = data->getZprime() ;
			if( TMath::Abs(zprime - mean) < window){
                            values.push_back(zprime);
			    usedCandTmp.push_back(data);
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
            data = usedCandFinal[i] ;

	    HGeomTransform &trans = fSpecGeometry->getSector(data->getSec())->getTransform();

	    transform(data,rLocal,alphaLocal);

	    r    = new(fPos  [count]) HGeomVector(trans.transFrom(rLocal));
	    alpha= new(fAlpha[count]) HGeomVector(trans.getRotMatrix() * alphaLocal);
	    //Feed to LSM fitter.
	    if(pointsToTarget(*r,*alpha,data->getSec(),0,locminZ,locmaxZ))
	    {
		fFitter.addLine(*r,*alpha); //Default weigth =1.0
		count ++;
	    }
	}


	if(count > 1){
	    fFitter.getVertex(estimate);  // will be  -1000,-1000,-1000 in case of fail
	} else {
	    estimate.setXYZ(-1000,-1000,-1000);
	    // in case we are running embedding we
	    // have to skip events with no vertex
	    if(isEmbedding){return kFALSE;}
	}

    } else {return kFALSE;}  // no input
    return kTRUE;
}

Bool_t HMdcVertexFind::readHits(HGeomVector &estimate)
{
    HGeomVector *r,*alpha,rLocal,alphaLocal;
    HMdcHit *data = 0;
    Int_t count   = 0,sector,module;
    Double_t dx,dy;

    fInput = gHades->getCurrentEvent()->getCategory(catMdcHit);
    if(fInput)
    {
	//First vertex estimation and filling of fPos, fAlpha.
	Double_t locmaxZ = fmaxZ;
	Double_t locminZ = fminZ;


	fIter = (HIterator *)fInput->MakeIterator();
	fIter->Reset();
	fFitter.reset();

	while( (data = (HMdcHit *)fIter->Next()) != 0)
	{
	    // Transform from MDC system to LAB system

	    if(isEmbedding && rejectEmbeddedTracks)
	    {
		// in embedding mode we have to reject the
		// embedded tracks from vertex calculations
		HMdcHitSim* datasim = (HMdcHitSim*) data;
		if( (datasim->getTrack(0) > 0 && datasim->getNTracks() == 1) ||
		     datasim->getNTracks() > 1
		  )
		{
		    continue;
		}
	    }

	    data->getSecMod(sector,module);
	    HGeomTransform &trans = fGeometry->getModule(sector,module)->getLabTransform();
	    rLocal.setX(data->getX());
	    rLocal.setY(data->getY());
	    rLocal.setZ(0.);
	    dx = data->getXDir();
	    dy = data->getYDir();
	    alphaLocal.setX(dx);
	    alphaLocal.setY(dy);
	    alphaLocal.setZ(TMath::Sqrt(1. - dx*dx - dy*dy));
	    r    = new(fPos  [count]) HGeomVector(trans.transFrom(rLocal));
	    alpha= new(fAlpha[count]) HGeomVector(trans.getRotMatrix() * alphaLocal);
	    //Feed to LSM fitter.
	    if(pointsToTarget(*r,*alpha,sector,module,locminZ,locmaxZ)){
		fFitter.addLine(*r,*alpha); //Default weigth =1.0
		count ++;
	    }
	}
	if(count>1){
	    fFitter.getVertex(estimate);
	} else {
	    estimate.setXYZ(-1000,-1000,-1000);
	    // in case we are running embedding we
	    // have to skip events with no vertex
	    if(isEmbedding) {return kFALSE;}
	}
    } else {return kFALSE;}
    return kTRUE;
}
Int_t HMdcVertexFind::execute(void)
{
    if(!fcallExecuteManual) return doFit();
    return 0;
}

Int_t HMdcVertexFind::doFit(void)
{
    HGeomVector *r,*alpha;
    HGeomVector oldVertex;
    HVertex     &event_vertex = gHades->getCurrentEvent()->getHeader()->getVertex();
    HGeomVector &vertex       = event_vertex.getPos();

    Int_t count = 0,i = 0,iteration = 0;
    Double_t weight,residual2,temp; //Weight and residual^2

    //-----------------------------------------------------------------
    // in embedding mode we have to take care
    // about the vertex : if useEventSeqNumber == kTRUE
    // we read the vertex coordinates of the primary GEANT
    // particles. To guaranty the sync between sim and real
    // we compare the event sequence numbers (written by PLUTO
    // to the userVal in HGeantKine). If a mismatch is detected
    // a kSkipEvent is returned (causing the second datasource
    // to rewind by one event).
    Float_t x,y,z;
    x = y = z = -1000;
    Int_t seqNumber   = -1;
    Int_t seqNumberEv = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
    x = y = z = -1000;
    if(isEmbedding && useEventSeqNumber)
    {
	geantKineIter->Reset();
	HGeantKine* kine = 0;

	while( (kine = (HGeantKine*) geantKineIter->Next()) )
	{

	    if(kine->getParentTrack() == 0){
		kine->getVertex(x,y,z);
		seqNumber = (Int_t) kine->getUserVal();
		break;
	    }
	}

	if( seqNumber >= 0)
	{
	    if(seqNumberEv < seqNumber )  {    // we do not match the events
		return kSkipEvent;
	    } else if( seqNumberEv > seqNumber ){
		Error("HMdcVertexFind::execute()","Running in useEventSeqNumber == kTRUE mode , but seqNumberEv (%i)> seqNumber (%i) from GEANT!",seqNumberEv,seqNumber );
	    }
	} else {
	    Error("HMdcVertexFind::execute()","Running in useEventSeqNumber == kTRUE mode , but could not get seqNumber from GEANT!");
	}
	vertex.setXYZ(x,y,z);
	event_vertex.setPos(vertex);
	event_vertex.setSumOfWeights(-1);
	event_vertex.setChi2(-1);
	event_vertex.setIterations(-1);

	return 0;
    }
    //-----------------------------------------------------------------




    switch(fInputMode){
    case kSegments:
	if(!readSegments(vertex)){
	    if(isEmbedding && doSkipNoVertex) {return kSkipEvent;}
	    else                              {return 1;}
	}
	break;
    case kHits:
	if(!readHits(vertex)){
	    if(isEmbedding && doSkipNoVertex) {return kSkipEvent;}
	    else                              {return 1;}
	}
	break;
    default:
	Error("execute","Unrecognized mode");
    }
    if(fUsingTukey)
    {

	count = fPos.GetEntriesFast();
	if(count > 1 && (vertex.Z()!=-1000. && vertex.X()!=-1000. && vertex.Y()!=-1000.))
	{
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

	    //Iteration on weights
	    Float_t sumOfResiduals = 0;
	    Float_t sumOfWeights   = 0;
	    for (i = 0; i < count; i ++) {
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
	    do
	    {
		sumOfResiduals = 0;
		sumOfWeights   = 0;
		oldVertex      = vertex;
		//fData->Fill(vertex(0),vertex(1),vertex(2),iteration,0.);
		iteration ++;
		fFitter.reset();
		for (i = 0; i <count; i ++)
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
		    weight = (temp < width) ? ((1. - (temp/width)*(temp/width)) *
					       (1. - (temp/width)*(temp/width))):0.0;
		    sumOfResiduals += weight*residual2;
		    sumOfWeights   += weight;
		    fFitter.addLine(*r,*alpha,weight);
		}
		width = locTukeyConst * sqrt(sumOfResiduals / sumOfWeights);

		fFitter.getVertex(vertex);
	    } // iteration loop
	    while(!hasConverged(vertex,oldVertex) && (iteration < fMaxIterations));


	    if(iteration < fMaxIterations){
		event_vertex.setIterations(iteration);
		event_vertex.setChi2(sumOfResiduals / sumOfWeights);
		event_vertex.setSumOfWeights(sumOfWeights);
	    } else {  // iteration >=  fMaxIterations
		event_vertex.setIterations(-2);
		event_vertex.setChi2(-1);
		event_vertex.setSumOfWeights(-1);
	    }
	} else {  // count <=1
	    event_vertex.setIterations(-1);
	    event_vertex.setChi2(-1);
	    event_vertex.setSumOfWeights(-1);
	}
	//fData->Fill(vertex(0),vertex(1),vertex(2),iteration,temp);
    } else {  // !fUsingTukey
	event_vertex.setIterations(1);
	event_vertex.setChi2(-1);
	event_vertex.setSumOfWeights(-1);
	//fData->Fill(vertex(0),vertex(1),vertex(2),0,1);
    }

    fPos.Clear();
    fAlpha.Clear();

    return 0;
}

Bool_t HMdcVertexFind::hasConverged(HGeomVector &v,HGeomVector &oldV) {
    Bool_t r =((v - oldV).length() < fEpsilon) ? kTRUE:kFALSE;
    return r;
}

ClassImp(HMdcVertexFind)
