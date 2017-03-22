#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"

#include "hades.h"
#include "htool.h"
#include "hphysicsconstants.h"
#include "hrootsource.h"
#include "hiterator.h"
#include "hloop.h"
#include "hhistmap.h"
#include "hdst.h"

#include "haddef.h"
#include "heventheader.h"
//#include "hgeantdef.h"
#include "/u/harabasz/myhydralibs-newmixer-generic/eventmixer/heventmixer.h"
#include "hparticleanglecor.h"
#include "hparticledef.h"
#include "hparticlestructs.h"
#include "hparticlecand.h"
#include "hparticlecandsim.h"
#include "hcategorymanager.h"
#include "hparticletracksorter.h"
#include "hparticlevertexfind.h"
#include "hparticleevtinfo.h"
#include "htaskset.h"
//#include "hgeantkine.h"
#include "TMVA/Reader.h"


#include "hstart2hit.h"
#include "hstart2cal.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "horadef.h"
#include "horasimdef.h"
#include "hstartdef.h"
#include "richdef.h"
#include "rpcdef.h"
#include "showerdef.h"
#include "simulationdef.h"
#include "tofdef.h"
#include "walldef.h"


#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
// version   1.0   Szymon Harabasz 30.1.2017


#include "TStopwatch.h"
#include "TCutG.h"
#include "TF1.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TNtuple.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <stdio.h>

#include "eventclassifier.h"

using namespace std;

//------------------------------------------------------------

Int_t dieleAna(TString inputlist, TString outfile, Int_t nev=-1)
{

    // ...
    sorter.init();                                                  // get catgegory pointers etc...
    //    --------------------------------------------------------------------------------------------

    HParticleEventMixer eventmixer;
    eventmixer.setPIDs(2,3,1);
    eventmixer.setBuffSize(80);
    eventmixer.setEventClassifier(eventClassifierMultRPhiTarget);

    Int_t evtsInFile = loop->getEntries();
    if(nev < 0 || nev > evtsInFile ) nev = evtsInFile;

    for(Int_t i = 1; i < nev; i++)
    {
        if(loop->nextEvent(i) <= 0) { cout<<" end recieved "<<endl; break; } // last event reached
        HTool::printProgress(i,nev,1,"Analyze pairs :");

        // ...

	//------------------------------------------------------------
        // Loop to fill vectors
        vector<HParticleCand *> vep;
        vector<HParticleCand *> vem;
	for(Int_t j = 0; j < size; j ++){
	    cand1 = HCategoryManager::getObject(cand1,candCat,j);

            if (leptonFlag[j] && !removedIncomplete[j] && !removedNearest[j] && !removedRecursive[j]) {
                if (cand1->getCharge() == -1) {
                    vem.push_back(new HParticleCand(*cand1));
                }
                if (cand1->getCharge() == 1) {
                    vep.push_back(new HParticleCand(*cand1));
                }
            }
        }
	//------------------------------------------------------------


	//------------------------------------------------------------
        // Fill the mixer
	eventmixer.nextEvent();
        eventmixer.addVector(vep,2);
        eventmixer.addVector(vem,3);
	//------------------------------------------------------------


	//------------------------------------------------------------
	// Analysis of mixed pairs
        //
	vector<HParticlePair>& pairsVec = eventmixer.getMixedVector();

        Int_t eventClass = eventmixer.currentEventClass();
        Double_t eventClassMult = 0;
        eventClassMult += pEventClass->GetBinContent(pEventClass->FindBin(eventClass));
        Double_t eventClassWeight = 1./eventClassMult;

        size = pairsVec.size();
        for (Int_t j = 0; j < size; j ++) {
            HParticlePair& pair = pairsVec[j];

            cand1 = pair.getCand(0);
            if(!cand1->isFlagBit(Particle::kIsLepton)) continue;
            cand2 = pair.getCand(1);
            if(!cand2->isFlagBit(Particle::kIsLepton)) continue;
            //                if (cand1->getSector() != cand2->getSector()) continue;
            if (cand1->getCharge() == -1 && cand2->getCharge() == 1) {
                HParticleCand *temp = cand1;
                cand1 = cand2;
                cand2 = temp;
            }


            TLorentzVector dilep = (*cand1) + (*cand2);
            Float_t oAngle   = cand1->Angle(cand2->Vect())*TMath::RadToDeg();
            // ...
        }
	//------------------------------------------------------------
        
	//------------------------------------------------------------
        // Clean up
	// User code is responsible to delete what it created with "new",
        // event mixing class only gives a hint what can be safely deleted
        vector <HParticleCand *>* toDel = eventmixer.getObjectsToDelete();
        for (UInt_t ii = 0; ii < toDel->size(); ++ii) {
            delete toDel->at(ii);
        }
        toDel->clear();
        delete toDel;
	//------------------------------------------------------------




    } // end event loop

    sorter.finalize();
    // ...

    return 0;
}
