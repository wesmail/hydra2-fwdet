#include "HBeam.h"
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// HBeam
//
//    This class simulates a beam line. Beam particles (HBeamParticle) will be produced using
//    a certain beam profile and momentum smearing. The particle will be tranported through
//    the beam line using matrices for the single beam elements (HBeamElement). The particles
//    cand be registered in detectors along the beam line.
//
//    HBeam pionbeam;
//    pionbeam.setBeam           (HPhysicsConstants::pid("pi-"),1.3,60,60,0.0,0.0); // id, total mom [GeV], beamtube x and y size, xoff,yoff
//    pionbeam.SetBeamResolution (0.01,0.05,0.06);                                  // dpx [rad],dpy [rad],dpz [frac]  [+-] white random
//    pionbeam.setBeamProfile    (.05,0.0);                                         // sigma [mm], flatradius [mm]
//                                                                                  // sigma>0 will select the beam profile (gausian + flat top)
//                                                                                  // sigma==0, flatradius>0 will give an extended beam spot without gaussian borders
//                                                                                  // sigma==0,flatradius==0 spot like beam
//    if(!pionbeam.initBeamLine  ("pibeam_set6_mod.data",32)) return;               // transform input file and target element number
//    pionbeam.addDetector("det1", -17092.6,2,50.,50.);                             // [mm] relative to HADES 0,0,0    cutype (0,1,2), xcut[mm],ycut[mm]
//    pionbeam.addDetector("det2",  -5400.0,2,50.,50.);                             // [mm] relative to HADES 0,0,0    cutype (0,1,2), xcut[mm],ycut[mm]
//    pionbeam.addDetector("plane", -1300.0,1,60.,60.);                             // [mm] relative to HADES 0,0,0    cutype (0,1,2), xcut[mm],ycut[mm]
//
//    pionbeam.printBeamLine(kTRUE);          // kTRUE : print transform matrices in addition to name and distance
//    pionbeam.printDetectors();
//    pionbeam.printBeamProfile();
//
//
//    //-----------------------------------------------------------
//    // create particles
//    vector<HBeamParticle>& vhistory = pionbeam.newParticle();  // beam particle at : beam,det1,det2,plane.target
//    // check if particle was in acceptance (using the particle at the end of transport)
//    if(vhistory[vhistory.size()-1].fAccepted) {   ......   }
//
//    // get access to all beam elements and detectors (positions,accptance,statistics ...)
//    vector<HBeamElement>& elements  = pionbeam.getElements();
//    vector<HBeamElement>& detectors = pionbeam.getDetectors();
//
///////////////////////////////////////////////////////////////////////////////
