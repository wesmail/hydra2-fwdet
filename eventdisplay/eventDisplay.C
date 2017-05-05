//#ifndef __CINT__



#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"
#include "TGWindow.h"

#include "TEveManager.h"

#include "hedhelpers.h"
#include "hedfield.h"

#include "loadHadesGeom.C"

//#define IS_REAL_DATA   // comment this line for simulation


#ifdef  IS_REAL_DATA
  #include "createHadesReal.C"
#else
  //#include "createHades.C"
  #include "createHades_Future.C"  // proposal2018
#endif

#include "nextEvent.C"
#include "make_GUI.C"



#include <list>
#include <iostream>
#include "hades.h"


using namespace std;

//---------------------------------------------------------
//             HADES EVENT DISPLAY
// To start in ROOT session: .x eventDispaly.C+
//
// Macros belonging to the display:
// -- loadHadesGeom.C
// -- createHades.C
// -- nextEvent.C
// -- make_GUI.C
// For documentation see README.txt
//---------------------------------------------------------


void eventDisplay()
{

    TEveManager::Create(kTRUE);
    TGWindow* win = gEve->GetMainWindow();
    win->Resize(1200,900);

#define doHades


#ifdef doHades
   //----------------------------------------------------------
   // CREATE HADES
   if(!createHades()){
       cerr<<"EVENT DISPLAY : COULD NOT CREATE HADES ! ################################"<<endl;
       exit(1);
   }

   //----------------------------------------------------------
#endif
   Double_t fpol   = 1.;  // for eventdisplay polarity is reverse (convention comes from ALICE)
   //Double_t fscale = 0.72; //  apr12sim_mediumfieldalign_auau;
   //HEDField* field = new HEDField("HADES_FIELD_MAP","HADES_FIELD_0.72",fscale*fpol);
   Double_t fscale = 0.9235; //  apr12sim_mediumfieldalign_auau;
   HEDField* field = new HEDField("HADES_FIELD_MAP","HADES_FIELD_0.9235",fscale*fpol);
   field->ReadAscii("/misc/hadessoftware/etch32/input_eventdisplay/fieldpar.txt");
   field->SetPolarity(fscale*fpol);

   //----------------------------------------------------------
   // GEOMETRY MANAGER
   //if(!loadHadesGeom("/misc/hadessoftware/etch32/input_eventdisplay/GeomManager_Apr12_runID_12001.root")){
   if(!loadHadesGeom("/misc/hadessoftware/etch32/input_eventdisplay/GeomManager_prop2017_agag1650_runID_15000_rich700_ecal.root")){
       cerr<<"EVENT DISPLAY : Could NOT CREATE HADES GEOMMANAGER! #####################"<<endl;
       exit(1);
   }
   //----------------------------------------------------------

   gROOT->LoadMacro("./nextEvent.C+");
   make_GUI();

#ifdef  IS_REAL_DATA
   HEDEvtNavHandler::isRealData = kTRUE;
#endif

   gEve->Redraw3D();


}
