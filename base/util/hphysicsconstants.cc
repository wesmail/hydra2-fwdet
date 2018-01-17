// @(#)$Id: hphysicsconstants.cc,v 1.9 2009-07-03 12:10:14 halo Exp $
//*-- Author : Dan Magestro
//*-- Created: 03/09/01
//*-- Modified     : 24/02/03 by Marcin Jaskula
//                   new ids for fakes, get rid of TObject
//*-- Last modified: 24/02/03

//*-- Modified     : 03/11/06 by Alexander Belyaev
//                   leptonCharge, baryonCharge and strangeness are added

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//
//  HPhysicsConstants
//
//  This class contains look-up information for particle properties.  The
//  design is based on a scheme written by Marios Kargalis in his PData.h
//  class for Pluto++.  See PStdData.h class documentation for references.
//
//  Note: Particle Id's are reserved for 'pion' (44) and 'kaon' (48), which
//        can be either + or -. This is useful for graphical cuts on all
//        particles regardless of sign; see HCutHadronId.
//
//  Particle Id's between 500 and 500 + nPart are mapped back to Id - 500.
//
//
////////////////////////////////////////////////////////////////////////////////

#include "hphysicsconstants.h"
#include "TError.h"

#include <string.h>
#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(HPhysicsConstants)
// -----------------------------------------------------------------------------

#define NPART  70              // number of particles stored permanently
#define ARTIFICIAL_OFFSET 500


Bool_t HPhysicsConstants::isFake(Int_t iId)
{
// checks wheater the particle defined by its id is a fake

    return ( (iId >= ARTIFICIAL_OFFSET + NPART) || (iId == fakePos()) || (iId == fakeNeg()));
}

// -----------------------------------------------------------------------------

Bool_t HPhysicsConstants::isArtificial(Int_t iId)
{
// checks wheater the particle defined by its id is an artificial one

    return ((iId == artificialPos()) || (iId == artificialNeg()));
}

// -----------------------------------------------------------------------------

Bool_t HPhysicsConstants::addParticle(Int_t id, TString name, Double_t mass,Int_t chrg,Int_t leptchrg,Int_t barychrg,Int_t strange)
{
    if(idToProp.find(id) == idToProp.end() ){           // id was not used before
	if(nameToProp.find(name) == nameToProp.end() ){ // name was not used before
            particleproperties prop;
	    prop.fill(id,name,mass,chrg,leptchrg,barychrg,strange);
	    idToProp  [id]     = prop;
	    nameToProp[name]   = prop;
	    return kTRUE;
	} else {
	    ::Error("HPhysicsConstants::addParticle()","name was used already before!");
	    return kFALSE;
	}
    } else {
	::Error("HPhysicsConstants::addParticle()","id was used already before!");
	return kFALSE;
    }
}

Bool_t HPhysicsConstants::addParticle(particleproperties prop)
{
    if(idToProp.find(prop.fId) == idToProp.end() ){           // id was not used before
	if(nameToProp.find(prop.fName) == nameToProp.end() ){ // name was not used before
	    idToProp  [prop.fId]     = prop;
	    nameToProp[prop.fName]   = prop;
	    return kTRUE;
	} else {
	    ::Error("HPhysicsConstants::addParticle()","name was used already before!");
	    return kFALSE;
	}
    } else {
	::Error("HPhysicsConstants::addParticle()","id was used already before!");
	return kFALSE;
    }
}

// -----------------------------------------------------------------------------

particleproperties HPhysicsConstants::createParticle(Int_t id,TString name, Double_t mass,Int_t chrg,Int_t leptchrg,Int_t barychrg,Int_t strange)
{
    particleproperties prop;
    prop.fill(id,name,mass,chrg,leptchrg,barychrg,strange);
    return prop;
}

// -----------------------------------------------------------------------------

Bool_t HPhysicsConstants::removeParticle(Int_t id)
{
    // remove particle with ID id from data base
    map<Int_t,particleproperties>::iterator iter = idToProp.find(id);

    if(iter != idToProp.end() ) {
	nameToProp.erase(iter->second.fName);
	idToProp  .erase(id);
        return kTRUE;
    }
    return kFALSE;

}

// -----------------------------------------------------------------------------

Bool_t HPhysicsConstants::moveParticle(Int_t id,Int_t newId)
{
    // move particle with id to newId. Only performed if
    // newId is not used already
    map<Int_t,particleproperties>::iterator iter  = idToProp.find(id);
    map<Int_t,particleproperties>::iterator iter2 = idToProp.find(newId);

    if(iter != idToProp.end() ) {
	if(iter2 == idToProp.end() ) {

	    particleproperties prop = iter->second;
	    prop.fId = newId;
            removeParticle(id);
            addParticle(prop);

	    return kTRUE;
	} else {
           ::Error("HPhysicsConstants::moveParticle()","new id was used already before, cannot move id!");
	}
    }
    return kFALSE;

}

Bool_t HPhysicsConstants::cpParticle(Int_t id,Int_t newId,TString newName)
{
    // move particle with id to newId. Only performed if
    // newId is not used already
    map<Int_t,particleproperties>::iterator iter  = idToProp.find(id);
    map<Int_t,particleproperties>::iterator iter2 = idToProp.find(newId);

    if(iter != idToProp.end() ) {
	if(iter2 == idToProp.end() ) {

	    particleproperties prop = iter->second;
	    prop.fId = newId;
            prop.fName = newName;
            addParticle(prop);

	    return kTRUE;
	} else {
           ::Error("HPhysicsConstants::moveParticle()","new id was used already before, cannot move id!");
	}
    }
    return kFALSE;

}

// -----------------------------------------------------------------------------

void HPhysicsConstants::clearParticles()
{
    // clear all particles in data base
    idToProp  .clear();
    nameToProp.clear();
}

// -----------------------------------------------------------------------------

map<Int_t,particleproperties> HPhysicsConstants::initParticleID()
{
    // to be called only for init of static vectors


    map<Int_t,particleproperties> idToPropL;

    //                           id, name,         mass       chrg lep bar strange
    idToPropL[-4]=createParticle(-4,"artificial+", -1.,        1,  1,  1, 1);  /* -4 artificial+*/
    idToPropL[-3]=createParticle(-3,"artificial-", -1.,       -1, -1, -1,-1);  /* -3 artificial-*/
    idToPropL[-2]=createParticle(-2,"fake+",       -1.,        1,  1,  1, 1);  /* -2 fake+      */
    idToPropL[-1]=createParticle(-1,"fake-",       -1.,       -1, -1, -1,-1);  /* -1 fake-      */
    idToPropL[ 0]=createParticle( 0,"dummy",       0.0,        0,  0,  0, 0);  /* 0: dummy      */
    idToPropL[ 1]=createParticle( 1,"g",           0.0,        0,  0,  0, 0);  /* 1: Photon     */
    idToPropL[ 2]=createParticle( 2,"e+",          0.51099906, 1, -1,  0, 0);  /* 2: Positron   */
    idToPropL[ 3]=createParticle( 3,"e-",          0.51099906,-1, +1,  0, 0);  /* 3: Electron   */
    idToPropL[ 4]=createParticle( 4,"nu",          0.0,        0, +1,  0, 0);  /* 4: Neutrino   */
    idToPropL[ 5]=createParticle( 5,"mu+",         105.658389, 1, -1,  0, 0);  /* 5: mu+        */
    idToPropL[ 6]=createParticle( 6,"mu-",         105.658389,-1, +1,  0, 0);  /* 6: mu-        */
    idToPropL[ 7]=createParticle( 7,"pi0",         134.9764,   0,  0,  0, 0);  /* 7: pi0        */
    idToPropL[ 8]=createParticle( 8,"pi+",         139.56995,  1,  0,  0, 0);  /* 8: pi+        */
    idToPropL[ 9]=createParticle( 9,"pi-",         139.56995, -1,  0,  0, 0);  /* 9: pi-        */
    idToPropL[10]=createParticle(10,"K0L",         497.672,    0,  0,  0,+1);  /*10: K0_long    */
    idToPropL[11]=createParticle(11,"K+",          493.677,    1,  0,  0,+1);  /*11: K+         */
    idToPropL[12]=createParticle(12,"K-",          493.677,   -1,  0,  0,-1);  /*12: K-         */
    idToPropL[13]=createParticle(13,"n",           939.56563,  0,  0, +1, 0);  /*13: Neutron    */
    idToPropL[14]=createParticle(14,"p",           938.27231,  1,  0, +1, 0);  /*14: Proton     */
    idToPropL[15]=createParticle(15,"anti_p",      938.27231, -1,  0, -1, 0);  /*15: Antiproton */
    idToPropL[16]=createParticle(16,"K0S",         497.672,    0,  0,  0,+1);  /*16: K0_short   */
    idToPropL[17]=createParticle(17,"eta",         547.45,     0,  0,  0, 0);  /*17: Eta        */
    idToPropL[18]=createParticle(18,"Lambda",      1115.684,   0,  0, +1,-1);  /*18: Lambda     */
    idToPropL[19]=createParticle(19,"Sigma+",      1189.37,    1,  0, +1,-1);  /*19: Sigma+     */
    idToPropL[20]=createParticle(20,"Sigma0",      1192.55,    0,  0, +1,-1);  /*20: Sigma0     */
    idToPropL[21]=createParticle(21,"Sigma-",      1197.436,  -1,  0, +1,-1);  /*21: Sigma-     */
    idToPropL[22]=createParticle(22,"Xi0",         1314.9,     0,  0, +1,-2);  /*22: Xi0        */
    idToPropL[23]=createParticle(23,"Xi-",         1321.32,   -1,  0, +1,-2);  /*23: Xi-        */
    idToPropL[24]=createParticle(24,"Omega",       1672.45,   -1,  0, +1,-3);  /*24: Omega-     */
    idToPropL[25]=createParticle(25,"anti_n",      939.56563,  0,  0, -1, 0);  /*25: Antineutrn */
    idToPropL[26]=createParticle(26,"anti_Lambda", 1115.684,   0,  0, -1,+1);  /*26: Antilambda */
    idToPropL[27]=createParticle(27,"anti_Sigma-", 1189.37,   -1,  0, -1,+1);  /*27: Antisigma- */
    idToPropL[28]=createParticle(28,"anti_Sigma0", 1192.55,    0,  0, -1,+1);  /*28: Antisigma0 */
    idToPropL[29]=createParticle(29,"anti_Sigma+", 1197.436,  +1,  0, -1,+1);  /*29: Antisigma+ */
    idToPropL[30]=createParticle(30,"anti_Xi0",    1314.9,     0,  0, -1,+2);  /*30: Antixi0    */
    idToPropL[31]=createParticle(31,"anti_Xi+",    1321.32,    1,  0, -1,+2);  /*31: Antixi+    */
    idToPropL[32]=createParticle(32,"anti_Omega+", 1672.45,    1,  0, -1,+3);  /*32: Antiomega+ */
    idToPropL[33]=createParticle(33,"TC",          0.0,        0,  0,  0, 0);  /*33: TC         */
    idToPropL[34]=createParticle(34,"D0",          1232.0,     0,  0, +1, 0);  /*34: Delta0     */
    idToPropL[35]=createParticle(35,"D++",         1232.0,     2,  0, +1, 0);  /*35: Delta++    */
    idToPropL[36]=createParticle(36,"D+",          1232.0,     1,  0, +1, 0);  /*36: Delta+     */
    idToPropL[37]=createParticle(37,"D-",          1232.0,    -1,  0, +1, 0);  /*37: Delta-     */
    idToPropL[38]=createParticle(38,"NP11+",       1440.0,     1,  0, +1, 0);  /*38: NP11+      */
    idToPropL[39]=createParticle(39,"ND13+",       1520.0,     1,  0, +1, 0);  /*39: ND13+      */
    idToPropL[40]=createParticle(40,"NS11+",       1535.0,     1,  0, +1, 0);  /*40: NS11+      */
    idToPropL[41]=createParticle(41,"rho0",        769.9,      0,  0,  0, 0);  /*41: rho0       */
    idToPropL[42]=createParticle(42,"rho+",        769.9,      1,  0,  0, 0);  /*42: rho+       */
    idToPropL[43]=createParticle(43,"rho-",        769.9,     -1,  0,  0, 0);  /*43: rho-       */
    idToPropL[44]=createParticle(44,"pion",        139.56995,  0,  0,  0, 0);  /*44: PION       */
    idToPropL[45]=createParticle(45,"d",           1875.613,   1,  0, +2, 0);  /*45: Deuteron   */
    idToPropL[46]=createParticle(46,"t",           2809.25,    1,  0, +3, 0);  /*46: Tritium    */
    idToPropL[47]=createParticle(47,"alpha",       3727.417,   2,  0, +4, 0);  /*47: Alpha      */
    idToPropL[48]=createParticle(48,"kaon",        493.677,    0,  0,  0, 0);  /*48: KAON       */
    idToPropL[49]=createParticle(49,"He3",         2809.23,    2,  0, +3, 0);  /*49: He3        */
    idToPropL[50]=createParticle(50,"dimuon",      211.31678,  0,  0,  0, 0);  /*50: dimuon     */
    idToPropL[51]=createParticle(51,"dilepton",     1.022,     0,  0,  0, 0);  /*51: dilepton   */
    idToPropL[52]=createParticle(52,"w",           781.94,     0,  0,  0, 0);  /*52: omega      */
    idToPropL[53]=createParticle(53,"eta'",        957.7,      0,  0,  0, 0);  /*53: eta'       */
    idToPropL[54]=createParticle(54,"sigma",       600.,       0,  0,  0, 0);  /*54: sigma      */
    idToPropL[55]=createParticle(55,"phi",         1019.413,   0,  0,  0, 0);  /*55: phi        */
    idToPropL[56]=createParticle(56,"DP330",       1600.,      0,  0,  1, 0);  /*56: Delta0*P33 */
    idToPropL[57]=createParticle(57,"DP33++",      1600.,      2,  0,  1, 0);  /*57: Delta++ *P33*/
    idToPropL[58]=createParticle(58,"DP33+",       1600.,      1,  0,  1, 0);  /*58: Delta+*P33 */
    idToPropL[59]=createParticle(59,"DP33-",       1600.,     -1,  0,  1, 0);  /*59: Delta- *P33*/
    idToPropL[60]=createParticle(60,"DS310",       1620.,      0,  0,  1, 0);  /*60: Delta0*S31*/
    idToPropL[61]=createParticle(61,"DS31++",      1620.,      2,  0,  1, 0);  /*61: Delta++ *S31*/
    idToPropL[62]=createParticle(62,"DS31+",       1620.,      1,  0,  1, 0);  /*62: Delta+*S31 */
    idToPropL[63]=createParticle(63,"DS31-",       1620.,     -1,  0,  1, 0);  /*63: Delta- *S31 */
    idToPropL[64]=createParticle(64,"NP110",       1440.,      0,  0,  1, 0);  /*64: NP110      */
    idToPropL[65]=createParticle(65,"ND130",       1520.,      0,  0,  1, 0);  /*65: ND130      */
    idToPropL[66]=createParticle(66,"NS110",       1535.,      0,  0,  1, 0);  /*66: NS110      */
    idToPropL[67]=createParticle(67,"J/Psi",       3096.88,    0,  0,  0, 0);  /*67: J/Psi      */
    idToPropL[68]=createParticle(68,"Psi'",        3685.96,    0,  0,  0, 0);  /*68: Psi'       */
    idToPropL[69]=createParticle(69,"pn"  ,        2650.,      1,  0,  2, 0);  /*69: pn         */
    idToPropL[120]=createParticle(120,"Hypertriton",2991.,     1,  0,  3, 0);  /*69: Hypertriton */

    HPhysicsConstants::setDefaultGraphic(kMagenta,1);
    HPhysicsConstants::setGraphic(14,kGray    ,1); // proton
    HPhysicsConstants::setGraphic(13,kGreen-2 ,1); // neutron
    HPhysicsConstants::setGraphic(8 ,kRed-9   ,1); // pi+
    HPhysicsConstants::setGraphic(9 ,kBlue-9  ,1); // pi-
    HPhysicsConstants::setGraphic(7 ,kYellow  ,1); // pi0

    HPhysicsConstants::setGraphic(2 ,kRed     ,1); // e+
    HPhysicsConstants::setGraphic(3 ,kBlue    ,1); // e-
    HPhysicsConstants::setGraphic(1 ,kWhite   ,1); // gamma

    HPhysicsConstants::setGraphic(10,kGreen   ,1); // K0L
    HPhysicsConstants::setGraphic(11,kGreen-7 ,1); // K+
    HPhysicsConstants::setGraphic(12,kGreen-6 ,1); // K-
    HPhysicsConstants::setGraphic(45,kOrange  ,1); // d
    HPhysicsConstants::setGraphic(46,kOrange-3,1); // t



    return idToPropL;
}

// -----------------------------------------------------------------------------

map<TString,particleproperties> HPhysicsConstants::initParticleName()
{
    // to be called only for init of static vectors

    map<TString,particleproperties> nameToPropL;

    map<Int_t,particleproperties>::iterator iter;
    for( iter = idToProp.begin(); iter != idToProp.end(); ++iter ) {
          nameToPropL[iter->second.fName] = iter->second;
    }

    return nameToPropL;
}
// -----------------------------------------------------------------------------


map<Int_t,TString> HPhysicsConstants::initGeantProcess()
{
    // to be called only for init of static vectors

    map <Int_t,TString> mprocess;       // geant process code

    mprocess  [0] ="No Process";
    mprocess  [1] ="NEXT  : particle has reached the boundary of current volume";
    mprocess  [2] ="MULS  : multiple scattering";
    mprocess  [3] ="LOSS  : continuous energy loss";
    mprocess  [4] ="FIEL  : bending in magnetic field";
    mprocess  [5] ="DCAY  : particle decay";
    mprocess  [6] ="PAIR  : photon pair-production or muon direct pair production";
    mprocess  [7] ="COMP  : Compton scattering";
    mprocess  [8] ="PHOT  : photoelectric effect";
    mprocess  [9] ="BREM  : bremsstrahlung";
    mprocess [10] ="DRAY  : delta-ray production";
    mprocess [11] ="ANNI  : positron annihilation";
    mprocess [12] ="HADR  : hadronic interaction";
    mprocess [13] ="ECOH  : hadronic elastic coherent scattering";
    mprocess [14] ="EVAP  : nuclear evaporation";
    mprocess [15] ="FISS  : nuclear fission";
    mprocess [16] ="ABSO  : nuclear absorption";
    mprocess [17] ="ANNH  : anti-proton annihilation";
    mprocess [18] ="CAPT  : neutron capture";
    mprocess [19] ="EINC  : hadronic elastic incoherent scattering";
    mprocess [20] ="INHE  : hadronic inelastic scattering";
    mprocess [21] ="MUNU  : muon-nuclear interaction";
    mprocess [22] ="TOFM  : exceeded time of flight cut";
    mprocess [23] ="PFIS  : nuclear photo-fission";
    mprocess [24] ="SCUT  : the particle due to magnetic field was unexp. crossing vol. boundaries";
    mprocess [25] ="RAYL  : Rayleigh effect";
    mprocess [26] ="PARA  : parametrisation activated";
    mprocess [27] ="PRED  : error matrix computed (GEANE tracking)";
    mprocess [28] ="LOOP  : not used";
    mprocess [29] ="NULL  : no mechanism is active, usually at the entrance of a new volume";
    mprocess [30] ="STOP  : particle has fallen below energy threshold and tracking stops";
    mprocess [31] ="HADES : (pair creat. with immediate annihilation of stopped positron";
    mprocess [32] ="HADES : as 31, but electron also stopped";
    mprocess[101] ="LABS  : Cerenkov photon absorption";
    mprocess[102] ="LREF  : Cerenkov photon reflection/refraction";
    mprocess[103] ="SMAX  : step limited by STEMAX";
    mprocess[104] ="SCOR  : correction against loss of precision in boundary crossing";
    mprocess[105] ="CKOV  : Cerenkov photon generation";
    mprocess[106] ="REFL  : Cerenkov photon reflection";
    mprocess[107] ="REFR  : Cerenkov photon refraction";
    mprocess[108] ="SYNC  : synchrotron radiation generation";
    mprocess[109] ="STRA  : PAI or ASHO model used for energy loss fluctuations.";



    return mprocess;
}

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------

void HPhysicsConstants::loadGeantIons()
{
    // load the table of ions know by Geant3 (id 61-112). The pluto particles
    // with ids 61-69 will be removed.

    ::Info("loadGeantIons()","Adding new Particles(Fragments) - remove Patricle 61-69 -- Re-Intialize HPhysicsConstants with GEANT3 numbering ... ");
    for(UInt_t jj = 61 ; jj < 70; jj ++) HPhysicsConstants::removeParticle(jj);
    
    // Mass from GEANT3 gpions.F
    // and information from shield to evt convert.sh

//                                 id, name,           mass, chrg,leptc, baryc, strange
    HPhysicsConstants::addParticle(61,"Li_3_6",      5603.05,      3,  0,  6, 0);  //
    HPhysicsConstants::addParticle(62,"Li_3_7",      6535.36,      3,  0,  7, 0);  //
    HPhysicsConstants::addParticle(63,"Be_4_7",      6536.22,      4,  0,  7, 0);  //
    HPhysicsConstants::addParticle(64,"Be_4_9",      8394.79,      4,  0,  9, 0);  //
    HPhysicsConstants::addParticle(65,"Be_5_10",     9326.99,      5,  0, 10, 0);  //
    HPhysicsConstants::addParticle(66,"Be_5_11",    10255.10,      5,  0, 11, 0);  //
    HPhysicsConstants::addParticle(67,"C_6_12" ,    11177.93,      6,  0, 12, 0);  //
    HPhysicsConstants::addParticle(68,"N_7_14" ,    13043.78,      7,  0, 14, 0);  //
    HPhysicsConstants::addParticle(69,"O_8_16" ,    14899.17,      8,  0, 16, 0);  //
    HPhysicsConstants::addParticle(70,"F_9_19" ,    17696.90,      9,  0, 19, 0);  //
    HPhysicsConstants::addParticle(71,"Ne_10_20",   18622.84,     10,  0, 20, 0);  //
    HPhysicsConstants::addParticle(72,"Na_11_23",   21414.83,     11,  0, 23, 0);  //
    HPhysicsConstants::addParticle(73,"Mg_12_24",   22341.93,     12,  0, 24, 0);  //
    HPhysicsConstants::addParticle(74,"Al_13_27",   25133.14,     13,  0, 27, 0);  //
    HPhysicsConstants::addParticle(75,"Si_14_28",   26060.34,     14,  0, 28, 0);  //
    HPhysicsConstants::addParticle(76,"P_15_31" ,   28851.88,     15,  0, 31, 0);  //
    HPhysicsConstants::addParticle(77,"S_16_32" ,   29781.80,     16,  0, 32, 0);  //
    HPhysicsConstants::addParticle(78,"Cl_17_35",   32573.28,     17,  0, 35, 0);  //
    HPhysicsConstants::addParticle(79,"Ar_18_36",   33503.56,     18,  0, 36, 0);  //
    HPhysicsConstants::addParticle(80,"K_19_39" ,   36294.47,     19,  0, 39, 0);  //
    HPhysicsConstants::addParticle(81,"Ca_20_40",   37224.92,     20,  0, 40, 0);  //
    HPhysicsConstants::addParticle(82,"Sc_21_45",   41876.17,     21,  0, 45, 0);  //
    HPhysicsConstants::addParticle(83,"Ti_22_48",   44663.24,     22,  0, 48, 0);  //
    HPhysicsConstants::addParticle(84,"V_23_51" ,   47454.01,     23,  0, 51, 0);  //
    HPhysicsConstants::addParticle(85,"Cr_24_52",   48382.28,     24,  0, 52, 0);  //
    HPhysicsConstants::addParticle(86,"Mn_25_55",   51174.47,     25,  0, 55, 0);  //
    HPhysicsConstants::addParticle(87,"Fe_26_56",   52103.07,     26,  0, 56, 0);  //
    HPhysicsConstants::addParticle(88,"Co_27_59",   54895.93,     27,  0, 59, 0);  //
    HPhysicsConstants::addParticle(89,"Ni_28_58",   53966.44,     28,  0, 58, 0);  //
    HPhysicsConstants::addParticle(90,"Cu_29_63",   58618.56,     29,  0, 63, 0);  //
    HPhysicsConstants::addParticle(91,"Zn_30_64",   59549.63,     30,  0, 64, 0);  //
    HPhysicsConstants::addParticle(92,"Ge_32_74 ",  68857.15,     32,  0, 74, 0);  //
    HPhysicsConstants::addParticle(93,"Se_34_80 ",  74441.78,     34,  0, 80, 0);  //
    HPhysicsConstants::addParticle(94,"Kr_36_84 ",  78163.09,     36,  0, 84, 0);  //
    HPhysicsConstants::addParticle(95,"Sr_38_88 ",  81883.58,     38,  0, 88, 0);  //
    HPhysicsConstants::addParticle(96,"Zr_40_90 ",  83745.71,     40,  0, 90, 0);  //
    HPhysicsConstants::addParticle(97,"Mo_42_98 ",  91198.32,     42,  0, 98, 0);  //
    HPhysicsConstants::addParticle(98,"Pd_46_106",  98649.97,     46,  0, 106, 0);  //
    HPhysicsConstants::addParticle(99,"Cd_48_114", 106109.97,     48,  0, 114, 0);  //
    HPhysicsConstants::addParticle(100,"Sn_50_120",111688.21,     50,  0, 120, 0);  //
    HPhysicsConstants::addParticle(101,"Xe_54_132",122867.96,     54,  0, 132, 0);  //
    HPhysicsConstants::addParticle(102,"Ba_56_138",128457.93,     56,  0, 138, 0);  //
    HPhysicsConstants::addParticle(103,"Ce_58_140",130321.11,     58,  0, 140, 0);  //
    HPhysicsConstants::addParticle(104,"Sm_62_152",141512.36,     62,  0, 152, 0);  //
    HPhysicsConstants::addParticle(105,"Dy_66_164",152699.09,     66,  0, 164, 0);  //
    HPhysicsConstants::addParticle(106,"Yb_70_174",162022.45,     70,  0, 174, 0);  //
    HPhysicsConstants::addParticle(107,"W_74_184" ,171349.24,     74,  0, 184, 0);  //
    HPhysicsConstants::addParticle(108,"Pt_78_194",180675.13,     78,  0, 194, 0);  //
    HPhysicsConstants::addParticle(109,"Au_79_197",183473.24,     79,  0, 197, 0);  //
    HPhysicsConstants::addParticle(110,"Hg_80_202",188134.51,     80,  0, 110, 0);  //
    HPhysicsConstants::addParticle(111,"Pb_82_208",193729.07,     82,  0, 111, 0);  //
    HPhysicsConstants::addParticle(112,"U_92_238" ,221742.95,     92,  0, 112, 0);  //

    HPhysicsConstants::print();

    return;
} 


void HPhysicsConstants::print(){

    ::Info("HPhysicsConstants::print()","Printing table of all known particles and processes:");

    map<Int_t,particleproperties>::iterator iter;
    for( iter = idToProp.begin(); iter != idToProp.end(); ++iter ) {
          iter->second.print();
    }
    cout<<endl;
    cout<<"HGEANT processes :"<<endl;

    map<Int_t,TString>::iterator iter2;
    for( iter2 = mGeantProcess.begin(); iter2 != mGeantProcess.end(); ++iter2 ) {
	cout<<setw(3)<<iter2->first<<" "<<iter2->second<<endl;;
    }



}

// -----------------------------------------------------------------------------

Int_t HPhysicsConstants::pid(const Char_t *n)
{
    // Return integer id given a particle's name

    map<TString,particleproperties>::iterator iter = nameToProp.find(TString(n));
    if(iter != nameToProp.end()) return iter->second.fId;
    else {
	::Error("HPhysicsConstants::pid", "No particle with name=%s", n);
    }
    return 0;
}

// -----------------------------------------------------------------------------

const Char_t *HPhysicsConstants::pid(Short_t pid)
{
    // Return name of the particle

    Int_t newPid = pid;
    if(pid >= ARTIFICIAL_OFFSET && pid < ARTIFICIAL_OFFSET + NPART) newPid = pid - ARTIFICIAL_OFFSET;

    map<Int_t,particleproperties>::iterator iter = idToProp.find(newPid);
    if(iter != idToProp.end()) return iter->second.fName.Data();
    else {
	::Error("HPhysicsConstants::pid", "No particle with id=%d", newPid);
    }
    return NULL;
}

// -----------------------------------------------------------------------------

Int_t HPhysicsConstants::charge(const Int_t pid)
{
    // Return charge of the particle
    Int_t newPid = pid;
    if(pid >= ARTIFICIAL_OFFSET && pid < ARTIFICIAL_OFFSET + NPART) newPid = pid - ARTIFICIAL_OFFSET;

    map<Int_t,particleproperties>::iterator iter = idToProp.find(newPid);
    if(iter != idToProp.end()) return iter->second.fCharge;
    else {
	::Error("HPhysicsConstants::charge", "No particle with id=%d", newPid);
    }
    return 0;
}

// -----------------------------------------------------------------------------

Int_t HPhysicsConstants::leptonCharge(const Int_t pid)
{
    // Return leptonCharge of the particle

    Int_t newPid = pid;
    if(pid >= ARTIFICIAL_OFFSET && pid < ARTIFICIAL_OFFSET + NPART) newPid = pid - ARTIFICIAL_OFFSET;

    map<Int_t,particleproperties>::iterator iter = idToProp.find(newPid);
    if(iter != idToProp.end()) return iter->second.fLeptonCharge;
    else {
	::Error("HPhysicsConstants::leptonCharge", "No particle with id=%d", newPid);
    }
    return 0;
}

// -----------------------------------------------------------------------------

Int_t HPhysicsConstants::baryonCharge(const Int_t pid)
{
// Return baryonCharge of the particle

    Int_t newPid = pid;
    if(pid >= ARTIFICIAL_OFFSET && pid < ARTIFICIAL_OFFSET + NPART) newPid = pid - ARTIFICIAL_OFFSET;

    map<Int_t,particleproperties>::iterator iter = idToProp.find(newPid);
    if(iter != idToProp.end()) return iter->second.fBaryonCharge;
    else {
	::Error("HPhysicsConstants::baryonCharge", "No particle with id=%d", newPid);
    }
    return 0;
}

// -----------------------------------------------------------------------------

Int_t HPhysicsConstants::strangeness(const Int_t pid)
{
    // Return strangeness of the particle

    Int_t newPid = pid;
    if(pid >= ARTIFICIAL_OFFSET && pid < ARTIFICIAL_OFFSET + NPART) newPid = pid - ARTIFICIAL_OFFSET;

    map<Int_t,particleproperties>::iterator iter = idToProp.find(newPid);
    if(iter != idToProp.end()) return iter->second.fStrangeness;
    else {
	::Error("HPhysicsConstants::strangeness", "No particle with id=%d", newPid);
    }
    return 0;
}

// -----------------------------------------------------------------------------

Float_t HPhysicsConstants::mass(const Int_t pid)
{
// Return mass of the particle

    Int_t newPid = pid;
    if(pid >= ARTIFICIAL_OFFSET && pid < ARTIFICIAL_OFFSET + NPART) newPid = pid - ARTIFICIAL_OFFSET;

    map<Int_t,particleproperties>::iterator iter = idToProp.find(newPid);
    if(iter != idToProp.end()) return iter->second.fMass;
    else {
	::Error("HPhysicsConstants::mass", "No particle with id=%d", newPid);
    }
    return -1;
}

const Char_t* HPhysicsConstants::geantProcess(Int_t mech)
{
    // Return geant process description (kine mechanism)

    map<Int_t,TString>::iterator iter = mGeantProcess.find(mech);
    if(iter != mGeantProcess.end()) return iter->second.Data();
    else {
	::Error("HPhysicsConstants::geantProcess", "No mechanism with id=%d", mech);
    }
    return NULL;
}

Int_t HPhysicsConstants::lineColor(const Int_t pid)
{
    // Return line color of the particle

    Int_t newPid = pid;
    if(pid >= ARTIFICIAL_OFFSET && pid < ARTIFICIAL_OFFSET + NPART) newPid = pid - ARTIFICIAL_OFFSET;

    map<Int_t,particleproperties>::iterator iter = idToProp.find(newPid);
    if(iter != idToProp.end()) return iter->second.fLineColor;
    else {
	::Error("HPhysicsConstants::lineColor", "No particle with id=%d", newPid);
    }
    return 1;
}

Int_t HPhysicsConstants::lineStyle(const Int_t pid)
{
    // Return line style  of the particle

    Int_t newPid = pid;
    if(pid >= ARTIFICIAL_OFFSET && pid < ARTIFICIAL_OFFSET + NPART) newPid = pid - ARTIFICIAL_OFFSET;

    map<Int_t,particleproperties>::iterator iter = idToProp.find(newPid);
    if(iter != idToProp.end()) return iter->second.fLineStyle;
    else {
	::Error("HPhysicsConstants::lineColor", "No particle with id=%d", newPid);
    }
    return 1;
}


void HPhysicsConstants::setGraphic(Short_t pid, Int_t col, Int_t style)
{
    Int_t newPid = pid;
    if(pid >= ARTIFICIAL_OFFSET && pid < ARTIFICIAL_OFFSET + NPART) newPid = pid - ARTIFICIAL_OFFSET;

    map<Int_t,particleproperties>::iterator iter = idToProp.find(newPid);
    if(iter != idToProp.end()) {
	iter->second.fLineColor=col;
	iter->second.fLineStyle=style;
    }
    else {
	::Error("HPhysicsConstants::setGraphic", "No particle with id=%d", newPid);
    }

}
void HPhysicsConstants::setDefaultGraphic(Int_t col, Int_t style)
{
    map<Int_t,particleproperties>::iterator iter;
    for( iter = idToProp.begin(); iter != idToProp.end(); ++iter ) {
	iter->second.fLineColor=col;
        iter->second.fLineStyle=style;
    }
}



map<Int_t,particleproperties  > HPhysicsConstants::idToProp      = HPhysicsConstants::initParticleID();
map<TString,particleproperties> HPhysicsConstants::nameToProp    = HPhysicsConstants::initParticleName();
map<Int_t,TString>              HPhysicsConstants::mGeantProcess = HPhysicsConstants::initGeantProcess();




