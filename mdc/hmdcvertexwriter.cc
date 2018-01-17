//*-- AUTHOR : Jochen Markert

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcVertexWriter
//
// This class writes the reconstructed Event vertex to an root output file
// which will be later used for the preparation of the simulations files for
// the event embeddding. The class has to connect last to the task list to not
// write out skipped events.
//
// Output file name :
// 1. If the outputfile is set in HADES it will be equal outputfile_vertex.root.
//    The file will be splitted in the same way as the the root output file.
// 2. If the outputdir is set filenames are derived in the same way but the dir
//    corresponds to the set one.
// 3. If no outputdir is set and no output file is set in HADES the outputfile
//    will be equal to the current inputfile_vertex.root
// 4. if setOutputFile( filename_includingpath) is used all automatic derivation
//    from HADES input or outputfile name is disabled (manual mode, use this when
//    running from HRootSource)
//
//  static void HMdcVertexWriter::setSkipNoVertex (Bool_t skip = kTRUE){doSkipNoVertex = skip; }
//  With doSkipNoVertex == kTRUE (default : kTRUE) events without calculated vertex will be skipped.
//
//  with setVertexType(Int_t type)
//  kVertexCluster  = 1,(cluster finder vertex (mean middle of target segment position in z, x,y on 0,0))
//  kVertexSegment  = 2 (Mdc inner segments used to estimated the vertex,default),
//  kVertexParticle = 3 (fully reconstructed and sorted HParticleCand used to estimate the vertex),
//  kVertexClusterMeanXY = 4 (user provided mean x,y, z from vertexCluster)
//  kVertexGeant         = 5 (use primary vertex from HGeant)
//  the reconstructed vertex type can be selected.
//  with void setUserSelectionEvent(Bool_t (*function)(TObjArray* ),TObjArray* params=0) the user
//  can set a event selection function of the form
//
//  Bool_t myEventSelection(TObjArray* params){
//
//   if(goodEvent) return kTRUE;
//   else          return kFALSE;
// }
//
//  where params cand be used to pass parameters for the used
//  event selection. The event selection should be used in combination
//  with setSkipNoVertex(kTRUE)
//////////////////////////////////////////////////////////////////////////////

#include "hmdcvertexwriter.h"
#include "hades.h"
#include "hevent.h"
#include "heventheader.h"
#include "hcategorymanager.h"
#include "hdatasource.h"
#include "hcategory.h"
#include "hgeantkine.h"
#include "hgeantdef.h"


#include "TFile.h"
#include "TSystem.h"

#include <iostream> 
#include <iomanip>
#include <stdlib.h>



ClassImp(HMdcVertexWriter)

Bool_t  HMdcVertexWriter::doSkipNoVertex = kTRUE;
Int_t   HMdcVertexWriter::vertextype     = 2; // segments
Double_t HMdcVertexWriter::fMeanX        = -1000.;
Double_t HMdcVertexWriter::fMeanY        = -1000.;

HMdcVertexWriter::HMdcVertexWriter(void)
{
    // Default constructor .
    outfile       = NULL;
    vertex        = NULL;
    filename      = "";
    outputdir     = "";
    fileNameFromInput = kTRUE;
    fileNameManual    = kFALSE;
    isEmbedding       = kFALSE;
    pUserSelectEvent  = NULL;
    pUserParams       = NULL;
}
HMdcVertexWriter::HMdcVertexWriter(const Text_t* name,const Text_t* title)
:  HReconstructor(name,title)
{
    // Constructor calls the constructor of class HReconstructor with the name
    // and the title as arguments.
    outfile       = NULL;
    vertex        = NULL;
    filename      = "";
    outputdir     = "";
    fileNameFromInput = kTRUE;
    fileNameManual    = kFALSE;
    isEmbedding       = kFALSE;
    pUserSelectEvent  = NULL;
    pUserParams       = NULL;
}

HMdcVertexWriter::~HMdcVertexWriter(void) {
  // destructor close output if still open

    if (outfile){
        vertex ->Write();
	outfile->Close();
	outfile = NULL;
	vertex  = NULL;
    }
}

Bool_t HMdcVertexWriter::init(void)
{
    // init the ascii io ofstream with filename
    // taken from the current file of the data source
    // The file name will be checked inside the eventloop
    // against the HADES outputfile.


    //---------------------------------------------
    // now we have to finde the outputfile name
    // in the init function we have no access to
    // the outputfile name of hades (it will be created
    // after init). We take the file name from the datasource
    // end check in the eventloop if the file name has changed

    HDataSource* datasource = gHades->getDataSource();
    Bool_t  isROOT= kFALSE;
    TString type = gHades->getDataSource()->ClassName();
    if(type.CompareTo("HRootSource") == 0) isROOT=kTRUE;

    if(isROOT && !fileNameManual) {

	Error("HMdcVertexWriter::init()","Use setOutputFile(TString file = "") to define outputfile manual when running from HRootSource !");
	exit(1);

    }


    TString fname    = "";
    TString fullname = "";
    TString dir      = "";

    if(!isROOT && !fileNameManual){
	if(datasource){

	    filename = datasource->getCurrentFileName();

	    if(filename.Last('.') >= 0 )
	    {
		filename.Replace(filename.Last('.'),filename.Length() - filename.Last('.'),"_vertex.root");
		fname = gSystem->BaseName(filename.Data());
	    } else {
		Error("HMdcVertexWriter::init()","Could not retrieve current input file name = %s ! Something seems to be wrong!",filename.Data());
		exit(1);
	    }
	} else {
	    Error("HMdcVertexWriter::init()","Retrieve NULL pointer for datasource!");
	    exit(1);
	}

	if(outputdir != ""){ // replace path by outputdir
	    dir = outputdir;
	    fullname = dir + "/" + fname;
	} else {
	    dir = filename;

	    if(dir.Last('/') >= 0){
		// if filename contains a path.
		dir.Replace(dir.Last('/') + 1,dir.Length() - filename.Last('/') + 1,"");
	    } else {
		dir = gSystem->WorkingDirectory();
	    }

	    fullname = filename;
	}
	filename = fullname;
	filename.ReplaceAll("//","/");
    } else {
        filename=Form("%s/%s",outputdir.Data(),filename.Data());
        dir = outputdir;
    }


    Bool_t check = gSystem->AccessPathName(dir.Data(), kWritePermission);
    if(check == kFALSE)
    {   // inverse logic !!!
	outfile = new TFile(filename.Data(),"RECREATE");
	if(!outfile){
	    Error("HMdcVertexWriter::init()","Retrieve NULL for root output file = %s!",filename.Data());
	    exit(1);
	}
        outfile->cd();
	vertex = new TNtuple("vertex","vertex","vX:vY:vZ:seqNumber");

    } else {
	Warning("HMdcVertexWriter::init()","No Permission to write outputfile file = %s! \n Will try with outputfile name!",filename.Data());
    }
    //---------------------------------------------

    // find out if we are running embedding
    HCategory* catKine = (HCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);
    if(catKine && gHades ->getEmbeddingMode() > 0) {
	isEmbedding   = kTRUE;
    }

    return kTRUE;
}
Int_t HMdcVertexWriter::execute(void)
{
    // check if the root output file should be changed (taking
    // the file name from the HADES out put file). The coordinates
    // of the Event Vertex will be written to the root file output.


    //---------------------------------------------
    // Now we have to check if the outputfile name
    // has changed (due to file splitting)
    // Only needed if output is enabled otherwise
    // we have taken the input file name from the data source
    if(gHades->getOutputFile() && !fileNameManual)
    {
	if(fileNameFromInput && outfile){
	    // we have to close and remove the old file
	    // if we now want to use the output file name
	    // convention
	    outfile->cd();
	    vertex ->Write();
	    delete vertex;
	    outfile->Close();
	    gSystem->Exec(Form("rm %s",filename.Data()));
	    vertex  = NULL;
            outfile = NULL;
	}
	TString dir      = "";
        TString fname    = "";
        TString fullname = "";

	TString currentfile = gHades->getOutputFile()->GetName();

	if(currentfile.Last('.') >= 0 )
	{
	    currentfile.Replace(currentfile.Last('.'),currentfile.Length() - currentfile.Last('.'),"_vertex.root");
	}

	if(outputdir != ""){
	    fname = gSystem->BaseName(filename.Data());
	    fullname = outputdir + "/" + fname;
            fullname.ReplaceAll("//","/");
	}


	if(fullname.CompareTo(filename.Data()) != 0)
	{
	    // filename changed

	    filename = fullname;
	    if(outfile){
		// close old file
		outfile->cd();
		vertex  ->Write();
		delete vertex;
		outfile ->Close();
		vertex  = NULL;
		outfile = NULL;
	    }

	    outfile = new TFile(filename.Data(),"RECREATE");
	    if(!outfile){
		Error("HMdcVertexWriter::init()","Retrieve NULL for root output file = %s!",filename.Data());
		exit(1);
	    }
	    outfile->cd();
	    vertex = new TNtuple("vertex","vertex","vX:vY:vZ:seqNumber");

	}
	fileNameFromInput = kFALSE;
    } else {

	if(!outfile){
	    Error("HMdcVertexWriter::execute()","Could not create filename from  output and input file !");
	    exit(1);
	}


    }
    //---------------------------------------------
    if(outfile){
	HEvent* event = gHades->getCurrentEvent();
	if(event){
	    HEventHeader* header = event ->getHeader();
	    if(header){
                HVertex event_vertex ;
                if(vertextype == 1)  event_vertex = header->getVertexCluster();
                if(vertextype == 2)  event_vertex = header->getVertex();
                if(vertextype == 3)  event_vertex = header->getVertexReco();
                if(vertextype == 4)  event_vertex = header->getVertexCluster();

		if(vertextype == 5)
		{
		    //--------------------------------------------------------------
		    // retrive the vertex form first primary kine particle
		    HGeantKine* kine;
		    Float_t vx,vy,vz;

		    HCategory* kineCat = HCategoryManager::getCategory(catGeantKine);
		    if(!kineCat) {
                        Error("execute()","No catGeantKine in current Event! Cannot extract vertex from HGeantKine.");
			return 0;
		    }

		    Int_t n = kineCat->getEntries();
		    for(Int_t j = 0; j < n; j ++){
			kine = HCategoryManager::getObject(kine,kineCat,j);
			if(kine) {
			    if(kine->isPrimary())
			    {
				kine->getVertex(vx,vy,vz);
				if(doSkipNoVertex)
				{
				    Bool_t acceptedEvent = kTRUE;
				    if(pUserSelectEvent){
					acceptedEvent=(*pUserSelectEvent)(pUserParams);
				    }

				    if(acceptedEvent)vertex->Fill(vx,vy,vz,header->getEventSeqNumber());
				} else {
				    vertex->Fill(vx,vy,vz,header->getEventSeqNumber());
				}
                                break;
			    }
			}
		    }
		    //------------------------------------------------------------

		} else {
		    Double_t x,y,z;
		    x = event_vertex.getX();
		    y = event_vertex.getY();
		    z = event_vertex.getZ();

		    if(vertextype == 4){
			x = fMeanX;
			y = fMeanY;
		    }


		    if(doSkipNoVertex)
		    {
			Bool_t acceptedEvent = kTRUE;
			if(pUserSelectEvent){
			    acceptedEvent=(*pUserSelectEvent)(pUserParams);
			}
			// write only if a vertex has been
			// calculated
			if(event_vertex.getX() != -1000 &&
			   event_vertex.getY() != -1000 &&
			   event_vertex.getZ() != -1000 &&
			   acceptedEvent
			  )
			{
			    vertex->Fill(x,y,z,
					 header->getEventSeqNumber()
					);
			}

		    } else {
			// fill allways
			vertex->Fill(x,y,z,
				     event_vertex.getZ(),
				     header->getEventSeqNumber()
				    );
		    }
		}
	    }
	}
    }

    return 0;
}
Bool_t HMdcVertexWriter::finalize(void)
{
    // close the root file ouput

    if(outfile) {
	outfile->cd();
        vertex ->Write();
        delete vertex;
	outfile->Close();
	outfile = NULL;
        vertex  = NULL;
    }
    return kTRUE;
}

