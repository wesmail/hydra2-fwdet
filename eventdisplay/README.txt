//---------------------------------------------------------
//             HADES EVENT DISPLAY
//
// Copy all macros *.C to your local dir. You may need
// to edit them according to your needs.
//
// To start in ROOT session: .x eventDisplay.C+
//
// eventDisplay.C  : USER ACTION :
//                   -setup the TGeomManager root file
//                   -switch real sim data here.
//                   -set B-Field settings
// loadHadesGeom.C : NO USER ACTION required here.
//                   Loads the need volums for the display from
//                   root file reading TGeoManager.
// make_GUI.C      : NO USER ACTION required here.
//
// createHades.C   : USER ACTION required here.
//                   creates HADES including detectors,
//                   parameter and data sources. Setup and
//                   taskslists can be changed here.
//                   Basically a DST macro. Put 
//                   HEDTransform::setIsNewRich(kTRUE); in the top
//                   of the macro, if you want to run new Rich.
//                   Put HEDTransform::setIsEmc(kTRUE); in the top
//                   of the macro, if you want to run ECAL instead of SHOWER.
//                   HEDTranform functions , HED objects 
//                   nextEvent.C and loadHadesGeom.C will use the 
//                   flag to perform the proper action
// createHadesReal.C: USER ACTION required here. same as
//                    above, but for real data reading from hld file
//
// nextEvent.C     : USER ACTION required here.
//                   loads one HADES event into memory. Copy
//                   of hits and tracks to Eve objects is
//                   done here. The user can select / group
//                   and change property of the displayed
//                   objects. The parts where the user should
//                   edit the macro are marked
//                   \"######## USER ACTION #####\"
// userFunc.C      : bundle of functions like select functions
//                   for HParticleSorter and the rejectEvent()
//                   function which can be used to display
//                   only selected events (example for e+ e-
//                   is provided but disabled)
//---------------------------------------------------------

//---------------------------------------------------------
//             LOAD HADES GEOM (loadHadesGeom.C)
// Reads HADES geometry from root file containing TGeoManager.
// (createGeomManager.C can be used to create the need file)
// All volumes are set invisible by default and only some
// selected volumes are set visible again with the desired
// color and transparency. If the TEveManager does not
// exist it will be created. The Geometry will be added
// to the global scene (persistent). The pointer to the used
// TGeoVolumes and TGeoNodes are stored in HEDColorDef. This
// object is used by the GUI for changing the properties later.
// Compiled on load time. Coordinate transformations for the
// RICH pad plane and mirror are stored too.
//---------------------------------------------------------

//---------------------------------------------------------
//             MAKE GUI (make_GUI.C)
// Create the GUI for Display setup in Eve. Connect
// "next Event" button to HEDEvtNavHandler defined in
// nextEvent.C
// Compiled on load time.
//---------------------------------------------------------

//---------------------------------------------------------
//             NEXT EVENT (nextEvent.C)
// This macro will load a new event into memory.
// It performs a call to Hades event loop. After
// running the event loop the full event is available
// in memory. The different detector hits can be selected
// by the user, tranformed and added to the event scene of
// Eve. All objects of the previous event scene will be
// destroyed.
//
//
// Class HEDEvtNavHandler  : event handler connected to GUI
//
// this Class provides the selectEvent() function connected
// to the "next Event" button. The function then calls
// nextEventLoop() or nextEvent() depending if the the loop
// box is checked.
// HEDEvtNavHandler holds the user defined TEveElementLists
// which are inserted in the Event Scene of TEveManager. The
// user has to clean and fill this lists inside nextEvent().
// The lists appear in "Eve" tab of the GUI in the browser
// "Scenes/Event scene". The parts where the user should
// edit the macro are markerd \"######## USER ACTION #####\"
// Compiled on load time.
//
//---------------------------------------------------------

//---------------------------------------------------------
//             AVAILABLE GRAPHIC OBJECTS
//         (libEventDisplay.so, hedhitobjects.h,hedhelpers.h)
//
// Eventdisplay uses LAB coordinates withx,yz units in mm.
// Hence all hits objects from the analysis have to be
// transformed to LAB and cm. Functions used for coodinate
// transformations are located in HEDTransform. HEDMdcWireManager
// will do the count statistics for the MDC wires.
// HEDGroup and HEDGroup2D provide some help to group
// TEveElementLists in 1 or 2 dim arrays. This is useful
// to group graphic object like sector or sector/module.
// expample:
// HEDGroup* sectors  = new HEDGroup("sectors","sectors",6,"Sector");
// will create 1 main list containing 6 lists one for each sector.
// sectors->AddElement(Int_t sector,TEveElement* el)
// will add an object to the list of the sector. The elements
// can be TEveElementLists allowing to create complex structures.
//----------------------------------------------------------------
//
// All following objects work for REAL/SIM data
//
// HEDVertex          : public TEvePointSet (no input needed)
// HEDSegment         : public TEveLine     ==> HEDSegment(HMdcSegSim*)
// HEDMdcWire         : public TEveLine     ==> HEDMdcWire(HMdcCal1Sim*)
// HEDRichHit         : public TEveLine     ==> HEDRichHit(HRichHitSim*)
// HEDRichHitPadPlane : public TEvePointSet ==> HEDRichHitPadPlane(HRichHitSim*) // RICH hit at pad plane
// HEDRichRing        : public TEvePointSet ==> HEDRichRing(HRichHitSim*)        // RICH ring at pad plane
// HEDRichPadPlane    : public TEveQuadSet  ==> HEDRichPadPlane(Int_t sector)    // RICH pad plane + fired pads
// HEDRich700PadPlane() : public TEveQuadSet                                     // RICH700 pad plane + fired pads
// HEDRichCompound    : public TEveCompound ==> HEDRichCompound(HRichHitSim*)    // RICH hit at pad plane + ring + mirror hit
// HEDTofHit          : public TEvePointSet ==> HEDTofHit(HTofHitSim*)
// HEDTofCluster      : public TEvePointSet ==> HEDTofCluster(HTofClusterSim*)
// HEDRpcCluster      : public TEvePointSet ==> HEDRpcCluster(HRpcClusterSim*)
// HEDWallHit         : public TEvePointSet ==> HEDWallHit(HWallHitSim*)
// HEDWallPlane       : public TEveQuadSet  ==> HEDWallPlane()                   // fired WALL cells
// HEDShowerHit       : public TEvePointSet ==> HEDShowerHit(HShowerHitSim*)
// HEDEmcCluster      : public TEvePointSet ==> HEDEmcCluster(HEmcClusterSim*)
//----------------------------------------------------------------
// HEDParticleCand : public TEveCompound ==> HEDParticleCand(HParticleCandSim*);
//    consist out of all detector hits contributing to
//    the candidate. The object provives functions
//    to change the graphical representation/
//
//    void SetLineColor  (Color_t val)
//    void SetLineStyle  (Style_t val)
//    void SetLineWidth  (Style_t val)
//    void SetMarkerColor(Color_t val)
//    void SetMarkerStyle(Style_t val)
//    void SetMarkerSize (Size_t val)
//    void SetRnrLine    (Bool_t val)  // kTRUE: line  will be shown
//    void SetRnrPoints  (Bool_t val)  // kTRUE: points will be shown
//----------------------------------------------------------------
//
//----------------------------------------------------------------
//           HGEANT OBJECTS
// TEveTrack* track = HEDTransform::createKineParticle(kine,simTrackList->GetPropagator());
//                  : HEDField keeps the HADES filed map. The Runge Kutta propagator
//                  : of Eve is used to propagate the track trough the detector.
// HEDRichGeantPadPlane : to draw GEANT hits on the RICH PADplane
// HEDRichGeantMirror   : to draw GEANT Mirror hits
//
//
//
//---------------------------------------------------------
